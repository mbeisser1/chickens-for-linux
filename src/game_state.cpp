#include <cstdlib>

#include "game_state.h"

GameState::GameState(const Settings& settings)
{
    const char* user = std::getenv("USER");
    playername = (user != nullptr) ? user : "player";
    apply_settings(settings);
}
