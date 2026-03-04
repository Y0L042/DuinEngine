#pragma once

#include <vector>
#include <memory>
#include <Duin/Core/Signals/Signal.h>

class UIObject
{
  public:
  void AddConnectionHandle(std::shared_ptr<duin::ScopedConnection> handle);

  private:
	  std::vector<std::shared_ptr<duin::ScopedConnection>> connectionHandles;
};