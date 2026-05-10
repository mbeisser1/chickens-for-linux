#ifndef GEM_H
#define GEM_H

#include <allegro.h>

#include "helper.h"

constexpr int BONUS_TIMER = 0;
constexpr int BONUS_ROCKETSIZE = 1;
constexpr int BONUS_SCORE = 2;

struct Settings;

class Gem
{
  public:
    Gem() = default;

    void bind_settings(Settings* settings);
    void draw(const RenderContext& render_context) const;
    void launch(float x, float y);
    int run();

    bool active{};
    int type{};
  
  private:
    Settings* settings_{};
    float x{};
    float y{};
    float x_vel{};
    float y_vel{};

};

#endif
