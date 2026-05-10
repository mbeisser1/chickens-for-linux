#include "asset_manager.h"

#include "helper.h"

namespace
{
bool bitmap_belongs_to_datafile(const DATAFILE* datafile, const BITMAP* bitmap)
{
    if (datafile == nullptr || bitmap == nullptr)
    {
        return false;
    }

    const int item_count = AssetManager::items_in_datafile(datafile);
    for (int i = 0; i < item_count; ++i)
    {
        if (static_cast<const BITMAP*>(datafile[i].dat) == bitmap)
        {
            return true;
        }
    }

    return false;
}
} // namespace

AssetManager::~AssetManager()
{
    clear();
}

void AssetManager::clear()
{
    if (app_assets_.buffer != nullptr)
    {
        destroy_bitmap(app_assets_.buffer);
        app_assets_.buffer = nullptr;
    }

    if (app_assets_.background != nullptr &&
        !bitmap_belongs_to_datafile(app_assets_.background_data, app_assets_.background))
    {
        destroy_bitmap(app_assets_.background);
        app_assets_.background = nullptr;
    }

    clear_samples();
    clear_datafiles();
    app_assets_ = {};
}

bool AssetManager::load_datafile(const std::string& key, const char* path)
{
    DATAFILE* raw = ::load_datafile(path);
    if (raw == nullptr)
    {
        return false;
    }

    datafiles_[key] = DatafilePtr(raw);
    return true;
}

DATAFILE* AssetManager::get_datafile(const std::string& key)
{
    const auto it = datafiles_.find(key);
    if (it == datafiles_.end())
    {
        return nullptr;
    }

    return it->second.get();
}

const DATAFILE* AssetManager::get_datafile(const std::string& key) const
{
    const auto it = datafiles_.find(key);
    if (it == datafiles_.end())
    {
        return nullptr;
    }

    return it->second.get();
}

void AssetManager::clear_datafiles()
{
    datafiles_.clear();
    fonts_.clear();
}

bool AssetManager::register_font(const std::string& key, const DATAFILE* source_datafile, int index)
{
    if (source_datafile == nullptr || index < 0)
    {
        return false;
    }

    fonts_[key] = static_cast<FONT*>(source_datafile[index].dat);
    return fonts_[key] != nullptr;
}

FONT* AssetManager::get_font(const std::string& key)
{
    const auto it = fonts_.find(key);
    if (it == fonts_.end())
    {
        return nullptr;
    }

    return it->second;
}

const FONT* AssetManager::get_font(const std::string& key) const
{
    const auto it = fonts_.find(key);
    if (it == fonts_.end())
    {
        return nullptr;
    }

    return it->second;
}

void AssetManager::clear_fonts()
{
    fonts_.clear();
}

bool AssetManager::load_sample(const std::string& key, const char* path)
{
    SAMPLE* raw = ::load_sample(path);
    if (raw == nullptr)
    {
        return false;
    }

    samples_[key] = SamplePtr(raw);
    return true;
}

SAMPLE* AssetManager::get_sample(const std::string& key)
{
    const auto it = samples_.find(key);
    if (it == samples_.end())
    {
        return nullptr;
    }

    return it->second.get();
}

const SAMPLE* AssetManager::get_sample(const std::string& key) const
{
    const auto it = samples_.find(key);
    if (it == samples_.end())
    {
        return nullptr;
    }

    return it->second.get();
}

void AssetManager::clear_samples()
{
    samples_.clear();
}

