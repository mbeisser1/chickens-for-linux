#ifndef GEM_H
#define GEM_H 1

#include <allegro.h>

#include "helper.h"

#define BONUS_TIMER 0
#define BONUS_ROCKETSIZE 1

extern void play_sound(SAMPLE*, int, int, bool);

extern BITMAP* buffer;
extern DATAFILE* gem_data;
extern SAMPLE* sound_gemcollect;
extern int timer;

class GEM
{
  public:
    GEM();

    void draw();
    void launch(float, float);
    int run();
    int type;
    float x;
    float y;
    float x_vel;
    float y_vel;

    bool active;
};

#endif
