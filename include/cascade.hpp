#ifndef CASCADE_CASCADE_H
#define CASCADE_CASCADE_H

#include "entt.hpp"
#include "system.hpp"
#include "inputs.hpp"

#include "SDL.h"

class Cascade
{
public:
  Cascade();
  ~Cascade() {};

  entt::entity CreateEntity();
  void DestroyEntity(entt::entity entity);

  void StartFrame();
  void EndFrame();

  void UpdateInputEvents();

private:
  // ECS
  entt::registry m_entt_registry;
  std::vector<System> m_systems;

  // SDL Stuff
  std::string m_base_path;

  // Frames
  int fps{60};
  Uint32 m_frame_start_ticks;
  Uint32 m_frame_end_ticks;
  void EnforceFPS();

  // Inputs
  Inputs m_inputs;
  SDL_Event m_event;
};

#endif