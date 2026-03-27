#pragma once

#include <Duin/ECS/GameWorld.h>

void RegisterComponents(duin::GameWorld &world);

void ExecuteQueryUpdatePlayerYaw(duin::GameWorld &world);
void ExecuteQueryUpdateCameraPitch(duin::GameWorld &world);
void ExecuteQueryComputePlayerInputVelocity(duin::GameWorld &world);
void ExecuteQueryGravity(duin::GameWorld &world);

void ExecuteQueryMoveDebugCamera(duin::GameWorld &world);

void ExecuteQueryIdle(duin::GameWorld &world);
void ExecuteQueryRun(duin::GameWorld &world);
void ExecuteQuerySprint(duin::GameWorld &world);
void ExecuteQueryOnGroundJump(duin::GameWorld &world);

void ExecuteQueryResolveInputVelocities(duin::GameWorld &world);
void ExecuteQueryResolveInputForces(duin::GameWorld &world);
