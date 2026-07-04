#pragma once

#include "../EnginePluginAPI.h"
#include "IEditorWindow.h"

#include <memory>
#include <string>
#include <vector>

using EditorWindowFactory = std::unique_ptr<IEditorWindow>(*)();

class ENGINE_API EditorWindowRegistry
{
public:
    static void RegisterWindow(
        const std::string& windowName,
        EditorWindowFactory factory
    );

    static std::vector<std::unique_ptr<IEditorWindow>> CreateWindows();
};
#define REGISTER_EDITOR_WINDOW(WindowType)                                     \
    namespace                                                                  \
    {                                                                          \
        std::unique_ptr<IEditorWindow> Create##WindowType()                    \
        {                                                                      \
            return std::make_unique<WindowType>();                             \
        }                                                                      \
                                                                               \
        struct WindowType##EditorWindowRegistration                            \
        {                                                                      \
            WindowType##EditorWindowRegistration()                             \
            {                                                                  \
                EditorWindowRegistry::RegisterWindow(                          \
                    #WindowType,                                               \
                    &Create##WindowType                                        \
                );                                                             \
            }                                                                  \
        };                                                                     \
                                                                               \
        static WindowType##EditorWindowRegistration                            \
            s_##WindowType##EditorWindowRegistration;                          \
    }