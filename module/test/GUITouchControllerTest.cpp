#include "GUITouchController.h"
#include "GUIContainerMock.h"
#include "GUIObjectMock.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


class AGUITouchController : public Test
{
public:

  GUI::TouchEvent RANDOM_TOUCH_EVENT  = GUI::TouchEvent(0u, GUI::TouchEvent::Type::TOUCH_MOVE);
  GUI::TouchEvent START_TOUCH_EVENT   = GUI::TouchEvent(1u, GUI::TouchEvent::Type::TOUCH_START);
  GUI::TouchEvent MOVE_TOUCH_EVENT    = GUI::TouchEvent(2u, GUI::TouchEvent::Type::TOUCH_MOVE);
  GUI::TouchEvent STOP_TOUCH_EVENT    = GUI::TouchEvent(3u, GUI::TouchEvent::Type::TOUCH_STOP);
  GUI::TouchEvent START_TOUCH_EVENT_1 = GUI::TouchEvent(4u, GUI::TouchEvent::Type::TOUCH_START);
  GUI::TouchEvent START_TOUCH_EVENT_2 = GUI::TouchEvent(5u, GUI::TouchEvent::Type::TOUCH_START);
  NiceMock<GUIContainerMock> guiContainerMock;
  GUI::TouchController guiTouchController = GUI::TouchController();

  GUIObjectMock guiObjectMockStartup;
  GUIObjectMock guiObjectMock;
  GUIObjectMock guiObjectMock1;
  GUIObjectMock guiObjectMock2;

  void expectThatDispatchEventWillBeCalledWithEventObjectWithGivenTarget(GUIObjectMock *guiObjectMockPtr);
  void expectThatDispatchEventWillNotBeCalled(void);
  void onStartTouchEventReturnGivenObjectAsTargetObject(
    GUI::TouchEvent &touchEvent,
    GUIObjectMock *guiObjectMockPtr);
};

void AGUITouchController::expectThatDispatchEventWillBeCalledWithEventObjectWithGivenTarget(GUIObjectMock *guiObjectMockPtr)
{
  EXPECT_CALL(guiContainerMock, dispatchEvent(_))
    .WillOnce([=](GUI::TouchEvent &touchEvent)
    {
      ASSERT_THAT(touchEvent.getEventTargetObject(), Eq(guiObjectMockPtr));
    });
}

void AGUITouchController::expectThatDispatchEventWillNotBeCalled(void)
{
  EXPECT_CALL(guiContainerMock, dispatchEvent(_))
    .Times(0);
}

void AGUITouchController::onStartTouchEventReturnGivenObjectAsTargetObject(
  GUI::TouchEvent &touchEvent,
  GUIObjectMock *guiObjectMockPtr)
{
  ON_CALL(guiContainerMock, getEventTarget(touchEvent))
    .WillByDefault(Return(guiObjectMockPtr));
}


TEST_F(AGUITouchController, GetRegisteredContainerReturnsNullptrIfContainerIsNotRegistered)
{
  ASSERT_THAT(guiTouchController.getRegisteredContainer(), Eq(nullptr));
}

TEST_F(AGUITouchController, GetRegisteredContainerReturnsPointerToRegisteredGUIContainer)
{
  guiTouchController.registerContainer(&guiContainerMock);

  ASSERT_THAT(guiTouchController.getRegisteredContainer(), Eq(&guiContainerMock));
}

TEST_F(AGUITouchController, GetRegisteredContainerReturnsNullptrIfPreviouslyRegisteredContainerIsUnregistered)
{
  guiTouchController.registerContainer(&guiContainerMock);
  guiTouchController.unregisterContainer();

  ASSERT_THAT(guiTouchController.getRegisteredContainer(), Eq(nullptr));
}

TEST_F(AGUITouchController, NotifyDoesNotFailIfGUIContainerIsNotRegistered)
{
  guiTouchController.unregisterContainer();

  guiTouchController.notify(RANDOM_TOUCH_EVENT);

  SUCCEED();
}

