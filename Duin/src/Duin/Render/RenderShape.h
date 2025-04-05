#pragma once

#include <cstdint>

#include "Duin/Core/Maths/DuinMaths.h"
#include "Duin/Core/Utils/UUID.h"
#include "RenderGeometry.h"
#include "Color.h"

#include <bgfx/bgfx.h>

#include <memory>
#include <vector>

namespace duin {


    namespace RenderShapeType {
        typedef enum Type {
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
    };

    class RenderShape
    {
        public:
            RenderShape() : geometry(RenderGeometry()) {}
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

            void GenerateVertices();

            BoxRenderGeometry geometry;

            Vector3 position;
            Quaternion rotation;

        private:
    };
}
