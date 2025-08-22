#pragma once

#include "GameObjects/SceneWorld.h"
#include <Duin/Core/Utils/UUID.h>
#include <Duin/Core/Signals/SignalsModule.h>

class Tab;
struct TabBlackboard {
    duin::UUID uuid;
    Tab *tab = nullptr;
    std::shared_ptr<SceneWorld> sceneWorld = nullptr;
    duin::Signal<bool>* onFocusChange = nullptr;
};
