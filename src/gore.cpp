#include "gore.h"
#include "asset_manager.h"

void Blood::release(const float X, const float Y, const float X_VEL, const float Y_VEL)
{
    x = X + rand() % 10 - rand() % 10;
    y = Y + rand() % 10 - rand() % 10;
    x_vel = X_VEL + float(rand() % 5) / float(1 + rand() % 10) * (rand() % 2 == 1 ? 1 : -1);
    y_vel = Y_VEL + float(rand() % 5) / float(1 + rand() % 10) * (rand() % 2 == 1 ? 1 : -1);
}

void Blood::run()
{
    y_vel += game_settings.GRAVITY;
    x += x_vel;
    y += y_vel;
}

void Blood::draw(const RenderContext& render_context)
{
    if (x >= 0 && x < SCREEN_W)
    {
        if (y >= 0 && y < SCREEN_H)
        {
            putpixel(render_context.target, static_cast<int>(x), static_cast<int>(y), makecol(220, 0, 0));
        }
    }
}

Giblet::Giblet()
{
    image = rand() % items_in_datafile(asset_manager.app_assets().giblet_data);
}

void Kfc::explode()
{
    if(!released)
    {
        return;
    }

    for (int i = 0; i < game_settings.CHUNKS_PER_CHICKEN; ++i)
    {
        // If the chunk is visible on the screen
        if (chunk[i].x >= 0 && chunk[i].x < SCREEN_W)
        { // Thank you Jarno!

            // If it hit the ground
            if (chunk[i].y >= SCREEN_H - level.height[static_cast<int>(chunk[i].x)])
            {
                chunk[i].y = SCREEN_H - level.height[static_cast<int>(chunk[i].x)];
                chunk[i].x_vel = int(chunk[i].x_vel * 0.3);
                chunk[i].y_vel = -int(chunk[i].y_vel * 0.3);

                // If the chunk has landed for good
                if (!chunk[i].landed && chunk[i].y_vel == 0 && chunk[i].x_vel == 0)
                {
                    chunk[i].landed = true;

                    for (int j = 0; j < 10; ++j)
                    {
                        int v = static_cast<int>(chunk[i].x) + j;

                        if (v < SCREEN_W - 1 && v > 0)
                        {
                            if (level.height[v] - level.height[v - 1] < 1)
                            {
                                if (level.height[v] - level.height[v + 1] < 1)
                                {
                                    level.height[v]++;
                                }
                            }
                        }
                    }

                    // If the chunk is visible on the screen
                    if (chunk[i].x >= 0 && chunk[i].x < SCREEN_W)
                    {
                        draw_sprite(level.image,
                                    static_cast<BITMAP*>(
                                        asset_manager.app_assets().giblet_data[chunk[i].image].dat),
                                    static_cast<int>(chunk[i].x),
                                    MAX_LEVELHEIGHT - level.height[static_cast<int>(chunk[i].x)]);
                    }
                }
            }
        }
        else
        {
            chunk[i].landed = true;
        }

        chunk[i].y_vel += game_settings.GRAVITY;

        chunk[i].x += chunk[i].x_vel;
        chunk[i].y += chunk[i].y_vel;

        for (int j = 0; j < game_settings.BLOOD_PER_CHUNK; ++j)
        {
            chunk[i].blood[j].run();
        }
    }
}


void Kfc::draw(const RenderContext& render_context)
{
    if(!released)
    {
        return;
    }

    for (int i = 0; i < game_settings.CHUNKS_PER_CHICKEN; ++i)
    {
        if (chunk[i].landed == false)
        {
            draw_sprite(render_context.target,
                        static_cast<BITMAP*>(render_context.giblet_data[chunk[i].image].dat),
                        static_cast<int>(chunk[i].x),
                        static_cast<int>(chunk[i].y));
        }

        for (int j = 0; j < game_settings.BLOOD_PER_CHUNK; ++j)
        {
            chunk[i].blood[j].draw(render_context);
        }
    }
}

void Kfc::release(const float at_x, const float at_y, int accuracy, const int death, const int direction)
{
    chunk = std::unique_ptr<Giblet[]>(new Giblet[game_settings.CHUNKS_PER_CHICKEN]);

    accuracy -=
        CHICKEN_WIDTH / 2; // Base accuracy off the center of chicken, not its actual x position.

    for (int i = 0; i < game_settings.CHUNKS_PER_CHICKEN; ++i)
    {
        chunk[i].blood = std::unique_ptr<Blood[]>(new Blood[game_settings.BLOOD_PER_CHUNK]);

        chunk[i].landed = false;
        chunk[i].x = at_x + rand() % CHICKEN_WIDTH;
        chunk[i].y = at_y + rand() % CHICKEN_HEIGHT;

        switch (death)
        {
        case KILLED_WITH_ROCKET:
            chunk[i].x_vel =
                float(rand() % 5) / float(1 + rand() % 10) * (rand() % 2 == 1 ? 1 : -1) +
                (at_x - accuracy) / 3;
            chunk[i].y_vel = -game_settings.ROCKET_SIZE / 3 - rand() % 6;
            break;
        case KILLED_WITH_SHOTGUN:
            chunk[i].x_vel = game_settings.CHICKEN_SPEED * direction + rand() % 4 - rand() % 4;
            chunk[i].y_vel = -rand() % 6;
            break;
        case KILLED_WITH_TENDERIZER:
            chunk[i].x_vel = game_settings.CHICKEN_SPEED * direction;
            chunk[i].y_vel = -rand() % 25 - 5;
            break;
        }

        for (int j = 0; j < game_settings.BLOOD_PER_CHUNK; ++j)
        {
            chunk[i].blood[j].release(chunk[i].x, chunk[i].y, chunk[i].x_vel, chunk[i].y_vel);
        }
    }

    released = true;
}
