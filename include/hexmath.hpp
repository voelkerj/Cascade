#ifndef HEX_MATH_H
#define HEX_MATH_H

// Functions adapted from: https://www.redblobgames.com/grids/hexagons/

#define SQRT_3 1.73205080757

#include <vector>
#include <cmath>
#include <algorithm>

namespace HexMath
{
  inline const std::vector<std::vector<int>> axial_direction_vectors = {{1, 0}, {1, -1}, {0, -1}, {-1, 0}, {-1, 1}, {0, 1}};

  inline void cube_round(std::vector<int> &coords, const float q_in, const float r_in, const float s_in)
  {
    int q = round(q_in);
    int r = round(r_in);
    int s = round(s_in);

    float q_diff = abs(q - q_in);
    float r_diff = abs(r - r_in);
    float s_diff = abs(s - s_in);

    if (q_diff > r_diff && q_diff > s_diff)
      q = -r - s;
    else if (r_diff > s_diff)
      r = -q - s;
    else
      s = -q - r;

    coords[0] = q;
    coords[1] = r;
  }

  inline void offset_oddr_to_axial(std::vector<int> &coords)
  {
    // Convert offset coords to axial
    // coords input as [row, col]
    int parity = coords[0] & 1;
    int q = coords[1] - (coords[0] - parity) / 2.0;
    coords[1] = q;
  }

  inline void hex_corner_WCS(std::vector<float> &corner, std::vector<float> center, const float size, const float view_angle, const int i)
  {
    // For "pointy" hex orientation

    // Unsquash
    center[1] = center[1] / cos(view_angle * (M_PI / 180));

    float angle = (60.0 * i - 30.0) * (M_PI / 180.0);

    corner[0] = center[0] + size * cos(angle);
    corner[1] = (center[1] + size * sin(angle)) * cos(view_angle * (M_PI / 180));
  }

  inline void HEX2WCS(std::vector<float> &center, const float size, const int q, const int r)
  {
    // For axial coordinates, "pointy" hex orientation
    center[0] = (SQRT_3 * q + (SQRT_3 / 2.0) * r) * size;
    center[1] = (1.5 * r) * size;
  }

  inline void WCS2HEX(std::vector<int> &coords, std::vector<float> WCS_coords, const float size, float view_angle)
  {
    // For "pointy" hex orientation

    WCS_coords[1] = WCS_coords[1] / cos(view_angle * (M_PI / 180.0));

    float q = ((SQRT_3 / 3.0) * (WCS_coords[0] / size) - (1.0 / 3.0) * (WCS_coords[1] / size));
    float r = ((2.0 / 3.0) * (WCS_coords[1] / size));
    float s = -q - r;

    cube_round(coords, q, r, s);
  }

  inline int axial_distance(const int q1, const int r1, const int q2, const int r2)
  {
    return (abs(q1 - q2) + abs(q1 + r1 - q2 - r2) + abs(r1 - r2)) / 2;
  }

  inline std::vector<std::vector<int>> within_range(const std::vector<int> &start, int range)
  {
    std::vector<std::vector<int>> hexes_in_range;
    std::vector<int> valid{0, 0};

    for (int q = -range; q <= range; q++)
    {
      for (int r = std::max(-range, -q - range); r <= std::min(range, -q + range); r++)
      {
        if (!(q == 0 && r == 0))
        {
          valid[0] = start[0] + q;
          valid[1] = start[1] + r;
          hexes_in_range.push_back(valid);
        }
      }
    }
    return hexes_in_range;
  }

  inline std::vector<std::vector<int>> hex_reachable(const std::vector<int> &start, const std::vector<std::vector<int>>& obstacles, int range)
  {
    std::vector<std::vector<int>> visited;
    visited.push_back(start);

    std::vector<std::vector<std::vector<int>>> fringes(range + 1);
    fringes[0].push_back(start);
    
    std::vector<int> neighbor;

    // for each radial outward layer
    for (int k = 1; k <= range; k++)
    {
      // for each hex in the previous layer
      for (size_t hex_idx = 0; hex_idx < fringes[k - 1].size(); hex_idx++)
      {
        // for each of the six directions
        for (int direction = 0; direction < 6; direction++)
        {
          neighbor = {fringes[k - 1][hex_idx][0] + axial_direction_vectors[direction][0], fringes[k - 1][hex_idx][1] + axial_direction_vectors[direction][1]};

          // if neighbor not blocked
          if (std::find(obstacles.begin(), obstacles.end(), neighbor) == obstacles.end())
          {
            // if neighbor not already in visited
            if (std::find(visited.begin(), visited.end(), neighbor) == visited.end())
            {
              visited.push_back(neighbor);
              fringes[k].push_back(neighbor);
            }
          }
        }
      }
    }
    return visited;
  }
};

#endif