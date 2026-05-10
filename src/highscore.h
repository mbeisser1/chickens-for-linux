#ifndef HISCORE_H
#define HISCORE_H

#include <fstream>
#include <iostream>

#include <allegro.h>
#include <string.h>

struct AppAssets;

constexpr int HIGHSCORE_TABLE = 10;

void show_highscores(int rank, BITMAP* target, BITMAP* background, AppAssets& app_assets);
int save_highscore(const char* name, int score);

#endif
