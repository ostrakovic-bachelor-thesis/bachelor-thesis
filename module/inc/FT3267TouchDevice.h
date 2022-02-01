#ifndef FT3267_TOUCH_DEVICE_H
#define FT3267_TOUCH_DEVICE_H

#include "ITouchEventListener.h"
#include "FT3267.h"


class FT3267TouchDevice
{
public:
  FT3267TouchDevice(FT3267 &ft3267);

  //! This enum class represents errors which can happen during method calls
  enum class ErrorCode : uint8_t
  {
    OK = 0u,
  };

  ErrorCode init(void);

  ErrorCode registerTouchEventListener(ITouchEventListener *touchEventListenerPtr);

  ErrorCode unregisterTouchEventListener(void);

  ITouchEventListener* getTouchEventListener(void);

  static void touchEventCallback(void *ft3267TouchDevicePtr, FT3267::TouchEventInfo touchEventInfo);

private:

  FT3267 &m_ft3267;

  ITouchEventListener *m_touchEventListenerPtr = nullptr;
};

#endif // #ifndef FT3267_TOUCH_DEVICE_H