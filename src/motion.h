#ifndef __MOTION_H__
#define __MOTION_H__

#include <vector>

#include "ledctrl.h"
#include "effect.h"

typedef enum {
    OFF,
    DISCHARGE,
    GAUSS,
    SINE,
    COSINE
} SHAPE;

class motionseq
{
public:
    SHAPE shape;
    float param;
    float pos;
    float speed;
    int dir;
    int colr, colg, colb;
    int ofs;
    int amp;

    motionseq();

    // Static Functions
    static float getfunc(float n, SHAPE shape, float param);
    static float getfunc(int n, SHAPE shape, float param, float pos, int dir);
    static float normalize(SHAPE shape, float param, float pos);
    static float normalize(SHAPE shape, float param, float pos, int dir);

    // Dynamics
    void get(int led, int *r, int *g, int *b);
    void step();
};

class motion : public effect
{
protected:
    std::vector<motionseq *> *motionobjects;

public:
    param_t *mShape;
    param_t *mParam;
    param_t *mSpeed;
    param_t *mShapeN;
    colorgen *globalcol;

    void setNewMotion(motionseq *m);
    void setInitialMotion(motionseq *m);
    void updateShapes();

    motion(uint8_t *r, uint8_t *g, uint8_t *b);
    void setGlobalCol(colorgen *_col);
    void tick();
};

#endif // __MOTION_H__
