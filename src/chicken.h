#ifndef CHICKEN_H
#define CHICKEN_H

#include <allegro.h>

#include "animation.h"
#include "gore.h"

struct Level;

class Chicken
{
  public:
    Chicken();
    void bind_level(Level* terrain);
    void draw(const RenderContext& render_context);
    void reset();
    int run();

    Kfc dead{};
    Animation running{};
    Animation flying{};

    float x{};
    float y{};
    float y_vel{};
    int direction{};
    int flight{};
    int alive{};

  private:
    Level* terrain_{};
};

#endif
