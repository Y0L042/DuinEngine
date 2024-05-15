#include "dnpch.h"

#include "Object.h"
#include "Duin/Object/ObjectManager.h"

namespace Duin
{
	Object::Object()
	{
	}

	Object::~Object()
	{
		ConnectSignalsToCallbacks
		(
			onReady,
			onHandleInput,
			onUpdate,
			onPhysicsUpdate,
			onDraw
		);
	}

	void Object::AddChild(std::shared_ptr<Object> child)
	{
		if (child)
		{
			children.push_back(child);
			child->SetParent(shared_from_this());

			child->ConnectSignalsToCallbacks
			(
				onReady,
				onHandleInput,
				onUpdate,
				onPhysicsUpdate,
				onDraw
			);
			child->ProcessOnReady();
		}
	}

	void Object::RemoveChild(std::shared_ptr<Object> child)
	{
		if (child)
		{
			children.erase
			(
				std::remove(children.begin(), children.end(), child),
				children.end()
			);

			child->DisconnectSignalsFromCallbacks
			(
				onReady,
				onHandleInput,
				onUpdate,
				onPhysicsUpdate,
				onDraw
			);

			child->ResetParent();
		}
	}

	void Object::Initialize()
	{
		ObjectManager::AddObject(shared_from_this());
	}

	void Object::ConnectSignalsToCallbacks(Signal<>& i_onReady, Signal<InputEvent>& i_onHandleInput, Signal<double>& i_onUpdate, Signal<double>& i_onPhysicsUpdate, Signal<>& i_onDraw)
	{
		onReadySignalUUID = i_onReady.Connect([this]() { this->ProcessOnReady(); });
		onHandleInputSignalUUID = i_onHandleInput.Connect([this](InputEvent e) { this->ProcessOnHandleInput(e); });
		onUpdateSignalUUID = i_onUpdate.Connect([this](double rDelta) { this->ProcessOnUpdate(rDelta); });
		onPhysicsUpdateSignalUUID = i_onPhysicsUpdate.Connect([this](double pDelta) { this->ProcessOnPhysicsUpdate(pDelta); });
		onDrawSignalUUID = i_onDraw.Connect([this]() { this->ProcessOnDraw(); });
	}

	void Object::DisconnectSignalsFromCallbacks(Signal<>& i_onReady, Signal<InputEvent>& i_onHandleInput, Signal<double>& i_onUpdate, Signal<double>& i_onPhysicsUpdate, Signal<>& i_onDraw)
	{
		i_onReady.Disconnect(onReadySignalUUID);
		i_onHandleInput.Disconnect(onHandleInputSignalUUID);
		i_onUpdate.Disconnect(onUpdateSignalUUID);
		i_onPhysicsUpdate.Disconnect(onPhysicsUpdateSignalUUID);
		i_onDraw.Disconnect(onDrawSignalUUID);
	}

	void Object::ProcessOnReady()
	{
		onReady.Emit();
		Ready();
	}

	void Object::ProcessOnHandleInput(InputEvent e)
	{
		onHandleInput.Emit(e);
		HandleInput(e);
	}

	void Object::ProcessOnUpdate(double rDelta)
	{
		onUpdate.Emit(rDelta);
		Update(rDelta);
	}

	void Object::ProcessOnPhysicsUpdate(double pDelta)
	{
		onPhysicsUpdate.Emit(pDelta);
		PhysicsUpdate(pDelta);
	}

	void Object::ProcessOnDraw()
	{
		onDraw.Emit();
		Draw();
	}
}