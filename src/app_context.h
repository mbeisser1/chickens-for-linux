#ifndef APP_CONTEXT_H
#define APP_CONTEXT_H

#include "asset_manager.h"
#include "game_state.h"
#include "settings.h"

/** Non-owning bundle for the main session; storage lives in `main.cpp`. */
struct AppContext
{
    Settings& settings;
    GameState& state;
    AssetManager& asset_manager;

    AppAssets& assets() { return asset_manager.assets(); }
};

#endif
