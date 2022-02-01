#include "FT3267TouchDevice.h"


FT3267TouchDevice::FT3267TouchDevice(FT3267 &ft3267):
  m_ft3267(ft3267)
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

void FT3267TouchDevice::touchEventCallback(void *ft3267TouchDevicePtr, FT3267::TouchEventInfo touchEventInfo)
{

}