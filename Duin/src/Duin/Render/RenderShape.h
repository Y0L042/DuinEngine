#pragma once

#include <cstdint>

#include "Duin/Core/Maths/DuinMaths.h"
#include "Duin/Core/Utils/UUID.h"
#include "RenderGeometry.h"
#include "Color.h"

#include <memory>
#include <vector>

namespace duin
{

namespace RenderShapeType
{
typedef enum Type
{
    SPHERE,
    BOX,
    CAPSULE,
    PLANE,
    CONE,
    CYLINDER,
    DISK,
    TRIANGLE,
    TRIANGLEMESH,
    NONE,
    Count
} Type;
}; // namespace RenderShapeType

class RenderShape
{
  public:
    RenderShape() : geometry(RenderGeometry())
    {
    }
    UUID GetUUID();
    RenderShapeType::Type GetType();

  protected:
    UUID uuid;
    RenderGeometry geometry;
    RenderShapeType::Type type = RenderShapeType::NONE;
};

class BoxRenderShape : public RenderShape
{
  public:
    BoxRenderShape();
    BoxRenderShape(BoxRenderGeometry boxRenderGeometry);

    Vector3 GetPosition();
    void SetPosition(Vector3 newPosition);
    Quaternion GetRotation();
    void SetRotation(Quaternion newRotation);

    BoxRenderGeometry geometry;
    Vector3 position;
    Quaternion rotation;
};

class SphereRenderShape : public RenderShape
{
  public:
    SphereRenderShape() { type = RenderShapeType::SPHERE; }
    SphereRenderShape(SphereRenderGeometry g) : geometry(g) { type = RenderShapeType::SPHERE; }

    Vector3    GetPosition() { return position; }
    void       SetPosition(Vector3 p) { position = p; }
    Quaternion GetRotation() { return rotation; }
    void       SetRotation(Quaternion r) { rotation = r; }

    SphereRenderGeometry geometry;
    Vector3    position;
    Quaternion rotation;
};

class CapsuleRenderShape : public RenderShape
{
  public:
    CapsuleRenderShape() { type = RenderShapeType::CAPSULE; }
    CapsuleRenderShape(CapsuleRenderGeometry g) : geometry(g) { type = RenderShapeType::CAPSULE; }

    Vector3    GetPosition() { return position; }
    void       SetPosition(Vector3 p) { position = p; }
    Quaternion GetRotation() { return rotation; }
    void       SetRotation(Quaternion r) { rotation = r; }

    CapsuleRenderGeometry geometry;
    Vector3    position;
    Quaternion rotation;
};

class CylinderRenderShape : public RenderShape
{
  public:
    CylinderRenderShape() { type = RenderShapeType::CYLINDER; }
    CylinderRenderShape(CylinderRenderGeometry g) : geometry(g) { type = RenderShapeType::CYLINDER; }

    Vector3    GetPosition() { return position; }
    void       SetPosition(Vector3 p) { position = p; }
    Quaternion GetRotation() { return rotation; }
    void       SetRotation(Quaternion r) { rotation = r; }

    CylinderRenderGeometry geometry;
    Vector3    position;
    Quaternion rotation;
};

class ConeRenderShape : public RenderShape
{
  public:
    ConeRenderShape() { type = RenderShapeType::CONE; }
    ConeRenderShape(ConeRenderGeometry g) : geometry(g) { type = RenderShapeType::CONE; }

    Vector3    GetPosition() { return position; }
    void       SetPosition(Vector3 p) { position = p; }
    Quaternion GetRotation() { return rotation; }
    void       SetRotation(Quaternion r) { rotation = r; }

    ConeRenderGeometry geometry;
    Vector3    position;
    Quaternion rotation;
};

class DiskRenderShape : public RenderShape
{
  public:
    DiskRenderShape() { type = RenderShapeType::DISK; }
    DiskRenderShape(DiskRenderGeometry g) : geometry(g) { type = RenderShapeType::DISK; }

    Vector3    GetPosition() { return position; }
    void       SetPosition(Vector3 p) { position = p; }
    Quaternion GetRotation() { return rotation; }
    void       SetRotation(Quaternion r) { rotation = r; }

    DiskRenderGeometry geometry;
    Vector3    position;
    Quaternion rotation;
};

class TriangleRenderShape : public RenderShape
{
  public:
    TriangleRenderShape() { type = RenderShapeType::TRIANGLE; }
    TriangleRenderShape(TriangleRenderGeometry g) : geometry(g) { type = RenderShapeType::TRIANGLE; }

    Vector3    GetPosition() { return position; }
    void       SetPosition(Vector3 p) { position = p; }
    Quaternion GetRotation() { return rotation; }
    void       SetRotation(Quaternion r) { rotation = r; }

    TriangleRenderGeometry geometry;
    Vector3    position;
    Quaternion rotation;
};

} // namespace duin
