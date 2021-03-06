#include <Arduino.h>
#include <math.h>

#include "effect.h"

const uint8_t decaytable1[256]={0,0,1,2,3,4,5,6,7,8,9,9,10,11,12,13,14,15,16,17,18,18,19,20,21,22,23,24,25,26,27,27,
  28,29,30,31,32,33,34,35,36,36,37,38,39,40,41,42,43,44,45,45,46,47,48,49,50,51,52,53,54,54,55,56,
  57,58,59,60,61,62,63,63,64,65,66,67,68,69,70,71,72,72,73,74,75,76,77,78,79,80,81,81,82,83,84,85,
  86,87,88,89,90,90,91,92,93,94,95,96,97,98,99,99,100,101,102,103,104,105,106,107,108,108,109,110,111,112,113,114,
  115,116,117,117,118,119,120,121,122,123,124,125,126,126,127,128,129,130,131,132,133,134,135,135,136,137,138,139,140,141,142,143,
  144,144,145,146,147,148,149,150,151,152,153,153,154,155,156,157,158,159,160,161,162,162,163,164,165,166,167,168,169,170,171,171,
  172,173,174,175,176,177,178,179,180,180,181,182,183,184,185,186,187,188,189,189,190,191,192,193,194,195,196,197,198,198,199,200,
  201,202,203,204,205,206,207,207,208,209,210,211,212,213,214,215,216,216,217,218,219,220,221,222,223,224,225,225,226,227,228,229};
  
const uint8_t decaytable2[256]={0,0,1,2,2,3,4,4,5,6,7,7,8,9,9,10,11,11,12,13,14,14,15,16,16,17,18,18,19,20,21,21,
  22,23,23,24,25,25,26,27,28,28,29,30,30,31,32,32,33,34,35,35,36,37,37,38,39,39,40,41,42,42,43,44,
  44,45,46,46,47,48,49,49,50,51,51,52,53,53,54,55,56,56,57,58,58,59,60,60,61,62,63,63,64,65,65,66,
  67,67,68,69,70,70,71,72,72,73,74,74,75,76,77,77,78,79,79,80,81,81,82,83,84,84,85,86,86,87,88,88,
  89,90,91,91,92,93,93,94,95,95,96,97,98,98,99,100,100,101,102,102,103,104,105,105,106,107,107,108,109,109,110,111,
  112,112,113,114,114,115,116,116,117,118,119,119,120,121,121,122,123,123,124,125,126,126,127,128,128,129,130,130,131,132,133,133,
  134,135,135,136,137,137,138,139,140,140,141,142,142,143,144,144,145,146,147,147,148,149,149,150,151,151,152,153,154,154,155,156,
  156,157,158,158,159,160,161,161,162,163,163,164,165,165,166,167,168,168,169,170,170,171,172,172,173,174,175,175,176,177,177,178};
  
const uint8_t decaytable3[256]={0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13,14,14,15,15,
  16,16,17,17,18,18,19,19,20,20,21,21,22,22,23,23,24,24,25,25,26,26,27,27,28,28,29,29,30,30,31,31,
  32,32,33,33,34,34,35,35,36,36,37,37,38,38,39,39,40,40,41,41,42,42,43,43,44,44,45,45,46,46,47,47,
  48,48,49,49,50,50,51,51,52,52,53,53,54,54,55,55,56,56,57,57,58,58,59,59,60,60,61,61,62,62,63,63,
  64,64,65,65,66,66,67,67,68,68,69,69,70,70,71,71,72,72,73,73,74,74,75,75,76,76,77,77,78,78,79,79,
  80,80,81,81,82,82,83,83,84,84,85,85,86,86,87,87,88,88,89,89,90,90,91,91,92,92,93,93,94,94,95,95,
  96,96,97,97,98,98,99,99,100,100,101,101,102,102,103,103,104,104,105,105,106,106,107,107,108,108,109,109,110,110,111,111,
  112,112,113,113,114,114,115,115,116,116,117,117,118,118,119,119,120,120,121,121,122,122,123,123,124,124,125,125,126,126,127,127};

const uint8_t decaytable4[256]={0,0,0,0,1,1,1,2,2,2,3,3,3,3,4,4,4,5,5,5,6,6,6,6,7,7,7,8,8,8,9,9,
  9,9,10,10,10,11,11,11,12,12,12,12,13,13,13,14,14,14,15,15,15,15,16,16,16,17,17,17,18,18,18,18,
  19,19,19,20,20,20,21,21,21,21,22,22,22,23,23,23,24,24,24,24,25,25,25,26,26,26,27,27,27,27,28,28,
  28,29,29,29,30,30,30,30,31,31,31,32,32,32,33,33,33,33,34,34,34,35,35,35,36,36,36,36,37,37,37,38,
  38,38,39,39,39,39,40,40,40,41,41,41,42,42,42,42,43,43,43,44,44,44,45,45,45,45,46,46,46,47,47,47,
  48,48,48,48,49,49,49,50,50,50,51,51,51,51,52,52,52,53,53,53,54,54,54,54,55,55,55,56,56,56,57,57,
  57,57,58,58,58,59,59,59,60,60,60,60,61,61,61,62,62,62,63,63,63,63,64,64,64,65,65,65,66,66,66,66,
  67,67,67,68,68,68,69,69,69,69,70,70,70,71,71,71,72,72,72,72,73,73,73,74,74,74,75,75,75,75,76,76};
  
