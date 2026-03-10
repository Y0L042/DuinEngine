#pragma once

#include <string>
#include "EditorContext.h"
#include <Duin/Core/Utils/UUID.h>
#include <Duin/Scene/SceneBuilder.h>

using SceneHandle = duin::UUID;

struct Scene
{
    SceneHandle uuid;
    std::string sceneName = "New Scene";
    EditorContext ctx;
    duin::PackedScene packedScene;

    bool IsCtxValid()
    {
        bool res = ctx.editorWorld.get();

        return res;
    }

    std::weak_ptr<EditorWorld> GetWorld()
    {
        if (IsCtxValid())
        {
            return ctx.editorWorld;
        }
        return std::weak_ptr<EditorWorld>();
    }

    bool operator==(Scene &other) const
    {
        bool uuidEq = this->uuid == other.uuid;
        bool pscnEq = this->packedScene.uuid == other.packedScene.uuid;

        bool nameEq = this->sceneName == other.sceneName;
        if (uuidEq)
        {
            if (!nameEq)
            {
                DN_WARN("UUIDs correspond, but not names! {}[{}] vs {}[{}]", this->sceneName, this->uuid.ToStrHex(),
                        other.sceneName, other.uuid.ToStrHex());
            }
            if (!pscnEq)
            {
                DN_WARN("UUIDs correspond, but not PackedScenes! {}[{}] vs {}[{}]", this->sceneName,
                        this->uuid.ToStrHex(),
                        other.sceneName, other.uuid.ToStrHex());
            }   
        }
        return (uuidEq && pscnEq && nameEq);
    }

    bool operator!=(Scene &other) const
    {
        return !(*this == other);
    }
};