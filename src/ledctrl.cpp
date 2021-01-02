#include <Arduino.h>
#include <BLEServer.h>
#include <BLEService.h>
#include <BLEUtils.h>

#include <math.h>

#include "ledctrl.h"
#include "sparcle.h"
#include "flow.h"
#include "motion.h"
#include "fast.h"

uint8_t fadeComponent(int a, int ff, int b) {
  int q=(a * ff)/256 + (b * (256-ff))/256;
  if (q < 0) q=0;
  if (q > 255) q=255;
  return (uint8_t)q;
}

uint8_t interpolate(uint8_t a, int ff, uint8_t b) {
  if (ff <= 0) return b;
  if (ff >= 255) return a;
  ff = (ff * (int)a) / 255 + ((255-ff) * (int)b)/255;
  if (ff < 0) return 0;
  if (ff > 255) return 255;
  return ff;
}

int minterpolate(int a, int f, int max,  int b) {
  return (a*(max-f) + b*f)/max;
}

uint8_t fromDouble(double d) {
  int i=floor(d*255.0+0.5);
  if (i < 0) i=0;
  if (i > 255) i=255;
  return (uint8_t) i;
}

double sqr(double a) { return a*a; }

double sin2(double a) { return sqr(sin(a)); }

void randcol(uint8_t *r, uint8_t *g, uint8_t *b) {
  double col=(double)rand()/(double)RAND_MAX; // 0..1
  double colr=pow(sin2( col     *M_PI) ,EXPON);
  double colg=pow(sin2((col+OSG)*M_PI) ,EXPON);
  double colb=pow(sin2((col+OSB)*M_PI) ,EXPON);
  *r = fromDouble(colr);
  *g = fromDouble(colg);
  *b = fromDouble(colb);
}

// #define NOOP for (int i=0;i<100;i++)
// #define NOOP delayMicroseconds(1)
#define NOOP

#define CLOCKBIT(bit)           \
  digitalWrite(PINDATA, (bit)); \
  NOOP;                         \
  digitalWrite(PINCLK, HIGH);   \
  NOOP;                      \
  digitalWrite(PINCLK, LOW);

void  ledctrl::CLOCKBYTE(uint8_t b) {
  uint8_t i=0x80;
  while (i) {
    CLOCKBIT((b&i)!=0);
    i>>=1;
  }
}

int ledctrl::length() {
	return LEDS;
}
  
void ledctrl::progLeds() {
  short leds;
  uint8_t i;
  short ff;

  // Header --> 32 x LOW
  for (i=0;i<4;i++)
    CLOCKBYTE(0x00);

  // LED Data
  for (leds = 0;leds < LEDS;leds++) {
	  CLOCKBYTE(GLOBAL | 0xE0);
    CLOCKBYTE(LEDBLUE[leds]);
    CLOCKBYTE(LEDGREEN[leds]);
    CLOCKBYTE(LEDRED[leds]);
  }

  // Footer
  ff=LEDS/(2*8);
  if (ff*(2*8) < LEDS) ff++;
  if (ff < 4) ff=4;
  for (i=0;i<ff;i++) 
    CLOCKBYTE(0xFF);

  // digitalWrite(DATA, LOW);
  
}


void ledctrl::allLeds(uint8_t red, uint8_t green, uint8_t blue) {
  memset(LEDRED, red, LEDS);
  memset(LEDGREEN, green, LEDS);
  memset(LEDBLUE, blue, LEDS);
}

void ledctrl::allOff() {
  allLeds(0,0,0);
}

void ledctrl::startUpSequence() {
  // printf("All LEDS red...green...blue...off\n");
  GLOBAL=0x01;
  allLeds(0x30,0x00,0x00);
  progLeds();
  delay(100);
  allLeds(0x00,0x30,0x00);
  progLeds();
  delay(100);
  allLeds(0x00,0x00,0x30);
  progLeds();
  delay(100);
  allLeds(0x00,0x00,0x00);
  progLeds();
}

