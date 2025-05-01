#pragma once
#include "All.h"
#include <nlohmann/json.hpp>


class TagComponent : public ISerializableComponent {
public:
    std::string tag;

    TagComponent() = default;
    ~TagComponent() override = default;

    nlohmann::json Serialize() const override {
        return { {"tag", tag} };
    }
    void Deserialize(const nlohmann::json& j) override {
        tag = j.value("tag", tag);
    }
    void OnInspectorGUI() override
    {
        static char tagBuf[64];
        strncpy_s(tagBuf, tag.c_str(), sizeof(tagBuf));
        tagBuf[sizeof(tagBuf) - 1] = '\0';

        if (ImGui::InputText("Prefab Path", tagBuf, sizeof(tagBuf))) {
            tag = tagBuf;
        }

    }

};

REGISTER_SERIALIZABLE_COMPONENT(TagComponent);