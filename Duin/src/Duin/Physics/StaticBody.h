#pragma once

#include <memory>

#include "PhysicsBody.h"
#include "PhysicsMaterial.h"



namespace duin {
    class StaticBody : public PhysicsBody
    {
        public:
            static std::shared_ptr<StaticBody> Create();
            static std::shared_ptr<StaticBody> Create(PhysicsMaterial material);

            StaticBody() = default;
            ~StaticBody() = default;
        private:
    };
}
