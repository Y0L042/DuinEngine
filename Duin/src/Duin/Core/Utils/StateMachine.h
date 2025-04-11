#pragma once

#include <stack>
#include <string>
#include <memory>
#include <functional>

#include "Duin/Core/Utils/UUID.h"



namespace duin {
    class StateMachine;
    class State
    {
        public:
            std::function<void()> Enter;
            std::function<void()> HandleInput;
            std::function<void(double)> Update;
            std::function<void(double)> PhysicsUpdate;
            std::function<void()> Draw;
            std::function<void()> DrawUI;
            std::function<void()> Exit;

            State(StateMachine& owner);
            virtual ~State() = default;

            UUID GetUUID();
            StateMachine& GetOwner();

            const std::string GetName();

            bool operator==(const State &state) const
            {
                return this->uuid == state.uuid;
            }

            bool operator!=(const State &state) const
            {
                return this->uuid != state.uuid;
            }

        protected:
            UUID uuid;
            std::string stateName;
            StateMachine& owner;
    };



    class StateMachine
    {
        public:

            StateMachine();
            ~StateMachine();

            template<typename T>
            void SwitchState()
            {
                static_assert(std::is_base_of<State, T>::value, "T must derive from State");
                if (!stateStack.empty()) {
                    if (stateStack.top()->Exit) {
                        stateStack.top()->Exit();
                    }
                    if (!stateStack.empty()) {
                        stateStack.pop();
                    }
                }
                auto newState = std::make_unique<T>(*this);
                stateStack.emplace(std::move(newState));
                if (stateStack.top()->Enter) {
                    stateStack.top()->Enter();
                }
            }

            template<typename T>
            void FlushAndSwitchState()
            {
                static_assert(std::is_base_of<State, T>::value, "T must derive from State");
                while (!stateStack.empty()) {
                    if (stateStack.top()->Exit) {
                        stateStack.top()->Exit();
                    }
                    if (!stateStack.empty()) {
                        stateStack.pop();
                    }
                }
                auto newState = std::make_unique<T>(*this);
                stateStack.emplace(std::move(newState));
                if (stateStack.top()->Enter) {
                    stateStack.top()->Enter();
                }
            }

            template<typename T>
            void PushState()
            {
                static_assert(std::is_base_of<State, T>::value, "T must derive from State");
                auto newState = std::make_unique<T>(*this);
                stateStack.emplace(std::move(newState));
                if (stateStack.top()->Enter) {
                    stateStack.top()->Enter();
                }
            }

            UUID GetUUID();

            void PopState();
            void FlushStack();
            
            void ExecuteHandleInput();
            void ExecuteUpdate(double delta);
            void ExecutePhysicsUpdate(double delta);
            void ExecuteDraw();
            void ExecuteDrawUI();

        private:
            UUID uuid;
            std::stack<std::unique_ptr<State>> stateStack;

    };



}
