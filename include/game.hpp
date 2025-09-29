#ifndef CASCADE_GAME_H
#define CASCADE_GAME_H

#include <iostream>
#include <concepts>

#include "../external/entt/entt.hpp"
#include "system.hpp"
#include "inputs.hpp"
#include "components.hpp"

#include "../external/SDL/include/SDL3/SDL.h"

namespace Cascade
{
  class Game
  {
  public:
    Game();
    ~Game();

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

    // TODO: Upgrade to use concepts to check base class at compile time
    template <typename T, typename... Args>
    void AddSystem(std::string system_name, Args... args)
    {
      if (!std::is_base_of<System, T>::value)
      {
        std::cerr << system_name << " must derive from System class.\n";
        exit(1);
      }

      m_systems.emplace(system_name, std::make_unique<T>(std::forward<Args>(args)...));
    }

    template <typename T>
    T *GetSystem(std::string system_name)
    {
      return dynamic_cast<T *>(m_systems[system_name].get());
    }

    void LoadSpriteSheet(std::string sheet_name, std::string sheet_path);
    void CreateAnimation(std::string animation_name, std::string sheet_name, int update_interval);
    void SetAnimationOffset(std::string animation_name, int dx, int dy);
    void AddFrame(std::string animation_name, int x, int y, int w, int h);
    void SetCurrentAnimation(entt::entity entity, std::string animation_name, int end_behavior);
    void SetColor(entt::entity entity, int color[3]);
    void ResetColor(entt::entity entity);
    void DrawLine(float a[2], float b[2], int color[4]) { GetSystem<Graphics>("graphics")->DrawLine(a, b, color); };

    float GetCameraZoom() { return GetSystem<Graphics>("graphics")->GetCameraZoom(); };
    void SetCameraZoom(float zoom);

    int GetScreenWidth(){return GetSystem<Graphics>("graphics")->GetScreenWidth();};
    int GetScreenHeight(){return GetSystem<Graphics>("graphics")->GetScreenHeight();};

    void StartFrame();
    void EndFrame();

    void UpdateInputEvents();
    bool WasPressed(const SDL_Scancode &key) { return m_inputs.WasPressed(key); };
    bool WasPressed(entt::entity entity, int mouse_button);
    bool WasReleased(const SDL_Scancode &key) { return m_inputs.WasReleased(key); };
    bool IsHeld(const SDL_Scancode &key) { return m_inputs.IsHeld(key); };

    std::string GetBasePath() { return m_base_path; };

    void SetFPS(int fps) {m_fps = fps; };

  private:
    // ECS
    entt::registry m_entt_registry;
    std::map<std::string, std::unique_ptr<System>> m_systems;

    // SDL Stuff
    std::string m_base_path;

    // Frames
    int m_fps{60};
    Uint32 m_frame_start_ticks{0};
    Uint32 m_frame_end_ticks{1};
    void EnforceFPS();

    // Inputs
    Inputs m_inputs;
    SDL_Event m_event;
  };
}

#endif