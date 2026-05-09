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
    void release(const float x, const float y, const float x_vel, const float y_vel);
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
    void release(
        const float at_x, const float at_y, int accuracy, const int death, const int direction);
    void draw();

    bool released;
};

#endif
