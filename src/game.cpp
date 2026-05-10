#include "game.h"

#include <allegro.h>

#include "asset_manager.h"
#include "game_flow.h"
#include "game_state.h"
#include "highscore.h"
#include "level.h"

extern Settings game_settings;
extern GameState game_state;
extern Level level;
extern AppAssets& assets;
extern volatile int game_time;

Game::Game(const RenderContext& render_context) : render_context_(render_context) {}

void Game::show_highscores(int player_rank_one_based)
{
    ::show_highscores(player_rank_one_based - 1, assets.buffer, assets.background);
}

void Game::run()
{
    show_startup();
    fadeout(makecol(0, 0, 0), 50);
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
            mode_manager();
            mx = mouse_x;
            my = mouse_y;

            switch (game_state.mode)
            {
            case MODE_RESTART:
                restart(chicken_, gem_, smoke_);
                game_state.mode = MODE_PLAYING;
                if (game_state.level_mode == true)
                {
                    next_level(game_state.current_level);
                }
                break;
            case MODE_PLAYING:
                if (game_state.chickens_left < 0)
                {
                    game_state.chickens_left = 0;
                }
                for (int i = 0; i < MAX_SMOKE; ++i)
                {
                    smoke_[i].run();
                }
                for (int i = 0; i < MAX_GEMS; ++i)
                {
                    if (gem_[i].run())
                    {
                        earn_bonus(gem_[i].type);
                    }
                }
                if (game_state.timer_delay-- == 0)
                {
                    game_state.timer_delay = 60;
                    game_state.timer--;
                }
                if (game_state.timer <= 10)
                {
                    game_state.alert_mode = true;
                    if (!alert_sound)
                    {
                        play_sound(assets.sound_alarm, game_settings.VOLUME, 128, FOREVER);
                        alert_sound = true;
                    }
                }
                else if (game_state.alert_mode)
                {
                    stop_sample(assets.sound_alarm);
                    alert_sound = false;
                    game_state.alert_mode = false;
                }
                weapon_manager(&fire_rocket, &fire_shotgun);
                for (int i = 0; i < game_state.runners; ++i)
                {
                    if (chicken_[i].run() == CROSSED_THE_ROAD)
                    {
                        game_state.mode = MODE_GAMEOVER;
                    }

                    if (fire_rocket)
                    {
                        if (chicken_[i].alive == NOT_KILLED &&
                            ((SCREEN_H - chicken_[i].y) - level.height[mouse_x]) < game_settings.ROCKET_SIZE &&
                            (abs(mouse_x - static_cast<int>(chicken_[i].x)) < game_settings.ROCKET_SIZE ||
                             abs(mouse_x - static_cast<int>(chicken_[i].x) - CHICKEN_WIDTH) < game_settings.ROCKET_SIZE))
                        {
                            game_state.score += game_settings.POINTS_FOR_ROCKET;
                            chicken_[i].alive = KILLED_WITH_ROCKET;
                            ++game_state.kills;
                            --game_state.chickens_left;

                            if (game_state.runners < game_settings.MAX_CHICKENS - 1 &&
                                rand() % game_settings.RESPAWN_RATE <= 1)
                            {
                                game_state.runners++;
                            }

                            if (rand() % game_settings.CHANCE_OF_GEM <= 1)
                            {
                                for (int j = 0; j < MAX_GEMS; ++j)
                                {
                                    if (gem_[j].active == false)
                                    {
                                        gem_[j].launch(chicken_[i].x, chicken_[i].y);
                                        break;
                                    }
                                }
                            }
                        }
                    }

                    if (fire_shotgun)
                    {
                        if (chicken_[i].alive == NOT_KILLED &&
                            (abs(mouse_x - static_cast<int>(chicken_[i].x)) < game_settings.SHOTGUN_SIZE ||
                             abs(mouse_x - static_cast<int>(chicken_[i].x) - CHICKEN_WIDTH) < game_settings.SHOTGUN_SIZE) &&
                            (abs(mouse_y - static_cast<int>(chicken_[i].y)) < game_settings.SHOTGUN_SIZE ||
                             abs(mouse_y - static_cast<int>(chicken_[i].y) - CHICKEN_HEIGHT) < game_settings.SHOTGUN_SIZE))
                        {
                            game_state.score += game_settings.POINTS_FOR_SHOTGUN;
                            chicken_[i].alive = KILLED_WITH_SHOTGUN;
                            chicken_[i].flight = chicken_[i].direction;
                            ++game_state.kills;
                            --game_state.chickens_left;

                            if (game_state.runners < game_settings.MAX_CHICKENS - 1 &&
                                rand() % game_settings.RESPAWN_RATE <= 1)
                            {
                                game_state.runners++;
                            }

                            if (rand() % game_settings.CHANCE_OF_GEM <= 1)
                            {
                                for (int j = 0; j < MAX_GEMS; ++j)
                                {
                                    if (gem_[j].active == false)
                                    {
                                        gem_[j].launch(chicken_[i].x, chicken_[i].y);
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
                    ++game_state.shots_fired;
                }
                if (fire_shotgun)
                {
                    ++game_state.shots_fired;
                }

                if (game_state.level_mode == false)
                {
                    if (game_state.tenderizers > 0)
                    {
                        if (key[KEY_SPACE])
                        {
                            for (int i = 0; i < game_settings.MAX_CHICKENS; ++i)
                            {
                                if (chicken_[i].alive == NOT_KILLED)
                                {
                                    chicken_[i].alive = KILLED_WITH_TENDERIZER;
                                    chicken_[i].flight = chicken_[i].direction;
                                    game_state.score += game_settings.POINTS_FOR_TENDERIZER;
                                    game_state.kills++;
                                }
                            }

                            play_sound(assets.sound_tenderizer, game_settings.VOLUME, 128, ONCE);
                            game_state.tenderizers--;
                        }
                    }
                }

                if (game_state.timer <= 0)
                {
                    game_state.mode = MODE_GAMEOVER;
                }

                if (key[KEY_R])
                {
                    game_state.mode = MODE_RESTART;
                    fadeout(makecol(0, 0, 0), 20);
                }

                if (game_state.level_mode == true)
                {
                    if (game_state.chickens_left < 1)
                    {
                        if (game_state.delay_of_levelend-- == 0)
                        {
                            game_state.mode = MODE_NEXTLEVEL;
                        }
                    }
                }

                if (game_state.mode == MODE_GAMEOVER)
                {
                    if (game_state.level_mode == true)
                    {
                        if (game_state.chickens_left < 1)
                        {
                            game_state.mode = MODE_PLAYING;
                        }
                    }
                }
                break;
            case MODE_NEXTLEVEL:
                fadeout(makecol(0, 0, 0), 30);
                show_levelcompleted();
                restart(chicken_, gem_, smoke_);
                next_level(game_state.current_level + 1);
                show_levelnumber();
                game_state.mode = MODE_PLAYING;
                break;
            case MODE_PAUSED:
                if (key[KEY_C])
                {
                    game_state.mode = MODE_PLAYING;
                }
                if (key[KEY_Q])
                {
                    fadeout(makecol(0, 0, 0), 30);
                    show_modechooser();
                }
                break;
            case MODE_GAMEOVER:
                if (game_state.level_mode == false && game_state.not_dead)
                {
                    rank = save_highscore(game_state.playername, game_state.score);
                    stop_sample(assets.sound_alarm);
                    game_state.not_dead = false;
                    if (rank <= HIGHSCORE_TABLE)
                    {
                        play_sound(assets.sound_highscore, game_settings.VOLUME, 128, FOREVER);
                    }
                    else
                    {
                        play_sound(assets.sound_gameover, game_settings.VOLUME, 128, ONCE);
                    }
                }
                if (key[KEY_R])
                {
                    game_state.mode = MODE_RESTART;
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

        clear(assets.buffer);
        draw_sprite(assets.buffer, assets.background, 0, 0);
        switch (game_state.mode)
        {
        case MODE_PLAYING:
            for (int i = 0; i < MAX_SMOKE; ++i)
            {
                smoke_[i].draw(render_context_);
            }
            draw_sprite(assets.buffer, level.image, 0, SCREEN_H - MAX_LEVELHEIGHT);
            for (int i = 0; i < MAX_GEMS; ++i)
            {
                gem_[i].draw(render_context_);
            }
            for (int i = 0; i < game_state.runners; ++i)
            {
                chicken_[i].draw(render_context_);
            }
            show_statistics();
            break;
        case MODE_PAUSED:
            for (int i = 0; i < SCREEN_H; i += 2)
            {
                line(assets.buffer, 0, i, SCREEN_W, i, makecol(0, 0, 60));
            }
            CHICKENS_TEXTOUT_CENTRE(
                assets.buffer, assets.font_big, "PAUSED", SCREEN_W / 2, SCREEN_H / 2 - 40, makecol(255, 255, 255));
            CHICKENS_TEXTOUT_CENTRE(assets.buffer,
                                    font,
                                    "Press 'C' to continue or 'Q' to quit",
                                    SCREEN_W / 2,
                                    SCREEN_H / 2 + 60,
                                    makecol(255, 255, 255));
            break;
        case MODE_GAMEOVER:
            rectfill(assets.buffer, 0, SCREEN_H - 40, SCREEN_W, SCREEN_H, makecol(0, 0, 0));
            CHICKENS_TEXTOUT_CENTRE(assets.buffer,
                                    assets.font_big,
                                    "Armageddon",
                                    SCREEN_W / 2,
                                    SCREEN_H / 2 - 20,
                                    makecol(255, 255, 255));
            CHICKENS_TEXTOUT_CENTRE(assets.buffer,
                                    font,
                                    "The chickens have risen. Everyone is dead. Our world is gone.",
                                    SCREEN_W / 2,
                                    SCREEN_H / 2 + 40,
                                    makecol(255, 255, 255));
            CHICKENS_TEXTOUT_CENTRE(assets.buffer,
                                    font,
                                    "Press 'R' to play again!",
                                    SCREEN_W / 2,
                                    SCREEN_H - 35,
                                    makecol(0, 255, 0));
            CHICKENS_TEXTOUT_CENTRE(assets.buffer,
                                    font,
                                    "Not that this is a game. This is actually happening, in real life.",
                                    SCREEN_W / 2,
                                    SCREEN_H - 20,
                                    makecol(200, 200, 200));
            if (rank <= HIGHSCORE_TABLE)
            {
                CHICKENS_TEXTPRINTF_CENTRE(assets.buffer,
                                           assets.font_interface,
                                           SCREEN_W / 2,
                                           SCREEN_H / 2 + 65,
                                           makecol(0, 220, 0),
                                           "But you did get a High Score of %d points in rank %d!",
                                           game_state.score,
                                           rank);
            }
            break;
        default:
            break;
        }
        draw_sprite(assets.buffer, mouse_sprite, mx - 11, my - 11);
        blit(assets.buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
        while (game_time <= 0)
        {
        }
    } while (game_state.mode != MODE_QUIT);

    show_highscores(rank);
}
