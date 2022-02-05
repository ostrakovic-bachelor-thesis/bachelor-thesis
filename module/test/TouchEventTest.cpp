#include "TouchEvent.h"
#include "ArrayList.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


class ATouchEvent : public Test
{
public:

  ArrayList<TouchEvent::TouchPoint, 5u> touchEventTouchPoints;

  const TouchEvent::TouchPoint TOUCH_POINT_1 = { .x = 0,   .y = 110 };
  const TouchEvent::TouchPoint TOUCH_POINT_2 = { .x = -50, .y = 130 };
  const TouchEvent::TouchPoint TOUCH_POINT_3 = { .x = 100, .y = 10 };

  void assertThatTouchesAreEqual(
    const IArrayList<TouchEvent::TouchPoint> &touchEventTouchPoints,
    const IArrayList<TouchEvent::TouchPoint> &forwardedTouchPointsToConstructor);
};


void ATouchEvent::assertThatTouchesAreEqual(
  const IArrayList<TouchEvent::TouchPoint> &touchEventTouchPoints,
  const IArrayList<TouchEvent::TouchPoint> &forwardedTouchPointsToConstructor)
{
  ASSERT_THAT(touchEventTouchPoints.getSize(), Le(forwardedTouchPointsToConstructor.getSize()));

  auto it1 = touchEventTouchPoints.getBeginIterator();
  auto it2 = forwardedTouchPointsToConstructor.getBeginIterator();
  while (it1 != touchEventTouchPoints.getEndIterator())
  {
    ASSERT_THAT(it1->x,     Eq(it2->x));
    ASSERT_THAT((it1++)->y, Eq((it2++)->y));
  }
}


TEST_F(ATouchEvent, GetTypeReturnsTouchEventTypeSpecifiedAtConstructionTime)
{
  constexpr TouchEvent::Type EXPECTED_TOUCH_EVENT = TouchEvent::Type::TOUCH_MOVE;
  TouchEvent touchEvent(EXPECTED_TOUCH_EVENT, touchEventTouchPoints);

  ASSERT_THAT(touchEvent.getType(), Eq(EXPECTED_TOUCH_EVENT));
}

TEST_F(ATouchEvent, GetTouchPointsGetsReferenceToEmptyListOfTouchPointsIfEmptyListIsGivenAtConstructionTime)
{
  TouchEvent touchEvent(TouchEvent::Type::TOUCH_STOP, touchEventTouchPoints);

  const IArrayList<TouchEvent::TouchPoint> &touchPoints = touchEvent.getTouchPoints();

  ASSERT_THAT(touchPoints.isEmpty(),           Eq(true));
  ASSERT_THAT(touchEventTouchPoints.isEmpty(), Eq(true));
}

TEST_F(ATouchEvent, GetTouchPointsGetsReferenceToNonEmptyListOfTouchPointsIfNonEmptyListIsGivenAtConstructionTime)
{
  touchEventTouchPoints.addElement(TOUCH_POINT_1);
  TouchEvent touchEvent(TouchEvent::Type::TOUCH_START, touchEventTouchPoints);

  const IArrayList<TouchEvent::TouchPoint> &touchPoints = touchEvent.getTouchPoints();

  ASSERT_THAT(touchPoints.isEmpty(),           Eq(false));
  ASSERT_THAT(touchEventTouchPoints.isEmpty(), Eq(false));
}

TEST_F(ATouchEvent, TouchEventTouchPointsGetByGetTouchPointsMethodCorrespondToTouchPointsGivenAtConstructionTime)
{
  touchEventTouchPoints.addElement(TOUCH_POINT_1);
  touchEventTouchPoints.addElement(TOUCH_POINT_2);
  TouchEvent touchEvent(TouchEvent::Type::TOUCH_MOVE, touchEventTouchPoints);

  const IArrayList<TouchEvent::TouchPoint> &touchPoints = touchEvent.getTouchPoints();

  assertThatTouchesAreEqual(touchPoints, touchEventTouchPoints);
}

TEST_F(ATouchEvent, CanStoreMaximallyTwoTouchPointsSimultaneously)
{
  touchEventTouchPoints.addElement(TOUCH_POINT_1);
  touchEventTouchPoints.addElement(TOUCH_POINT_2);
  touchEventTouchPoints.addElement(TOUCH_POINT_3);
  TouchEvent touchEvent(TouchEvent::Type::TOUCH_START, touchEventTouchPoints);

  const IArrayList<TouchEvent::TouchPoint> &touchPoints = touchEvent.getTouchPoints();

  ASSERT_THAT(touchPoints.getSize(),           Eq(2u));
  ASSERT_THAT(touchEventTouchPoints.getSize(), Eq(3u));
  assertThatTouchesAreEqual(touchPoints, touchEventTouchPoints);
}

TEST_F(ATouchEvent, GetTouchPointsGetsReferenceToEmptyListOfTouchPointsIfConstructorWithoutTouchPointsListIsCalled)
{
  TouchEvent touchEvent(TouchEvent::Type::TOUCH_START);

  const IArrayList<TouchEvent::TouchPoint> &touchPoints = touchEvent.getTouchPoints();

  ASSERT_THAT(touchPoints.isEmpty(), Eq(true));
}