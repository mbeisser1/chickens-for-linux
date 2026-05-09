#ifndef HELPER_H
#define HELPER_H 1

#include <allegro.h>

/* Packaged data lives under assets/ (paths relative to the process cwd; run from repo root). */
#define CHICKENS_ASSETS_REL(relative_path) "assets/" relative_path

#define CHICKEN_HEIGHT 20
#define CHICKEN_WIDTH 24
#define FOREVER TRUE
#define KILLED_WITH_ROCKET 1
#define KILLED_WITH_SHOTGUN 2
#define KILLED_WITH_TENDERIZER 3
#define LEFT -1
#define MAX_GEMS 10
#define MODE_GAMEOVER 2
#define MODE_RESTART 3
#define MODE_PAUSED 5
#define MODE_PLAYING 6
#define MODE_NEXTLEVEL 7
#define MODE_QUIT 8
#define NOT_KILLED 0
#define ONCE FALSE
#define RIGHT 1

int items_in_datafile(DATAFILE*);
void Timer();

extern bool TRANSLUCENT_SMOKE;
extern float GRAVITY;
extern int CHANCE_OF_FLIGHT;
extern float CHICKEN_SPEED;
extern int ROCKET_SIZE;
extern int SMOKE_PUFFS;
extern int SMOKE_LINGERING;

extern volatile int game_time;

#endif
