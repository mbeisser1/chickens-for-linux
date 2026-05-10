#include "settings.h"

void load_config(const char* path)
{
    std::fstream file(path);

    std::string key;
    char comment[255];

    while (file >> key)
    {
        if (key[0] != '#')
        {
            if (key == "CHANCE_OF_FLIGHT")
            {
                file >> game_settings.CHANCE_OF_FLIGHT;
            }
            if (key == "CHANCE_OF_GEM")
            {
                file >> game_settings.CHANCE_OF_GEM;
            }
            if (key == "CHICKEN_SPEED")
            {
                file >> game_settings.CHICKEN_SPEED;
            }
            if (key == "FULLSCREEN")
            {
                file >> game_settings.FULLSCREEN;
            }
            if (key == "GAME_SPEED_OFFSET")
            {
                file >> game_settings.GAME_SPEED_OFFSET;
            }
            if (key == "BLOOD_PER_CHUNK")
            {
                file >> game_settings.BLOOD_PER_CHUNK;
            }
            if (key == "CHUNKS_PER_CHICKEN")
            {
                file >> game_settings.CHUNKS_PER_CHICKEN;
            }
            if (key == "GRAVITY")
            {
                file >> game_settings.GRAVITY;
            }
            if (key == "MAX_CHICKENS")
            {
                file >> game_settings.MAX_CHICKENS;
            }
            if (key == "MUTE")
            {
                file >> game_settings.MUTE;
            }
            if (key == "POINTS_FOR_ROCKET")
            {
                file >> game_settings.POINTS_FOR_ROCKET;
            }
            if (key == "POINTS_FOR_SHOTGUN")
            {
                file >> game_settings.POINTS_FOR_SHOTGUN;
            }
            if (key == "POINTS_FOR_TENDERIZER")
            {
                file >> game_settings.POINTS_FOR_TENDERIZER;
            }
            if (key == "INITIAL_CHICKENS")
            {
                file >> game_settings.INITIAL_CHICKENS;
            }
            if (key == "RESPAWN_RATE")
            {
                file >> game_settings.RESPAWN_RATE;
            }
            if (key == "ROCKET_RELOAD")
            {
                file >> game_settings.ROCKET_RELOAD;
            }
            if (key == "ROCKET_SIZE")
            {
                file >> game_settings.ROCKET_SIZE;
            }
            if (key == "SHOTGUN_RELOAD")
            {
                file >> game_settings.SHOTGUN_RELOAD;
            }
            if (key == "SHOTGUN_SIZE")
            {
                file >> game_settings.SHOTGUN_SIZE;
            }
            if (key == "SMOKE_LINGERING")
            {
                file >> game_settings.SMOKE_LINGERING;
            }
            if (key == "SMOKE_PUFFS")
            {
                file >> game_settings.SMOKE_PUFFS;
            }
            if (key == "TIMER")
            {
                file >> game_settings.TIMER;
            }
            if (key == "TRANSLUCENT_SMOKE")
            {
                file >> game_settings.TRANSLUCENT_SMOKE;
            }
            if (key == "VOLUME")
            {
                file >> game_settings.VOLUME;
            }
        }
        else
        {
            file.getline(comment, 255);
        }
    }

    file.close();
}

void load_stock_config()
{
    game_settings = Settings{};
}
