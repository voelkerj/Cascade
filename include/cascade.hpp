#ifndef CASCADE_CASCADE_H
#define CASCADE_CASCADE_H

#include "../external/entt/entt.hpp"
#include "system.hpp"
#include "inputs.hpp"

#include "../external/SDL/include/SDL3/SDL.h"
#include "../external/SDL_image/include/SDL3_image/SDL_image.h"

struct Camera {
  // X & Y Position of Camera's origin point (center of screen)
  float pos[2];

  float zoom{1}; // Camera zoom factor. Ratio of pixels per world unit.

  // Width & Height of the Camera's projected FOV in WCS
  float FOV[2];

  float screen_top_y;
  float screen_bottom_y;
  float screen_left_x;
  float screen_right_x;
};

// Shared, read-only animation definition
struct Animation {
  int update_interval; // milliseconds
  std::vector<SDL_FRect> frames;
  std::string sprite_sheet;
};

// Per-entity animation state
struct AnimationInstance {
  std::string animation_name; // ID to look up the AnimationData
  int frame_idx{0};
  Uint32 prev_update_ticks{0};
};

class Cascade
{
public:
  Cascade();
  ~Cascade();

  entt::entity CreateEntity();
  void DestroyEntity(entt::entity entity);

  template <typename T> void AddComponent(entt::entity entity, T component_data)
  {m_entt_registry.emplace<T>(entity, component_data);}
  
  template <typename T> void RemoveComponent(entt::entity entity)
  {m_entt_registry.remove<T>(entity);}

  void LoadSpriteSheet(std::string sheet_name, std::string sheet_path);
  void CreateAnimation(std::string animation_name, std::string sheet_name, int update_interval);
  void AddFrame(std::string animation_name, int x, int y, int w, int h);

  void StartFrame();
  void EndFrame();

  void UpdateInputEvents();

private:
  // ECS
  entt::registry m_entt_registry;
  std::vector<System> m_systems;

  // SDL Stuff
  std::string m_base_path;
  SDL_Window* m_window;
  int m_window_size[2]; // Width & Height
  SDL_Renderer* m_renderer;

  // Graphics
  Camera m_camera;
  float m_scale[2]; // X & Y scale
  std::map<std::string, SDL_Texture*> m_sprite_sheets;
  std::unordered_map<std::string, Animation> m_animations;

  // Frames
  int m_fps{60};
  Uint32 m_frame_start_ticks;
  Uint32 m_frame_end_ticks;
  void EnforceFPS();

  // Inputs
  Inputs m_inputs;
  SDL_Event m_event;
};

#endif