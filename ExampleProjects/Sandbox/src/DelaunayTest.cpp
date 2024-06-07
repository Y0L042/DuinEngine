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

    std::vector<Duin::Vector2> vertices;
    for (int x = 0; x < 100; x++)
    {
        vertices.push_back({ (float)distr(gen), (float)distr(gen) });
    };
    
    delaunayGraph.InsertVertices(vertices);
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
    delaunayGraph.Draw();
}
