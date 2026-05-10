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

#include "app_context.h"
#include "asset_manager.h"
#include "game.h"
#include "game_state.h"
#include "settings.h"

int process_command_line_args(int argc, char* argv[], AppContext& app);
void show_cli_help(const AppContext& app);
void show_cli_version();

volatile int game_time{};

int main(int argc, char* argv[])
{
    Settings settings = Settings(Settings::kDefaultConfigPath);
    GameState state{settings};
    AssetManager asset_manager{};
    AppContext app{settings, state, asset_manager};

    const int cmdline_result = process_command_line_args(argc, argv, app);
    if (cmdline_result != 0)
    {
        return cmdline_result;
    }

    Game game(app);
    game.run();
    allegro_exit();

    return 0;
}
END_OF_MAIN();

int process_command_line_args(int argc, char* argv[], AppContext& app)
{
    Settings& settings = app.settings;
    GameState& state = app.state;

    for (int i = 1; i < argc; ++i)
    {
        const char* arg = argv[i];

        if (!strcmp(arg, "--window"))
        {
            settings.FORCE_WINDOWED = true;
        }
        else if (!strcmp(arg, "-s"))
        {
            if (i < argc - 1)
            {
                settings.config_path = argv[++i];
                settings = Settings(settings.config_path);
                state.apply_settings(settings);
            }
            else
            {
                allegro_message("Error - Terrible Syntax. The -s parameter requires a path to a config file.\n");
                return -1;
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
                return -1;
            }
        }
        else if (!strcmp(arg, "--warp") && argc > i)
        {
            if (i < argc - 1)
            {
                state.current_level = std::atoi(argv[++i]);
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
            return -1;
        }
        else if (!strcmp(arg, "--version"))
        {
            show_cli_version();
            return -1;
        }
        else
        {
            allegro_message("Error - Unknown Parameter '%s'\n", arg);
            return -1;
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
