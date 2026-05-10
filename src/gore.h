#ifndef GORE_H
#define GORE_H

#include <memory>

#include <allegro.h>

#include "helper.h"

struct Level;
struct Settings;
struct AppAssets;

struct Blood
{
    void release(const float x, const float y, const float x_vel, const float y_vel);
    void run(Settings& settings);
    void draw(const RenderContext& render_context);

  private:
    float x{};
    float y{};
    float x_vel{};
    float y_vel{};
};

struct Giblet
{
    std::unique_ptr<Blood[]> blood{};
    float x{};
    float y{};
    float x_vel{};
    float y_vel{};
    int image{};
    bool landed{};
};

struct Kfc
{
    Kfc() = default;
    std::unique_ptr<Giblet[]> chunk{};

    void explode(Level& terrain, Settings& settings, AppAssets& app_assets);
    void release(const float at_x,
                   const float at_y,
                   int accuracy,
                   const int death,
                   const int direction,
                   Settings& settings,
                   AppAssets& app_assets);
    void draw(const RenderContext& render_context, Settings& settings);

    bool released{};
};

#endif
