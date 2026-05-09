#include "settings.h"

using namespace std;

int load_config(char* path)
{
    fstream file(path);

    string key;
    char comment[255];

    while (file >> key)
    {
        if (key[0] != '#')
        {
            if (key == "CHANCE_OF_FLIGHT")
            {
                file >> CHANCE_OF_FLIGHT;
            }
            if (key == "CHANCE_OF_GEM")
            {
                file >> CHANCE_OF_GEM;
            }
            if (key == "CHICKEN_SPEED")
            {
                file >> CHICKEN_SPEED;
            }
            if (key == "FULLSCREEN")
            {
                file >> FULLSCREEN;
            }
            if (key == "GAME_SPEED_OFFSET")
            {
                file >> GAME_SPEED_OFFSET;
            }
            if (key == "BLOOD_PER_CHUNK")
            {
                file >> BLOOD_PER_CHUNK;
            }
            if (key == "CHUNKS_PER_CHICKEN")
            {
                file >> CHUNKS_PER_CHICKEN;
            }
            if (key == "GRAVITY")
            {
                file >> GRAVITY;
            }
            if (key == "MAX_CHICKENS")
            {
                file >> MAX_CHICKENS;
            }
            if (key == "MUTE")
            {
                file >> MUTE;
            }
            if (key == "POINTS_FOR_ROCKET")
            {
                file >> POINTS_FOR_ROCKET;
            }
            if (key == "POINTS_FOR_SHOTGUN")
            {
                file >> POINTS_FOR_SHOTGUN;
            }
            if (key == "POINTS_FOR_TENDERIZER")
            {
                file >> POINTS_FOR_TENDERIZER;
            }
            if (key == "INITIAL_CHICKENS")
            {
                file >> INITIAL_CHICKENS;
            }
            if (key == "RESPAWN_RATE")
            {
                file >> RESPAWN_RATE;
            }
            if (key == "ROCKET_RELOAD")
            {
                file >> ROCKET_RELOAD;
            }
            if (key == "ROCKET_SIZE")
            {
                file >> ROCKET_SIZE;
            }
            if (key == "SHOTGUN_RELOAD")
            {
                file >> SHOTGUN_RELOAD;
            }
            if (key == "SHOTGUN_SIZE")
            {
                file >> SHOTGUN_SIZE;
            }
            if (key == "SMOKE_LINGERING")
            {
                file >> SMOKE_LINGERING;
            }
            if (key == "SMOKE_PUFFS")
            {
                file >> SMOKE_PUFFS;
            }
            if (key == "TIMER")
            {
                file >> TIMER;
            }
            if (key == "TRANSLUCENT_SMOKE")
            {
                file >> TRANSLUCENT_SMOKE;
            }
            if (key == "VOLUME")
            {
                file >> VOLUME;
            }
        }
        else
        {
            file.getline(comment, 255);
        }
    }

    file.close();

    return 0;
}

int load_stock_config()
{
    // Load the default settings!

    BLOOD_PER_CHUNK = 30;
    CHANCE_OF_FLIGHT = 2000;
    CHANCE_OF_GEM = 25;
    CHICKEN_SPEED = 5;
    CHUNKS_PER_CHICKEN = 20;
    FULLSCREEN = 0;
    GAME_SPEED_OFFSET = 0;
    GRAVITY = 0.7;
    MAX_CHICKENS = 80;
    MUTE = 0;
    POINTS_FOR_ROCKET = 100;
    POINTS_FOR_SHOTGUN = 250;
    POINTS_FOR_TENDERIZER = 100;
    INITIAL_CHICKENS = 4;
    RESPAWN_RATE = 6;
    ROCKET_RELOAD = 10;
    ROCKET_SIZE = 50;
    SHOTGUN_RELOAD = 55;
    SHOTGUN_SIZE = 40;
    SMOKE_LINGERING = 30;
    SMOKE_PUFFS = 20;
    TIMER = 120;
    TRANSLUCENT_SMOKE = 1;
    VOLUME = 255;

    return 0;
}
