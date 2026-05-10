#include "game.h"

#include <cstdlib>
#include <ctime>
#include <stdexcept>

#include <allegro.h>

#include <cmath>

#include "animation.h"
#include "graphics_display.h"
#include "highscore.h"

extern volatile int game_time;

namespace
{
/**
 * Allegro core setup: subsystems, timer IRQ, and registering state the timer ISR uses.
 * Runs before graphics mode and asset load; pair the process with `allegro_exit()`.
 */
void init_allegro(const Settings& settings)
{
    allegro_init();

    install_mouse();
    install_keyboard();
    install_sound(DIGI_AUTODETECT, MIDI_NONE, nullptr);
    install_timer();

    /* Allegro’s timer-ISR locking hooks. On Linux they expand to nothing (see
     * `allegro/internal/alconfig.h` defaults); on DOS they pinned code/data for IRQ safety.
     * Kept before `install_int_ex` as the usual portable recipe. */
    LOCK_VARIABLE(game_time);
    LOCK_FUNCTION(Timer);

    install_int_ex(Timer, BPS_TO_TIMER(60 + settings.GAME_SPEED_OFFSET));
}

void load_datafiles(AssetManager& asset_manager)
{
    if (!asset_manager.load_app_assets())
    {
        const auto msg = std::string{"Failed to load one or more game assets.\n"};
        allegro_message("%s:%s", msg.c_str(), allegro_error);
        throw std::runtime_error(msg);
    }

    // allegro default font pointer
    font = asset_manager.assets().font;
}

void set_gfx_mode(const Settings& settings)
{
    if (!GraphicsDisplay::try_set_mode(settings))
    {
        char buf[256]= {0};
        snprintf(buf, sizeof(buf), "Unable to set graphics mode %dx%d.\n%s\n",
            GraphicsDisplay::WIDTH,
            GraphicsDisplay::HEIGHT,
            allegro_error);
        allegro_message("%s", buf);
        throw std::runtime_error(buf);
    }
}
} // namespace

/* Session flow / UI (merged from former game_flow.cpp). */
int mode_manager(AppContext& ctx);
void fadeout(AppContext& ctx, int color, int duration);
void earn_bonus(AppContext& ctx, int type);
void next_level(AppContext& ctx, int level_number);
void show_statistics(AppContext& ctx);
void weapon_manager(AppContext& ctx, bool* fire_rocket, bool* fire_shotgun);
void play_sound(AppContext& ctx, const SAMPLE* snd, int volume, int pan, bool loop);

int mode_manager(AppContext& ctx)
{
    if (ctx.state.mode == MODE_PLAYING)
    {
        set_mouse_sprite(static_cast<BITMAP*>(ctx.assets().cursors_data[1].dat));

        if (key[KEY_ESC] || key[KEY_PAUSE])
        {
            ctx.state.mode = MODE_PAUSED;
        }
    }
    else
    {
        set_mouse_sprite(static_cast<BITMAP*>(ctx.assets().cursors_data[0].dat));
    }

    return ctx.state.mode;
}


void fadeout(AppContext& ctx, int color, int duration)
{
    int fader{255};
    int fader_step{duration % fader};

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
        draw_trans_sprite(ctx.assets().buffer, fading, 0, 0);

        blit(ctx.assets().buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        while (game_time <= 0)
        {
        }

    } while (duration > 0);
}

void next_level(AppContext& ctx, int level_number)
{
    stop_sample(ctx.assets().sound_alarm);
    stop_sample(ctx.assets().sound_highscore);
    stop_sample(ctx.assets().sound_gameover);

    ctx.state.current_level = level_number;
    ctx.state.chickens_left = ctx.settings.INITIAL_CHICKENS + (level_number * 4);
    ctx.settings.CHICKEN_SPEED = static_cast<int>(sqrt(static_cast<double>(ctx.state.chickens_left))) / 2;
    ctx.state.timer = 60;
}

