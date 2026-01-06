#pragma once

#include "Duin/Core/Utils/UUID.h"

namespace duin
{
struct SignalConnections
{
    UUID onReady;
    UUID onEvent;
    UUID onUpdate;
    UUID onPhysicsUpdate;
    UUID onDraw;
    UUID onDrawUI;
    UUID onDebug;
    UUID onEnter;
    UUID onExit;
};
} // namespace duin