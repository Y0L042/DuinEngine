#pragma once

#include <PxPhysicsAPI.h>
#include <PxPhysics.h>
#include <characterkinematic/PxController.h>

#include <functional>

namespace duin {
    class PhysXControllerHitReport : public physx::PxUserControllerHitReport
    {
        public:
            std::function<void(const physx::PxControllerShapeHit&)> OnShapeHitCallback;
            std::function<void(const physx::PxControllersHit&)> OnControllerHitCallback;
            std::function<void(const physx::PxControllerObstacleHit&)> OnObstacleHitCallback;

            PhysXControllerHitReport();
            virtual ~PhysXControllerHitReport() = default;

            virtual void OnShapeHit(const physx::PxControllerShapeHit& hit) {};

            virtual void OnControllerHit(const physx::PxControllersHit& hit) {};

            virtual void OnObstacleHit(const physx::PxControllerObstacleHit& hit) {};

            void onShapeHit(const physx::PxControllerShapeHit& hit) override;
            void onControllerHit(const physx::PxControllersHit& hit) override;
            void onObstacleHit(const physx::PxControllerObstacleHit& hit) override;

            const std::vector<physx::PxControllerShapeHit>& GetShapeHitReports();
            const std::vector<physx::PxControllersHit>& GetControllerHitReports();
            const std::vector<physx::PxControllerObstacleHit>& GetObstacleHitReports();
        private:
            std::vector<physx::PxControllerShapeHit> shapeHitReports_1;
            std::vector<physx::PxControllerShapeHit> shapeHitReports_2;
            std::vector<physx::PxControllerShapeHit> *currentFrameShapeHitReports = NULL;
            std::vector<physx::PxControllerShapeHit> *previousFrameShapeHitReports = NULL;

            std::vector<physx::PxControllersHit> controllerHitReports_1;
            std::vector<physx::PxControllersHit> controllerHitReports_2;
            std::vector<physx::PxControllersHit>* currentFrameControllerHitReports = NULL;
            std::vector<physx::PxControllersHit>* previousFrameControllerHitReports = NULL;

            std::vector<physx::PxControllerObstacleHit> obstacleHitReports_1;
            std::vector<physx::PxControllerObstacleHit> obstacleHitReports_2;
            std::vector<physx::PxControllerObstacleHit>* currentFrameObstacleHitReports = NULL;
            std::vector<physx::PxControllerObstacleHit>* previousFrameObstacleHitReports = NULL;

            void CleanHitReports();

    };
}
