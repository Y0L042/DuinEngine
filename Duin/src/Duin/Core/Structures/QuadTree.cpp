#include "dnpch.h"

#include "QuadTree.h"

//For Draw()
#include "RLImGuiComponent.h"

namespace Duin
{
    QuadTreeComponent::QuadTreeComponent(UUID uuid, Vector2 position)
        : uuid(uuid), position(position), tree(nullptr)
    {
    }

    QuadTreeComponent::QuadTreeComponent(QuadTree* tree, UUID uuid, Vector2 position)
        : uuid(uuid), position(position), tree(tree)
    {
        if (tree)
        {
            this->tree = tree;
            tree->Insert(*this);
        }
    }

    QuadTreeComponent::~QuadTreeComponent()
    {
    }

    void QuadTreeComponent::UpdatePosition(Vector2 newPosition)
    {

    }

    QuadTree::QuadTree(Rectangle bounds, int maxNodes, int maxLevels, int currentLevel)
        : bounds(bounds), maxNodes(maxNodes), maxLevels(maxLevels), currentLevel(currentLevel), uuid(UUID())
    {
    }

    QuadTree::~QuadTree()
    {
    }

    UUID QuadTree::GetUUID() const
    {
        return uuid;
    }

    void QuadTree::Insert(QuadTreeComponent node)
    {
        if (!bounds.Contains(node.GetPosition()))
        {
            //DN_CORE_INFO("Bounds of {0} does not contain node {1} pos!", static_cast<uint64_t>(uuid), static_cast<uint64_t>(node.uuid));
            return;
        }
        //DN_CORE_INFO("Bounds of {0} does contains node {1} pos.", static_cast<uint64_t>(uuid), static_cast<uint64_t>(node.uuid));

        // TODO Make it clearer when tree is empty/when children are empty/does not exist/when children are full...
        // If there is space
        if (!children[0] && nodes.size() < maxNodes)// || currentLevel >= maxLevels)
        {
            //DN_CORE_INFO("Tree {0} inserts node {1}.", static_cast<uint64_t>(uuid), static_cast<uint64_t>(node.uuid));
            nodes.push_back(node);
            return;
        }
        // Else
        //DN_CORE_INFO("Tree {0} full, passing node {1} on...", static_cast<uint64_t>(uuid), static_cast<uint64_t>(node.uuid));

        // (Create children if no children
        if (!children[0])
        {
            //DN_CORE_INFO("Tree {0} splitting!", static_cast<uint64_t>(uuid), static_cast<uint64_t>(node.uuid));
            Split();
            PassOnNodes();
        }
        // Insert nodes into children
        int index = GetQuadIndex(node.GetPosition());
        if (index != -1) 
        {
            children[index]->Insert(node);
        }
    }

    void QuadTree::Query(Rectangle area, std::vector<UUID>& resultVec) const
    {
        if (!bounds.Intersects(area))
        {
            return;
        }

        for (const auto& node : nodes)
        {
            if (area.Contains(node.GetPosition()))
            {
                resultVec.push_back(node.uuid);
            }
        }

        if (children[0])
        {
            for (int i = 0; i < 4; ++i)
            {
                children[i]->Query(area, resultVec);
            }
        }
    }

    bool QuadTree::Remove(QuadTreeComponent node)
    {
        if (!bounds.Contains(node.GetPosition()))
        {
            return false;
        }

        auto it = std::find_if(
            nodes.begin(),
            nodes.end(),
            [&node](const QuadTreeComponent& n)
            {
                return n.uuid == node.uuid;
            }
        );
        if (it != nodes.end())
        {
            nodes.erase(it);
            return true;
        }

        if (children[0])
        {
            int index = GetQuadIndex(node.GetPosition());
            if (index != -1 && children[index])
            {
                if (children[index]->Remove(node))
                {
                    bool allChildrenEmpty = true;
                    for (int i = 0; i < 4; ++i)
                    {
                        if (children[i] && children[i]->nodes.size() != 0)
                        {
                            allChildrenEmpty = false;
                            break;
                        }
                    }
                    if (allChildrenEmpty)
                    {
                        for (int i = 0; i < 4; ++i)
                        {
                            children[i].reset();
                        }
                    }
                    return true;
                }
            }
        }

        return false;
    }

