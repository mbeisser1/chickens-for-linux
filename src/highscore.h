#ifndef HISCORE_H
#define HISCORE_H

#include <fstream>
#include <iostream>

#include <allegro.h>
#include <string.h>

constexpr int HIGHSCORE_TABLE = 10;

void show_highscores(int, BITMAP*, BITMAP*);
int save_highscore(const char* name, int score);

#endif
