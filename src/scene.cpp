#include "../include/scene.hpp"
#include "../include/game.hpp"

void Cascade::Scene::TrackEntity(std::string entity_name, entt::entity entity)
{
  m_tracked_entites[entity_name] = entity;
}

// The definition of Cascade::Game is needed for the EndScene function.
void Cascade::Scene::EndScene() 
{
  m_game.SceneEnd();
  m_end_scene = true;
}