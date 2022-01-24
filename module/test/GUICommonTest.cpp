#include "GUICommon.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


TEST(GUIPosition, IsEqualToAnotherGUIPositionOnlyIfTheirXandYCoordinatesTogetherWithPositionTagsAreTheSame)
{
  const GUI::Position position1 =
  {
    .x   = 250,
    .y   = -40,
    .tag = GUI::Position::Tag::CENTER
  };
  const GUI::Position position2 =
  {
    .x   = 250,
    .y   = -40,
    .tag = GUI::Position::Tag::CENTER
  };

  ASSERT_THAT(position1.x, Eq(position2.x));
  ASSERT_THAT(position1.y, Eq(position2.y));
  ASSERT_THAT(position1.tag, Eq(position2.tag));
  ASSERT_THAT(position1, Eq(position2));
}

TEST(GUIPosition, IsNotEqualToAnotherGUIPositionIfTheirXCoordinatesAreNotTheSame)
{
  const GUI::Position position1 =
  {
    .x   = 250,
    .y   = 300,
    .tag = GUI::Position::Tag::BOTTOM_RIGHT_CORNER
  };
  const GUI::Position position2 =
  {
    .x   = 240,
    .y   = 300,
    .tag = GUI::Position::Tag::BOTTOM_RIGHT_CORNER
  };

  ASSERT_THAT(position1.x, Ne(position2.x));
  ASSERT_THAT(position1.y, Eq(position2.y));
  ASSERT_THAT(position1.tag, Eq(position2.tag));
  ASSERT_THAT(position1, Ne(position2));
}

TEST(GUIPosition, IsNotEqualToAnotherGUIPositionIfTheirYCoordinatesAreNotTheSame)
{
  const GUI::Position position1 =
  {
    .x   = 300,
    .y   = -80,
    .tag = GUI::Position::Tag::TOP_LEFT_CORNER
  };
  const GUI::Position position2 =
  {
    .x   = 300,
    .y   = 80,
    .tag = GUI::Position::Tag::TOP_LEFT_CORNER
  };

  ASSERT_THAT(position1.x, Eq(position2.x));
  ASSERT_THAT(position1.y, Ne(position2.y));
  ASSERT_THAT(position1.tag, Eq(position2.tag));
  ASSERT_THAT(position1, Ne(position2));
}

TEST(GUIPosition, IsNotEqualToAnotherGUIPositionIfTheirPositionTagsAreNotTheSame)
{
  const GUI::Position position1 =
  {
    .x   = 420,
    .y   = -100,
    .tag = GUI::Position::Tag::TOP_RIGHT_CORNER
  };
  const GUI::Position position2 =
  {
    .x   = 420,
    .y   = -100,
    .tag = GUI::Position::Tag::CENTER
  };

  ASSERT_THAT(position1.x, Eq(position2.x));
  ASSERT_THAT(position1.y, Eq(position2.y));
  ASSERT_THAT(position1.tag, Ne(position2.tag));
  ASSERT_THAT(position1, Ne(position2));
}

TEST(GUIDimension, IsEqualToAnotherGUIDimensionOnlyIfTheirWidthAndHeightAreTheSame)
{
  const GUI::Dimension dimension1 =
  {
    .width  = 250,
    .height = 130
  };
  const GUI::Dimension dimension2 =
  {
    .width  = 250,
    .height = 130
  };

  ASSERT_THAT(dimension1.width, Eq(dimension2.width));
  ASSERT_THAT(dimension1.height, Eq(dimension2.height));
  ASSERT_THAT(dimension1, Eq(dimension2));
}

TEST(GUIDimension, IsNotEqualToAnotherGUIDimensionIfTheirWidthsAreNotTheSame)
{
  const GUI::Dimension dimension1 =
  {
    .width  = 300,
    .height = 130
  };
  const GUI::Dimension dimension2 =
  {
    .width  = 250,
    .height = 130
  };

  ASSERT_THAT(dimension1.width, Ne(dimension2.width));
  ASSERT_THAT(dimension1.height, Eq(dimension2.height));
  ASSERT_THAT(dimension1, Ne(dimension2));
}

TEST(GUIDimension, IsNotEqualToAnotherGUIDimensionIfTheirHeightsAreNotTheSame)
{
  const GUI::Dimension dimension1 =
  {
    .width  = 110,
    .height = 90
  };
  const GUI::Dimension dimension2 =
  {
    .width  = 110,
    .height = 200
  };

  ASSERT_THAT(dimension1.width, Eq(dimension2.width));
  ASSERT_THAT(dimension1.height, Ne(dimension2.height));
  ASSERT_THAT(dimension1, Ne(dimension2));
}

TEST(GUIColor, IsEqualToAnotherGUIColorOnlyIfAllTheirColorComponentsAreTheSame)
{
  const GUI::Color color1 =
  {
    .red   = 240u,
    .green = 110u,
    .blue  = 160u
  };
  const GUI::Color color2 =
  {
    .red   = 240u,
    .green = 110u,
    .blue  = 160u
  };

  ASSERT_THAT(color1.red, Eq(color2.red));
  ASSERT_THAT(color1.green, Eq(color2.green));
  ASSERT_THAT(color1.blue, Eq(color2.blue));
  ASSERT_THAT(color1, Eq(color2));
}

TEST(GUIColor, IsNotEqualToAnotherGUIColorIfTheirRedComponentsAreNotTheSame)
{
  const GUI::Color color1 =
  {
    .red   = 90u,
    .green = 150u,
    .blue  = 60u
  };
  const GUI::Color color2 =
  {
    .red   = 240u,
    .green = 150u,
    .blue  = 60u
  };

  ASSERT_THAT(color1.red, Ne(color2.red));
  ASSERT_THAT(color1.green, Eq(color2.green));
  ASSERT_THAT(color1.blue, Eq(color2.blue));
  ASSERT_THAT(color1, Ne(color2));
}

TEST(GUIColor, IsNotEqualToAnotherGUIColorIfTheirGreenComponentsAreNotTheSame)
{
  const GUI::Color color1 =
  {
    .red   = 30u,
    .green = 150u,
    .blue  = 110u
  };
  const GUI::Color color2 =
  {
    .red   = 30u,
    .green = 200u,
    .blue  = 110u
  };

  ASSERT_THAT(color1.red, Eq(color2.red));
  ASSERT_THAT(color1.green, Ne(color2.green));
  ASSERT_THAT(color1.blue, Eq(color2.blue));
  ASSERT_THAT(color1, Ne(color2));
}

TEST(GUIColor, IsNotEqualToAnotherGUIColorIfTheirBlueComponentsAreNotTheSame)
{
  const GUI::Color color1 =
  {
    .red   = 250u,
    .green = 160u,
    .blue  = 40u
  };
  const GUI::Color color2 =
  {
    .red   = 250u,
    .green = 160u,
    .blue  = 0u
  };

  ASSERT_THAT(color1.red, Eq(color2.red));
  ASSERT_THAT(color1.green, Eq(color2.green));
  ASSERT_THAT(color1.blue, Ne(color2.blue));
  ASSERT_THAT(color1, Ne(color2));
}