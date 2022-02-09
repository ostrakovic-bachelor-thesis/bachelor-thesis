#ifndef I_GUI_TOUCH_DEVICE_H
#define I_GUI_TOUCH_DEVICE_H

#include "IGUITouchEventListener.h"


namespace GUI
{
  class ITouchDevice
  {
  public:
    virtual ~ITouchDevice() = default;

    virtual ErrorCode registerTouchEventListener(ITouchEventListener *touchEventListenerPtr) = 0;

    virtual ErrorCode unregisterTouchEventListener(void) = 0;

    virtual ITouchEventListener* getTouchEventListener(void) = 0;
  };
}

#endif // #ifndef I_GUI_TOUCH_DEVICE_H