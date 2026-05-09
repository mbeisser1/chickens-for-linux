#include "gem.h"

GEM::GEM()
{
    active = false;
}

void GEM::draw()
{
    if (active)
    {
        draw_sprite(buffer, (BITMAP*)gem_data[type].dat, (int)x, (int)y);
    }
}

void GEM::launch(float X, float Y)
{
    type = rand() % 3;

    x = X + rand() % CHICKEN_WIDTH;
    y = Y + rand() % CHICKEN_HEIGHT / 2;
    x_vel = rand() % 6 - rand() % 6;
    y_vel = -(rand() % 5) - 12;
    active = true;
}

int GEM::run()
{
    if (active)
    {
        y_vel += GRAVITY / 2;
        x += x_vel;
        y += y_vel;

        if (mouse_x > x - 10 && mouse_x < x + 20)
        {
            if (mouse_y > y - 10 && mouse_y < y + 18)
            {
                active = false;
                return 1;
            }
        }

        if (y > SCREEN_H)
        {
            active = false;
        }
    }

    return 0;
}