void show_statistics(AppContext& ctx)
{
    int minutes{ctx.state.timer / 60};
    int seconds{ctx.state.timer - (minutes * 60)};
    const char* format{(seconds < 10) ? "%d:0%d" : "%d:%d"};

    if (ctx.state.level_mode == true)
    {
        // Level Mode:
        CHICKENS_TEXTPRINTF(
            ctx.assets().buffer, ctx.assets().font_interface, 20, 5, makecol(255, 255, 255), "Level: %d", ctx.state.current_level);
        CHICKENS_TEXTPRINTF(ctx.assets().buffer,
                            ctx.assets().font_interface,
                            20,
                            25,
                            makecol(255, 255, 255),
                            "Chickens: %d    ",
                            ctx.state.chickens_left);
        CHICKENS_TEXTPRINTF(
            ctx.assets().buffer, ctx.assets().font_big, SCREEN_W - 90, 0, makecol(255, 255, 255), format, minutes, seconds);
    }
    else
    {
        // Classic Mode:
        CHICKENS_TEXTPRINTF(
            ctx.assets().buffer, ctx.assets().font_interface, 20, 5, makecol(255, 255, 255), "Score: %d    ", ctx.state.score);
        CHICKENS_TEXTPRINTF(
            ctx.assets().buffer, ctx.assets().font_interface, 20, 25, makecol(255, 255, 255), "Kills: %d    ", ctx.state.kills);
        CHICKENS_TEXTPRINTF(
            ctx.assets().buffer, ctx.assets().font_big, SCREEN_W - 90, 0, makecol(255, 255, 255), format, minutes, seconds);

        if (ctx.state.alert_mode)
        {
            set_trans_blender(255, 255, 255, 150);
            draw_trans_sprite(
                ctx.assets().buffer, static_cast<BITMAP*>(ctx.assets().icons_data[0].dat), SCREEN_W / 2 - 50, SCREEN_H / 2 - 50);
            CHICKENS_TEXTOUT_CENTRE(
                ctx.assets().buffer, ctx.assets().font_big, "ALERT", SCREEN_W / 2, SCREEN_H / 2 - 30, makecol(255, 255, 255));
        }
    }
}

void weapon_manager(AppContext& ctx, bool* fire_rocket, bool* fire_shotgun)
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
        reloading_rocket = ctx.settings.ROCKET_RELOAD; // reload time
        play_sound(ctx, ctx.assets().sound_rocket, ctx.settings.VOLUME, int(mouse_x / 3.13), ONCE);

        --ctx.state.timer;
    }

    if (*fire_shotgun)
    {
        reloading_shotgun = ctx.settings.SHOTGUN_RELOAD; // reload time
        play_sound(ctx, ctx.assets().sound_shotgun,
                   ctx.settings.VOLUME,
                   int(mouse_x / 3.13),
                   ONCE); // Pan speaker output to mouse location

        --ctx.state.timer;
    }

    return;
}

void play_sound(AppContext& ctx, const SAMPLE* snd, int volume, int pan, bool loop)
{
    if (ctx.state.mute_sound == false)
    {
        play_sample(snd, volume, pan, 1000, loop);
    }
}

Game::Game(AppContext& app) : ctx_(app)
{
    init_allegro(ctx_.settings);
    set_gfx_mode(ctx_.settings);
    load_datafiles(ctx_.asset_manager);

    AppAssets& loaded = ctx_.assets();
    ctx_.render_context = RenderContext{
        loaded.buffer, loaded.gem_data, loaded.icons_data, loaded.giblet_data};

    set_mouse_sprite(static_cast<BITMAP*>(ctx_.assets().cursors_data[0].dat));

    smoke_.clear();
    smoke_.reserve(MAX_SMOKE);
    for (int i = 0; i < MAX_SMOKE; ++i)
    {
        smoke_.emplace_back(terrain_, ctx_.settings);
    }
    gem_.clear();
    gem_.reserve(MAX_GEMS);
    for (int i = 0; i < MAX_GEMS; ++i)
    {
        gem_.emplace_back(ctx_.settings);
    }
    chickens_.clear();
    chickens_.reserve(MAX_CHICKENS_CAPACITY);
    for (int i = 0; i < MAX_CHICKENS_CAPACITY; ++i)
    {
        chickens_.emplace_back(ctx_.assets(), terrain_, ctx_.settings);
    }
}

