#pragma once

#include <Duin.h>

#include "./RenderableComponent.h"
#include "./TransformComponent.h"

struct RenderableCMPManager
{
    static void Update() 
    {
        auto view = Duin::Registry::GetRegistry().view<TransformCMP, RenderableCMP>();
        for (auto [entity, transform, renderable] : view.each()) 
        {
            renderable.texture->Draw(transform.position);
        }
    }
};