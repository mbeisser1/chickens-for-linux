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
    /** Loads sprite datafiles and binds level/settings; calls `reset()` for initial placement. */
    Chicken(AppAssets& app_assets, Level& terrain, Settings& settings);

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
    /** One `AppAssets` bundle per run; set from each `Chicken` construction. */
    static AppAssets* session_assets_;
};

#endif
