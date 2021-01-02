#include <arduino.h>
#include <stdint.h>

#include "sparcle.h"

sparcle::sparcle(uint8_t *r, uint8_t *g, uint8_t *b) : effect("Sparcle", r, b, g)
{
  sparcN = newparam("N", 1, 100, 10);
  col = newparam("Col", (uint8_t)0, (uint8_t)0, (uint8_t)255);
  useglobalcol = newparam("global", true);
  decayr = newparam("decayr", 0, 4, 2);
  decayg = newparam("decayg", 0, 4, 2);
  decayb = newparam("decayb", 0, 4, 2);
  globalcol = NULL;
  col2 = newparam("Col2", (uint8_t)255, (uint8_t)255, (uint8_t)255);
  usecol2 = newparam("usecol2", false);
  wt = 0;
}

void sparcle::setGlobalCol(colorgen *_col) {
  globalcol = _col;
}

void sparcle::tick() {
  parametric::tick();
  uint8_t r, g, b;
  if (wt == 0) {
    listdecayr(decayr->d.i.val.val);
    listdecayg(decayg->d.i.val.val);
    listdecayb(decayb->d.i.val.val);
    wt=3;
  }
  wt--;
  if (useglobalcol->d.b.val.val && globalcol) {
    r = globalcol->r;
    g = globalcol->g;
    b = globalcol->b;
  } else {
    r = R(col->d.c.val.val);
    g = G(col->d.c.val.val);
    b = B(col->d.c.val.val);
  }
  if (!usecol2->d.b.val.val) {
    for (int i=0;i<sparcN->d.i.val.val;i++) {
      uint32_t idx = rand() % (LEDS*30);
      if (idx < LEDS) {
        list.r[idx]=r;
        list.g[idx]=g;
        list.b[idx]=b;
      }
    }
  } else {
    int i=0;
    for (;i<sparcN->d.i.val.val/2;i++) {
      uint32_t idx = rand() % (LEDS*30);
      if (idx < LEDS) {
        list.r[idx]=r;
        list.g[idx]=g;
        list.b[idx]=b;
      }
    }
    for (;i<sparcN->d.i.val.val;i++) {
      uint32_t idx = rand() % (LEDS*30);
      if (idx < LEDS) {
        list.r[idx]=R(col2->d.c.val.val);
        list.g[idx]=G(col2->d.c.val.val);
        list.b[idx]=B(col2->d.c.val.val);
      }
    }
  }
}
