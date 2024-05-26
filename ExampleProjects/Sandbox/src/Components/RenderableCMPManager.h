#pragma once

#include <Duin.h>

#include "./RenderableComponent.h"
#include "./TransformComponent.h"

struct RenderableCMPManager : entt::process<RenderableCMPManager, uint32_t>
{
    static void update(uint32_t delta) 
    {
        auto view = Duin::Registry::GetRegistry().view<TransformCMP, RenderableCMP>();
        for (auto [entity, transform, renderable] : view.each()) 
        {
            renderable.texture->Draw(transform.position);
        }
    }
};