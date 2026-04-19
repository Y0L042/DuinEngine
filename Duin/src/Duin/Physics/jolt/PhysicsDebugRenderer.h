#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Renderer/DebugRenderer.h>
#include "Duin/Core/Maths/DuinMaths.h"
#include "Duin/Render/Renderer.h"
#include "JoltConversions.h"

namespace duin
{
/*
        DrawLine
        DrawTriangle
        DrawText3D
        CreateTriangleBatch
        DrawGeometry
*/
class PhysicsDebugRendererCore : public JPH::DebugRenderer
{
  public:
    PhysicsDebugRendererCore()
    {
    }

    ~PhysicsDebugRendererCore()
    {
    }

    void Initialize()
    {
        JPH::DebugRenderer::Initialize();
    }

    void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override
    {
        RHIDebugDrawSetColor(inColor.GetUInt32());
        RHIDebugDrawLine(
            (float)inFrom.GetX(),
            (float)inFrom.GetY(),
            (float)inFrom.GetZ(),
            (float)inTo.GetX(),
            (float)inTo.GetY(),
            (float)inTo.GetZ());
    }

    void DrawTriangle(
        JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor,
        ECastShadow = ECastShadow::Off) override
    {
        RHIDebugDrawSetColor(inColor.GetUInt32());
        DrawDebugTriangle(FromJPHVec3(JPH::Vec3(inV1)), FromJPHVec3(JPH::Vec3(inV2)), FromJPHVec3(JPH::Vec3(inV3)));
    }

    void DrawText3D(
        JPH::RVec3Arg inPosition, const JPH::string_view &inString, JPH::ColorArg inColor = JPH::Color::sWhite,
        float inHeight = 0.5f) override
    {
    }

    Batch CreateTriangleBatch(
        const Vertex *inVertices, int inVertexCount, const uint32_t *inIndices, int inIndexCount) override;

    Batch CreateTriangleBatch(const Triangle *inTriangles, int inTriangleCount) override;

    void DrawGeometry(
        JPH::RMat44Arg inModelMatrix, const JPH::AABox &inWorldSpaceBounds, float inLODScaleSq,
        JPH::ColorArg inModelColor, const GeometryRef &inGeometry, ECullMode inCullMode = ECullMode::CullBackFace,
        ECastShadow inCastShadow = ECastShadow::On, EDrawMode inDrawMode = EDrawMode::Solid) override;

  private:
};

class PhysicsDebugRenderer
{
  public:
    void Initialize()
    {
        core.Initialize();
    }

  private:
    friend class PhysicsServer;

    PhysicsDebugRendererCore core;
};
} // namespace duin