#include "DelaunayTest.h"

#include <CDT.h>
#include <raylib.h>

#include <vector>



#include <random>


void DelaunayTest::Initialize()
{
}

void DelaunayTest::Ready()
{
     //Generate random points
    int min = 20;
    int max = 700;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(min, max);

    points = {
        CDT::V2d<double>(distr(gen), distr(gen)),
        CDT::V2d<double>(distr(gen), distr(gen)),
        CDT::V2d<double>(distr(gen), distr(gen)),
        CDT::V2d<double>(distr(gen), distr(gen)),
        CDT::V2d<double>(distr(gen), distr(gen)),
        CDT::V2d<double>(distr(gen), distr(gen))
    };

    // Perform Delaunay triangulation
    cdt.insertVertices(points);
    cdt.eraseSuperTriangle();
    triangles = cdt.triangles;
}

void DelaunayTest::HandleInputs(Duin::InputEvent e)
{
}

void DelaunayTest::Process(double rDelta)
{
}

void DelaunayTest::PhysicsProcess(double pDelta)
{
}

void DelaunayTest::Draw()
{
    for (const auto& triangle : cdt.triangles)
    {
        const auto& v1 = cdt.vertices[triangle.vertices[0]];
        const auto& v2 = cdt.vertices[triangle.vertices[1]];
        const auto& v3 = cdt.vertices[triangle.vertices[2]];

        Duin::DebugDraw::DrawLine(v1.x, v1.y, v2.x, v2.y, Duin::BLACK);
        Duin::DebugDraw::DrawLine(v3.x, v3.y, v2.x, v2.y, Duin::BLACK);
        Duin::DebugDraw::DrawLine(v1.x, v1.y, v3.x, v3.y, Duin::BLACK);

    }
}
