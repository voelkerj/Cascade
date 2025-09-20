#ifndef CASCADE_CASCADE_MATH_H
#define CASCADE_CASCADE_MATH_H

#include <random>
#include <chrono>

#include "components.hpp"

namespace Cascade
{
  float RandInRange(float min, float max)
  {
    std::mt19937 generator(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<> distribution(min, max);

    return distribution(generator);
  }

  float Distance(State state1, State state2)
  {
    return sqrt(pow(state2.X - state1.X, 2) + pow(state2.Y - state1.Y, 2));
  }
}

#endif