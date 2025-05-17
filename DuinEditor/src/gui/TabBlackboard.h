#pragma once

#include <Duin/Core/Utils/UUID.h>
#include <Duin/Core/Signals/SignalsModule.h>

class Tab;
struct TabBlackboard {
    duin::UUID uuid;
    Tab *tab = nullptr;
    duin::Signal<bool>* onFocusChange = nullptr;
};
