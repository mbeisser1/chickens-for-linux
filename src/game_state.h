#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "helper.h"
#include "settings.h"

struct GameState
{
    explicit GameState(const Settings& settings);

    /** Mirror file/settings into per-run fields. Does not change `playername`. */
    void apply_settings(const Settings& settings)
    {
        tmp_rocket_size = settings.ROCKET_SIZE;
        mute_sound = settings.MUTE || mute_via_cli;
    }

    int chickens_left{};
    int current_level{1};
    int delay_of_levelend{};
    int mode{MODE_PLAYING};
    int kills{};
    int runners{};
    int score{};
    int shots_fired{};
    int timer{};
    int timer_delay{};
    int tenderizers{};
    int tmp_rocket_size{};
    bool alert_mode{};
    bool level_mode{};
    bool mute_sound{};
    /** If true, reloads from config still keep the session muted (`--mute`). */
    bool mute_via_cli{};
    bool not_dead{};
    const char* playername{"player"};
};

#endif
