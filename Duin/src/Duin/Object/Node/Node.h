#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Object/Object.h"
#include "Duin/Object/ObjectManager.h"
#include "Duin/Events/InputEvent.h"
#include "Duin/Events/InputMap.h"

namespace Duin
{
	class DUIN_API Node : public Object
	{
	public:
		Node();
		~Node();

		template <typename T, typename... Args>
		static std::shared_ptr<T> Instantiate(Args&&... args)
		{
			static_assert(std::is_base_of<Node, T>::value, "T must be a Node derived class");
			std::shared_ptr<T> objInstance = std::make_shared<T>(std::forward<Args>(args)...);
			std::shared_ptr<Node> objPtr = std::static_pointer_cast<Node>(objInstance);
			ObjectManager::AddNode(objPtr, objPtr->GetUUID());
			return objInstance;
		}

		template<typename T, typename... Args>
		std::shared_ptr<T> InstantiateChild(Args&&... args)
		{
			static_assert(std::is_base_of<Node, T>::value, "T must be a Node derived class");
			std::shared_ptr<T> childSPtr = std::make_shared<T>(std::forward<Args>(args)...);
			std::shared_ptr<Node> castSPtr = std::static_pointer_cast<Node>(childSPtr);
			ObjectManager::AddNode(castSPtr, castSPtr->GetUUID());
			AddChild(castSPtr);
			return childSPtr;
		}

		template<typename T>
		std::shared_ptr<T> GetPointer()
		{
			static_assert(std::is_base_of<Node, T>::value, "T must be an Node derived class");
			std::shared_ptr<Node> selfSPtr = ObjectManager::GetNodePtr(GetUUID());
			return std::dynamic_pointer_cast<T>(selfSPtr);
		}

		void SetNodeAlias(std::string alias);
		std::string GetNodeAlias();

		void AddChild(std::shared_ptr<Node> child);
		void RemoveChild(std::shared_ptr<Node> child);

		std::shared_ptr<Node> GetParent() { return parentPtr; }
		void SetParent(std::shared_ptr<Node> parent) { parentPtr = parent; }
		void ResetParent() { parentPtr.reset(); }

		void ProcessInitialize();
		void ProcessOnReady();
		void ProcessOnHandleInput(InputEvent e);
		void ProcessOnUpdate(double rDelta);
		void ProcessOnPhysicsUpdate(double pDelta);
		void ProcessOnDraw();

		virtual void InternalInitialize();

		virtual void Initialize();
		virtual void Ready();
		virtual void HandleInput(InputEvent e);
		virtual void Update(double rDelta);
		virtual void PhysicsUpdate(double pDelta);
		virtual void Draw();



		// ---- DEFUNCT ---- //
		std::map<std::string, SignalBase*> signalMenu;

		Signal<> onReady;
		Signal<InputEvent> onHandleInput;
		Signal<double> onUpdate;
		Signal<double> onPhysicsUpdate;
		Signal<> onDraw;

		virtual std::map<std::string, SignalBase*>& GetSignalMenu();
		void ConnectSignalsToCallbacks(
			const std::map<std::string, SignalBase*>& signalMenu,
			const std::map<std::string, std::any>& callbacks
		);

		void DisconnectSignalsFromCallbacks(
			const std::map<std::string, SignalBase*>& signalMenu,
			const std::vector<std::string>& signalNames
		);
		virtual void SetSignalMenu();
		virtual void RequestSignalConnections();
		// ---- DEFUNCT ---- //



	protected:
		std::vector<std::shared_ptr<Node>> children;
		std::shared_ptr<Node> parentPtr;

	private:
		std::string nodeAlias = "DefaultNodeAlias";
	};
}