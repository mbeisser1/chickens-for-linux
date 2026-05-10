#include "chicken.h"

Chicken::Chicken()
{
    running.load(chicken_data);
    flying.load(flyingchicken_data);
    flying.slide = true;
    reset();
}

void Chicken::reset()
{
    if (rand() % 2 == 1)
    {
        x = SCREEN_W + rand() % 500;
        y = SCREEN_H - level.height[SCREEN_W - 1] - CHICKEN_HEIGHT;
        running.hflip = true;
        flying.hflip = true;
        direction = LEFT;
    }
    else
    {
        x = -(rand() % 500) - CHICKEN_WIDTH;
        y = SCREEN_H - level.height[0] - CHICKEN_HEIGHT;
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
    if (alive == NOT_KILLED)
    {
        int ground;

        running.x = x;
        running.y = y;
        flying.x = x;
        flying.y = y;

        x += game_settings.CHICKEN_SPEED * direction;

        if (x >= 0 && x < SCREEN_W)
        { // Only if the chicken is visible on screen

            ground = SCREEN_H - level.height[(int)x] - CHICKEN_HEIGHT;

            if (rand() % game_settings.CHANCE_OF_FLIGHT <= 1) // Chance of flying
            {
                flight = -1 - (rand() % 300) - rand() % 100;
            }

            if (flight < 0)
            {
                y_vel -= game_settings.GRAVITY * 3;
                flight++;
            }

            y = ground + y_vel;

            if (y < ground)
            {
                y_vel += game_settings.GRAVITY;
            }
            else
            {
                y = ground;
            }
        }

        if (x < -20)
        {
            y = SCREEN_H - level.height[0] - CHICKEN_HEIGHT;
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
            dead.release(x, y, mouse_x, alive, direction);
        }

        dead.explode();

        for (int i = 0; i < game_settings.CHUNKS_PER_CHICKEN; ++i)
        {
            if (!dead.chunk[i].landed)
            {
                break;
            }

            if (i == game_settings.CHUNKS_PER_CHICKEN - 1)
            {
                reset();
            }
        }

        return 0;
    }
}

void Chicken::draw(const RenderContext& render_context)
{
    if (x >= -CHICKEN_WIDTH && x < SCREEN_W)
    {
        if (alive == NOT_KILLED)
        {
            if (y < SCREEN_H - level.height[(int)x] - CHICKEN_HEIGHT)
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
            dead.draw(render_context);
        }
    }
}
