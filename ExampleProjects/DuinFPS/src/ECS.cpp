#include "ECS.h"
#include "Singletons.h"


ECSManager::ECSManager()
{}

ECSManager::~ECSManager()
{}

flecs::world ECSManager::GetWorld()
{
    return world;
}

void ECSManager::RegisterComponents()
{
    world.component<Position3D>();
    world.component<Velocity3D>();
    world.component<Quaternion>();
    world.component<Rotation3D>();
    world.component<AngularVelocity3D>();
    world.component<MovementInput3D>();
    world.component<RotationInput3D>();
    world.component<Camera3D>();
    world.component<PlayerMovementInputVec2>();
    world.component<MouseInputVec2>();

    world.component<Global>();
    world.component<Local>();
    world.component<PlayerTag>();
    world.component<CameraTag>();
}

void ECSManager::CreateQueries()
{
    queryUpdatePosition3D = world.query_builder<Position3D, const Velocity3D>()
        .term_at(0).second<Local>()
        .cached().build();
    queryHierarchicalUpdatePosition3D = world.query_builder<const Position3D, const Position3D *, Position3D>()
        .term_at(0).second<Local>()
        .term_at(1).second<Global>()
        .term_at(2).second<Global>()
        .term_at(1)
            .parent().cascade()
        .cached().build();

    queryUpdateRotation3D = world.query_builder<Rotation3D, const AngularVelocity3D>()
        .term_at(0).second<Local>()
        .cached().build();
    queryHierarchicalUpdateRotation3D = world.query_builder<const Rotation3D, const Rotation3D *, Rotation3D>()
        .term_at(0).second<Local>()
        .term_at(1).second<Global>()
        .term_at(2).second<Global>()
        .term_at(1)
            .parent().cascade()
        .cached().build();

    // Query to update player yaw
    queryUpdatePlayerYaw = world.query_builder<Rotation3D, RotationInput3D, MouseInputVec2>()
        .with<PlayerTag>() // Filter entities tagged as Player
        .with<Rotation3D, Local>()
        .with<RotationInput3D>()
        .with<MouseInputVec2>()
        .build();

    // Query to update camera pitch
    queryUpdateCameraPitch = world.query_builder<Rotation3D, RotationInput3D, MouseInputVec2>()
        .with<CameraTag>() // Filter entities tagged as Camera
        .with<Rotation3D, Local>()
        .with<RotationInput3D>()
        .with<MouseInputVec2>()
        .build();

    queryMovementInput = world.query_builder<Velocity3D, const MovementInput3D>()
        .cached().build();
    queryRotationInput = world.query_builder<AngularVelocity3D, const RotationInput3D>()
        .cached().build();

    queryControlPlayerMovement = world.query_builder<MovementInput3D, const PlayerMovementInputVec2>()
        .cached().build();
    queryControlPlayerRotation = world.query_builder<RotationInput3D, const MouseInputVec2>()
        .cached().build();

    queryControlCamera = world.query_builder<Camera3D, const Position3D, const Rotation3D>()
        .term_at(1).second<Global>()
        .term_at(2).second<Global>()
        .cached().build();


}

void ECSManager::FreeQueries()
{
    queryHierarchicalUpdatePosition3D.destruct();
    queryUpdatePosition3D.destruct();
    queryMovementInput.destruct();
    queryControlCamera.destruct();
    queryControlPlayerMovement.destruct();
}

void ECSManager::CreatePrefabs()
{

}

void ECSManager::ExecuteQueryUpdatePosition3D(double delta) 
{
    queryUpdatePosition3D.each(
        [delta](
            Position3D& p, const Velocity3D& v) 
        {
            Vector3 dv = Vector3Scale(v.value, static_cast<float>(delta)); 
            p.value = Vector3Add(p.value, dv); 
            // dbgConsole.LogEx(Duin::LogLevel::Info, "VALUES{ %.1f, %.1f, %.1f }", p.value.x, p.value.y, p.value.z);
        }
    );
}

void ECSManager::ExecuteQueryHierarchicalUpdatePosition3D()
{
    queryHierarchicalUpdatePosition3D.each(
        [](flecs::iter& it, size_t index,
           const Position3D& p, const Position3D *p_parent, Position3D& p_out)
        {
            p_out.value.x = p.value.x;
            p_out.value.y = p.value.y;
            p_out.value.z = p.value.z;
            if (p_parent) {
                p_out.value.x += p_parent->value.x;
                p_out.value.y += p_parent->value.y;
                p_out.value.z += p_parent->value.z;
            }
        }
    );
}

