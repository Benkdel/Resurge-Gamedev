#include "inferface.h"

#include <engine/interface/Widgets.h>

#include <engine/io/Keyboard.h>
#include <engine/io/Mouse.h>


namespace Amba {

    EntityId Interface::s_SelectedEntity            = -1;
    EntityId Interface::s_EntUnderCursor            = -1;
    EntityId Interface::s_SelectedCameraEntity      = -1;

    const char* glsl_version = "#version 130";

    Interface::Interface()
    : m_Clear_color(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)), m_MousePicker(MousePicker())
    {
    }

    void Interface::Setup(Window* window)
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        ImGui_ImplGlfw_InitForOpenGL(window->getWindow(), true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        // set up placeholders
        m_Clear_color = glm::vec4(0.45f, 0.55f, 0.60f, 1.00f);
        
        // Set up mouse Picker
        m_MousePicker.SetUp(window->GetWidth(), window->GetHeight());

        m_ScrWidth = window->GetWidth();
        m_ScrHeight = window->GetHeight();

        p_Window = window;
    }

    void Interface::BindScene(Scene* scene)
    {
        // check if scene is null, where?
        AB_ASSERT(!scene==NULL, "Trying to bind a NULL scene");
        p_CurrentScene = scene;
        m_MousePicker.UpdateScene(p_CurrentScene);

        p_EditorCamera = scene->GetComponent<CameraComponent>(scene->GetEntityByTag(EDITOR_CAMERA_TAG))->GetCamera();
        p_ActiveCamera = p_EditorCamera;
    }

    void Interface::ProcessUserInput(Camera* camera, double dt)
    {
        // check if windows size has changed
        if (m_ScrHeight != p_Window->GetHeight() || m_ScrWidth != p_Window->GetWidth())
        {
            m_ScrHeight = p_Window->GetHeight();
            m_ScrWidth = p_Window->GetWidth();
            m_firstRunFlag = true;
        }
        
        // update mouse picker
        if (m_IsEditorCameraActive)
            m_MousePicker.UpdateMousePos(camera);

        m_Status = (Amba::Mouse::isMouseLocked() == true) ? STATUS::ACTIVE : STATUS::INNACTIVE;

        if (m_Status == STATUS::ACTIVE)
            s_EntUnderCursor = m_MousePicker.SelectEntity(camera);
        else
        {
            s_EntUnderCursor = -1;
            s_SelectedEntity = -1;
        }

        // user input
        if (m_Status == STATUS::ACTIVE)
        {
            // Select entity
            if (EntityHandler::IsEntityValid(s_EntUnderCursor))
            {
                if (Amba::KeyBoard::KeyWentDown(GLFW_KEY_P) || Amba::Mouse::ButtonWentDown(GLFW_MOUSE_BUTTON_1))
                    Interface::s_SelectedEntity = s_EntUnderCursor;
            }

            // Apply force - temporal
            if (EntityHandler::IsEntityValid(Interface::s_SelectedEntity))
            {
                if (Amba::KeyBoard::KeyWentDown(GLFW_KEY_LEFT))
                    p_CurrentScene->GetComponent<PhysicsComponent>(Interface::s_SelectedEntity)->IncreaseForce(glm::vec3(-0.5f, 0.0f, 0.0f));
                if (Amba::KeyBoard::KeyWentDown(GLFW_KEY_RIGHT))
                    p_CurrentScene->GetComponent<PhysicsComponent>(Interface::s_SelectedEntity)->IncreaseForce(glm::vec3(0.5f, 0.0f, 0.0f));
            }
        }

        // proces camera controller input - only if camera is active
        // temp, always true and default:
        if (m_IsEditorCameraActive && EntityHandler::IsEntityValid(Interface::s_SelectedCameraEntity))
        {
            glm::vec3 pos = p_CurrentScene->GetComponent<TransformComponent>(Interface::s_SelectedCameraEntity)->GetPosition();
            EditorCamController::ProcessInput(pos, camera, dt);
            p_CurrentScene->GetComponent<TransformComponent>(Interface::s_SelectedCameraEntity)->UpdatePosition(pos);
        }
    }

    void Interface::Run(double dt)
    {
        
        ProcessUserInput(p_ActiveCamera, dt);
        
        // runs ImGui interface
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        /* ========================
         ========= LAYOUT =========
         ========================== */

        // main menu - left column
        {
            if (m_firstRunFlag)
            {
                float columnWidth = m_ScrWidth * 0.20f;
                ImGui::SetNextWindowPos(ImVec2{0.0f, 0.0f});
                ImGui::SetNextWindowSize(ImVec2{ columnWidth, (float)m_ScrHeight });
            }

            ImGui::Begin("Menu");
            
            ShowDiagnosticInfo(p_CurrentScene, m_Clear_color);
            ShowActiveEntitiesInScene(p_CurrentScene);
            ShowTransform(p_CurrentScene);
            ShowAdditionalEntitiesData(p_CurrentScene);
            ShowEntityUnderMouse(p_CurrentScene);
            ShowCameraSelection(p_CurrentScene, p_ActiveCamera, m_IsEditorCameraActive);

            DebugingMethods(p_CurrentScene);

            ImGui::End();
        }
        


        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        // set first run flag to false after first loop
        m_firstRunFlag = false;


        // if editor camera is active, set boolean to false
    }

    void Interface::Cleanup()
    {
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    std::string Interface::GetActiveEntity(EntityId ent)
    {
        if (EntityHandler::IsEntityValid(ent))
            return std::to_string(ent);
        return "-";
    }

    std::string Interface::GetActiveEntityTag(EntityId ent, Scene* scene)
    {
        if (EntityHandler::IsEntityValid(ent))
            return scene->GetComponent<TagComponent>(ent)->m_Tag;
        return "-";
    }

}
