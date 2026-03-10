#include "dnpch.h"
#include "ECSObservers.h"
#include "ECSComponents.h"
#include "ECSTag.h"

namespace duin
{

namespace ECSObserver
{
void RegisterObservers(duin::World &world)
{
    flecs::world &flecsWorld = world.GetFlecsWorld();

    flecs::observer localObserver;
    flecs::observer globalObserver;

    // On .set<Position3D, Local>
    localObserver = flecsWorld.observer<flecs::pair<ECSComponent::Position3D, ECSTag::Local>>()
                        .event(flecs::OnSet)
                        .each([globalObserver](flecs::iter &it, size_t i, ECSComponent::Position3D &lPos) {
                            // DN_CORE_INFO(".set<Position3D, Local>");
                            if (globalObserver.is_valid())
                            {
                                globalObserver.disable();
                                // DN_CORE_INFO("syncinc global");
                                Vector3 gPos = it.entity(i).try_get<ECSComponent::Position3D, ECSTag::Global>()->value;
                                it.entity(i).set<ECSComponent::Position3D, ECSTag::Global>({{0.0f, 0.0f, 0.0f}});
                                globalObserver.enable();
                            }
                        });

    // On .set<Position3D, Global>
    globalObserver = flecsWorld.observer<flecs::pair<ECSComponent::Position3D, ECSTag::Global>>()
                         .event(flecs::OnSet)
                         .each([localObserver](flecs::iter &it, size_t i, ECSComponent::Position3D &gPos) {
                             // DN_CORE_INFO(".set<Position3D, Global>");
                             if (localObserver.is_valid())
                             {
                                 localObserver.disable();
                                 // DN_CORE_INFO("syncinc local");
                                 it.entity(i).set<ECSComponent::Position3D, ECSTag::Local>({{0.0f, 0.0f, 0.0f}});
                                 localObserver.enable();
                             }
                         });
}
}; // namespace ECSObserver

} // namespace duin
