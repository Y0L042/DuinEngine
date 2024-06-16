#pragma once

#include "Duin/Core/Core.h"

#include "Duin/Entity/Entity.h"
#include "Duin/Events/InputEvent.h"
#include "Duin/Entity/Registry.h"
#include "Duin/Entity/ComponentManager.h"
#include "Duin/Assets/AssetManager.h"

#include <entt/entt.hpp>

#include <memory>
#include <vector>


namespace Duin
{
    class Node;

    class DUIN_API SceneManager {
    public:
        SceneManager()
            : registry(std::make_shared<Registry>()), assetManager(std::make_shared<AssetManager>())
        {}
        ~SceneManager() = default; 

        std::shared_ptr<Registry> GetRegistry()
        {
            return registry;
        }

        std::shared_ptr<AssetManager> GetAssetManager()
        {
            return assetManager;
        }

        std::shared_ptr<Entity> CreateEntity();

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
            DN_CORE_INFO("Node <{}> created", typeid(T).name());
            std::shared_ptr<T> tPtr = std::make_shared<T>(std::forward<Args>(args)...);
            AddNodeToRootNodeList(tPtr); // nodes stored in scenemanager
            AddToNodeMap(tPtr); // map of ALL nodes
            NodeSetup(tPtr); // Sets node parent & scenemanager & and node.Ready()
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
        std::shared_ptr<AssetManager> assetManager;

        void AddNodeToRootNodeList(std::shared_ptr<Node> node);
        void RemoveNodeFromRootNodeList(std::shared_ptr<Node> node);
    };
}