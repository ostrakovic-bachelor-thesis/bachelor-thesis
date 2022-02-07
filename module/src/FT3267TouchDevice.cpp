#include "FT3267TouchDevice.h"


FT3267TouchDevice::FT3267TouchDevice(FT3267 &ft3267):
  m_nextTouchEventId(0u),
  m_ft3267(ft3267),
  m_lastTouchEvent(getNextTouchEventId(), GUI::TouchEvent::Type::TOUCH_STOP)
{}

FT3267TouchDevice::ErrorCode FT3267TouchDevice::init(void)
{
  m_ft3267.registerTouchEventCallback(touchEventCallback, this);

  return ErrorCode::OK;
}

FT3267TouchDevice::ErrorCode
FT3267TouchDevice::registerTouchEventListener(ITouchEventListener *touchEventListenerPtr)
{
  m_touchEventListenerPtr = touchEventListenerPtr;

  return ErrorCode::OK;
}

FT3267TouchDevice::ErrorCode FT3267TouchDevice::unregisterTouchEventListener(void)
{
  m_touchEventListenerPtr = nullptr;

  return ErrorCode::OK;
}

ITouchEventListener* FT3267TouchDevice::getTouchEventListener(void)
{
  return m_touchEventListenerPtr;
}

uint64_t FT3267TouchDevice::getNextTouchEventId(void)
{
  return (m_nextTouchEventId++);
}

GUI::TouchEvent::Type FT3267TouchDevice::getTypeOfNextTouchEvent(const FT3267::TouchEventInfo &touchEventInfo) const
{
  GUI::TouchEvent::Type touchEventType;

  switch (m_lastTouchEvent.getType())
  {
    case GUI::TouchEvent::Type::TOUCH_STOP:
    {
      touchEventType = GUI::TouchEvent::Type::TOUCH_START;
    }
    break;

    case GUI::TouchEvent::Type::TOUCH_START:
    case GUI::TouchEvent::Type::TOUCH_MOVE:
    {
      touchEventType = GUI::TouchEvent::Type::TOUCH_MOVE;
    }
    break;

    default:
    {
      touchEventType = GUI::TouchEvent::Type::TOUCH_STOP;
    }
    break;
  }

  // if touch count is zero, generate TOUCH STOP event, no matter which event was generated previously
  if (0u == touchEventInfo.touchCount)
  {
    touchEventType = GUI::TouchEvent::Type::TOUCH_STOP;
  }

  return touchEventType;
}

void FT3267TouchDevice::notifyTouchEventListenerIfRegistered(const GUI::TouchEvent &touchEvent)
{
  if (nullptr != m_touchEventListenerPtr)
  {
    m_touchEventListenerPtr->notify(touchEvent);
  }
}

bool FT3267TouchDevice::shouldEventListenerDeviceBeNotified(GUI::TouchEvent::Type touchEventType) const
{
  return (GUI::TouchEvent::Type::TOUCH_STOP != touchEventType) ||
         (GUI::TouchEvent::Type::TOUCH_STOP != m_lastTouchEvent.getType());
}

GUI::TouchEvent FT3267TouchDevice::getTouchEvent(
  uint64_t touchEventId,
  GUI::TouchEvent::Type touchEventType,
  const FT3267::TouchEventInfo &touchEventInfo)
{
  ArrayList<GUI::Point, 2u> touchPoints;

  for (uint8_t i = 0u; i < touchEventInfo.touchCount; ++i)
  {
    GUI::Point touchPoint =
    {
      .x = static_cast<int16_t>(touchEventInfo.touchPoints[i].position.x),
      .y = static_cast<int16_t>(touchEventInfo.touchPoints[i].position.y)
    };
    touchPoints.addElement(touchPoint);
  }

  return GUI::TouchEvent(touchEventId, touchEventType, touchPoints);
}

void FT3267TouchDevice::touchEventCallback(void *ft3267TouchDevicePtr, FT3267::TouchEventInfo touchEventInfo)
{
  FT3267TouchDevice *touchDevicePtr = reinterpret_cast<FT3267TouchDevice*>(ft3267TouchDevicePtr);

  if (nullptr != touchDevicePtr)
  {
    const uint64_t touchEventId = touchDevicePtr->getNextTouchEventId();
    const GUI::TouchEvent::Type touchEventType = touchDevicePtr->getTypeOfNextTouchEvent(touchEventInfo);

    GUI::TouchEvent touchEvent = getTouchEvent(touchEventId, touchEventType, touchEventInfo);
    if (GUI::TouchEvent::Type::TOUCH_STOP == touchEventType)
    {
      touchEvent = GUI::TouchEvent(touchEventId, touchEventType, touchDevicePtr->m_lastTouchEvent.getTouchPoints());
    }

    if (touchDevicePtr->shouldEventListenerDeviceBeNotified(touchEventType))
    {
      touchDevicePtr->notifyTouchEventListenerIfRegistered(touchEvent);
    }

    touchDevicePtr->m_lastTouchEvent = touchEvent;
  }
}