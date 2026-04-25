#ifndef CASCADE_SYSTEM_H
#define CASCADE_SYSTEM_H

// This file houses the base system class and some baked-in systems

#include "../external/entt/entt.hpp"
#include "../external/SDL/include/SDL3/SDL.h"
#include "../external/SDL_ttf/include/SDL3_ttf/SDL_ttf.h"
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
    // World Coordinates X & Y of Camera's origin point (center of screen)
    std::vector<float> pos_WCS{0, 0};
    float zoom{1}; // Camera zoom factor. Ratio of pixels per world unit.
  };

  // Shared, read-only animation definition
  struct Animation
  {
    int update_interval; // milliseconds
    std::vector<SDL_FRect> frames;
    std::string sprite_sheet;
    int offset[2]{0, 0};
  };

  struct Sound
  {
    Uint8 *wav_data;
    Uint32 wav_data_length;
    SDL_AudioStream *stream;
    bool loop_sound;
    bool play{false}; // if true, schedule this sound to be played
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

    void LoadSpriteSheet(std::string sheet_name, std::string sheet_path);
    void StoreSpriteSheet(std::string sheet_name, SDL_Texture *sprite_sheet);
    void GetSpriteSheetSize(std::string sheet_name, float &width, float &height);
    SDL_Texture* GetSpriteSheet(std::string sheet_name) { return m_sprite_sheets[sheet_name]; };
    void CreateAnimation(std::string animation_name, std::string sheet_name, int update_interval);
    bool AnimationExists(std::string animation_name);
    void AddFrame(std::string animation_name, int x, int y, int w, int h);
    SDL_FRect &GetFrame(std::string animation_name, int frame_idx) { return m_animations[animation_name].frames[frame_idx]; };
    void SetAnimationOffset(std::string animation_name, int dx, int dy);
    void SetLayer(entt::registry &registry, entt::entity entity, int layer);
    void SetDrawColliders(bool draw_colliders);
    void SetTileViewAngle(float view_angle) {m_tile_view_angle = view_angle; };
    float GetTileViewAngle() {return m_tile_view_angle; };

    void SetCurrentAnimation(entt::registry &registry, entt::entity entity, std::string animation_name, int end_behavior);
    std::string GetCurrentAnimation(entt::registry &registry, entt::entity entity);

    float GetCameraZoom() { return m_camera.zoom; };
    void SetCameraZoom(float zoom);
    void SetCameraPosition(const std::vector<float> &position);
    std::vector<float> GetCameraPosition();

    int GetScreenWidth();
    int GetScreenHeight();

    // Coordinate Conversions
    std::vector<float> PCS2WCS(std::vector<float> &pos_PCS);
    std::vector<float> WCS2PCS(std::vector<float> &pos_WCS);
    std::vector<float> PCS2SDL(std::vector<float> &pos_PCS);
    std::vector<float> SDL2PCS(std::vector<float> &pos_SDL);

    void CalculateDestinations(entt::registry &registry);
    void DrawEntities(entt::registry &registry);
    void DrawColliders(entt::registry &registry);
    void UpdateDrawingState(DrawingState& drawing_state);
    void DrawLineWCS(float a[2], float b[2], int color[4]);
    void DrawLineWCS(std::vector<float> a, std::vector<float> b, int color[4]);
    void DrawCircleWCS(const std::vector<float> center, const float radius, int color[4]);

    void UpdateUIAnimations(entt::registry &registry);

    void LoadFont(std::string font_name, std::string font_path, float font_size);
    void WriteText(std::string text, std::string font_name, float position[2], Cascade::Color color, int layer);

    SDL_Renderer *GetRenderer() { return m_renderer; };
    SDL_Window *GetWindow() { return m_window; };

  private:
    SDL_Window *m_window;
    SDL_Renderer *m_renderer;
    Camera m_camera;
    // float m_scale[2]; // X & Y scale
    std::map<std::string, SDL_Texture *> m_sprite_sheets;
    std::unordered_map<std::string, Animation> m_animations;
    bool m_draw_colliders{false};
    std::map<std::string, std::shared_ptr<TTF_Font>> m_fonts;
    float m_tile_view_angle{0};
  };

  class Audio : public System
  {
  public:
    using System::System;

    void Load() override;
    void Update() override;
    void Cleanup() override;

    void LoadSound(std::string sound_name, std::string sound_path);
    void PlaySound(std::string sound_name, bool loop_sound);
    void SetFrequencyRatio(std::string sound_name, float ratio);
    bool IsSoundPlaying(std::string sound_name);
    void StopSound(std::string sound_name);
    void StopAllSounds();

  private:
    SDL_AudioDeviceID m_audio_device;
    std::map<std::string, std::unique_ptr<Sound>> m_sounds;
  };
}

#endif