void ledctrl::setGlobal(uint16_t c) {
  if (c > 0x1F) c=0x1F;
  GLOBAL=c;
}

void ledctrl::setRange(uint16_t first, uint16_t last, uint16_t red, uint16_t green, uint16_t blue) {
 for (int i=first;i<=last;i++) {
  if ((i >= 0) && (i < LEDS)) {
    LEDRED[i]=red;
    LEDGREEN[i]=green;
    LEDBLUE[i]=blue;
  }
 }
}

void ledctrl::shiftEnds(char fromLeft, uint16_t red, uint16_t green, uint16_t blue) {
  if (fromLeft) {
    for (int i=LEDS-1;i>0;i--) {
      LEDRED[i]=LEDRED[i-1];
      LEDGREEN[i]=LEDGREEN[i-1];
      LEDBLUE[i]=LEDBLUE[i-1];
    }
    LEDRED[0]=red;
    LEDGREEN[0]=green;
    LEDBLUE[0]=blue;
  } else {
    for (int i=0;i<LEDS-1;i++) {
      LEDRED[i]=LEDRED[i+1];
      LEDGREEN[i]=LEDGREEN[i+1];
      LEDBLUE[i]=LEDBLUE[i+1];
    }
    LEDRED[LEDS-1]=red;
    LEDGREEN[LEDS-1]=green;
    LEDBLUE[LEDS-1]=blue;
  }
}

void ledctrl::shiftCenter(char FlowOut, uint16_t red, uint16_t green, uint16_t blue) {
	int center = LEDS/2;
	if (FlowOut) {
		for (int i=LEDS-1;i>center;i--) {
			LEDRED[i]=LEDRED[i-1];
			LEDGREEN[i]=LEDGREEN[i-1];
			LEDBLUE[i]=LEDBLUE[i-1];
		}
		for (int i=0;i<center;i++) {
			LEDRED[i]=LEDRED[i+1];
			LEDGREEN[i]=LEDGREEN[i+1];
			LEDBLUE[i]=LEDBLUE[i+1];
		}    
		LEDRED[center]=red;
		LEDGREEN[center]=green;
		LEDBLUE[center]=blue;
	} else {
		for (int i=center;i<LEDS-1;i++) {
			LEDRED[i]=LEDRED[i+1];
			LEDGREEN[i]=LEDGREEN[i+1];
			LEDBLUE[i]=LEDBLUE[i+1];
		} 
		for (int i=center;i>0;i--) {
			LEDRED[i]=LEDRED[i-1];
			LEDGREEN[i]=LEDGREEN[i-1];
			LEDBLUE[i]=LEDBLUE[i-1];
		}
		LEDRED[0]=red;
		LEDGREEN[0]=green;
		LEDBLUE[0]=blue;
		LEDRED[LEDS-1]=red;
		LEDGREEN[LEDS-1]=green;
		LEDBLUE[LEDS-1]=blue;
	}
}

void ledctrl::fadeTo(uint16_t ff, uint16_t red, uint16_t green, uint16_t blue) {
  for (int i=0;i<LEDS;i++) {
    LEDRED[i]=fadeComponent(LEDRED[i],ff,red);
    LEDGREEN[i]=fadeComponent(LEDGREEN[i],ff,green);
    LEDBLUE[i]=fadeComponent(LEDBLUE[i],ff,blue);
  }
}

void ledctrl::cloudIn(char sym, int pos, int width, uint16_t red, uint16_t green, uint16_t blue) {
  for (int i=pos- 2*width;i<=pos + 2*width;i++) {
    int q=i-pos;
    int val;
    val=(width*width*255)/(q*q*5 + width*width);
    if ((i >= 0) && (i < LEDS)) {
      LEDRED[i]=interpolate(red,val,LEDRED[i]);
      LEDGREEN[i]=interpolate(green,val,LEDGREEN[i]);
      LEDBLUE[i]=interpolate(blue,val,LEDBLUE[i]);
    }
  }
}

