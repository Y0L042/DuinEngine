#pragma once
#include "PhysicsObject.h"

#include "Duin/Core/Maths/DuinMaths.h"

#include <memory>


#include <PxPhysicsAPI.h>
#include <PxPhysics.h>
#include <characterkinematic/PxController.h>
#include <characterkinematic/PxControllerManager.h>
#include <cooking/PxCooking.h>

namespace duin {
    struct CharacterBodyDesc 
    {
        float height;
        float radius;
        float slopeLimit;
        float stepOffset;
        float contactOffset;
        Vector3 position;
        Vector3 upDirection;
    };

    class CharacterBody : public PhysicsObject
    {
        public:
            static std::shared_ptr<CharacterBody> Create(CharacterBodyDesc desc);

            CharacterBody(CharacterBodyDesc desc);
            ~CharacterBody();

            Vector3 GetPosition();
            Vector3 GetFootPosition();
            Vector3 GetCurrentVelocity();
            int IsOnFloor();
            int IsOnFloorOnly();
            void Move(Vector3 displacement, double delta);

            void OnShapeHit(/*TODO*/);
            void OnCharacterHit(/*TODO*/);
            void OnObstacleHit(/*TODO*/);

            int OnFloorShapeCast(double delta);

            const CharacterBodyDesc GetDescriptor() const;

            CharacterBody(const CharacterBody&) = delete;
            CharacterBody& operator=(const CharacterBody&) = delete;

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

            // TODO
            // PhysXControllerHitReport pxControllerHitReport;

            physx::PxControllerFilters pxFilters;
            physx::PxController *pxController = NULL;
            physx::PxMaterial *pxMaterial = NULL;
            physx::PxObstacleContext *pxObstacles = NULL;
    };
}
