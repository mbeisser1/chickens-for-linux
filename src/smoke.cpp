#include "smoke.h"

#include <memory>

#include "level.h"
#include "settings.h"

void Smoke::bind_level(Level* terrain)
{
    terrain_ = terrain;
}

void Smoke::bind_settings(Settings* settings)
{
    settings_ = settings;
}

int Smoke::run()
{
    Settings& tune = *settings_;

    if (active)
    {
        int gone = 0;

        explo_size -= 15;
        w++;
        h++;

        for (int i = 0; i < tune.SMOKE_PUFFS; ++i)
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

        return active = gone < tune.SMOKE_PUFFS;
    }

    explo_size = tune.ROCKET_SIZE;
    w = 6;
    h = 4;

    return 0;
}

void Smoke::release()
{
    Settings& tune = *settings_;

    puff = std::unique_ptr<SmokePuff[]>(new SmokePuff[tune.SMOKE_PUFFS]);

    explo_size = tune.ROCKET_SIZE;
    x = mouse_x;
    y = terrain_->height[mouse_x] - CHICKEN_HEIGHT;
    w = 6;
    h = 4;

    for (int i = 0; i < tune.SMOKE_PUFFS; ++i)
    {
        puff[i].x = mouse_x;
        puff[i].y = CHICKEN_HEIGHT - rand() % CHICKEN_HEIGHT + 15;
        puff[i].x_vel = rand() % 3 - rand() % 3;
        puff[i].wait = i;
        puff[i].life = tune.SMOKE_LINGERING;
    }

    active = true;
}

int Smoke::draw(const RenderContext& render_context)
{
    Settings& tune = *settings_;

    BITMAP* light;
    BITMAP* cloud;

    if (active)
    {
        if (explo_size > 0)
        {
            light = create_system_bitmap(tune.ROCKET_SIZE * 2, tune.ROCKET_SIZE * 2);

            clear_to_color(light, makecol(255, 0, 255));

            circlefill(
                light, tune.ROCKET_SIZE, tune.ROCKET_SIZE, explo_size, makecol(255, rand() % 255, 0));

            if (tune.TRANSLUCENT_SMOKE)
            {
                set_trans_blender(255, 255, 255, 100);
                draw_trans_sprite(render_context.target,
                                  light,
                                  x - tune.ROCKET_SIZE,
                                  SCREEN_H - (y + tune.ROCKET_SIZE + CHICKEN_HEIGHT));
            }
            else
            {
                draw_sprite(render_context.target,
                            light,
                            x - tune.ROCKET_SIZE,
                            SCREEN_H - (y + tune.ROCKET_SIZE + CHICKEN_HEIGHT));
            }

            destroy_bitmap(light);
        }

        cloud = create_system_bitmap(w, h);

        for (int i = 0; i < tune.SMOKE_PUFFS; ++i)
        {
            if (--puff[i].wait < 0)
            {
                clear_to_color(cloud, makecol(255, 0, 255));

                if (puff[i].life > 0)
                {
                    stretch_sprite(
                        cloud, static_cast<BITMAP*>(render_context.icons_data[1].dat), 0, 0, w, h);

                    if (tune.TRANSLUCENT_SMOKE)
                    {
                        set_trans_blender(255, 255, 255, puff[i].life * 2);
                        draw_trans_sprite(render_context.target,
                                          cloud,
                                          puff[i].x - w / 2,
                                          SCREEN_H - (y + puff[i].y) - h / 2);
                    }
                    else
                    {
                        draw_sprite(render_context.target,
                                    cloud,
                                    puff[i].x - w / 2,
                                    SCREEN_H - (y + puff[i].y) - h / 2);
                    }
                }
            }
        }

        destroy_bitmap(cloud);
    }

    return 0;
}
