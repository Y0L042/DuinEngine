#pragma once

#include <Duin.h>

#include <CDT.h>

class DelaunayTest : public Duin::Application
{
public:
	DelaunayTest() {}
	~DelaunayTest() {}

	void Initialize() override;
	void Ready() override;
	void HandleInputs(Duin::InputEvent e) override;
	void Update(double rDelta) override;
	void PhysicsUpdate(double pDelta) override;
	void Draw() override;

	Duin::Delaunay delaunayGraph;

};
