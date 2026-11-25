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

class ScriptContext : public das::Context
{
  public:
    using das::Context::Context;

    ~ScriptContext();

    void TransferMemoryTo(ScriptContext &other);
    void Clear();

    /*
     *  Only collect heap when NO daslang code is being executed!
     */
    void CollectHeap();

    void Restart();

    std::shared_ptr<ScriptMemory> scriptMemory;
    GameObject* rootGameObject = nullptr;
};

} // namespace duin
