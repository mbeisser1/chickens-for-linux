#include "asset_manager.h"

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
}
