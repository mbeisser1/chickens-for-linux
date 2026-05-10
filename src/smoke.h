#ifndef SMOKE_H
#define SMOKE_H

#include <memory>

#include <allegro.h>

#include "helper.h"

struct Level;
struct Settings;

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
    Smoke(Level& terrain, Settings& settings);

    int draw(const RenderContext& render_context);
    int run();
    void release();

    std::unique_ptr<SmokePuff[]> puff{};
    bool active{};

  private:
    Level* terrain_{};
    Settings* settings_{};
    int explo_size{};
    int x{};
    int y{};
    int w{};
    int h{};
    int ratio_w{};
    int ratio_h{};
};

#endif
