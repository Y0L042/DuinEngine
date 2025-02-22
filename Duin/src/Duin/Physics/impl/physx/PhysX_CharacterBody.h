#pragma once

#include "PhysX_PhysicsBody.h"
#include "Duin/Physics/CharacterBody.h"


#include <PxPhysicsAPI.h>
#include <PxPhysics.h>
#include <characterkinematic/PxController.h>
#include <characterkinematic/PxControllerManager.h>
#include <cooking/PxCooking.h>

namespace duin {
    class PhysXCharacterBody 
        : public PhysXPhysicsBody, public CharacterBody
    {
        public:
             ~PhysXCharacterBody() = default;

            Vector3 GetPosition() override;
            Vector3 GetFootPosition() override;
            Vector3 GetCurrentVelocity() override;
            int IsOnFloor() override;
            int IsOnFloorOnly() override;
            void Move(Vector3 displacement, double delta) override;

            void OnShapeHit(/*TODO*/) override;
            void OnCharacterHit(/*TODO*/) override;
            void OnObstacleHit(/*TODO*/) override;

        private:
    };
}
