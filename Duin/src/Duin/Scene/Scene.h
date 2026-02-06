#pragma once
#include <vector>
#include <stdint.h>
#include "Duin/ECS/DECS/DECS.h"
#include "Duin/Core/Utils/UUID.h"

namespace duin
{
struct EntityData
{
    UUID uuid;
    uint64_t ecs_id;
};

struct ComponentData
{
    UUID uuid;
};

struct Scene
{
  public:
    Scene();
    ~Scene();

    // Scene -> Serial
    std::string SceneToSerial();

    // Serial -> Scene
    static Scene SerialToScene(const std::string &serial);

    // Scene -> Instance
    void SceneToInstance(World &world);

    // Instance -> Scene
     static Scene InstanceToScene(World& world);

  private:
    std::vector<EntityData> entityData;
    std::vector<ComponentData> componentData;
};

} // namespace duin