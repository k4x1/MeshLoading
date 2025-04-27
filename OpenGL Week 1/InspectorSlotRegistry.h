#pragma once
#include "AssetManager.h"
#include <functional>
#include <unordered_map>
#include <vector>
#include <string>
#include <typeindex>
#include "EnginePluginAPI.h"

struct ENGINE_API AssetSlot {
    std::string                              label = " ";
    std::function<std::string(Component*)>   getter;
    std::function<void(Component*, const Asset&)> setter;
    std::vector<AssetType>                   acceptedTypes;
};



class ENGINE_API InspectorSlotRegistry {
public:
    template<typename Comp>
    static void RegisterSlot(
        std::string label,
        std::function<std::string(Comp*)>   get,
        std::function<void(Comp*, const Asset&)> set,
        std::vector<AssetType> types
    ) {
        auto wrapGet = [get](Component* c) {
            return get(static_cast<Comp*>(c));
            };
        auto wrapSet = [set](Component* c, const Asset& a) {
            set(static_cast<Comp*>(c), a);
            };
        slots()[std::type_index(typeid(Comp))].push_back({ label, wrapGet, wrapSet, types });
    }

    static const std::vector<AssetSlot>& GetSlotsFor(Component* c) {
        return slots()[std::type_index(typeid(*c))];
    }
private:
    static std::unordered_map<std::type_index, std::vector<AssetSlot>>& slots() {
        static std::unordered_map<std::type_index, std::vector<AssetSlot>> m;
        return m;
    }
};
