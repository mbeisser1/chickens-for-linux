#ifndef GORE_H
#define GORE_H

#include <memory>

#include <allegro.h>

#include "helper.h"
#include "level.h"

extern Level level;
extern DATAFILE* giblet_data;

struct Blood
{
    void release(const float x, const float y, const float x_vel, const float y_vel);
    void run();
    void draw(BITMAP* target);

  private:
    float x{};
    float y{};
    float x_vel{};
    float y_vel{};
};

struct Giblet
{
    Giblet();
    std::unique_ptr<Blood[]> blood{};
    float x{};
    float y{};
    float x_vel{};
    float y_vel{};
    int image{};
    bool landed{};
};

struct Kfc
{
    Kfc() = default;
    std::unique_ptr<Giblet[]> chunk{};

    void explode();
    void release(const float at_x, const float at_y, int accuracy, const int death, const int direction);
    void draw(BITMAP* target, const DATAFILE* giblet_data);

    bool released{};
};

#endif
