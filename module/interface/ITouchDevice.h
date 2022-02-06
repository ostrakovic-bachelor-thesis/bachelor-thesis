#ifndef I_TOUCH_DEVICE_H
#define I_TOUCH_DEVICE_H

#include "ITouchEventListener.h"


class ITouchDevice
{
public:
  virtual ~ITouchDevice() = default;

  //! This enum class represents errors which can happen during method calls
  enum class ErrorCode : uint8_t
  {
    OK = 0u,
  };

  virtual ErrorCode registerTouchEventListener(ITouchEventListener *touchEventListenerPtr) = 0;

  virtual ErrorCode unregisterTouchEventListener(void) = 0;

  virtual ITouchEventListener* getTouchEventListener(void) = 0;
};

#endif // #ifndef I_TOUCH_DEVICE_H