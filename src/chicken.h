#ifndef CHICKEN_H
#define CHICKEN_H

#include <allegro.h>

#include "animation.h"
#include "gore.h"

extern DATAFILE* chicken_data;
extern DATAFILE* flyingchicken_data;
extern Level level;

class Chicken
{
  public:
    Chicken();
    void draw(BITMAP* target, const DATAFILE* giblet_data);
    void reset();
    int run();

    Kfc dead{};
    Animation running{};
    Animation flying{};

    float x{};
    float y{};
    float y_vel{};
    int direction{};
    int flight{};
    int alive{};
};

#endif
