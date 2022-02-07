#include "GUITouchEvent.h"


GUI::TouchEvent::TouchEvent(uint64_t id, Type type):
  m_id(id),
  m_type(type)
{}

GUI::TouchEvent::TouchEvent(uint64_t id, Type type, const IArrayList<Point> &touchPoints):
  m_id(id),
  m_type(type)
{
  for (auto it = touchPoints.getBeginIterator(); it != touchPoints.getEndIterator(); ++it)
  {
    m_touchPoints.addElement(*it);
  }
}

bool GUI::TouchEvent::operator==(const TouchEvent &touchEvent) const
{
  return (touchEvent.m_id == m_id) && (touchEvent.m_type == m_type) && (touchEvent.m_touchPoints == m_touchPoints);
}

bool GUI::TouchEvent::operator!=(const TouchEvent &touchEvent) const
{
  return not (touchEvent == *this);
}