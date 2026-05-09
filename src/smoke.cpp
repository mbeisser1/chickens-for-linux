#include "smoke.h"

SMOKE::SMOKE()
{
    active = false;
}

int SMOKE::run()
{
    if (active)
    {
        int gone = 0;

        explo_size -= 15;
        w++;
        h++;

        for (int i = 0; i < SMOKE_PUFFS; ++i)
        {
            if (--puff[i].wait < 0)
            {
                if (puff[i].life > 0)
                {
                    puff[i].x += puff[i].x_vel;
                    puff[i].y += 5;
                    puff[i].life--;
                }
                else
                {
                    ++gone;
                }
            }
        }

        return active = gone < SMOKE_PUFFS;
    }

    explo_size = ROCKET_SIZE;
    w = 6;
    h = 4;

    return 0;
}

int SMOKE::release()
{
    puff = new SMOKEPUFF[SMOKE_PUFFS];

    explo_size = ROCKET_SIZE;
    x = mouse_x;
    y = level.height[mouse_x] - CHICKEN_HEIGHT;
    w = 6;
    h = 4;

    for (int i = 0; i < SMOKE_PUFFS; ++i)
    {
        puff[i].x = mouse_x;
        puff[i].y = CHICKEN_HEIGHT - rand() % CHICKEN_HEIGHT + 15;
        puff[i].x_vel = rand() % 3 - rand() % 3;
        puff[i].wait = i;
        puff[i].life = SMOKE_LINGERING;
    }

    active = true;
}

int SMOKE::draw()
{
    BITMAP* light;
    BITMAP* cloud;

    if (active)
    {
        if (explo_size > 0)
        {
            light = create_system_bitmap(ROCKET_SIZE * 2, ROCKET_SIZE * 2);

            clear_to_color(light, makecol(255, 0, 255));

            circlefill(light, ROCKET_SIZE, ROCKET_SIZE, explo_size, makecol(255, rand() % 255, 0));

            if (TRANSLUCENT_SMOKE)
            {
                set_trans_blender(255, 255, 255, 100);
                draw_trans_sprite(
                    buffer, light, x - ROCKET_SIZE, SCREEN_H - (y + ROCKET_SIZE + CHICKEN_HEIGHT));
            }
            else
            {
                draw_sprite(
                    buffer, light, x - ROCKET_SIZE, SCREEN_H - (y + ROCKET_SIZE + CHICKEN_HEIGHT));
            }

            destroy_bitmap(light);
        }

        cloud = create_system_bitmap(w, h);

        for (int i = 0; i < SMOKE_PUFFS; ++i)
        {
            if (--puff[i].wait < 0)
            {
                clear_to_color(cloud, makecol(255, 0, 255));

                if (puff[i].life > 0)
                {
                    stretch_sprite(cloud, (BITMAP*)icons_data[1].dat, 0, 0, w, h);

                    if (TRANSLUCENT_SMOKE)
                    {
                        set_trans_blender(255, 255, 255, puff[i].life * 2);
                        draw_trans_sprite(
                            buffer, cloud, puff[i].x - w / 2, SCREEN_H - (y + puff[i].y) - h / 2);
                    }
                    else
                    {
                        draw_sprite(
                            buffer, cloud, puff[i].x - w / 2, SCREEN_H - (y + puff[i].y) - h / 2);
                    }
                }
            }
        }

        destroy_bitmap(cloud);
    }
}
