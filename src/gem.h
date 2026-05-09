#ifndef GEM_H
#define GEM_H

#include <allegro.h>

#include "helper.h"

#define BONUS_TIMER 0
#define BONUS_ROCKETSIZE 1

extern void play_sound(const SAMPLE* sample, int volume, int pan, bool loop);

extern BITMAP* buffer;
extern DATAFILE* gem_data;
extern SAMPLE* sound_gemcollect;
extern int timer;

class GEM
{
  public:
    GEM();

    void draw() const;
    void launch(float x, float y);
    int run();
    int type;
    float x;
    float y;
    float x_vel;
    float y_vel;

    bool active;
};

#endif
