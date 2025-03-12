#pragma once

#include "Duin/Core/Maths/DuinMaths.h"

#include <PxPhysicsAPI.h>

namespace duin {
    typedef struct Transform3D {
        Vector3 translation;
        Quaternion rotation;

        physx::PxTransformT<float> ToPhysX() const
        {
            return physx::PxTransformT<float>(
                                              translation.x,
                                              translation.y,
                                              translation.z,
                                              rotation.ToPhysXQuaternion()
                                              );
        }
    } Transform3D;
}
