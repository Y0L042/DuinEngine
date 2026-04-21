#pragma once

#include <daScript/daScript.h>
#include <string>
#include <functional>
#include "ScriptMemory.h"
#include <memory>
#include <Duin/Objects/GameObject.h>
#include <daScript/simulate/simulate.h>

namespace duin
{

class GameWorld;

class ScriptContext : public das::Context
{
  public:
    using das::Context::Context;

    ~ScriptContext()
    {
        rootGameObject = nullptr;
        gameWorld = nullptr;
    }

    void TransferMemoryTo(ScriptContext &other)
    {
        other.scriptMemory = this->scriptMemory;
        other.rootGameObject = this->rootGameObject;
        other.gameWorld = this->gameWorld;
    }

    void Clear()
    {
        scriptMemory.reset();
        rootGameObject = nullptr;
        gameWorld = nullptr;
    }

    std::shared_ptr<ScriptMemory> scriptMemory;
    GameObject *rootGameObject = nullptr;
    GameWorld *gameWorld = nullptr;
};

} // namespace duin