#pragma once

#include <Duin/Core/Utils/StateMachine.h>
#include <Duin/Core/Events/EventsModule.h>

class State_InAir_Strafe : public duin::State
{
    public:
		State_InAir_Strafe(duin::StateMachine& owner)
			: State(owner)
		{
			stateName = "State_InAir_Strafe";
		}
		~State_InAir_Strafe() override = default;

		void Enter() override;
		void OnEvent(duin::Event e) override;
		void Update(double delta) override;
		void PhysicsUpdate(double delta) override;
		void Draw() override;
		void DrawUI() override;
		void Exit() override;

    private:
};
