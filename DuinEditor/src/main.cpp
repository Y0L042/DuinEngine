#include "main.h"

#include <DuinMeta.h>
#include <Duin/EntryPoint.h>

#ifndef DEBUG
    #define DEBUG
#endif

Editor *Editor::instance = nullptr;

duin::Application *duin::CreateApplication()
{
    return new Editor();
}

Editor &Editor::Get()
{
    return *instance;
}

void Editor::Initialize()
{
    Editor::instance = this;

    duin::SetFramerate(244);
    SetWindowStartupSize(1600, 900);
    SetWindowName("DuinEditor");
}

void Editor::Ready()
{
    duin::SetWindowResizable(true);
}

void Editor::OnEvent(duin::Event event)
{
}

void Editor::Update(double rDelta)
{
}

void Editor::PhysicsUpdate(double pDelta)
{
}

void Editor::Draw()
{
}

void Editor::DrawUI()
{
}

void Editor::Debug()
{
}
