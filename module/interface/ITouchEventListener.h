#ifndef I_TOUCH_EVENT_LISTENER
#define I_TOUCH_EVENT_LISTENER

#include "GUITouchEvent.h"


class ITouchEventListener
{
public:
  virtual ~ITouchEventListener() = default;

  virtual void notify(const GUI::TouchEvent &touchEvent) = 0;
};

#endif // #ifndef I_TOUCH_EVENT_LISTENER