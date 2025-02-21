#pragma once

namespace duin {
    enum PhysicsServerError {
        SUCCESS = 0,
        INIT_FAILED,
        MATERIAL_CREATION_FAILED
    };

    class PhysicsServer
    {
        public:
            static PhysicsServer& Get();
            static int CatchError(void *ptr, int errorCode);

            PhysicsServer() = default;
            virtual ~PhysicsServer() = default;

            virtual void Initialize() = 0;
            virtual void Clean() = 0;

            virtual void StepPhysics(double delta) = 0;

        private:
            static void PrintErrorCode(int errorCode);
    };
}