TEST_F(AGUITouchController, WhenNotifiedAboutTouchStartEventItCallsGUIContainerGetEventTargetMethod)
{
  guiTouchController.registerContainer(&guiContainerMock);
  EXPECT_CALL(guiContainerMock, getEventTarget(START_TOUCH_EVENT))
    .Times(1u);

  guiTouchController.notify(START_TOUCH_EVENT);
}

TEST_F(AGUITouchController, WhenNotifiedAboutTouchMoveEventItDoesNotCallGUIContainerGetEventTargetMethod)
{
  guiTouchController.registerContainer(&guiContainerMock);
  EXPECT_CALL(guiContainerMock, getEventTarget(_))
    .Times(0u);

  guiTouchController.notify(MOVE_TOUCH_EVENT);
}

TEST_F(AGUITouchController, WhenNotifiedAboutTouchStopEventItDoesNotCallGUIContainerGetEventTargetMethod)
{
  guiTouchController.registerContainer(&guiContainerMock);
  EXPECT_CALL(guiContainerMock, getEventTarget(_))
    .Times(0u);

  guiTouchController.notify(STOP_TOUCH_EVENT);
}

TEST_F(AGUITouchController, WhenNotifiedAboutTouchStartEventItCallsGUIContainerDispatchEventAfterGetEventTargetHasBeenCalled)
{
  guiTouchController.registerContainer(&guiContainerMock);
  onStartTouchEventReturnGivenObjectAsTargetObject(START_TOUCH_EVENT, &guiObjectMock);
  {
    InSequence expectCallsInSequence;

    EXPECT_CALL(guiContainerMock, getEventTarget(_))
      .Times(1u);
    EXPECT_CALL(guiContainerMock, dispatchEvent(_))
      .Times(1u);
  }

  guiTouchController.notify(START_TOUCH_EVENT);
}

TEST_F(AGUITouchController, DispatchEventObjectHasTheSameIdAsTouchEventForwardedToGUIControllerNotify)
{
  guiTouchController.registerContainer(&guiContainerMock);
  onStartTouchEventReturnGivenObjectAsTargetObject(START_TOUCH_EVENT, &guiObjectMock);
  EXPECT_CALL(guiContainerMock, dispatchEvent(_))
    .WillOnce([&](GUI::TouchEvent &touchEvent)
    {
      ASSERT_THAT(touchEvent.getId(), Eq(START_TOUCH_EVENT.getId()));
    });

  guiTouchController.notify(START_TOUCH_EVENT);
}

TEST_F(AGUITouchController, DispatchEventObjectHasTheSameTypeAsTouchEventForwardedToGUIControllerNotify)
{
  guiTouchController.registerContainer(&guiContainerMock);
  onStartTouchEventReturnGivenObjectAsTargetObject(START_TOUCH_EVENT, &guiObjectMock);
  EXPECT_CALL(guiContainerMock, dispatchEvent(_))
    .WillOnce([&](GUI::TouchEvent &touchEvent)
    {
      ASSERT_THAT(touchEvent.getType(), Eq(START_TOUCH_EVENT.getType()));
    });

  guiTouchController.notify(START_TOUCH_EVENT);
}

TEST_F(AGUITouchController, DispatchEventObjectHasTheSameTouchPointsAsTouchEventForwardedToGUIControllerNotify)
{
  guiTouchController.registerContainer(&guiContainerMock);
  onStartTouchEventReturnGivenObjectAsTargetObject(START_TOUCH_EVENT, &guiObjectMock);
  EXPECT_CALL(guiContainerMock, dispatchEvent(_))
    .WillOnce([&](GUI::TouchEvent &touchEvent)
    {
      ASSERT_EQ(touchEvent.getTouchPoints(), START_TOUCH_EVENT.getTouchPoints());
    });

  guiTouchController.notify(START_TOUCH_EVENT);
}

