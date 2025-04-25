#pragma once

/* Abstract base class */
#include "Panel.h"

/* Implementations */
#include "ViewportPanel.h"

namespace panel {
    enum PanelType {
        INVALID = 0,
        VIEWPORT
    };
}
