#ifndef HISCORE_H
#define HISCORE_H

#include <fstream>
#include <iostream>

#include <allegro.h>
#include <string.h>

constexpr int HIGHSCORE_TABLE = 10;

extern BITMAP* buffer;
extern FONT* font_big;
extern FONT* font_interface;

void show_highscores(int, BITMAP*);
int save_highscore(const char* name, int score);

#endif
