#include <arduino.h>
#include <stdint.h>

#include "parametric.h"

#define LOCAL(ba) ((ba)[0])
#define REMOTE(ba) ((ba)[1])

parametric::parametric(const char *_name) {
  strcpy(name, _name);
  params = NULL;
  fastcol0=fastcol1=fastcol2=fastcol3=fastcol4=0;
}

param_t *parametric::addparam(param_t *p) {
  if (!p) return NULL;
  param_t *q = params;
  p->next = NULL;
  if (q) {
    while (q->next) q=q->next;
    q->next = p;
  } else {
    params = p;
  }  
  return p;
}

param_t *parametric::newparam(const char *name) 
{
  if (strlen(name) > 31) return NULL;
  param_t *p = (param_t *)malloc(sizeof(param_t));
  if (!p) return NULL;
  strcpy(p->name, name);
  p->type = PARAMTYPE::INT;
  return p;
}

param_t *parametric::newparam(const char *name, int32_t minval, int32_t maxval, int32_t defaultval)
{
  param_t *p = newparam(name);
  if (!p) return NULL;
  p->type = PARAMTYPE::INT;
  p->d.i.val.min = minval;
  p->d.i.val.max = maxval;
  p->d.i.defaultval = defaultval;
  p->d.i.val.val = defaultval;
  p->d.i.val.changeflag = 0;
  return addparam(p);
}

param_t *parametric::newparam(const char *name, uint8_t rdefault, uint8_t gdefault, uint8_t bdefault)
{
  param_t *p = newparam(name);
  if (!p) return NULL;
  p->type = PARAMTYPE::COL;
  p->d.c.defaultval = RGB(rdefault, gdefault, bdefault);
  p->d.c.val.val = RGB(rdefault, gdefault, bdefault);
  p->d.c.val.min = 0;
  p->d.c.val.max = 0xFFFFFF;
  p->d.c.val.changeflag = 0;
  return addparam(p);
}

param_t *parametric::newparam(const char *name, bool defaultval)
{
  param_t *p = newparam(name);
  if (!p) return NULL;
  p->type = PARAMTYPE::BOOL;
  p->d.b.val.min = 0;
  p->d.b.val.max = 1;
  p->d.b.defaultval = defaultval;
  p->d.b.val.val = (defaultval)?1:0;
  p->d.b.val.changeflag = 0;
  return addparam(p);
}

param_t *parametric::newparam(const char *name, const char *sdefault)
{
  if (sdefault && (strlen(sdefault) > 16)) 
    return NULL; // Too long default already
  param_t *p = newparam(name);
  if (!p) return NULL;
  p->type = PARAMTYPE::STRING;
  if (sdefault) {
    strcpy(p->d.s.val.str, sdefault);
    p->d.s.defaultval = (char *)malloc(strlen(sdefault)+1);
    if (!p->d.s.defaultval) {
      // Out of memory
      free(p);
      return NULL;
    }
    strcpy(p->d.s.defaultval, sdefault);
  }
  else {
    strcpy(p->d.s.val.str, "");
    p->d.s.defaultval = NULL;
  }
  p->d.s.val.changeflag = 0;
  return addparam(p);
}

param_t *parametric::findparam(char *name) {
  if (!name) return NULL;
  if (!*name) return NULL;
  param_t *q = params;
  while (q) {
    if (strcasecmp(name,q->name) == 0) {
      return q;
    }
    q=q->next;
  }
  return NULL;
}

bool parametric::setparam(param_t *p, bool val) {
  if (!p) return false;
  if (p->type != PARAMTYPE::BOOL) return false;
  uint32_t nval = (val)?1:0;
  if (nval != p->d.b.val.val) {
    p->d.b.val.val = nval;
    SETLOCAL(p->d.b.val);
  }
  return true;
}

bool parametric::setparam(param_t *p, int val) {
  if (!p) return false;
  if (p->type == PARAMTYPE::BOOL) return setparam(p, (val!=0)?true:false);
  if (p->type == PARAMTYPE::COL) {
    uint32_t nval = (uint32_t) val;
    if (nval != p->d.c.val.val) {
      p->d.c.val.val = nval;
      SETLOCAL(p->d.c.val);
    }
    return true;
  }
  if (p->type != PARAMTYPE::INT) return false;
  if (val < p->d.i.val.min) val = p->d.i.val.min;
  if (val > p->d.i.val.max) val = p->d.i.val.max;
  if (val != p->d.i.val.val) {
    p->d.i.val.val = val;
    SETLOCAL(p->d.c.val);
  }
  return true;
}

