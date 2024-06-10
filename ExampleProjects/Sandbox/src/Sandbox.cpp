#include <Duin.h>
#include <Duin/EntryPoint.h>


#include "SandboxGame.h"
Duin::Application* Duin::CreateApplication() { return new Sandbox(); }

//#include "DelaunayTest.h"
//Duin::Application* Duin::CreateApplication() { return new DelaunayTest(); }