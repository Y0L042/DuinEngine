#pragma once

#include "DNLog.h"

namespace duin 
{
    #define DNASSERT(condition, errorMsg, out) \
        ((!condition) ? { DN_CORE_FATAL(errorMsg); assert(condition); } : out)
}
