#pragma once

#include <Duin/Core/Utils/StateMachine.h>
#include <Duin/Core/Events/EventsModule.h>
#include <Duin/Core/Signals/SignalsModule.h>

extern bool isGameEditorValid;

class EditorState_GameEditor : public duin::State
{
    public:
		EditorState_GameEditor(duin::StateMachine& owner);
		~EditorState_GameEditor() override;

		static duin::Signal<double> onUpdateSignal;
		static duin::Signal<double> onPhysicsUpdateSignal;
		static duin::Signal<duin::Event> onEventSignal;
		static duin::Signal<> onDrawSignal;
		static duin::Signal<> onDrawUISignal;

		void Enter() override;
		void OnEvent(duin::Event e) override;
		void Update(double delta) override;
		void PhysicsUpdate(double delta) override;
		void Draw() override;
		void DrawUI() override;
		void Exit() override;

    private:
};
