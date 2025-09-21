#ifndef CASCADE_CASCADE_MATH_H
#define CASCADE_CASCADE_MATH_H

#include <random>
#define _USE_MATH_DEFINES
#include <cmath>
#include <chrono>

namespace Cascade
{
  // Forward-declare State to break circular dependency
  struct State;

  float RandInRange(float min, float max);

  float Distance(const State state1, const State state2);

  float Dot(const float v1[2], const float v2[2]);

  float Norm(const float v[2]);

  std::vector<float> Unit(const float v[2]);

  float VectorAngle(const float v1[2], const float v2[2]);
}

#endif