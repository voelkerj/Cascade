#ifndef CASCADE_COMPONENTS_H
#define CASCADE_COMPONENTS_H

#include "cascade_math.hpp"

// This file contains some baked-in components for Cascade

namespace Cascade
{
  struct State
  {
    float X{0}, Y{0}, VX{0}, VY{0}, AX{0}, AY{0}, SizeX{1}, SizeY{1}, Angle{0};
  };

  // Animation and rendering state
  struct DrawingState
  {
    std::string animation_name; // string to look up the Animation
    int frame_idx{0};
    uint32_t prev_update_ticks{0};
    int color[3]{(int)RandInRange(0,255), (int)RandInRange(0,255), (int)RandInRange(0,255)};
  };
}

#endif