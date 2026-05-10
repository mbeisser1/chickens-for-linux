#include "level.h"
#include "asset_manager.h"

void Level::create()
{
    int divider;

    image = create_bitmap(SCREEN_W, MAX_LEVELHEIGHT + 50); // the +50 adds room for gore to pile up
    terrain = static_cast<BITMAP*>(asset_manager.assets().terrain_data[0].dat);
    clear_to_color(image, makecol(255, 0, 255)); // Clear it to transparent pink
    height[0] = 20 + rand() % (MAX_LEVELHEIGHT - 70);
    draw_sprite(image, terrain, 0, MAX_LEVELHEIGHT - height[0]);

    for (int i = 1; i < SCREEN_W; ++i)
    {
        do
        {
            divider = 1 + rand() % 8;
            height[i] = height[i - 1] + rand() % 2 - rand() % 2;
            height[i] = height[i] + int(sin(rand() % SCREEN_W) / divider);

        } while (height[i] > MAX_LEVELHEIGHT || height[i] < 10);

        draw_sprite(image, terrain, i, MAX_LEVELHEIGHT - height[i]);
    }
}
