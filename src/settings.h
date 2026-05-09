#ifndef SETTINGS_H
#define SETTINGS_H

#include <fstream>

#include <string.h>

extern bool FULLSCREEN;
extern bool MUTE;
extern bool TRANSLUCENT_SMOKE;
extern float GRAVITY;
extern int BLOOD_PER_CHUNK;
extern int CHANCE_OF_FLIGHT;
extern int CHANCE_OF_GEM;
extern float CHICKEN_SPEED;
extern int GAME_SPEED_OFFSET;
extern int CHUNKS_PER_CHICKEN;
extern int MAX_CHICKENS;
extern int POINTS_FOR_ROCKET;
extern int POINTS_FOR_SHOTGUN;
extern int POINTS_FOR_TENDERIZER;
extern int INITIAL_CHICKENS;
extern int RESPAWN_RATE;
extern int ROCKET_RELOAD;
extern int ROCKET_SIZE;
extern int SHOTGUN_RELOAD;
extern int SHOTGUN_SIZE;
extern int SMOKE_LINGERING;
extern int SMOKE_PUFFS;
extern int TIMER;
extern int VOLUME;

int load_config(const char*);
int load_stock_config();

#endif
