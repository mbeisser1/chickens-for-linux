#ifndef GEM_H
#define GEM_H

#include <allegro.h>

#include "helper.h"

constexpr int BONUS_TIMER = 0;
constexpr int BONUS_ROCKETSIZE = 1;
constexpr int BONUS_SCORE = 2;

class Gem
{
  public:
    Gem() = default;

    void draw(const RenderContext& render_context) const;
    void launch(float x, float y);
    int run();

    bool active{};
    int type{};
  
  private:
    float x{};
    float y{};
    float x_vel{};
    float y_vel{};

};

#endif
