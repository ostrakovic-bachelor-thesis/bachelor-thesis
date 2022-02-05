#ifndef I_TOUCH_EVENT_LISTENER
#define I_TOUCH_EVENT_LISTENER

#include "TouchEvent.h"


class ITouchEventListener
{
public:
  virtual ~ITouchEventListener() = default;

  virtual void notify(const TouchEvent &touchEvent) = 0;
};

#endif // #ifndef I_TOUCH_EVENT_LISTENER