void Game::show_startup()
{
    int mx{};
    int my{};

    do
    {
        while (game_time > 0)
        {
            mx = mouse_x;
            my = mouse_y;
            game_time--;
        }

        clear(ctx_.assets().buffer);

        draw_sprite(
            ctx_.assets().buffer, static_cast<BITMAP*>(ctx_.assets().icons_data[2].dat), SCREEN_W / 2 - 177, 4);
        draw_sprite(
            ctx_.assets().buffer,
            static_cast<BITMAP*>(ctx_.assets().gem_data[1].dat),
            SCREEN_W / 2 - 143,
            SCREEN_H / 2 + 100);
        draw_sprite(ctx_.assets().buffer,
                    static_cast<BITMAP*>(ctx_.assets().gem_data[0].dat),
                    SCREEN_W / 2 - 3,
                    SCREEN_H / 2 + 100);
        draw_sprite(
            ctx_.assets().buffer,
            static_cast<BITMAP*>(ctx_.assets().gem_data[2].dat),
            SCREEN_W / 2 + 147,
            SCREEN_H / 2 + 100);

        line(ctx_.assets().buffer, 0, 104, SCREEN_W, 104, makecol(100, 0, 0));
        line(ctx_.assets().buffer, 0, SCREEN_H - 30, SCREEN_W, SCREEN_H - 30, makecol(100, 0, 0));

        CHICKENS_TEXTOUT_CENTRE(
            ctx_.assets().buffer,
            font,
            "In the final days of Armageddon, the chickens are preparing",
            SCREEN_W / 2,
            130,
            makecol(200, 200, 200));
        CHICKENS_TEXTOUT_CENTRE(
            ctx_.assets().buffer,
            font,
            "to detonate our planet. They are trying to run across your",
            SCREEN_W / 2,
            145,
            makecol(200, 200, 200));
        CHICKENS_TEXTOUT_CENTRE(
            ctx_.assets().buffer,
            font,
            "screen, and if even a single chicken succeeds, it's all over.",
            SCREEN_W / 2,
            160,
            makecol(200, 200, 200));
        CHICKENS_TEXTOUT_CENTRE(
            ctx_.assets().buffer,
            font,
            "Please look in README for info about LEVEL MODE--this screen is temporary!",
            SCREEN_W / 2,
            180,
            makecol(220, 0, 0));
        CHICKENS_TEXTOUT_CENTRE(
            ctx_.assets().buffer, font, "CLASSIC MODE:", SCREEN_W / 2, 200, makecol(220, 0, 0));
        CHICKENS_TEXTOUT_CENTRE(ctx_.assets().buffer,
                                font,
                                "You are on a 2 minute timer. Each time that you shoot, you",
                                SCREEN_W / 2,
                                215,
                                makecol(255, 255, 255));
        CHICKENS_TEXTOUT_CENTRE(ctx_.assets().buffer,
                                font,
                                "lose an additional second, so try to kill as many chickens",
                                SCREEN_W / 2,
                                230,
                                makecol(255, 255, 255));
        CHICKENS_TEXTOUT_CENTRE(
            ctx_.assets().buffer,
            font,
            "per click as possible. Killing chickens with rockets gives",
            SCREEN_W / 2,
            245,
            makecol(255, 255, 255));
        CHICKENS_TEXTOUT_CENTRE(
            ctx_.assets().buffer,
            font,
            "you 100 points. A shotgun kill earns you 250, but it takes",
            SCREEN_W / 2,
            260,
            makecol(255, 255, 255));
        CHICKENS_TEXTOUT_CENTRE(ctx_.assets().buffer,
                                font,
                                "longer to reload. Chickens high up in flight must be shot",
                                SCREEN_W / 2,
                                275,
                                makecol(255, 255, 255));
        CHICKENS_TEXTOUT_CENTRE(
            ctx_.assets().buffer,
            font,
            "with the shotgun, as rockets can only shoot at ground level.",
            SCREEN_W / 2,
            290,
            makecol(255, 255, 255));
        CHICKENS_TEXTOUT_CENTRE(ctx_.assets().buffer,
                                font,
                                "You are given 1 tenderizer. In case of emergency hit SPACE.",
                                SCREEN_W / 2,
                                305,
                                makecol(255, 70, 70));
        CHICKENS_TEXTOUT_CENTRE(ctx_.assets().buffer,
                                font,
                                "Occasionally, cool gems fly out of exploding chickens. Catch",
                                SCREEN_W / 2,
                                320,
                                makecol(255, 255, 255));
        CHICKENS_TEXTOUT_CENTRE(ctx_.assets().buffer,
                                font,
                                "them with your mouse cursor (don't click!) to earn bonuses.",
                                SCREEN_W / 2,
                                335,
                                makecol(255, 255, 255));
        CHICKENS_TEXTOUT_CENTRE(
            ctx_.assets().buffer,
            font,
            "All gems automatically give you 2 extra seconds.",
            SCREEN_W / 2,
            350,
            makecol(255, 255, 255));

        CHICKENS_TEXTOUT_CENTRE(
            ctx_.assets().buffer,
            font,
            "+5 radius",
            SCREEN_W / 2 - 140,
            SCREEN_H / 2 + 120,
            makecol(255, 255, 255));
        CHICKENS_TEXTOUT_CENTRE(
            ctx_.assets().buffer, font, "+8 sec", SCREEN_W / 2, SCREEN_H / 2 + 120, makecol(255, 255, 255));
        CHICKENS_TEXTOUT_CENTRE(ctx_.assets().buffer,
                                font,
                                "+5000 pts",
                                SCREEN_W / 2 + 152,
                                SCREEN_H / 2 + 120,
                                makecol(255, 255, 255));

        CHICKENS_TEXTOUT_CENTRE(
            ctx_.assets().buffer, font, "Rocket: Left-click", SCREEN_W / 2 - 100, SCREEN_H - 50, makecol(255, 30, 30));
        CHICKENS_TEXTOUT_CENTRE(
            ctx_.assets().buffer, font, "Shotgun: Right-click", SCREEN_W / 2 + 100, SCREEN_H - 50, makecol(255, 30, 30));

        CHICKENS_TEXTOUT_RIGHT(ctx_.assets().buffer,
                               font,
                               "moistrous software 2004 (0.2.4)",
                               SCREEN_W - 10,
                               SCREEN_H - 25,
                               makecol(255, 255, 255));

        draw_sprite(ctx_.assets().buffer, mouse_sprite, mx, my);
        blit(ctx_.assets().buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        while (game_time <= 0)
        {
        }

    } while (!keypressed() && mouse_b != 1);
}

void Game::show_modechooser()
{
    Animation bigchick;

    bool done{};
    bool right_foot{};
    int counter{};
    int mx{};
    int my{};
    int bigchick_bloodyfeet{};
    int bigchick_tspeed{};
    float bigchick_xspeed{};
    float bigchick_yspeed{};

    bigchick.load(ctx_.assets().bigchicken_data);
    bigchick.x = 100;
    bigchick.y = 100;

    stop_sample(ctx_.assets().sound_alarm);
    stop_sample(ctx_.assets().sound_highscore);
    stop_sample(ctx_.assets().sound_menu);
    play_sound(ctx_, ctx_.assets().sound_menu, ctx_.settings.VOLUME, 128, FOREVER);

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

            if (mouse_b & 1
                && abs(static_cast<int>(bigchick_xspeed)) + abs(static_cast<int>(bigchick_yspeed)) < 3)
            {
                // Entering Classic Mode
                if (mouse_x < 90)
                {
                    if (mouse_y > SCREEN_H - 160 && mouse_y < SCREEN_H - 90)
                    {
                        ctx_.state.mode = MODE_RESTART;
                        ctx_.state.level_mode = false;
                        // Load original config settings (easier than having a ton of variables to remember them)
                        ctx_.settings = Settings(ctx_.settings.config_path); 
                        ctx_.state.apply_settings(ctx_.settings);
                        
                        done = true;
                    }
                }

                // Surrendering (quit game)
                if (mouse_x > 430 && mouse_x < 530)
                {
                    if (mouse_y > SCREEN_H - 100)
                    {
                        ctx_.state.mode = MODE_QUIT;
                        done = true;
                    }
                }

                // Entering Level Mode
                if (mouse_x > SCREEN_W - 90)
                {
                    if (mouse_y > 280 && mouse_y < 380)
                    {
                        ctx_.state.mode = MODE_RESTART;
                        ctx_.state.level_mode = true;
                        next_level(ctx_, ctx_.state.current_level);
                        done = true;
                    }
                }
            }

            mx = mouse_x;
            my = mouse_y;
            game_time--;
        }

        clear(ctx_.assets().buffer);

        bigchick.x -= bigchick_xspeed;
        bigchick.y -= bigchick_yspeed;

        // Give the chicken a stupid waddle
        bigchick_tspeed =
            abs(static_cast<int>(bigchick_xspeed)) + abs(static_cast<int>(bigchick_yspeed));
        bigchick.angle += (rand() % (1 + bigchick_tspeed) - rand() % (1 + bigchick_tspeed)) / 2;

        if (bigchick_bloodyfeet > 0 && counter % 7 == 0)
        {
            BITMAP* track = create_bitmap(30, 30);
            clear_to_color(track, makecol(255, 0, 255));

            rotate_sprite(track,
                          static_cast<BITMAP*>(ctx_.assets().modechooser_data[1].dat),
                          0,
                          0,
                          itofix(static_cast<int>(bigchick.angle) + rand() % 6 - rand() % 6));
            set_trans_blender(255, 255, 255, bigchick_bloodyfeet * 10);
            draw_trans_sprite(static_cast<BITMAP*>(ctx_.assets().modechooser_data[0].dat),
                              track,
                              static_cast<int>(bigchick.x) + 15 - rand() % 20,
                              static_cast<int>(bigchick.y) + 30 - rand() % 20);
            bigchick_bloodyfeet--;

            destroy_bitmap(track);

            right_foot = !right_foot;
        }

        draw_sprite(ctx_.assets().buffer, static_cast<BITMAP*>(ctx_.assets().modechooser_data[0].dat), 0, 0);
        bigchick.play(ctx_.assets().buffer);

        draw_sprite(ctx_.assets().buffer, mouse_sprite, mx, my);
        blit(ctx_.assets().buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        while (game_time <= 0)
        {
        }

    } while (!done);

    fadeout(ctx_, makecol(0, 0, 0), 40);

    if (ctx_.state.mode == MODE_RESTART && ctx_.state.level_mode == true)
    {
        show_levelnumber();
    }
}

