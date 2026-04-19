#include "dnpch.h"
#include "PhysicsDebugRenderer.h"

#include <atomic>

namespace duin
{

class BatchImpl : public JPH::RefTargetVirtual
{
  public:
    JPH_OVERRIDE_NEW_DELETE

    void AddRef() override
    {
        ++mRefCount;
    }
    void Release() override
    {
        if (--mRefCount == 0)
            delete this;
    }

    JPH::Array<JPH::DebugRenderer::Triangle> mTriangles;

  private:
    std::atomic<JPH::uint32> mRefCount = 0;
};

PhysicsDebugRendererCore::Batch PhysicsDebugRendererCore::CreateTriangleBatch(
    const Vertex *inVertices, int inVertexCount, const uint32_t *inIndices, int inIndexCount)
{
    BatchImpl *batch = new BatchImpl;
    if (!inVertices || inVertexCount == 0 || !inIndices || inIndexCount == 0)
        return batch;

    batch->mTriangles.resize(inIndexCount / 3);
    for (int t = 0; t < inIndexCount / 3; ++t)
    {
        Triangle &tri = batch->mTriangles[t];
        tri.mV[0] = inVertices[inIndices[t * 3 + 0]];
        tri.mV[1] = inVertices[inIndices[t * 3 + 1]];
        tri.mV[2] = inVertices[inIndices[t * 3 + 2]];
    }
    return batch;
}

PhysicsDebugRendererCore::Batch PhysicsDebugRendererCore::CreateTriangleBatch(
    const Triangle *inTriangles, int inTriangleCount)
{
    BatchImpl *batch = new BatchImpl;
    if (!inTriangles || inTriangleCount == 0)
        return batch;

    batch->mTriangles.assign(inTriangles, inTriangles + inTriangleCount);
    return batch;
}

void PhysicsDebugRendererCore::DrawGeometry(
    JPH::RMat44Arg inModelMatrix, const JPH::AABox &inWorldSpaceBounds, float inLODScaleSq, JPH::ColorArg inModelColor,
    const GeometryRef &inGeometry, ECullMode inCullMode, ECastShadow inCastShadow, EDrawMode inDrawMode)
{
    if (inGeometry->mLODs.empty())
        return;

    const LOD &lod = inGeometry->mLODs.front();
    const BatchImpl *batch = static_cast<const BatchImpl *>(lod.mTriangleBatch.GetPtr());

    for (const Triangle &triangle : batch->mTriangles)
    {
        JPH::RVec3 v0 = inModelMatrix * JPH::Vec3(triangle.mV[0].mPosition);
        JPH::RVec3 v1 = inModelMatrix * JPH::Vec3(triangle.mV[1].mPosition);
        JPH::RVec3 v2 = inModelMatrix * JPH::Vec3(triangle.mV[2].mPosition);
        JPH::Color color = inModelColor * triangle.mV[0].mColor;

        if (inDrawMode == EDrawMode::Wireframe)
        {
            DrawLine(v0, v1, color);
            DrawLine(v1, v2, color);
            DrawLine(v2, v0, color);
        }
        else
        {
            DrawTriangle(v0, v1, v2, color, inCastShadow);
        }
    }
}

} // namespace duin
