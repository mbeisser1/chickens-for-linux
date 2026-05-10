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
    BITMAP* background{};
    BITMAP* buffer{};
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

/** Draw-time view of bitmap + datafile pointers (built from loaded `AppAssets`). */
struct RenderContext
{
    BITMAP* target{};
    const DATAFILE* gem_data{};
    const DATAFILE* icons_data{};
    const DATAFILE* giblet_data{};
};

class AssetManager
{
  public:
    /** Count of usable objects in an Allegro `DATAFILE` (excludes `DAT_END` sentinel). */
    static int items_in_datafile(const DATAFILE* data);

    ~AssetManager();
    bool load_app_assets();
    void clear();
    AppAssets& assets();
    const AppAssets& assets() const;

  private:
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

    std::unordered_map<std::string, DatafilePtr> datafiles_;
    std::unordered_map<std::string, FONT*> fonts_;
    std::unordered_map<std::string, SamplePtr> samples_;
    AppAssets app_assets_{};
};

#endif
