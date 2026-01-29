#pragma once

#include <Duin/Core/Maths/MathsModule.h>
#include <Duin/ECS/ECSModule.h>
#include <Duin/Objects/ObjectModule.h>

class EditorCamera : public duin::GameObject
{
  public:
    duin::Camera camera;

    EditorCamera(duin::Vector3 pos = {0.0f, 5.0f, 5.0f}, duin::Vector3 target = {0.0f, 0.0f, 0.0f});

    void Render();

    void Enable(bool enable);
    void UpdatePosition(duin::Vector3 pos);
    void UpdateTarget(duin::Vector3 target);

    void Yaw(float angle, bool aroundTarget);
    void Pitch(float angle, bool aroundTarget);
    void Roll(float angle);

    void MovePosition(double delta);

    void PhysicsUpdate(double delta) override;

  private:
    void SetupInput();
};
