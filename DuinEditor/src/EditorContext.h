#pragma once
#include <memory>

#include "EditorWorld.h"

#include <Duin/Core/Signals/Signal.h>
#include <Duin/ECS/GameWorld.h>

struct EditorContext
{
    std::shared_ptr<EditorWorld> editorWorld;
};

