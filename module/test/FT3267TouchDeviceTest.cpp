#include "FT3267TouchDevice.h"
#include "FT3267Mock.h"
#include "GUITouchEventListenerMock.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


class AFT3267TouchDevice : public Test
{
public:

  NiceMock<GUITouchEventListenerMock> guiTouchEventListenerMock;
  NiceMock<FT3267Mock> ft3267Mock;
  FT3267::TouchEventInfo touchEventInfo;
  FT3267TouchDevice ft3267TouchDevice = FT3267TouchDevice(ft3267Mock);

  constexpr static FT3267::TouchPosition EXPECTED_TOUCH_POSITION =
  {
    .x = 15,
    .y = 130
  };

  constexpr static FT3267::TouchPosition EXPECTED_TOUCH_POSITION_1 =
  {
    .x = 120,
    .y = 10
  };

  constexpr static FT3267::TouchPosition EXPECTED_TOUCH_POSITION_2 =
  {
    .x = 0,
    .y = 300
  };

  bool m_isCalledForFirstTime = true;
  uint64_t m_lastTouchEventId = 0u;

  void expectThatTouchStartEventWillBeGenerated(GUITouchEventListenerMock &guiTouchEventListenerMock);
  void expectThatNTouchMoveEventsWillBeGenerated(GUITouchEventListenerMock &guiTouchEventListenerMock, uint32_t count);
  void expectThatTouchStopEventWillBeGenerated(GUITouchEventListenerMock &guiTouchEventListenerMock);
  void expectThatNoEventWillBeGenerated(GUITouchEventListenerMock &guiTouchEventListenerMock);
  void expectThatTouchEventWillBeGeneratedWithGivenTouchPosition(FT3267::TouchPosition touchPosition);
  void expectThatTouchEventWillBeGeneratedWithGivenTouchPositions(
    FT3267::TouchPosition touchPosition1,
    FT3267::TouchPosition touchPosition2);
  void expectThatTouchEventIdWillBeIncrementedByOneBetweenAdjacentTouchEvents(void);
};

constexpr FT3267::TouchPosition AFT3267TouchDevice::EXPECTED_TOUCH_POSITION;
constexpr FT3267::TouchPosition AFT3267TouchDevice::EXPECTED_TOUCH_POSITION_1;
constexpr FT3267::TouchPosition AFT3267TouchDevice::EXPECTED_TOUCH_POSITION_2;

void AFT3267TouchDevice::expectThatTouchStartEventWillBeGenerated(GUITouchEventListenerMock &guiTouchEventListenerMock)
{
  EXPECT_CALL(guiTouchEventListenerMock, notify(_))
    .Times(1)
    .WillOnce([&](const GUI::TouchEvent &touchEvent)
    {
      ASSERT_THAT(touchEvent.getType(), Eq(GUI::TouchEvent::Type::TOUCH_START));
    });
}

void AFT3267TouchDevice::expectThatNTouchMoveEventsWillBeGenerated(
  GUITouchEventListenerMock &guiTouchEventListenerMock,
  uint32_t count)
{
  EXPECT_CALL(guiTouchEventListenerMock, notify(_))
    .Times(count)
    .WillRepeatedly([&](const GUI::TouchEvent &touchEvent)
    {
      ASSERT_THAT(touchEvent.getType(), Eq(GUI::TouchEvent::Type::TOUCH_MOVE));
    });
}

void AFT3267TouchDevice::expectThatTouchStopEventWillBeGenerated(GUITouchEventListenerMock &guiTouchEventListenerMock)
{
  EXPECT_CALL(guiTouchEventListenerMock, notify(_))
    .Times(1u)
    .WillOnce([&](const GUI::TouchEvent &touchEvent)
    {
      ASSERT_THAT(touchEvent.getType(), Eq(GUI::TouchEvent::Type::TOUCH_STOP));
    });
}

void AFT3267TouchDevice::expectThatNoEventWillBeGenerated(GUITouchEventListenerMock &guiTouchEventListenerMock)
{
  EXPECT_CALL(guiTouchEventListenerMock, notify(_))
    .Times(0u);
}

void AFT3267TouchDevice::expectThatTouchEventWillBeGeneratedWithGivenTouchPosition(FT3267::TouchPosition touchPosition)
{
  EXPECT_CALL(guiTouchEventListenerMock, notify(_))
    .Times(1u)
    .WillOnce([&](const GUI::TouchEvent &touchEvent)
    {
      const IArrayList<GUI::Point> &touchPoints = touchEvent.getTouchPoints();

      GUI::Point touchPoint;
      touchPoints.getElement(0u, &touchPoint);

      ASSERT_THAT(touchPoints.getSize(), Eq(1u));
      ASSERT_THAT(touchPoint.x, Eq(touchPosition.x));
      ASSERT_THAT(touchPoint.y, Eq(touchPosition.y));
    });
}