bool parametric::setparam(param_t *p, uint8_t r, uint8_t g, uint8_t b) {
  if (!p) return false;
  if (p->type != PARAMTYPE::COL) return false;
  uint32_t nval = RGB(r,g,b);
  if (nval != p->d.c.val.val) {
    p->d.c.val.val = nval;
    SETLOCAL(p->d.c.val);
  }
  return true;  
}

bool parametric::setparam(param_t *p, const char *val) {
  if (!p) return false;
  if (p->type != PARAMTYPE::STRING) return false;
  if (!val) {
    // Empty string
    if (strlen(p->d.s.val.str) != 0) {
      strcpy(p->d.s.val.str,"");
      SETLOCAL(p->d.s.val);
    }
    return true;
  }
  if (strlen(val) > 16) return false; // too long
  if (strcmp(p->d.s.val.str, val)) {
    strcpy(p->d.s.val.str, val);
    SETLOCAL(p->d.s.val);
  }
  return true;
}

bool parametric::formatparam(param_t *p, char *buffer, int buflen) {
  if (!p) return false;
  if (!buffer) return false;
  if (buflen < 1) return false;
  switch (p->type) {
    case PARAMTYPE::BOOL:
      if (buflen < 4) return false;
      if (p->d.b.val.val == 1) 
        strcpy(buffer,"1");
      else
        strcpy(buffer,"0");
      return true;
    case PARAMTYPE::INT:
      if (buflen < 10) return false;
      sprintf(buffer,"%d",p->d.i.val.val);
      return true;
    case PARAMTYPE::COL:
      if (buflen < 8) return false;
      sprintf(buffer,"%d",p->d.c.val.val);
      return true;
    case PARAMTYPE::STRING:
      if (buflen < strlen(p->d.s.val.str)+1) return false;
      strcpy(buffer,p->d.s.val.str);
      return true;
  }
  return false;
}

bool parametric::resetparam(param_t *p) {
  if (!p) return false;
  switch (p->type) {
  case PARAMTYPE::BOOL:
    if (p->d.b.val.val != ((p->d.b.defaultval)?1:0)) {
      p->d.b.val.val = p->d.b.defaultval;
      SETLOCAL(p->d.b.val);
    }
    return true;
  case PARAMTYPE::INT:
    if (p->d.i.val.val != p->d.i.defaultval) {
      p->d.i.val.val = p->d.i.defaultval;  
      SETLOCAL(p->d.i.val);
    }
    return true;
  case PARAMTYPE::COL:
    if (p->d.c.val.val != p->d.c.defaultval) {
      p->d.c.val.val = p->d.c.defaultval;
      SETLOCAL(p->d.c.val);
    }
    return true;
  case PARAMTYPE::STRING:
    if (!p->d.s.defaultval) {
      // Empty default
      if (strlen(p->d.s.val.str)) {
        strcpy(p->d.s.val.str,"");
        SETLOCAL(p->d.s.val);        
      }
    } else {
      if (strcmp(p->d.s.val.str, p->d.s.defaultval)) {
        strcpy(p->d.s.val.str, p->d.s.defaultval);
        SETLOCAL(p->d.s.val);
      }
    }
    return true;
  }
  return false;
}

bool parametric::resetparams() {
  param_t *p = params;
  while (p) {
    if (!resetparam(p)) return false;
    p=p->next;
  }
  return true;
}

void parametric::updateparams() {
  // Nothing to do  
}

void parametric::updateparam(param_t *param) {
  // Nothing to do
}

void parametric::tick() {
  // Nothing to do
}

void parametric::fastdata(uint8_t *buffer, uint8_t len) {
  if (len >= 3) memcpy(&fastcol0,buffer+0,3);
  if (len >= 6) memcpy(&fastcol1,buffer+3,3);
  if (len >= 9) memcpy(&fastcol2,buffer+6,3);
  if (len >= 12) memcpy(&fastcol3,buffer+9,3);
  if (len >= 15) memcpy(&fastcol4,buffer+12,3);
}
