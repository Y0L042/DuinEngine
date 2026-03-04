#include "UIObject.h"

void UIObject::AddConnectionHandle(std::shared_ptr<duin::ScopedConnection> handle)
{
    connectionHandles.push_back(handle);
}
