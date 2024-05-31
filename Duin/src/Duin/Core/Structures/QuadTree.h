#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Core/Maths/src/Rectangle.h"
#include "Duin/Core/Maths/src/Vector2.h"
#include "Duin/Core/UUID.h"

#include <vector>
#include <memory>
#include <algorithm>

namespace Duin
{
    struct QuadTree;

    struct DUIN_API QuadTreeComponent
    {
    public:
        UUID uuid;

        QuadTreeComponent(UUID uuid, Vector2 position);
        QuadTreeComponent(QuadTree* tree, UUID uuid, Vector2 position);
        ~QuadTreeComponent();

        Vector2 GetPosition() const { return position; }
        void UpdatePosition(Vector2 newPosition);

    private:
        QuadTree* tree;
        Vector2 position;
    };

    struct DUIN_API QuadTree
    {
    public:
        Rectangle bounds;
        int maxNodes;
        int maxLevels;
        int currentLevel;

        QuadTree(Rectangle bounds, int maxNodes, int maxLevels, int currentLevel);
        ~QuadTree();

        UUID GetUUID() const;

        void Insert(QuadTreeComponent node);
        void Query(Rectangle area, std::vector<UUID>& resultVec);
        bool Remove(QuadTreeComponent node);

        void Draw();
    protected:
        UUID uuid;
    private:
        std::vector<QuadTreeComponent> nodes;
        std::unique_ptr<QuadTree> children[4];

        void Split();
        void PassOnNodes();
        int GetQuadIndex(Vector2 pos);
    };
}
