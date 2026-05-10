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

    void tick_mode_restart(AppContext& c);
    void tick_mode_playing(AppContext& c, bool& alert_sound, bool& fire_rocket, bool& fire_shotgun);
    void tick_mode_next_level(AppContext& c);
    void tick_mode_paused(AppContext& c);
    void tick_mode_gameover(AppContext& c, int& rank);

    void draw_frame_playing(AppContext& c);
    void draw_frame_paused(AppContext& c);
    void draw_frame_gameover(AppContext& c, int rank);

    AppContext& ctx_;
    Level terrain_{};
    std::array<Smoke, MAX_SMOKE> smoke_{};
    std::array<Gem, MAX_GEMS> gem_{};
    std::array<Chicken, MAX_CHICKENS_CAPACITY> chicken_{};
};

#endif
