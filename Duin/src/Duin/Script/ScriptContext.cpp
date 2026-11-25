#include "dnpch.h"
#include "ScriptContext.h"

duin::ScriptContext::~ScriptContext()
{
    Clear();
}

void duin::ScriptContext::TransferMemoryTo(ScriptContext &other)
{
    other.scriptMemory = this->scriptMemory;
    other.rootGameObject = this->rootGameObject;
}

void duin::ScriptContext::Clear()
{
    scriptMemory.reset();

    rootGameObject = nullptr;
}

void duin::ScriptContext::CollectHeap()
{
    das::Context::collectHeap(nullptr, true, true);
}

void duin::ScriptContext::Restart()
{
    das::Context::restart();
}
