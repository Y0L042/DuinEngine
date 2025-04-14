#pragma once

#include <Duin/Core/Utils/StateMachine.h>
#include <Duin/Core/Events/EventsModule.h>

class State_OnGround_Idle : public duin::State
{
    public:
		State_OnGround_Idle(duin::StateMachine& owner)
			: State(owner)
		{
			stateName = "State_OnGround_Idle";
		}
		~State_OnGround_Idle() override = default;

		void Enter() override;
		void OnEvent(duin::Event e) override;
		void Update(double delta) override;
		void PhysicsUpdate(double delta) override;
		void Draw() override;
		void DrawUI() override;
		void Exit() override;

    private:
};
