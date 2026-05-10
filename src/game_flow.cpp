#include "game_flow.h"

#include <cmath>
#include <cstdlib>

#include "animation.h"
#include "asset_manager.h"
#include "game_state.h"
#include "level.h"

extern Settings game_settings;
extern GameState game_state;
extern Level level;
extern AppAssets& assets;

int mode_manager()
{
    if (game_state.mode == MODE_PLAYING)
    {
        set_mouse_sprite(static_cast<BITMAP*>(assets.cursors_data[1].dat));

        if (key[KEY_ESC] || key[KEY_PAUSE])
        {
            game_state.mode = MODE_PAUSED;
        }
    }
    else
    {
        set_mouse_sprite(static_cast<BITMAP*>(assets.cursors_data[0].dat));
    }

    return game_state.mode;
}

void show_startup()
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

        clear(assets.buffer);

        draw_sprite(
            assets.buffer, static_cast<BITMAP*>(assets.icons_data[2].dat), SCREEN_W / 2 - 177, 4);
        draw_sprite(
            assets.buffer,
            static_cast<BITMAP*>(assets.gem_data[1].dat),
            SCREEN_W / 2 - 143,
            SCREEN_H / 2 + 100);
        draw_sprite(assets.buffer,
                    static_cast<BITMAP*>(assets.gem_data[0].dat),
                    SCREEN_W / 2 - 3,
                    SCREEN_H / 2 + 100);
        draw_sprite(
            assets.buffer,
            static_cast<BITMAP*>(assets.gem_data[2].dat),
            SCREEN_W / 2 + 147,
            SCREEN_H / 2 + 100);

        line(assets.buffer, 0, 104, SCREEN_W, 104, makecol(100, 0, 0));
        line(assets.buffer, 0, SCREEN_H - 30, SCREEN_W, SCREEN_H - 30, makecol(100, 0, 0));

        CHICKENS_TEXTOUT_CENTRE(
            assets.buffer,
            font,
            "In the final days of Armageddon, the chickens are preparing",
            SCREEN_W / 2,
            130,
            makecol(200, 200, 200));
        CHICKENS_TEXTOUT_CENTRE(
            assets.buffer,
            font,
            "to detonate our planet. They are trying to run across your",
            SCREEN_W / 2,
            145,
            makecol(200, 200, 200));
        CHICKENS_TEXTOUT_CENTRE(
            assets.buffer,
            font,
            "screen, and if even a single chicken succeeds, it's all over.",
            SCREEN_W / 2,
            160,
            makecol(200, 200, 200));
        CHICKENS_TEXTOUT_CENTRE(
            assets.buffer,
            font,
            "Please look in README for info about LEVEL MODE--this screen is temporary!",
            SCREEN_W / 2,
            180,
            makecol(220, 0, 0));
        CHICKENS_TEXTOUT_CENTRE(
            assets.buffer, font, "CLASSIC MODE:", SCREEN_W / 2, 200, makecol(220, 0, 0));
        CHICKENS_TEXTOUT_CENTRE(assets.buffer,
                                font,
                                "You are on a 2 minute game_state.timer. Each time that you shoot, you",
                                SCREEN_W / 2,
                                215,
                                makecol(255, 255, 255));
        CHICKENS_TEXTOUT_CENTRE(assets.buffer,
                                font,
                                "lose an additional second, so try to kill as many chickens",
                                SCREEN_W / 2,
                                230,
                                makecol(255, 255, 255));
        CHICKENS_TEXTOUT_CENTRE(
            assets.buffer,
            font,
            "per click as possible. Killing chickens with rockets gives",
            SCREEN_W / 2,
            245,
            makecol(255, 255, 255));
        CHICKENS_TEXTOUT_CENTRE(
            assets.buffer,
            font,
            "you 100 points. A shotgun kill earns you 250, but it takes",
            SCREEN_W / 2,
            260,
            makecol(255, 255, 255));
        CHICKENS_TEXTOUT_CENTRE(assets.buffer,
                                font,
                                "longer to reload. Chickens high up in flight must be shot",
                                SCREEN_W / 2,
                                275,
                                makecol(255, 255, 255));
        CHICKENS_TEXTOUT_CENTRE(
            assets.buffer,
            font,
            "with the shotgun, as rockets can only shoot at ground level.",
            SCREEN_W / 2,
            290,
            makecol(255, 255, 255));
        CHICKENS_TEXTOUT_CENTRE(assets.buffer,
                                font,
                                "You are given 1 tenderizer. In case of emergency hit SPACE.",
                                SCREEN_W / 2,
                                305,
                                makecol(255, 70, 70));
        CHICKENS_TEXTOUT_CENTRE(assets.buffer,
                                font,
                                "Occasionally, cool gems fly out of exploding chickens. Catch",
                                SCREEN_W / 2,
                                320,
                                makecol(255, 255, 255));
        CHICKENS_TEXTOUT_CENTRE(assets.buffer,
                                font,
                                "them with your mouse cursor (don't click!) to earn bonuses.",
                                SCREEN_W / 2,
                                335,
                                makecol(255, 255, 255));
        CHICKENS_TEXTOUT_CENTRE(
            assets.buffer,
            font,
            "All gems automatically give you 2 extra seconds.",
            SCREEN_W / 2,
            350,
            makecol(255, 255, 255));

        CHICKENS_TEXTOUT_CENTRE(
            assets.buffer,
            font,
            "+5 radius",
            SCREEN_W / 2 - 140,
            SCREEN_H / 2 + 120,
            makecol(255, 255, 255));
        CHICKENS_TEXTOUT_CENTRE(
            assets.buffer, font, "+8 sec", SCREEN_W / 2, SCREEN_H / 2 + 120, makecol(255, 255, 255));
        CHICKENS_TEXTOUT_CENTRE(assets.buffer,
                                font,
                                "+5000 pts",
                                SCREEN_W / 2 + 152,
                                SCREEN_H / 2 + 120,
                                makecol(255, 255, 255));

        CHICKENS_TEXTOUT_CENTRE(
            assets.buffer, font, "Rocket: Left-click", SCREEN_W / 2 - 100, SCREEN_H - 50, makecol(255, 30, 30));
        CHICKENS_TEXTOUT_CENTRE(
            assets.buffer, font, "Shotgun: Right-click", SCREEN_W / 2 + 100, SCREEN_H - 50, makecol(255, 30, 30));

        CHICKENS_TEXTOUT_RIGHT(assets.buffer,
                               font,
                               "moistrous software 2004 (0.2.4)",
                               SCREEN_W - 10,
                               SCREEN_H - 25,
                               makecol(255, 255, 255));

        draw_sprite(assets.buffer, mouse_sprite, mx, my);
        blit(assets.buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        while (game_time <= 0)
        {
        }

    } while (!keypressed() && mouse_b != 1);
}

