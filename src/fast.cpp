#include <stdio.h>
#include <stdlib.h>

#include "fast.h"

fast::fast(uint8_t *r, uint8_t *g, uint8_t *b) : effect("Fast",r,g,b)
{
  fs = newparam("speed", 0, 50, 15); 
  flowctr=0;
  shift=0;
}

void fast::tick() {
  if (fs->d.i.val.val < 20) {
    // 0..19
    int spd = 21 - fs->d.i.val.val; // 2..21
    if (flowctr >= spd) {
      flowctr = 0;
      shift++;
      if (shift >= 60) shift=0;
    }
    flowctr++;
  } else {
    // 20..
    int spd = fs->d.i.val.val - 19; // 1..x
    shift=(shift + spd)%60;
  }
}
