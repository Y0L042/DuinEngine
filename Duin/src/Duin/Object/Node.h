#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Core/Scene/Object.h"
#include "Duin/Core/Scene/SceneManager.h"
#include "Duin/Events/InputEvent.h"
#include "Duin/Events/InputMap.h"

namespace Duin
{
	class DUIN_API Node : public Object
	{
	public:
		Node();
		~Node();

		template<typename T, typename... Args>
		std::shared_ptr<T> CreateChild(Args&&... args)
		{
			if (!SceneManagerValidCheck()) { return; }
			static_assert(std::is_base_of<Node, T>::value, "T must be a Node derived class");
			std::shared_ptr<T> tPtr = std::make_shared<T>(std::forward<Args>(args)...);
			sceneManager->AddToNodeMap(tPtr);
			AddChild(tPtr);
			return tPtr;
		}

		template<typename T>
		std::shared_ptr<T> GetPointer()
		{
			if (!SceneManagerValidCheck()) { return; }
			static_assert(std::is_base_of<Node, T>::value, "T must be an Node derived class");
			std::shared_ptr<Node> selfPtr = sceneManager->GetNodePtr(GetUUID());
			return std::dynamic_pointer_cast<T>(selfPtr);
		}

		void SetNodeAlias(std::string alias);
		std::string GetNodeAlias();

		void AddChild(std::shared_ptr<Node> child);
		void RemoveChild(std::shared_ptr<Node> child);

		std::shared_ptr<Node> GetParent() { return parentPtr; }
		void SetParent(std::shared_ptr<Node> parent) { parentPtr = parent; }
		void ResetParent() { parentPtr.reset(); }

		void SetSceneManager(SceneManager* manager);

		void ProcessOnInitialize();
		void ProcessOnReady();
		void ProcessOnHandleInput(InputEvent e);
		void ProcessOnUpdate(double rDelta);
		void ProcessOnPhysicsUpdate(double pDelta);
		void ProcessOnDraw();

		virtual void Initialize();
		virtual void Ready();
		virtual void HandleInput(InputEvent e);
		virtual void Update(double rDelta);
		virtual void PhysicsUpdate(double pDelta);
		virtual void Draw();

	protected:
		std::vector<std::shared_ptr<Node>> childNodesList;
		std::shared_ptr<Node> parentPtr;

	private:
		std::string nodeAlias = "DefaultNodeAlias";
		SceneManager* sceneManager;

		bool SceneManagerValidCheck();

	};
}