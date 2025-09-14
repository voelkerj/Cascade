#include <iostream>

#include "cascade.hpp"

Cascade::Cascade()
{
  // Initialize SDL
  SDL_Init(SDL_INIT_VIDEO);
  m_base_path = SDL_GetBasePath();

  // Initialize Window
  m_window = SDL_CreateWindow("Cascade", 0, 0, SDL_WINDOW_FULLSCREEN);
  if (m_window == nullptr) {
    std::cout << SDL_GetError() << std::endl;
    exit(1);
  }

  // Initialize Renderer
  m_renderer = SDL_CreateRenderer(m_window, NULL);
  if (m_renderer == nullptr) {
    std::cout << SDL_GetError() << std::endl;
    exit(1);
  }

  SDL_SetRenderDrawColor(m_renderer, 0x26, 0x26, 0x26, 0xFF);

  // Initialize Camera
  SDL_GetWindowSize(m_window, &m_window_size[0], &m_window_size[1]);
  int screen_width, screen_height;
  SDL_GetCurrentRenderOutputSize(m_renderer, &screen_width, &screen_height);

  m_camera.pos[0] = 0;
  m_camera.pos[1] = 0;
  m_camera.FOV[0] = screen_width / m_camera.zoom;
  m_camera.FOV[1] = screen_height / m_camera.zoom;

  m_scale[0] = m_window_size[0] / m_camera.FOV[0];
  m_scale[1] = m_window_size[1] / m_camera.FOV[1];
}

Cascade::~Cascade() {
  SDL_DestroyWindow(m_window);
}

entt::entity Cascade::CreateEntity()
{
  return m_entt_registry.create();
}

void Cascade::DestroyEntity(entt::entity entity)
{
  m_entt_registry.destroy(entity);
}

template <typename T> void Cascade::AddComponent(entt::entity entity, T component_data)
{
  m_entt_registry.emplace<T>(entity, component_data);
}

template <typename T> void Cascade::RemoveComponent(entt::entity entity)
{
  m_entt_registry.remove<T>(entity);
}

void Cascade::LoadSpriteSheet(std::string sheet_name, std::string sheet_path)
{
  SDL_Texture* sprite_sheet = IMG_LoadTexture(m_renderer, sheet_path.c_str());

  if (!sprite_sheet) {
    std::cout << "Failed to load file " << sheet_path << "\n";
    exit(1);
  }

  SDL_SetTextureScaleMode(sprite_sheet, SDL_SCALEMODE_NEAREST);  // use nearest pixel scaling

  m_sprite_sheets.emplace(sheet_name, sprite_sheet);
}

void Cascade::CreateAnimation(std::string animation_name, std::string sheet_name, int update_interval)
{
  Animation new_animation;
  new_animation.sprite_sheet = sheet_name;
  new_animation.update_interval = update_interval;

  m_animations.emplace(animation_name, new_animation);
}

void Cascade::AddFrame(std::string animation_name, int x, int y, int w, int h)
{
  SDL_FRect frame;
  frame.x = x;
  frame.y = y;
  frame.w = w;
  frame.h = h;

  m_animations[animation_name].frames.push_back(frame);
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

  if (elapsed_time < (1.0 / m_fps)) {
    SDL_Delay((1.0 / m_fps) - elapsed_time);
    elapsed_time = 1.0 / m_fps;
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