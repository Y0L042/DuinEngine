#pragma once

#include "DNLog.h"

namespace duin {
    #define DNASSERT(condition, errorMsg) \
    do { \
        if (!condition) { DN_CORE_FATAL(errorMsg); } \
        assert(condition); \
    } while(0)
}