void AFT3267TouchDevice::expectThatTouchEventWillBeGeneratedWithGivenTouchPositions(
  FT3267::TouchPosition touchPosition1,
  FT3267::TouchPosition touchPosition2)
{
  EXPECT_CALL(guiTouchEventListenerMock, notify(_))
    .Times(1u)
    .WillOnce([&](const GUI::TouchEvent &touchEvent)
    {
      const IArrayList<GUI::Point> &touchPoints = touchEvent.getTouchPoints();

      GUI::Point touchPoint1;
      GUI::Point touchPoint2;
      touchPoints.getElement(0u, &touchPoint1);
      touchPoints.getElement(1u, &touchPoint2);

      ASSERT_THAT(touchPoints.getSize(), Eq(2u));
      ASSERT_THAT(touchPoint1.x , Eq(touchPosition1.x));
      ASSERT_THAT(touchPoint1.y , Eq(touchPosition1.y));
      ASSERT_THAT(touchPoint2.x , Eq(touchPosition2.x));
      ASSERT_THAT(touchPoint2.y , Eq(touchPosition2.y));
    });
}

void AFT3267TouchDevice::expectThatTouchEventIdWillBeIncrementedByOneBetweenAdjacentTouchEvents(void)
{
  ON_CALL(guiTouchEventListenerMock, notify(_))
    .WillByDefault([&](const GUI::TouchEvent &touchEvent)
    {
      if (m_isCalledForFirstTime)
      {
        m_isCalledForFirstTime = false;
      }
      else
      {
        ASSERT_THAT(touchEvent.getId(), Eq(m_lastTouchEventId + 1u));
      }

      m_lastTouchEventId = touchEvent.getId();
    });
}


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

  GUI::ErrorCode errorCode = ft3267TouchDevice.init();

  ASSERT_THAT(errorCode, Eq(GUI::ErrorCode::OK));
}

TEST_F(AFT3267TouchDevice, GetTouchEventListenerReturnsNullPointerIfTouchEventListenerIsNotRegistered)
{
  ft3267TouchDevice.init();

  ASSERT_THAT(ft3267TouchDevice.getTouchEventListener(), Eq(nullptr));
}

TEST_F(AFT3267TouchDevice, GetTouchEventListenerReturnsPointerToRegisteredTouchEventListener)
{
  ft3267TouchDevice.init();

  GUI::ErrorCode errorCode = ft3267TouchDevice.registerTouchEventListener(&guiTouchEventListenerMock);

  ASSERT_THAT(ft3267TouchDevice.getTouchEventListener(), Eq(&guiTouchEventListenerMock));
  ASSERT_THAT(errorCode, Eq(GUI::ErrorCode::OK));
}

TEST_F(AFT3267TouchDevice, GetTouchEventListenerReturnsNullPointerAfterTouchEventListenerIsUnregistered)
{
  ft3267TouchDevice.init();
  ft3267TouchDevice.registerTouchEventListener(&guiTouchEventListenerMock);

  GUI::ErrorCode errorCode = ft3267TouchDevice.unregisterTouchEventListener();

  ASSERT_THAT(ft3267TouchDevice.getTouchEventListener(), Eq(nullptr));
  ASSERT_THAT(errorCode, Eq(GUI::ErrorCode::OK));
}

TEST_F(AFT3267TouchDevice, TouchEventCallbackDoesNotCauseFaultIfTouchEvenListenerIsNotRegistered)
{
  ft3267TouchDevice.init();

  FT3267TouchDevice::touchEventCallback(&ft3267TouchDevice, touchEventInfo);

  SUCCEED();
}

TEST_F(AFT3267TouchDevice, WhenCalledTouchEventCallbackNotifiesRegisteredTouchEventListenerAboutTouchEvent)
{
  ft3267TouchDevice.init();
  ft3267TouchDevice.registerTouchEventListener(&guiTouchEventListenerMock);
  EXPECT_CALL(guiTouchEventListenerMock, notify(_))
    .Times(1);

  touchEventInfo.touchCount = 2u;
  FT3267TouchDevice::touchEventCallback(&ft3267TouchDevice, touchEventInfo);
}

