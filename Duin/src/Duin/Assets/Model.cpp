#include "dnpch.h"
#include "Model.h"
#include <Duin/Core/Maths/DuinMaths.h>
#include <Duin/Render/Renderer.h>
#include <Duin/Core/Debug/DNLog.h>

void duin::Model::Draw(Vector3 position, Quaternion rotation, Vector3 scale) const
{
    if (data)
    {
        Vector3 vScale(scale.x * baseScale.x, scale.y * baseScale.y, scale.z * baseScale.z);
        QueueRender(data->vertexBufferHandle, data->indexBufferHandle, position, rotation, vScale);
    }
    else
    {
        DN_CORE_WARN("No valid Data for Model!");
    }
}
