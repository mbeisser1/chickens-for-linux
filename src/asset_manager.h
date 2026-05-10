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

class AssetManager
{
  public:
    bool load_datafile(const std::string& key, const char* path);
    DATAFILE* get_datafile(const std::string& key);
    const DATAFILE* get_datafile(const std::string& key) const;
    void clear_datafiles();

  private:
    std::unordered_map<std::string, DatafilePtr> datafiles_;
};

#endif
