#include "FT3267TouchDevice.h"
#include "FT3267Mock.h"
#include "TouchEventListenerMock.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


class AFT3267TouchDevice : public Test
{
public:

  TouchEventListenerMock touchEventListenerMock;
  NiceMock<FT3267Mock> ft3267Mock;
  FT3267::TouchEventInfo touchEventInfo;
  FT3267TouchDevice ft3267TouchDevice = FT3267TouchDevice(ft3267Mock);
};

// TODO
// 4) When ft3267TouchEventCallback() is called, all registered ITouchListener are notified about event
// 5) TODO determine mapping between events, from observation

TEST_F(AFT3267TouchDevice, TouchEventCallbackDoesNotCauseFaultIfForwardedFT3267TouchDevicePtrIsNullptr)
{
  FT3267TouchDevice::touchEventCallback(nullptr, touchEventInfo);

  SUCCEED();
}

TEST_F(AFT3267TouchDevice, InitRegistersTouchEventCallbackAsFT3267TouchEventCallback)
{
  EXPECT_CALL(ft3267Mock, registerTouchEventCallback(FT3267TouchDevice::touchEventCallback, &ft3267TouchDevice))
    .Times(1)
    .WillOnce(Return(FT3267::ErrorCode::OK));

  FT3267TouchDevice::ErrorCode errorCode = ft3267TouchDevice.init();

  ASSERT_THAT(errorCode, Eq(FT3267TouchDevice::ErrorCode::OK));
}

TEST_F(AFT3267TouchDevice, GetTouchEventListenerReturnsNullPointerIfTouchEventListenerIsNotRegistered)
{
  ft3267TouchDevice.init();

  ASSERT_THAT(ft3267TouchDevice.getTouchEventListener(), Eq(nullptr));
}

TEST_F(AFT3267TouchDevice, GetTouchEventListenerReturnsPointerToRegisteredTouchEventListener)
{
  ft3267TouchDevice.init();

  FT3267TouchDevice::ErrorCode errorCode = ft3267TouchDevice.registerTouchEventListener(&touchEventListenerMock);

  ASSERT_THAT(ft3267TouchDevice.getTouchEventListener(), Eq(&touchEventListenerMock));
  ASSERT_THAT(errorCode, Eq(FT3267TouchDevice::ErrorCode::OK));
}

TEST_F(AFT3267TouchDevice, GetTouchEventListenerReturnsNullPointerAfterTouchEventListenerIsUnregistered)
{
  ft3267TouchDevice.init();
  ft3267TouchDevice.registerTouchEventListener(&touchEventListenerMock);

  FT3267TouchDevice::ErrorCode errorCode = ft3267TouchDevice.unregisterTouchEventListener();

  ASSERT_THAT(ft3267TouchDevice.getTouchEventListener(), Eq(nullptr));
  ASSERT_THAT(errorCode, Eq(FT3267TouchDevice::ErrorCode::OK));
}

TEST_F(AFT3267TouchDevice, WhenCalledTouchEventCallbackNotifiesRegisteredTouchEventListenerAboutTouchEvent)
{
  ft3267TouchDevice.init();
  ft3267TouchDevice.registerTouchEventListener(&touchEventListenerMock);
  EXPECT_CALL(touchEventListenerMock, notify())
    .Times(1);

  FT3267TouchDevice::touchEventCallback(&ft3267TouchDevice, touchEventInfo);
}