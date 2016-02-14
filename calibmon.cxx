////////////////////////////////////////////////////////////////////////
//
// calibmon defs
//
////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <iostream>

#include "calibmon.h"

#ifndef __CINT__
#include "TError.h"
#endif

#include "KeyPressTimer.h"
#include "calibmon.h"

TApplication  *calibmon::fApp;
Bool_t         calibmon::fAppDelete;
TFile         *calibmon::fFile;
TFolder       *calibmon::fFolder;
TObjArray     *calibmon::fFolderStack;
TServerSocket *calibmon::fServ;
int            calibmon::fServPort;
TThread       *calibmon::fServThread;
TThread       *calibmon::fRunThread;
TMonitor      *calibmon::fMon;
TList         *calibmon::fSockThreads;
KeyPressTimer *calibmon::fKeyPressTimer;
TH1F          *calibmon::fHist1;

KeyPressTimer *calibmon::GetKeyPressTimer()
{
  return fKeyPressTimer;
}

TFolder *calibmon::ReadFolderPointer(TSocket* fSocket)
{
  // read pointer to current folder
  TMessage* m = 0;
  fSocket->Recv(m);
  size_t p;
  *m >> p;
  return (TFolder*) p;
}

void calibmon::root_server_thread(void *arg)
{
  char request[256];
  
  TSocket *sock = (TSocket *) arg;
  
  do {
    
    /* close connection if client has disconnected */
    if (sock->Recv(request, sizeof(request)) <= 0) {
      // printf("Closed connection to %s\n", sock->GetInetAddress().GetHostName());
      sock->Close();
      delete sock;
      return;
      
    } else {
      
      TMessage *message = new TMessage(kMESS_OBJECT);
      
      if (strcmp(request, "GetListOfFolders") == 0) {
	
	TFolder *folder = ReadFolderPointer(sock);
	if (folder == NULL) {
	  message->Reset(kMESS_OBJECT);
	  message->WriteObject(NULL);
	  sock->Send(*message);
	  delete message;
	  continue;
	}
	//get folder names
	TObject *obj;
	TObjArray *names = new TObjArray(100);
	
	TCollection *folders = folder->GetListOfFolders();
	TIterator *iterFolders = folders->MakeIterator();
	while ((obj = iterFolders->Next()) != NULL)
	  names->Add(new TObjString(obj->GetName()));
	
	//write folder names
	message->Reset(kMESS_OBJECT);
	message->WriteObject(names);
	sock->Send(*message);
	
	for (int i = 0; i < names->GetLast() + 1; i++)
	  delete(TObjString *) names->At(i);
	
	delete names;
	
	delete message;
	
      } else if (strncmp(request, "FindObject", 10) == 0) {
	
	TFolder *folder = ReadFolderPointer(sock);
	
	//get object
	TObject *obj;
	if (strncmp(request + 10, "Any", 3) == 0)
	  obj = folder->FindObjectAny(request + 14);
	else
	  obj = folder->FindObject(request + 11);
	
	//write object
	if (!obj)
	  sock->Send("Error");
	else {
	  message->Reset(kMESS_OBJECT);
	  message->WriteObject(obj);
	  sock->Send(*message);
	}
	delete message;
	
      } else if (strncmp(request, "FindFullPathName", 16) == 0) {
	
	TFolder *folder = ReadFolderPointer(sock);
	
	//find path
	const char *path = folder->FindFullPathName(request + 17);
	
	//write path
	if (!path) {
	  sock->Send("Error");
	} else {
	  TObjString *obj = new TObjString(path);
	  message->Reset(kMESS_OBJECT);
	  message->WriteObject(obj);
	  sock->Send(*message);
	  delete obj;
	}
	delete message;
	
      } else if (strncmp(request, "Occurence", 9) == 0) {
	
	TFolder *folder = ReadFolderPointer(sock);
	
	//read object
	TMessage *m = 0;
	sock->Recv(m);
	TObject *obj = ((TObject *) m->ReadObject(m->GetClass()));
	
	//get occurence
	Int_t retValue = folder->Occurence(obj);
	
	//write occurence
	message->Reset(kMESS_OBJECT);
	*message << retValue;
	sock->Send(*message);
	
	delete message;
	
      } else if (strncmp(request, "GetPointer", 10) == 0) {
	
	//find object
	TObject *obj = gROOT->FindObjectAny(request + 11);
	
	//write pointer
	message->Reset(kMESS_ANY);
	size_t p = (size_t) obj;
	*message << p;
	sock->Send(*message);
	
	delete message;
	
      } else if (strncmp(request, "Command", 7) == 0) {
	char objName[100], method[100];
	sock->Recv(objName, sizeof(objName));
	sock->Recv(method, sizeof(method));
	TObject *object = gROOT->FindObjectAny(objName);
	if (object && object->InheritsFrom(TH1::Class())
	    && strcmp(method, "Reset") == 0)
	  static_cast < TH1 * >(object)->Reset();
	
      } else if (strncmp(request, "SetCut", 6) == 0) {
	
	//read new settings for a cut
	char name[256];
	sock->Recv(name, sizeof(name));
	TCutG *cut = (TCutG *) calibmon::fFolder->FindObjectAny(name);
	
	TMessage *m = 0;
	sock->Recv(m);
	TCutG *newc = ((TCutG *) m->ReadObject(m->GetClass()));
	
	if (cut) {
	  printf("root server thread, changing cut %s\n", newc->GetName());
	  newc->TAttMarker::Copy(*cut);
	  newc->TAttFill::Copy(*cut);
	  newc->TAttLine::Copy(*cut);
	  newc->TNamed::Copy(*cut);
	  cut->Set(newc->GetN());
	  for (int i = 0; i < cut->GetN(); ++i) {
	    cut->SetPoint(i, newc->GetX()[i], newc->GetY()[i]);
	  }
	} else {
	  printf("root server thread, ignoring receipt of unknown cut %s\n", newc->GetName());
	}
	delete newc;
	
      } else
	printf("SocketServer: Received unknown command \"%s\"\n", request);
    }
  } while (1);
  
  return;
}

