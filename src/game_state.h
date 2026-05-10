#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <cstdlib>

#include "helper.h"
#include "settings.h"

struct GameState
{
    void apply_settings(const Settings& settings)
    {
        playername = getenv("USER");
        if (playername == nullptr)
        {
            playername = "player";
        }

        mute_sound = settings.MUTE;
        tmp_rocket_size = settings.ROCKET_SIZE;
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
    bool not_dead{};
    const char* config_path{CHICKENS_ASSETS_REL("options.cfg")};
    const char* playername{"player"};
};

#endif
