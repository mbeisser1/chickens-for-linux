#ifndef ANIMATION_H
#define ANIMATION_H

#include <allegro.h>

#include "helper.h"

extern BITMAP* buffer;

struct ANIMATION
{
    ANIMATION();
    void load(DATAFILE*);
    void play(BITMAP*); // Play the animation!
    void reverse();     // Play it backwards (or forwards if it's already backwards)

    DATAFILE* data; // Datafile containing images
    float x;        // Location to draw on *target
    float y;
    int step;    // Current frame
    int speed;   // Frame-rate in milliseconds
    bool loop;   // Shall we repeat the animation continuously?
    bool slide;  // Slide from first frame to last, then back.
    bool hflip;  // Flip the animation horizontally?
    float angle; // Angle to rotate the images

  private:
    int frames;          // Total number of frames in animation
    int frame_direction; // Current direction of the sliding frames
    float old_angle;     // Previous angle
    bool played_once;    // This is only used when loop == false
};

struct CUTSCENE
{
    CUTSCENE();
    void load(char*);
    int play(BITMAP*); // Play the cutscene!

    DATAFILE* data; // Datafile containing images
    float x;        // Location to draw on *target
    float y;
    int step; // Current frame

  private:
    int frames; // Total number of frames in cutscene
};

#endif