TEST_F(AFT3267TouchDevice, AfterInitWhenTouchEventCallbackIsCalledWithNumberOfTouchesNotEqualToZeroItGeneratesTouchStartEvent)
{
  ft3267TouchDevice.init();
  ft3267TouchDevice.registerTouchEventListener(&guiTouchEventListenerMock);
  expectThatTouchStartEventWillBeGenerated(guiTouchEventListenerMock);

  touchEventInfo.touchCount = 1u;
  FT3267TouchDevice::touchEventCallback(&ft3267TouchDevice, touchEventInfo);
}

TEST_F(AFT3267TouchDevice, AfterInitWhenTouchEventCallbackIsCalledWithNumberOfTouchesEqualToZeroNoEventWillBeGenerated)
{
  ft3267TouchDevice.init();
  ft3267TouchDevice.registerTouchEventListener(&guiTouchEventListenerMock);
  expectThatNoEventWillBeGenerated(guiTouchEventListenerMock);

  touchEventInfo.touchCount = 0u;
  FT3267TouchDevice::touchEventCallback(&ft3267TouchDevice, touchEventInfo);
}

TEST_F(AFT3267TouchDevice, IfTouchStartWasPreviousEventAndTouchEventCallbackIsCalledWithNumberOfTouchesNotEqualToZeroTouchMoveEventIsGenerated)
{
  ft3267TouchDevice.init();
  ft3267TouchDevice.registerTouchEventListener(&guiTouchEventListenerMock);
  // generate TOUCH START event
  touchEventInfo.touchCount = 2u;
  FT3267TouchDevice::touchEventCallback(&ft3267TouchDevice, touchEventInfo);
  expectThatNTouchMoveEventsWillBeGenerated(guiTouchEventListenerMock, 1u);

  touchEventInfo.touchCount = 1u;
  FT3267TouchDevice::touchEventCallback(&ft3267TouchDevice, touchEventInfo);
}

TEST_F(AFT3267TouchDevice, IfTouchMoveWasPreviousEventAndTouchEventCallbackIsCalledWithNumberOfTouchesNotEqualToZeroTouchMoveEventIsGenerated)
{
  ft3267TouchDevice.init();
  ft3267TouchDevice.registerTouchEventListener(&guiTouchEventListenerMock);
  // generate TOUCH START event
  touchEventInfo.touchCount = 1u;
  FT3267TouchDevice::touchEventCallback(&ft3267TouchDevice, touchEventInfo);
  // generate TOUCH MOVE event
  touchEventInfo.touchCount = 2u;
  FT3267TouchDevice::touchEventCallback(&ft3267TouchDevice, touchEventInfo);
  expectThatNTouchMoveEventsWillBeGenerated(guiTouchEventListenerMock, 5u);

  for (uint32_t i = 0u; i < 5u; ++i)
  {
    FT3267TouchDevice::touchEventCallback(&ft3267TouchDevice, touchEventInfo);
  }
}

TEST_F(AFT3267TouchDevice, IfTouchStopWasPreviousEventAndTouchEventCallbackIsCalledWithNumberOfTouchesNotEqualToZeroTouchStartEventIsGenerated)
{
  ft3267TouchDevice.init();
  ft3267TouchDevice.registerTouchEventListener(&guiTouchEventListenerMock);
  // generate TOUCH STOP event
  touchEventInfo.touchCount = 0u;
  FT3267TouchDevice::touchEventCallback(&ft3267TouchDevice, touchEventInfo);
  expectThatTouchStartEventWillBeGenerated(guiTouchEventListenerMock);

  touchEventInfo.touchCount = 1u;
  FT3267TouchDevice::touchEventCallback(&ft3267TouchDevice, touchEventInfo);
}

TEST_F(AFT3267TouchDevice, IfTouchStopWasPreviousEventAndTouchEventCallbackIsCalledWithNumberOfTouchesEqualToZeroNoEventWillBeGenerated)
{
  ft3267TouchDevice.init();
  ft3267TouchDevice.registerTouchEventListener(&guiTouchEventListenerMock);
  // generate TOUCH STOP event
  touchEventInfo.touchCount = 0u;
  FT3267TouchDevice::touchEventCallback(&ft3267TouchDevice, touchEventInfo);
  expectThatNoEventWillBeGenerated(guiTouchEventListenerMock);

  touchEventInfo.touchCount = 0u;
  FT3267TouchDevice::touchEventCallback(&ft3267TouchDevice, touchEventInfo);
}

TEST_F(AFT3267TouchDevice, IfTouchStartWasPreviousEventAndTouchEventCallbackIsCalledWithNumberOfTouchesEqualToZeroTouchStopEventIsGenerated)
{
  ft3267TouchDevice.init();
  ft3267TouchDevice.registerTouchEventListener(&guiTouchEventListenerMock);
  // generate TOUCH START event
  touchEventInfo.touchCount = 1u;
  FT3267TouchDevice::touchEventCallback(&ft3267TouchDevice, touchEventInfo);
  expectThatTouchStopEventWillBeGenerated(guiTouchEventListenerMock);

  touchEventInfo.touchCount = 0u;
  FT3267TouchDevice::touchEventCallback(&ft3267TouchDevice, touchEventInfo);
}

