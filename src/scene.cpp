#include "../include/scene.hpp"
#include "../include/game.hpp"

void Cascade::Scene::TrackEntity(std::string entity_name, entt::entity entity)
{
  m_tracked_entites[entity_name] = entity;
}

entt::entity Cascade::Scene::GetEntity(std::string entity_name)
{
  if (m_tracked_entites.find(entity_name) != m_tracked_entites.end())
  {
    return m_tracked_entites[entity_name];
  } else {
    std::cerr << "No entity " << entity_name << "\n";
    exit(1);
  }
}

// The definition of Cascade::Game is needed for the EndScene function.
void Cascade::Scene::EndScene() 
{
  m_game.SceneEnd();
  m_end_scene = true;
}