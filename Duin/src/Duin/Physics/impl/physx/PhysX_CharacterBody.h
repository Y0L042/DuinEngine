#pragma once

#include "PhysX_PhysicsBody.h"
#include "Duin/Physics/CharacterBody.h"


#include <PxPhysicsAPI.h>
#include <PxPhysics.h>
#include <characterkinematic/PxController.h>
#include <characterkinematic/PxControllerManager.h>
#include <cooking/PxCooking.h>


namespace duin {
    class PhysXPhysicsServer;
    class PhysXCharacterBody 
        : public PhysXPhysicsBody, virtual public CharacterBody
    {
        public:
            PhysXCharacterBody();
            PhysXCharacterBody(CharacterBodyDesc desc);
             ~PhysXCharacterBody();

            void SetCollisionShape() override;
            Vector3 GetPosition() override;
            Vector3 GetFootPosition() override;
            Vector3 GetCurrentVelocity() override;
            int IsOnFloor() override;
            int IsOnFloorOnly() override;
            void Move(Vector3 displacement, double delta) override;

            void OnShapeHit(/*TODO*/) override;
            void OnCharacterHit(/*TODO*/) override;
            void OnObstacleHit(/*TODO*/) override;

            int OnFloorShapeCast(double delta);

        private:
            Vector3 currentVelocity;
            double onFloorGrace = 0.1;
            double timeWhenLastMoved;
            double timeSinceOnFloor;
            int isOnFloor;

            CharacterBodyDesc desc{
                .height = 1.85f,
                .radius = 0.35f,
                .slopeLimit = std::cosf(physx::PxPi / 4.0),
                .stepOffset = 0.5f,
                .contactOffset = 0.1f,
                .position = Vector3(0.0f, 1.85f / 2.0f, 0.0f),
                .upDirection = Vector3(0.0f, 1.0f, 0.0f),
            };
            PhysXControllerHitReport pxControllerHitReport;

            physx::PxControllerFilters pxFilters;
            physx::PxController *pxController = NULL;
            physx::PxMaterial *pxMaterial = NULL;
            physx::PxObstacleContext *pxObstacles = NULL;
    };
}
