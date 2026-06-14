#include "../include/events.hpp"

// Add an event as resolved
void Cascade::Events::AddEvent(std::string event_name)
{
  m_events[event_name] = false;
}

// Add an event and trigger it
void Cascade::Events::AddAndTriggerEvent(std::string event_name)
{
  m_events[event_name] = true;
}

// Trigger an existing event
void Cascade::Events::TriggerEvent(std::string event_name)
{
  if (EventExists(event_name))
  {
    m_events[event_name] = true;
  }
  else
  {
    std::cerr << "Cannot trigger non-existant event " << event_name << "\n";
  }
}

void Cascade::Events::RemoveEvent(std::string event_name)
{
  if (EventExists(event_name))
  {
    m_events.erase(event_name);
  }
  else
  {
    std::cerr << "Cannot remove non-existant event " << event_name  << "\n";
  }
}

void Cascade::Events::ResolveEvent(std::string event_name)
{
  if (EventExists(event_name))
  {
    m_events[event_name] = false;
  }
  else
  {
    std::cerr << "Cannot resolve non-existant event " << event_name << "\n";
  }
}

bool Cascade::Events::IsResolved(std::string event_name)
{
  if (EventExists(event_name))
  {
    return !m_events[event_name];
  }
  else
  {
    std::cerr << "Cannot check status of non-existant event " << event_name << "\n";
  }
  return false;
}

bool Cascade::Events::EventExists(std::string event_name)
{
  if (m_events.contains(event_name))
  {
    return true;
  }

  return false;
}