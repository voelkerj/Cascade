#ifndef CASCADE_COMPONENTS_H
#define CASCADE_COMPONENTS_H

#include <iostream>

#include "cascade_math.hpp"

#include "../external/SDL/include/SDL3/SDL.h"

// This file contains some baked-in components for Cascade

namespace Cascade
{
  struct TileLayer{};
  
  struct State
  {
    float X{0}, Y{0}, VX{0}, VY{0}, AX{0}, AY{0}, ScaleX{1}, ScaleY{1}, Angle{0};
  };

  // Animation and rendering state
  struct DrawingState
  {
    int layer{0};
    std::string animation_name; // string to look up the Animation
    int frame_idx{0};
    uint32_t prev_update_ticks{0};
    bool enable_tint{false};
    int color[3]{0, 0, 0};
    int current_animation_end_behavior{0}; // 0 = loop, 1 = once
    std::string default_animation_name;
    SDL_FRect destination_rect;
    float angle{0};
    SDL_FlipMode flip{SDL_FLIP_NONE};
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

  struct NonRotatingCollider
  {
    bool static_collider{false};
    float X{0};
    float Y{0};
    float width{1};
    float height{1};
  };

  // This isn't actually a component, it's just a wrapper around
  // SDL_Color, but idk where else to put it so I guess it goes here???
  class Color
  {
  public:
    Color(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha)
    {
      SetRed(red);
      SetGreen(green);
      SetBlue(blue);
      SetAlpha(alpha);
    };

    void SetRed(Uint8 red)
    {
      if (red < 0 || red > 255)
      {
        std::cerr << "Red color invalid range.\n";
        exit(1);
      }
      m_color.r = red;
    }

    void SetGreen(Uint8 green)
    {
      if (green < 0 || green > 255)
      {
        std::cerr << "Green color invalid range.\n";
        exit(1);
      }
      m_color.g = green;
    }

    void SetBlue(Uint8 blue)
    {
      if (blue < 0 || blue > 255)
      {
        std::cerr << "Blue color invalid range.\n";
        exit(1);
      }
      m_color.b = blue;
    }

    void SetAlpha(Uint8 alpha)
    {
      if (alpha < 0 || alpha > 255)
      {
        std::cerr << "Alpha invalid range.\n";
        exit(1);
      }
      m_color.a = alpha;
    }

    SDL_Color GetColor(){ return m_color; };
  private:
    SDL_Color m_color = {0xFF, 0xFF, 0xFF, 0x0}; // white
  };
}

#endif