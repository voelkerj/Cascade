#include <iostream>

#include "../include/cascade_math.hpp"
#include "../include/components.hpp"

float Cascade::RandInRange(float min, float max)
{
  if (min > max)
  {
    std::cerr << "Min: " << min << " cannot be greater than Max: " << max << "\n";
    exit(1);
  }
  std::mt19937 generator(std::chrono::system_clock::now().time_since_epoch().count());
  std::uniform_real_distribution<> distribution(min, max);

  return distribution(generator);
}

float Cascade::Distance(const State state_1, const State state_2)
{
  return sqrt(pow(state_2.X - state_1.X, 2) + pow(state_2.Y - state_1.Y, 2));
}

float Cascade::Dot(const float v1[2], const float v2[2])
{
  return v1[0] * v2[0] + v1[1] * v2[1];
}

float Cascade::Norm(const float v[2])
{
  return sqrt(pow(v[0], 2) + pow(v[1], 2));
}

std::vector<float> Cascade::Unit(const float v[2])
{
  std::vector<float> out;
  out.push_back(v[0] / Norm(v));
  out.push_back(v[1] / Norm(v));

  return out;
}

float Cascade::VectorAngle(const float v1[2], const float v2[2])
{
  return (180 / M_PI) * acos(Dot(v1, v2) / (Norm(v1) * Norm(v2)));
}