#pragma once
#include "dnpch.h"
// For use by Duin applications only!

// --------------------------------
// Client Application API Headers
// --------------------------------

// ---------- Root -----------

// ---------- Core -----------
#include "Duin/Core/Application.h"
#include "Duin/Core/AppWindow.h"
#include "Duin/Core/Signal.h"
#include "Duin/Core/Debug/Log.h"
#include "Duin/Core/Maths/DuinMaths.h"
#include "Duin/Core/Structures/RenderStructs.h"
#include "Duin/Core/Scene/SceneManager.h"

// ---------- Entities -----------
#include "Duin/Entity/Entity.h"
#include "Duin/Entity/Registry.h"
#include "Duin/Entity/Component/Component.h"

// ---------- Events -----------
#include "Duin/Events/Event.h"
#include "Duin/Events/InputEvent.h"
#include "Duin/Events/InputMap.h"

// ---------- Graphics -----------
#include "Duin/Graphics/TextureRes.h"
#include "Duin/Graphics/TextureLoader.h"

// ---------- Objects -----------
#include "Duin/Object/Object.h"
#include "Duin/Object/ObjectManager.h"
#include "Duin/Object/Blackboard.h"
#include "Duin/Object/Node/Node.h"
#include "Duin/Object/Node/Node2D/Node2D.h"
#include "Duin/Object/Node/Node2D/Sprite2D.h"
#include "Duin/Object/Node/Node3D/Node3D.h"

// ---------- External Dependencies -----------
//#include <RLImGuiComponent.h>