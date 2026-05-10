#ifndef GAME_H
#define GAME_H

#include <array>

#include "chicken.h"
#include "gem.h"
#include "helper.h"
#include "settings.h"
#include "smoke.h"

class Game
{
  public:
    Game(std::array<Smoke, MAX_SMOKE>& smoke,
         std::array<Gem, MAX_GEMS>& gem,
         std::array<Chicken, MAX_CHICKENS_CAPACITY>& chicken,
         const RenderContext& render_context);

    int run();

  private:
    std::array<Smoke, MAX_SMOKE>& smoke_;
    std::array<Gem, MAX_GEMS>& gem_;
    std::array<Chicken, MAX_CHICKENS_CAPACITY>& chicken_;
    const RenderContext& render_context_;
};

#endif
