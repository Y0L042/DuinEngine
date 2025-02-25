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
    std::shared_ptr<CharacterBody> CharacterBody::Create()
    {
        std::shared_ptr<PHYSICS_BODY> body = std::make_shared<PHYSICS_BODY>();
        std::shared_ptr<CharacterBody> characterBody = std::static_pointer_cast<CharacterBody>(body);

        return characterBody;
    }

    std::shared_ptr<CharacterBody> CharacterBody::Create(CharacterBodyDesc desc)
    {
        std::shared_ptr<PHYSICS_BODY> body = std::make_shared<PHYSICS_BODY>(desc);
        std::shared_ptr<CharacterBody> characterBody = std::static_pointer_cast<CharacterBody>(body);

        return characterBody;
    }
}
