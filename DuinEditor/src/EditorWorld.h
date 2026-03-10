#pragma once

#include <Duin/ECS/GameWorld.h>

class EditorWorld : public duin::GameWorld
{
  public:
    EditorWorld();
    ~EditorWorld();

    void PostUpdateQueryExecution(double delta) override;
    void PostPhysicsUpdateQueryExecution(double delta) override;
    void PostDrawQueryExecution() override;
    void PostDrawUIQueryExecution() override;

    void ExecuteQueryDrawTransform3DGizmos();
    void ExecuteQuerySyncCharacterBodyWithTransform3D();
};
