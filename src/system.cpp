#include <iostream>

#include "../include/system.hpp"
#include "../include/game.hpp"
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

void Cascade::Graphics::GetSpriteSheetSize(std::string sheet_name, float &width, float &height)
{
  SDL_GetTextureSize(m_sprite_sheets[sheet_name], &width, &height);
}

void Cascade::Graphics::CreateAnimation(std::string animation_name, std::string sheet_name, int update_interval)
{
  Animation new_animation;
  new_animation.sprite_sheet = sheet_name;
  new_animation.update_interval = update_interval;

  m_animations.emplace(animation_name, new_animation);
}

bool Cascade::Graphics::AnimationExists(std::string animation_name)
{
  if (m_animations.find(animation_name) == m_animations.end())
  {
    return false;
  }

  return true;
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

void Cascade::Graphics::SetAnimationOffset(std::string animation_name, int dx, int dy)
{
  m_animations[animation_name].offset[0] = dx;
  m_animations[animation_name].offset[1] = dy;
}

void Cascade::Graphics::SetLayer(entt::registry &registry, entt::entity entity, int layer)
{
  if (auto drawing_state = registry.try_get<DrawingState>(entity))
  {
    drawing_state->layer = layer;

    return;
  }
}

void Cascade::Graphics::SetCurrentAnimation(entt::registry &registry, entt::entity entity, std::string animation_name, int end_behavior)
{
  // First check if this is a valid animation name
  if (!AnimationExists(animation_name))
  {
    std::cerr << animation_name << " is not a valid animation!\n";
    exit(1);
  }

  if (auto drawing_state = registry.try_get<DrawingState>(entity))
  {
    drawing_state->animation_name = animation_name;

    drawing_state->current_animation_end_behavior = end_behavior;

    drawing_state->frame_idx = 0;

    return;
  }

  if (end_behavior == 1)
  {
    std::cerr << "Cannot set animation to run once if it has no previous animation!\n";
    exit(1);
  }

  DrawingState new_drawing_state;
  new_drawing_state.animation_name = animation_name;
  new_drawing_state.default_animation_name = animation_name;

  registry.emplace<DrawingState>(entity, new_drawing_state);
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

int Cascade::Graphics::GetScreenWidth()
{
  int screen_width, screen_height;
  SDL_GetCurrentRenderOutputSize(m_renderer, &screen_width, &screen_height);
  return screen_width;
}

int Cascade::Graphics::GetScreenHeight()
{
  int screen_width, screen_height;
  SDL_GetCurrentRenderOutputSize(m_renderer, &screen_width, &screen_height);
  return screen_height;
}

void Cascade::Graphics::Update(Cascade::Game &cascade)
{
  entt::registry &registry = cascade.GetRegistry();
  UpdateUIAnimations(registry);
  CalculateDestinations(registry);
  DrawEntities(registry);
  SDL_SetRenderDrawColor(m_renderer, 0x01, 0x06, 0x0d, 0xFF);
  SDL_RenderPresent(m_renderer);
}

void Cascade::Graphics::CalculateDestinations(entt::registry &registry)
{
  // World Entities
  auto view = registry.view<DrawingState, const State>();

  for (auto [entity, drawing_state, state] : view.each())
  {
    // Get clipping rectangle based on frame index
    SDL_FRect clipping_rect = m_animations[drawing_state.animation_name].frames[drawing_state.frame_idx];

    drawing_state.destination_rect.x = (state.X + m_animations[drawing_state.animation_name].offset[0] - clipping_rect.w / 2 - (m_camera.pos[0] - (m_camera.FOV[0] / 2))) * m_scale[0];
    drawing_state.destination_rect.y = m_window_size[1] - (state.Y + m_animations[drawing_state.animation_name].offset[1] - clipping_rect.h / 2 - (m_camera.pos[1] - (m_camera.FOV[1] / 2))) * m_scale[1] - (clipping_rect.h * m_scale[1]);
    drawing_state.destination_rect.w = clipping_rect.w * state.ScaleX * m_scale[0];
    drawing_state.destination_rect.h = clipping_rect.h * state.ScaleY * m_scale[1];

    drawing_state.angle = -state.Angle;
  }

  // UI Entities
  auto view2 = registry.view<DrawingState, UIElement>();

  for (auto [entity, drawing_state, ui_element] : view2.each())
  {
    // Get clipping rectangle based on frame index
    SDL_FRect clipping_rect = m_animations[drawing_state.animation_name].frames[drawing_state.frame_idx]; 

    // Check if we need to change size
    if (ui_element.size[0] == 0)
    {
      ui_element.size[0] = clipping_rect.w;
    }

    if (ui_element.size[1] == 0)
    {
      ui_element.size[0] = clipping_rect.h;
    }

    drawing_state.destination_rect.w = ui_element.size[0];
    drawing_state.destination_rect.h = ui_element.size[1];

    // Get destination rectangle
    // UI Element Drawing is based on % location on screen
    drawing_state.destination_rect.x = GetScreenWidth() * ui_element.position[0] - drawing_state.destination_rect.w / 2;
    drawing_state.destination_rect.y = GetScreenHeight() * ui_element.position[1] - drawing_state.destination_rect.h / 2;

    // If we clicked and the animation is done
    if (!ui_element.click_animation_done && (drawing_state.animation_name == drawing_state.default_animation_name))
    {
      ui_element.click_animation_done = true;
    }

    drawing_state.angle = -ui_element.angle;
  }
}

void Cascade::Graphics::DrawEntities(entt::registry &registry)
{
  // First we sort by layer
  registry.sort<DrawingState>([](const DrawingState &a, const DrawingState &b)
  {
    return a.layer > b.layer;
  });

  auto view = registry.view<DrawingState>();

  for (auto [entity, drawing_state] : view.each())
  {
    UpdateDrawingState(drawing_state);

    SDL_FRect clipping_rect = m_animations[drawing_state.animation_name].frames[drawing_state.frame_idx]; 

    // TODO: Not necessary to allocate a string here for every animation, every frame.
    //       But it sure does help with readability.
    std::string sprite_sheet_name = m_animations[drawing_state.animation_name].sprite_sheet;

    if (drawing_state.enable_tint)
      SDL_SetTextureColorMod(m_sprite_sheets[sprite_sheet_name], drawing_state.color[0], drawing_state.color[1], drawing_state.color[2]);

    SDL_RenderTextureRotated(m_renderer, m_sprite_sheets[sprite_sheet_name], &clipping_rect, &drawing_state.destination_rect,
                             drawing_state.angle, NULL, SDL_FLIP_NONE);
  }
}

void Cascade::Graphics::UpdateDrawingState(DrawingState &drawing_state)
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
      // If we are only running this animation once
      if (drawing_state.current_animation_end_behavior == 1)
      {
        // return to previous animation
        drawing_state.animation_name = drawing_state.default_animation_name;
      }

      drawing_state.frame_idx = 0;
    }
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

void Cascade::Graphics::UpdateUIAnimations(entt::registry &registry)
{
  auto view = registry.view<UIElement>();

  for (auto [entity, ui_element] : view.each())
  {
    // Check for clicks first so we don't check hover unless we need to
    // (click animation supercedes hover animation)
    if ((ui_element.click_type[0] ||
         ui_element.click_type[1] ||
         ui_element.click_type[2]) && !ui_element.click_animation.empty())
    {
      SetCurrentAnimation(registry, entity, ui_element.click_animation, 1);
      ui_element.click_animation_done = false;
      continue;
    }

    if (ui_element.click_animation_done && ui_element.hover && !ui_element.hover_animation.empty())
    {
      SetCurrentAnimation(registry, entity, ui_element.hover_animation, 1);
    }
  }
}