#include "GUITouchController.h"


void GUI::TouchController::notify(const TouchEvent &touchEvent)
{
  if (nullptr != m_containerPtr)
  {
    m_containerPtr->getEventTarget(touchEvent);
  }
}
