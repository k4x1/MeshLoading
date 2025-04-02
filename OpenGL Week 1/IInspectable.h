#pragma once

class IInspectable {
public:
    virtual void OnInspectorGUI() = 0;
    virtual ~IInspectable() = default;
};
