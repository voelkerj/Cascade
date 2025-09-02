#include "inputs.hpp"

void Inputs::StartFrame()
{
  m_pressed_keys.clear();
  m_released_keys.clear();
}

void Inputs::HandleKeyboardEvent(SDL_Event event)
{
  if (event.type == SDL_EVENT_KEY_DOWN) {
    m_pressed_keys[event.key.scancode] = true;
    m_held_keys[event.key.scancode]    = true;
  }
  if (event.type == SDL_EVENT_KEY_UP) {
    m_held_keys[event.key.scancode]     = false;
    m_released_keys[event.key.scancode] = true;
  }
}

void Inputs::HandleMouseEvent(SDL_Event event)
{
  SDL_GetMouseState(&m_mouse_coords[0], &m_mouse_coords[1]);

  if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
    if (event.button.button == SDL_BUTTON_LEFT)
      m_left_click = true;
    if (event.button.button == SDL_BUTTON_MIDDLE)
      m_middle_click = true;
    if (event.button.button == SDL_BUTTON_RIGHT)
      m_right_click = true;
  }
  if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
    if (event.button.button == SDL_BUTTON_LEFT)
      m_left_click = false;
    if (event.button.button == SDL_BUTTON_MIDDLE)
      m_middle_click = false;
    if (event.button.button == SDL_BUTTON_RIGHT)
      m_right_click = false;
  }
}

bool Inputs::WasPressed(const SDL_Scancode& key)
{
  return m_pressed_keys[key];
}

bool Inputs::WasReleased(const SDL_Scancode& key)
{
  return m_released_keys[key];
}

bool Inputs::IsHeld(const SDL_Scancode& key)
{
  return m_held_keys[key];
}