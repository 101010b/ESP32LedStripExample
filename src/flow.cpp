#include <arduino.h>
#include <stdint.h>

#include "flow.h"

// Is it a linear system (linear LED Chain)
#define IS_LINEAR

#define FLOW_SPIRALDOWN 0
#define FLOW_SPIRALUP 1
#define FLOW_MAX 1

#ifndef IS_LINEAR
// Linear Chain 
const uint16_t spiral[LEDS]={
  0,1,2,3,4,5,6,7,8,9,
  10,11,12};
#endif

// #define LEDW 30

flow::flow(uint8_t *r, uint8_t *g, uint8_t *b) : effect("Flow", r,b,g)
{
  fm = newparam("mode", FLOW_SPIRALDOWN, FLOW_MAX, FLOW_SPIRALDOWN);
  fs = newparam("speed", 0, 50, 15); 
  /*flowtabler = (uint8_t*) malloc(LEDW);
  flowtableg = (uint8_t*) malloc(LEDW);
  flowtableb = (uint8_t*) malloc(LEDW);
  memset(flowtabler,0,LEDW);
  memset(flowtableg,0,LEDW);
  memset(flowtableb,0,LEDW);*/
  flowctr=0;
  globalcol = NULL;
}

void flow::setGlobalCol(colorgen *_col) {
  globalcol = _col;
}

/*void shiftbuffer(uint8_t *buffer, int n, uint8_t fill) {
  if (n <= 0) return;
  if (n >= LEDW) {
    memset(buffer,fill, LEDW);
    return;
  }
  memmove(buffer+n, buffer, LEDW-n);
  memset(buffer,fill,n);
}*/

void flow::tick() {
  parametric::tick();
  // uint8_t r, g, b;
  bool exec=false;
  if (!globalcol) return;
  if (fs->d.i.val.val < 20) {
    // 0..19
    int spd = 42 - fs->d.i.val.val*2; // 2..21
    if (flowctr >= spd) {
      flowctr = 0;
      //shiftbuffer(flowtabler, 1, globalcol->r);
      //shiftbuffer(flowtableg, 1, globalcol->g);
      //shiftbuffer(flowtableb, 1, globalcol->b);
      exec=true;
    }
    flowctr++;
  } else {
    // 20..
    int spd = fs->d.i.val.val - 19; // 1..x
    //shiftbuffer(flowtabler, spd, globalcol->r);
    //shiftbuffer(flowtableg, spd, globalcol->g);
    //shiftbuffer(flowtableb, spd, globalcol->b);
    exec = true;
  }
  if (exec) {
    switch (fm->d.i.val.val)
    {
    case FLOW_SPIRALUP:
#ifdef IS_LINEAR
      memmove(&list.r[1], &list.r[0], LEDS - 1);
      memmove(&list.g[1], &list.g[0], LEDS - 1);
      memmove(&list.b[1], &list.b[0], LEDS - 1);
      list.r[0] = globalcol->r;
      list.g[0] = globalcol->g;
      list.b[0] = globalcol->b;
#else
      for (int i = LEDS - 1; i > 0; i--)
      {
        int a = spiral[i];
        int b = spiral[i - 1];
        list.r[a] = list.r[b];
        list.g[a] = list.g[b];
        list.b[a] = list.b[b];
      }
      list.r[spiral[0]] = globalcol->r;
      list.g[spiral[0]] = globalcol->g;
      list.b[spiral[0]] = globalcol->b;
#endif
      break;
    case FLOW_SPIRALDOWN:
#ifdef IS_LINEAR
      memmove(&list.r[0], &list.r[1], LEDS - 1);
      memmove(&list.g[0], &list.g[1], LEDS - 1);
      memmove(&list.b[0], &list.b[1], LEDS - 1);
      list.r[LEDS - 1] = globalcol->r;
      list.g[LEDS - 1] = globalcol->g;
      list.b[LEDS - 1] = globalcol->b;
#else
      for (int i = 0; i < LEDS - 1; i++)
      {
        int a = spiral[i];
        int b = spiral[i + 1];
        list.r[a] = list.r[b];
        list.g[a] = list.g[b];
        list.b[a] = list.b[b];
      }
      list.r[spiral[LEDS - 1]] = globalcol->r;
      list.g[spiral[LEDS - 1]] = globalcol->g;
      list.b[spiral[LEDS - 1]] = globalcol->b;
#endif
      break;
    }
  }
}
