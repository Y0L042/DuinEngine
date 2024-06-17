#include "Handlers.h"

void Handler_PlayerInput::Update(Duin::InputEvent e)
{
	auto view = registry->View<Component_PlayerInput, Component_Movement>();
	for (auto [entity, c_pinput, c_movement] : view.each())
	{
		c_movement.targetSpeed = 0;
		if (e.IsKeyDown(Duin::KEY_W))
		{
			c_movement.targetSpeed += c_movement.maxLinearSpeed;
		}
		if (e.IsKeyDown(Duin::KEY_S))
		{
			c_movement.targetSpeed -= c_movement.maxLinearSpeed * c_movement.brakingFactor;
		}

		c_movement.targetRotation = c_movement.currRotation;
		if (e.IsKeyDown(Duin::KEY_A))
		{
			c_movement.targetRotation -= c_movement.maxRotationSpeed;
		}
		if (e.IsKeyDown(Duin::KEY_D))
		{
			c_movement.targetRotation += c_movement.maxRotationSpeed;
		}

		c_movement.inputVel = Duin::Vector2::AngleToVector2(Duin::Maths::DegreesToRadians(c_movement.targetRotation - 90.0f))
			* c_movement.targetSpeed;
	}
}

void Handler_PlayerMovement::Update(double delta)
{
	auto view = registry->View<Component_PlayerInput, Component_Movement>();
	for (auto [entity, c_pinput, c_movement] : view.each())
	{
		c_movement.prevVelocity = c_movement.currVelocity;
		c_movement.currVelocity += c_movement.inputVel.Normalized() * c_movement.accelerationFactor * delta;
		c_movement.currVelocity.LimitLength(0.0f, c_movement.maxLinearSpeed);
		c_movement.currRotation = c_movement.targetRotation;
		c_movement.currSpeed = c_movement.currVelocity.Length();
	}
}

void Handler_Renderable::Update()
{
	auto view = registry->View<Component_Renderable, Component_Transform>();
	for (auto [entity, c_renderable, c_transform] : view.each())
	{
		c_renderable.texture->Draw(c_transform.position, 0);
	}
}

void Handler_RenderableEntity::Update()
{
	auto view = registry->View<Component_Movement, Component_Renderable, Component_Transform>();
	for (auto [entity, c_movement, c_renderable, c_transform] : view.each())
	{
		//static std::queue<Duin::Vector2> prevPositionsQ;
		//prevPositionsQ.push(c_transform.position);
		//if (prevPositionsQ.size() > 5000)
		//{
		//	prevPositionsQ.pop();
		//}
		//std::queue<Duin::Vector2> tempQ = prevPositionsQ;
		//while(!tempQ.empty())
		//{
		//	Duin::DebugDraw::DrawCircle(tempQ.front(), 5, Duin::Color{222, 180, 180, 255});
		//	tempQ.pop();
		//}

		float rot = 0.0f;
		if (c_renderable.pointInInputDir)
		{
			rot = c_movement.currRotation;
		}
		//Duin::DebugDraw::DrawCircle(c_transform.position, 5, Duin::Color{ 0, 228, 48, 255 });
		c_renderable.texture->Draw(c_transform.position, rot);
	}
}

void Handler_UpdateTransform::Update(double delta)
{
	auto view = registry->View<Component_Transform, Component_Movement>();
	for (auto [entity, c_transform, c_movement] : view.each())
	{
		c_transform.prevPosition = c_transform.position;

		// Update the position based on current velocity and delta time
		c_transform.position += c_movement.currVelocity * delta;

		// Wrap around logic
		if (c_transform.position.x < 0)
		{
			c_transform.position.x = width;
		}
		else if (c_transform.position.x > width)
		{
			c_transform.position.x = 0;
		}

		if (c_transform.position.y < 0)
		{
			c_transform.position.y = height;
		}
		else if (c_transform.position.y > height)
		{
			c_transform.position.y = 0;
		}
	}
}

