#ifndef KeyPress_h
#define KeyPress_h
////////////////////////////////////////////////////////////////////////
//
//	calibmonrun
//

class KeyPress {
public:
  static int Get(bool reset=false);
  static bool Hit();

  struct Char
  {
    enum Type {
      BS=   8,
      TAB=  9,
      CR=   13,
      EXT=  0x100,
      HOME= (EXT+0),
      INSERT= (EXT+1),
      DELETE= (EXT+2),
      END=    (EXT+3),
      PUP=    (EXT+4),
      PDOWN=  (EXT+5),
      UP=     (EXT+6),
      DOWN=   (EXT+7),
      RIGHT=  (EXT+8),
      LEFT=   (EXT+9),
    };
    Type t_;
    Char(Type t) : t_(t) {}
    operator Type () const {return t_;}

  private:
    //prevent automatic conversion for any other built-in types such as bool, int, etc
    template<typename T>
    operator T () const;
  };
};
#endif
