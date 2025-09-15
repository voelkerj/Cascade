#ifndef CASCADE_CASCADE_H
#define CASCADE_CASCADE_H

#include "../external/entt/entt.hpp"
#include "system.hpp"
#include "inputs.hpp"

#include "../external/SDL/include/SDL3/SDL.h"

class Cascade
{
public:
  Cascade();
  ~Cascade();

  entt::entity CreateEntity();
  void DestroyEntity(entt::entity entity);

  template <typename T>
  void AddComponent(entt::entity entity, T component_data)
  {
    m_entt_registry.emplace<T>(entity, component_data);
  }

  template <typename T>
  void RemoveComponent(entt::entity entity)
  {
    m_entt_registry.remove<T>(entity);
  }

  template <typename T, typename... Args>
  void AddSystem(std::string system_name, Args... args)
  {
    if (std::is_base_of<System, T>::value)
    {
      std::cerr << system_name << " must derive from System class.\n";
      exit(1);
    }

    m_systems.emplace(system_name, std::make_unique<T>(std::forward<Args>(args)...));
  }

  template <typename T>
  T *GetSystem(std::string system_name)
  {
    return m_systems[system_name].get();
  }

  void LoadSpriteSheet(std::string sheet_name, std::string sheet_path);
  void CreateAnimation(std::string animation_name, std::string sheet_name, int update_interval);
  void AddFrame(std::string animation_name, int x, int y, int w, int h);
  void SetCurrentAnimation(entt::entity entity, std::string animation_name);

  void StartFrame();
  void EndFrame();

  void UpdateInputEvents();
  bool WasPressed(const SDL_Scancode &key) { return m_inputs.WasPressed(key); };
  bool WasReleased(const SDL_Scancode &key) { return m_inputs.WasPressed(key); };
  bool IsHeld(const SDL_Scancode &key) { return m_inputs.WasPressed(key); };

private:
  // ECS
  entt::registry m_entt_registry;
  std::map<std::string, std::unique_ptr<System>> m_systems;

  // SDL Stuff
  std::string m_base_path;

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