void Handler_ProcessBoids::Update(double delta)
{
	/*
	* for local_boids (exl this):
	*	cohesion: move towards boids' avg position
	*	alignment: get boids' avg velocity
	*	separation: get avg vec away from boids position
	*/

	auto view = registry->View<Component_Boid, Component_Movement, Component_Transform>();
	for (auto [entity, c_boid, c_movement, c_transform] : view.each())
	{
		const float MXNGHBR = 10.0f;
		const float MXSEP = 4.0f;
			
		const float MAXNEIGHBORDISTSQRD = MXNGHBR * MXNGHBR;
		const float MAXSEPARATIONDISTSQRD = MXSEP * MXSEP;

		Duin::Vector2 cohesion = Duin::Vector2::ZERO;
		Duin::Vector2 alignment = Duin::Vector2::ZERO;
		Duin::Vector2 separation = Duin::Vector2::ZERO;
		int neighborCount = 0;
		int separationNeighborCount = 0;;

		for (auto [otherEntity, c_otherBoid, c_otherMovement, c_otherTransform] : view.each())
		{
			if (entity == otherEntity) { continue; }

			Duin::Vector2 deltaPos = c_otherTransform.position - c_transform.position;
			Duin::Vector2 deltaPosNorm = deltaPos.Normalized();
			float distSqrd = deltaPos.LengthSqrd();
			if (distSqrd < MAXNEIGHBORDISTSQRD)
			{
				neighborCount++;
																			// Cohesion
				cohesion += deltaPosNorm;
				alignment += c_otherMovement.currVelocity.Normalized();		// Alignment

				if (distSqrd < MAXSEPARATIONDISTSQRD)
				{
					separationNeighborCount++;
					separation -= deltaPosNorm;								// Separation
				}
			}
		}

		if (neighborCount > 0)
		{
			cohesion /= neighborCount;
			alignment /= neighborCount;
			
			if (separationNeighborCount > 0)
			{
				separation /= separationNeighborCount;
			}
		}

		cohesion *= c_boid.cohesionFactor;
		alignment *= c_boid.alignmentFactor;
		separation *= c_boid.separationFactor;

		Duin::Vector2 sum = (cohesion + alignment + separation) / 3.0f;
		Duin::Vector2 inputVel = (sum * c_movement.maxLinearSpeed) - c_movement.currVelocity;

		c_movement.inputVelocityList.push_back(std::pair(inputVel, 5.0f)); // Output
			
		//DN_INFO("Boid Process: ({0} , {1}) | {2}", inputVel.x, inputVel.y, neighborCount);
	}
}

void Handler_PlayerBoidMovement::Update(double delta)
{
	auto view = registry->View<Component_Boid, Component_Movement>();
	for (auto [entity, c_boid, c_movement] : view.each())
	{
		c_movement.prevVelocity = c_movement.currVelocity;
		if (c_movement.inputVelocityList.size() != 0)
		{
			c_movement.inputVel = Duin::Vector2::ZERO;
			for (auto& [velocity, factor] : c_movement.inputVelocityList)
			{
				c_movement.inputVel += velocity.Normalized() * factor;
			}
			c_movement.inputVel = (c_movement.inputVel / c_movement.inputVelocityList.size());
		}
		c_movement.inputVel = c_movement.inputVel.Normalized();
		c_movement.currVelocity += c_movement.inputVel * c_movement.accelerationFactor * delta;
		c_movement.currVelocity.LimitLength(0.0f, c_movement.maxLinearSpeed);
		c_movement.currRotation = c_movement.targetRotation;
		c_movement.currSpeed = c_movement.currVelocity.Length();
	}
}

void Handler_BoidsFollowLeader::Update(double delta)
{
	auto view = registry->View<Component_Boid, Component_BoidGroup, Component_Movement, Component_Transform>();
	for (auto [entity, c_boid, c_boidGroup, c_movement, c_transform] : view.each())
	{
		if (c_boidGroup.leader == nullptr) { continue; }
		if (!c_boidGroup.leader->HasComponent<Component_Transform>()) { continue; }

		Duin::Vector2 leaderPos = c_boidGroup.leader->GetComponent<Component_Transform>().position;
		Duin::Vector2 deltaPos = leaderPos - c_transform.position;
		Duin::Vector2 deltaPosNorm = deltaPos.Normalized();
		//Duin::Vector2 inputVel = (deltaPosNorm - c_movement.currVelocity.Normalized()).Normalized();
		Duin::Vector2 inputVel = deltaPosNorm;

		c_movement.inputVelocityList.push_back(std::pair(inputVel, c_boidGroup.leaderAttractionFactor));
	}
}