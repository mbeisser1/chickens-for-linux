#include "animation.h"

ANIMATION::ANIMATION()
{
    loop = true;
    slide = false;
    hflip = false;
    speed = 400;
    x = 0;
    y = 0;
}

void ANIMATION::load(DATAFILE* d)
{
    data = d;
    frames = items_in_datafile(data);
    step = 0;
    angle = 0;
    old_angle = 0;
    frame_direction = 1;
    played_once = false;
}

void ANIMATION::play(BITMAP* target)
{
    if (angle == 0)
    {
        if (hflip)
        {
            draw_sprite_h_flip(target, (BITMAP*)data[step].dat, (int)x, (int)y);
        }
        else
        {
            draw_sprite(target, (BITMAP*)data[step].dat, (int)x, (int)y);
        }
    }
    else
    {
        rotate_sprite(target, (BITMAP*)data[step].dat, (int)x, (int)y, itofix((int)angle));
    }

    if (played_once && loop == false)
    {
        return;
    }

    step += frame_direction;

    if (slide)
    {
        if (step == frames)
        {
            reverse();
            step = frames - 2;
        }

        if (step < 0)
        {
            reverse();
            step = 1;
            played_once = true;
        }
    }

    if (step == frames)
    {
        played_once = true;
        step = 0;
    }

    if (step < 0)
    {
        step = frames - 1;
    }
}

void ANIMATION::reverse()
{
    frame_direction = -frame_direction;
}

CUTSCENE::CUTSCENE()
{
    x = 0;
    y = 0;
}

void CUTSCENE::load(char* path)
{
    data = load_datafile(path);
    frames = items_in_datafile(data);
    step = 0;
}

int CUTSCENE::play(BITMAP* target)
{
    draw_rle_sprite(target, (RLE_SPRITE*)data[step++].dat, (int)x, (int)y);

    if (step == frames)
    {
        --step;
        return 0;
    }
    else
    {
        return 1;
    }
}