void calibmon::root_socket_server(void *arg)
{
  // Server loop listening for incoming network connections on specified port.
  // Starts a searver_thread for each connection.
  
  // printf("Root server listening on port %d...\n", port);
  TServerSocket *lsock = new TServerSocket(fServPort, kTRUE);

  do {
    TSocket *sock = lsock->Accept();
    
    // printf("Established connection to %s\n", sock->GetInetAddress().GetHostName());
    
    TThread *thread = new TThread("Server", root_server_thread, sock);
    thread->Run();
  } while (true);
  
  return;
}

void calibmon::begin(int pServPort)
{
  fServPort = pServPort;
  if (gApplication) {
    fApp = gApplication;
    fAppDelete = kFALSE;
  } else {
    fApp = new TApplication("calibmon", 0, 0);
    fAppDelete = kTRUE;
  }

  fFolder = gROOT->GetRootFolder()->AddFolder("histos", "Calibmon Histograms");
  fFolderStack = new TObjArray();
  gROOT->GetListOfBrowsables()->Add(calibmon::fFolder, "histos");

  fHist1 = new TH1F("hist1", "histogram #1", 100, -4, 4);
  fFolder->Add(fHist1);

  printf("Root server listening on port %d\n", fServPort);
  fServThread = new TThread("Calibmon_Histo_Server", root_socket_server, &fServPort);
  fServThread->Run();
}

void calibmon::run()
{
  gRandom->SetSeed();
  
  KeyPressTimer kpt(100, '!', 0);
  printf("Press '!' to quit\n", fServPort);
  while(kpt.KeepGoing()) {
    Float_t val;
    val = gRandom->Gaus();
    fHist1->Fill(val);
    
    gSystem->ProcessEvents();
    gSystem->Sleep(1);
  }
}

void calibmon::end()
{
  delete fServThread;
  delete fHist1;
  delete fFolderStack;
  delete fKeyPressTimer;
  if (fAppDelete) delete fApp;
}
