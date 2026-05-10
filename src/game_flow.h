#ifndef GAME_FLOW_H
#define GAME_FLOW_H

#include <array>

#include "app_context.h"
#include "chicken.h"
#include "gem.h"
#include "helper.h"
#include "smoke.h"

struct Level;

int mode_manager(AppContext& ctx);

void show_startup(AppContext& ctx);
void show_modechooser(AppContext& ctx);
void show_levelcompleted(AppContext& ctx);
void show_levelnumber(AppContext& ctx);

void fadeout(AppContext& ctx, int color, int duration);

void restart(std::array<Chicken, MAX_CHICKENS_CAPACITY>& chicken,
             std::array<Gem, MAX_GEMS>& gem,
             std::array<Smoke, MAX_SMOKE>& smoke,
             Level& terrain,
             AppContext& ctx);

void earn_bonus(AppContext& ctx, int type);

void next_level(AppContext& ctx, int level_number);

void show_statistics(AppContext& ctx);

void weapon_manager(AppContext& ctx, bool* fire_rocket, bool* fire_shotgun);

void play_sound(AppContext& ctx, const SAMPLE* snd, int volume, int pan, bool loop);

#endif
