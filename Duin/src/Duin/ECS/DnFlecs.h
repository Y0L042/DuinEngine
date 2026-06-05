#pragma once

#include <flecs.h>
#include <Duin/Core/Debug/DNAssert.h>

namespace duin
{

inline void SetFlecsDaslangAssert()
{
    static bool s_installed = false;
    if (s_installed)
        return;
    ecs_os_set_api_defaults();
    ecs_os_api_t api = ecs_os_get_api();
    api.abort_ = []() { duin::OnCrash("flecs abort"); };
    ecs_os_set_api(&api);
    s_installed = true;
}

} // namespace duin