    void QuadTree::AddNodeToRebuildQueue(QuadTreeComponent node)
    {
        nodesQueuedForRebuild.push_back(node);
    }

    void QuadTree::RebuildTree()
    {
        nodes.clear();
        children->reset();

        for (auto& node : nodesQueuedForRebuild)
        {
            Insert(node);
        }

        nodesQueuedForRebuild.clear();
    }

    void QuadTree::Draw()
    {
        if (!this) { return; }
        ::DrawRectangleLines(bounds.origin.x, bounds.origin.y, bounds.width/2, bounds.height/2, LIGHTGRAY); //NW
        ::DrawRectangleLines(bounds.origin.x + bounds.width / 2, bounds.origin.y, bounds.width / 2, bounds.height / 2, LIGHTGRAY); //NE
        ::DrawRectangleLines(bounds.origin.x, bounds.origin.y + bounds.height / 2, bounds.width / 2, bounds.height / 2, LIGHTGRAY); //SW
        ::DrawRectangleLines(bounds.origin.x+bounds.width/2, bounds.origin.y+bounds.height/2, bounds.width / 2, bounds.height / 2, LIGHTGRAY); //SE

        ::DrawRectangleLines(bounds.origin.x, bounds.origin.y, bounds.width, bounds.height, BLUE);
        
        if (children[0])
        {
            children[0]->Draw();
            children[1]->Draw();
            children[2]->Draw();
            children[3]->Draw();
        }
    }

    void QuadTree::Split()
    {
        float subWidth = bounds.width / 2.0f;
        float subHeight = bounds.height / 2.0f;
        float x = bounds.origin.x;
        float y = bounds.origin.y;

        children[0] = std::make_unique<QuadTree>(Duin::Rectangle( x + subWidth, y, subWidth, subHeight ), maxNodes, maxLevels, currentLevel + 1);
        children[1] = std::make_unique<QuadTree>(Duin::Rectangle( x, y, subWidth, subHeight ), maxNodes, maxLevels, currentLevel + 1);
        children[2] = std::make_unique<QuadTree>(Duin::Rectangle( x, y + subHeight, subWidth, subHeight ), maxNodes, maxLevels, currentLevel + 1);
        children[3] = std::make_unique<QuadTree>(Duin::Rectangle( x + subWidth, y + subHeight, subWidth, subHeight ), maxNodes, maxLevels, currentLevel + 1);
    }

    void QuadTree::PassOnNodes()
    {
        if (!children[0])
        {
            return;
        }

        for (auto& node : nodes)
        {
            children[0]->Insert(node);
            children[1]->Insert(node);
            children[2]->Insert(node);
            children[3]->Insert(node);
        }

        nodes.clear();
    }

    int QuadTree::GetQuadIndex(Vector2 pos) const
    {
        int index = -1;
        float verticalMidpoint = bounds.origin.x + bounds.width / 2.0f;
        float horizontalMidpoint = bounds.origin.y + bounds.height / 2.0f;

        bool topQuadrant = pos.y > horizontalMidpoint;
        bool bottomQuadrant = pos.y < horizontalMidpoint;
        bool leftQuadrant = pos.x < verticalMidpoint;
        bool rightQuadrant = pos.x > verticalMidpoint;

        if (leftQuadrant) {
            if (topQuadrant) {
                index = 2;
            }
            else if (bottomQuadrant) {
                index = 1;
            }
        }
        else if (rightQuadrant) {
            if (topQuadrant) {
                index = 3;
            }
            else if (bottomQuadrant) {
                index = 0;
            }
        }

        return index;
    }
}
