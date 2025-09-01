#include "../include/cascade.hpp"

entt::entity Cascade::CreateEntity()
{
  return m_entt_registry.create();
}

void Cascade::DestroyEntity(entt::entity entity)
{
  m_entt_registry.destroy(entity);
}