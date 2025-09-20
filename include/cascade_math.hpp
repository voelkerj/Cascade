#ifndef CASCADE_CASCADE_MATH_H
#define CASCADE_CASCADE_MATH_H

#include <random>
#define _USE_MATH_DEFINES
#include <cmath>
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

  float Distance(const State state1, const State state2)
  {
    return sqrt(pow(state2.X - state1.X, 2) + pow(state2.Y - state1.Y, 2));
  }

  float Dot(const float v1[2], const float v2[2])
  {
    return v1[0] * v2[0] + v1[1] * v2[1];
  }

  float Norm(const float v[2])
  {
    return sqrt(pow(v[0], 2) + pow(v[1], 2));
  }

  std::vector<float> Unit(const float v[2])
  {
    std::vector<float> out;
    out.push_back(v[0] / Norm(v));
    out.push_back(v[1] / Norm(v));

    return out;
  }

  float VectorAngle(const float v1[2], const float v2[2])
  {
    return (180 / M_PI) * (acos(Dot(v1, v2) / (Norm(v1) * Norm(v2))));
  }
}

#endif