#pragma once

#include <Duin/Core/Utils/StateMachine.h>
#include <Duin/Core/Events/EventsModule.h>

class EditorState_GameEditor : public duin::State
{
    public:
		EditorState_GameEditor(duin::StateMachine& owner)
			: State(owner)
		{
			stateName = "EditorState_SelectProject";
		}
		~EditorState_GameEditor() override = default;

		void Enter() override;
		void OnEvent(duin::Event e) override;
		void Update(double delta) override;
		void PhysicsUpdate(double delta) override;
		void Draw() override;
		void DrawUI() override;
		void Exit() override;
    private:
};
