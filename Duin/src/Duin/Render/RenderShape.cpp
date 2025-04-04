#include "dnpch.h"
#include "RenderShape.h"

namespace duin {
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

    BoxRenderShape::BoxRenderShape(BoxRenderGeometry boxRenderGeometry)
        : geometry(boxRenderGeometry)
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

    void BoxRenderShape::GenerateVertices()
    {
        // Given width, height, depth, & position, rotation,
        // generate the box vertices
        float width = 2;
        float height = 2;
        float depth = 2;

        vertices[0] = {-1.0f,  1.0f,  1.0f, 0xff000000 };
        vertices[1] = { 1.0f,  1.0f,  1.0f, 0xff0000ff };
        vertices[2] = {-1.0f, -1.0f,  1.0f, 0xff00ff00 };
        vertices[3] = { 1.0f, -1.0f,  1.0f, 0xff00ffff };
        vertices[4] = {-1.0f,  1.0f, -1.0f, 0xffff0000 };
        vertices[5] = { 1.0f,  1.0f, -1.0f, 0xffff00ff };
        vertices[6] = {-1.0f, -1.0f, -1.0f, 0xffffff00 };
        vertices[7] = { 1.0f, -1.0f, -1.0f, 0xffffffff };

        // for (int i = 0; i < 8; i++) {
        //     vertices[i].x * width/2.0f;
        //     vertices[i].y * height/2.0f;
        // }
    }
}
