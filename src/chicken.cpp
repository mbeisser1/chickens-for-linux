#include "chicken.h"

#include "level.h"
#include "settings.h"
#include "asset_manager.h"

AppAssets* Chicken::session_assets_{};
Level* Chicken::session_terrain_{};
Settings* Chicken::session_settings_{};

void Blood::release(const float X, const float Y, const float X_VEL, const float Y_VEL)
{
    x = X + rand() % 10 - rand() % 10;
    y = Y + rand() % 10 - rand() % 10;
    x_vel = X_VEL + float(rand() % 5) / float(1 + rand() % 10) * (rand() % 2 == 1 ? 1 : -1);
    y_vel = Y_VEL + float(rand() % 5) / float(1 + rand() % 10) * (rand() % 2 == 1 ? 1 : -1);
}

void Blood::run(Settings& settings)
{
    y_vel += settings.GRAVITY;
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

void Kfc::explode(Level& terrain, Settings& settings, AppAssets& app_assets)
{
    if (!released)
    {
        return;
    }

    for (int i = 0; i < settings.CHUNKS_PER_CHICKEN; ++i)
    {
        // If the chunk is visible on the screen
        if (chunk[i].x >= 0 && chunk[i].x < SCREEN_W)
        { // Thank you Jarno!

            // If it hit the ground
            if (chunk[i].y >= SCREEN_H - terrain.height[static_cast<int>(chunk[i].x)])
            {
                chunk[i].y = SCREEN_H - terrain.height[static_cast<int>(chunk[i].x)];
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
                            if (terrain.height[v] - terrain.height[v - 1] < 1)
                            {
                                if (terrain.height[v] - terrain.height[v + 1] < 1)
                                {
                                    terrain.height[v]++;
                                }
                            }
                        }
                    }

                    // If the chunk is visible on the screen
                    if (chunk[i].x >= 0 && chunk[i].x < SCREEN_W)
                    {
                        draw_sprite(terrain.image,
                                    static_cast<BITMAP*>(app_assets.giblet_data[chunk[i].image].dat),
                                    static_cast<int>(chunk[i].x),
                                    MAX_LEVELHEIGHT - terrain.height[static_cast<int>(chunk[i].x)]);
                    }
                }
            }
        }
        else
        {
            chunk[i].landed = true;
        }

        chunk[i].y_vel += settings.GRAVITY;

        chunk[i].x += chunk[i].x_vel;
        chunk[i].y += chunk[i].y_vel;

        for (int j = 0; j < settings.BLOOD_PER_CHUNK; ++j)
        {
            chunk[i].blood[j].run(settings);
        }
    }
}

void Kfc::draw(const RenderContext& render_context, Settings& settings)
{
    if (!released)
    {
        return;
    }

    for (int i = 0; i < settings.CHUNKS_PER_CHICKEN; ++i)
    {
        if (chunk[i].landed == false)
        {
            draw_sprite(render_context.target,
                        static_cast<BITMAP*>(render_context.giblet_data[chunk[i].image].dat),
                        static_cast<int>(chunk[i].x),
                        static_cast<int>(chunk[i].y));
        }

        for (int j = 0; j < settings.BLOOD_PER_CHUNK; ++j)
        {
            chunk[i].blood[j].draw(render_context);
        }
    }
}

void Kfc::release(const float at_x,
                  const float at_y,
                  int accuracy,
                  const int death,
                  const int direction,
                  Settings& settings,
                  AppAssets& app_assets)
{
    chunk = std::unique_ptr<Giblet[]>(new Giblet[settings.CHUNKS_PER_CHICKEN]);

    accuracy -=
        CHICKEN_WIDTH / 2; // Base accuracy off the center of chicken, not its actual x position.

    for (int i = 0; i < settings.CHUNKS_PER_CHICKEN; ++i)
    {
        chunk[i].blood = std::unique_ptr<Blood[]>(new Blood[settings.BLOOD_PER_CHUNK]);
        chunk[i].image = rand() % AssetManager::items_in_datafile(app_assets.giblet_data);

        chunk[i].landed = false;
        chunk[i].x = at_x + rand() % CHICKEN_WIDTH;
        chunk[i].y = at_y + rand() % CHICKEN_HEIGHT;

        switch (death)
        {
        case KILLED_WITH_ROCKET:
            chunk[i].x_vel =
                float(rand() % 5) / float(1 + rand() % 10) * (rand() % 2 == 1 ? 1 : -1) +
                (at_x - accuracy) / 3;
            chunk[i].y_vel = -settings.ROCKET_SIZE / 3 - rand() % 6;
            break;
        case KILLED_WITH_SHOTGUN:
            chunk[i].x_vel = settings.CHICKEN_SPEED * direction + rand() % 4 - rand() % 4;
            chunk[i].y_vel = -rand() % 6;
            break;
        case KILLED_WITH_TENDERIZER:
            chunk[i].x_vel = settings.CHICKEN_SPEED * direction;
            chunk[i].y_vel = -rand() % 25 - 5;
            break;
        }

        for (int j = 0; j < settings.BLOOD_PER_CHUNK; ++j)
        {
            chunk[i].blood[j].release(chunk[i].x, chunk[i].y, chunk[i].x_vel, chunk[i].y_vel);
        }
    }

    released = true;
}

