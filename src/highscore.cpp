#include "helper.h"
#include "highscore.h"
#include "asset_manager.h"

void show_highscores(int rank, BITMAP* target, BITMAP* background, AppAssets& app_assets)
{
    std::fstream file(CHICKENS_ASSETS_REL("HighScores"));

    std::string players[HIGHSCORE_TABLE];
    int scores[HIGHSCORE_TABLE];

    for (int i = 0; i < HIGHSCORE_TABLE; ++i)
    {
        file >> players[i];
        file >> scores[i];
    }

    file.close();

    do
    {
        clear(target);
        draw_sprite(target, background, 0, 0);

        CHICKENS_TEXTOUT_CENTRE(
            target, app_assets.font_big, "High Scores", SCREEN_W / 2, 5, makecol(220, 0, 0));

        for (int i = 0; i < HIGHSCORE_TABLE; ++i)
        {
            int d = 255 - (i * 20);
            int m = i * 50;
            int col = makecol(d, d, d);

            if (i == rank)
            {
                col = makecol(0, 255, 0);
            }

            CHICKENS_TEXTPRINTF(target,
                                app_assets.font_interface,
                                130 + (i > 8 ? 20 : 0),
                                75 + m,
                                col,
                                "%s",
                                players[i].c_str());
            CHICKENS_TEXTPRINTF(target,
                                app_assets.font_big,
                                100,
                                70 + m,
                                makecol(d, d, 0),
                                "%d",
                                i + 1);
            CHICKENS_TEXTPRINTF(
                target, font, 130 + (i > 8 ? 20 : 0), 105 + m, makecol(d, d, 0), "%d", scores[i]);
        }

        draw_sprite(target, mouse_sprite, mouse_x, mouse_y);
        blit(target, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

    } while (!key[KEY_ESC] && !key[KEY_SPACE] && !key[KEY_ENTER] && mouse_b != 1);
}

int save_highscore(const char* name, int score)
{
    std::fstream file(CHICKENS_ASSETS_REL("HighScores"));

    std::string players[HIGHSCORE_TABLE];
    int scores[HIGHSCORE_TABLE];

    int rank = 0;

    for (int i = 0; i < HIGHSCORE_TABLE; ++i)
    {
        file >> players[i];
        file >> scores[i];

        if (scores[i] >= score)
        {
            ++rank;
        }
    }

    file.close();

    if (rank < HIGHSCORE_TABLE)
    {
        for (int i = HIGHSCORE_TABLE - 1; i > rank; --i)
        {
            players[i] = players[i - 1];
            scores[i] = scores[i - 1];
        }

        players[rank] = name;
        scores[rank] = score;

        file.open(CHICKENS_ASSETS_REL("HighScores"), std::ios::out);

        for (int i = 0; i < HIGHSCORE_TABLE; ++i)
        {
            file << players[i] << "\n";
            file << scores[i] << "\n";
        }

        file.close();
    }

    return rank + 1;
}
