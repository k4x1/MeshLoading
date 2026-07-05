#pragma once

#include "../../EnginePluginAPI.h"
#include "InspectorPropertyList.h"

class ENGINE_API InspectorPropertyDrawer
{
public:
    static bool DrawProperties(const InspectorPropertyList& propertyList);

private:
    static bool DrawProperty(const InspectorProperty& property);

    static bool DrawStringProperty(const InspectorProperty& property);
    static bool DrawAssetPathProperty(const InspectorProperty& property);
};