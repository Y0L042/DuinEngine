#pragma once

#include "Panel.h"
#include "../EditorState_GameEditor.h"
#include "../GameObjects/EditorCamera.h"

#include <Duin/IO/IOModule.h>
#include <Duin/Render/RenderModule.h>


#include <string>

class PanelManager;
class ViewportPanel : public Panel
{
    public:
        ViewportPanel(const std::string& name, PanelManager *panelManager);
        ViewportPanel(duin::DataValue value);
        ~ViewportPanel() override;

        void Init();

        void Deserialise(duin::DataValue value) override;
        void DrawContent() override;

        void OnEvent(duin::Event e);
        void SimulatePhysics(double delta);
        void Render();
        void RenderUI();

    private:
        duin::RenderTexture target;
        EditorCamera mainCamera;

        void ConnectToGameEditorSignals();
        void CreateRenderTexture();
        void MoveMainCamera();
};
