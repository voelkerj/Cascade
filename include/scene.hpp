#ifndef CASCADE_SCENE_H
#define CASCADE_SCENE_H

#include "../external/entt/entt.hpp"

namespace Cascade
{
  class Game;

  // Scenes are class instances that control which entities are active
  // at any given time. They also control behavior of any user input
  // during that scene.
  // Generally there is only one scene at a time, but this is not a 
  // hard restriction and there can be an unlimited number of active 
  // scenes.
  // If an entity is created in the Load() function and needs to be 
  // referenced in Update() or End(), then a TrackEntity function is
  // provided to do this.

  class Scene
  {
  public: 
    Scene(Cascade::Game &cascade) : m_game(cascade) {};

    virtual void Load() = 0;
    virtual void Update() = 0;
    void EndScene();
    virtual void Cleanup() = 0; // Cleanup function is called automatically when the scene is ended

    void TrackEntity(std::string entity_name, entt::entity entity); // entities are just id numbers so we don't mind copying them
    entt::entity GetEntity(std::string entity_name);

    bool m_end_scene{false};
    bool m_start_scene{false};

    private:
    std::map<std::string, entt::entity> m_tracked_entites;

    protected:
    Cascade::Game &m_game;
  };
}

#endif