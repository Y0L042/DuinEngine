#pragma once

#include <Duin/Core/Utils/StateMachine.h>
#include <Duin/Core/Events/EventsModule.h>

class EditorState_SelectProject : public duin::State
{
    public:
		EditorState_SelectProject(duin::StateMachine& owner)
			: State(owner)
		{
			stateName = "EditorState_SelectProject";
		}
		~EditorState_SelectProject() override = default;

		void Enter() override;
		void OnEvent(duin::Event e) override;
		void Update(double delta) override;
		void PhysicsUpdate(double delta) override;
		void Draw() override;
		void DrawUI() override;
		void Exit() override;

        void InitProjectList();
        void LoadSelectedProject();
        void DrawGUI();
        void LoadEditorCFG();
        void ImportProject();

    private:
};
