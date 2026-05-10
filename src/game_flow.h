#ifndef GAME_FLOW_H
#define GAME_FLOW_H

#include <array>

#include "chicken.h"
#include "gem.h"
#include "helper.h"
#include "smoke.h"

struct Level;

int mode_manager();

void show_startup();
void show_modechooser();
void show_levelcompleted();
void show_levelnumber();

void fadeout(int color, int duration);

void restart(std::array<Chicken, MAX_CHICKENS_CAPACITY>& chicken,
             std::array<Gem, MAX_GEMS>& gem,
             std::array<Smoke, MAX_SMOKE>& smoke,
             Level& terrain);

void earn_bonus(int type);

void next_level(int level_number);

void show_statistics();

void weapon_manager(bool* fire_rocket, bool* fire_shotgun);

void play_sound(const SAMPLE* snd, int volume, int pan, bool loop);

#endif
