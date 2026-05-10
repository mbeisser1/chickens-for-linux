#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include <memory>
#include <string>
#include <unordered_map>

#include <allegro.h>

template <typename T, void (*DestroyFn)(T*)>
struct AllegroDeleter
{
    void operator()(T* ptr) const
    {
        if (ptr != nullptr)
        {
            DestroyFn(ptr);
        }
    }
};

using DatafilePtr = std::unique_ptr<DATAFILE, AllegroDeleter<DATAFILE, unload_datafile>>;
using SamplePtr = std::unique_ptr<SAMPLE, AllegroDeleter<SAMPLE, destroy_sample>>;
using BitmapPtr = std::unique_ptr<BITMAP, AllegroDeleter<BITMAP, destroy_bitmap>>;

struct AppAssets
{
    DATAFILE* background_data{};
    DATAFILE* bigchicken_data{};
    DATAFILE* chicken_data{};
    DATAFILE* cursors_data{};
    DATAFILE* flyingchicken_data{};
    DATAFILE* fonts_data{};
    DATAFILE* gem_data{};
    DATAFILE* giblet_data{};
    DATAFILE* icons_data{};
    DATAFILE* modechooser_data{};
    DATAFILE* terrain_data{};
    FONT* font{};
    FONT* font_big{};
    FONT* font_interface{};
    SAMPLE* sound_alarm{};
    SAMPLE* sound_gameover{};
    SAMPLE* sound_gemcollect{};
    SAMPLE* sound_highscore{};
    SAMPLE* sound_menu{};
    SAMPLE* sound_rocket{};
    SAMPLE* sound_shotgun{};
    SAMPLE* sound_tenderizer{};
};

class AssetManager
{
  public:
    bool load_datafile(const std::string& key, const char* path);
    DATAFILE* get_datafile(const std::string& key);
    const DATAFILE* get_datafile(const std::string& key) const;
    void clear_datafiles();
    bool register_font(const std::string& key, const DATAFILE* source_datafile, int index);
    FONT* get_font(const std::string& key);
    const FONT* get_font(const std::string& key) const;
    void clear_fonts();
    bool load_sample(const std::string& key, const char* path);
    SAMPLE* get_sample(const std::string& key);
    const SAMPLE* get_sample(const std::string& key) const;
    void clear_samples();
    const AppAssets& load_app_assets();
    const AppAssets& app_assets() const;

  private:
    std::unordered_map<std::string, DatafilePtr> datafiles_;
    std::unordered_map<std::string, FONT*> fonts_;
    std::unordered_map<std::string, SamplePtr> samples_;
    AppAssets app_assets_{};
};

#endif
