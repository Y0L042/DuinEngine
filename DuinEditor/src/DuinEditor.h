#ifndef DUIN_EDITOR_H
#define DUIN_EDITOR_H

#include <Duin.h>

class DuinEditor : public duin::Application
{
public:
	DuinEditor() {}
	~DuinEditor() {}

	void Initialize() override;
	void Ready() override;
	void HandleInputs() override;
	void Update(double rDelta) override;
	void PhysicsUpdate(double pDelta) override;
	void Draw() override;
    void DrawUI() override;

};

#endif /* DUIN_EDITOR_H */
