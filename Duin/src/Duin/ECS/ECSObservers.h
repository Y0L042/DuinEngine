/**
 * @file ECSObservers.h
 * @brief ECS observer registration.
 * @ingroup ECS
 */

#pragma once

#include "Duin/ECS/DECS/World.h"

namespace duin
{

namespace ECSObserver
{
void RegisterObservers(duin::World &world);
}; // namespace ECSObserver

} // namespace duin
