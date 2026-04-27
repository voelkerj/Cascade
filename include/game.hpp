#ifndef CASCADE_GAME_H
#define CASCADE_GAME_H

#include <iostream>
#include <concepts>
#include <bitset>

#include "../external/entt/entt.hpp"
#include "system.hpp"
#include "inputs.hpp"
#include "components.hpp"
#include "scene.hpp"

#include "../external/SDL/include/SDL3/SDL.h"
#include "../external/SDL_ttf/include/SDL3_ttf/SDL_ttf.h"

namespace Cascade
{
  class Game
  {
  public:
    Game(){};
    ~Game(){};

    void Run();
    void Quit();
    bool Continue(){return m_continue;};

    entt::entity CreateEntity();
    void DestroyEntity(entt::entity entity);
    void DestroyAllEntities();

    entt::registry& GetRegistry(){return m_entt_registry;};

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

    template <typename T>
    T &GetComponent(entt::entity entity)
    {
      return m_entt_registry.get<T>(entity);
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

      auto new_system = m_systems.emplace(system_name, std::make_unique<T>(*this, std::forward<Args>(args)...));
      new_system.first->second->Load(); // Call the system's load function
    }

    // TODO: Upgrade to use concepts to check base class at compile time
    template <typename T, typename... Args>
    void RemoveSystem(std::string system_name, Args... args)
    {
      if (!std::is_base_of<System, T>::value)
      {
        std::cerr << system_name << " must derive from System class.\n";
        exit(1);
      }

      // Call system's cleanup function
      T* system = GetSystem<T>(system_name);
      system->Cleanup();

      // Erase from map
      m_systems.erase(system_name);
    }

    template <typename T, typename... Args>
    void AddScene(std::string scene_name, Args... args)
    {
      if (!std::is_base_of<Scene, T>::value)
      {
        std::cerr << scene_name << " must derive from Scene class.\n";
        exit(1);
      }

      m_scenes.emplace(scene_name, std::make_unique<T>(*this, std::forward<Args>(args)...));
    }

    template <typename T>
    T *GetSystem(std::string system_name)
    {
      return dynamic_cast<T *>(m_systems[system_name].get());
    }

    template <typename T>
    T *GetScene(std::string scene_name)
    {
      return dynamic_cast<T *>(m_scenes[scene_name].get());
    }

    void LoadSpriteSheet(std::string sheet_name, std::string sheet_path);
    std::vector<std::vector<int>> ReadTileFile(std::string tile_file);
    std::string ExtractTileLayerName(const std::string tile_file);
    void LoadTileLayer(std::string tile_file, int tile_size, std::string sprite_sheet_name, int drawing_layer);
    void LoadHexTileLayer(std::string tile_file, int tile_size, std::string sprite_sheet_name, int drawing_layer);
    void SetTileViewAngle(float view_angle) { GetSystem<Graphics>("graphics")->SetTileViewAngle(view_angle); };
    float GetTileViewAngle() { return GetSystem<Graphics>("graphics")->GetTileViewAngle(); };
    void SetColliderTiles(std::string tile_file, int tile_width, std::vector<int> collider_tiles);
    void CreateAnimation(std::string animation_name, std::string sheet_name, int update_interval);
    void SetAnimationOffset(std::string animation_name, int dx, int dy);
    void AddFrame(std::string animation_name, int x, int y, int w, int h);
    void SetCurrentAnimation(entt::entity entity, std::string animation_name, int end_behavior);
    std::string GetCurrentAnimation(entt::entity entity);
    void SetLayer(entt::entity entity, int layer){ GetSystem<Graphics>("graphics")->SetLayer(m_entt_registry, entity, layer); };
    void SortDrawingLayers();
    void SetColor(entt::entity entity, int color[3]);
    void FlipHorizontal(entt::entity entity);
    void FlipVertical(entt::entity entity);
    void ResetFlipHorizontal(entt::entity entity);
    void ResetFlipVertical(entt::entity entity);
    void ResetColor(entt::entity entity);
    void SetDrawColliders(bool draw_colliders) { GetSystem<Graphics>("graphics")->SetDrawColliders(draw_colliders); }; 
    void DrawLineWCS(float a[2], float b[2], int color[4]) { GetSystem<Graphics>("graphics")->DrawLineWCS(a, b, color); };
    void DrawLineWCS(std::vector<float> a, std::vector<float> b, int color[4]) { GetSystem<Graphics>("graphics")->DrawLineWCS(a, b, color); };
    void DrawCircleWCS(const std::vector<float> center, const float radius, int color[4]) { GetSystem<Graphics>("graphics")->DrawCircleWCS(center, radius, color); };

