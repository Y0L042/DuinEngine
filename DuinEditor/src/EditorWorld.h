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

  protected:
    void InitializeQueries() override;
    void ResetQueries() override;

  private:
    duin::Query<const duin::ECSComponent::Transform3D> queryEntityPositionGizmos;
    duin::Query<duin::ECSComponent::CharacterBodyComponent, duin::ECSComponent::Transform3D>
        querySyncCharacterBodyWithTransform3D;
};