TEST_F(AFT3267TouchDevice, IfTouchMoveWasPreviousEventAndTouchEventCallbackIsCalledWithNumberOfTouchesEqualToZeroTouchStopEventIsGenerated)
{
  ft3267TouchDevice.init();
  ft3267TouchDevice.registerTouchEventListener(&guiTouchEventListenerMock);
  // generate TOUCH START event
  touchEventInfo.touchCount = 2u;
  FT3267TouchDevice::touchEventCallback(&ft3267TouchDevice, touchEventInfo);
  // generate TOUCH MOVE event
  touchEventInfo.touchCount = 1u;
  FT3267TouchDevice::touchEventCallback(&ft3267TouchDevice, touchEventInfo);
  expectThatTouchStopEventWillBeGenerated(guiTouchEventListenerMock);

  touchEventInfo.touchCount = 0u;
  FT3267TouchDevice::touchEventCallback(&ft3267TouchDevice, touchEventInfo);
}

TEST_F(AFT3267TouchDevice, TouchStartEventIsGeneratedWithTheSameTouchPointsForwardedToTouchEventCallback)
{
  ft3267TouchDevice.init();
  ft3267TouchDevice.registerTouchEventListener(&guiTouchEventListenerMock);
  expectThatTouchEventWillBeGeneratedWithGivenTouchPosition(EXPECTED_TOUCH_POSITION);

  touchEventInfo.touchCount              = 1u;
  touchEventInfo.touchPoints[0].position = EXPECTED_TOUCH_POSITION;
  FT3267TouchDevice::touchEventCallback(&ft3267TouchDevice, touchEventInfo);
}

TEST_F(AFT3267TouchDevice, TouchMoveEventIsGeneratedWithTheSameTouchPointsForwardedToTouchEventCallback)
{
  ft3267TouchDevice.init();
  ft3267TouchDevice.registerTouchEventListener(&guiTouchEventListenerMock);
  // generate TOUCH START event
  touchEventInfo.touchCount = 1u;
  FT3267TouchDevice::touchEventCallback(&ft3267TouchDevice, touchEventInfo);
  expectThatTouchEventWillBeGeneratedWithGivenTouchPositions(EXPECTED_TOUCH_POSITION_1, EXPECTED_TOUCH_POSITION_2);

  touchEventInfo.touchCount              = 2u;
  touchEventInfo.touchPoints[0].position = EXPECTED_TOUCH_POSITION_1;
  touchEventInfo.touchPoints[1].position = EXPECTED_TOUCH_POSITION_2;
  FT3267TouchDevice::touchEventCallback(&ft3267TouchDevice, touchEventInfo);
}

TEST_F(AFT3267TouchDevice, TouchStopEventIsGeneratedWithTheSameTouchPointsAsPreviousTouchEvent)
{
  ft3267TouchDevice.init();
  ft3267TouchDevice.registerTouchEventListener(&guiTouchEventListenerMock);
  // generate TOUCH START event
  touchEventInfo.touchCount              = 1u;
  touchEventInfo.touchPoints[0].position = EXPECTED_TOUCH_POSITION;
  FT3267TouchDevice::touchEventCallback(&ft3267TouchDevice, touchEventInfo);
  expectThatTouchEventWillBeGeneratedWithGivenTouchPosition(EXPECTED_TOUCH_POSITION);

  touchEventInfo.touchCount = 0u;
  FT3267TouchDevice::touchEventCallback(&ft3267TouchDevice, touchEventInfo);
}

TEST_F(AFT3267TouchDevice, EachGeneratedTouchEventHasIdForOneBiggerComparedToThePreviouslyGeneratedTouchEvent)
{
  ft3267TouchDevice.init();
  ft3267TouchDevice.registerTouchEventListener(&guiTouchEventListenerMock);
  expectThatTouchEventIdWillBeIncrementedByOneBetweenAdjacentTouchEvents();

  touchEventInfo.touchCount = 1u;
  FT3267TouchDevice::touchEventCallback(&ft3267TouchDevice, touchEventInfo);
  touchEventInfo.touchCount = 2u;
  FT3267TouchDevice::touchEventCallback(&ft3267TouchDevice, touchEventInfo);
  touchEventInfo.touchCount = 1u;
  FT3267TouchDevice::touchEventCallback(&ft3267TouchDevice, touchEventInfo);
}