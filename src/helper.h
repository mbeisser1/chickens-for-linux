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

constexpr int CHICKEN_HEIGHT = 20;
constexpr int CHICKEN_WIDTH = 24;
constexpr bool FOREVER = true;
constexpr int KILLED_WITH_ROCKET = 1;
constexpr int KILLED_WITH_SHOTGUN = 2;
constexpr int KILLED_WITH_TENDERIZER = 3;
constexpr int LEFT = -1;
constexpr int MAX_GEMS = 10;
constexpr int MODE_GAMEOVER = 2;
constexpr int MODE_RESTART = 3;
constexpr int MODE_PAUSED = 5;
constexpr int MODE_PLAYING = 6;
constexpr int MODE_NEXTLEVEL = 7;
constexpr int MODE_QUIT = 8;
constexpr int NOT_KILLED = 0;
constexpr bool ONCE = false;
constexpr int RIGHT = 1;
constexpr int CROSSED_THE_ROAD = 2;

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
