#ifndef LEVEL_H
#define LEVEL_H

#include <allegro.h>
#include <math.h>

struct AppAssets;

constexpr int MAX_LEVELHEIGHT = 150;

struct Level
{
    void create(AppAssets& app_assets);

    BITMAP* image{};
    BITMAP* terrain{};
    int height[800]{};
};

#endif
