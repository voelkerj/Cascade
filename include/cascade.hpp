#ifndef CASCADE_CASCADE_H
#define CASCADE_CASCADE_H

#include "../external/entt/entt.hpp"
#include "system.hpp"

class Cascade
{
  public:
  Cascade(){};
  ~Cascade(){};

  entt::entity CreateEntity();
  void DestroyEntity(entt::entity entity);

  private:
  entt::registry m_entt_registry;
  std::vector<System> systems;
};

#endif