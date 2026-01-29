#include "dnpch.h"
#include "RenderShape.h"

namespace duin
{
UUID RenderShape::GetUUID()
{
    return uuid;
}

RenderShapeType::Type RenderShape::GetType()
{
    return type;
}

BoxRenderShape::BoxRenderShape()
{
    type = RenderShapeType::BOX;
}

BoxRenderShape::BoxRenderShape(BoxRenderGeometry boxRenderGeometry) : geometry(boxRenderGeometry)
{
    type = RenderShapeType::BOX;
}

Vector3 BoxRenderShape::GetPosition()
{
    return position;
}

void BoxRenderShape::SetPosition(Vector3 newPosition)
{
    position = newPosition;
}

Quaternion BoxRenderShape::GetRotation()
{
    return rotation;
}

void BoxRenderShape::SetRotation(Quaternion newRotation)
{
    rotation = newRotation;
}
} // namespace duin
