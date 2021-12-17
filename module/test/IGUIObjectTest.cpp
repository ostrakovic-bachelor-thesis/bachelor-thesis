#include "IGUIObject.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


TEST(IGUIObjectPosition, IsEqualToAnotherIGUIObjectPositionOnlyIfBothTheirXandYCoordinatesAreTheSame)
{
  const IGUIObject::Position position1 =
  {
    .x = 250,
    .y = -40
  };
  const IGUIObject::Position position2 =
  {
    .x = 250,
    .y = -40
  };

  ASSERT_THAT(position1.x, Eq(position2.x));
  ASSERT_THAT(position1.y, Eq(position2.y));
  ASSERT_THAT(position1, Eq(position2));
}

TEST(IGUIObjectPosition, IsNotEqualToAnotherIGUIObjectIfTheirXCoordinatesAreNotTheSame)
{
  const IGUIObject::Position position1 =
  {
    .x = 250,
    .y = -40
  };
  const IGUIObject::Position position2 =
  {
    .x = 240,
    .y = -40
  };

  ASSERT_THAT(position1.x, Ne(position2.x));
  ASSERT_THAT(position1.y, Eq(position2.y));
  ASSERT_THAT(position1, Ne(position2));
}

TEST(IGUIObjectPosition, IsNotEqualToAnotherIGUIObjectIfTheirYCoordinatesAreNotTheSame)
{
  const IGUIObject::Position position1 =
  {
    .x = 250,
    .y = -40
  };
  const IGUIObject::Position position2 =
  {
    .x = 250,
    .y = 40
  };

  ASSERT_THAT(position1.x, Eq(position2.x));
  ASSERT_THAT(position1.y, Ne(position2.y));
  ASSERT_THAT(position1, Ne(position2));
}
