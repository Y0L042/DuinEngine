#include "Game.h"

#include <Duin.h>
#include <Duin/EntryPoint.h>
#include <Duin/Core/Events/EventsModule.h>

#include "Singletons.h"
#include "PlayerConstants.h"
#include "GameStates.h"

#define DEBUG

duin::Application *duin::CreateApplication()
{
#ifdef DN_DEBUG
    duin::fs::SetBinDebugMode(true);
#endif /* DN DEBUG */

    return new Game();
}

duin::DebugConsole debugConsole;
duin::DebugWatchlist debugWatchlist;

std::shared_ptr<duin::GameStateMachine> mainStateMachine;

void Game::Initialize()
{
    duin::SetFramerate(PlayerConstants::TARGET_FRAMERATE);
    SetWindowStartupSize(1600, 900);
    duin::AddInputActionBinding("TogglePause", DN_KEYBOARD_01, DN_SCANCODE_P, DN_KEVENT_PRESSED, DN_KEY_MOD_ALT);
}

void Game::Ready()
{
    debugConsole.Log("Game is ready!");
    mainStateMachine = CreateChildObject<duin::GameStateMachine>();
    mainStateMachine->SwitchState<GameState_Gameplay>();
}

void Game::OnEvent(duin::Event event)
{
}

void Game::Update(double rDelta)
{
    static bool isPaused = false;
    duin::OnInputActionTriggered("TogglePause", [&]() {
        if (isPaused)
        {
            duin::ResumePhysics();
        }
        else
        {
            duin::PausePhysics();
        }
        isPaused = !isPaused;
    });
}

void Game::PhysicsUpdate(double pDelta)
{
}

void Game::Draw()
{
}

void Game::DrawUI()
{
    debugWatchlist.Draw("Watchlist");
    debugConsole.Draw("Console");
}

void Game::Debug()
{
}
