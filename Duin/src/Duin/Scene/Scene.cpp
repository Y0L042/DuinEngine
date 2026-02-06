#include "dnpch.h"
#include "Scene.h"

duin::Scene::Scene()
{
}

duin::Scene::~Scene()
{
}

std::string duin::Scene::SceneToSerial()
{

    return std::string();
}

duin::Scene duin::Scene::SerialToScene(const std::string &serial)
{
    return Scene();
}

void duin::Scene::SceneToInstance(World &world)
{
}

duin::Scene duin::Scene::InstanceToScene(World &world)
{
    return Scene();
}
