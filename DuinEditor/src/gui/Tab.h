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
        EditorWindow* owner = nullptr;
        duin::Signal<bool> onFocusChange;
        std::shared_ptr<TabBlackboard> blackboard;

        static std::shared_ptr<Tab> Create(EditorWindow *owner);
        static std::shared_ptr<Tab> Create(EditorWindow *owner, duin::JSONValue value);
        static std::shared_ptr<Tab> Create(EditorWindow* owner, const std::string& title);

        Tab() {};
        Tab(duin::JSONValue value);

        std::string GetPanelManagerID();
        void SetFocussed(bool status);
        void SetOwner(EditorWindow* owner);
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
        std::shared_ptr<PanelManager> panelManager = nullptr;
        std::shared_ptr<SceneWorld> sceneWorld;

        void DrawMenu();
};