    void SetHoverAnimation(entt::entity button, std::string animation_name);
    void SetClickAnimation(entt::entity button, std::string animation_name);

    void LoadSound(std::string sound_name, std::string sound_path);
    void PlaySound(std::string sound_name, bool loop_sound);
    void SetFrequencyRatio(std::string sound_name, float ratio);
    bool IsSoundPlaying(std::string sound_name);
    void StopSound(std::string sound_name);
    void StopAllSounds();

    void LoadFont(std::string font_name, std::string font_path, float font_size);
    void WriteText(std::string text, std::string font_name, float position[2], Cascade::Color color, int layer);

    float GetCameraZoom() { return GetSystem<Graphics>("graphics")->GetCameraZoom(); };
    void SetCameraZoom(float zoom);
    void SetCameraPosition(const std::vector<float> &position);
    std::vector<float> GetCameraPosition() {return GetSystem<Graphics>("graphics")->GetCameraPosition(); };

    // Coordinate Conversions
    std::vector<float> PCS2WCS(std::vector<float> &pos_PCS) {return GetSystem<Graphics>("graphics")->PCS2WCS(pos_PCS); };
    std::vector<float> WCS2PCS(std::vector<float> &pos_WCS) {return GetSystem<Graphics>("graphics")->WCS2PCS(pos_WCS); };
    std::vector<float> PCS2SDL(std::vector<float> &pos_PCS) {return GetSystem<Graphics>("graphics")->PCS2SDL(pos_PCS); };
    std::vector<float> SDL2PCS(std::vector<float> &pos_SDL) {return GetSystem<Graphics>("graphics")->SDL2PCS(pos_SDL); };

    void UpdateCollider(entt::entity entity);
    std::bitset<4> GetAABBCollisions(entt::entity entity_1);

    int GetScreenWidth(){return GetSystem<Graphics>("graphics")->GetScreenWidth();};
    int GetScreenHeight(){return GetSystem<Graphics>("graphics")->GetScreenHeight();};

    void StartFrame();
    void EndFrame();

    void UpdateUIAnimations();

    void UpdateInputEvents();
    bool WasPressed(const SDL_Scancode &key) { return m_inputs.WasPressed(key); };
    bool WasPressed(entt::entity entity, int mouse_button); // mouse_button: 0 = left, 1 = middle, 2 = right
    bool WasReleased(const SDL_Scancode &key) { return m_inputs.WasReleased(key); };
    bool IsHeld(const SDL_Scancode &key) { return m_inputs.IsHeld(key); };
    std::vector<float> GetMouseCoords(){return m_inputs.GetMouseCoords(); };
    bool LeftClick() {return m_inputs.m_left_click; };
    bool MiddleClick() {return m_inputs.m_middle_click; };
    bool RightClick() {return m_inputs.m_right_click; };

    void AddActiveScene(std::string scene_name);
    void RemoveActiveScene(std::string scene_name);
    void SceneEnd() {m_scene_ending_needed = true;};

    std::string GetBasePath() { return m_base_path; };

    void SetFPS(int fps) {m_fps = fps; };

  private:
    // Flow Control
    bool m_continue{true}; // true = continue running game, false = end game

    // ECS
    entt::registry m_entt_registry;
    std::map<std::string, std::unique_ptr<System>> m_systems;

    // SDL Stuff
    std::string m_base_path;

    // Frames
    bool m_first_frame{true};
    int m_fps{60};
    Uint32 m_frame_start_ticks{0};
    Uint32 m_frame_end_ticks{1};
    void EnforceFPS();

    // Inputs
    Inputs m_inputs;
    SDL_Event m_event;

    // Scenes
    std::map<std::string, std::unique_ptr<Scene>> m_scenes; // don't access directly, use GetScene()
    std::map<std::string, bool> m_active_scenes; // bool is false until scene is loaded
    bool m_scene_loading_needed{false};
    bool m_scene_ending_needed{false};
  };
}

#endif