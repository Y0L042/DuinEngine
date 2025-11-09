#pragma once

#include "GameObjects/SceneWorld.h"
#include <Duin/Core/Utils/UUID.h>
#include <Duin/Core/Signals/SignalsModule.h>
#include "TabSignals.h"
#include <memory>

class Tab;
struct TabBlackboard {
    duin::UUID uuid;
    Tab *tab = nullptr;
    std::shared_ptr<SceneWorld> sceneWorld = nullptr;

    // Tab signals
    std::shared_ptr<TabSignals> signals = nullptr;
};
