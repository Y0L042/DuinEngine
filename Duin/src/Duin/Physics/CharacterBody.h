#pragma once

#include "PhysicsBody.h"
#include "Duin/Core/Maths/DuinMaths.h"

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

    class CharacterBody : public PhysicsBody
    {
        public:
            CharacterBody() = default;
            virtual ~CharacterBody() = default;

            virtual Vector3 GetPosition();
            virtual Vector3 GetFootPosition();
            virtual Vector3 GetCurrentVelocity();
            virtual int IsOnFloor();
            virtual int IsOnFloorOnly();
            virtual void Move(Vector3 displacement, double delta);

            virtual void OnShapeHit(/*TODO*/) {};
            virtual void OnCharacterHit(/*TODO*/) {};
            virtual void OnObstacleHit(/*TODO*/) {};

            const CharacterBodyDesc GetDescriptor() const;

            CharacterBody(const CharacterBody&) = delete;
            CharacterBody& operator=(const CharacterBody&) = delete;

        private:
    };
}
