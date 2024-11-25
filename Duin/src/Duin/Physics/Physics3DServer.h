#pragma once

#include <PxPhysicsAPI.h>
#include <PxPhysics.h>
#include <characterkinematic/PxController.h>
#include <characterkinematic/PxControllerManager.h>
#include <cooking/PxCooking.h>

#include "Duin/Core/Maths/DuinMaths.h"

#include <vector>

namespace duin {

    struct PhysicsMaterial 
    {
        float staticFriction;
        float dynamicFriction;
        float restitutionCoeff;
    };


    class CharacterBody3D;
    class ControllerHitReport : public physx::PxUserControllerHitReport
    {
    public:
        std::function<void(const physx::PxControllerShapeHit&)> OnShapeHitCallback;
        std::function<void(const physx::PxControllersHit&)> OnControllerHitCallback;
        std::function<void(const physx::PxControllerObstacleHit&)> OnObstacleHitCallback;

        ControllerHitReport(CharacterBody3D& owner);
        virtual ~ControllerHitReport() = default;

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
        CharacterBody3D& owner;
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



    struct CharacterBody3DDesc 
    {
        float height;
        float radius;
        float slopeLimit;
        float stepOffset;
        float contactOffset;
        Vector3 position;
        Vector3 upDirection;
        
        physx::PxCapsuleControllerDesc ToPhysX()
        {
            physx::PxCapsuleControllerDesc pxDesc;

            pxDesc.height = height;
            pxDesc.radius = radius;
            pxDesc.slopeLimit = slopeLimit;
            pxDesc.stepOffset = stepOffset;
            pxDesc.contactOffset = contactOffset;
            pxDesc.position = position.ToPhysXd();
            pxDesc.upDirection = upDirection.ToPhysX();
            
            return pxDesc;
        }
    };

    class Physics3DServer;
    class CharacterBody3D
    {
    public:
        Physics3DServer& server;


        CharacterBody3D(Physics3DServer& server);
        CharacterBody3D(Physics3DServer& server, CharacterBody3DDesc desc, PhysicsMaterial material);
        ~CharacterBody3D();

        const CharacterBody3DDesc GetDescriptor() const;
        physx::PxController* GetPxController() const;

        void SetShapeHitCallback(std::function<void(const physx::PxControllerShapeHit&)> OnShapeHitCallback);
        void SetControllerHitCallback(std::function<void(const physx::PxControllersHit&)> OnControllerHitCallback);
        void SetObstacleHitCallback(std::function<void(const physx::PxControllerObstacleHit&)> OnObstacleHitCallback);

        Vector3 GetPosition();
        Vector3 GetFootPosition();
        Vector3 GetCurrentVelocity();

        void Move(Vector3 displacement);
        int IsOnFloor();
        int IsOnFloorOnly();

        CharacterBody3D(const CharacterBody3D&) = delete;
        CharacterBody3D& operator=(const CharacterBody3D&) = delete;

    private:
        double timeSinceLastMove = 0.0;
        Vector3 currentVelocity;
        int isOnFloor;
        double timeSinceOnFloor;
        double onFloorGrace = 0.1;

        CharacterBody3DDesc desc{
            .height = 1.85f,
            .radius = 0.35f,
            .slopeLimit = std::cosf(physx::PxPi / 4.0),
            .stepOffset = 0.5f,
            .contactOffset = 0.1f,
            .position = Vector3(0.0f, 1.85f / 2.0f, 0.0f),
            .upDirection = Vector3(0.0f, 1.0f, 0.0f),
        };
        ControllerHitReport controllerHitReportCallback;

        physx::PxControllerFilters pxFilters;
        physx::PxController *pxController = NULL;
        physx::PxMaterial *pxMaterial = NULL;
        physx::PxObstacleContext *pxObstacles = NULL;

        int OnFloorRaycast(double delta);

        void CreatePxController(physx::PxCapsuleControllerDesc pxDesc);
    };
    


    struct StaticCollisionPlaneDesc
    {
        
    };

    class Physics3DServer;
    class StaticCollisionPlane
    {
    public:
        Physics3DServer& server;

        StaticCollisionPlane(Physics3DServer& server);

    private:

    };



    class Physics3DServer
    {
    public:
        physx::PxDefaultAllocator pxAllocatorCallback;
        physx::PxDefaultErrorCallback pxErrorCallback;
        physx::PxDefaultCpuDispatcher* pxDispatcher = NULL;

        physx::PxTolerancesScale pxToleranceScale;
        physx::PxFoundation* pxFoundation = NULL;
        physx::PxPhysics* pxPhysics = NULL;

        physx::PxScene* pxScene = NULL;
        physx::PxControllerManager* pxControllerManager = NULL;
        physx::PxMaterial* pxMaterial = NULL;

        physx::PxPvd* pxPvd = NULL;

        Physics3DServer();
        ~Physics3DServer();

        void Initialize();
        void Clean();
        physx::PxMaterial* CreateMaterial(float staticFriction, float dynamicFriction, float restitution);
        physx::PxMaterial* CreateMaterial(PhysicsMaterial material);

        void StepPhysics(double delta);

        Physics3DServer(const Physics3DServer&) = delete;
        Physics3DServer& operator=(const Physics3DServer&) = delete;

    private:

    };

}
