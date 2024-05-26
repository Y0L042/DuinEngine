#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Core/UUID.h"
#include "Duin/Core/Signal.h"
#include "Duin/Events/InputEvent.h"

#include <unordered_map>
#include <vector>
#include <cinttypes>
#include <any>

namespace Duin
{
	class Node; // Forward Declaration
	class DUIN_API ObjectManager
	{
	public:

		static ObjectManager& GetInstance()
		{
			static ObjectManager* instance = new ObjectManager();
			return *instance;
		}

		static void SetRootNode(std::shared_ptr<Node> root);
		static void AddObject(std::shared_ptr<Node> obj);
		static void RemoveObject(std::shared_ptr<Node> obj);

		static void AddNode(std::shared_ptr<Node> node, UUID uuid);
		static void RemoveNode(UUID uuid);
		static std::shared_ptr<Node> GetNodePtr(UUID uuid);

		static void CallReady();
		static void CallHandleInput(InputEvent e);
		static void CallUpdate(double rDelta);
		static void CallPhysicsUpdate(double pDelta);
		static void CallDraw();

		static std::shared_ptr<Node> GetRootNode();
		
		void ConnectSignalsToCallbacks(
			const std::map<std::string, SignalBase*>& signalMenu,
			const std::map<std::string, std::any>& callbacks
		);

	private:
		Signal<> onReady;
		Signal<InputEvent> onHandleInput;
		Signal<double> onUpdate;
		Signal<double> onPhysicsUpdate;
		Signal<> onDraw;

		std::map<std::string, SignalBase*> signalMenu;

		std::shared_ptr<Node> rootNode;
		std::vector<std::shared_ptr<Node>> objectCollection;
		std::unordered_map<UUID, std::shared_ptr<Node>> nodeMap;

		static void SetSignalMenu();

		ObjectManager();
		~ObjectManager() = default;
		
		void Initialize();
	};
}