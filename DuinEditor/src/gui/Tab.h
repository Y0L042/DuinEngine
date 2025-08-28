#pragma once

#include "PanelManager.h"
#include "TabBlackboard.h"
#include "GameObjects/SceneWorld.h"

#include <Duin/Core/Utils/UUID.h>
#include <Duin/IO/IOModule.h>
#include <Duin/Core/Signals/SignalsModule.h>

#include <string>

class EditorWindow;
class Tab
{
    public:
        std::string title;
        bool open = true;
        bool renaming = false;
        bool flag_requestDelete = false;
        bool isFocussed = false;
        bool prevIsFocussed = false;
        duin::Signal<bool> onFocusChange;
        std::shared_ptr<TabBlackboard> blackboard;

        static std::shared_ptr<Tab> Create(EditorWindow *owner);

        Tab(EditorWindow* owner);

        std::string GetPanelManagerID();
        void SetFocussed(bool status);
        void SetOwner(EditorWindow* owner);
        void SetSceneWorld(std::shared_ptr<SceneWorld> newSceneWorld);
		void SetTitle(const std::string& newTitle);
        std::shared_ptr<PanelManager> CreatePanelManager();
        std::shared_ptr<PanelManager> CreatePanelManager(duin::JSONValue value);

        std::shared_ptr<SceneWorld> GetSceneWorld();

        void ProcessBlackboard();

        duin::UUID GetUUID();

        duin::JSONValue Serialise();
        duin::JSONValue Deserialise(duin::JSONValue value);

        void DrawWorkspace();

    private:
        duin::UUID uuid;
        EditorWindow* owner = nullptr;
        std::shared_ptr<PanelManager> panelManager = nullptr;
        std::shared_ptr<SceneWorld> sceneWorld;

        void DrawMenu();
};
