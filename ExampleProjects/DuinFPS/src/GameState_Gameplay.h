#pragma once

#include <Duin/Core/Utils/StateMachine.h>
#include <Duin/Core/Events/EventsModule.h>

class GameState_Gameplay : public duin::GameState
{
    public:
		GameState_Gameplay(duin::GameStateMachine& owner)
			: GameState(owner)
		{
			stateName = "GameState_Gameplay";
		}
		~GameState_Gameplay() override = default;

		void Enter() override;
		void OnEvent(duin::Event e) override;
		void Update(double delta) override;
		void PhysicsUpdate(double delta) override;
		void Draw() override;
		void DrawUI() override;
		void Exit() override;

    private:
};
