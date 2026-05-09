#ifndef HISCORE_H
#define HISCORE_H 1

#define HIGHSCORE_TABLE 10

#include <fstream>
#include <iostream>

#include <allegro.h>
#include <string.h>

extern BITMAP* buffer;
extern FONT* font_big;
extern FONT* font_interface;

void show_highscores(int, BITMAP*);
int save_highscore(char*, int);

#endif
