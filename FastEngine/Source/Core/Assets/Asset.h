#pragma once
#include "AssetCache.h"
#include "Filesystem.h"

#include "Core.h"

#define CHECK_ASSET_LOADED(asset) if (!(asset)->isLoaded) { ENGINE_CORE_ERROR("Asset {0} is not loaded", (asset)->AssetPath); }

namespace Engine
{
    

    
    
    class Asset : public Engine
    {
    public:
        Asset() = default;
        virtual ~Asset() = default;

        virtual void Load() = 0;
        virtual void Unload() = 0;

        bool IsLoaded() const { return isLoaded; }

        /// Get the path to the asset file on disk
        /// @return Absolute path to asset file if AssetPath is valid (not guaranteed)
        std::filesystem::path GetAssetPath() const { return AssetPath; }
        
        
    protected:
        bool isLoaded;
        std::filesystem::path AssetPath;
        
    };
}