bool AssetManager::load_app_assets()
{
    bool success{true};
    clear();

    success &= load_datafile("background", CHICKENS_ASSETS_REL("dat/background.dat"));
    success &= load_datafile("bigchicken", CHICKENS_ASSETS_REL("dat/bigchicken.dat"));
    success &= load_datafile("chicken", CHICKENS_ASSETS_REL("dat/chicken.dat"));
    success &= load_datafile("cursors", CHICKENS_ASSETS_REL("dat/cursors.dat"));
    success &= load_datafile("flyingchicken", CHICKENS_ASSETS_REL("dat/flyingchicken.dat"));
    success &= load_datafile("fonts", CHICKENS_ASSETS_REL("dat/fonts.dat"));
    success &= load_datafile("gem", CHICKENS_ASSETS_REL("dat/gem.dat"));
    success &= load_datafile("giblet", CHICKENS_ASSETS_REL("dat/giblets.dat"));
    success &= load_datafile("icons", CHICKENS_ASSETS_REL("dat/icons.dat"));
    success &= load_datafile("modechooser", CHICKENS_ASSETS_REL("dat/modechooser.dat"));
    success &= load_datafile("terrain", CHICKENS_ASSETS_REL("dat/terrain.dat"));

    app_assets_.background_data = get_datafile("background");
    app_assets_.bigchicken_data = get_datafile("bigchicken");
    app_assets_.chicken_data = get_datafile("chicken");
    app_assets_.cursors_data = get_datafile("cursors");
    app_assets_.flyingchicken_data = get_datafile("flyingchicken");
    app_assets_.fonts_data = get_datafile("fonts");
    app_assets_.gem_data = get_datafile("gem");
    app_assets_.giblet_data = get_datafile("giblet");
    app_assets_.icons_data = get_datafile("icons");
    app_assets_.modechooser_data = get_datafile("modechooser");
    app_assets_.terrain_data = get_datafile("terrain");

    success &= register_font("font", app_assets_.fonts_data, 0);
    success &= register_font("font_big", app_assets_.fonts_data, 1);
    success &= register_font("font_interface", app_assets_.fonts_data, 2);
    app_assets_.font = get_font("font");
    app_assets_.font_big = get_font("font_big");
    app_assets_.font_interface = get_font("font_interface");

    success &= load_sample("sound_alarm", CHICKENS_ASSETS_REL("sound/alarm.wav"));
    success &= load_sample("sound_gameover", CHICKENS_ASSETS_REL("sound/gameover.wav"));
    success &= load_sample("sound_gemcollect", CHICKENS_ASSETS_REL("sound/gemcollect.wav"));
    success &= load_sample("sound_highscore", CHICKENS_ASSETS_REL("sound/highscore.wav"));
    success &= load_sample("sound_menu", CHICKENS_ASSETS_REL("sound/menu.wav"));
    success &= load_sample("sound_rocket", CHICKENS_ASSETS_REL("sound/rocket.wav"));
    success &= load_sample("sound_shotgun", CHICKENS_ASSETS_REL("sound/shotgun.wav"));
    success &= load_sample("sound_tenderizer", CHICKENS_ASSETS_REL("sound/tenderizer.wav"));

    app_assets_.sound_alarm = get_sample("sound_alarm");
    app_assets_.sound_gameover = get_sample("sound_gameover");
    app_assets_.sound_gemcollect = get_sample("sound_gemcollect");
    app_assets_.sound_highscore = get_sample("sound_highscore");
    app_assets_.sound_menu = get_sample("sound_menu");
    app_assets_.sound_rocket = get_sample("sound_rocket");
    app_assets_.sound_shotgun = get_sample("sound_shotgun");
    app_assets_.sound_tenderizer = get_sample("sound_tenderizer");

    if (!success)
    {
        return false;
    }

    app_assets_.buffer = create_system_bitmap(SCREEN_W, SCREEN_H);
    app_assets_.background = create_bitmap(SCREEN_W, SCREEN_H);
    if (app_assets_.buffer == nullptr || app_assets_.background == nullptr)
    {
        return false;
    }

    return true;
}

AppAssets& AssetManager::assets()
{
    return app_assets_;
}

const AppAssets& AssetManager::assets() const
{
    return app_assets_;
}

int AssetManager::items_in_datafile(const DATAFILE* data)
{
    int len = 0;
    while (data[len].type != DAT_END)
    {
        len++;
    }

    return len - 1;
}
