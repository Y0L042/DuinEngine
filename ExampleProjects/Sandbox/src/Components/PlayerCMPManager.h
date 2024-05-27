#pragma once

#include <Duin.h>

#include "./PlayerComponent.h"
#include "./PlayerInputComponent.h"
#include "./MovementComponent.h"

struct PlayerCMPManager
{
    static void Update(Duin::InputEvent e)
    {
		auto view = Duin::Registry::GetRegistry().view<PlayerCMP, PlayerInputCMP>();
		for (auto [entity, pinput] : view.each())
		{
			pinput.inputVec = e.GetInputVector(KEY_W, KEY_S, KEY_A, KEY_D);
		}
    }
};