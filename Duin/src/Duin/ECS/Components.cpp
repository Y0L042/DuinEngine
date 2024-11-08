#include "dnpch.h"
#include "Components.h"

void RegisterComponents(flecs::world& world)
{
    world.component<Vector2>();
    world.component<Position2D>();
    world.component<Velocity2D>();

    world.component<Vector3>();
    world.component<Position3D>();
    world.component<Velocity3D>();
}
