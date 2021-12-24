#include "IGUIObject.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


TEST(IGUIObjectPosition, IsEqualToAnotherIGUIObjectPositionOnlyIfTheirXandYCoordinatesTogetherWithPositionTagsAreTheSame)
{
  const IGUIObject::Position position1 =
  {
    .x   = 250,
    .y   = -40,
    .tag = IGUIObject::Position::Tag::CENTER
  };
  const IGUIObject::Position position2 =
  {
    .x   = 250,
    .y   = -40,
    .tag = IGUIObject::Position::Tag::CENTER
  };

  ASSERT_THAT(position1.x, Eq(position2.x));
  ASSERT_THAT(position1.y, Eq(position2.y));
  ASSERT_THAT(position1.tag, Eq(position2.tag));
  ASSERT_THAT(position1, Eq(position2));
}

TEST(IGUIObjectPosition, IsNotEqualToAnotherIGUIObjectIfTheirXCoordinatesAreNotTheSame)
{
  const IGUIObject::Position position1 =
  {
    .x   = 250,
    .y   = 300,
    .tag = IGUIObject::Position::Tag::BOTTOM_RIGHT_CORNER
  };
  const IGUIObject::Position position2 =
  {
    .x   = 240,
    .y   = 300,
    .tag = IGUIObject::Position::Tag::BOTTOM_RIGHT_CORNER
  };

  ASSERT_THAT(position1.x, Ne(position2.x));
  ASSERT_THAT(position1.y, Eq(position2.y));
 ASSERT_THAT(position1.tag, Eq(position2.tag));
  ASSERT_THAT(position1, Ne(position2));
}

TEST(IGUIObjectPosition, IsNotEqualToAnotherIGUIObjectIfTheirYCoordinatesAreNotTheSame)
{
  const IGUIObject::Position position1 =
  {
    .x   = 300,
    .y   = -80,
    .tag = IGUIObject::Position::Tag::TOP_LEFT_CORNER
  };
  const IGUIObject::Position position2 =
  {
    .x   = 300,
    .y   = 80,
    .tag = IGUIObject::Position::Tag::TOP_LEFT_CORNER
  };

  ASSERT_THAT(position1.x, Eq(position2.x));
  ASSERT_THAT(position1.y, Ne(position2.y));
  ASSERT_THAT(position1.tag, Eq(position2.tag));
  ASSERT_THAT(position1, Ne(position2));
}

TEST(IGUIObjectPosition, IsNotEqualToAnotherIGUIObjectIfTheirPositionTagsAreNotTheSame)
{
  const IGUIObject::Position position1 =
  {
    .x   = 420,
    .y   = -100,
    .tag = IGUIObject::Position::Tag::TOP_RIGHT_CORNER
  };
  const IGUIObject::Position position2 =
  {
    .x   = 420,
    .y   = -100,
    .tag = IGUIObject::Position::Tag::CENTER
  };

  ASSERT_THAT(position1.x, Eq(position2.x));
  ASSERT_THAT(position1.y, Eq(position2.y));
  ASSERT_THAT(position1.tag, Ne(position2.tag));
  ASSERT_THAT(position1, Ne(position2));
}