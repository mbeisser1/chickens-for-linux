#include "smoke.h"
#include <memory>

int Smoke::run()
{
    if (active)
    {
        int gone = 0;

        explo_size -= 15;
        w++;
        h++;

        for (int i = 0; i < game_settings.SMOKE_PUFFS; ++i)
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

        return active = gone < game_settings.SMOKE_PUFFS;
    }

    explo_size = game_settings.ROCKET_SIZE;
    w = 6;
    h = 4;

    return 0;
}

void Smoke::release()
{
    puff = std::unique_ptr<SmokePuff[]>(new SmokePuff[game_settings.SMOKE_PUFFS]);

    explo_size = game_settings.ROCKET_SIZE;
    x = mouse_x;
    y = level.height[mouse_x] - CHICKEN_HEIGHT;
    w = 6;
    h = 4;

    for (int i = 0; i < game_settings.SMOKE_PUFFS; ++i)
    {
        puff[i].x = mouse_x;
        puff[i].y = CHICKEN_HEIGHT - rand() % CHICKEN_HEIGHT + 15;
        puff[i].x_vel = rand() % 3 - rand() % 3;
        puff[i].wait = i;
        puff[i].life = game_settings.SMOKE_LINGERING;
    }

    active = true;
}

int Smoke::draw()
{
    BITMAP* light;
    BITMAP* cloud;

    if (active)
    {
        if (explo_size > 0)
        {
            light = create_system_bitmap(game_settings.ROCKET_SIZE * 2, game_settings.ROCKET_SIZE * 2);

            clear_to_color(light, makecol(255, 0, 255));

            circlefill(light, game_settings.ROCKET_SIZE, game_settings.ROCKET_SIZE, explo_size, makecol(255, rand() % 255, 0));

            if (game_settings.TRANSLUCENT_SMOKE)
            {
                set_trans_blender(255, 255, 255, 100);
                draw_trans_sprite(
                    buffer, light, x - game_settings.ROCKET_SIZE, SCREEN_H - (y + game_settings.ROCKET_SIZE + CHICKEN_HEIGHT));
            }
            else
            {
                draw_sprite(
                    buffer, light, x - game_settings.ROCKET_SIZE, SCREEN_H - (y + game_settings.ROCKET_SIZE + CHICKEN_HEIGHT));
            }

            destroy_bitmap(light);
        }

        cloud = create_system_bitmap(w, h);

        for (int i = 0; i < game_settings.SMOKE_PUFFS; ++i)
        {
            if (--puff[i].wait < 0)
            {
                clear_to_color(cloud, makecol(255, 0, 255));

                if (puff[i].life > 0)
                {
                    stretch_sprite(cloud, (BITMAP*)icons_data[1].dat, 0, 0, w, h);

                    if (game_settings.TRANSLUCENT_SMOKE)
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

    return 0;
}
