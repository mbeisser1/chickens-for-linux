#ifndef LEVEL_H
#define LEVEL_H

#include <allegro.h>
#include <math.h>

constexpr int MAX_LEVELHEIGHT = 150;

extern DATAFILE* terrain_data;

struct Level
{
    void create();

    BITMAP* image{};
    BITMAP* terrain{};
    int height[800]{};
};

#endif
