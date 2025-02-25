#include "dnpch.h"
#include "StaticBody.h"

// Use PhysX by default
#ifndef JOLT
#define PHYSICS_STATIC PhysXStaticBody
#include "./impl/physx/PhysX_Includes.h"
#endif

#ifdef JOLT
#ifndef PHYSX
#define PHYSICS_STATIC JoltStaticBody
#endif
#endif

namespace duin {
    std::shared_ptr<StaticBody> StaticBody::Create()
    {
        std::shared_ptr<PHYSICS_STATIC> body= std::make_shared<PHYSICS_STATIC>();
        return body;
    }

}
