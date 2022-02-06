#ifndef TOUCH_EVENT_H
#define TOUCH_EVENT_H

#include "GUICommon.h"
#include "IArrayList.h"
#include "ArrayList.h"
#include <cstdint>


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

  TouchEvent(uint64_t id, Type type);

  TouchEvent(uint64_t id, Type type, const IArrayList<GUI::Point> &touchPoints);

  inline uint64_t getId(void) const
  {
    return m_id;
  }

  inline Type getType(void) const
  {
    return m_type;
  }

  inline const IArrayList<GUI::Point>& getTouchPoints(void) const
  {
    return m_touchPoints;
  }

  bool operator==(const TouchEvent &touchEvent) const;
  bool operator!=(const TouchEvent &touchEvent) const;

private:

  uint64_t m_id;

  Type m_type;

  ArrayList<GUI::Point, MAX_NUM_OF_SIMULTANEOUS_TOUCH_POINTS> m_touchPoints;
};

#endif // #ifndef TOUCH_EVENT_H