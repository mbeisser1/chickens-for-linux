#ifndef LEVEL_H
#define LEVEL_H

#include <allegro.h>
#include <math.h>

#define MAX_LEVELHEIGHT 150

extern DATAFILE* terrain_data;

struct LEVEL
{
    void create();

    BITMAP* image;
    BITMAP* terrain;
    int height[800];
};

#endif
