#include "sandbox.h"

#include <Duin.h>
#include <Duin/entrypoint.h>

Duin::Application* Duin::CreateApplication() { return new Sandbox(); }


void Sandbox::Initialize()
{
}

void Sandbox::Ready()
{
}

void Sandbox::HandleInputs()
{
}

void Sandbox::Update(double rDelta)
{
}


void Sandbox::PhysicsUpdate(double pDelta)
{
}

void Sandbox::Draw()
{
}

