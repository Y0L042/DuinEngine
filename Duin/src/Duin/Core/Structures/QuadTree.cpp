#include "dnpch.h"

#include "QuadTree.h"

namespace Duin
{
    QuadTreeComponent::QuadTreeComponent(UUID uuid, Vector2 position)
        : uuid(uuid), position(position)
    {
    }

    QuadTreeComponent::QuadTreeComponent(QuadTree* tree, UUID uuid, Vector2 position)
        : uuid(uuid), position(position)
    {
        if (tree)
        {
            tree->Insert(*this);
        }
    }

    QuadTreeComponent::~QuadTreeComponent()
    {
    }

    QuadTree::QuadTree(Rectangle bounds, int maxNodes, int maxLevels, int currentLevel)
        : bounds(bounds), maxNodes(maxNodes), maxLevels(maxLevels), currentLevel(currentLevel)
    {
    }

    QuadTree::~QuadTree()
    {
    }

    void QuadTree::Insert(QuadTreeComponent node)
    {
        if (!bounds.Contains(node.position))
        {
            return;
        }

        if (nodes.size() < maxNodes || currentLevel >= maxLevels)
        {
            nodes.push_back(node);
            return;
        }

        if (!children[0])
        {
            Split();
        }

        int index = GetQuadIndex(node.position);
        if (index != -1) {
            children[index]->Insert(node);
        }
        else
        {
            nodes.push_back(node);
        }
    }

    void QuadTree::Query(Rectangle area, std::vector<UUID>& resultVec)
    {
        if (!bounds.Intersects(area))
        {
            return;
        }

        for (const auto& node : nodes)
        {
            if (area.Contains(node.position))
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
        if (!bounds.Contains(node.position))
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
            int index = GetQuadIndex(node.position);
            if (index != -1 && children[index]->Remove(node))
            {
                if (children[index]->nodes.size() == 0)
                {
                    children[index].reset();
                }
                return true;
            }
        }

        return false;
    }

    void QuadTree::Split()
    {
        float subWidth = bounds.width / 2.0f;
        float subHeight = bounds.height / 2.0f;
        float x = bounds.origin.x;
        float y = bounds.origin.y;

        children[0] = std::make_unique<QuadTree>(Rectangle{ x + subWidth, y, subWidth, subHeight }, maxNodes, maxLevels, currentLevel + 1);
        children[1] = std::make_unique<QuadTree>(Rectangle{ x, y, subWidth, subHeight }, maxNodes, maxLevels, currentLevel + 1);
        children[2] = std::make_unique<QuadTree>(Rectangle{ x, y + subHeight, subWidth, subHeight }, maxNodes, maxLevels, currentLevel + 1);
        children[3] = std::make_unique<QuadTree>(Rectangle{ x + subWidth, y + subHeight, subWidth, subHeight }, maxNodes, maxLevels, currentLevel + 1);
    }

    int QuadTree::GetQuadIndex(Vector2 pos)
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
