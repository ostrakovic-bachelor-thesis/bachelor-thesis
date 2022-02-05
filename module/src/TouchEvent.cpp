#include "TouchEvent.h"


TouchEvent::TouchEvent(Type type):
  m_touchType(type)
{}

TouchEvent::TouchEvent(Type type, const IArrayList<TouchPoint> &touchPoints):
  m_touchType(type)
{
  for (auto it = touchPoints.getBeginIterator(); it != touchPoints.getEndIterator(); ++it)
  {
    m_touchPoints.addElement(*it);
  }
}