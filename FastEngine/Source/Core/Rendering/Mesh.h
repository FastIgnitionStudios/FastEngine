#pragma once
#include "Assets/AssetTypes/MeshAsset.h"

namespace Engine
{
    struct MeshComponent
    {
        UUID id {UUID()};
        Ref<MeshAsset> mesh;
    };

    
}
