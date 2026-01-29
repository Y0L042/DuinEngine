/**
 * @file SignalConnections.h
 * @brief Bundle of signal connection UUIDs for lifecycle events.
 * @ingroup Core_Signals
 */

#pragma once

#include "Duin/Core/Utils/UUID.h"

namespace duin
{

/**
 * @struct SignalConnections
 * @brief Holds UUIDs for signal connections to lifecycle events.
 * @ingroup Core_Signals
 *
 * Used by GameObject and GameState to track their signal connections.
 * Each member stores the UUID returned by Signal::Connect(), enabling
 * disconnection when the object is destroyed.
 */
struct SignalConnections
{
    UUID onReady;         ///< Connection to ready signal.
    UUID onEvent;         ///< Connection to event signal.
    UUID onUpdate;        ///< Connection to update signal.
    UUID onPhysicsUpdate; ///< Connection to physics update signal.
    UUID onDraw;          ///< Connection to draw signal.
    UUID onDrawUI;        ///< Connection to draw UI signal.
    UUID onDebug;         ///< Connection to debug signal.
    UUID onEnter;         ///< Connection to state enter signal.
    UUID onExit;          ///< Connection to state exit signal.
};

} // namespace duin
