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
	struct DUIN_API QuadTreeComponent
	{
		UUID uuid;
		Vector2 position;

		QuadTreeComponent(UUID uuid, Vector2 position)
			: uuid(uuid), position(position)
		{}
	};

	struct DUIN_API QuadTree
	{
	public:
		Rectangle bounds;
		int maxNodes;
		int maxLevels;
		int currentLevel;

		QuadTree(Rectangle bounds, int maxNodes, int maxLevels, int currentLevel)
			: bounds(bounds), maxNodes(maxNodes), maxLevels(maxLevels), currentLevel(currentLevel)
		{}

		void Insert(QuadTreeComponent node)
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

		void Query(Rectangle area, std::vector<UUID>& resultVec)
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

		bool Remove(QuadTreeComponent node) {
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

	private:
		std::vector<QuadTreeComponent> nodes;
		std::unique_ptr<QuadTree> children[4];

		void Split()
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

		int GetQuadIndex(Vector2 pos)
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
	};
}