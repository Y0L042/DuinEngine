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
        void OnPanelEvent(PanelEvent e) override;

        void OnEvent(duin::Event e);
        void SimulatePhysics(double delta);
        void Render();
        void RenderUI();

    private:
        duin::RenderTexture target;
        EditorCamera mainCamera;

        float windowX = 0.0f;
        float windowY = 0.0f;

        duin::UUID eventSignalTag;
        duin::UUID physicsSignalTag;
        duin::UUID drawSignalTag;
        duin::UUID drawuiSignalTag;

        void ConnectToGameEditorSignals();
        void DisconnectFromGameEditorSignals();
        void CreateRenderTexture();
        void MoveMainCamera(double delta);
};
