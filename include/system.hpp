#ifndef CASCADE_SYSTEM_H
#define CASCADE_SYSTEM_H

// This file houses the base system class and some baked-in systems

#include "../external/entt/entt.hpp"
#include "../external/SDL/include/SDL3/SDL.h"
#include "../external/SDL_image/include/SDL3_image/SDL_image.h"

#include "components.hpp"

namespace Cascade
{
  class System
  {
  public:
    virtual void Update(entt::registry &registry) = 0;
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
    Graphics();
    ~Graphics();

    void LoadSpriteSheet(std::string sheet_name, std::string sheet_path);
    void CreateAnimation(std::string animation_name, std::string sheet_name, int update_interval);
    void AddFrame(std::string animation_name, int x, int y, int w, int h);
    void SetAnimationOffset(std::string animation_name, int dx, int dy);
    void SetLayer(entt::registry &registry, entt::entity entity, int layer);

    void SetCurrentAnimation(entt::registry &registry, entt::entity entity, std::string animation_name, int end_behavior);

    float GetCameraZoom() { return m_camera.zoom; };
    void SetCameraZoom(float zoom);

    int GetScreenWidth();
    int GetScreenHeight();

    void Update(entt::registry &registry) override;
    void CalculateDestinations(entt::registry &registry);
    void DrawEntities(entt::registry &registry);
    void UpdateDrawingState(DrawingState& drawing_state);
    void DrawLine(float a[2], float b[2], int color[4]);

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
  };
}

#endif