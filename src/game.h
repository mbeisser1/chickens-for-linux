#ifndef GAME_H
#define GAME_H

#include <array>

#include "app_context.h"
#include "chicken.h"
#include "gem.h"
#include "helper.h"
#include "level.h"
#include "settings.h"
#include "smoke.h"

class Game
{
  public:
    /** Installs Allegro, sets gfx mode, loads assets, binds entities. `app` must outlive `Game`. */
    explicit Game(AppContext& app);

    void run();

  private:
    void show_highscores(int player_rank_one_based);

    AppContext& ctx_;
    Level terrain_{};
    std::array<Smoke, MAX_SMOKE> smoke_{};
    std::array<Gem, MAX_GEMS> gem_{};
    std::array<Chicken, MAX_CHICKENS_CAPACITY> chicken_{};
};

#endif
