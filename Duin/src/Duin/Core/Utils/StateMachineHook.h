#pragma once

#include "Duin/Core/Utils/StateMachine.h"

namespace duin
{
	class StateMachineHook
	{
	public:
        StateMachineHook();
        ~StateMachineHook();

        void HookToMachine(StateMachine* sm);
        void HookToState(State *state);
        void Unhook();

        virtual void OnEvent(Event e);
        virtual void Update(double delta);
        virtual void PhysicsUpdate(double delta);
        virtual void Draw();
        virtual void DrawUI();

        UUID GetEventSignalHandle();
        UUID GetUpdateSignalHandle();
        UUID GetPhysicsUpdateSignalHandle();
        UUID GetDrawSignalHandle();
        UUID GetDrawUISignalHandle();

	private:
        StateMachine *hookedSM = nullptr;
        UUID onEventSignalHandle = UUID::INVALID;
        UUID onUpdateSignalHandle = UUID::INVALID;
        UUID onPhysicsUpdateSignalHandle = UUID::INVALID;
        UUID onDrawSignalHandle = UUID::INVALID;
        UUID onDrawUISignalHandle = UUID::INVALID;

        void ExecuteOnEvent(Event e);
        void ExecuteUpdate(double delta);
        void ExecutePhysicsUpdate(double delta);
        void ExecuteDraw();
        void ExecuteDrawUI();
	};
}
