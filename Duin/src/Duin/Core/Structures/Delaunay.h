#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Core/Maths/DuinMaths.h"
#include "Duin/Core/Debug/DebugDraw.h"

#include <CDT.h>

#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace Duin
{
	struct DUIN_API DelaunayEdge
	{
		Vector2 v1;
		Vector2 v2;

		DelaunayEdge();
		DelaunayEdge(Vector2 v1, Vector2 v2);

		bool operator==(const DelaunayEdge& other) const
		{
			return (v1 == other.v1 && v2 == other.v2) || (v1 == other.v2 && v2 == other.v1);
		}
	};

	struct EdgeHash
	{
		std::size_t operator()(const DelaunayEdge& edge) const;
	};




	struct DUIN_API DelaunayTriangle
	{
		DelaunayTriangle(Vector2 v1, Vector2 v2, Vector2 v3);

		Vector2 v1;
		Vector2 v2;
		Vector2 v3;

		Vector2 center;
		DelaunayEdge e1;
		DelaunayEdge e2;
		DelaunayEdge e3;
		
		DelaunayTriangle* neighbors[3] = { nullptr, nullptr, nullptr };
	};





	class DUIN_API Delaunay
	{
	public:
		Delaunay();
		~Delaunay();

        void InsertVertices(std::vector<Vector2> vertices);
		void Clear();
		void Draw();
		
	private:
        std::vector<Vector2> vertices;
		std::vector<DelaunayTriangle> triangleGraph;
		std::vector<DelaunayEdge> centerEdgeGraph;

		void CalculateGraph();
	};
}