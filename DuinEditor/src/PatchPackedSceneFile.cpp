#include "PatchPackedSceneFile.h"
#include <Duin/Scene/SceneBuilder.h>
#include <Duin/Core/Debug/DNLog.h>

bool EnsurePackedSceneFile(std::shared_ptr<FSNode> node)
{
    duin::JSONValue v = duin::JSONValue::ParseFromFile(node->path);
    duin::SceneBuilder sceneBuilder;
    duin::PackedScene scene = sceneBuilder.DeserializeScene(v);

    for (auto& entity : scene.entities)
    {
        EnsurePackedEntity(entity);
    }

    return false;
}

bool EnsurePackedEntity(duin::PackedEntity &e)
{
    bool res = true;

    if (e.instanceOf.has_value())
    {
        auto &assetRef = e.instanceOf.value();
        if (!ValidateAssetRef(assetRef))
        {
            DN_WARN("Invalid entity {} AssetRef at <{}>!", e.uuid.ToStrHex(), assetRef.rPath);
            res = false;
        }
    }
    
    for (auto& component : e.components)
    {
        if (!EnsurePackedComponent(component))
        {
            DN_WARN("Invalid component {} for entity {}!", component.componentTypeName, e.uuid.ToStrHex());
            res = false;
        }
    }

    for (auto& child : e.children)
    {
        if (!EnsurePackedEntity(child))
        {
            DN_WARN("Invalid child {} for entity {}!", child.uuid.ToStrHex(), e.uuid.ToStrHex());
            res = false;
        }
    }

    return res;
}

bool EnsurePackedComponent(duin::PackedComponent &c)
{
    duin::JSONValue v = duin::JSONValue::Parse(c.jsonData);
    // TODO
    return false;
}

bool ValidateAssetRef(duin::AssetRef &assetRef)
{
    if (assetRef.fileExt == duin::FS_FILEEXT_PSCN)
    {
        // validate scene
        //return EnsurePackedSceneFile();
    }
    else
    {
        // validate other file
    }
    return false;
}
