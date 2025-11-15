#ifndef CASCADE_SYSTEM_H
#define CASCADE_SYSTEM_H

// This file houses the base system class and some baked-in systems

#include "../external/entt/entt.hpp"
#include "../external/SDL/include/SDL3/SDL.h"
#include "../external/SDL_image/include/SDL3_image/SDL_image.h"

#include "components.hpp"

namespace Cascade
{
  class Game;
  
  class System
  {
  public:
    System(Cascade::Game &cascade) : m_game(cascade) {};
    virtual void Load() = 0;
    virtual void Update() = 0;
    virtual void Cleanup() = 0;

    protected:
    Cascade::Game &m_game;
  };

  struct Camera
  {
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
  struct Animation
  {
    int update_interval; // milliseconds
    std::vector<SDL_FRect> frames;
    std::string sprite_sheet;
    int offset[2]{0, 0};
  };

  // Graphics System
  // Operates on components: CurrentAnimation and State
  class Graphics : public System
  {
  public:
    using System::System;

    void Load() override;
    void Update() override;
    void Cleanup() override;

    void UpdateCamera();
    void LoadSpriteSheet(std::string sheet_name, std::string sheet_path);
    void GetSpriteSheetSize(std::string sheet_name, float &width, float &height);
    void CreateAnimation(std::string animation_name, std::string sheet_name, int update_interval);
    bool AnimationExists(std::string animation_name);
    void AddFrame(std::string animation_name, int x, int y, int w, int h);
    void SetAnimationOffset(std::string animation_name, int dx, int dy);
    void SetLayer(entt::registry &registry, entt::entity entity, int layer);
    void SetDrawColliders(bool draw_colliders);

    void SetCurrentAnimation(entt::registry &registry, entt::entity entity, std::string animation_name, int end_behavior);
    std::string GetCurrentAnimation(entt::registry &registry, entt::entity entity);

    float GetCameraZoom() { return m_camera.zoom; };
    void SetCameraZoom(float zoom);
    void SetCameraPosition(float position[2]);

    int GetScreenWidth();
    int GetScreenHeight();
    std::vector<float> ConvertWCStoScreenCoords(float point[2]);

    void CalculateDestinations(entt::registry &registry);
    void DrawEntities(entt::registry &registry);
    void DrawColliders(entt::registry &registry);
    void UpdateDrawingState(DrawingState& drawing_state);
    void DrawLineWCS(float a[2], float b[2], int color[4]);

    void UpdateUIAnimations(entt::registry &registry);

    SDL_Renderer *GetRenderer() { return m_renderer; };

  private:
    SDL_Window *m_window;
    int m_window_size[2]; // Width & Height
    SDL_Renderer *m_renderer;
    Camera m_camera;
    float m_scale[2]; // X & Y scale
    std::map<std::string, SDL_Texture *> m_sprite_sheets;
    std::unordered_map<std::string, Animation> m_animations;
    bool m_draw_colliders{false};
  };
}

#endif