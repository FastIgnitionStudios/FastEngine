#pragma once
#include "Core.h"

namespace Engine
{
    class Asset;
    
    class AssetCache : public Engine
    {
    public:

        static Ref<AssetCache> CreateAssetCache();
        static Ref<AssetCache> GetPrimaryDataCache();

        bool CacheAsset(Ref<Asset> asset);


    private:
        static Ref<AssetCache> primaryDataCache;
        static std::optional<std::vector<AssetCache>> secondaryDataCaches;
    };
}