void show_modechooser()
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

    bigchick.load(assets.bigchicken_data);
    bigchick.x = 100;
    bigchick.y = 100;

    stop_sample(assets.sound_alarm);
    stop_sample(assets.sound_highscore);
    stop_sample(assets.sound_menu);
    play_sound(assets.sound_menu, game_settings.VOLUME, 128, FOREVER);

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
                        game_state.mode = MODE_RESTART;
                        game_state.level_mode = false;
                        game_settings = Settings(
                            game_state.config_path); // Load original config settings (easier than a
                        game_state.apply_settings(game_settings);
                        // having a ton of variables to remember them)
                        done = true;
                    }
                }

                // Surrendering (quit game)
                if (mouse_x > 430 && mouse_x < 530)
                {
                    if (mouse_y > SCREEN_H - 100)
                    {
                        game_state.mode = MODE_QUIT;
                        done = true;
                    }
                }

                // Entering Level Mode
                if (mouse_x > SCREEN_W - 90)
                {
                    if (mouse_y > 280 && mouse_y < 380)
                    {
                        game_state.mode = MODE_RESTART;
                        game_state.level_mode = true;
                        next_level(game_state.current_level);
                        done = true;
                    }
                }
            }

            mx = mouse_x;
            my = mouse_y;
            game_time--;
        }

        clear(assets.buffer);

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
                          static_cast<BITMAP*>(assets.modechooser_data[1].dat),
                          0,
                          0,
                          itofix(static_cast<int>(bigchick.angle) + rand() % 6 - rand() % 6));
            set_trans_blender(255, 255, 255, bigchick_bloodyfeet * 10);
            draw_trans_sprite(static_cast<BITMAP*>(assets.modechooser_data[0].dat),
                              track,
                              static_cast<int>(bigchick.x) + 15 - rand() % 20,
                              static_cast<int>(bigchick.y) + 30 - rand() % 20);
            bigchick_bloodyfeet--;

            destroy_bitmap(track);

            right_foot = !right_foot;
        }

        draw_sprite(assets.buffer, static_cast<BITMAP*>(assets.modechooser_data[0].dat), 0, 0);
        bigchick.play(assets.buffer);

        draw_sprite(assets.buffer, mouse_sprite, mx, my);
        blit(assets.buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        while (game_time <= 0)
        {
        }

    } while (!done);

    fadeout(makecol(0, 0, 0), 40);

    if (game_state.mode == MODE_RESTART && game_state.level_mode == true)
    {
        show_levelnumber();
    }
}

