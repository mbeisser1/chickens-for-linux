#ifndef GAME_H
#define GAME_H

#include <vector>

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

    void show_startup();
    void show_modechooser();
    void show_levelcompleted();
    void show_levelnumber();
    void restart();

    void tick_mode_restart();
    void tick_mode_playing(bool& alert_sound, bool& fire_rocket, bool& fire_shotgun);
    void tick_mode_next_level();
    void tick_mode_paused();
    void tick_mode_gameover(int& rank);

    void draw_frame_playing();
    void draw_frame_paused();
    void draw_frame_gameover(int rank);

    AppContext& ctx_;
    Level terrain_{};
    std::vector<Smoke> smoke_{};
    std::vector<Gem> gem_{};
    std::vector<Chicken> chickens_{};
};

#endif
