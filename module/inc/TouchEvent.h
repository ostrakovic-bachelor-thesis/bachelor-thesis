#ifndef TOUCH_EVENT_H
#define TOUCH_EVENT_H

#include <cstdint>
#include "IArrayList.h"
#include "ArrayList.h"


class TouchEvent
{
public:

  static constexpr uint32_t MAX_NUM_OF_SIMULTANEOUS_TOUCH_POINTS = 2u;

  enum class Type : uint8_t
  {
    TOUCH_START = 0u,
    TOUCH_MOVE  = 1u,
    TOUCH_STOP  = 2u
  };

  struct TouchPoint
  {
    int16_t x;
    int16_t y;
  };

  TouchEvent(Type type);

  TouchEvent(Type type, const IArrayList<TouchPoint> &touchPoints);

  inline Type getType(void) const
  {
    return m_touchType;
  }

  inline const IArrayList<TouchPoint>& getTouchPoints(void) const
  {
    return m_touchPoints;
  }

private:

  Type m_touchType;

  ArrayList<TouchPoint, MAX_NUM_OF_SIMULTANEOUS_TOUCH_POINTS> m_touchPoints;
};

#endif // #ifndef TOUCH_EVENT_H