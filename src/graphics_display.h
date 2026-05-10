#ifndef GRAPHICS_DISPLAY_H
#define GRAPHICS_DISPLAY_H

struct Settings;

// Allegro gfx driver constants (e.g. GFX_AUTODETECT_WINDOWED) come from allegro.h

class GraphicsDisplay
{
  public:
    static constexpr int WIDTH{800};
    static constexpr int HEIGHT{600};

    /**
     * Set 800×600 gfx mode from settings (`FORCE_WINDOWED`, `FULLSCREEN`),
     * with color-depth fallbacks, optional X11 fallback, and fullscreen → windowed
     * fallback when needed.
     */
    static bool try_set_mode(const Settings& settings);

    /**
     * Set 800×600 gfx mode with color-depth fallbacks, optional X11 fallback,
     * and fullscreen → windowed fallback when needed.
     */
    static bool try_set_mode(int requested_driver);

  private:
    static bool try_depths_for_driver(int gfx_driver);
};

#endif
