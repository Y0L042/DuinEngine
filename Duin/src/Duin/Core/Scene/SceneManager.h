#pragma once

#include "Duin/Core/Core.h"

#include "Duin/Entity/Entity.h"
#include "Duin/Entity/Registry.h"
#include "Duin/Entity/ComponentManager.h"
#include "Duin/Events/InputEvent.h"

#include <entt/entt.hpp>

#include <memory>
#include <vector>


namespace Duin
{
    class Node;

    class DUIN_API SceneManager {
    public:
        SceneManager()
            : registry(std::make_shared<Registry>()) 
        {}
        ~SceneManager() = default; 

        std::shared_ptr<Entity> CreateEntity();
        std::shared_ptr<Registry> GetRegistry()
        {
            return registry;
        }
        template<typename CompMgr, typename... Args>
        CompMgr& CreateComponentManager(Args&&... args)
        {
            static_assert(std::is_base_of<ComponentManager, CompMgr>::value, "CpmpMgr must be a ComponentManager derived struct!");
            CompMgr* compMgr = new CompMgr(std::forward<Args>(args)...);
            compMgr->SetRegistry(registry.get());
            return *compMgr;
        }
        template<typename... Component>
        auto ViewEntitiesWith()
        {
            return registry->View<Component...>();
        }

        template<typename T, typename... Args>
        std::shared_ptr<T> CreateNode(Args&&... args)
        {
            static_assert(std::is_base_of<Node, T>::value, "T must be a Node derived class");
            std::shared_ptr<T> tPtr = std::make_shared<T>(std::forward<Args>(args)...);
            NodeSetup(tPtr);
            AddNodeToRootNodeList(tPtr);
            AddToNodeMap(tPtr);
            return tPtr;
        }

        void NodeSetup(std::shared_ptr<Node> node);
        void RemoveNode(std::shared_ptr<Node> node);
        void AddToNodeMap(std::shared_ptr<Node> node);
        void RemoveFromNodeMap(std::shared_ptr<Node> node);

        std::shared_ptr<Node> GetNodePtr(UUID uuid);
        void CallReady();
        void CallHandleInput(InputEvent e);
        void CallUpdate(double rDelta);
        void CallPhysicsUpdate(double pDelta);
        void CallDraw();


    private:
        UUID uuid;
        std::shared_ptr<Registry> registry;
        std::vector<std::shared_ptr<Node>> rootNodeList;
        std::unordered_map<UUID, std::shared_ptr<Node>> nodeMap;

        void AddNodeToRootNodeList(std::shared_ptr<Node> node);
        void RemoveNodeFromRootNodeList(std::shared_ptr<Node> node);
    };
}