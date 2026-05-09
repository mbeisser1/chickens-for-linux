#ifndef HELPER_H
#define HELPER_H

#include <allegro.h>

/* Packaged data lives under assets/ (paths relative to the process cwd; run from repo root). */
#define CHICKENS_ASSETS_REL(relative_path) ("assets/" relative_path)

/* Non-deprecated Allegro 4 text APIs (-1 = transparent text background). */
#define CHICKENS_TEXTOUT_CENTRE(bmp, f, str, x, y, color) textout_centre_ex((bmp), (f), (str), (x), (y), (color), -1)
#define CHICKENS_TEXTOUT_RIGHT(bmp, f, str, x, y, color) textout_right_ex((bmp), (f), (str), (x), (y), (color), -1)
#define CHICKENS_TEXTPRINTF(bmp, f, x, y, color, fmt, ...) textprintf_ex((bmp), (f), (x), (y), (color), -1, (fmt), ##__VA_ARGS__)
#define CHICKENS_TEXTPRINTF_CENTRE(bmp, f, x, y, color, fmt, ...) \
    textprintf_centre_ex((bmp), (f), (x), (y), (color), -1, (fmt), ##__VA_ARGS__)

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

int items_in_datafile(const DATAFILE* data);
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
