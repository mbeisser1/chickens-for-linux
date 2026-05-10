#ifndef SETTINGS_H
#define SETTINGS_H

constexpr int MAX_CHICKENS_CAPACITY{512};

struct Settings
{
    Settings() = default;
    explicit Settings(const char* path);

    bool FULLSCREEN{false};
    bool MUTE{false};
    bool TRANSLUCENT_SMOKE{true};
    float GRAVITY{0.7F};
    int BLOOD_PER_CHUNK{30};
    int CHANCE_OF_FLIGHT{2000};
    int CHANCE_OF_GEM{25};
    float CHICKEN_SPEED{5.0F};
    int GAME_SPEED_OFFSET{0};
    int CHUNKS_PER_CHICKEN{20};
    int MAX_CHICKENS{MAX_CHICKENS_CAPACITY};
    int POINTS_FOR_ROCKET{100};
    int POINTS_FOR_SHOTGUN{250};
    int POINTS_FOR_TENDERIZER{100};
    int INITIAL_CHICKENS{4};
    int RESPAWN_RATE{6};
    int ROCKET_RELOAD{10};
    int ROCKET_SIZE{50};
    int SHOTGUN_RELOAD{55};
    int SHOTGUN_SIZE{40};
    int SMOKE_LINGERING{30};
    int SMOKE_PUFFS{20};
    int TIMER{120};
    int VOLUME{255};
};

#endif