TEST_F(AGUITouchController, DispatchEventObjectOnStartTouchEventHasAsTargetObjectTheOneObtainedViaGetEventTargetMethod)
{
  guiTouchController.registerContainer(&guiContainerMock);
  onStartTouchEventReturnGivenObjectAsTargetObject(START_TOUCH_EVENT, &guiObjectMock);
  expectThatDispatchEventWillBeCalledWithEventObjectWithGivenTarget(&guiObjectMock);

  guiTouchController.notify(START_TOUCH_EVENT);
}

TEST_F(AGUITouchController, DispatchEventObjectOnMoveTouchEventHasAsTargetObjectTheOneObtainedAtTheLastStartTouchEvent)
{
  guiTouchController.registerContainer(&guiContainerMock);
  onStartTouchEventReturnGivenObjectAsTargetObject(START_TOUCH_EVENT_1, &guiObjectMock1);
  onStartTouchEventReturnGivenObjectAsTargetObject(START_TOUCH_EVENT_2, &guiObjectMock2);
  guiTouchController.notify(START_TOUCH_EVENT_1);
  guiTouchController.notify(START_TOUCH_EVENT_2);
  expectThatDispatchEventWillBeCalledWithEventObjectWithGivenTarget(&guiObjectMock2);

  guiTouchController.notify(MOVE_TOUCH_EVENT);
}

TEST_F(AGUITouchController, DispatchEventObjectOnStopTouchEventHasAsTargetObjectTheOneObtainedAtTheLastStartTouchEvent)
{
  guiTouchController.registerContainer(&guiContainerMock);
  onStartTouchEventReturnGivenObjectAsTargetObject(START_TOUCH_EVENT_1, &guiObjectMock1);
  onStartTouchEventReturnGivenObjectAsTargetObject(START_TOUCH_EVENT_2, &guiObjectMock2);
  guiTouchController.notify(START_TOUCH_EVENT_1);
  guiTouchController.notify(START_TOUCH_EVENT_2);
  expectThatDispatchEventWillBeCalledWithEventObjectWithGivenTarget(&guiObjectMock2);

  guiTouchController.notify(STOP_TOUCH_EVENT);
}

TEST_F(AGUITouchController, OnStartTouchEventDispatchEventWillNotBeCalledIfEventTargetIsNotSuccessfullyObtained)
{
  guiTouchController.registerContainer(&guiContainerMock);
  onStartTouchEventReturnGivenObjectAsTargetObject(START_TOUCH_EVENT, nullptr);
  expectThatDispatchEventWillNotBeCalled();

  guiTouchController.notify(START_TOUCH_EVENT);
}

TEST_F(AGUITouchController, OnMoveTouchEventDispatchEventWillNotBeCalledIfEventTargetWasNotSuccessfullyObtainedAtTheLastStartTouchEvent)
{
  guiTouchController.registerContainer(&guiContainerMock);
  onStartTouchEventReturnGivenObjectAsTargetObject(START_TOUCH_EVENT_1, &guiObjectMock1);
  onStartTouchEventReturnGivenObjectAsTargetObject(START_TOUCH_EVENT_2, nullptr);
  guiTouchController.notify(START_TOUCH_EVENT_1);
  guiTouchController.notify(START_TOUCH_EVENT_2);
  expectThatDispatchEventWillNotBeCalled();

  guiTouchController.notify(MOVE_TOUCH_EVENT);
}

TEST_F(AGUITouchController, OnStopTouchEventDispatchEventWillNotBeCalledIfEventTargetWasNotSuccessfullyObtainedAtTheLastStartTouchEvent)
{
  guiTouchController.registerContainer(&guiContainerMock);
  onStartTouchEventReturnGivenObjectAsTargetObject(START_TOUCH_EVENT_1, &guiObjectMock1);
  onStartTouchEventReturnGivenObjectAsTargetObject(START_TOUCH_EVENT_2, nullptr);
  guiTouchController.notify(START_TOUCH_EVENT_1);
  guiTouchController.notify(START_TOUCH_EVENT_2);
  expectThatDispatchEventWillNotBeCalled();

  guiTouchController.notify(STOP_TOUCH_EVENT);
}