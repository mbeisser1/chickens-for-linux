#include "animation.h"

void Animation::load(const DATAFILE* d)
{
    data = d;
    frames = items_in_datafile(data);
    step = {};
    angle = {};
    old_angle = {};
    frame_direction = 1;
    played_once = {};
}

void Animation::play(BITMAP* target)
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
        step = {};
    }

    if (step < 0)
    {
        step = frames - 1;
    }
}

void Animation::reverse()
{
    frame_direction = -frame_direction;
}

void Cutscene::load(const char* path)
{
    data = load_datafile(path);
    frames = items_in_datafile(data);
    step = {};
}

int Cutscene::play(BITMAP* target)
{
    draw_rle_sprite(target, (RLE_SPRITE*)data[step++].dat, (int)x, (int)y);

    if (step == frames)
    {
        --step;
        return {};
    }
    else
    {
        return {1};
    }
}