void ledctrl::errorCode(uint8_t x) {
  GLOBAL=1;
  allLeds(0x00,0x00,0x00);
  for (uint8_t i=0x80, j=0;i;i>>=1,j++)
    LEDRED[j]=(x & i)?0x30:0x00;
  progLeds();  
}
/*
void ledctrl::updatefromeffect(effect *e) {
  memcpy(LEDRED, e->list->r, LEDS);
  memcpy(LEDGREEN, e->list->g, LEDS);
  memcpy(LEDBLUE, e->list->b, LEDS);
}
*/
effect *ledctrl::getselectedeffect() {
  if ((selectedEffect < 0) || (list.size() < 1)) return NULL;
  if (selectedEffect >= list.size()) 
    selectedEffect = list.size()-1;
  std::list<effect *>::iterator it = list.begin();
  std::advance(it, selectedEffect);
  return (*it);
}

void ledctrl::tick() {
  // Update Globals first
  for (std::list<parametric*>::iterator it = globals.begin(); it != globals.end(); ++it) 
    (*it)->tick();
    
  // Then run current effect
  effect *e = getselectedeffect();
  if (e) {
    e->tick();
    // updatefromeffect(e);
  } else 
    allLeds(0,0,0);
}

void ledctrl::fastdata(uint8_t *buffer, uint8_t len) {
  /*for (std::list<parametric*>::iterator it = globals.begin(); it != globals.end(); ++it) 
    (*it)->fastdata(buffer, len);*/
  for (std::list<effect*>::iterator it = list.begin(); it != list.end(); ++it) 
    (*it)->fastdata(buffer, len);
}

effect *ledctrl::findeffect(char *e) {
  for (std::list<effect*>::iterator it = list.begin(); it != list.end(); ++it) {
    if (strcasecmp((*it)->name, e)==0) 
      return (*it);
  }
  return NULL;  
}

parametric *ledctrl::findglobal(char *e) {
  for (std::list<parametric *>::iterator it = globals.begin(); it != globals.end(); ++it) {
    if (strcasecmp((*it)->name, e)==0) 
      return (*it);
  }
  return NULL;  
}

parametric *ledctrl::findparametric(char *s) {
  parametric *p = findglobal(s);
  if (p) 
    return p;  
  effect *e = findeffect(s); 
  if (e) 
    return e;
  return NULL;
}

int ledctrl::findeffectindex(effect *e) {
  int i=0;
  for (std::list<effect*>::iterator it = list.begin(); it != list.end(); ++it) {
    if ((*it) == e) return i;
    i++;
  }
  return -1;  
}

void ledctrl::selecteffect(char *e) {
  effect *eff = findeffect(e);
  if (eff) {
    int index = findeffectindex(eff);
    if (index >= 0) 
      selectedEffect = index;
  }
}

ledctrl::ledctrl() {
	pinMode(PINCLK, OUTPUT);
	pinMode(PINDATA, OUTPUT);

  selectedEffect = -1;
  
  colorgenA = new colorgen("ColA");
  globals.push_back(colorgenA);
  // colorgenB = new colorgen("ColB");globals.push_back(colorgenB);

  sparcle *s = new sparcle(&LEDRED[0], &LEDGREEN[0], &LEDBLUE[0]);
  s->setGlobalCol(colorgenA);
  list.push_back(s);
  flow *f = new flow(&LEDRED[0], &LEDGREEN[0], &LEDBLUE[0]);
  f->setGlobalCol(colorgenA);
  list.push_back(f);
  motion *m = new motion(&LEDRED[0], &LEDGREEN[0], &LEDBLUE[0]);
  m->setGlobalCol(colorgenA);
  list.push_back(m);
  fast *a = new fast(&LEDRED[0], &LEDGREEN[0], &LEDBLUE[0]);
  list.push_back(a);

  startUpSequence();

  selectedEffect = 0;
}

// EOF
