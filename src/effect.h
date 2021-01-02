#ifndef __EFFECT_H__
#define __EFFECT_H__

#include <arduino.h>
#include <stdint.h>

#include "parametric.h"

#define LEDS 300

typedef struct ledlist_s {
  uint8_t *r;
  uint8_t *g;
  uint8_t *b;
} ledlist_t;

class effect: public parametric {
  public:
  ledlist_t list;
  
  effect(const char *_name, uint8_t *r, uint8_t *g, uint8_t *b);
  
  void listdecay(uint8_t decayfactor);  
  void listdecayr(uint8_t decayfactor);
  void listdecayg(uint8_t decayfactor);
  void listdecayb(uint8_t decayfactor);

  void shiftbufferleft(uint8_t *buffer, uint16_t len, uint16_t n);
  void shiftbufferright(uint8_t *buffer, uint16_t len, uint16_t n);
  void rollbufferleft(uint8_t *buffer, uint16_t len, uint16_t n);
  void rollbufferright(uint8_t *buffer, uint16_t len, uint16_t n);
};


#endif
// EOF
