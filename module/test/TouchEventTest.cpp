#include "TouchEvent.h"
#include "ArrayList.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


class ATouchEvent : public Test
{
public:

  ArrayList<GUI::Point, 5u> touchEventTouchPoints;

  static constexpr uint64_t RANDOM_TOUCH_EVENT_ID = 0u;
  static constexpr TouchEvent::Type RANDOM_TOUCH_EVENT_TYPE = TouchEvent::Type::TOUCH_MOVE;

  const GUI::Point TOUCH_POINT_1 = { .x = 0,   .y = 110 };
  const GUI::Point TOUCH_POINT_2 = { .x = -50, .y = 130 };
  const GUI::Point TOUCH_POINT_3 = { .x = 100, .y = 10 };

  void assertThatTouchPointsAreEqual(
    const IArrayList<GUI::Point> &touchEventTouchPoints,
    const IArrayList<GUI::Point> &forwardedTouchPointsToConstructor);
};


void ATouchEvent::assertThatTouchPointsAreEqual(
  const IArrayList<GUI::Point> &touchEventTouchPoints,
  const IArrayList<GUI::Point> &forwardedTouchPointsToConstructor)
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


TEST_F(ATouchEvent, GetIdReturnsTouchEventIdentifierSpecifiedAtConstructionTime)
{
  constexpr uint64_t EXPECTED_TOUCH_EVENT_ID = 8391506u;
  TouchEvent touchEvent(EXPECTED_TOUCH_EVENT_ID, RANDOM_TOUCH_EVENT_TYPE, touchEventTouchPoints);

  ASSERT_THAT(touchEvent.getId(), Eq(EXPECTED_TOUCH_EVENT_ID));
}

TEST_F(ATouchEvent, GetTypeReturnsTouchEventTypeSpecifiedAtConstructionTime)
{
  constexpr TouchEvent::Type EXPECTED_TOUCH_EVENT_TYPE = TouchEvent::Type::TOUCH_MOVE;
  TouchEvent touchEvent(RANDOM_TOUCH_EVENT_ID, EXPECTED_TOUCH_EVENT_TYPE, touchEventTouchPoints);

  ASSERT_THAT(touchEvent.getType(), Eq(EXPECTED_TOUCH_EVENT_TYPE));
}

TEST_F(ATouchEvent, GetTouchPointsGetsReferenceToEmptyListOfTouchPointsIfEmptyListIsGivenAtConstructionTime)
{
  TouchEvent touchEvent(RANDOM_TOUCH_EVENT_ID, RANDOM_TOUCH_EVENT_TYPE, touchEventTouchPoints);

  const IArrayList<GUI::Point> &touchPoints = touchEvent.getTouchPoints();

  ASSERT_THAT(touchPoints.isEmpty(),           Eq(true));
  ASSERT_THAT(touchEventTouchPoints.isEmpty(), Eq(true));
}

TEST_F(ATouchEvent, GetTouchPointsGetsReferenceToNonEmptyListOfTouchPointsIfNonEmptyListIsGivenAtConstructionTime)
{
  touchEventTouchPoints.addElement(TOUCH_POINT_1);
  TouchEvent touchEvent(RANDOM_TOUCH_EVENT_ID, RANDOM_TOUCH_EVENT_TYPE, touchEventTouchPoints);

  const IArrayList<GUI::Point> &touchPoints = touchEvent.getTouchPoints();

  ASSERT_THAT(touchPoints.isEmpty(),           Eq(false));
  ASSERT_THAT(touchEventTouchPoints.isEmpty(), Eq(false));
}

TEST_F(ATouchEvent, TouchEventTouchPointsGetByGetTouchPointsMethodCorrespondToTouchPointsGivenAtConstructionTime)
{
  touchEventTouchPoints.addElement(TOUCH_POINT_1);
  touchEventTouchPoints.addElement(TOUCH_POINT_2);
  TouchEvent touchEvent(RANDOM_TOUCH_EVENT_ID, RANDOM_TOUCH_EVENT_TYPE, touchEventTouchPoints);

  const IArrayList<GUI::Point> &touchPoints = touchEvent.getTouchPoints();

  assertThatTouchPointsAreEqual(touchPoints, touchEventTouchPoints);
}

