#include "GUITouchController.h"


void GUI::TouchController::notify(const TouchEvent &touchEvent)
{
  if (nullptr != m_containerPtr)
  {
    if (TouchEvent::Type::TOUCH_START == touchEvent.getType())
    {
      m_eventTargetObjectPtr = m_containerPtr->getEventTarget(touchEvent);
    }

    if (nullptr != m_eventTargetObjectPtr)
    {
      TouchEvent event(
        touchEvent.getId(),
        touchEvent.getType(),
        touchEvent.getTouchPoints(),
        m_eventTargetObjectPtr);

      m_containerPtr->dispatchEvent(event);
    }
  }
}
