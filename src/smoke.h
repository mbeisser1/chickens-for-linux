#ifndef SMOKE_H
#define SMOKE_H

#include <memory>

#include <allegro.h>

#include "helper.h"

struct Level;

constexpr int MAX_SMOKE = 20;
constexpr int MAX_SMOKEPUFFS = 20;

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
    void bind_level(Level* terrain);
    int draw(const RenderContext& render_context);
    int run();
    void release();

    std::unique_ptr<SmokePuff[]> puff{};
    bool active{};

  private:
    Level* terrain_{};
    int explo_size{};
    int x{};
    int y{};
    int w{};
    int h{};
    int ratio_w{};
    int ratio_h{};
};

#endif