TEST_F(ATouchEvent, CanStoreMaximallyTwoTouchPointsSimultaneously)
{
  touchEventTouchPoints.addElement(TOUCH_POINT_1);
  touchEventTouchPoints.addElement(TOUCH_POINT_2);
  touchEventTouchPoints.addElement(TOUCH_POINT_3);
  TouchEvent touchEvent(RANDOM_TOUCH_EVENT_ID, RANDOM_TOUCH_EVENT_TYPE, touchEventTouchPoints);

  const IArrayList<GUI::Point> &touchPoints = touchEvent.getTouchPoints();

  ASSERT_THAT(touchPoints.getSize(),           Eq(2u));
  ASSERT_THAT(touchEventTouchPoints.getSize(), Eq(3u));
  assertThatTouchPointsAreEqual(touchPoints, touchEventTouchPoints);
}

TEST_F(ATouchEvent, GetTouchPointsGetsReferenceToEmptyListOfTouchPointsIfConstructorWithoutTouchPointsListIsCalled)
{
  TouchEvent touchEvent(RANDOM_TOUCH_EVENT_ID, RANDOM_TOUCH_EVENT_TYPE);

  const IArrayList<GUI::Point> &touchPoints = touchEvent.getTouchPoints();

  ASSERT_THAT(touchPoints.isEmpty(), Eq(true));
}

TEST_F(ATouchEvent, IsEqualToAnotherTouchEventOnlyIfTheirIdentifiersAndTypesAndTouchPointsAreTheSame)
{
  touchEventTouchPoints.addElement(TOUCH_POINT_1);
  const TouchEvent touchEvent1(50u, TouchEvent::Type::TOUCH_MOVE, touchEventTouchPoints);
  const TouchEvent touchEvent2(50u, TouchEvent::Type::TOUCH_MOVE, touchEventTouchPoints);

  ASSERT_THAT(touchEvent1.getId(),        Eq(touchEvent2.getId()));
  ASSERT_THAT(touchEvent1.getType(),      Eq(touchEvent2.getType()));
  ASSERT_EQ(touchEvent1.getTouchPoints(), touchEvent2.getTouchPoints());
  ASSERT_THAT(touchEvent1,                Eq(touchEvent2));
}

TEST_F(ATouchEvent, IsNotEqualToAnotherTouchEventIfTheirIdentifiersAreNotTheSame)
{
  touchEventTouchPoints.addElement(TOUCH_POINT_1);
  const TouchEvent touchEvent1(20u, TouchEvent::Type::TOUCH_STOP, touchEventTouchPoints);
  const TouchEvent touchEvent2(10u, TouchEvent::Type::TOUCH_STOP, touchEventTouchPoints);

  ASSERT_THAT(touchEvent1.getId(),        Ne(touchEvent2.getId()));
  ASSERT_THAT(touchEvent1.getType(),      Eq(touchEvent2.getType()));
  ASSERT_EQ(touchEvent1.getTouchPoints(), touchEvent2.getTouchPoints());
  ASSERT_THAT(touchEvent1,                Ne(touchEvent2));
}

TEST_F(ATouchEvent, IsNotEqualToAnotherTouchEventIfTheirTypesAreNotTheSame)
{
  touchEventTouchPoints.addElement(TOUCH_POINT_1);
  const TouchEvent touchEvent1(15u, TouchEvent::Type::TOUCH_STOP, touchEventTouchPoints);
  const TouchEvent touchEvent2(15u, TouchEvent::Type::TOUCH_MOVE, touchEventTouchPoints);

  ASSERT_THAT(touchEvent1.getId(),        Eq(touchEvent2.getId()));
  ASSERT_THAT(touchEvent1.getType(),      Ne(touchEvent2.getType()));
  ASSERT_EQ(touchEvent1.getTouchPoints(), touchEvent2.getTouchPoints());
  ASSERT_THAT(touchEvent1,                Ne(touchEvent2));
}

TEST_F(ATouchEvent, IsNotEqualToAnotherTouchEventIfTheirTouchPointsAreNotTheSame)
{
  const TouchEvent touchEvent1(0u, TouchEvent::Type::TOUCH_START, touchEventTouchPoints);
  touchEventTouchPoints.addElement(TOUCH_POINT_1);
  const TouchEvent touchEvent2(0u, TouchEvent::Type::TOUCH_START, touchEventTouchPoints);

  ASSERT_THAT(touchEvent1.getId(),        Eq(touchEvent2.getId()));
  ASSERT_THAT(touchEvent1.getType(),      Eq(touchEvent2.getType()));
  ASSERT_NE(touchEvent1.getTouchPoints(), touchEvent2.getTouchPoints());
  ASSERT_THAT(touchEvent1,                Ne(touchEvent2));
}