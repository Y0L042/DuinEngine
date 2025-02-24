#include "dnpch.h"
#include "PhysX_ControllerHitReport.h"

#include "Duin/Core/Application.h"

constexpr int MAX_HIT_REPORTS = 8;

namespace duin {
    PhysXControllerHitReport::PhysXControllerHitReport()
    {
        // TODO
        // QueuePostFrameCallback(std::function<void(void)>([this]() { CleanHitReports(); }));

        currentFrameShapeHitReports = &shapeHitReports_1;
        previousFrameShapeHitReports = &shapeHitReports_2;

        currentFrameControllerHitReports = &controllerHitReports_1;
        previousFrameControllerHitReports = &controllerHitReports_2;

        currentFrameObstacleHitReports = &obstacleHitReports_1;
        previousFrameObstacleHitReports = &obstacleHitReports_2;
    }

    void PhysXControllerHitReport::onShapeHit(const physx::PxControllerShapeHit& hit)
    {
        OnShapeHit(hit);
        if (OnShapeHitCallback) 
        {
            OnShapeHitCallback(hit);
        }
        if (currentFrameShapeHitReports->size() < MAX_HIT_REPORTS)
        {
            currentFrameShapeHitReports->push_back(hit);
        }
    }

    void PhysXControllerHitReport::onControllerHit(const physx::PxControllersHit& hit)
    {
        OnControllerHit(hit);
        if (OnControllerHitCallback)
        {
            OnControllerHitCallback(hit);
        }
        if (currentFrameControllerHitReports->size() < MAX_HIT_REPORTS)
        {
            currentFrameControllerHitReports->push_back(hit);
        }
    }

    void PhysXControllerHitReport::onObstacleHit(const physx::PxControllerObstacleHit& hit)
    {
        OnObstacleHit(hit);
        if (OnObstacleHitCallback)
        {
            OnObstacleHitCallback(hit);
        }
        if (currentFrameObstacleHitReports->size() < MAX_HIT_REPORTS)
        {
            currentFrameObstacleHitReports->push_back(hit);
        }
    }

    const std::vector<physx::PxControllerShapeHit>& PhysXControllerHitReport::GetShapeHitReports()
    {
        return *previousFrameShapeHitReports;
    }

    const std::vector<physx::PxControllersHit>& PhysXControllerHitReport::GetControllerHitReports()
    {
        return *previousFrameControllerHitReports;
    }

    const std::vector<physx::PxControllerObstacleHit>& PhysXControllerHitReport::GetObstacleHitReports()
    {
        return *previousFrameObstacleHitReports;
    }

    void PhysXControllerHitReport::CleanHitReports()
    {
        previousFrameShapeHitReports->clear();
        previousFrameControllerHitReports->clear();
        previousFrameObstacleHitReports->clear();

        auto* swapShapePtrs = currentFrameShapeHitReports;
        currentFrameShapeHitReports = previousFrameShapeHitReports;
        previousFrameShapeHitReports = swapShapePtrs;

        auto* swapControllerPtrs = currentFrameControllerHitReports;
        currentFrameControllerHitReports = previousFrameControllerHitReports;
        previousFrameControllerHitReports = swapControllerPtrs;

        auto* swapObstaclePtrs = currentFrameObstacleHitReports;
        currentFrameObstacleHitReports = previousFrameObstacleHitReports;
        previousFrameObstacleHitReports = swapObstaclePtrs;
    }
}
