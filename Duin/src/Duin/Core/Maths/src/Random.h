#pragma once

#include "../MathsCore.h"

#include <random>

namespace Duin
{
    struct DUIN_MATHS_API Random
    {
        static int GetRandomInt(int min, int max) 
        {
            // Create a random device and seed it
            std::random_device rd;
            std::mt19937 gen(rd());

            // Define a distribution within the specified range
            std::uniform_int_distribution<> dis(min, max);

            // Generate and return the random number
            return dis(gen);
        }

        static float GetRandomFloat(float min, float max) 
        {
            // Create a random device and seed it
            std::random_device rd;
            std::mt19937 gen(rd());

            // Define a distribution within the specified range
            std::uniform_real_distribution<> dis(min, max);

            // Generate and return the random number
            return dis(gen);
        }
    };
}