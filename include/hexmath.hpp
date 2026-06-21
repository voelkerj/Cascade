#ifndef HEX_MATH_H
#define HEX_MATH_H

// Functions adapted from: https://www.redblobgames.com/grids/hexagons/

#define SQRT_3 1.73205080757

#include <vector>
#include <cmath>
#include <algorithm>
#include <queue>

namespace HexMath
{
  inline const std::vector<std::vector<int>> axial_direction_vectors = {{1, 0}, {1, -1}, {0, -1}, {-1, 0}, {-1, 1}, {0, 1}};

  class Hex
  {
  public:
    Hex() {};

    Hex(int coord_1, int coord_2)
    {
      coords[0] = coord_1;
      coords[1] = coord_2;
    }

    // We need non-const so we can assign values
    inline int &operator[](int idx)
    {
      if (idx < 0 || idx > 1)
      {
        throw std::out_of_range("Hex index out of range");
      }
      return coords[idx];
    }

    // we need const so this works with the const functors below
    inline int const &operator[](int idx) const
    {
      if (idx < 0 || idx > 1)
      {
        throw std::out_of_range("Hex index out of range");
      }
      return coords[idx];
    }

    // Check if hexes are the same
    inline bool operator==(Hex const& compare_hex) const
    {
      if (compare_hex[0] == coords[0] && compare_hex[1] == coords[1])
      {
        return true;
      }
      return false;
    }

  private:
    int coords[2]{0, 0};
  };

  // This functor tells the unordered_map below how to hash a Hex object. 
  // Since Hex is a custom class, the unordered_map will not know how to hash it.
  // Why am I using an unordered_map and not just a map? The unordered map is faster
  // because it will first store things in a bucket (hash) then use the standard ==
  // lookup to find the key within that bucket. Apparently, maps are O(log(n)) and
  // unordered_maps are O(1).
  struct HexHash {
    size_t operator()(Hex const& hex) const
    {
      // Our hash function for a hex is based on the only identifying information we
      // actually have: it's q an r coordinates. We take the q coordinate and shift it
      // 32 bits, so now the first 32 bits are zeros. Then we inclusive OR it with the
      // r coordinate, which is in the bottom 32 bits. This effectively merges the two
      // 32 bit q and r values into a single 64 bit number.
      // Assumptions are that q and r are not larger than 32 bits and that we are 
      // compiling a 64 bit executable.
      return ((size_t)hex[0] << 32 | (size_t)hex[1]);
    }
  };

