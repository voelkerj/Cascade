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

void Cascade::Game::Run()
{
  while (Continue())
  {
    StartFrame();
    EndFrame();
  }
}

void Cascade::Game::Quit()
{
  m_continue = false;
  DestroyAllEntities();
  SDL_Quit();
}

entt::entity Cascade::Game::CreateEntity()
{
  return m_entt_registry.create();
}

void Cascade::Game::DestroyEntity(entt::entity entity)
{
  m_entt_registry.destroy(entity);
}

void Cascade::Game::DestroyAllEntities()
{
  m_entt_registry.clear();
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

void Cascade::Game::SetCurrentAnimation(entt::entity entity, std::string animation_name, int end_behavior)
{
  GetSystem<Graphics>("graphics")->SetCurrentAnimation(m_entt_registry, entity, animation_name, end_behavior);
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

void Cascade::Game::SetHoverAnimation(entt::entity button, std::string animation_name)
{
  if (auto ui_element = m_entt_registry.try_get<UIElement>(button))
  {
    ui_element->hover_animation = animation_name;
    return;
  }

  std::cerr << "Cannot set hover animation, not a UI element!\n";
  exit(1);
}

void Cascade::Game::SetClickAnimation(entt::entity button, std::string animation_name)
{
  if (auto ui_element = m_entt_registry.try_get<UIElement>(button))
  {
    ui_element->click_animation = animation_name;
    return;
  }

  std::cerr << "Cannot set click animation, not a UI element!\n";
  exit(1);
}

void Cascade::Game::SetCameraZoom(float zoom)
{
  GetSystem<Graphics>("graphics")->SetCameraZoom(zoom);
}

void Cascade::Game::StartFrame()
{
  // Load scenes if needed
  if (m_scene_loading_needed)
  {
    for (const auto &pair : m_active_scenes)
    {
      // if scene not loaded, load it
      if (!pair.second)
      {
        GetScene<Scene>(pair.first)->Load(*this);
      }
    }

    m_scene_loading_needed = false;
  }

  m_frame_start_ticks = SDL_GetTicks();
  SDL_RenderClear(GetSystem<Graphics>("graphics")->GetRenderer());

  m_inputs.StartFrame(m_entt_registry, GetScreenWidth(), GetScreenHeight());
  UpdateInputEvents();
}

void Cascade::Game::EndFrame()
{
  if (!m_continue)
  {
    return;
  }

  // Update all active scenes
  for (const auto &pair : m_active_scenes)
  {
    GetScene<Scene>(pair.first)->Update(*this);
  }

  // Update all systems
  for (const auto& pair : m_systems)
  {
    pair.second->Update(m_entt_registry);
  }

  // End scenes
  if (m_scene_ending_needed)
  {
    for (const auto &pair : m_active_scenes)
    {
      if (m_scenes[pair.first]->m_end_scene)
      {
        GetScene<Scene>(pair.first)->Cleanup(*this);

        RemoveActiveScene(pair.first);

        GetScene<Scene>(pair.first)->m_end_scene = false;
      }
    }

    m_scene_ending_needed = false;
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

void Cascade::Game::UpdateUIAnimations()
{
  GetSystem<Graphics>("graphics")->UpdateUIAnimations(m_entt_registry);
}

void Cascade::Game::UpdateInputEvents()
{
  if (SDL_PollEvent(&m_event))
  {
    if (m_event.type == SDL_EVENT_KEY_DOWN || m_event.type == SDL_EVENT_KEY_UP)
    {
      m_inputs.HandleKeyboardEvent(m_event);
    }
    else if (m_event.type == SDL_EVENT_MOUSE_BUTTON_DOWN || m_event.type == SDL_EVENT_MOUSE_BUTTON_UP)
    {
      m_inputs.HandleMouseEvent(m_event, m_entt_registry, GetScreenWidth(), GetScreenHeight());
    }
  }
}

bool Cascade::Game::WasPressed(entt::entity entity, int mouse_button)
{
  UIElement& ui_element = m_entt_registry.get<UIElement>(entity);
  return ui_element.click_type[mouse_button];
}

void Cascade::Game::AddActiveScene(std::string scene_name)
{
  // Enforce scene can only be added once
  if (m_active_scenes.find(scene_name) != m_active_scenes.end())
  {
    std::cerr << "Cannot add same scene to active scenes more than once!\n";
    exit(1);
  }

  // Add scene
  m_active_scenes[scene_name] = false;

  // Tell game we have a new scene to load
  m_scene_loading_needed = true;
}

void Cascade::Game::RemoveActiveScene(std::string scene_name)
{
  m_active_scenes.erase(scene_name);
}