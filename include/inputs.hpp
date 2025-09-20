#ifndef CASCADE_INPUTS_H
#define CASCADE_INPUTS_H

#include <map>

#include "../external/SDL/include/SDL3/SDL.h"

namespace Cascade
{
  class Inputs
  {
  public:
    Inputs() {};
    ~Inputs() {};

    void StartFrame();

    void HandleKeyboardEvent(SDL_Event event);
    void HandleMouseEvent(SDL_Event event);

    bool WasPressed(const SDL_Scancode &key);
    bool WasReleased(const SDL_Scancode &key);
    bool IsHeld(const SDL_Scancode &key);

    bool m_left_click{false};
    bool m_right_click{false};
    bool m_middle_click{false};
    float m_mouse_coords[2]; // Screen coordinate system

  private:
    std::map<SDL_Scancode, bool> m_pressed_keys;
    std::map<SDL_Scancode, bool> m_released_keys;
    std::map<SDL_Scancode, bool> m_held_keys;
  };
}

#endif