#pragma once

#include <module_imgui.h>
#include "./modules/moduleDNLog/Module_DnLog.h"
#include "./modules/moduleDoctest/Module_Doctest.h"
#include "./modules/moduleRenderer/Module_DnRenderer.h"
#include "./modules/moduleRenderer/Module_DnCamera.h"
#include "./modules/moduleGameObject/Module_DnGameObject.h"
#include "./modules/moduleGameObject/Module_DnGameStateMachine.h"
#include "./modules/moduleECS/Module_DnECS.h"
#include "./modules/moduleECS/Module_DnPipeline.h"
#include "./modules/moduleInput/Module_DnInput.h"
#include "./modules/modulePhysics/Module_DnPhysicsServer.h"
#include "./modules/modulePhysics/ModuleDnCharacterBody.h"
#include "./modules/moduleApplication/Module_DnApplication.h"
#include "./modules/moduleTOML/Module_TOMLC17.h"

#include <daScript/daScriptModule.h>

// As of daslang v0.6.4 NEED_MODULE only *calls* register_Module_X; it no longer
// declares it. Only the built-in modules are pre-declared (in daScriptModule.h),
// so every separately compiled module needs a DECLARE_MODULE at file scope
// before a NEED_MODULE/PULL_MODULE of it will compile.
DECLARE_MODULE(Module_flecs);
DECLARE_MODULE(Module_imgui);

DECLARE_MODULE(Module_DnLog);
DECLARE_MODULE(Module_Doctest);
DECLARE_MODULE(Module_DnRenderer);
DECLARE_MODULE(Module_DnCamera);
DECLARE_MODULE(Module_DnGameObject);
DECLARE_MODULE(Module_DnGameStateMachine);
DECLARE_MODULE(Module_DnECS);
DECLARE_MODULE(Module_DnPipeline);
DECLARE_MODULE(Module_DnSceneBuilder);
DECLARE_MODULE(Module_DnInput);
DECLARE_MODULE(Module_DnPhysicsServer);
DECLARE_MODULE(Module_DnCharacterBody);
DECLARE_MODULE(Module_DnApplication);
DECLARE_MODULE(Module_DnFilesystem);
DECLARE_MODULE(Module_TOMLC17);
DECLARE_MODULE(Module_DnUUID);

//#include "./ECS/Module_DecsEntity.h"
//#include "./ECS/Module_DecsWorld.h"
//#include "./ECS/Module_DecsComponents.h"
//#include "./ECS/Module_DecsQuery.h"