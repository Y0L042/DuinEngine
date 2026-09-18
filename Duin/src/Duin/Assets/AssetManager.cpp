#include "dnpch.h"
#include "AssetManager.h"
#include "FileTypes.h"
#include "Duin/IO/Filesystem.h"
#include <memory>


duin::AssetManager::AssetManager() : basePath(fs::GetBasePath())
{
}

duin::AssetManager::~AssetManager()
{
}

duin::UUID duin::AssetManager::GetUUID()
{
    return uuid;
}

bool duin::AssetManager::HasAsset(const std::string &path)
{
    return assetMap.contains(path);
}

duin::AssetManager &duin::AssetManager::Get()
{
    static duin::AssetManager am;
    return am;
}

std::optional<duin::Model> duin::AssetManager::LoadModel(const std::string &path)
{
    AssetManager& am = AssetManager::Get();
    Model model;
    if (am.HasAsset(path))
    {
        model.data = std::static_pointer_cast<ModelData>(am.assetMap[path]);
        return model;
    }
    else
    {
        std::shared_ptr<duin::ModelData> mesh = LoadMesh(path);
        if (mesh)
        {
            am.assetMap[path] = mesh;
            model.data = mesh;
            return model;
        }
    }

    return std::nullopt;
}

std::shared_ptr<duin::ModelData> duin::AssetManager::LoadMesh(const std::string &path)
{
    auto sysPath = fs::MapVirtualToSystemPath(path);
    const aiScene *scene = importer.ReadFile(
        sysPath,
        aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

    // If the import failed, report it
    if (nullptr == scene)
    {
        DN_CORE_FATAL(importer.GetErrorString());
        return nullptr;
    }

    if (!scene->HasMeshes())
    {
        DN_CORE_ERROR("No meshes in {}", path);
        return nullptr;
    }

    auto *mesh = scene->mMeshes[0];
    DN_CORE_INFO("Num vertices: {}", mesh->mNumVertices);

    std::shared_ptr<ModelData> modelData = std::make_shared<ModelData>();
    modelData->vertices.clear();
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
    {
        aiVector3D v = mesh->mVertices[i];
        aiVector3D n = mesh->HasNormals() ? mesh->mNormals[i] : aiVector3D(0.0f, 1.0f, 0.0f);

        // Colour from normal, so the shape is visible with the default shader
        uint8_t r = (uint8_t)((n.x * 0.5f + 0.5f) * 255.0f);
        uint8_t g = (uint8_t)((n.y * 0.5f + 0.5f) * 255.0f);
        uint8_t b = (uint8_t)((n.z * 0.5f + 0.5f) * 255.0f);
        uint32_t abgr = 0xff000000 | (b << 16) | (g << 8) | r;

        modelData->vertices.push_back({v.x, v.y, v.z, abgr});
    }

    modelData->indices.clear();
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        modelData->indices.push_back((uint16_t)face.mIndices[0]);
        modelData->indices.push_back((uint16_t)face.mIndices[2]);
        modelData->indices.push_back((uint16_t)face.mIndices[1]);
    }

    duin::RHIVertexLayout layout;
    layout.Add(duin::RHIAttrib::Position, 3, duin::RHIAttribType::Float)
        .Add(duin::RHIAttrib::Color0, 4, duin::RHIAttribType::Uint8, true);

    modelData->vertexLayout = layout;
    modelData->bufferSize = modelData->vertices.size();
    modelData->vertexBufferHandle = duin::RHICreateVertexBuffer(
        modelData->vertices.data(), (uint32_t)(modelData->vertices.size() * sizeof(duin::PosColorVertex)), modelData->vertexLayout);
    modelData->indexBufferHandle = duin::RHICreateIndexBuffer(modelData->indices.data(), (uint32_t)modelData->indices.size());

    DN_CORE_INFO("Uploaded {} vertices, {} indices", modelData->vertices.size(), modelData->indices.size());

    return modelData;
}