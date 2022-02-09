#ifndef GUI_TOUCH_CONTROLLER_H
#define GUI_TOUCH_CONTROLLER_H

#include "IGUITouchEventListener.h"
#include "IGUIContainer.h"


namespace GUI
{
  class TouchController : public ITouchEventListener
  {
  public:

    inline IContainer* getRegisteredContainer(void)
    {
      return m_containerPtr;
    }

    inline void registerContainer(IContainer *containerPtr)
    {
      m_containerPtr = containerPtr;
    }

    inline void unregisterContainer(void)
    {
      m_containerPtr = nullptr;
    }

    void notify(const TouchEvent &touchEvent) override;

  private:

    IContainer *m_containerPtr = nullptr;
  };
}

#endif // #ifndef GUI_TOUCH_CONTROLLER_H