const uint8_t decaytable5[256]={0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,3,3,
  3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,6,6,6,6,
  6,6,6,6,6,6,7,7,7,7,7,7,7,7,7,7,8,8,8,8,8,8,8,8,8,8,9,9,9,9,9,9,
  9,9,9,9,10,10,10,10,10,10,10,10,10,10,11,11,11,11,11,11,11,11,11,11,12,12,12,12,12,12,12,12,
  12,12,13,13,13,13,13,13,13,13,13,13,14,14,14,14,14,14,14,14,14,14,15,15,15,15,15,15,15,15,15,15,
  16,16,16,16,16,16,16,16,16,16,17,17,17,17,17,17,17,17,17,17,18,18,18,18,18,18,18,18,18,18,19,19,
  19,19,19,19,19,19,19,19,20,20,20,20,20,20,20,20,20,20,21,21,21,21,21,21,21,21,21,21,22,22,22,22,
  22,22,22,22,22,22,23,23,23,23,23,23,23,23,23,23,24,24,24,24,24,24,24,24,24,24,25,25,25,25,25,25};

effect::effect(const char *_name, uint8_t *r, uint8_t *g, uint8_t *b) : parametric(_name)
{
  list.r = r;
  list.g = g;
  list.b = b;
}

void effect::listdecay(uint8_t decayfactor) {
  switch (decayfactor) {
  case 0:  
    for (int i=0;i<LEDS;i++) {
      list.r[i] = decaytable1[list.r[i]];
      list.g[i] = decaytable1[list.g[i]];
      list.b[i] = decaytable1[list.b[i]];
    }
    break;
  case 1:  
    for (int i=0;i<LEDS;i++) {
      list.r[i] = decaytable2[list.r[i]];
      list.g[i] = decaytable2[list.g[i]];
      list.b[i] = decaytable2[list.b[i]];
    }
    break;
  case 2:  
    for (int i=0;i<LEDS;i++) {
      list.r[i] = decaytable3[list.r[i]];
      list.g[i] = decaytable3[list.g[i]];
      list.b[i] = decaytable3[list.b[i]];
    }
    break;
  case 3:  
    for (int i=0;i<LEDS;i++) {
      list.r[i] = decaytable4[list.r[i]];
      list.g[i] = decaytable4[list.g[i]];
      list.b[i] = decaytable4[list.b[i]];
    }
    break;
  default:
    for (int i=0;i<LEDS;i++) {
      list.r[i] = decaytable5[list.r[i]];
      list.g[i] = decaytable5[list.g[i]];
      list.b[i] = decaytable5[list.b[i]];
    }
    break;
  }
}

const uint8_t *getdecaytable(uint8_t decayfactor) {
  switch (decayfactor) {
    case 0: return decaytable1;
    case 1: return decaytable2;
    case 2: return decaytable3;
    case 3: return decaytable4;
    default: return decaytable5;
  }
}

void listdecaybuffer(uint8_t *buffer, uint8_t decayfactor) {
  const uint8_t *decaytable = getdecaytable(decayfactor);
  for (int i=0;i<LEDS;i++)
    buffer[i] = decaytable[buffer[i]];
}

void effect::listdecayr(uint8_t decayfactor) {
  listdecaybuffer(list.r, decayfactor);  
}

void effect::listdecayg(uint8_t decayfactor) {
  listdecaybuffer(list.g, decayfactor);  
}

void effect::listdecayb(uint8_t decayfactor) {
  listdecaybuffer(list.b, decayfactor);  
}

void effect::shiftbufferleft(uint8_t *buffer, uint16_t len, uint16_t n) {
  if (n == 0) return;
  if (n >= len) {
    memset(buffer,0,len);
    return;
  }
  memmove(buffer, buffer+n,len-n);
  memset(buffer+len-n, 0, n);
}

void effect::shiftbufferright(uint8_t *buffer, uint16_t len, uint16_t n) {
  if (n == 0) return;
  if (n >= len) {
    memset(buffer,0,len);
    return;
  }
  memmove(buffer+n, buffer,len-n);
  memset(buffer, 0, n);
}

uint8_t *roll_temp = NULL;
void effect::rollbufferleft(uint8_t *buffer, uint16_t len, uint16_t n) {
  n = n % len;
  if (n == 0) return;
  if (!roll_temp) roll_temp = (uint8_t*)malloc(LEDS);
  if (!roll_temp) return;
  memcpy(roll_temp,buffer,n);
  memmove(buffer, buffer+n,len-n);
  memcpy(buffer+len-n,roll_temp,n);
}

void effect::rollbufferright(uint8_t *buffer, uint16_t len, uint16_t n) {
  n = n % len;
  if (n == 0) return;
  if (!roll_temp) roll_temp = (uint8_t*)malloc(LEDS);
  if (!roll_temp) return;
  memcpy(buffer+n,roll_temp,n);
  memmove(buffer+n, buffer,len-n);
  memcpy(buffer,roll_temp,n);
}
