#pragma once

#include "EnginePluginAPI.h"

class ENGINE_API  IInspectable {
public:
    virtual void OnInspectorGUI() = 0;
    virtual ~IInspectable() = default;
};
