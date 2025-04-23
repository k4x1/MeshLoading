#include "ComponentFactory.h"
//
//ComponentFactory& ComponentFactory::Instance() {
//    static ComponentFactory inst;
//    return inst;
//}
//
//void ComponentFactory::Register(const std::string& typeName,
//    CompCreator    creator,
//    CompSerializer serializer)
//{
//    registry[typeName] = { std::move(creator), std::move(serializer) };
//}
//
//Component* ComponentFactory::Create(const std::string& typeName,
//    const nlohmann::json& params,
//    GameObject* owner) const
//{
//    auto it = registry.find(typeName);
//    if (it == registry.end()) return nullptr;
//    return it->second.create(params, owner);
//}
//
//nlohmann::json ComponentFactory::Serialize(Component* comp) const {
//    for (auto& [typeName, entry] : registry) {
//        auto j = entry.serialize(comp);
//        if (!j.is_null()) {
//            j["type"] = typeName;
//            return j;
//        }
//    }
//    return nullptr;
//}
//
//const std::map<std::string, CompEntry>&
//ComponentFactory::GetRegistry() const
//{
//    return registry;
//}
