#include "GameWindow.h"
#include "../EditorWindowRegistry.h"

REGISTER_EDITOR_WINDOW(GameWindow);

bool GameWindow::IsHovered() const
{
    return isHovered;
}

void GameWindow::Draw(EditorContext& context)
{
    FrameBuffer* gameFB = context.gameFrameBuffer;
    Scene* scene = context.scene;
    EditorState& editorState = *context.editorState;

    ImGui::Begin("Game View");

    isHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);

    if (ImGui::Button("Stop"))  editorState = EditorState::Stop;
    ImGui::SameLine();
    if (ImGui::Button("Play"))  editorState = EditorState::Play;
    ImGui::SameLine();
    if (ImGui::Button("Pause")) editorState = EditorState::Pause;
    ImGui::SameLine();
    {
        static const char* names[] = { "Free","16:9","4:3","1:1" };
        int idx = (int)gameAspect;
        if (ImGui::Combo("Aspect", &idx, names, IM_ARRAYSIZE(names))) {
            gameAspect = (AspectRatio)idx;
        }
    }

    if (editorState == EditorState::Play) {
        ImGui::SameLine();
        ImGui::Checkbox("Lock Mouse", &lockMouse);
        ImGui::TextColored({ 0,1,0,1 }, "PLAY MODE");

        GLFWwindow* window = InputManager::Instance().GetWindow();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            lockMouse = false;
        }

        if (lockMouse) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
    ImVec2 avail = ImGui::GetContentRegionAvail();
    ImVec2 target = avail;
    if (gameAspect != AspectRatio::Free) {
        float ar = (gameAspect == AspectRatio::R16_9 ? 16.f / 9.f :
            gameAspect == AspectRatio::R4_3 ? 4.f / 3.f : 1.f);
        if (avail.x / avail.y > ar) target.x = avail.y * ar;
        else                      target.y = avail.x / ar;
    }

    if ((int)target.x != gameFB->GetWidth() ||
        (int)target.y != gameFB->GetHeight())
    {
        gameFB->Resize((int)target.x, (int)target.y);
        if (auto cam = scene->camera)
            cam->InitCamera(target.x, target.y);
    }

    gameFB->Bind();
    glViewport(0, 0, (int)target.x, (int)target.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    scene->Render(gameFB, nullptr);
    gameFB->Unbind();

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (avail.x - target.x) * 0.5f);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (avail.y - target.y) * 0.5f);
    ImGui::Image((ImTextureID)(intptr_t)gameFB->GetTextureID(), target);

    ImGui::End();
}

const char* GameWindow::GetWindowName() const
{
    return "Game";
}
