#include <Duin.h>

class Sandbox : public Duin::Application
{
public:
	Sandbox() {}
	~Sandbox() {}
};

Duin::Application* Duin::CreateApplication()
{
	return new Application();
}