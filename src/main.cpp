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
#include <iostream>

#include <allegro.h>
#include <math.h>

#include "animation.h"
#include "chicken.h"
#include "gem.h"
#include "helper.h"
#include "highscore.h"
#include "level.h"
#include "settings.h"
#include "smoke.h"

int mode_manager();
void load_datafiles();
void load_fonts();
void load_sounds();
void initialize(int);
void show_startup();
void show_modechooser();
void show_levelcompleted();
void restart(CHICKEN[], GEM[MAX_GEMS], SMOKE[MAX_SMOKE]);
void earn_bonus(int);
void show_levelnumber();
void fadeout(int, int);
void next_level(int);
int ctoi(char*);
inline void show_statistics();
inline void weapon_manager(bool*, bool*);
inline void play_sound(SAMPLE*, int, int, bool);

// Configurable Variables
bool FULLSCREEN;
bool MUTE;
bool TRANSLUCENT_SMOKE;
float GRAVITY;
int BLOOD_PER_CHUNK;
int CHANCE_OF_FLIGHT;
int CHANCE_OF_GEM;
float CHICKEN_SPEED;
int CHUNKS_PER_CHICKEN;
int GAME_SPEED_OFFSET;
int INITIAL_CHICKENS;
int MAX_CHICKENS;
int POINTS_FOR_ROCKET;
int POINTS_FOR_SHOTGUN;
int POINTS_FOR_TENDERIZER;
int RESPAWN_RATE;
int ROCKET_RELOAD;
int ROCKET_SIZE;
int SHOTGUN_RELOAD;
int SHOTGUN_SIZE;
int SMOKE_LINGERING;
int SMOKE_PUFFS;
int TIMER;
int VOLUME;
// End Configurable Variables

volatile int game_time;

int chickens_left;     // Number of chickens left to kill, for levelmode only
int current_level;     // Current level, levelmode only
int delay_of_levelend; // Set a delay so the level doesn't end until you actually see the final
                       // chicken die
int mode;              // Current game mode
int kills;             // Number of chickens killed
int runners;           // Number of chickens currently in game
int score;             // Current game score
int shots_fired;       // Number of rockets/shotguns fired
int timer;             // Game time
int timer_delay;       // Keeps track of each second
int tenderizers;       // Number of tenderizers available (you can really only have 1)
int tmp_rocket_size;   // Used for remembering the default rocket size
bool alert_mode;       // Alert mode!
bool level_mode;       // Whether level mode is on
bool mute_sound;       // Shall we play sounds or not?
bool not_dead;         // If the player isn't dead yet
const char* config_path; // Path to configuration file
char* playername;

BITMAP* background;
BITMAP* buffer;
DATAFILE* background_data;
DATAFILE* bigchicken_data;
DATAFILE* cursors_data;
DATAFILE* chicken_data;
DATAFILE* flyingchicken_data;
DATAFILE* terrain_data;
DATAFILE* gem_data;
DATAFILE* giblet_data;
DATAFILE* icons_data;
DATAFILE* fonts_data;
DATAFILE* modechooser_data;
LEVEL level;
FONT* font_big;
FONT* font_interface;
SAMPLE* sound_alarm;
SAMPLE* sound_gameover;
SAMPLE* sound_gemcollect;
SAMPLE* sound_highscore;
SAMPLE* sound_menu;
SAMPLE* sound_rocket;
SAMPLE* sound_shotgun;
SAMPLE* sound_tenderizer;

