#pragma once

#include "EditorCommand.h"
#include <Duin/ECS/DECS/Entity.h>
#include <Duin/ECS/DECS/World.h>
#include <Duin/Core/Debug/DNLog.h>
#include <flecs.h>
#include <memory>

class AddEntityCommand : public EditorCommand
{
  public:
    AddEntityCommand(std::shared_ptr<duin::World> world) : world(world), newEnt(std::make_shared<duin::Entity>())
    {
        Do = [this]() {
            *newEnt = this->world->Entity();
            DN_INFO("Adding root entity {}", newEnt->GetID());
        };
        Undo = [this]() { this->world->DeleteEntity(*newEnt); };
    }

  private:
    std::shared_ptr<duin::World> world;
    std::shared_ptr<duin::Entity> newEnt;
};

class AddChildEntityCommand : public EditorCommand
{
  public:
    AddChildEntityCommand(std::shared_ptr<duin::World> world, duin::Entity parent)
        : world(world), parent(parent), newEnt(std::make_shared<duin::Entity>())
    {
        Do = [this]() {
            *newEnt = this->world->Entity().ChildOf(this->parent);
            DN_INFO("Adding child entity {}", newEnt->GetID());
        };
        Undo = [this]() { this->world->DeleteEntity(*newEnt); };
    }

  private:
    std::shared_ptr<duin::World> world;
    duin::Entity parent;
    std::shared_ptr<duin::Entity> newEnt;
};

class RemoveEntityCommand : public EditorCommand
{
  public:
    RemoveEntityCommand(std::shared_ptr<duin::World> world, duin::Entity entity)
        : world(world), entity(entity), parentPtr(std::make_shared<duin::Entity>()),
          newEnt(std::make_shared<duin::Entity>())
    {
        Do = [this]() {
            if (this->entity.GetParent().IsValid())
            {
                *parentPtr = this->entity.GetParent();
            }
            this->world->DeleteEntity(this->entity);
        };
        Undo = [this]() {
            *newEnt = this->world->Entity();
            if (parentPtr->IsValid())
            {
                (*newEnt).ChildOf(*parentPtr);
            }
            DN_INFO("Adding root entity {}", newEnt->GetID());
        };
    }

  private:
    std::shared_ptr<duin::World> world;
    duin::Entity entity;
    std::shared_ptr<duin::Entity> parentPtr;
    std::shared_ptr<duin::Entity> newEnt;
};

class ReparentEntityCommand : public EditorCommand
{
  public:
    ReparentEntityCommand(std::shared_ptr<duin::World> world, duin::Entity child, duin::Entity newParent)
        : world(world), child(child), newParent(newParent), oldParentPtr(std::make_shared<duin::Entity>())
    {
        Do = [this]() {
            if (this->child.GetParent().IsValid())
            {
                *oldParentPtr = this->child.GetParent();
            }
            this->child.ChildOf(this->newParent);
            DN_INFO("Reparented entity {} to {}", this->child.GetID(), this->newParent.GetID());
        };
        Undo = [this]() {
            if (oldParentPtr->IsValid())
            {
                this->child.ChildOf(*oldParentPtr);
            }
            else
            {
                this->child.RemovePair(static_cast<uint64_t>(flecs::ChildOf), this->child.GetParent().GetID());
            }
        };
    }

  private:
    std::shared_ptr<duin::World> world;
    duin::Entity child;
    duin::Entity newParent;
    std::shared_ptr<duin::Entity> oldParentPtr;
};

class ReparentEntityToRootCommand : public EditorCommand
{
  public:
    ReparentEntityToRootCommand(std::shared_ptr<duin::World> world, duin::Entity child)
        : world(world), child(child), oldParentPtr(std::make_shared<duin::Entity>())
    {
        Do = [this]() {
            if (this->child.GetParent().IsValid())
            {
                *oldParentPtr = this->child.GetParent();
            }
            this->child.RemovePair(static_cast<uint64_t>(flecs::ChildOf), oldParentPtr->GetID());
            DN_INFO("Reparented entity {} to root", this->child.GetID());
        };
        Undo = [this]() {
            if (oldParentPtr->IsValid())
            {
                this->child.ChildOf(*oldParentPtr);
            }
        };
    }

  private:
    std::shared_ptr<duin::World> world;
    duin::Entity child;
    std::shared_ptr<duin::Entity> oldParentPtr;
};
