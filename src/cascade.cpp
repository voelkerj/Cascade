#include "cascade.hpp"

Cascade::Cascade()
{
  SDL_Init(SDL_INIT_VIDEO);
  m_base_path = SDL_GetBasePath();
}

entt::entity Cascade::CreateEntity()
{
  return m_entt_registry.create();
}

void Cascade::DestroyEntity(entt::entity entity)
{
  m_entt_registry.destroy(entity);
}

void Cascade::StartFrame()
{
  m_frame_start_ticks = SDL_GetTicks();

  m_inputs.StartFrame();
  // graphics start frame
  UpdateInputEvents();
}

void Cascade::EndFrame()
{
  // update all systems
  EnforceFPS();
}

void Cascade::EnforceFPS()
{
  m_frame_end_ticks = SDL_GetTicks();

  float elapsed_time = (m_frame_end_ticks - m_frame_start_ticks) / 1000;

  if (elapsed_time < (1.0 / fps)) {
    SDL_Delay((1.0 / fps) - elapsed_time);
    elapsed_time = 1.0 / fps;
  }
}

void Cascade::UpdateInputEvents()
{
  if (SDL_PollEvent(&m_event))
  {
    if (m_event.type == SDL_EVENT_KEY_DOWN || m_event.type == SDL_EVENT_KEY_UP)
      m_inputs.HandleKeyboardEvent(m_event);
    else if (m_event.type == SDL_EVENT_MOUSE_BUTTON_DOWN || m_event.type == SDL_EVENT_MOUSE_BUTTON_UP)
      m_inputs.HandleMouseEvent(m_event);
  }
}