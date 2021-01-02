#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "motion.h"

// Constructor
motion::motion(uint8_t *r, uint8_t *g, uint8_t *b) : effect("Motion", r, g, b)
{
    motionobjects = NULL;

    mShape = newparam("shape", SHAPE::DISCHARGE, SHAPE::COSINE, SHAPE::DISCHARGE);
    mParam = newparam("param", 1, 20, 10); // Check Range
    mSpeed = newparam("speed", 1, 20, 7); // Check Range
    mShapeN = newparam("shapeN", 1, 20, 10); // Check Range

    globalcol = NULL;

    updateShapes();
}

// MOTION SEQUENCE OBJECT AND UTILS 
motionseq::motionseq()
{
    pos = 0.0;

    shape = SHAPE::OFF;
    param = 10;
    speed = 1.0;
    dir = 1;

    colr = colg = colb = 0;
    ofs = 0;
    amp = 100;
}

float motionseq::getfunc(float n, SHAPE shape, float param)
{
    switch (shape)
    {
    case SHAPE::DISCHARGE: // limited exp decay step
        if (n > 0)
            return 0.0;
        if (n < -4.0 * param)
            return 0.0;
        return exp(n / param);
    case SHAPE::GAUSS: // exp pulse
        if ((n > 2.0 * param) || (n < -2.0 * param))
            return 0.0;
        return exp(-n * n / (param * param));
    case SHAPE::SINE: // endless Sine
        return (1.0 + sin(2.0 * M_PI * n / (50.0 * param))) / 2.0;
    case SHAPE::COSINE: // endless CoSine
        return (1.0 + cos(2.0 * M_PI * n / (50.0 * param))) / 2.0;
    default:
        return 0.0;
    }
    return 0.0;
}

float motionseq::getfunc(int n, SHAPE shape, float param, float pos, int dir)
{
    if (dir > 0)
        return getfunc((float)n - pos, shape, param);
    return getfunc(pos - (float)n, shape, param);
}

float motionseq::normalize(SHAPE shape, float param, float pos)
{
    switch (shape)
    {
    case SHAPE::DISCHARGE:
        return pos;
    case SHAPE::GAUSS:
        return pos;
    case SHAPE::SINE:
        return pos - floor(pos / (50.0 * param)) * (50.0 * param);
    case SHAPE::COSINE:
        return pos - floor(pos / (50.0 * param)) * (50.0 * param);
    default:
        return 0.0;
    }
    return 0.0;
}

float motionseq::normalize(SHAPE shape, float param, float pos, int dir)
{
    if (dir > 0)
        return normalize(shape, param, pos);
    return -normalize(shape, param, -pos);
}

void motionseq::get(int led, int *r, int *g, int *b)
{
    if (shape <= 0)
    {
        *r = *g = *b = 0;
        return;
    }
    float f = (float)amp / 100 * getfunc(led, shape, param, pos, dir) + (float)ofs / 100;

    if (f < 0)
        f = 0;
    if (f > 1.0)
        f = 1.0;

    *r = (int)floor((float)colr * f);
    *g = (int)floor((float)colg * f);
    *b = (int)floor((float)colb * f);
}

void motionseq::step()
{
    if (shape == 0)
        return;

    if (dir > 0)
        pos += speed;
    else
        pos -= speed;

    switch (shape)
    {
    case SHAPE::DISCHARGE:
        if ((dir > 0) && (pos > LEDS + 4 * param))
            shape = SHAPE::OFF;
        else if ((dir <= 0) && (pos < -4 * param))
            shape = SHAPE::OFF;
        break;
    case SHAPE::GAUSS:
        if ((dir > 0) && (pos > LEDS + 2 * param))
            shape = SHAPE::OFF;
        else if ((dir <= 0) && (pos < -2 * param))
            shape = SHAPE::OFF;
        break;
    case SHAPE::SINE:
        pos = normalize(shape, param, pos, dir);
        break;
    case SHAPE::COSINE:
        pos = normalize(shape, param, pos, dir);
        break;
    default:
        break;
    }
}

// MOTION OBJECT
void motion::tick()
{
    if (motionobjects->size() != mShapeN->d.i.val.val)
        updateShapes();
    for (int i = 0; i < LEDS; i++)
    {
        int r, g, b;
        int tr, tg, tb;
        r = g = b = 0;
        for (int j = 0; j < motionobjects->size(); j++)
        {
            motionobjects->at(j)->get(i, &tr, &tg, &tb);
            r += tr;
            g += tg;
            b += tb;
        }
        if (r < 0)
            r = 0;
        if (r > 255)
            r = 255;
        if (g < 0)
            g = 0;
        if (g > 255)
            g = 255;
        if (b < 0)
            b = 0;
        if (b > 255)
            b = 255;
        list.r[i] = r;
        list.g[i] = g;
        list.b[i] = b;
    }
    for (int i = 0; i < motionobjects->size(); i++)
        motionobjects->at(i)->step();
    for (int i = 0; i < motionobjects->size(); i++)
        if (motionobjects->at(i)->shape == SHAPE::OFF)
            setNewMotion(motionobjects->at(i));
}

void motion::setNewMotion(motionseq *m)
{
    int speed = mSpeed->d.i.val.val * 5 / 10;
    int sp = rand() % speed  + speed;
    m->shape = (SHAPE)mShape->d.i.val.val;
    int dr = rand() % 2;
    if (dr == 1)
    {
        m->pos = 0;
        m->dir = 1;
    }
    else
    {
        m->pos = LEDS - 1;
        m->dir = -1;
    }
    m->speed = (float)sp / 4.0;
    m->param = (float)mParam->d.i.val.val / 2.0;
    unsigned char ur, ug, ub;
    randcol(&ur, &ug, &ub);
    m->colr = ur;
    m->colg = ug;
    m->colb = ub;
}

void motion::setInitialMotion(motionseq *m)
{
    setNewMotion(m);
    m->pos = rand() % LEDS;
}

void motion::updateShapes()
{
    if (motionobjects == NULL)
    {
        motionobjects = new std::vector<motionseq *>();
        for (int i = 0; i < mShapeN->d.i.val.val; i++) 
            motionobjects->push_back(new motionseq());
    }
    else
    {
        if (mShapeN->d.i.val.val != motionobjects->size())
        {
            if (mShapeN->d.i.val.val < motionobjects->size())
            {
                while (motionobjects->size() != mShapeN->d.i.val.val)
                {
                    motionseq *seq = motionobjects->back();
                    motionobjects->pop_back();
                    delete seq;
                }
            }
            else
            {
                while (motionobjects->size() != mShapeN->d.i.val.val)
                {
                    motionseq *ms = new motionseq();
                    setInitialMotion(ms);
                    motionobjects->push_back(ms);
                }
            }
        }
    }
    for (int i = 0; i < motionobjects->size(); i++)
    {
        if (motionobjects->at(i)->shape == SHAPE::OFF)
        {
            // Set new
            setInitialMotion(motionobjects->at(i));
        }
        else if (motionobjects->at(i)->shape != (SHAPE)mShape->d.i.val.val)
        {
            // Move to new shape
            switch (motionobjects->at(i)->shape)
            {
            case SHAPE::SINE:
            case SHAPE::COSINE:
                setNewMotion(motionobjects->at(i));
                break;
            default:
                break;
            }
        }
    }
}

void motion::setGlobalCol(colorgen *_col)
{
    globalcol = _col;
}