void Game::show_levelcompleted()
{
    SAMPLE* sound_count = load_sample(CHICKENS_ASSETS_REL("sound/counter.wav"));

    int b{};
    int mx{};
    int my{};
    float a{};
    float accuracy = ((static_cast<float>(ctx_.state.kills) / static_cast<float>(ctx_.state.shots_fired)) * 70)
                     + ((ctx_.state.timer / 60) * 30);
    float bonus{};

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
                play_sound(ctx_, sound_count, int(float(a / accuracy) * ctx_.settings.VOLUME), 128, ONCE);
            }
        }

        clear(ctx_.assets().buffer);

        CHICKENS_TEXTPRINTF_CENTRE(ctx_.assets().buffer,
                                   ctx_.assets().font_big,
                                   SCREEN_W / 2,
                                   SCREEN_H / 2,
                                   makecol(255, 255, 255),
                                   "Level %d Completed!",
                                   ctx_.state.current_level);
        {
            /* a>100: green text here; old inner loop only set unused `color`. */
            const int acc_fg = (a > 100.0f) ? makecol(0, 255, 0) : makecol(255, 255, 255);
            CHICKENS_TEXTPRINTF_CENTRE(ctx_.assets().buffer,
                                       ctx_.assets().font_interface,
                                       SCREEN_W / 2,
                                       SCREEN_H / 2 + 90,
                                       acc_fg,
                                       "Accuracy: %d%%",
                                       static_cast<int>(a));
        }
        if (a == 100 && b > 0)
        {
            CHICKENS_TEXTPRINTF_CENTRE(ctx_.assets().buffer,
                                       ctx_.assets().font_interface,
                                       SCREEN_W / 2 + 60,
                                       SCREEN_H / 2 + 90 - (255 - b),
                                       makecol(0, b, 0),
                                       "+%d",
                                       static_cast<int>(bonus));
            b -= 2;
        }

        draw_sprite(ctx_.assets().buffer, mouse_sprite, mx, my);
        blit(ctx_.assets().buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        while (game_time <= 0)
        {
        }

    } while (mouse_b != 1);

    destroy_sample(sound_count);

    fadeout(ctx_, makecol(0, 0, 0), 40);
}

