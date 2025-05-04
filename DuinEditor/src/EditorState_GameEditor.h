#pragma once

#include <Duin/Core/Utils/StateMachine.h>
#include <Duin/Core/Events/EventsModule.h>
#include <Duin/Core/Signals/SignalsModule.h>

extern bool isGameEditorValid;
extern duin::Signal<double> onPhysicsUpdateSignal;
extern duin::Signal<duin::Event> onEventSignal;
extern duin::Signal<> onDrawSignal;
extern duin::Signal<> onDrawUISignal;

class EditorState_GameEditor : public duin::State
{
    public:
		EditorState_GameEditor(duin::StateMachine& owner);
		~EditorState_GameEditor() override;

		void Enter() override;
		void OnEvent(duin::Event e) override;
		void Update(double delta) override;
		void PhysicsUpdate(double delta) override;
		void Draw() override;
		void DrawUI() override;
		void Exit() override;

    private:
};
