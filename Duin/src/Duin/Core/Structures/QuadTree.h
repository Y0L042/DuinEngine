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
        UUID uuid;
        Vector2 position;

        QuadTreeComponent(UUID uuid, Vector2 position);
        QuadTreeComponent(QuadTree* tree, UUID uuid, Vector2 position);
        ~QuadTreeComponent();
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

        void Insert(QuadTreeComponent node);
        void Query(Rectangle area, std::vector<UUID>& resultVec);
        bool Remove(QuadTreeComponent node);

    private:
        std::vector<QuadTreeComponent> nodes;
        std::unique_ptr<QuadTree> children[4];

        void Split();
        int GetQuadIndex(Vector2 pos);
    };
}
