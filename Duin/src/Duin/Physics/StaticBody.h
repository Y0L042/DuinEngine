#pragma once

#include <memory>

#include "PhysicsBody.h"



namespace duin {
    class StaticBody : public PhysicsBody
    {
        public:
            static std::shared_ptr<StaticBody> Create();

            StaticBody() = default;
            ~StaticBody() = default;
        private:
    };
}
