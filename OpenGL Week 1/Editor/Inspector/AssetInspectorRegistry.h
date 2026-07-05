#pragma once

#include "../../EnginePluginAPI.h"
#include "IAssetInspector.h"

#include <memory>
#include <string>

struct EditorContext;

using AssetInspectorFactory = std::unique_ptr<IAssetInspector>(*)();

class ENGINE_API AssetInspectorRegistry
{
public:
    static void RegisterInspector(
        const std::string& inspectorName,
        AssetInspectorFactory factory
    );

    static IAssetInspector* GetInspectorForAsset(const Asset& asset);

    static bool DrawInspectorForAsset(
        EditorContext& context,
        const Asset& asset
    );
};
#define REGISTER_ASSET_INSPECTOR(InspectorType)                                   \
    namespace                                                                     \
    {                                                                             \
        std::unique_ptr<IAssetInspector> Create##InspectorType()                  \
        {                                                                         \
            return std::make_unique<InspectorType>();                             \
        }                                                                         \
                                                                                  \
        struct InspectorType##AssetInspectorRegistration                          \
        {                                                                         \
            InspectorType##AssetInspectorRegistration()                           \
            {                                                                     \
                AssetInspectorRegistry::RegisterInspector(                        \
                    #InspectorType,                                               \
                    &Create##InspectorType                                        \
                );                                                                \
            }                                                                     \
        };                                                                        \
                                                                                  \
        static InspectorType##AssetInspectorRegistration                          \
            s_##InspectorType##AssetInspectorRegistration;                        \
    }