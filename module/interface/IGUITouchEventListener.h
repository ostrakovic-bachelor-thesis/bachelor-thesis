#ifndef I_GUI_TOUCH_EVENT_LISTENER
#define I_GUI_TOUCH_EVENT_LISTENER

#include "GUITouchEvent.h"


namespace GUI
{
  class ITouchEventListener
  {
  public:
    virtual ~ITouchEventListener() = default;

    virtual void notify(const TouchEvent &touchEvent) = 0;
  };
}

#endif // #ifndef I_GUI_TOUCH_EVENT_LISTENER