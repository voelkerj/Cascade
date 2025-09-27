#include <iostream>

#include "../include/system.hpp"
#include "../include/components.hpp"

Cascade::Graphics::Graphics()
{
  // Initialize Window
  m_window = SDL_CreateWindow("Cascade", 0, 0, SDL_WINDOW_FULLSCREEN);
  if (m_window == nullptr)
  {
    std::cerr << SDL_GetError() << std::endl;
    exit(1);
  }

  // Initialize Renderer
  m_renderer = SDL_CreateRenderer(m_window, NULL);
  if (m_renderer == nullptr)
  {
    std::cerr << SDL_GetError() << std::endl;
    exit(1);
  }

  SDL_SetRenderDrawColor(m_renderer, 0x01, 0x06, 0x0d, 0xFF);

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

Cascade::Graphics::~Graphics()
{
  SDL_DestroyWindow(m_window);
}

void Cascade::Graphics::LoadSpriteSheet(std::string sheet_name, std::string sheet_path)
{
  SDL_Texture *sprite_sheet = IMG_LoadTexture(m_renderer, sheet_path.c_str());

  if (!sprite_sheet)
  {
    std::cerr << "Failed to load file " << sheet_path << "\n";
    std::cerr << SDL_GetError() << std::endl;
    exit(1);    
  }

  SDL_SetTextureScaleMode(sprite_sheet, SDL_SCALEMODE_NEAREST); // use nearest pixel scaling

  m_sprite_sheets.emplace(sheet_name, sprite_sheet);
}

void Cascade::Graphics::CreateAnimation(std::string animation_name, std::string sheet_name, int update_interval)
{
  Animation new_animation;
  new_animation.sprite_sheet = sheet_name;
  new_animation.update_interval = update_interval;

  m_animations.emplace(animation_name, new_animation);
}

void Cascade::Graphics::AddFrame(std::string animation_name, int x, int y, int w, int h)
{
  SDL_FRect frame;
  frame.x = x;
  frame.y = y;
  frame.w = w;
  frame.h = h;

  m_animations[animation_name].frames.push_back(frame);
}

void Cascade::Graphics::SetCameraZoom(float zoom)
{
  m_camera.zoom = zoom;

  int screen_width, screen_height;
  SDL_GetCurrentRenderOutputSize(m_renderer, &screen_width, &screen_height);
  m_camera.FOV[0] = screen_width / m_camera.zoom;
  m_camera.FOV[1] = screen_height / m_camera.zoom;
  m_scale[0] = m_window_size[0] / m_camera.FOV[0];
  m_scale[1] = m_window_size[1] / m_camera.FOV[1];
}

void Cascade::Graphics::Update(entt::registry &registry)
{
  DrawEntities(registry);
  SDL_SetRenderDrawColor(m_renderer, 0x01, 0x06, 0x0d, 0xFF);
  SDL_RenderPresent(m_renderer);
}

void Cascade::Graphics::DrawEntities(entt::registry &registry)
{
  auto view = registry.view<DrawingState, const State>();

  for (auto [entity, drawing_state, state] : view.each())
  {
    // Get frame index based on elapsed time
    // Only do this if there is more than one frame in this animation
    if (m_animations[drawing_state.animation_name].frames.size() > 1)
    {
      Uint32 elapsed_ticks = SDL_GetTicks() - drawing_state.prev_update_ticks;
      if (elapsed_ticks >= m_animations[drawing_state.animation_name].update_interval)
      {
        drawing_state.prev_update_ticks = SDL_GetTicks();
        drawing_state.frame_idx++;
      }

      // Don't overrun frame vector
      if (drawing_state.frame_idx >= m_animations[drawing_state.animation_name].frames.size())
      {
        drawing_state.frame_idx = 0;
      }
    }

    // Get clipping rectangle based on frame index
    SDL_FRect clipping_rect = m_animations[drawing_state.animation_name].frames[drawing_state.frame_idx];

    // Get destination rectangle
    SDL_FRect destination_rect;
    destination_rect.x = (state.X - clipping_rect.w / 2 - (m_camera.pos[0] - (m_camera.FOV[0] / 2))) * m_scale[0];
    destination_rect.y = m_window_size[1] - (state.Y - clipping_rect.h / 2 - (m_camera.pos[1] - (m_camera.FOV[1] / 2))) * m_scale[1] - (clipping_rect.h * m_scale[1]);
    destination_rect.h = clipping_rect.h * m_scale[1];
    destination_rect.w = clipping_rect.w * m_scale[0];

    // TODO: Not necessary to allocate a string here for every animation, every frame.
    //       But it sure does help with readability.
    std::string sprite_sheet_name = m_animations[drawing_state.animation_name].sprite_sheet;

    // std::cout << sprite_sheet_name << "\n";
    // std::cout << destination_rect.x << ", " << destination_rect.y << ", " << destination_rect.h << ", " << destination_rect.w << "\n";
    // std::cout << clipping_rect.x << ", " << clipping_rect.y << ", " << clipping_rect.h << ", " << clipping_rect.w << "\n";

    if (drawing_state.enable_tint)
      SDL_SetTextureColorMod(m_sprite_sheets[sprite_sheet_name], drawing_state.color[0], drawing_state.color[1], drawing_state.color[2]);

    SDL_RenderTextureRotated(m_renderer, m_sprite_sheets[sprite_sheet_name], &clipping_rect, &destination_rect,
                             -state.Angle, NULL, SDL_FLIP_NONE);
  }
}

void Cascade::Graphics::DrawLine(float a[2], float b[2], int color[4])
{
  float a_x = (a[0] - (m_camera.pos[0] - (m_camera.FOV[0] / 2))) * m_scale[0];
  float a_y = m_window_size[1] - (a[1] - (m_camera.pos[1] - (m_camera.FOV[1] / 2))) * m_scale[1];

  float b_x = (b[0] - (m_camera.pos[0] - (m_camera.FOV[0] / 2))) * m_scale[0];
  float b_y = m_window_size[1] - (b[1] - (m_camera.pos[1] - (m_camera.FOV[1] / 2))) * m_scale[1];

  SDL_SetRenderDrawColor(m_renderer, color[0], color[1], color[2], color[3]);

  SDL_RenderLine(m_renderer, a_x, a_y, b_x, b_y);
}