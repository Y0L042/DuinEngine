#ifndef SANDBOX_H
#define SANDBOX_H

#define CAMERA3D
#include <Duin.h>


class Sandbox : public Duin::Application
{
public:
	Sandbox() {}
	~Sandbox() {}

	void Initialize() override;
	void Ready() override;
	void HandleInputs() override;
	void Update(double rDelta) override;
	void PhysicsUpdate(double pDelta) override;
	void Draw() override;

};

#endif /* SANDBOX_H */