Chicken::Chicken(AppAssets& app_assets, Level& terrain, Settings& settings)
{
    session_assets_ = &app_assets;
    session_terrain_ = &terrain;
    session_settings_ = &settings;
    running.load(app_assets.chicken_data);
    flying.load(app_assets.flyingchicken_data);
    flying.slide = true;
    reset();
}

void Chicken::reset()
{
    Level& terrain = *session_terrain_;

    if (rand() % 2 == 1)
    {
        x = SCREEN_W + rand() % 500;
        y = SCREEN_H - terrain.height[SCREEN_W - 1] - CHICKEN_HEIGHT;
        running.hflip = true;
        flying.hflip = true;
        direction = LEFT;
    }
    else
    {
        x = -(rand() % 500) - CHICKEN_WIDTH;
        y = SCREEN_H - terrain.height[0] - CHICKEN_HEIGHT;
        running.hflip = false;
        flying.hflip = false;
        direction = RIGHT;
    }

    dead.chunk.reset();
    dead.released = false;
    alive = NOT_KILLED;
    y_vel = 0;
    flight = 0;
    running.x = x;
    running.y = y;
    flying.x = x;
    flying.y = y;
}

int Chicken::run()
{
    Level& terrain = *session_terrain_;
    Settings& tune = *session_settings_;

    if (alive == NOT_KILLED)
    {
        int ground;

        running.x = x;
        running.y = y;
        flying.x = x;
        flying.y = y;

        x += tune.CHICKEN_SPEED * direction;

        if (x >= 0 && x < SCREEN_W)
        { // Only if the chicken is visible on screen

            ground = SCREEN_H - terrain.height[static_cast<int>(x)] - CHICKEN_HEIGHT;

            if (rand() % tune.CHANCE_OF_FLIGHT <= 1) // Chance of flying
            {
                flight = -1 - (rand() % 300) - rand() % 100;
            }

            if (flight < 0)
            {
                y_vel -= tune.GRAVITY * 3;
                flight++;
            }

            y = ground + y_vel;

            if (y < ground)
            {
                y_vel += tune.GRAVITY;
            }
            else
            {
                y = ground;
            }
        }

        if (x < -20)
        {
            y = SCREEN_H - terrain.height[0] - CHICKEN_HEIGHT;
        }

        if ((direction == LEFT && x < -CHICKEN_WIDTH) || (direction == RIGHT && x > SCREEN_W))
        {
            reset();
            return CROSSED_THE_ROAD;
        }

        return 1;
    }
    else
    {
        if (!dead.released)
        {
            dead.release(x, y, mouse_x, alive, direction, tune, *session_assets_);
        }

        dead.explode(terrain, tune, *session_assets_);

        for (int i = 0; i < tune.CHUNKS_PER_CHICKEN; ++i)
        {
            if (!dead.chunk[i].landed)
            {
                break;
            }

            if (i == tune.CHUNKS_PER_CHICKEN - 1)
            {
                reset();
            }
        }

        return 0;
    }
}

void Chicken::draw(const RenderContext& render_context)
{
    Level& terrain = *session_terrain_;
    Settings& tune = *session_settings_;

    if (x >= -CHICKEN_WIDTH && x < SCREEN_W)
    {
        if (alive == NOT_KILLED)
        {
            if (y < SCREEN_H - terrain.height[static_cast<int>(x)] - CHICKEN_HEIGHT)
            {
                flying.play(render_context.target);
            }
            else
            {
                running.play(render_context.target);
            }
        }
        else
        {
            dead.draw(render_context, tune);
        }
    }
}
