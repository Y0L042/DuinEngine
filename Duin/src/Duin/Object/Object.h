#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Core/UUID.h"
#include "Duin/Core/Signal.h"
#include "Duin/Events/InputEvent.h"

namespace Duin
{
	class DUIN_API Object : public std::enable_shared_from_this<Object>
	{
	public:
		Object();
		~Object();

		template <typename T, typename... Args>
		static std::shared_ptr<T> Instantiate(Args&&... args)
		{
			static_assert(std::is_base_of<Object, T>::value, "T must be an Object derived class");
			std::shared_ptr<T> objInstance = std::make_shared<T>(std::forward<Args>(args)...);
			std::shared_ptr<Object> objPtr = std::static_pointer_cast<Object>(objInstance);
			objPtr->Initialize();
			
			return objInstance;
		}

		template<typename T, typename... Args>
		std::shared_ptr<T> InstantiateChild(Args&&... args)
		{
			std::shared_ptr<T> objPtr = Instantiate<T>(std::forward<Args>(args)...);
			AddChild(objPtr);
			return objPtr;
		}

		template<typename T>
		T& GetRef()
		{
			return *this;
		}

		
		void AddChild(std::shared_ptr<Object> child);
		void RemoveChild(std::shared_ptr<Object> child);

		std::shared_ptr<Object> GetParent()
		{
			return parentObj;
		}

		void SetParent(std::shared_ptr<Object> parent)
		{
			parentObj = parent;
		}

		void ResetParent()
		{
			parentObj.reset();
		}

		void ConnectSignalsToCallbacks
		(
			Signal<>& i_onReady,
			Signal<InputEvent>& i_onHandleInput,
			Signal<double>& i_onUpdate,
			Signal<double>& i_onPhysicsUpdate,
			Signal<>& i_onDraw
		);

		void DisconnectSignalsFromCallbacks
		(
			Signal<>& i_onReady,
			Signal<InputEvent>& i_onHandleInput,
			Signal<double>& i_onUpdate,
			Signal<double>& i_onPhysicsUpdate,
			Signal<>& i_onDraw
		);

		template <typename T>
		std::shared_ptr<T> GetPointer() { return shared_from_this(); } 

		virtual void Initialize();

		const UUID& GetUUID() { return objectUUID; }

	protected:
		Signal<> onReady;
		Signal<InputEvent> onHandleInput;
		Signal<double> onUpdate;
		Signal<double> onPhysicsUpdate;
		Signal<> onDraw;

		void ProcessOnReady();
		void ProcessOnHandleInput(InputEvent e);
		void ProcessOnUpdate(double rDelta);
		void ProcessOnPhysicsUpdate(double pDelta);
		void ProcessOnDraw();

		virtual void Ready() = 0;
		virtual void HandleInput(InputEvent e) = 0;
		virtual void Update(double rDelta) = 0;
		virtual void PhysicsUpdate(double pDelta) = 0;
		virtual void Draw() = 0;

	private:
		UUID onReadySignalUUID;
		UUID onHandleInputSignalUUID;
		UUID onUpdateSignalUUID;
		UUID onPhysicsUpdateSignalUUID;
		UUID onDrawSignalUUID;

		UUID objectUUID;
		std::shared_ptr<Object> parentObj;
		std::vector<std::shared_ptr<Object>> children;
	};
}