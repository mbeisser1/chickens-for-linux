#ifndef GRAPHICS_DISPLAY_H
#define GRAPHICS_DISPLAY_H

// Allegro gfx driver constants (e.g. GFX_AUTODETECT_WINDOWED) come from allegro.h

struct GfxLaunchRequest
{
    bool cli_force_windowed{};
    bool settings_fullscreen{};
};

class GraphicsDisplay
{
  public:
    static constexpr int WIDTH{800};
    static constexpr int HEIGHT{600};

    /** Merge CLI window override with config fullscreen preference. */
    static int requested_driver(const GfxLaunchRequest& launch);

    /**
     * Set 800×600 gfx mode with color-depth fallbacks, optional X11 fallback,
     * and fullscreen → windowed fallback when needed.
     */
    static bool try_set_mode(int requested_driver);

  private:
    static bool try_depths_for_driver(int gfx_driver);
};

#endif
