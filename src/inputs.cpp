#include "inputs.hpp"

void Cascade::Inputs::StartFrame(entt::registry &registry)
{
  m_pressed_keys.clear();
  m_released_keys.clear();

  // Reset UI Elements
  auto view = registry.view<UIElement>();

  for (auto [entity, ui_element] : view.each())
  {
    ui_element.click_type[0] = false;
    ui_element.click_type[1] = false;
    ui_element.click_type[2] = false;
    ui_element.hover = false;
  }
}

void Cascade::Inputs::HandleKeyboardEvent(SDL_Event event)
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

void Cascade::Inputs::HandleMouseEvent(SDL_Event event, entt::registry &registry, int screen_width, int screen_height)
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
    UpdateUIElements(registry, screen_width, screen_height);

    if (event.button.button == SDL_BUTTON_LEFT)
      m_left_click = false;
    if (event.button.button == SDL_BUTTON_MIDDLE)
      m_middle_click = false;
    if (event.button.button == SDL_BUTTON_RIGHT)
      m_right_click = false;
  }
}

void Cascade::Inputs::UpdateUIElements(entt::registry &registry, int screen_width, int screen_height)
{
  auto view = registry.view<UIElement>();

  for (auto [entity, ui_element] : view.each())
  {
    // Check if mouse pointer was within ui element when pressed
    if ((m_mouse_coords[0] >= ui_element.position[0] * screen_width) && 
    (m_mouse_coords[0] <= ui_element.position[0] * screen_width + ui_element.size[0]) && 
    (m_mouse_coords[1] >= ui_element.position[1] * screen_height) && 
    (m_mouse_coords[1] <= ui_element.position[1] * screen_height + ui_element.size[1]))
    {
      if (m_left_click)
        ui_element.click_type[0] = true;
      if (m_middle_click)
        ui_element.click_type[1] = true;
      if (m_right_click)
        ui_element.click_type[2] = true;

      ui_element.hover = true;
    }    
  }
}

bool Cascade::Inputs::WasPressed(const SDL_Scancode& key)
{
  return m_pressed_keys[key];
}

bool Cascade::Inputs::WasReleased(const SDL_Scancode& key)
{
  return m_released_keys[key];
}

bool Cascade::Inputs::IsHeld(const SDL_Scancode& key)
{
  return m_held_keys[key];
}