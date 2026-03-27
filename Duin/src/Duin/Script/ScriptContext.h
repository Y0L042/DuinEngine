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

    ~ScriptContext()
    {
        rootGameObject = nullptr;
    }

    std::shared_ptr<ScriptMemory> scriptMemory;
    GameObject *rootGameObject = nullptr;
};

} // namespace duin