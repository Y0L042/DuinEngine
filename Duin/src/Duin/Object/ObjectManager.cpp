#include "dnpch.h"

#include "ObjectManager.h"
#include "Duin/Object/Node/Node.h"

namespace Duin
{
	void ObjectManager::SetRootNode(std::shared_ptr<Node> rootObj)
	{
		auto& instance = GetInstance();
		SetSignalMenu();
		instance.AddObject(rootObj);
		instance.rootNode = rootObj;
		instance.rootNode->ProcessInitialize();
	}

	void ObjectManager::AddObject(std::shared_ptr<Node> obj)
	{
		auto& instance = GetInstance();
		instance.objectCollection.push_back(obj);
	}

	void ObjectManager::RemoveObject(std::shared_ptr<Node> obj)
	{
		auto& instance = GetInstance();
		auto it = std::find_if
		(
			instance.objectCollection.begin(), instance.objectCollection.end(),
			[&obj](const std::shared_ptr<Node>& ob) { return ob == obj; }
		);

		if (it != instance.objectCollection.end())
		{
			instance.objectCollection.erase(it);
		}
	}

	void ObjectManager::AddNode(std::shared_ptr<Node> node, UUID uuid)
	{
		auto& instance = GetInstance();
		instance.nodeMap[uuid] = node;
	}

	void ObjectManager::RemoveNode(UUID uuid)
	{
		auto& instance = GetInstance();
		instance.nodeMap.erase(uuid);
	}

	void ObjectManager::CallReady()
	{
		auto& instance = GetInstance();
		instance.onReady.Emit();
	}

	void ObjectManager::CallHandleInput(InputEvent e)
	{
		auto& instance = GetInstance();
		instance.onHandleInput.Emit(e);
	}

	void ObjectManager::CallUpdate(double rDelta)
	{
		auto& instance = GetInstance();
		instance.onUpdate.Emit(rDelta);
	}

	void ObjectManager::CallPhysicsUpdate(double pDelta)
	{
		auto& instance = GetInstance();
		instance.onPhysicsUpdate.Emit(pDelta);
	}

	void ObjectManager::CallDraw()
	{
		auto& instance = GetInstance();
		instance.onDraw.Emit();
	}

	std::shared_ptr<Node> ObjectManager::GetRootNode()
	{
		auto& instance = GetInstance();
		return instance.rootNode;
	}
	void ObjectManager::ConnectSignalsToCallbacks(
		const std::map<std::string, SignalBase*>& signalMenu,
		const std::map<std::string, std::any>& callbacks
	)
	{
		for (const auto& callbackPair : callbacks)
		{
			const std::string& signalName = callbackPair.first;
			auto it = signalMenu.find(signalName);
			if (it != signalMenu.end())
			{
				it->second->ConnectCallback(callbackPair.second);
			}
			else
			{
				std::cerr << "Warning: Parent does not have signal: " << signalName << std::endl;
			}
		}
	}

	std::shared_ptr<Node> ObjectManager::GetNodePtr(UUID uuid) // Add a way for Nodes to get pointers to themselves
	{
		auto& instance = GetInstance();
		return instance.nodeMap.at(uuid);
	}

	void ObjectManager::SetSignalMenu()
	{
		auto& instance = GetInstance();
		instance.signalMenu["READY"] = new SignalWrapper<>(instance.onReady);
		instance.signalMenu["HANDLEINPUT"] = new SignalWrapper<InputEvent>(instance.onHandleInput);
		instance.signalMenu["UPDATE"] = new SignalWrapper<double>(instance.onUpdate);
		instance.signalMenu["PHYSICSUPDATE"] = new SignalWrapper<double>(instance.onPhysicsUpdate);
		instance.signalMenu["DRAW"] = new SignalWrapper<>(instance.onDraw);
	}
}