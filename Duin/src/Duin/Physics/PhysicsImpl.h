#pragma once 

#ifndef JOLT // i.e. PhysX is used
#include "impl/physx/PhysX_Includes.h"
using  

#elif 
#ifdef JOLT
#define PHYSICS_STATIC JoltStaticBody
#endif

#endif
