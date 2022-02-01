#ifndef I_TOUCH_EVENT_LISTENER
#define I_TOUCH_EVENT_LISTENER

class ITouchEventListener
{
public:
  virtual ~ITouchEventListener() = default;

  virtual void notify(void) = 0;
};

#endif // #ifndef I_TOUCH_EVENT_LISTENER