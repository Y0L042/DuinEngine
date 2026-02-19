#include "BeachBall.h"

#include "../../Singletons.h"
#include "../../ECS.h"

#include <Duin/Physics/PhysicsModule.h>
#include <Duin/Render/Renderer.h>

float radius = 0.5f;

void BeachBall::Ready()
{
    duin::Vector3 position(1.0f, 5.0f, 1.0f);
    body = duin::DynamicBody::Create({position}, duin::SphereGeometry(radius), duin::PhysicsMaterial(0.4f, 0.4f, 0.5f));

    entity = ecs.world->CreateEntity("BeachBall")
                 .IsA(duin::ECSPrefab::PhysicsDynamicBody)
                 .Set<duin::ECSComponent::Transform3D>({position})
                 .Set<duin::ECSComponent::DynamicBodyComponent>(body);
}

void BeachBall::PhysicsUpdate(double delta)
{
}

void BeachBall::Draw()
{
    duin::DrawDebugSphere(body->GetPosition(), radius);
    // duin::DrawBox(body->GetPosition(), body->GetRotation(), { radius, radius, radius });
}

void BeachBall::DrawUI()
{
    debugWatchlist.Post("BeachBall Position: ", "%f, %f, %f", body->GetPosition().x, body->GetPosition().y,
                        body->GetPosition().z);
}
