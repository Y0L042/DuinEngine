#pragma once

#include <cstdint>

#include "Duin/Core/Maths/DuinMaths.h"
#include "Duin/Core/Utils/UUID.h"
#include "RenderGeometry.h"
#include "Color.h"

#include <bgfx/bgfx.h>

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

    struct PosColorVertex {
        static bgfx::VertexLayout ms_layout;

        float x, y, z;
        uint32_t abgr;


        static void init()
        {
            ms_layout
                .begin()
                .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
                .add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
                .end();
        };
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
            PosColorVertex vertices[8] = {
                {-1.0f,  1.0f,  1.0f, 0xff000000 },
                { 1.0f,  1.0f,  1.0f, 0xff0000ff },
                {-1.0f, -1.0f,  1.0f, 0xff00ff00 },
                { 1.0f, -1.0f,  1.0f, 0xff00ffff },
                {-1.0f,  1.0f, -1.0f, 0xffff0000 },
                { 1.0f,  1.0f, -1.0f, 0xffff00ff },
                {-1.0f, -1.0f, -1.0f, 0xffffff00 },
                { 1.0f, -1.0f, -1.0f, 0xffffffff }
            };
            uint16_t triList[36] = {    
                0, 1, 2, // 0
                1, 3, 2,
                4, 6, 5, // 2
                5, 6, 7,
                0, 2, 4, // 4
                4, 2, 6,
                1, 5, 3, // 6
                5, 7, 3,
                0, 4, 1, // 8
                4, 5, 1,
                2, 3, 6, // 10
                6, 3, 7
            };
            Vector3 position;
            Quaternion rotation;

        private:
    };
}
