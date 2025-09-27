#include <iostream>

#include "game.hpp"

Cascade::Game::Game()
{
  // Initialize SDL
  SDL_Init(SDL_INIT_VIDEO);
  m_base_path = SDL_GetBasePath();

  AddSystem<Graphics>("graphics");
}

Cascade::Game::~Game(){}

entt::entity Cascade::Game::CreateEntity()
{
  return m_entt_registry.create();
}

void Cascade::Game::DestroyEntity(entt::entity entity)
{
  m_entt_registry.destroy(entity);
}

void Cascade::Game::LoadSpriteSheet(std::string sheet_name, std::string sheet_path)
{
  GetSystem<Graphics>("graphics")->LoadSpriteSheet(sheet_name, sheet_path);
}

void Cascade::Game::CreateAnimation(std::string animation_name, std::string sheet_name, int update_interval)
{
  GetSystem<Graphics>("graphics")->CreateAnimation(animation_name, sheet_name, update_interval);
}

void Cascade::Game::AddFrame(std::string animation_name, int x, int y, int w, int h)
{
  GetSystem<Graphics>("graphics")->AddFrame(animation_name, x, y, w, h);
}

void Cascade::Game::SetAnimationOffset(std::string animation_name, int dx, int dy)
{
  GetSystem<Graphics>("graphics")->SetAnimationOffset(animation_name, dx, dy);
}

void Cascade::Game::SetCurrentAnimation(entt::entity entity, std::string animation_name)
{
  if (auto drawing_state = m_entt_registry.try_get<DrawingState>(entity))
  {
    drawing_state->animation_name = animation_name;
    return;
  }

  m_entt_registry.emplace<DrawingState>(entity, animation_name);
}

void Cascade::Game::SetColor(entt::entity entity, int color[3])
{
  Cascade::DrawingState& drawing_state = m_entt_registry.get<Cascade::DrawingState>(entity);
  drawing_state.enable_tint = true;
  drawing_state.color[0] = color[0];
  drawing_state.color[1] = color[1];
  drawing_state.color[2] = color[2];
}

void Cascade::Game::ResetColor(entt::entity entity)
{
  Cascade::DrawingState& drawing_state = m_entt_registry.get<Cascade::DrawingState>(entity);
  drawing_state.enable_tint = false;
}

void Cascade::Game::SetCameraZoom(float zoom)
{
  GetSystem<Graphics>("graphics")->SetCameraZoom(zoom);
}

void Cascade::Game::StartFrame()
{
  m_frame_start_ticks = SDL_GetTicks();
  SDL_RenderClear(GetSystem<Graphics>("graphics")->GetRenderer());

  m_inputs.StartFrame();
  UpdateInputEvents();
}

void Cascade::Game::EndFrame()
{
  // Update all systems
  for (const auto& pair : m_systems)
  {
    pair.second->Update(m_entt_registry);
  }

  m_frame_end_ticks = SDL_GetTicks();
  EnforceFPS();
}

void Cascade::Game::EnforceFPS()
{
  float elapsed_time = (m_frame_end_ticks - m_frame_start_ticks) / 1000.f;

  if (elapsed_time < (1.0 / m_fps))
  {
    SDL_Delay((1.0 / m_fps) - elapsed_time);
    elapsed_time = 1.0 / m_fps;
  }
}

void Cascade::Game::UpdateInputEvents()
{
  if (SDL_PollEvent(&m_event))
  {
    if (m_event.type == SDL_EVENT_KEY_DOWN || m_event.type == SDL_EVENT_KEY_UP)
      m_inputs.HandleKeyboardEvent(m_event);
    else if (m_event.type == SDL_EVENT_MOUSE_BUTTON_DOWN || m_event.type == SDL_EVENT_MOUSE_BUTTON_UP)
      m_inputs.HandleMouseEvent(m_event);
  }
}