void ECSManager::ExecuteQueryUpdateRotation3D(double delta)
{
    queryUpdateRotation3D.each(
        [delta](flecs::iter&it, size_t index,
            Rotation3D& r, const AngularVelocity3D& v)
        {
            Vector3 dv = Vector3Scale(v.value, static_cast<float>(delta));
            // TODO
        }
    );
}

void ECSManager::ExecuteQueryHierarchicalUpdateRotation3D()
{
    queryHierarchicalUpdateRotation3D.each(
        [](flecs::iter& it, size_t index,
           const Rotation3D& r, const Rotation3D *r_parent, Rotation3D& r_out)
        {
            if (r_parent) {
                r_out.value = QuaternionMultiply(r_parent->value, r.value);
                r_out.value = QuaternionNormalize(r_out.value);
            }
        }
    );
}

void ECSManager::ExecuteQueryMovementInput() 
{
    queryMovementInput.each(
        [](flecs::iter& it, size_t index,
                Velocity3D& v, const MovementInput3D& i) 
        {
            Vector3 inputFactor = Vector3Scale(i.value, 30);
            v.value = inputFactor;
        }
    );
}

void ECSManager::ExecuteQueryRotationInput()
{
    queryRotationInput.each(
        [](flecs::iter& it, size_t index,
                AngularVelocity3D& v, const RotationInput3D& i) 
        {
            Vector3 inputFactor = Vector3Scale(i.value, 30);
            v.value = inputFactor;
        }
    );
}

void ECSManager::ExecuteQueryControlPlayerMovement(double delta)
{
    queryControlPlayerMovement.each(
        [](flecs::iter& it, size_t index,
                MovementInput3D& i, const PlayerMovementInputVec2& pv) 
        {
            i.value.x = pv.value.x;
            i.value.z = pv.value.y;
        }
    );
}

void ECSManager::ExecuteQueryControlPlayerRotation(double delta)
{
    queryControlPlayerRotation.each(
        [](flecs::iter& it, size_t index,
                RotationInput3D& i, const MouseInputVec2& pv) 
        {
            // TODO
        }
    );
}

void ECSManager::ExecuteQueryUpdatePlayerYaw(double delta)
{
    queryUpdatePlayerYaw.each(
        [](flecs::iter& it, size_t index,
            Rotation3D& r, RotationInput3D& ri, const MouseInputVec2& mouseDelta) 
        {
            // Define sensitivity factor for yaw
            const float sensitivity = 0.001f; // Adjust as needed

            // Calculate yaw rotation quaternion based on mouse X movement
            float yawAngle = mouseDelta.value.x * sensitivity;
            Quaternion yawQuat = QuaternionFromAxisAngle(Vector3{0.0f, 1.0f, 0.0f}, yawAngle);

            // Update current rotation by applying yaw
            r.value = QuaternionMultiply(yawQuat, r.value);
            r.value = QuaternionNormalize(r.value);
        }
    );
}

void ECSManager::ExecuteQueryUpdateCameraPitch(double delta)
{
    queryUpdateCameraPitch.each(
        [](flecs::iter& it, size_t index,
            Rotation3D& r, RotationInput3D& ri, const MouseInputVec2& mouseDelta) 
        {
            // // Define sensitivity factor for pitch
            // const float sensitivity = 0.001f; // Adjust as needed
            //
            // // Calculate pitch rotation quaternion based on mouse Y movement
            // float pitchAngle = mouseDelta.value.y * sensitivity;
            // Quaternion pitchQuat = QuaternionFromAxisAngle((Vector3){1.0f, 0.0f, 0.0f}, pitchAngle);
            //
            // // Update current rotation by applying pitch
            // r.value = QuaternionMultiply(pitchQuat, r.value);
            // r.value = QuaternionNormalize(r.value);
            //
            // // Clamp the pitch to prevent flipping
            // Vector3 euler = QuaternionToEuler(r.value);
            // if (euler.x > 89.0f) euler.x = 89.0f;
            // if (euler.x < -89.0f) euler.x = -89.0f;
            // r.value = QuaternionFromEuler(euler);
        }
    );
}

void ECSManager::ExecuteQueryControlCamera(double delta)
{
    queryControlCamera.each(
        [](flecs::iter& it, size_t index,
                Camera3D& c, const Position3D& p, const Rotation3D& r) 
        {
            c.position = p.value;
            
            // TODO
        }
    );
}


void ECSManager::ExecuteQueryControlFPView(double delta)
{
}
