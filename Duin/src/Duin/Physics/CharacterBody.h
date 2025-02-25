#pragma once
#include "PhysicsBody.h"

#include <memory>

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
            static std::shared_ptr<CharacterBody> Create();
            static std::shared_ptr<CharacterBody> Create(CharacterBodyDesc desc);

            CharacterBody() = default;
            ~CharacterBody() = default;

            virtual Vector3 GetPosition() = 0;
            virtual Vector3 GetFootPosition() = 0;
            virtual Vector3 GetCurrentVelocity() = 0;
            virtual int IsOnFloor() = 0;
            virtual int IsOnFloorOnly() = 0;
            virtual void Move(Vector3 displacement, double delta) = 0;

            virtual void OnShapeHit(/*TODO*/) {};
            virtual void OnCharacterHit(/*TODO*/) {};
            virtual void OnObstacleHit(/*TODO*/) {};

            const CharacterBodyDesc GetDescriptor() const;

            CharacterBody(const CharacterBody&) = delete;
            CharacterBody& operator=(const CharacterBody&) = delete;

        private:
    };
}
