#ifndef GORE_H
#define GORE_H

#include <allegro.h>

#include "helper.h"
#include "level.h"

extern int CHUNKS_PER_CHICKEN;
extern int BLOOD_PER_CHUNK;
extern BITMAP* buffer;
extern LEVEL level;
extern DATAFILE* giblet_data;

struct BLOOD
{
    void release(float, float, float, float);
    void run();
    void draw();

  private:
    float x;
    float y;
    float x_vel;
    float y_vel;
};

struct GIBLET
{
    GIBLET();
    BLOOD* blood;
    float x;
    float y;
    float x_vel;
    float y_vel;
    int image;
    bool landed;
};

struct KFC
{
    KFC();
    GIBLET* chunk;

    void explode();
    void release(float, float, int, int, int);
    void draw();

    bool released;
};

#endif
