#ifndef __FAST_H__
#define __FAST_H__

#include "effect.h"
#include "colorgen.h"

class fast : public effect {
  public:
  param_t *fs;
  int flowctr;
  int shift;

  fast(uint8_t *r, uint8_t *g, uint8_t *b);
  void tick();
};


#endif