  inline void cube_round(Hex &coords, const float q_in, const float r_in, const float s_in)
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
    coords[1] = coords[0]; // r = row
    coords[0] = q;
    // coords re-written as [q, r]
  }

  inline void hex_corner_WCS(std::vector<float> &corner, std::vector<float> center, const float size, const int i)
  {
    // For "pointy" hex orientation
    float angle = (60.0 * i - 30.0) * (M_PI / 180.0);

    corner[0] = center[0] + size * cos(angle);
    corner[1] = center[1] + size * sin(angle);
  }

  inline void HEX2WCS(std::vector<float> &center, const float size, Hex hex)
  {
    // For axial coordinates, "pointy" hex orientation
    center[0] = (SQRT_3 * hex[0] + (SQRT_3 / 2.0) * hex[1]) * size;
    center[1] = -(1.5 * hex[1]) * size; // -1 becuase SDL is inverted from WCS
  }

  inline void WCS2HEX(Hex &coords, std::vector<float> WCS_coords, const float size)
  {
    WCS_coords[1] *= -1; // Because SDL is inverted from WCS

    // For "pointy" hex orientation
    float q = ((SQRT_3 / 3.0) * (WCS_coords[0] / size) - (1.0 / 3.0) * (WCS_coords[1] / size));
    float r = ((2.0 / 3.0) * (WCS_coords[1] / size));
    float s = -q - r;

    cube_round(coords, q, r, s);
  }

  inline int axial_distance(Hex start, Hex end)
  {
    // const int q1, const int r1, const int q2, const int r2
    // return (abs(q1 - q2) + abs(q1 + r1 - q2 - r2) + abs(r1 - r2)) / 2;
    return (abs(start[0] - end[0]) + abs(start[0] + start[1] - end[0] - end[1]) + abs(start[1] - end[1])) / 2;
  }

  inline std::vector<Hex> within_range(Hex &start, int range)
  {
    std::vector<Hex> hexes_in_range;
    Hex valid;

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

  inline std::vector<Hex> hex_reachable(Hex start, std::vector<Hex> obstacles, int range)
  {
    std::unordered_set<Hex, HexHash> obstacle_set(obstacles.begin(), obstacles.end());
    std::unordered_set<Hex, HexHash> visited{};
    visited.insert(start);

    std::vector<Hex> current_layer;
    std::vector<Hex> previous_layer;
    // std::vector<std::vector<Hex>> fringes(range + 1); // first dimension is outward layer number (steps away from start)
    // fringes[0].push_back(start);
    previous_layer.push_back(start);

    Hex neighbor;

    // for each radial outward layer
    for (int k = 1; k <= range; k++)
    {
      // for each hex in the previous layer
      for (int hex_idx = 0; hex_idx < previous_layer.size(); hex_idx++)
      {
        // for each of the six directions
        for (int direction = 0; direction < 6; direction++)
        {
          neighbor = Hex(previous_layer[hex_idx][0] + axial_direction_vectors[direction][0], previous_layer[hex_idx][1] + axial_direction_vectors[direction][1]);

          // if neighbor not blocked
          if (obstacle_set.find(neighbor) == obstacle_set.end())
          {
            // if neighbor not already in visited
            if (visited.find(neighbor) == visited.end())
            {
              visited.insert(neighbor);
              current_layer.push_back(neighbor);
            }
          }
        }
      }

      previous_layer = current_layer;
      current_layer.clear();
    }

    // Convert back to vector
    std::vector<Hex> result;
    result.reserve(visited.size());
    int idx = 0;
    for (const auto& hex : visited)
    {
      result.push_back(hex);
      idx++;
    }

    return result;
  }

  class Node
  {
  public:
    Node(Hex hex_in, int g_in, int heuristic, Hex previous_in) : hex(hex_in), g(g_in), h(heuristic), previous(previous_in) {};

    int cost() const { return g + h; };

    Hex hex;
    int g;
    int h;
    Hex previous;
  };

  // This functor tells the priority_queue how to compare hexes
  struct NodeCompare
  {
    bool operator()(const Node& n1, const Node& n2)
    {
      return n1.cost() > n2.cost();
    }
  };

  inline std::vector<Hex> a_star_hex(Hex start, Hex goal, std::vector<Hex> obstacles)
  {
    Hex neighbor;
    std::priority_queue<Node, std::vector<Node>, NodeCompare> fringes;
    // We also store the fringes in a map since we can't search through a priority_queue.
    // We need to be able to see if a Hex is already in the queue so we don't duplicate it.
    std::unordered_map<Hex, int, HexHash> fringes_map;  
    std::vector<Node> checked;

    bool found_path{false};
    int lowest_cost{0};
    int cheapest_idx{0};

    // TODO: Update heuristic to also account for vector with smallest angle to goal
    int h = axial_distance(start, goal);
    int g = 0;
    Node current_node(start, g, h, start);
    fringes.push(current_node);
    fringes_map[start] = g + h;

    while (!found_path)
    {
      current_node = fringes.top();
      fringes.pop();

      // copy to checked
      checked.push_back(current_node);

      // if it equals the goal
      if (current_node.hex == goal)
      {
        found_path = true;
      }
      else
      {
        // for each neighbor of current
        for (int direction = 0; direction < 6; direction++)
        {
          neighbor = Hex(current_node.hex[0] + axial_direction_vectors[direction][0], current_node.hex[1] + axial_direction_vectors[direction][1]);

          // if not obstacle
          if (std::find(obstacles.begin(), obstacles.end(), neighbor) == obstacles.end())
          {
            // Check if this we've already reached this hex with equal or better cost
            h = axial_distance(neighbor, goal);
            g = current_node.g + 1;

            if (fringes_map.contains(neighbor) && (h + g) >= fringes_map[neighbor])
            {
              continue;
            }

            // Othewise, add to fringes
            fringes.push({neighbor, g, h, current_node.hex});
            fringes_map[neighbor] = g + h;
          }
        }
      }
    }

    // Reconstruct path
    std::vector<Hex> path;
    path.push_back(current_node.hex); // goal

    while (true)
    {
      path.push_back(current_node.previous);
      
      if (current_node.previous == start)
      {
        break;
      }

      // set current_node to previous node
      int idx{0};
      for (idx = 0; idx < checked.size(); idx++)
      {
        if (checked[idx].hex == current_node.previous)
        {
          break;
        }
      }

      // set this as new current node
      current_node = checked[idx];
    }

    return path;
  }
};

#endif