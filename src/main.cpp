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
#include <allegro.h>
#include <math.h>

#include "allegro_runtime.h"
#include "app_context.h"
#include "asset_manager.h"
#include "game.h"
#include "game_state.h"
#include "graphics_display.h"
#include "settings.h"

int process_command_line_args(int argc, char* argv[], bool& cli_force_windowed, AppContext& app);
void show_cli_help(const AppContext& app);
void show_cli_version();
void load_datafiles(AssetManager& asset_manager);
void initialize(AppContext& app, int requested_gfx_driver);
int ctoi(const char* t);

volatile int game_time{};

int main(int argc, char* argv[])
{
    Settings settings{};
    GameState state{};
    AssetManager asset_manager{};
    AppContext app{settings, state, asset_manager};

    state.seed_default_playername();
    settings = Settings(state.config_path);
    state.apply_settings(settings);

    bool cli_force_windowed{};
    const int cmdline_result = process_command_line_args(argc, argv, cli_force_windowed, app);
    if (cmdline_result != 0)
    {
        return cmdline_result;
    }

    const GfxLaunchRequest gfx_launch{cli_force_windowed, settings.FULLSCREEN};
    const int requested_gfx_driver = GraphicsDisplay::requested_driver(gfx_launch);
    settings.MAX_CHICKENS = std::min(settings.MAX_CHICKENS, MAX_CHICKENS_CAPACITY);

    initialize(app, requested_gfx_driver);

    AppAssets& a = asset_manager.assets();
    a.buffer = create_system_bitmap(SCREEN_W, SCREEN_H);
    a.background =
        create_bitmap(SCREEN_W, SCREEN_H); // Prevent a segfault if they exit without playing
                                           // anything (ie no background image gets loaded)

    const RenderContext render_context{
        a.buffer, a.gem_data, a.icons_data, a.giblet_data};

    Game game(app, render_context);
    game.run();

    allegro_exit();

    return 0;
}
END_OF_MAIN();

int process_command_line_args(int argc, char* argv[], bool& cli_force_windowed, AppContext& app)
{
    Settings& settings = app.settings;
    GameState& state = app.state;

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
                state.config_path = argv[++i];
                settings = Settings(state.config_path);
                state.apply_settings(settings);
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
            settings = Settings{};
            state.apply_settings(settings);
        }
        else if (!strcmp(arg, "--mute"))
        {
            state.mute_via_cli = true;
            state.mute_sound = true;
        }
        else if (!strcmp(arg, "-u") && argc > i)
        {
            if (i < argc - 1)
            {
                state.playername = argv[++i];
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
                state.current_level = ctoi(argv[++i]);
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
            show_cli_help(app);
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

void show_cli_help(const AppContext& app)
{
    show_cli_version();
    allegro_message("  -u %smaster\tSpecify player name (default is $USER)\n", app.state.playername);
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

void load_datafiles(AssetManager& asset_manager)
{
    if (!asset_manager.load_app_assets())
    {
        allegro_message("Failed to load one or more game assets.\n");
        std::exit(EXIT_FAILURE);
    }

    font = asset_manager.assets().font;
}

static void install_allegro_core(const Settings& settings)
{
    AllegroRuntime::install_core(settings);
}

static void set_gfx_mode_or_exit(int gfx_driver)
{
    if (!GraphicsDisplay::try_set_mode(gfx_driver))
    {
        allegro_message(
            "Unable to set graphics mode %dx%d.\n%s\n",
            GraphicsDisplay::WIDTH,
            GraphicsDisplay::HEIGHT,
            allegro_error);
        std::exit(EXIT_FAILURE);
    }
}

void initialize(AppContext& app, int requested_gfx_driver)
{
    install_allegro_core(app.settings);
    set_gfx_mode_or_exit(requested_gfx_driver);
    load_datafiles(app.asset_manager);

    set_mouse_sprite(static_cast<BITMAP*>(app.assets().cursors_data[0].dat));
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
