#pragma once

#include <Duin.h>

#include "./RenderableComponent.h"
#include "./TransformComponent.h"

struct RenderableCMPManager
{
    static void Update(Duin::Registry* registry) 
    {
        auto view = registry->View<TransformCMP, RenderableCMP>();
        for (auto [entity, transform, renderable] : view.each()) 
        {
            renderable.texture->Draw(transform.position, 0);
        }
    }
};