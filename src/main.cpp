/*
    Version 0.2.4
    -----------------------------------------------------------------------------

    Welcome to the Chickens for Linux! source code.

    For optimal viewing pleasure, adjust your indentation tab width to 8.

    This is the first-ever completed game I've written. Although I do my best
    at writing decent code, I'm not trying to set any sort of example. So as
    you're picking apart my code like an evil chicken is your flesh, please tell
    yourself: There's probably a better way.

    But I didn't see it.

    - blake
    (missing@chickensforlinux.com)
    -----------------------------------------------------------------------------
*/

#include <cstdlib>
#include <algorithm>
#include <array>
#include <iostream>

#include <allegro.h>
#include <math.h>

#include "allegro_runtime.h"
#include "asset_manager.h"
#include "game.h"
#include "game_state.h"
#include "graphics_display.h"
#include "highscore.h"
#include "level.h"
#include "settings.h"

int process_command_line_args(int argc, char* argv[], bool& cli_force_windowed);
void show_cli_help();
void show_cli_version();
void load_datafiles();
void initialize(int requested_gfx_driver);
int ctoi(const char* t);

volatile int game_time{};

struct AppContext
{
    Settings& game_settings;
    AssetManager& asset_manager;
    Level& level;
    GameState& game_state;
};

Settings game_settings{};
AssetManager asset_manager{};
Level level{};
GameState game_state{};
AppContext app_context{game_settings, asset_manager, level, game_state};
AppAssets& assets{asset_manager.assets()};

int main(int argc, char* argv[])
{
    game_settings = Settings(game_state.config_path);
    game_state.apply_settings(game_settings);

    bool cli_force_windowed{};
    const int cmdline_result = process_command_line_args(argc, argv, cli_force_windowed);
    if (cmdline_result != 0)
    {
        return cmdline_result;
    }

    const int requested_gfx_driver =
        GraphicsDisplay::preferred_driver(cli_force_windowed, game_settings.FULLSCREEN);
    game_settings.MAX_CHICKENS = std::min(game_settings.MAX_CHICKENS, MAX_CHICKENS_CAPACITY);
    srand(time(nullptr));

    initialize(requested_gfx_driver);

    assets.buffer = create_system_bitmap(SCREEN_W, SCREEN_H);
    assets.background =
        create_bitmap(SCREEN_W, SCREEN_H); // Prevent a segfault if they exit without playing
                                           // anything (ie no background image gets loaded)

    //////

    std::array<Smoke, MAX_SMOKE> smoke;
    std::array<Gem, MAX_GEMS> gem;
    std::array<Chicken, MAX_CHICKENS_CAPACITY> chicken;

    const RenderContext render_context{
        assets.buffer, assets.gem_data, assets.icons_data, assets.giblet_data};

    Game game(smoke, gem, chicken, render_context);
    const int rank = game.run();

    show_highscores(rank - 1, assets.buffer, assets.background);

    allegro_exit();

    return 0;
}
END_OF_MAIN();

int process_command_line_args(int argc, char* argv[], bool& cli_force_windowed)
{
    // Process those pesky command line parameters
    for (int i = 1; i < argc; ++i)
    {
        const char* arg = argv[i];

        if (!strcmp(arg, "--window"))
        {
            cli_force_windowed = true;
        }
        else if (!strcmp(arg, "-s"))
        {
            if (i < argc - 1)
            {
                game_state.config_path = argv[++i];
                game_settings = Settings(game_state.config_path);
                game_state.apply_settings(game_settings);
            }
            else
            {
                allegro_message("Error - Terrible Syntax. The -s parameter requires a path to a "
                                "config file.\n");
                return 4;
            }
        }
        else if (!strcmp(arg, "--stock"))
        {
            game_settings = Settings{};
            game_state.apply_settings(game_settings);
        }
        else if (!strcmp(arg, "--mute"))
        {
            game_state.mute_sound = true;
        }
        else if (!strcmp(arg, "-u") && argc > i)
        {
            if (i < argc - 1)
            {
                game_state.playername = argv[++i];
            }
            else
            {
                allegro_message(
                    "Error - Terrible Syntax. The -u parameter requires a player name.\n");
                return 4;
            }
        }
        else if (!strcmp(arg, "--warp") && argc > i)
        {
            if (i < argc - 1)
            {
                game_state.current_level = ctoi(argv[++i]);
            }
            else
            {
                allegro_message(
                    "Error - Terrible Syntax. The --warp parameter requires a level number.\n");
                return 4;
            }
        }
        else if (!strcmp(arg, "--help") || !strcmp(arg, "-h"))
        {
            show_cli_help();
            return 1;
        }
        else if (!strcmp(arg, "--version"))
        {
            show_cli_version();
            return 2;
        }
        else
        {
            allegro_message("Error - Unknown Parameter '%s'\n", arg);
            return 3;
        }
    }

    return 0;
}

void show_cli_help()
{
    show_cli_version();
    allegro_message("  -u %smaster\tSpecify player name (default is $USER)\n", game_state.playername);
    allegro_message("  -s file.cfg\t\tSpecify config file\n");
    allegro_message("  --warp x\t\tWarp to level 'x'\n");
    allegro_message("  --window\t\tRun in windowed mode\n");
    allegro_message("  --mute\t\tDon't play any sound or music\n");
    allegro_message("  --stock\t\tUse stock settings\n");
    allegro_message("  --help | -h\t\tDisplay this informative help screen\n");
    allegro_message("  --version\t\tShow version number\n");
}

void show_cli_version()
{
    allegro_message("Chickens for Linux! 0.2.4\n");
}

void load_datafiles()
{
    if (!asset_manager.load_app_assets())
    {
        allegro_message("Failed to load one or more game assets.\n");
        std::exit(EXIT_FAILURE);
    }

    font = assets.font;
}

void initialize(int requested_gfx_driver)
{
    AllegroRuntime::install_core(game_settings);

    if (!GraphicsDisplay::try_set_mode(requested_gfx_driver))
    {
        allegro_message(
            "Unable to set graphics mode %dx%d.\n%s\n",
            GraphicsDisplay::WIDTH,
            GraphicsDisplay::HEIGHT,
            allegro_error);
        std::exit(EXIT_FAILURE);
    }

    load_datafiles();

    set_mouse_sprite(static_cast<BITMAP*>(assets.cursors_data[0].dat));
}

int ctoi(const char* t)
{
    // Convert char* into integers, for processing the damn commandline parameters.

    int v{};
    int l{static_cast<int>(strlen(t))};
    int n{(t[0] == '-') ? 1 : 0};

    for (int i = l - 1; i >= 0 + n; --i)
    {
        v += (t[l - i - 1 + n] - 48) * static_cast<int>(pow(10, i - n));
    }

    return v = n ? -v : v;
}
