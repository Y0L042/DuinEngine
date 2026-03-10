#include "dnpch.h"
#include "ECSTag.h"
#include "Duin/ECS/ComponentInspector.h"

namespace duin
{

namespace ECSTag
{
void RegisterTags(duin::World &world)
{
    world.Component<ECSTag::Local>();
    world.Component<ECSTag::Global>();

    world.Component<ECSTag::CreateExternalRef>();
    world.Component<ECSTag::ActiveExternalRef>();
    world.Component<ECSTag::DeleteExternalRef>();

    world.Component<ECSTag::PxKinematic>();
    world.Component<ECSTag::PxDynamic>();
    world.Component<ECSTag::PxStatic>();
    world.Component<ECSTag::NonPx>();

    world.Component<ECSTag::SetCameraAsActive>();
    world.Component<ECSTag::CameraIsActive>();
    world.Component<ECSTag::ActiveCamera>();

    auto &inspector = duin::ComponentInspector::Get();
    inspector.RegisterComponent<ECSTag::Local>("Local");
    inspector.RegisterComponent<ECSTag::Global>("Global");
    inspector.RegisterComponent<ECSTag::CreateExternalRef>("CreateExternalRef");
    inspector.RegisterComponent<ECSTag::ActiveExternalRef>("ActiveExternalRef");
    inspector.RegisterComponent<ECSTag::DeleteExternalRef>("DeleteExternalRef");
    inspector.RegisterComponent<ECSTag::PxKinematic>("PxKinematic");
    inspector.RegisterComponent<ECSTag::PxDynamic>("PxDynamic");
    inspector.RegisterComponent<ECSTag::PxStatic>("PxStatic");
    inspector.RegisterComponent<ECSTag::NonPx>("NonPx");
    inspector.RegisterComponent<ECSTag::SetCameraAsActive>("SetCameraAsActive");
    inspector.RegisterComponent<ECSTag::CameraIsActive>("CameraIsActive");
    inspector.RegisterComponent<ECSTag::ActiveCamera>("ActiveCamera");
}
} // namespace ECSTag

} // namespace duin
