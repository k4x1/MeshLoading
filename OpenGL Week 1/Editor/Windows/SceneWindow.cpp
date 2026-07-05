#include "SceneWindow.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "Editor/EditorWindowRegistry.h"
#include "../EditorUI.h"
#include "../../Rendering/FrameBuffer.h"
#include "../../Scene/Scene.h"
#include "../../ObjectSystem/GameObject.h"
#include "../../Rendering/Camera.h"
#include "../../ObjectSystem/AssetManager.h"
#include "../../ObjectSystem/PrefabSystem.h"
#include "../InspectorSlotRegistry.h"
#include "../../Utils/Debug.h"
#include <type_traits>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include "../CameraMovement.h"

REGISTER_EDITOR_WINDOW(SceneWindow);

AspectRatio SceneWindow::g_SceneAspect = AspectRatio::R16_9;

void SceneWindow::Draw(EditorContext& context)
{
    FrameBuffer* editorFB = context.editorFrameBuffer;
    GameObject* editorCamera = context.editorCamera;
    Scene* scene = context.scene;
    GameObject* selected = nullptr;
    if (context.selectedGameObject != nullptr)
    {
        selected = *context.selectedGameObject;
    }
    
    float       deltaTime = context.deltaTime;

    ImGui::Begin("Scene View");
    {
        static const char* names[] = { "Free","16:9","4:3","1:1" };
        int idx = (int)g_SceneAspect;
        if (ImGui::Combo("Aspect", &idx, names, IM_ARRAYSIZE(names))) {
            g_SceneAspect = (AspectRatio)idx;
        }
    }

    isHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
    RefreshCameraMovement(editorCamera);
    
    if (editorCameraMovement != nullptr)
    {
        editorCameraMovement->SetCanCaptureMouse(isHovered);
    }

    editorCamera->Update(deltaTime);

    Camera* cam = editorCamera->GetComponent<Camera>();

    ImVec2 avail = ImGui::GetContentRegionAvail();
    ImVec2 target = avail;
    if (g_SceneAspect != AspectRatio::Free) {
        float ar = 1.0f;
        switch (g_SceneAspect) {
        case AspectRatio::R16_9: ar = 16.0f / 9.0f; break;
        case AspectRatio::R4_3:  ar = 4.0f / 3.0f;  break;
        case AspectRatio::R1_1:  ar = 1.0f;         break;
        default: break;
        }
        if (avail.x / avail.y > ar) {
            target.x = avail.y * ar;
        }
        else {
            target.y = avail.x / ar;
        }
    }

    if ((int)target.x != editorFB->GetWidth() ||
        (int)target.y != editorFB->GetHeight())
    {
        editorFB->Resize((int)target.x, (int)target.y);
        Camera* cam = editorCamera->GetComponent<Camera>();
        cam->InitCamera(target.x, target.y);
    }

    editorFB->Bind();
    glViewport(0, 0, (int)target.x, (int)target.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    scene->Render(editorFB, cam);
    scene->DrawGizmos(cam);
    editorFB->Unbind();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (avail.x - target.x) * 0.5f);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (avail.y - target.y) * 0.5f);
    ImGui::Image((ImTextureID)(intptr_t)editorFB->GetTextureID(), target);

    ImVec2 img_min = ImGui::GetItemRectMin();
    ImVec2 img_max = ImGui::GetItemRectMax();
    ImVec2 img_sz{ img_max.x - img_min.x, img_max.y - img_min.y };

    ImGuizmo::BeginFrame();
    ImGuizmo::Enable(selected != nullptr);
    ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetRect(img_min.x, img_min.y, img_sz.x, img_sz.y);

    glm::mat4 view = cam->GetViewMatrix();
    glm::mat4 proj = cam->GetProjectionMatrix();
    proj[1][1] *= -1.0f; 

    if (selected) {
        ImGuizmo::OPERATION op = ImGuizmo::TRANSLATE;
        glm::mat4 model = selected->GetWorldMatrix();

        ImGui::PushClipRect(img_min, img_max, true);
        bool wasDepth = glIsEnabled(GL_DEPTH_TEST);
        if (wasDepth) glDisable(GL_DEPTH_TEST);

        ImGuizmo::Manipulate(glm::value_ptr(view),
            glm::value_ptr(proj),
            op,
            ImGuizmo::LOCAL,
            glm::value_ptr(model));

        if (wasDepth) glEnable(GL_DEPTH_TEST);
        ImGui::PopClipRect();

        if (ImGuizmo::IsUsing()) {
            glm::vec3 t, r, s;
            ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(model),
                glm::value_ptr(t),
                glm::value_ptr(r),
                glm::value_ptr(s));
            selected->transform.position = t;
            selected->transform.rotation = glm::quat(glm::radians(r));
            selected->transform.scale = s;
        }
    }

    if (ImGui::BeginDragDropTarget()) {
        if (auto pl = ImGui::AcceptDragDropPayload("ASSET_PAYLOAD")) {
            int idx = *(int*)pl->Data;
            auto& a = AssetManager::GetAssets()[idx];
            if (a.type == AssetType::Prefab) {
                GameObject* go = PrefabSystem::Instantiate(a.path);
                scene->AddGameObject(go);
            }
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::End();
}
void SceneWindow::RefreshCameraMovement(GameObject* editorCamera)
{
    if (cashedEditorCamera == editorCamera)
    {
        return;
    }

    cashedEditorCamera = editorCamera;
    editorCameraMovement = nullptr;

    if (cashedEditorCamera != nullptr)
    {
        editorCameraMovement = cashedEditorCamera->GetComponent<CameraMovement>();
    }
}
const char* SceneWindow::GetWindowName() const
{
    return "Scene";
}

bool SceneWindow::IsHovered() const
{
    return isHovered;
}
