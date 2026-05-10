#ifndef SMOKE_H
#define SMOKE_H

#include <allegro.h>

#include "helper.h"
#include "level.h"

constexpr int MAX_SMOKE = 20;
constexpr int MAX_SMOKEPUFFS = 20;

extern BITMAP* buffer;
extern Level level;
extern DATAFILE* icons_data;

struct SmokePuff
{
    int x_vel{};
    int x{};
    int y{};
    int wait{};
    int life{};
};

class Smoke
{
  public:
    Smoke() = default;
    int draw();
    int run();
    void release();

    SmokePuff* puff{};
    bool active{};
    
    private:
    int explo_size{};
    int x{};
    int y{};
    int w{};
    int h{};
    int ratio_w{};
    int ratio_h{};
};

#endif
