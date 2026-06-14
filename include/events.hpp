#ifndef CASCADE_EVENTS_H
#define CASCADE_EVENTS_H

#include "system.hpp"

namespace Cascade
{
  class Events : public System
  {
  public:
    using System::System;

    void Load() override {};
    void Update() override {};
    void Cleanup() override {};

    void AddEvent(std::string event_name);
    void AddAndTriggerEvent(std::string event_name);
    void TriggerEvent(std::string event_name);

    void RemoveEvent(std::string event_name);    
    void ResolveEvent(std::string event_name);

    bool IsResolved(std::string event_name);

    bool EventExists(std::string event_name);

  private:
    std::map<std::string, bool> m_events; // True = Triggered, False = Resolved
  };
}

#endif