void Game::restart()
{

    for (int i = 0; i < ctx_.settings.MAX_CHICKENS; ++i)
    {
        chickens_[i].reset();
    }

    for (int i = 0; i < MAX_GEMS; ++i)
    {
        gem_[i].active = false;
    }

    for (int i = 0; i < MAX_SMOKE; ++i)
    {
        smoke_[i].active = false;
    }

    ctx_.assets().background = static_cast<BITMAP*>(
        ctx_.assets().background_data[rand() % AssetManager::items_in_datafile(ctx_.assets().background_data)].dat);
    terrain_.create(ctx_.assets());

    ctx_.settings.ROCKET_SIZE = ctx_.state.tmp_rocket_size;
    ctx_.state.tenderizers = 1;

    if (ctx_.state.level_mode == false)
    {
        ctx_.state.timer = ctx_.settings.TIMER;
    }

    ctx_.state.delay_of_levelend = 40;
    ctx_.state.timer_delay = 0;
    ctx_.state.score = 0;
    ctx_.state.shots_fired = 0;
    ctx_.state.kills = 0;
    ctx_.state.runners = ctx_.settings.INITIAL_CHICKENS;
    ctx_.state.alert_mode = false;
    ctx_.state.not_dead = true;

    stop_sample(ctx_.assets().sound_gameover); // They might not always be playing but stop them anyway
    stop_sample(ctx_.assets().sound_highscore);
    stop_sample(ctx_.assets().sound_tenderizer);
    stop_sample(ctx_.assets().sound_menu);
}

