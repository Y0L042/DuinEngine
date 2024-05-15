#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Core/UUID.h"
#include "Duin/Core/Signal.h"
#include "Duin/Events/InputEvent.h"

#include <vector>
#include <cinttypes>

namespace Duin
{
	class Object; // Forward Declaration
	class DUIN_API ObjectManager
	{
	public:

		static void SetRootNode(std::shared_ptr<Object> rootObj);

		static void AddObject(std::shared_ptr<Object> obj);
		static void RemoveObject(std::shared_ptr<Object> obj);

		static void CallReady();
		static void CallHandleInput(InputEvent e);
		static void CallUpdate(double rDelta);
		static void CallPhysicsUpdate(double pDelta);
		static void CallDraw();

		static ObjectManager& GetInstance()
		{
			static ObjectManager* instance = new ObjectManager();
			return *instance;
		}

		static Object& GetRootNode();

	private:
		Signal<> onReady;
		Signal<InputEvent> onHandleInput;
		Signal<double> onUpdate;
		Signal<double> onPhysicsUpdate;
		Signal<> onDraw;

		std::shared_ptr<Object> rootObject;
		std::vector<std::shared_ptr<Object>> objectCollection;

		ObjectManager() = default;
		~ObjectManager() = default;
	};
}