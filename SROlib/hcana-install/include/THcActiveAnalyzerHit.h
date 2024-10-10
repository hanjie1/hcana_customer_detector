#ifndef ROOT_THcActiveAnalyzerHit
#define ROOT_THcActiveAnalyzerHit

#include "THcRawHodoHit.h"

class THcActiveAnalyzerHit : public THcRawHodoHit {

 public:
  friend class THcCherenkov;

 protected:

 private:

  ClassDef(THcActiveAnalyzerHit,0);	// Raw Cherenkov hit
};

#endif