void earn_bonus(AppContext& ctx, int type)
{
    play_sound(ctx, ctx.assets().sound_gemcollect, ctx.settings.VOLUME, int(mouse_x / 3.13), ONCE);

    switch (type)
    {
    case BONUS_TIMER: // Blue gem
        if (ctx.state.level_mode == false)
        {
            ctx.state.timer += 6;
        }
        else
        {
            ctx.state.timer++;
        }
        break;

    case BONUS_ROCKETSIZE: // Green gem
        if (ctx.state.level_mode == false)
        {
            ctx.state.timer += 2;
        }
        ctx.settings.ROCKET_SIZE += 5;
        break;

    case BONUS_SCORE: // Red gem
        if (ctx.state.level_mode == false)
        {
            ctx.state.score += 5000;
            ctx.state.timer += 2;
        }
        else
        {
            ctx.state.timer += 3;
        }
        break;

    default:
        break;
    }
}
void Game::show_levelnumber()
{
    int mx{};
    int my{};

    do
    {
        while (game_time > 0)
        {
            mx = mouse_x;
            my = mouse_y;
            game_time--;
        }

        clear(ctx_.assets().buffer);

        CHICKENS_TEXTPRINTF_CENTRE(ctx_.assets().buffer,
                                   ctx_.assets().font_big,
                                   SCREEN_W / 2,
                                   SCREEN_H / 2,
                                   makecol(255, 255, 255),
                                   "Level %d",
                                   ctx_.state.current_level);

        draw_sprite(ctx_.assets().buffer, mouse_sprite, mx, my);
        blit(ctx_.assets().buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        while (game_time <= 0)
        {
        }

    } while (mouse_b != 1);

    fadeout(ctx_, makecol(0, 0, 0), 30);
}

void Game::show_highscores(int player_rank_one_based)
{
    ::show_highscores(player_rank_one_based - 1, ctx_.assets().buffer, ctx_.assets().background, ctx_.assets());
}

void Game::tick_mode_restart(AppContext& c)
{
    restart();
    c.state.mode = MODE_PLAYING;
    if (c.state.level_mode == true)
    {
        next_level(c, c.state.current_level);
    }
}

void Game::tick_mode_playing(AppContext& c, bool& alert_sound, bool& fire_rocket, bool& fire_shotgun)
{
    if (c.state.chickens_left < 0)
    {
        c.state.chickens_left = 0;
    }
    for (int i = 0; i < MAX_SMOKE; ++i)
    {
        smoke_[i].run();
    }
    for (int i = 0; i < MAX_GEMS; ++i)
    {
        if (gem_[i].run())
        {
            earn_bonus(c, gem_[i].type);
        }
    }
    if (c.state.timer_delay-- == 0)
    {
        c.state.timer_delay = 60;
        c.state.timer--;
    }
    if (c.state.timer <= 10)
    {
        c.state.alert_mode = true;
        if (!alert_sound)
        {
            play_sound(c, c.assets().sound_alarm, c.settings.VOLUME, 128, FOREVER);
            alert_sound = true;
        }
    }
    else if (c.state.alert_mode)
    {
        stop_sample(c.assets().sound_alarm);
        alert_sound = false;
        c.state.alert_mode = false;
    }
    weapon_manager(c, &fire_rocket, &fire_shotgun);
    for (int i = 0; i < c.state.runners; ++i)
    {
        if (chickens_[i].run() == CROSSED_THE_ROAD)
        {
            c.state.mode = MODE_GAMEOVER;
        }

        if (fire_rocket)
        {
            if (chickens_[i].alive == NOT_KILLED &&
                ((SCREEN_H - chickens_[i].y) - terrain_.height[mouse_x]) < c.settings.ROCKET_SIZE &&
                (abs(mouse_x - static_cast<int>(chickens_[i].x)) < c.settings.ROCKET_SIZE ||
                 abs(mouse_x - static_cast<int>(chickens_[i].x) - CHICKEN_WIDTH) < c.settings.ROCKET_SIZE))
            {
                c.state.score += c.settings.POINTS_FOR_ROCKET;
                chickens_[i].alive = KILLED_WITH_ROCKET;
                ++c.state.kills;
                --c.state.chickens_left;

                if (c.state.runners < c.settings.MAX_CHICKENS - 1 &&
                    rand() % c.settings.RESPAWN_RATE <= 1)
                {
                    c.state.runners++;
                }

                if (rand() % c.settings.CHANCE_OF_GEM <= 1)
                {
                    for (int j = 0; j < MAX_GEMS; ++j)
                    {
                        if (gem_[j].active == false)
                        {
                            gem_[j].launch(chickens_[i].x, chickens_[i].y);
                            break;
                        }
                    }
                }
            }
        }

        if (fire_shotgun)
        {
            if (chickens_[i].alive == NOT_KILLED &&
                (abs(mouse_x - static_cast<int>(chickens_[i].x)) < c.settings.SHOTGUN_SIZE ||
                 abs(mouse_x - static_cast<int>(chickens_[i].x) - CHICKEN_WIDTH) < c.settings.SHOTGUN_SIZE) &&
                (abs(mouse_y - static_cast<int>(chickens_[i].y)) < c.settings.SHOTGUN_SIZE ||
                 abs(mouse_y - static_cast<int>(chickens_[i].y) - CHICKEN_HEIGHT) < c.settings.SHOTGUN_SIZE))
            {
                c.state.score += c.settings.POINTS_FOR_SHOTGUN;
                chickens_[i].alive = KILLED_WITH_SHOTGUN;
                chickens_[i].flight = chickens_[i].direction;
                ++c.state.kills;
                --c.state.chickens_left;

                if (c.state.runners < c.settings.MAX_CHICKENS - 1 &&
                    rand() % c.settings.RESPAWN_RATE <= 1)
                {
                    c.state.runners++;
                }

                if (rand() % c.settings.CHANCE_OF_GEM <= 1)
                {
                    for (int j = 0; j < MAX_GEMS; ++j)
                    {
                        if (gem_[j].active == false)
                        {
                            gem_[j].launch(chickens_[i].x, chickens_[i].y);
                            break;
                        }
                    }
                }
            }
        }
    }
    if (fire_rocket)
    {
        for (int i = 0; i < MAX_SMOKE; ++i)
        {
            if (smoke_[i].active == false)
            {
                smoke_[i].release();
                break;
            }
        }
        ++c.state.shots_fired;
    }
    if (fire_shotgun)
    {
        ++c.state.shots_fired;
    }

    if (c.state.level_mode == false)
    {
        if (c.state.tenderizers > 0)
        {
            if (key[KEY_SPACE])
            {
                for (int i = 0; i < c.settings.MAX_CHICKENS; ++i)
                {
                    if (chickens_[i].alive == NOT_KILLED)
                    {
                        chickens_[i].alive = KILLED_WITH_TENDERIZER;
                        chickens_[i].flight = chickens_[i].direction;
                        c.state.score += c.settings.POINTS_FOR_TENDERIZER;
                        c.state.kills++;
                    }
                }

                play_sound(c, c.assets().sound_tenderizer, c.settings.VOLUME, 128, ONCE);
                c.state.tenderizers--;
            }
        }
    }

    if (c.state.timer <= 0)
    {
        c.state.mode = MODE_GAMEOVER;
    }

    if (key[KEY_R])
    {
        c.state.mode = MODE_RESTART;
        fadeout(c, makecol(0, 0, 0), 20);
    }

    if (c.state.level_mode == true)
    {
        if (c.state.chickens_left < 1)
        {
            if (c.state.delay_of_levelend-- == 0)
            {
                c.state.mode = MODE_NEXTLEVEL;
            }
        }
    }

    if (c.state.mode == MODE_GAMEOVER)
    {
        if (c.state.level_mode == true)
        {
            if (c.state.chickens_left < 1)
            {
                c.state.mode = MODE_PLAYING;
            }
        }
    }
}

void Game::tick_mode_next_level(AppContext& c)
{
    fadeout(c, makecol(0, 0, 0), 30);
    show_levelcompleted();
    restart();
    next_level(c, c.state.current_level + 1);
    show_levelnumber();
    c.state.mode = MODE_PLAYING;
}

void Game::tick_mode_paused(AppContext& c)
{
    if (key[KEY_C])
    {
        c.state.mode = MODE_PLAYING;
    }
    if (key[KEY_Q])
    {
        fadeout(c, makecol(0, 0, 0), 30);
        show_modechooser();
    }
}

void Game::tick_mode_gameover(AppContext& c, int& rank)
{
    if (c.state.level_mode == false && c.state.not_dead)
    {
        rank = save_highscore(c.state.playername, c.state.score);
        stop_sample(c.assets().sound_alarm);
        c.state.not_dead = false;
        if (rank <= HIGHSCORE_TABLE)
        {
            play_sound(c, c.assets().sound_highscore, c.settings.VOLUME, 128, FOREVER);
        }
        else
        {
            play_sound(c, c.assets().sound_gameover, c.settings.VOLUME, 128, ONCE);
        }
    }
    if (key[KEY_R])
    {
        c.state.mode = MODE_RESTART;
        fadeout(c, makecol(0, 0, 0), 30);
    }
    if (key[KEY_ESC] || key[KEY_SPACE] || key[KEY_ENTER])
    {
        fadeout(c, makecol(0, 0, 0), 30);
        show_modechooser();
    }
}

void Game::draw_frame_playing(AppContext& c)
{
    for (int i = 0; i < MAX_SMOKE; ++i)
    {
        smoke_[i].draw(c.render_context);
    }
    draw_sprite(c.assets().buffer, terrain_.image, 0, SCREEN_H - MAX_LEVELHEIGHT);
    for (int i = 0; i < MAX_GEMS; ++i)
    {
        gem_[i].draw(c.render_context);
    }
    for (int i = 0; i < c.state.runners; ++i)
    {
        chickens_[i].draw(c.render_context);
    }
    show_statistics(c);
}

void Game::draw_frame_paused(AppContext& c)
{
    for (int i = 0; i < SCREEN_H; i += 2)
    {
        line(c.assets().buffer, 0, i, SCREEN_W, i, makecol(0, 0, 60));
    }
    CHICKENS_TEXTOUT_CENTRE(
        c.assets().buffer, c.assets().font_big, "PAUSED", SCREEN_W / 2, SCREEN_H / 2 - 40, makecol(255, 255, 255));
    CHICKENS_TEXTOUT_CENTRE(c.assets().buffer,
                            font,
                            "Press 'C' to continue or 'Q' to quit",
                            SCREEN_W / 2,
                            SCREEN_H / 2 + 60,
                            makecol(255, 255, 255));
}

void Game::draw_frame_gameover(AppContext& c, int rank)
{
    rectfill(c.assets().buffer, 0, SCREEN_H - 40, SCREEN_W, SCREEN_H, makecol(0, 0, 0));
    CHICKENS_TEXTOUT_CENTRE(c.assets().buffer,
                            c.assets().font_big,
                            "Armageddon",
                            SCREEN_W / 2,
                            SCREEN_H / 2 - 20,
                            makecol(255, 255, 255));
    CHICKENS_TEXTOUT_CENTRE(c.assets().buffer,
                            font,
                            "The chickens have risen. Everyone is dead. Our world is gone.",
                            SCREEN_W / 2,
                            SCREEN_H / 2 + 40,
                            makecol(255, 255, 255));
    CHICKENS_TEXTOUT_CENTRE(c.assets().buffer,
                            font,
                            "Press 'R' to play again!",
                            SCREEN_W / 2,
                            SCREEN_H - 35,
                            makecol(0, 255, 0));
    CHICKENS_TEXTOUT_CENTRE(c.assets().buffer,
                            font,
                            "Not that this is a game. This is actually happening, in real life.",
                            SCREEN_W / 2,
                            SCREEN_H - 20,
                            makecol(200, 200, 200));
    if (rank <= HIGHSCORE_TABLE)
    {
        CHICKENS_TEXTPRINTF_CENTRE(c.assets().buffer,
                                   c.assets().font_interface,
                                   SCREEN_W / 2,
                                   SCREEN_H / 2 + 65,
                                   makecol(0, 220, 0),
                                   "But you did get a High Score of %d points in rank %d!",
                                   c.state.score,
                                   rank);
    }
}

void Game::run()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    AppContext& c = ctx_;
    show_startup();
    fadeout(c, makecol(0, 0, 0), 50);
    show_modechooser();

    int rank{HIGHSCORE_TABLE + 1};
    bool alert_sound{};
    bool fire_rocket{};
    bool fire_shotgun{};
    int mx{};
    int my{};

    do
    {
        while (game_time > 0)
        {
            mode_manager(c);
            mx = mouse_x;
            my = mouse_y;

            switch (c.state.mode)
            {
            case MODE_RESTART:
                tick_mode_restart(c);
                break;
            case MODE_PLAYING:
                tick_mode_playing(c, alert_sound, fire_rocket, fire_shotgun);
                break;
            case MODE_NEXTLEVEL:
                tick_mode_next_level(c);
                break;
            case MODE_PAUSED:
                tick_mode_paused(c);
                break;
            case MODE_GAMEOVER:
                tick_mode_gameover(c, rank);
                break;
            default:
                break;
            }
            game_time--;
        }

        clear(c.assets().buffer);
        draw_sprite(c.assets().buffer, c.assets().background, 0, 0);
        switch (c.state.mode)
        {
        case MODE_PLAYING:
            draw_frame_playing(c);
            break;
        case MODE_PAUSED:
            draw_frame_paused(c);
            break;
        case MODE_GAMEOVER:
            draw_frame_gameover(c, rank);
            break;
        default:
            break;
        }
        draw_sprite(c.assets().buffer, mouse_sprite, mx - 11, my - 11);
        blit(c.assets().buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
        while (game_time <= 0)
        {
        }
    } while (c.state.mode != MODE_QUIT);

    show_highscores(rank);
}
