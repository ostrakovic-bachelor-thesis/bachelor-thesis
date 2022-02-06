#ifndef FT3267_TOUCH_DEVICE_H
#define FT3267_TOUCH_DEVICE_H

#include "ITouchDevice.h"
#include "FT3267.h"


class FT3267TouchDevice : public ITouchDevice
{
public:
  FT3267TouchDevice(FT3267 &ft3267);

  ErrorCode init(void);

  ErrorCode registerTouchEventListener(ITouchEventListener *touchEventListenerPtr) override;

  ErrorCode unregisterTouchEventListener(void) override;

  ITouchEventListener* getTouchEventListener(void) override;

  static void touchEventCallback(void *ft3267TouchDevicePtr, FT3267::TouchEventInfo touchEventInfo);

private:

  uint64_t getNextTouchEventId(void);

  TouchEvent::Type getTypeOfNextTouchEvent(const FT3267::TouchEventInfo &touchEventInfo) const;
  void notifyTouchEventListenerIfRegistered(const TouchEvent &touchEvent);
  bool shouldEventListenerDeviceBeNotified(TouchEvent::Type touchEventType) const;

  static TouchEvent getTouchEvent(
    uint64_t touchEventId,
    TouchEvent::Type touchEventType,
    const FT3267::TouchEventInfo &touchEventInfo);

  uint64_t m_nextTouchEventId;

  FT3267 &m_ft3267;

  TouchEvent m_lastTouchEvent;

  ITouchEventListener *m_touchEventListenerPtr = nullptr;
};

#endif // #ifndef FT3267_TOUCH_DEVICE_H