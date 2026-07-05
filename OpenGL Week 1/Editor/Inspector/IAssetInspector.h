#pragma once

#include "../../EnginePluginAPI.h"
#include "../../ObjectSystem/AssetManager.h"

struct EditorContext;

class ENGINE_API IAssetInspector
{
public:
    virtual ~IAssetInspector() = default;

    virtual bool CanInspect(const Asset& asset) const = 0;

    virtual void Draw(
        EditorContext& context,
        const Asset& asset
    ) = 0;
};