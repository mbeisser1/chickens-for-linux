#ifndef CHICKEN_H
#define CHICKEN_H

#include <allegro.h>

#include "animation.h"
#include "gore.h"

struct Level;
struct Settings;
struct AppAssets;

class Chicken
{
  public:
    Chicken();
    /** Call after Allegro loads datafiles (`chicken_data`, `flyingchicken_data`). */
    void load_sprites(AppAssets& app_assets);
    void bind_level(Level* terrain);
    void bind_settings(Settings* settings);

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
    Settings* settings_{};
    AppAssets* app_assets_{};
};

#endif
