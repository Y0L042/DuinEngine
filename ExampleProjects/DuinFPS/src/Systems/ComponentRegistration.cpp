#include "Components.h"
#include "Systems.h"

using namespace duin::ECSComponent;
using namespace duin::ECSTag;

void RegisterComponents(duin::GameWorld &world)
{
    world.Component<PlayerMovementInputVec3>();
    world.Component<GravityComponent>();
    world.Component<DebugCameraTarget>();
    world.Component<DebugCameraTag>();
    world.Component<MouseInputVec2>();
    world.Component<CameraPitchComponent>();
    world.Component<CameraYawComponent>();
    world.Component<InputVelocities>();
    world.Component<Movement3DInput>();
    world.Component<InputForces>();
    world.Component<InputVelocityDirection>();
    world.Component<Mass>();
    world.Component<GroundFriction>();
    world.Component<AirFriction>();
    world.Component<IdleTag>();
    world.Component<VelocityBob>();

    world.Component<CanRunComponent>();
    world.Component<RunTag>();
    world.Component<CanJumpComponent>();
    world.Component<JumpTag>();
    world.Component<CanSprintComponent>();
    world.Component<SprintTag>();
    world.Component<OnGroundTag>();
    world.Component<InAirTag>();
    world.Component<CanGravity>();
}
