#include "dnpch.h"

#include "ObjectManager.h"
#include "Duin/Object/Object.h"

namespace Duin
{
	void ObjectManager::SetRootNode(std::shared_ptr<Object> rootObj)
	{
		auto& instance = GetInstance();
		instance.rootObject = rootObj;
		instance.rootObject->ConnectSignalsToCallbacks
		(
			instance.onReady,
			instance.onHandleInput,
			instance.onUpdate,
			instance.onPhysicsUpdate,
			instance.onDraw
		);
	}

	void ObjectManager::AddObject(std::shared_ptr<Object> obj)
	{
		auto& instance = GetInstance();
		instance.objectCollection.push_back(obj);
	}

	void ObjectManager::RemoveObject(std::shared_ptr<Object> obj)
	{
		auto& instance = GetInstance();
		auto it = std::find_if
		(
			instance.objectCollection.begin(), instance.objectCollection.end(),
			[&obj](const std::shared_ptr<Object>& ob) { return ob == obj; }
		);

		if (it != instance.objectCollection.end())
		{
			instance.objectCollection.erase(it);
		}
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

	Object& ObjectManager::GetRootNode()
	{
		auto& instance = GetInstance();
		return *instance.rootObject;
	}
}