#include "LoadModel.h"
#include <Duin/Assets/AssetManager.h>
#include <Duin/Core/Debug/DNLog.h>
#include <Duin/Render/Renderer.h>

std::optional<duin::Model> LoadMesh(const std::string &path)
{
    std::optional<duin::Model> model = duin::AssetManager::Get().LoadModel(path);
    if (!model.has_value())
    {
        DN_FATAL("Model {} failed to load!", path);
    }

    return model;
}

void DrawMesh(const std::optional<duin::Model> &model, duin::Vector3 position, float scale, duin::Quaternion rotation)
{
    if (!model.has_value() || !model->data)
    {
        return;
    }

    const duin::Vector3 &baseScale = model->baseScale;
    duin::Vector3 vScale = duin::Vector3(scale * baseScale.x, scale * baseScale.y, scale * baseScale.z);

    duin::QueueRender(model->data->vertexBufferHandle, model->data->indexBufferHandle, position, rotation, vScale);
}