void show_levelcompleted()
{
    SAMPLE* sound_count = load_sample(CHICKENS_ASSETS_REL("sound/counter.wav"));

    int b{};
    int mx{};
    int my{};
    float a{};
    float accuracy = ((static_cast<float>(game_state.kills) / static_cast<float>(game_state.shots_fired)) * 70)
                     + ((game_state.timer / 60) * 30);
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
                play_sound(sound_count, int(float(a / accuracy) * game_settings.VOLUME), 128, ONCE);
            }
        }

        clear(assets.buffer);

        CHICKENS_TEXTPRINTF_CENTRE(assets.buffer,
                                   assets.font_big,
                                   SCREEN_W / 2,
                                   SCREEN_H / 2,
                                   makecol(255, 255, 255),
                                   "Level %d Completed!",
                                   game_state.current_level);
        {
            /* a>100: green text here; old inner loop only set unused `color`. */
            const int acc_fg = (a > 100.0f) ? makecol(0, 255, 0) : makecol(255, 255, 255);
            CHICKENS_TEXTPRINTF_CENTRE(assets.buffer,
                                       assets.font_interface,
                                       SCREEN_W / 2,
                                       SCREEN_H / 2 + 90,
                                       acc_fg,
                                       "Accuracy: %d%%",
                                       static_cast<int>(a));
        }
        if (a == 100 && b > 0)
        {
            CHICKENS_TEXTPRINTF_CENTRE(assets.buffer,
                                       assets.font_interface,
                                       SCREEN_W / 2 + 60,
                                       SCREEN_H / 2 + 90 - (255 - b),
                                       makecol(0, b, 0),
                                       "+%d",
                                       static_cast<int>(bonus));
            b -= 2;
        }

        draw_sprite(assets.buffer, mouse_sprite, mx, my);
        blit(assets.buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        while (game_time <= 0)
        {
        }

    } while (mouse_b != 1);

    destroy_sample(sound_count);

    fadeout(makecol(0, 0, 0), 40);
}

void restart(std::array<Chicken, MAX_CHICKENS_CAPACITY>& chicken,
             std::array<Gem, MAX_GEMS>& gem,
             std::array<Smoke, MAX_SMOKE>& smoke)
{
    for (int i = 0; i < game_settings.MAX_CHICKENS; ++i)
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

    assets.background = static_cast<BITMAP*>(
        assets.background_data[rand() % items_in_datafile(assets.background_data)].dat);
    level.create();

    game_settings.ROCKET_SIZE = game_state.tmp_rocket_size;
    game_state.tenderizers = 1;

    if (game_state.level_mode == false)
    {
        game_state.timer = game_settings.TIMER;
    }

    game_state.delay_of_levelend = 40;
    game_state.timer_delay = 0;
    game_state.score = 0;
    game_state.shots_fired = 0;
    game_state.kills = 0;
    game_state.runners = game_settings.INITIAL_CHICKENS;
    game_state.alert_mode = false;
    game_state.not_dead = true;

    stop_sample(assets.sound_gameover); // They might not always be playing but stop them anyway
    stop_sample(assets.sound_highscore);
    stop_sample(assets.sound_tenderizer);
    stop_sample(assets.sound_menu);
}

void earn_bonus(int type)
{
    play_sound(assets.sound_gemcollect, game_settings.VOLUME, int(mouse_x / 3.13), ONCE);

    switch (type)
    {
    case BONUS_TIMER: // Blue gem
        if (game_state.level_mode == false)
        {
            game_state.timer += 6;
        }
        else
        {
            game_state.timer++;
        }
        break;

    case BONUS_ROCKETSIZE: // Green gem
        if (game_state.level_mode == false)
        {
            game_state.timer += 2;
        }
        game_settings.ROCKET_SIZE += 5;
        break;

    case BONUS_SCORE: // Red gem
        if (game_state.level_mode == false)
        {
            game_state.score += 5000;
            game_state.timer += 2;
        }
        else
        {
            game_state.timer += 3;
        }
        break;

    default:
        break;
    }
}

