#include "TouchEvent.h"


TouchEvent::TouchEvent(uint64_t id, Type type):
  m_id(id),
  m_type(type)
{}

TouchEvent::TouchEvent(uint64_t id, Type type, const IArrayList<GUI::Point> &touchPoints):
  m_id(id),
  m_type(type)
{
  for (auto it = touchPoints.getBeginIterator(); it != touchPoints.getEndIterator(); ++it)
  {
    m_touchPoints.addElement(*it);
  }
}

bool TouchEvent::operator==(const TouchEvent &touchEvent) const
{
  return (touchEvent.m_id == m_id) && (touchEvent.m_type == m_type) && (touchEvent.m_touchPoints == m_touchPoints);
}

bool TouchEvent::operator!=(const TouchEvent &touchEvent) const
{
  return not (touchEvent == *this);
}