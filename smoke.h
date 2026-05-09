#ifndef SMOKE_H
#define SMOKE_H 1

#include <allegro.h>

#include "helper.h"
#include "level.h"

#define MAX_SMOKE 20
#define MAX_SMOKEPUFFS 20

extern BITMAP* buffer;
extern LEVEL level;
extern DATAFILE* icons_data;
extern int ROCKET_SIZE;

struct SMOKEPUFF
{
    int x_vel;
    int x;
    int y;
    int wait;
    int life;
};

class SMOKE
{
  public:
    SMOKE();

    SMOKEPUFF* puff;

    int draw();
    int run();
    int release();

    int explo_size;
    int x;
    int y;
    int w;
    int h;
    int ratio_w;
    int ratio_h;
    bool active;
};

#endif