void show_levelnumber()
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

        clear(assets.buffer);

        CHICKENS_TEXTPRINTF_CENTRE(assets.buffer,
                                   assets.font_big,
                                   SCREEN_W / 2,
                                   SCREEN_H / 2,
                                   makecol(255, 255, 255),
                                   "Level %d",
                                   game_state.current_level);

        draw_sprite(assets.buffer, mouse_sprite, mx, my);
        blit(assets.buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        while (game_time <= 0)
        {
        }

    } while (mouse_b != 1);

    fadeout(makecol(0, 0, 0), 30);
}

void fadeout(int color, int duration)
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
        draw_trans_sprite(assets.buffer, fading, 0, 0);

        blit(assets.buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        while (game_time <= 0)
        {
        }

    } while (duration > 0);
}

void next_level(int level_number)
{
    stop_sample(assets.sound_alarm);
    stop_sample(assets.sound_highscore);
    stop_sample(assets.sound_gameover);

    game_state.current_level = level_number;
    game_state.chickens_left = game_settings.INITIAL_CHICKENS + (level_number * 4);
    game_settings.CHICKEN_SPEED = static_cast<int>(sqrt(static_cast<double>(game_state.chickens_left))) / 2;
    game_state.timer = 60;
}

void show_statistics()
{
    int minutes{game_state.timer / 60};
    int seconds{game_state.timer - (minutes * 60)};
    const char* format{(seconds < 10) ? "%d:0%d" : "%d:%d"};

    if (game_state.level_mode == true)
    {
        // Level Mode:
        CHICKENS_TEXTPRINTF(
            assets.buffer, assets.font_interface, 20, 5, makecol(255, 255, 255), "Level: %d", game_state.current_level);
        CHICKENS_TEXTPRINTF(assets.buffer,
                            assets.font_interface,
                            20,
                            25,
                            makecol(255, 255, 255),
                            "Chickens: %d    ",
                            game_state.chickens_left);
        CHICKENS_TEXTPRINTF(
            assets.buffer, assets.font_big, SCREEN_W - 90, 0, makecol(255, 255, 255), format, minutes, seconds);
    }
    else
    {
        // Classic Mode:
        CHICKENS_TEXTPRINTF(
            assets.buffer, assets.font_interface, 20, 5, makecol(255, 255, 255), "Score: %d    ", game_state.score);
        CHICKENS_TEXTPRINTF(
            assets.buffer, assets.font_interface, 20, 25, makecol(255, 255, 255), "Kills: %d    ", game_state.kills);
        CHICKENS_TEXTPRINTF(
            assets.buffer, assets.font_big, SCREEN_W - 90, 0, makecol(255, 255, 255), format, minutes, seconds);

        if (game_state.alert_mode)
        {
            set_trans_blender(255, 255, 255, 150);
            draw_trans_sprite(
                assets.buffer, static_cast<BITMAP*>(assets.icons_data[0].dat), SCREEN_W / 2 - 50, SCREEN_H / 2 - 50);
            CHICKENS_TEXTOUT_CENTRE(
                assets.buffer, assets.font_big, "ALERT", SCREEN_W / 2, SCREEN_H / 2 - 30, makecol(255, 255, 255));
        }
    }
}

void weapon_manager(bool* fire_rocket, bool* fire_shotgun)
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
        reloading_rocket = game_settings.ROCKET_RELOAD; // reload time
        play_sound(assets.sound_rocket, game_settings.VOLUME, int(mouse_x / 3.13), ONCE);

        --game_state.timer;
    }

    if (*fire_shotgun)
    {
        reloading_shotgun = game_settings.SHOTGUN_RELOAD; // reload time
        play_sound(assets.sound_shotgun,
                   game_settings.VOLUME,
                   int(mouse_x / 3.13),
                   ONCE); // Pan speaker output to mouse location

        --game_state.timer;
    }

    return;
}

void play_sound(const SAMPLE* snd, int volume, int pan, bool loop)
{
    if (game_state.mute_sound == false)
    {
        play_sample(snd, volume, pan, 1000, loop);
    }
}
