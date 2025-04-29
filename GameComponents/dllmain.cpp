#include <reactphysics3d/reactphysics3d.h>
#include <imgui.h>
#include <Windows.h>
#include "All.h"


extern "C" ENGINE_API void SetImGuiContext(ImGuiContext* context);


void SetImGuiContext(ImGuiContext* context)
{
    ImGui::SetCurrentContext(context);
}
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
    
