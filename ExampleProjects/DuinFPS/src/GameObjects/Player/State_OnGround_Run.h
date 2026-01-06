#pragma once

#include <Duin/Core/Utils/StateMachine.h>
#include <Duin/Core/Events/EventsModule.h>

class State_OnGround_Run : public duin::GameState
{
    public:
		State_OnGround_Run(duin::GameStateMachine& owner)
			: GameState(owner)
		{
			stateName = "State_OnGround_Run";
		}
		~State_OnGround_Run() override = default;

		void Enter() override;
		void OnEvent(duin::Event e) override;
		void Update(double delta) override;
		void PhysicsUpdate(double delta) override;
		void Draw() override;
		void DrawUI() override;
		void Exit() override;

    private:
};
