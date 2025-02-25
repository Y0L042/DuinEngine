#pragma once

#include "Duin/Physics/PhysicsBody.h"
#include "PhysX_CollisionObject.h"
#include "PhysX_ControllerHitReport.h"

namespace duin {
    class PhysXPhysicsBody 
        : public PhysXCollisionObject, virtual public PhysicsBody
    {
        public:

        protected:
    };
}
