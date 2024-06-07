#include "dnpch.h"
#include "Delaunay.h"

namespace Duin
{
    DelaunayEdge::DelaunayEdge()
        : v1(Vector2::ZERO), v2(Vector2::ZERO)
    {
    }

    DelaunayEdge::DelaunayEdge(Vector2 v1, Vector2 v2)
    {
        // Ensure consistent order
        if (v1.x > v2.x || (v1.x == v2.x && v1.y > v2.y))  
        {
            std::swap(v1, v2);
        }

        this->v1 = v1;
        this->v2 = v2;
    }

    std::size_t EdgeHash::operator()(const DelaunayEdge& edge) const
    {
        {
            auto round_and_hash = [](float val) -> std::size_t {
                return std::hash<int>{}(static_cast<int>(std::round(val * 1.0f))); // Scale and round for precision
                };

            std::size_t h1 = round_and_hash(edge.v1.x) ^ round_and_hash(edge.v1.y);
            std::size_t h2 = round_and_hash(edge.v2.x) ^ round_and_hash(edge.v2.y);
            return h1 ^ (h2 << 1); // Combine hashes
        }
    }

    DelaunayTriangle::DelaunayTriangle(Vector2 v1, Vector2 v2, Vector2 v3)
        : v1(v1), v2(v2), v3(v3)
    {
        center = (v1 + v2 + v3) / 3;
        e1 = DelaunayEdge(v1, v2);
        e2 = DelaunayEdge(v2, v3);
        e3 = DelaunayEdge(v1, v3);
    }

    Delaunay::Delaunay()
    {
    }

    Delaunay::~Delaunay()
    {
    }

    void Delaunay::InsertVertices(std::vector<Vector2> vertices)
    {
        std::vector<CDT::V2d<double>> cdtVertices;
        for (auto& vert : vertices)
        {
            cdtVertices.push_back(CDT::V2d<double>{ (double)vert.x, (double)vert.y });
        }

        CDT::Triangulation<double> cdt;
        cdt.insertVertices(cdtVertices);
        cdt.eraseSuperTriangle();
        CDT::TriangleVec triangleVec = cdt.triangles;

        for (const auto& triangle : triangleVec)
        {
            const auto& cdtV1 = cdt.vertices[triangle.vertices[0]];
            const auto& cdtV2 = cdt.vertices[triangle.vertices[1]];
            const auto& cdtV3 = cdt.vertices[triangle.vertices[2]];

            Vector2 v1{ (float)cdtV1.x, (float)cdtV1.y };
            Vector2 v2{ (float)cdtV2.x, (float)cdtV2.y };
            Vector2 v3{ (float)cdtV3.x, (float)cdtV3.y };

            DelaunayTriangle triangle{ v1, v2, v3 };

            triangleGraph.push_back(triangle);
        }

        cdtVertices.clear();
        for (auto& triangle : triangleGraph)
        {
            cdtVertices.push_back(CDT::V2d<double>{ (double)triangle.center.x, (double)triangle.center.y });
        }
        CDT::Triangulation<double> triangleCenterCDT;

        triangleCenterCDT.insertVertices(cdtVertices);
        triangleCenterCDT.eraseSuperTriangle();
        CDT::TriangleVec centerVec = triangleCenterCDT.triangles;

        for (const auto& triangle : centerVec)
        {
            const auto& cdtV1 = triangleCenterCDT.vertices[triangle.vertices[0]];
            const auto& cdtV2 = triangleCenterCDT.vertices[triangle.vertices[1]];
            const auto& cdtV3 = triangleCenterCDT.vertices[triangle.vertices[2]];

            Vector2 v1{ (float)cdtV1.x, (float)cdtV1.y };
            Vector2 v2{ (float)cdtV2.x, (float)cdtV2.y };
            Vector2 v3{ (float)cdtV3.x, (float)cdtV3.y };

            DelaunayTriangle triangle{ v1, v2, v3 };

            centerEdgeGraph.push_back(triangle.e1);
            centerEdgeGraph.push_back(triangle.e2);
            centerEdgeGraph.push_back(triangle.e3);
        }   
    }

    void Delaunay::Clear()
    {
        vertices.clear();
        triangleGraph.clear();
    }

    void Delaunay::CalculateGraph()
    {
        std::unordered_map<DelaunayEdge, std::vector<DelaunayTriangle*>, EdgeHash> edgeToTriangles;

        // Map edges to triangles
        for (auto& triangle : triangleGraph)
        {
            edgeToTriangles[triangle.e1].push_back(&triangle);
            edgeToTriangles[triangle.e2].push_back(&triangle);
            edgeToTriangles[triangle.e3].push_back(&triangle);
        }

        int x = 0;
        // Assign neighbors based on shared edges
        for (const auto& [edge, triangles] : edgeToTriangles)
        {
            if (triangles.size() == 2)
            {
                x++;

                DelaunayTriangle* t1 = triangles[0];
                DelaunayTriangle* t2 = triangles[1];

                bool t1NeighborAssigned = false;
                bool t2NeighborAssigned = false;

                for (int i = 0; i < 3; ++i)
                {
                    if (t1->neighbors[i] == nullptr && !t1NeighborAssigned)
                    {
                        t1->neighbors[i] = t2;
                        t1NeighborAssigned = true;
                    }
                    if (t2->neighbors[i] == nullptr && !t2NeighborAssigned)
                    {
                        t2->neighbors[i] = t1;
                        t2NeighborAssigned = true;
                    }
                }
            }
        }
        DN_CORE_INFO("Triangles = {}", triangleGraph.size());
        DN_CORE_INFO("x = {}", x);
    }

    void Delaunay::Draw()
    {
        for (const auto& triangle : triangleGraph)
        {
            DebugDraw::DrawTriangleLines(triangle.v1, triangle.v2, triangle.v3, GREEN);
            DebugDraw::DrawCircle(triangle.center, 2.0f, DARKBLUE);

            if (triangle.neighbors[0] != nullptr)
            {
                DebugDraw::DrawLine(triangle.center, triangle.neighbors[0]->center, RED);
            }
            if (triangle.neighbors[1] != nullptr)
            {
                DebugDraw::DrawLine(triangle.center, triangle.neighbors[1]->center, RED);
            }
            if (triangle.neighbors[2] != nullptr)
            {
                DebugDraw::DrawLine(triangle.center, triangle.neighbors[2]->center, RED);
            }
        }

        for (auto& edge : centerEdgeGraph)
        {
            DebugDraw::DrawLine(edge.v1, edge.v2, PINK);
        }
    }


}