int main(int argc, char* argv[])
{
    int rank; // Player rank
    int mx;   // Store previous mouse location
    int my;
    bool alert_sound;  // When true, the alert sound is playing
    bool fire_rocket;  // When true, a rocket is being fired
    bool fire_shotgun; // When true, the shotgun is being fired

    mode = MODE_PLAYING;
    alert_mode = false;
    alert_sound = false;
    level_mode = false;
    playername = getenv("USER");
    rank = HIGHSCORE_TABLE + 1;
    config_path = CHICKENS_ASSETS_REL("options.cfg");
    current_level = 1;
    timer_delay = 0;

    load_stock_config();      // Load game defaults, in case of broken config file
    load_config(config_path); // Read in some game variables

    if (MUTE)
    {
        mute_sound = true;
    }
    else
    {
        mute_sound = false;
    }

    bool cli_force_windowed = false;
    int windowmode = GFX_AUTODETECT_WINDOWED;

    // Process those pesky command line parameters

    for (int i = 1; i < argc; ++i)
    {
        if (!strcmp(argv[i], "--window"))
        {
            cli_force_windowed = true;
        }

        else if (!strcmp(argv[i], "-s"))
        {
            if (i < argc - 1)
            {
                config_path = argv[++i];
                load_config(config_path); // Load the new config file
            }
            else
            {
                allegro_message("Error - Terrible Syntax. The -s parameter requires a path to a "
                                "config file.\n");
                return 4;
            }
        }

        else if (!strcmp(argv[i], "--stock"))
        {
            load_stock_config();
        }

        else if (!strcmp(argv[i], "--mute"))
        {
            mute_sound = true;
        }

        else if (!strcmp(argv[i], "-u") && argc > i)
        {
            if (i < argc - 1)
            {
                playername = argv[++i];
            }
            else
            {
                allegro_message(
                    "Error - Terrible Syntax. The -u parameter requires a player name.\n");
                return 4;
            }
        }

        else if (!strcmp(argv[i], "--warp") && argc > i)
        {
            if (i < argc - 1)
            {
                current_level = ctoi(argv[++i]);
            }
            else
            {
                allegro_message(
                    "Error - Terrible Syntax. The --warp parameter requires a level number.\n");
                return 4;
            }
        }

        else if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h"))
        {
            allegro_message("Chickens for Linux! 0.2.4\n");
            allegro_message("  -u %smaster\tSpecify player name (default is $USER)\n", playername);
            allegro_message("  -s file.cfg\t\tSpecify config file\n");
            allegro_message("  --warp x\t\tWarp to level 'x'\n");
            allegro_message("  --window\t\tRun in windowed mode\n");
            allegro_message("  --mute\t\tDon't play any sound or music\n");
            allegro_message("  --stock\t\tUse stock settings\n");
            allegro_message("  --help | -h\t\tDisplay this informative help screen\n");
            allegro_message("  --version\t\tShow version number\n");
            return 1;
        }

        else if (!strcmp(argv[i], "--version"))
        {
            allegro_message("Chickens for Linux! 0.2.4\n");
            return 2;
        }

        else
        {
            allegro_message("Error - Unknown Parameter '%s'\n", argv[i]);
            return 3;
        }
    }

    if (cli_force_windowed)
    {
        windowmode = GFX_AUTODETECT_WINDOWED;
    }
    else if (FULLSCREEN)
    {
        windowmode = GFX_AUTODETECT_FULLSCREEN;
    }
    else
    {
        windowmode = GFX_AUTODETECT_WINDOWED;
    }

    srand(time(NULL));

    initialize(windowmode);

    buffer = create_system_bitmap(SCREEN_W, SCREEN_H);
    background =
        create_bitmap(SCREEN_W, SCREEN_H); // Prevent a segfault if they exit without playing
                                           // anything (ie no background image gets loaded)
    tmp_rocket_size = ROCKET_SIZE;

    SMOKE smoke[MAX_SMOKE]; // Allegro needs to be running before we can initialize these!
    GEM gem[MAX_GEMS];
    CHICKEN chicken[MAX_CHICKENS];

    show_startup();
    fadeout(makecol(0, 0, 0), 50);
    show_modechooser();

    do
    {
        while (game_time > 0)
        {
            mode_manager(); // Check for user input and change game mode as needed
            mx = mouse_x;
            my = mouse_y;

            // Logic
            switch (mode)
            {
            case MODE_RESTART:
                restart(chicken, gem, smoke);
                mode = MODE_PLAYING;
                if (level_mode == true)
                {
                    next_level(current_level);
                }

                break;

            case MODE_PLAYING:

                if (chickens_left < 0)
                {
                    chickens_left = 0;
                }

                for (int i = 0; i < MAX_SMOKE; ++i)
                {
                    smoke[i].run();
                }

                for (int i = 0; i < MAX_GEMS; ++i)
                {
                    if (gem[i].run())
                    {
                        earn_bonus(gem[i].type);
                    }
                }

                if (timer_delay-- == 0)
                {
                    timer_delay = 60;
                    timer--;
                }

                if (timer <= 10)
                { // When the timer starts getting low, enter Alert Mode
                    alert_mode = true;
                    if (!alert_sound)
                    { // Protect against playing the sound repeatedly with each cycle
                        play_sound(sound_alarm, VOLUME, 128, FOREVER);
                        alert_sound = true;
                    }
                }
                else if (alert_mode)
                {
                    stop_sample(sound_alarm);
                    alert_sound = false;
                    alert_mode = false;
                }

                weapon_manager(&fire_rocket, &fire_shotgun); // Determine if a weapon is being fired

                for (int i = 0; i < runners; ++i)
                {
                    if (chicken[i].run() == CROSSED_THE_ROAD)
                    {
                        mode = MODE_GAMEOVER;
                    }

                    if (fire_rocket)
                    {
                        if (chicken[i].alive == NOT_KILLED &&
                            ((SCREEN_H - chicken[i].y) - level.height[mouse_x]) < ROCKET_SIZE &&
                            (abs(mouse_x - (int)chicken[i].x) < ROCKET_SIZE ||
                             abs(mouse_x - (int)chicken[i].x - CHICKEN_WIDTH) < ROCKET_SIZE))
                        {
                            score += POINTS_FOR_ROCKET;
                            chicken[i].alive = KILLED_WITH_ROCKET;
                            ++kills;
                            --chickens_left;

                            if (runners < MAX_CHICKENS - 1 && rand() % RESPAWN_RATE <= 1)
                            {
                                runners++;
                            }

                            if (rand() % CHANCE_OF_GEM <= 1)
                            {
                                for (int j = 0; j < MAX_GEMS; ++j)
                                {
                                    if (gem[j].active == false)
                                    {
                                        gem[j].launch(chicken[i].x, chicken[i].y);
                                        break;
                                    }
                                }
                            }
                        }
                    }

                    if (fire_shotgun)
                    {
                        if (chicken[i].alive == NOT_KILLED &&
                            (abs(mouse_x - (int)chicken[i].x) < SHOTGUN_SIZE ||
                             abs(mouse_x - (int)chicken[i].x - CHICKEN_WIDTH) < SHOTGUN_SIZE) &&
                            (abs(mouse_y - (int)chicken[i].y) < SHOTGUN_SIZE ||
                             abs(mouse_y - (int)chicken[i].y - CHICKEN_HEIGHT) < SHOTGUN_SIZE))
                        { // Look! it's the worlds longest line of code.

                            score += POINTS_FOR_SHOTGUN;
                            chicken[i].alive = KILLED_WITH_SHOTGUN;
                            chicken[i].flight = chicken[i].direction;
                            ++kills;
                            --chickens_left;

                            if (runners < MAX_CHICKENS - 1 && rand() % RESPAWN_RATE <= 1)
                            {
                                runners++;
                            }

                            if (rand() % CHANCE_OF_GEM <= 1)
                            {
                                for (int j = 0; j < MAX_GEMS; ++j)
                                {
                                    if (gem[j].active == false)
                                    {
                                        gem[j].launch(chicken[i].x, chicken[i].y);
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }

                // Send out a new explosion if they fired a rocket

                if (fire_rocket)
                {
                    for (int i = 0; i < MAX_SMOKE; ++i)
                    {
                        if (smoke[i].active == false)
                        {
                            smoke[i].release();
                            break;
                        }
                    }

                    ++shots_fired;
                }

                if (fire_shotgun)
                {
                    ++shots_fired;
                }

                if (level_mode == false)
                {
                    // Don't allow the Tenderizer in Level Mode

                    // Emergency chicken tenderizer (kill all chickens instantly)
                    if (tenderizers > 0)
                    {
                        if (key[KEY_SPACE])
                        {
                            for (int i = 0; i < MAX_CHICKENS; ++i)
                            {
                                if (chicken[i].alive == NOT_KILLED)
                                {
                                    chicken[i].alive = KILLED_WITH_TENDERIZER;
                                    chicken[i].flight = chicken[i].direction;
                                    score += POINTS_FOR_TENDERIZER;
                                    kills++;
                                }
                            }

                            play_sound(sound_tenderizer, VOLUME, 128, ONCE);
                            tenderizers--;
                        }
                    }
                }

                if (timer <= 0)
                {
                    mode = MODE_GAMEOVER;
                }

                if (key[KEY_R])
                {
                    mode = MODE_RESTART;
                    fadeout(makecol(0, 0, 0), 20);
                }

                if (level_mode == true)
                {
                    if (chickens_left < 1)
                    {
                        if (delay_of_levelend-- == 0)
                        {
                            mode = MODE_NEXTLEVEL;
                        }
                    }
                }

                // Protect against getting a Gameover in between the time you killed the last
                // remaining chicken and the rollover delay time.
                if (mode == MODE_GAMEOVER)
                {
                    if (level_mode == true)
                    {
                        if (chickens_left < 1)
                        {
                            mode = MODE_PLAYING;
                        }
                    }
                }

                break;

            case MODE_NEXTLEVEL:
                fadeout(makecol(0, 0, 0), 30);
                show_levelcompleted();
                restart(chicken, gem, smoke);
                next_level(current_level + 1);
                show_levelnumber();
                mode = MODE_PLAYING;
                break;

            case MODE_PAUSED:
                if (key[KEY_C])
                {
                    mode = MODE_PLAYING;
                }

                if (key[KEY_Q])
                {
                    fadeout(makecol(0, 0, 0), 30);
                    show_modechooser();
                }

                break;

            case MODE_GAMEOVER:

                if (level_mode == false)
                {
                    if (not_dead)
                    {
                        rank = save_highscore(playername, score);
                        stop_sample(sound_alarm);
                        not_dead = false;

                        if (rank <= HIGHSCORE_TABLE)
                        {
                            play_sound(sound_highscore, VOLUME, 128, FOREVER);
                        }
                        else
                        {
                            play_sound(sound_gameover, VOLUME, 128, ONCE);
                        }
                    }
                }

                if (key[KEY_R])
                {
                    mode = MODE_RESTART;
                    fadeout(makecol(0, 0, 0), 30);
                }

                if (key[KEY_ESC] || key[KEY_SPACE] || key[KEY_ENTER])
                {
                    fadeout(makecol(0, 0, 0), 30);
                    show_modechooser();
                }

                break;
            }

            game_time--;
        }

        // DRAW EVERYTHING

        clear(buffer);
        draw_sprite(buffer, background, 0, 0);

        switch (mode)
        {
        case MODE_PLAYING:

            for (int i = 0; i < MAX_SMOKE; ++i)
            {
                smoke[i].draw();
            }

            draw_sprite(buffer, level.image, 0, SCREEN_H - MAX_LEVELHEIGHT);

            for (int i = 0; i < MAX_GEMS; ++i)
            {
                gem[i].draw();
            }

            for (int i = 0; i < runners; ++i)
            {
                chicken[i].draw();
            }

            show_statistics();

            break;

        case MODE_PAUSED:

            for (int i = 0; i < SCREEN_H; i += 2)
            {
                line(buffer, 0, i, SCREEN_W, i, makecol(0, 0, 60));
            }

            CHICKENS_TEXTOUT_CENTRE(buffer,
                           font_big,
                           "PAUSED",
                           SCREEN_W / 2,
                           SCREEN_H / 2 - 40,
                           makecol(255, 255, 255));
            CHICKENS_TEXTOUT_CENTRE(buffer,
                           font,
                           "Press 'C' to continue or 'Q' to quit",
                           SCREEN_W / 2,
                           SCREEN_H / 2 + 60,
                           makecol(255, 255, 255));

            break;

        case MODE_GAMEOVER:

            rectfill(buffer, 0, SCREEN_H - 40, SCREEN_W, SCREEN_H, makecol(0, 0, 0));

            CHICKENS_TEXTOUT_CENTRE(buffer,
                           font_big,
                           "Armageddon",
                           SCREEN_W / 2,
                           SCREEN_H / 2 - 20,
                           makecol(255, 255, 255));
            CHICKENS_TEXTOUT_CENTRE(buffer,
                           font,
                           "The chickens have risen. Everyone is dead. Our world is gone.",
                           SCREEN_W / 2,
                           SCREEN_H / 2 + 40,
                           makecol(255, 255, 255));
            CHICKENS_TEXTOUT_CENTRE(buffer,
                           font,
                           "Press 'R' to play again!",
                           SCREEN_W / 2,
                           SCREEN_H - 35,
                           makecol(0, 255, 0));
            CHICKENS_TEXTOUT_CENTRE(buffer,
                           font,
                           "Not that this is a game. This is actually happening, in real life.",
                           SCREEN_W / 2,
                           SCREEN_H - 20,
                           makecol(200, 200, 200));

            if (rank <= HIGHSCORE_TABLE)
            {
                CHICKENS_TEXTPRINTF_CENTRE(buffer,
                                  font_interface,
                                  SCREEN_W / 2,
                                  SCREEN_H / 2 + 65,
                                  makecol(0, 220, 0),
                                  "But you did get a High Score of %d points in rank %d!",
                                  score,
                                  rank);
            }

            break;
        }

        draw_sprite(buffer, mouse_sprite, mx - 11, my - 11);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        while (game_time <= 0)
        {
        }

    } while (mode != MODE_QUIT);

    show_highscores(rank - 1, background);

    // Let's free up some memory

    unload_datafile(background_data);
    unload_datafile(bigchicken_data);
    unload_datafile(chicken_data);
    unload_datafile(cursors_data);
    unload_datafile(fonts_data);
    unload_datafile(flyingchicken_data);
    unload_datafile(gem_data);
    unload_datafile(giblet_data);
    unload_datafile(icons_data);
    unload_datafile(modechooser_data);
    unload_datafile(terrain_data);

    destroy_sample(sound_alarm);
    destroy_sample(sound_gameover);
    destroy_sample(sound_gemcollect);
    destroy_sample(sound_rocket);
    destroy_sample(sound_shotgun);

    destroy_bitmap(buffer);

    allegro_exit();

    return 0;
}
END_OF_MAIN();

int mode_manager()
{
    if (mode == MODE_PLAYING)
    {
        set_mouse_sprite((BITMAP*)cursors_data[1].dat);

        if (key[KEY_ESC] || key[KEY_PAUSE])
        {
            mode = MODE_PAUSED;
        }
    }
    else
    {
        set_mouse_sprite((BITMAP*)cursors_data[0].dat);
    }

    return mode;
}

void load_datafiles()
{
    background_data = load_datafile(CHICKENS_ASSETS_REL("dat/background.dat"));
    bigchicken_data = load_datafile(CHICKENS_ASSETS_REL("dat/bigchicken.dat"));
    chicken_data = load_datafile(CHICKENS_ASSETS_REL("dat/chicken.dat"));
    cursors_data = load_datafile(CHICKENS_ASSETS_REL("dat/cursors.dat"));
    flyingchicken_data = load_datafile(CHICKENS_ASSETS_REL("dat/flyingchicken.dat"));
    fonts_data = load_datafile(CHICKENS_ASSETS_REL("dat/fonts.dat"));
    gem_data = load_datafile(CHICKENS_ASSETS_REL("dat/gem.dat"));
    giblet_data = load_datafile(CHICKENS_ASSETS_REL("dat/giblets.dat"));
    icons_data = load_datafile(CHICKENS_ASSETS_REL("dat/icons.dat"));
    modechooser_data = load_datafile(CHICKENS_ASSETS_REL("dat/modechooser.dat"));
    terrain_data = load_datafile(CHICKENS_ASSETS_REL("dat/terrain.dat"));
}

void load_fonts()
{
    font = (FONT*)fonts_data[0].dat;
    font_big = (FONT*)fonts_data[1].dat;
    font_interface = (FONT*)fonts_data[2].dat;
}

void load_sounds()
{
    sound_alarm = load_sample(CHICKENS_ASSETS_REL("sound/alarm.wav"));
    sound_gameover = load_sample(CHICKENS_ASSETS_REL("sound/gameover.wav"));
    sound_gemcollect = load_sample(CHICKENS_ASSETS_REL("sound/gemcollect.wav"));
    sound_highscore = load_sample(CHICKENS_ASSETS_REL("sound/highscore.wav"));
    sound_menu = load_sample(CHICKENS_ASSETS_REL("sound/menu.wav"));
    sound_rocket = load_sample(CHICKENS_ASSETS_REL("sound/rocket.wav"));
    sound_shotgun = load_sample(CHICKENS_ASSETS_REL("sound/shotgun.wav"));
    sound_tenderizer = load_sample(CHICKENS_ASSETS_REL("sound/tenderizer.wav"));
}

static bool try_depths_for_driver(int gfx_driver)
{
    static const int depths[] = {24, 32, 16};
    int d;

    for (d = 0; d < 3; ++d)
    {
        set_color_depth(depths[d]);
        if (set_gfx_mode(gfx_driver, 800, 600, 0, 0) == 0)
        {
            return true;
        }
    }
    return false;
}

static bool try_set_gfx_mode(int requested_mode)
{
    if (try_depths_for_driver(requested_mode))
    {
        return true;
    }

#ifdef GFX_XWINDOWS
    if (requested_mode != GFX_XWINDOWS)
    {
        if (try_depths_for_driver(GFX_XWINDOWS))
        {
            return true;
        }
    }
#endif

    if (requested_mode == GFX_AUTODETECT_FULLSCREEN)
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

void initialize(int windowmode)
{
    allegro_init();
    install_mouse();
    install_keyboard();
    install_sound(DIGI_AUTODETECT, MIDI_NONE, NULL);
    install_timer();

    LOCK_VARIABLE(CONTROLLING_FRAMERATE);
    LOCK_FUNCTION(timer_framerate);
    LOCK_VARIABLE(game_time);
    LOCK_FUNCTION(Timer);

    install_int_ex(Timer, BPS_TO_TIMER(60 + GAME_SPEED_OFFSET));

    if (!try_set_gfx_mode(windowmode))
    {
        allegro_message("Unable to set graphics mode 800x600.\n%s\n", allegro_error);
        std::exit(EXIT_FAILURE);
    }

    load_datafiles();
    load_fonts();
    load_sounds();

    set_mouse_sprite((BITMAP*)cursors_data[0].dat);
}

void show_startup()
{
    int mx;
    int my;

    do
    {
        while (game_time > 0)
        {
            mx = mouse_x;
            my = mouse_y;
            game_time--;
        }

        clear(buffer);

        draw_sprite(buffer, (BITMAP*)icons_data[2].dat, SCREEN_W / 2 - 177, 4);
        draw_sprite(buffer, (BITMAP*)gem_data[1].dat, SCREEN_W / 2 - 143, SCREEN_H / 2 + 100);
        draw_sprite(buffer, (BITMAP*)gem_data[0].dat, SCREEN_W / 2 - 3, SCREEN_H / 2 + 100);
        draw_sprite(buffer, (BITMAP*)gem_data[2].dat, SCREEN_W / 2 + 147, SCREEN_H / 2 + 100);

        line(buffer, 0, 104, SCREEN_W, 104, makecol(100, 0, 0));
        line(buffer, 0, SCREEN_H - 30, SCREEN_W, SCREEN_H - 30, makecol(100, 0, 0));

        CHICKENS_TEXTOUT_CENTRE(buffer,
                       font,
                       "In the final days of Armageddon, the chickens are preparing",
                       SCREEN_W / 2,
                       130,
                       makecol(200, 200, 200));
        CHICKENS_TEXTOUT_CENTRE(buffer,
                       font,
                       "to detonate our planet. They are trying to run across your",
                       SCREEN_W / 2,
                       145,
                       makecol(200, 200, 200));
        CHICKENS_TEXTOUT_CENTRE(buffer,
                       font,
                       "screen, and if even a single chicken succeeds, it's all over.",
                       SCREEN_W / 2,
                       160,
                       makecol(200, 200, 200));
        CHICKENS_TEXTOUT_CENTRE(buffer,
                       font,
                       "Please look in README for info about LEVEL MODE--this screen is temporary!",
                       SCREEN_W / 2,
                       180,
                       makecol(220, 0, 0));
        CHICKENS_TEXTOUT_CENTRE(buffer, font, "CLASSIC MODE:", SCREEN_W / 2, 200, makecol(220, 0, 0));
        CHICKENS_TEXTOUT_CENTRE(buffer,
                       font,
                       "You are on a 2 minute timer. Each time that you shoot, you",
                       SCREEN_W / 2,
                       215,
                       makecol(255, 255, 255));
        CHICKENS_TEXTOUT_CENTRE(buffer,
                       font,
                       "lose an additional second, so try to kill as many chickens",
                       SCREEN_W / 2,
                       230,
                       makecol(255, 255, 255));
        CHICKENS_TEXTOUT_CENTRE(buffer,
                       font,
                       "per click as possible. Killing chickens with rockets gives",
                       SCREEN_W / 2,
                       245,
                       makecol(255, 255, 255));
        CHICKENS_TEXTOUT_CENTRE(buffer,
                       font,
                       "you 100 points. A shotgun kill earns you 250, but it takes",
                       SCREEN_W / 2,
                       260,
                       makecol(255, 255, 255));
        CHICKENS_TEXTOUT_CENTRE(buffer,
                       font,
                       "longer to reload. Chickens high up in flight must be shot",
                       SCREEN_W / 2,
                       275,
                       makecol(255, 255, 255));
        CHICKENS_TEXTOUT_CENTRE(buffer,
                       font,
                       "with the shotgun, as rockets can only shoot at ground level.",
                       SCREEN_W / 2,
                       290,
                       makecol(255, 255, 255));
        CHICKENS_TEXTOUT_CENTRE(buffer,
                       font,
                       "You are given 1 tenderizer. In case of emergency hit SPACE.",
                       SCREEN_W / 2,
                       305,
                       makecol(255, 70, 70));
        CHICKENS_TEXTOUT_CENTRE(buffer,
                       font,
                       "Occasionally, cool gems fly out of exploding chickens. Catch",
                       SCREEN_W / 2,
                       320,
                       makecol(255, 255, 255));
        CHICKENS_TEXTOUT_CENTRE(buffer,
                       font,
                       "them with your mouse cursor (don't click!) to earn bonuses.",
                       SCREEN_W / 2,
                       335,
                       makecol(255, 255, 255));
        CHICKENS_TEXTOUT_CENTRE(buffer,
                       font,
                       "All gems automatically give you 2 extra seconds.",
                       SCREEN_W / 2,
                       350,
                       makecol(255, 255, 255));

        CHICKENS_TEXTOUT_CENTRE(buffer,
                       font,
                       "+5 radius",
                       SCREEN_W / 2 - 140,
                       SCREEN_H / 2 + 120,
                       makecol(255, 255, 255));
        CHICKENS_TEXTOUT_CENTRE(
            buffer, font, "+8 sec", SCREEN_W / 2, SCREEN_H / 2 + 120, makecol(255, 255, 255));
        CHICKENS_TEXTOUT_CENTRE(buffer,
                       font,
                       "+5000 pts",
                       SCREEN_W / 2 + 152,
                       SCREEN_H / 2 + 120,
                       makecol(255, 255, 255));

        CHICKENS_TEXTOUT_CENTRE(buffer,
                       font,
                       "Rocket: Left-click",
                       SCREEN_W / 2 - 100,
                       SCREEN_H - 50,
                       makecol(255, 30, 30));
        CHICKENS_TEXTOUT_CENTRE(buffer,
                       font,
                       "Shotgun: Right-click",
                       SCREEN_W / 2 + 100,
                       SCREEN_H - 50,
                       makecol(255, 30, 30));

        CHICKENS_TEXTOUT_RIGHT(buffer,
                      font,
                      "moistrous software 2004 (0.2.4)",
                      SCREEN_W - 10,
                      SCREEN_H - 25,
                      makecol(255, 255, 255));

        draw_sprite(buffer, mouse_sprite, mx, my);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        while (game_time <= 0)
        {
        }

    } while (!keypressed() && mouse_b != 1);
}

void show_modechooser()
{
    ANIMATION bigchick;

    bool done = false;
    bool right_foot = false;
    int counter = 0;
    int mx;
    int my;
    int bigchick_bloodyfeet;
    int bigchick_tspeed;
    float bigchick_xspeed;
    float bigchick_yspeed;

    bigchick.load(bigchicken_data);
    bigchick.x = 100;
    bigchick.y = 100;
    bigchick.angle = 0;

    stop_sample(sound_alarm);
    stop_sample(sound_highscore);
    stop_sample(sound_menu);
    play_sound(sound_menu, VOLUME, 128, FOREVER);

    do
    {
        counter++;
        while (game_time > 0)
        {
            bigchick_xspeed = (bigchick.x - mouse_x + (mouse_x > bigchick.x ? 0 : 0)) / 35;
            bigchick_yspeed = (bigchick.y - mouse_y + (mouse_y > bigchick.y ? 0 : 0)) / 35;

            // make that big chicken chase the mouse
            bigchick.angle =
                192 + atan((bigchick.y - mouse_y) / (bigchick.x - mouse_x)) * (128 / 3.14159);
            bigchick.angle += mouse_x > bigchick.x ? 128 : 0;

            if (bigchick.x > 80 && bigchick.x < 240)
            {
                if (bigchick.y > 60 && bigchick.y < 180)
                {
                    bigchick_bloodyfeet = 10;
                }
            }

            if (mouse_b & 1 && abs((int)bigchick_xspeed) + abs((int)bigchick_yspeed) < 3)
            {
                // Entering Classic Mode
                if (mouse_x < 90)
                {
                    if (mouse_y > SCREEN_H - 160 && mouse_y < SCREEN_H - 90)
                    {
                        mode = MODE_RESTART;
                        level_mode = false;
                        load_config(config_path); // Load original config settings (easier than a
                                                  // having a ton of variables to remember them)
                        done = true;
                    }
                }

                // Surrendering (quit game)
                if (mouse_x > 430 && mouse_x < 530)
                {
                    if (mouse_y > SCREEN_H - 100)
                    {
                        mode = MODE_QUIT;
                        done = true;
                    }
                }

                // Entering Level Mode
                if (mouse_x > SCREEN_W - 90)
                {
                    if (mouse_y > 280 && mouse_y < 380)
                    {
                        mode = MODE_RESTART;
                        level_mode = true;
                        next_level(current_level);
                        done = true;
                    }
                }
            }

            mx = mouse_x;
            my = mouse_y;
            game_time--;
        }

        clear(buffer);

        bigchick.x -= bigchick_xspeed;
        bigchick.y -= bigchick_yspeed;

        // Give the chicken a stupid waddle
        bigchick_tspeed = abs((int)bigchick_xspeed) + abs((int)bigchick_yspeed);
        bigchick.angle += (rand() % (1 + bigchick_tspeed) - rand() % (1 + bigchick_tspeed)) / 2;

        if (bigchick_bloodyfeet > 0 && counter % 7 == 0)
        {
            BITMAP* track = create_bitmap(30, 30);
            clear_to_color(track, makecol(255, 0, 255));

            rotate_sprite(track,
                          (BITMAP*)modechooser_data[1].dat,
                          0,
                          0,
                          itofix((int)bigchick.angle + rand() % 6 - rand() % 6));
            set_trans_blender(255, 255, 255, bigchick_bloodyfeet * 10);
            draw_trans_sprite((BITMAP*)modechooser_data[0].dat,
                              track,
                              (int)bigchick.x + 15 - rand() % 20,
                              (int)bigchick.y + 30 - rand() % 20);
            bigchick_bloodyfeet--;

            destroy_bitmap(track);

            right_foot = !right_foot;
        }

        draw_sprite(buffer, (BITMAP*)modechooser_data[0].dat, 0, 0);
        bigchick.play(buffer);

        draw_sprite(buffer, mouse_sprite, mx, my);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        while (game_time <= 0)
        {
        }

    } while (!done);

    fadeout(makecol(0, 0, 0), 40);

    if (mode == MODE_RESTART && level_mode == true)
    {
        show_levelnumber();
    }
}

void show_levelcompleted()
{
    SAMPLE* sound_count = load_sample(CHICKENS_ASSETS_REL("sound/counter.wav"));

    int b = 0;
    int mx;
    int my;
    float a = 0;
    float accuracy = (((float)kills / (float)shots_fired) * 70) + ((timer / 60) * 30);
    float bonus = 0;

    if (accuracy > 100)
    {
        bonus = accuracy - 100;
        accuracy = 100;
        b = 255;
    }

    do
    {
        while (game_time > 0)
        {
            mx = mouse_x;
            my = mouse_y;
            game_time--;

            if (a < accuracy && game_time % 300 == 0)
            {
                a++;
                play_sound(sound_count, int(float(a / accuracy) * VOLUME), 128, ONCE);
            }

        }

        clear(buffer);

        CHICKENS_TEXTPRINTF_CENTRE(buffer,
                                    font_big,
                                    SCREEN_W / 2,
                                    SCREEN_H / 2,
                                    makecol(255, 255, 255),
                                    "Level %d Completed!",
                                    current_level);
        {
            /* a>100: green text here; old inner loop only set unused `color`. */
            const int acc_fg = (a > 100.0f) ? makecol(0, 255, 0) : makecol(255, 255, 255);
            CHICKENS_TEXTPRINTF_CENTRE(buffer,
                                        font_interface,
                                        SCREEN_W / 2,
                                        SCREEN_H / 2 + 90,
                                        acc_fg,
                                        "Accuracy: %d%%",
                                        (int)a);
        }
        if (a == 100 && b > 0)
        {
            CHICKENS_TEXTPRINTF_CENTRE(buffer,
                              font_interface,
                              SCREEN_W / 2 + 60,
                              SCREEN_H / 2 + 90 - (255 - b),
                              makecol(0, b, 0),
                              "+%d",
                              (int)bonus);
            b -= 2;
        }

        draw_sprite(buffer, mouse_sprite, mx, my);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        while (game_time <= 0)
        {
        }

    } while (mouse_b != 1);

    destroy_sample(sound_count);

    fadeout(makecol(0, 0, 0), 40);
}

void restart(CHICKEN chicken[], GEM gem[MAX_GEMS], SMOKE smoke[MAX_SMOKE])
{
    for (int i = 0; i < MAX_CHICKENS; ++i)
    {
        chicken[i].reset();
    }

    for (int i = 0; i < MAX_GEMS; ++i)
    {
        gem[i].active = false;
    }

    for (int i = 0; i < MAX_SMOKE; ++i)
    {
        smoke[i].active = false;
    }

    background = (BITMAP*)background_data[rand() % items_in_datafile(background_data)].dat;
    level.create();

    ROCKET_SIZE = tmp_rocket_size;
    tenderizers = 1;

    if (level_mode == false)
    {
        timer = TIMER;
    }

    delay_of_levelend = 40;
    timer_delay = 0;
    score = 0;
    shots_fired = 0;
    kills = 0;
    runners = INITIAL_CHICKENS;
    alert_mode = false;
    not_dead = true;

    stop_sample(sound_gameover); // They might not always be playing but stop them anyway
    stop_sample(sound_highscore);
    stop_sample(sound_tenderizer);
    stop_sample(sound_menu);
}

void earn_bonus(int type)
{
    play_sound(sound_gemcollect, VOLUME, int(mouse_x / 3.13), ONCE);

    switch (type)
    {
    case 0: // Blue gem
        if (level_mode == false)
        {
            timer += 6;
        }
        else
        {
            timer++;
        }
        break;

    case 1: // Green gem
        if (level_mode == false)
        {
            timer += 2;
        }
        ROCKET_SIZE += 5;
        break;

    case 2: // Red gem
        if (level_mode == false)
        {
            score += 5000;
            timer += 2;
        }
        else
        {
            timer += 3;
        }
        break;

    default:
        break;
    }
}

void show_levelnumber()
{
    int mx;
    int my;

    do
    {
        while (game_time > 0)
        {
            mx = mouse_x;
            my = mouse_y;
            game_time--;
        }

        clear(buffer);

        CHICKENS_TEXTPRINTF_CENTRE(buffer,
                          font_big,
                          SCREEN_W / 2,
                          SCREEN_H / 2,
                          makecol(255, 255, 255),
                          "Level %d",
                          current_level);

        draw_sprite(buffer, mouse_sprite, mx, my);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        while (game_time <= 0)
        {
        }

    } while (mouse_b != 1);

    fadeout(makecol(0, 0, 0), 30);
}

void fadeout(int color, int duration)
{
    int fader = 255;
    int fader_step = duration % fader;

    BITMAP* fading = create_bitmap(SCREEN_W, SCREEN_H);
    clear_to_color(fading, color);

    do
    {
        while (game_time > 0)
        {
            game_time--;
            duration--;
            fader -= fader_step;
        }

        set_trans_blender(255, 255, 255, 60);
        draw_trans_sprite(buffer, fading, 0, 0);

        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        while (game_time <= 0)
        {
        }

    } while (duration > 0);
}

void next_level(int level)
{
    stop_sample(sound_alarm);
    stop_sample(sound_highscore);
    stop_sample(sound_gameover);

    current_level = level;
    chickens_left = INITIAL_CHICKENS + (level * 4);
    CHICKEN_SPEED = (int)sqrt(chickens_left) / 2;
    timer = 60;
}

int ctoi(char* t)
{
    // Convert char* into integers, for processing the damn commandline parameters.

    int v = 0;
    int l = strlen(t);
    int n = t[0] == '-' ? 1 : 0;

    for (int i = l - 1; i >= 0 + n; --i)
    {
        v += (t[l - i - 1 + n] - 48) * (int)pow(10, i - n);
    }

    return v = n ? -v : v;
}

inline void show_statistics()
{
    const char* format;
    int minutes = timer / 60;
    int seconds = timer - (minutes * 60);
    if (seconds < 10)
    {
        format = "%d:0%d";
    }
    else
    {
        format = "%d:%d";
    }

    if (level_mode == true)
    {
        // Level Mode:
        CHICKENS_TEXTPRINTF(
            buffer, font_interface, 20, 5, makecol(255, 255, 255), "Level: %d", current_level);
        CHICKENS_TEXTPRINTF(buffer,
                   font_interface,
                   20,
                   25,
                   makecol(255, 255, 255),
                   "Chickens: %d    ",
                   chickens_left);
        CHICKENS_TEXTPRINTF(
            buffer, font_big, SCREEN_W - 90, 0, makecol(255, 255, 255), format, minutes, seconds);
    }
    else
    {
        // Classic Mode:
        CHICKENS_TEXTPRINTF(buffer, font_interface, 20, 5, makecol(255, 255, 255), "Score: %d    ", score);
        CHICKENS_TEXTPRINTF(buffer, font_interface, 20, 25, makecol(255, 255, 255), "Kills: %d    ", kills);
        CHICKENS_TEXTPRINTF(
            buffer, font_big, SCREEN_W - 90, 0, makecol(255, 255, 255), format, minutes, seconds);

        if (alert_mode)
        {
            set_trans_blender(255, 255, 255, 150);
            draw_trans_sprite(
                buffer, (BITMAP*)icons_data[0].dat, SCREEN_W / 2 - 50, SCREEN_H / 2 - 50);
            CHICKENS_TEXTOUT_CENTRE(
                buffer, font_big, "ALERT", SCREEN_W / 2, SCREEN_H / 2 - 30, makecol(255, 255, 255));
        }
    }
}

inline void weapon_manager(bool* fire_rocket, bool* fire_shotgun)
{
    static int reloading_rocket = 0;
    static int reloading_shotgun = 0;

    *fire_rocket = mouse_b & 1;
    *fire_shotgun = mouse_b & 2;

    if (reloading_rocket)
    {
        --reloading_rocket;
        *fire_rocket = false;
    }

    if (reloading_shotgun)
    {
        --reloading_shotgun;
        *fire_shotgun = false;
    }

    if (*fire_rocket)
    {
        reloading_rocket = ROCKET_RELOAD; // reload time
        play_sound(sound_rocket, VOLUME, int(mouse_x / 3.13), ONCE);

        --timer;
    }

    if (*fire_shotgun)
    {
        reloading_shotgun = SHOTGUN_RELOAD; // reload time
        play_sound(sound_shotgun,
                   VOLUME,
                   int(mouse_x / 3.13),
                   ONCE); // Pan speaker output to mouse location

        --timer;
    }

    return;
}

inline void play_sound(SAMPLE* snd, int volume, int pan, bool loop)
{
    if (mute_sound == false)
    {
        play_sample(snd, volume, pan, 1000, loop);
    }
}
