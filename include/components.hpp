#ifndef CASCADE_COMPONENTS_H
#define CASCADE_COMPONENTS_H

#include "cascade_math.hpp"

// This file contains some baked-in components for Cascade

namespace Cascade
{
  struct State
  {
    float X{0}, Y{0}, VX{0}, VY{0}, AX{0}, AY{0}, ScaleX{1}, ScaleY{1}, Angle{0};
  };

  // Animation and rendering state
  struct DrawingState
  {
    std::string animation_name; // string to look up the Animation
    int frame_idx{0};
    uint32_t prev_update_ticks{0};
    bool enable_tint{false};
    int color[3]{0, 0, 0};
    int current_animation_end_behavior{0}; // 0 = loop, 1 = once
    std::string default_animation_name;
  };

  struct UIElement
  {
    float position[2]{0, 0};
    int size[2]{0, 0};
    float angle{0};
    bool click_type[3]{false, false, false}; // left, middle, right
    bool hover{false};
    std::string hover_animation{""};
    std::string click_animation{""};
    bool click_animation_done{true};
  };
}

#endif