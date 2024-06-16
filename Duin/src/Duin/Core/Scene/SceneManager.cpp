#include "dnpch.h"

#include "SceneManager.h"
#include "Duin/Object/Node.h"

namespace Duin
{
	std::shared_ptr<Entity> SceneManager::CreateEntity()
	{
		return Entity::Create(registry.get());
	}

	void SceneManager::NodeSetup(std::shared_ptr<Node> node)
	{
		node->SetParent(nullptr);
		node->SetSceneManager(this);
		node->ProcessOnReady();
	}

	void SceneManager::RemoveNode(std::shared_ptr<Node> node)
	{
		RemoveNodeFromRootNodeList(node);
		RemoveFromNodeMap(node);
	}

	std::shared_ptr<Node> SceneManager::GetNodePtr(UUID uuid)
	{
		auto it = nodeMap.find(uuid);
		if (it != nodeMap.end())
		{
			return it->second;
		}
		return nullptr;
	}

	void SceneManager::CallReady()
	{
		for (auto& node : rootNodeList)
		{
			node->ProcessOnReady();
		}
	}
	void SceneManager::CallHandleInput(InputEvent e)
	{
		for (auto& node : rootNodeList)
		{
			node->ProcessOnHandleInput(e);
		}
	}
	void SceneManager::CallUpdate(double rDelta)
	{
		for (auto& node : rootNodeList)
		{
			node->ProcessOnUpdate(rDelta);
		}
	}
	void SceneManager::CallPhysicsUpdate(double pDelta)
	{
		for (auto& node : rootNodeList)
		{
			node->ProcessOnPhysicsUpdate(pDelta);
		}
	}
	void SceneManager::CallDraw()
	{
		for (auto& node : rootNodeList)
		{
			node->ProcessOnDraw();
		}
	}

	void SceneManager::AddToNodeMap(std::shared_ptr<Node> node)
	{
		nodeMap[node->GetUUID()] = node;
	}

	void SceneManager::RemoveFromNodeMap(std::shared_ptr<Node> node)
	{
		nodeMap.erase(node->GetUUID());
	}

	void SceneManager::AddNodeToRootNodeList(std::shared_ptr<Node> node)
	{
		rootNodeList.push_back(node);
	}
	void SceneManager::RemoveNodeFromRootNodeList(std::shared_ptr<Node> node)
	{
		auto it = std::remove(rootNodeList.begin(), rootNodeList.end(), node);
		if (it != rootNodeList.end())
		{
			rootNodeList.erase(it, rootNodeList.end());
		}
	}
}