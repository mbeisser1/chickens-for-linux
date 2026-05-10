#include "graphics_display.h"

#include <allegro.h>

int GraphicsDisplay::preferred_driver(bool cli_force_windowed, bool settings_fullscreen)
{
    return (cli_force_windowed || !settings_fullscreen) ? GFX_AUTODETECT_WINDOWED
                                                         : GFX_AUTODETECT_FULLSCREEN;
}

bool GraphicsDisplay::try_depths_for_driver(int gfx_driver)
{
    static const int depths[]{24, 32, 16};

    for (int depth : depths)
    {
        set_color_depth(depth);
        if (set_gfx_mode(gfx_driver, WIDTH, HEIGHT, 0, 0) == 0)
        {
            return true;
        }
    }
    return false;
}

bool GraphicsDisplay::try_set_mode(int requested_driver)
{
    if (try_depths_for_driver(requested_driver))
    {
        return true;
    }

#ifdef GFX_XWINDOWS
    if (requested_driver != GFX_XWINDOWS)
    {
        if (try_depths_for_driver(GFX_XWINDOWS))
        {
            return true;
        }
    }
#endif

    if (requested_driver == GFX_AUTODETECT_FULLSCREEN)
    {
        allegro_message("Fullscreen mode failed; trying windowed.\n%s\n", allegro_error);
        if (try_depths_for_driver(GFX_AUTODETECT_WINDOWED))
        {
            return true;
        }
#ifdef GFX_XWINDOWS
        if (try_depths_for_driver(GFX_XWINDOWS))
        {
            return true;
        }
#endif
    }

    return false;
}
