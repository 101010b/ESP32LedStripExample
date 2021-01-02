#ifndef __PARAMETRIC_H__
#define __PARAMETRIC_H__

#include "MultiServiceIO/MultiServiceIO.h"

typedef struct intparam_s {
  I32Var_t val;
  int32_t defaultval;
} intparam_t;

typedef struct colparam_s {
  UI32Var_t val;
  uint32_t defaultval;
} colparam_t;

typedef struct boolparam_s {
  UI32Var_t val;
  bool defaultval;
} boolparam_t;

typedef struct stringparam_s {
  S16Var_t val;
  char *defaultval;
} stringparam_t;

typedef enum {
  INT=0,
  COL,
  BOOL,
  STRING
} PARAMTYPE;

typedef struct param_s {
  char name[32];
  PARAMTYPE type;
  union
  {
    intparam_t i;
    colparam_t c;
    boolparam_t b;
    stringparam_t s;
  } d;
  struct param_s *next;
} param_t;

class parametric {
  public:
  char name[32];
  param_t *params;
  unsigned int fastcol0, fastcol1, fastcol2, fastcol3, fastcol4;

  param_t *findparam(char *name);
  bool setparam(param_t *p, bool val);
  bool setparam(param_t *p, int val);
  bool setparam(param_t *p, uint8_t r, uint8_t g, uint8_t b);
  bool setparam(param_t *p, uint32_t val);
  bool setparam(param_t *p, const char *val);
  bool resetparam(param_t *p);
  bool resetparams();
  param_t *addparam(param_t *p);
  param_t *newparam(const char *name);
  param_t *newparam(const char *name, int32_t minval, int32_t maxval, int32_t defaultval);
  param_t *newparam(const char *name, bool defaultval);
  param_t *newparam(const char *name, uint8_t rdefault, uint8_t gdefault, uint8_t bdefault);
  param_t *newparam(const char *name, const char *sdefault);
  bool formatparam(param_t *p, char *buffer, int buflen);
  virtual void updateparams();
  virtual void updateparam(param_t *param);
  virtual void tick();
  void fastdata(uint8_t *buffer, uint8_t len);

  parametric(const char *_name);
};

#endif
// EOF
