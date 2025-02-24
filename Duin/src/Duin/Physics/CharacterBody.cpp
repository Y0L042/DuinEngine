#include "dnpch.h"

#include "CharacterBody.h"

// Use PhysX by default
#ifndef JOLT
#define PHYSICS_BODY PhysXCharacterBody
#include "./impl/physx/PhysX_Includes.h"
#endif

#ifdef JOLT
#ifndef PHYSX
#define PHYSICS_BODY JoltCharacterBody
#endif
#endif

namespace duin {
    CharacterBody* CharacterBody::Create()
    {
        PHYSICS_BODY *body = new PHYSICS_BODY();
        return body;
    }
}
