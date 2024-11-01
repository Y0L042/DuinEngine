#ifndef GAME_H
#define GAME_H

#define CAMERA3D
#include <Duin.h>


class Game : public Duin::Application
{
public:
	Game() {}
	~Game() {}

	void Initialize() override;
	void Ready() override;
	void HandleInputs() override;
	void Update(double rDelta) override;
	void PhysicsUpdate(double pDelta) override;
	void Draw() override;

};

#endif /* GAME_H */
