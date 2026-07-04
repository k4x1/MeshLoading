#pragma once
#include "../EnginePluginAPI.h"
class Scene;
class GameObject;
class FrameBuffer;

enum class EditorState;

struct ENGINE_API  EditorContext
{
    Scene* scene = nullptr;
    GameObject* selectedGameObject = nullptr;
    GameObject* editorCamera = nullptr;
    GLFWwindow* window = nullptr;
    FrameBuffer* editorFrameBuffer = nullptr;
    FrameBuffer* gameFrameBuffer = nullptr;
    EditorState* editorState = nullptr;

    float deltaTime = 0.0f;
};