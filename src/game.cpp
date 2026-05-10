#include "game.h"

#include <cstdlib>
#include <ctime>
#include <stdexcept>

#include <allegro.h>

#include "game_flow.h"
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

Game::Game(AppContext& app) : ctx_(app)
{
    init_allegro(ctx_.settings);
    set_gfx_mode(ctx_.settings);
    load_datafiles(ctx_.asset_manager);

    AppAssets& loaded = ctx_.assets();
    ctx_.render_context = RenderContext{
        loaded.buffer, loaded.gem_data, loaded.icons_data, loaded.giblet_data};

    set_mouse_sprite(static_cast<BITMAP*>(ctx_.assets().cursors_data[0].dat));

    for (auto& puff : smoke_)
    {
        puff.bind_level(&terrain_);
        puff.bind_settings(&ctx_.settings);
    }
    for (auto& rock : gem_)
    {
        rock.bind_settings(&ctx_.settings);
    }
    for (auto& hen : chicken_)
    {
        hen.load_sprites(ctx_.assets());
        hen.bind_level(&terrain_);
        hen.bind_settings(&ctx_.settings);
        hen.reset();
    }
}

void Game::show_highscores(int player_rank_one_based)
{
    ::show_highscores(player_rank_one_based - 1, ctx_.assets().buffer, ctx_.assets().background, ctx_.assets());
}

void Game::tick_mode_restart(AppContext& c)
{
    restart(chicken_, gem_, smoke_, terrain_, c);
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
        if (chicken_[i].run() == CROSSED_THE_ROAD)
        {
            c.state.mode = MODE_GAMEOVER;
        }

        if (fire_rocket)
        {
            if (chicken_[i].alive == NOT_KILLED &&
                ((SCREEN_H - chicken_[i].y) - terrain_.height[mouse_x]) < c.settings.ROCKET_SIZE &&
                (abs(mouse_x - static_cast<int>(chicken_[i].x)) < c.settings.ROCKET_SIZE ||
                 abs(mouse_x - static_cast<int>(chicken_[i].x) - CHICKEN_WIDTH) < c.settings.ROCKET_SIZE))
            {
                c.state.score += c.settings.POINTS_FOR_ROCKET;
                chicken_[i].alive = KILLED_WITH_ROCKET;
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
                (abs(mouse_x - static_cast<int>(chicken_[i].x)) < c.settings.SHOTGUN_SIZE ||
                 abs(mouse_x - static_cast<int>(chicken_[i].x) - CHICKEN_WIDTH) < c.settings.SHOTGUN_SIZE) &&
                (abs(mouse_y - static_cast<int>(chicken_[i].y)) < c.settings.SHOTGUN_SIZE ||
                 abs(mouse_y - static_cast<int>(chicken_[i].y) - CHICKEN_HEIGHT) < c.settings.SHOTGUN_SIZE))
            {
                c.state.score += c.settings.POINTS_FOR_SHOTGUN;
                chicken_[i].alive = KILLED_WITH_SHOTGUN;
                chicken_[i].flight = chicken_[i].direction;
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
                    if (chicken_[i].alive == NOT_KILLED)
                    {
                        chicken_[i].alive = KILLED_WITH_TENDERIZER;
                        chicken_[i].flight = chicken_[i].direction;
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
    show_levelcompleted(c);
    restart(chicken_, gem_, smoke_, terrain_, c);
    next_level(c, c.state.current_level + 1);
    show_levelnumber(c);
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
        show_modechooser(c);
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
        show_modechooser(c);
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
        chicken_[i].draw(c.render_context);
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
    show_startup(c);
    fadeout(c, makecol(0, 0, 0), 50);
    show_modechooser(c);

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
