#ifndef calibmon_h
#define calibmon_h
//////////////////////////////////////////////////////////////////////////
//
// calibmon decl
//
//////////////////////////////////////////////////////////////////////////

#include <TApplication.h>
#include <TKey.h>
#include <TROOT.h>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TTree.h>
#include <TLeaf.h>
#include <TMonitor.h>
#include <TSocket.h>
#include <TServerSocket.h>
#include <TMessage.h>
#include <TObjString.h>
#include <TSystem.h>
#include <TFolder.h>
#include "TDirectory.h"
#include <TRint.h>
#include <TThread.h>
#include <TCutG.h>
#include "TRandom.h"
#include "TProcessID.h"

#include "KeyPressTimer.h"

class calibmon {
public:
  calibmon() {}
  ~calibmon() {}
  static void begin(int port=9090);
  static void run();
  static void end();
  static KeyPressTimer *GetKeyPressTimer();
  
 private:
  static TApplication  *fApp;
  static Bool_t        fAppDelete;
  static TFile         *fFile;  // output file
  static TFolder       *fFolder; // histo folder
  static TObjArray     *fFolderStack;
  static TServerSocket *fServ;      // server socket
  static int           fServPort;
  static TThread       *fServThread;
  static TThread       *fRunThread;
  static TMonitor      *fMon;       // socket monitor (not used)
  static TList         *fSockThreads;   // socket threads (not used yet)
  static KeyPressTimer *fKeyPressTimer;
  
  static TH1F          *fHist1;       // 1-D histogram
  
  static void run_thread(void *arg);
  static void root_server_thread(void *arg);
  static void root_socket_server(void *arg);
  static TFolder *ReadFolderPointer(TSocket* fSocket);
};

#endif
