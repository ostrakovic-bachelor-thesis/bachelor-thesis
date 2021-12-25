#include "GUIRectangleBase.h"
#include "FrameBuffer.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


class AGUIRectangleBase : public Test
{
public:
  FrameBuffer<50u, 50u, IFrameBuffer::ColorFormat::RGB888> guiRectangleFrameBuffer;
  GUIRectangleBase guiRectangleBase = GUIRectangleBase(guiRectangleFrameBuffer);
  GUIRectangleBase::GUIRectangleBaseDescription guiRectangleBaseDescription;

  void SetUp() override;
};

void AGUIRectangleBase::SetUp()
{

}

TEST_F(AGUIRectangleBase, GetWidthReturnsZeroWhenCalledOnUninitializedGUIRectangle)
{
  ASSERT_THAT(guiRectangleBase.getWidth(), Eq(0u));
}

TEST_F(AGUIRectangleBase, GetWidthReturnsWidthSpecifiedAtInit)
{
  constexpr uint16_t EXPECTED_GUI_RECTANGLE_BASE_WIDTH = 100u;
  guiRectangleBaseDescription.width = EXPECTED_GUI_RECTANGLE_BASE_WIDTH;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getWidth(), Eq(EXPECTED_GUI_RECTANGLE_BASE_WIDTH));
}

TEST_F(AGUIRectangleBase, GetHeightReturnsZeroWhenCalledOnUninitializedGUIRectangle)
{
  ASSERT_THAT(guiRectangleBase.getHeight(), Eq(0u));
}

TEST_F(AGUIRectangleBase, GetHeightReturnsHeightSpecifiedAtInit)
{
  constexpr uint16_t EXPECTED_GUI_RECTANGLE_BASE_HEIGHT = 150u;
  guiRectangleBaseDescription.height = EXPECTED_GUI_RECTANGLE_BASE_HEIGHT;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getHeight(), Eq(EXPECTED_GUI_RECTANGLE_BASE_HEIGHT));
}

TEST_F(AGUIRectangleBase, GetPositionWithAnyTagReturnsPositionWithBothXAndYSetToZeroWhenCalledOnUninitializedGUIRectangle)
{
  const GUIRectangleBase::Position position = guiRectangleBase.getPosition(GUIRectangleBase::Position::Tag::CENTER);

  ASSERT_THAT(position.x, Eq(0));
  ASSERT_THAT(position.y, Eq(0));
}

TEST_F(AGUIRectangleBase, GetPositionWithAnyTagReturnsPositionWithThatTag)
{
  const GUIRectangleBase::Position position =
    guiRectangleBase.getPosition(GUIRectangleBase::Position::Tag::BOTTOM_LEFT_CORNER);

  ASSERT_THAT(position.tag, Eq(GUIRectangleBase::Position::Tag::BOTTOM_LEFT_CORNER));
}

TEST_F(AGUIRectangleBase, GetPositionTopLeftCornerReturnsExactSameTopLeftCornerPositionSpecifiedAtTheInit)
{
  constexpr GUIRectangleBase::Position EXPECTED_GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = 90,
    .y   = 140,
    .tag = GUIRectangleBase::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangleBaseDescription.position = EXPECTED_GUI_RECTANGLE_BASE_POSITION;
  guiRectangleBase.init(guiRectangleBaseDescription);

  const GUIRectangleBase::Position position =
    guiRectangleBase.getPosition(GUIRectangleBase::Position::Tag::TOP_LEFT_CORNER);

  ASSERT_THAT(position, Eq(EXPECTED_GUI_RECTANGLE_BASE_POSITION));
}

TEST_F(AGUIRectangleBase, GetPositionTopLeftCornerReturnsCorrectValueIfTopRightCornerPositionIsSpecifiedAtTheInit)
{
  constexpr uint16_t GUI_RECTANGLE_BASE_WIDTH = 20;
  constexpr GUIRectangleBase::Position GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = 49,
    .y   = 50,
    .tag = GUIRectangleBase::Position::Tag::TOP_RIGHT_CORNER,
  };
  constexpr GUIRectangleBase::Position EXPECTED_GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = GUI_RECTANGLE_BASE_POSITION.x - GUI_RECTANGLE_BASE_WIDTH + 1,
    .y   = GUI_RECTANGLE_BASE_POSITION.y,
    .tag = GUIRectangleBase::Position::Tag::TOP_LEFT_CORNER,
  };
  guiRectangleBaseDescription.position = GUI_RECTANGLE_BASE_POSITION;
  guiRectangleBaseDescription.width    = GUI_RECTANGLE_BASE_WIDTH;
  guiRectangleBase.init(guiRectangleBaseDescription);

   const GUIRectangleBase::Position position =
    guiRectangleBase.getPosition(GUIRectangleBase::Position::Tag::TOP_LEFT_CORNER);

  ASSERT_THAT(position, Eq(EXPECTED_GUI_RECTANGLE_BASE_POSITION));
}

TEST_F(AGUIRectangleBase, GetPositionTopLeftCornerReturnsCorrectValueIfBottomLeftCornerPositionIsSpecifiedAtTheInit)
{
  constexpr uint16_t GUI_RECTANGLE_BASE_HEIGHT = 25;
  constexpr GUIRectangleBase::Position GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = 180,
    .y   = 74,
    .tag = GUIRectangleBase::Position::Tag::BOTTOM_LEFT_CORNER,
  };
  constexpr GUIRectangleBase::Position EXPECTED_GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = GUI_RECTANGLE_BASE_POSITION.x,
    .y   = GUI_RECTANGLE_BASE_POSITION.y - GUI_RECTANGLE_BASE_HEIGHT + 1,
    .tag = GUIRectangleBase::Position::Tag::TOP_LEFT_CORNER,
  };
  guiRectangleBaseDescription.position = GUI_RECTANGLE_BASE_POSITION;
  guiRectangleBaseDescription.height   = GUI_RECTANGLE_BASE_HEIGHT;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getPosition(GUIRectangleBase::Position::Tag::TOP_LEFT_CORNER),
    Eq(EXPECTED_GUI_RECTANGLE_BASE_POSITION));
}

TEST_F(AGUIRectangleBase, GetPositionTopLeftCornerReturnsCorrectValueIfCenterPositionIsSpecifiedAtTheInit)
{
  constexpr uint16_t GUI_RECTANGLE_BASE_WIDTH  = 25;
  constexpr uint16_t GUI_RECTANGLE_BASE_HEIGHT = 40;
  constexpr GUIRectangleBase::Position GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = 100,
    .y   = 100,
    .tag = GUIRectangleBase::Position::Tag::CENTER,
  };
  constexpr GUIRectangleBase::Position EXPECTED_GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = GUI_RECTANGLE_BASE_POSITION.x - (GUI_RECTANGLE_BASE_WIDTH / 2),
    .y   = GUI_RECTANGLE_BASE_POSITION.y - (GUI_RECTANGLE_BASE_HEIGHT / 2),
    .tag = GUIRectangleBase::Position::Tag::TOP_LEFT_CORNER,
  };
  guiRectangleBaseDescription.position = GUI_RECTANGLE_BASE_POSITION;
  guiRectangleBaseDescription.width    = GUI_RECTANGLE_BASE_WIDTH;
  guiRectangleBaseDescription.height   = GUI_RECTANGLE_BASE_HEIGHT;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getPosition(GUIRectangleBase::Position::Tag::TOP_LEFT_CORNER),
    Eq(EXPECTED_GUI_RECTANGLE_BASE_POSITION));
}

TEST_F(AGUIRectangleBase, GetPositionTopLeftCornerReturnsCorrectValueIfBottomRightCornerPositionIsSpecifiedAtTheInit)
{
  constexpr uint16_t GUI_RECTANGLE_BASE_WIDTH  = 49;
  constexpr uint16_t GUI_RECTANGLE_BASE_HEIGHT = 49;
  constexpr GUIRectangleBase::Position GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = 99,
    .y   = 99,
    .tag = GUIRectangleBase::Position::Tag::BOTTOM_RIGHT_CORNER,
  };
  constexpr GUIRectangleBase::Position EXPECTED_GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = GUI_RECTANGLE_BASE_POSITION.x - GUI_RECTANGLE_BASE_WIDTH + 1,
    .y   = GUI_RECTANGLE_BASE_POSITION.y - GUI_RECTANGLE_BASE_HEIGHT + 1,
    .tag = GUIRectangleBase::Position::Tag::TOP_LEFT_CORNER,
  };
  guiRectangleBaseDescription.position = GUI_RECTANGLE_BASE_POSITION;
  guiRectangleBaseDescription.width    = GUI_RECTANGLE_BASE_WIDTH;
  guiRectangleBaseDescription.height   = GUI_RECTANGLE_BASE_HEIGHT;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getPosition(GUIRectangleBase::Position::Tag::TOP_LEFT_CORNER),
    Eq(EXPECTED_GUI_RECTANGLE_BASE_POSITION));
}

TEST_F(AGUIRectangleBase, GetPositionTopRightCornerReturnsCorrectValueNoMatterWhichPositionTagIsSpecifiedAtTheInit)
{
  constexpr uint16_t GUI_RECTANGLE_BASE_WIDTH = 100;
  constexpr GUIRectangleBase::Position GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = 50,
    .y   = 50,
    .tag = GUIRectangleBase::Position::Tag::TOP_LEFT_CORNER,
  };
  constexpr GUIRectangleBase::Position EXPECTED_GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = GUI_RECTANGLE_BASE_POSITION.x + GUI_RECTANGLE_BASE_WIDTH - 1,
    .y   = GUI_RECTANGLE_BASE_POSITION.y,
    .tag = GUIRectangleBase::Position::Tag::TOP_RIGHT_CORNER,
  };
  guiRectangleBaseDescription.position = GUI_RECTANGLE_BASE_POSITION;
  guiRectangleBaseDescription.width    = GUI_RECTANGLE_BASE_WIDTH;
  guiRectangleBase.init(guiRectangleBaseDescription);

   const GUIRectangleBase::Position position =
    guiRectangleBase.getPosition(GUIRectangleBase::Position::Tag::TOP_RIGHT_CORNER);

  ASSERT_THAT(position, Eq(EXPECTED_GUI_RECTANGLE_BASE_POSITION));
}

TEST_F(AGUIRectangleBase, GetPositionBottomLeftCornerReturnsCorrectValueNoMatterWhichPositionTagIsSpecifiedAtTheInit)
{
  constexpr uint16_t GUI_RECTANGLE_BASE_WIDTH  = 50;
  constexpr uint16_t GUI_RECTANGLE_BASE_HEIGHT = 25;
  constexpr GUIRectangleBase::Position GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = 75,
    .y   = 75,
    .tag = GUIRectangleBase::Position::Tag::CENTER,
  };
  constexpr GUIRectangleBase::Position EXPECTED_GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = GUI_RECTANGLE_BASE_POSITION.x - (GUI_RECTANGLE_BASE_WIDTH / 2),
    .y   = GUI_RECTANGLE_BASE_POSITION.y - (GUI_RECTANGLE_BASE_HEIGHT / 2) + GUI_RECTANGLE_BASE_HEIGHT - 1,
    .tag = GUIRectangleBase::Position::Tag::BOTTOM_LEFT_CORNER,
  };
  guiRectangleBaseDescription.position = GUI_RECTANGLE_BASE_POSITION;
  guiRectangleBaseDescription.width    = GUI_RECTANGLE_BASE_WIDTH;
  guiRectangleBaseDescription.height   = GUI_RECTANGLE_BASE_HEIGHT;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getPosition(GUIRectangleBase::Position::Tag::BOTTOM_LEFT_CORNER),
    Eq(EXPECTED_GUI_RECTANGLE_BASE_POSITION));
}

TEST_F(AGUIRectangleBase, GetPositionBottomRightCornerReturnsCorrectValueNoMatterWhichPositionTagIsSpecifiedAtTheInit)
{
  constexpr uint16_t GUI_RECTANGLE_BASE_WIDTH = 50;
  constexpr GUIRectangleBase::Position GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = 100,
    .y   = 100,
    .tag = GUIRectangleBase::Position::Tag::BOTTOM_LEFT_CORNER,
  };
  constexpr GUIRectangleBase::Position EXPECTED_GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = GUI_RECTANGLE_BASE_POSITION.x + GUI_RECTANGLE_BASE_WIDTH - 1,
    .y   = GUI_RECTANGLE_BASE_POSITION.y,
    .tag = GUIRectangleBase::Position::Tag::BOTTOM_RIGHT_CORNER,
  };
  guiRectangleBaseDescription.position = GUI_RECTANGLE_BASE_POSITION;
  guiRectangleBaseDescription.width    = GUI_RECTANGLE_BASE_WIDTH;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getPosition(GUIRectangleBase::Position::Tag::BOTTOM_RIGHT_CORNER),
    Eq(EXPECTED_GUI_RECTANGLE_BASE_POSITION));
}

TEST_F(AGUIRectangleBase, GetPositionCenterReturnsCorrectValueNoMatterWhichPositionTagIsSpecifiedAtTheInit)
{
  constexpr uint16_t GUI_RECTANGLE_BASE_WIDTH  = 50;
  constexpr uint16_t GUI_RECTANGLE_BASE_HEIGHT = 55;
  constexpr GUIRectangleBase::Position GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = 100,
    .y   = 100,
    .tag = GUIRectangleBase::Position::Tag::TOP_RIGHT_CORNER,
  };
  constexpr GUIRectangleBase::Position EXPECTED_GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = GUI_RECTANGLE_BASE_POSITION.x - (GUI_RECTANGLE_BASE_WIDTH - 1) + (GUI_RECTANGLE_BASE_WIDTH / 2),
    .y   = GUI_RECTANGLE_BASE_POSITION.y + (GUI_RECTANGLE_BASE_HEIGHT / 2),
    .tag = GUIRectangleBase::Position::Tag::CENTER,
  };
  guiRectangleBaseDescription.position = GUI_RECTANGLE_BASE_POSITION;
  guiRectangleBaseDescription.width    = GUI_RECTANGLE_BASE_WIDTH;
  guiRectangleBaseDescription.height   = GUI_RECTANGLE_BASE_HEIGHT;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getPosition(GUIRectangleBase::Position::Tag::CENTER),
    Eq(EXPECTED_GUI_RECTANGLE_BASE_POSITION));
}

TEST_F(AGUIRectangleBase, GetFrameBufferReturnsPointerToTheFrameBufferAssociatedWithGUIRectangleBase)
{
  FrameBuffer<50, 50, IFrameBuffer::ColorFormat::RGB888> frameBuffer;
  GUIRectangleBase guiRectangleBase(frameBuffer);

  ASSERT_EQ(guiRectangleBase.getFrameBuffer(), frameBuffer);
}

TEST_F(AGUIRectangleBase, MoveToPositionMovesGUIRectangleBaseToGivenPosition)
{
  constexpr GUIRectangleBase::Position EXPECTED_GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = 250,
    .y   = 450,
    .tag = GUIRectangleBase::Position::Tag::CENTER
  };
  guiRectangleBase.init(guiRectangleBaseDescription);

  guiRectangleBase.moveToPosition(EXPECTED_GUI_RECTANGLE_BASE_POSITION);

  ASSERT_THAT(guiRectangleBase.getPosition(GUIRectangleBase::Position::Tag::CENTER),
    Eq(EXPECTED_GUI_RECTANGLE_BASE_POSITION));
}

TEST_F(AGUIRectangleBase, GetVisiblePartWidthReturnsWidthSpecifiedAtInitIfNoPartIsOutOfTheScreenAlongXAxis)
{
  constexpr uint16_t GUI_RECTANGLE_BASE_WIDTH = 20u;
  constexpr uint16_t EXPECTED_VISIBLE_PART_WIDTH = GUI_RECTANGLE_BASE_WIDTH;
  guiRectangleBaseDescription.position =
  {
    .x   = 0,
    .y   = 0,
    .tag = GUIRectangleBase::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangleBaseDescription.width = GUI_RECTANGLE_BASE_WIDTH;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getVisiblePartWidth(), Eq(EXPECTED_VISIBLE_PART_WIDTH));
}

TEST_F(AGUIRectangleBase, GetVisiblePartWidthReturnsCorrectlyWidthOfVisiblePartWhenItIsOutOfScreenFromLeftSide)
{
  constexpr int16_t GUI_RECTANGLE_BASE_XPOS      = -20;
  constexpr uint16_t GUI_RECTANGLE_BASE_WIDTH    = 50u;
  constexpr uint16_t EXPECTED_VISIBLE_PART_WIDTH = GUI_RECTANGLE_BASE_XPOS + GUI_RECTANGLE_BASE_WIDTH;
  guiRectangleBaseDescription.position =
  {
    .x   = GUI_RECTANGLE_BASE_XPOS,
    .y   = -10,
    .tag = GUIRectangleBase::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangleBaseDescription.width = GUI_RECTANGLE_BASE_WIDTH;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getVisiblePartWidth(), Eq(EXPECTED_VISIBLE_PART_WIDTH));
}

TEST_F(AGUIRectangleBase, GetVisiblePartWidthReturnsCorrectlyWidthOfVisiblePartWhenItIsOutOfScreenFromRightSide)
{
  constexpr int16_t GUI_RECTANGLE_BASE_XPOS   = 30;
  constexpr uint16_t GUI_RECTANGLE_BASE_WIDTH = 500u;
  const uint16_t EXPECTED_VISIBLE_PART_WIDTH  = guiRectangleFrameBuffer.getWidth() - GUI_RECTANGLE_BASE_XPOS;
  guiRectangleBaseDescription.position =
  {
    .x   = GUI_RECTANGLE_BASE_XPOS,
    .y   = 20,
    .tag = GUIRectangleBase::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangleBaseDescription.width = GUI_RECTANGLE_BASE_WIDTH;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getVisiblePartWidth(), Eq(EXPECTED_VISIBLE_PART_WIDTH));
}

TEST_F(AGUIRectangleBase, GetVisiblePartWidthIsEqualToFrameBufferWidthWhenGUIRectangleIsOutOfScreenFromBothSides)
{
  constexpr int16_t GUI_RECTANGLE_BASE_XPOS   = -10;
  constexpr uint16_t GUI_RECTANGLE_BASE_WIDTH = 500u;
  const uint16_t EXPECTED_VISIBLE_PART_WIDTH  = guiRectangleFrameBuffer.getWidth();
  guiRectangleBaseDescription.position =
  {
    .x   = GUI_RECTANGLE_BASE_XPOS,
    .y   = 110,
    .tag = GUIRectangleBase::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangleBaseDescription.width = GUI_RECTANGLE_BASE_WIDTH;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getVisiblePartWidth(), Eq(EXPECTED_VISIBLE_PART_WIDTH));
}

TEST_F(AGUIRectangleBase, GetVisiblePartWidthReturnsZeroWhenGUIRectangleIsOutOfScreenAlongXAxis)
{
  constexpr int16_t GUI_RECTANGLE_BASE_XPOS   = -500;
  constexpr uint16_t GUI_RECTANGLE_BASE_WIDTH = 50u;
  const uint16_t EXPECTED_VISIBLE_PART_WIDTH  = 0u;
  guiRectangleBaseDescription.position =
  {
    .x   = GUI_RECTANGLE_BASE_XPOS,
    .y   = 110,
    .tag = GUIRectangleBase::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangleBaseDescription.width = GUI_RECTANGLE_BASE_WIDTH;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getVisiblePartWidth(), Eq(EXPECTED_VISIBLE_PART_WIDTH));
}

TEST_F(AGUIRectangleBase, GetVisiblePartHeightReturnsHeightSpecifiedAtInitIfNoPartIsOutOfTheScreenAlongYAxis)
{
  constexpr uint16_t GUI_RECTANGLE_BASE_HEIGHT = 30u;
  constexpr uint16_t EXPECTED_VISIBLE_PART_HEIGHT = GUI_RECTANGLE_BASE_HEIGHT;
  guiRectangleBaseDescription.position =
  {
    .x   = 0,
    .y   = 0,
    .tag = GUIRectangleBase::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangleBaseDescription.height = GUI_RECTANGLE_BASE_HEIGHT;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getVisiblePartHeight(), Eq(EXPECTED_VISIBLE_PART_HEIGHT));
}

TEST_F(AGUIRectangleBase, GetVisiblePartHeightReturnsCorrectlyHeightOfVisiblePartWhenItIsOutOfScreenFromAbove)
{
  constexpr int16_t GUI_RECTANGLE_BASE_YPOS       = -20;
  constexpr uint16_t GUI_RECTANGLE_BASE_HEIGHT    = 50u;
  constexpr uint16_t EXPECTED_VISIBLE_PART_HEIGHT = GUI_RECTANGLE_BASE_YPOS + GUI_RECTANGLE_BASE_HEIGHT;
  guiRectangleBaseDescription.position =
  {
    .x   = 20,
    .y   = GUI_RECTANGLE_BASE_YPOS,
    .tag = GUIRectangleBase::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangleBaseDescription.height = GUI_RECTANGLE_BASE_HEIGHT;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getVisiblePartHeight(), Eq(EXPECTED_VISIBLE_PART_HEIGHT));
}

TEST_F(AGUIRectangleBase, GetVisiblePartHeightReturnsCorrectlyHeightOfVisiblePartWhenItIsOutOfScreenFromBelow)
{
  constexpr int16_t GUI_RECTANGLE_BASE_YPOS    = 30;
  constexpr uint16_t GUI_RECTANGLE_BASE_HEIGHT = 400u;
  const uint16_t EXPECTED_VISIBLE_PART_HEIGHT  = guiRectangleFrameBuffer.getHeight() - GUI_RECTANGLE_BASE_YPOS;
  guiRectangleBaseDescription.position =
  {
    .x   = -10,
    .y   = GUI_RECTANGLE_BASE_YPOS,
    .tag = GUIRectangleBase::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangleBaseDescription.height = GUI_RECTANGLE_BASE_HEIGHT;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getVisiblePartHeight(), Eq(EXPECTED_VISIBLE_PART_HEIGHT));
}

TEST_F(AGUIRectangleBase, GetVisiblePartHeightIsEqualToFrameBufferHeightWhenGUIRectangleIsOutOfScreenFromAboveAndBelow)
{
  constexpr int16_t GUI_RECTANGLE_BASE_YPOS    = -10;
  constexpr uint16_t GUI_RECTANGLE_BASE_HEIGHT = 400u;
  const uint16_t EXPECTED_VISIBLE_PART_HEIGHT  = guiRectangleFrameBuffer.getHeight();
  guiRectangleBaseDescription.position =
  {
    .x   = 30,
    .y   = GUI_RECTANGLE_BASE_YPOS,
    .tag = GUIRectangleBase::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangleBaseDescription.height = GUI_RECTANGLE_BASE_HEIGHT;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getVisiblePartHeight(), Eq(EXPECTED_VISIBLE_PART_HEIGHT));
}

TEST_F(AGUIRectangleBase, GetVisiblePartHeightReturnsZeroWhenGUIRectangleIsOutOfScreenAlongYAxis)
{
  constexpr int16_t GUI_RECTANGLE_BASE_YPOS       = 900;
  constexpr uint16_t GUI_RECTANGLE_BASE_HEIGHT    = 40u;
  constexpr uint16_t EXPECTED_VISIBLE_PART_HEIGHT = 0u;
  guiRectangleBaseDescription.position =
  {
    .x   = 90,
    .y   = GUI_RECTANGLE_BASE_YPOS,
    .tag = GUIRectangleBase::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangleBaseDescription.height = GUI_RECTANGLE_BASE_HEIGHT;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getVisiblePartHeight(), Eq(EXPECTED_VISIBLE_PART_HEIGHT));
}

TEST_F(AGUIRectangleBase, GetVisiblePartPositionWithAnyTagReturnsThatPositionIfPixelIsVisibleInTheFrameBuffer)
{
  constexpr GUIRectangleBase::Position EXPECTED_GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = 30,
    .y   = 40,
    .tag = GUIRectangleBase::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangleBaseDescription.position = EXPECTED_GUI_RECTANGLE_BASE_POSITION;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getVisiblePartPosition(GUIRectangleBase::Position::Tag::TOP_LEFT_CORNER),
    Eq(EXPECTED_GUI_RECTANGLE_BASE_POSITION));
}

TEST_F(AGUIRectangleBase, GetVisiblePartPositionWithAnyTagReturnsXPositionEqualToZeroIfPixelIsOutOfScreenFromLeftSide)
{
  constexpr GUIRectangleBase::Position GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = -30,
    .y   = 40,
    .tag = GUIRectangleBase::Position::Tag::TOP_RIGHT_CORNER
  };
  constexpr GUIRectangleBase::Position EXPECTED_GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = 0,
    .y   = 40,
    .tag = GUIRectangleBase::Position::Tag::TOP_RIGHT_CORNER
  };
  guiRectangleBaseDescription.position = GUI_RECTANGLE_BASE_POSITION;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getVisiblePartPosition(GUIRectangleBase::Position::Tag::TOP_RIGHT_CORNER),
    Eq(EXPECTED_GUI_RECTANGLE_BASE_POSITION));
}


TEST_F(AGUIRectangleBase, GetVisiblePartPositionWithAnyTagReturnsXPositionEqualToFrameBufferWidthIfPixelIsOutOfScreenFromRightSide)
{
  constexpr GUIRectangleBase::Position GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = 500,
    .y   = 10,
    .tag = GUIRectangleBase::Position::Tag::BOTTOM_LEFT_CORNER
  };
  const GUIRectangleBase::Position EXPECTED_GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = static_cast<int16_t>(guiRectangleFrameBuffer.getWidth()),
    .y   = 10,
    .tag = GUIRectangleBase::Position::Tag::BOTTOM_LEFT_CORNER
  };

  guiRectangleBaseDescription.position = GUI_RECTANGLE_BASE_POSITION;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getVisiblePartPosition(GUIRectangleBase::Position::Tag::BOTTOM_LEFT_CORNER),
    Eq(EXPECTED_GUI_RECTANGLE_BASE_POSITION));
}

TEST_F(AGUIRectangleBase, GetVisiblePartPositionWithAnyTagReturnsYPositionEqualToZeroIfPixelIsOutOfScreenFromAbove)
{
  constexpr GUIRectangleBase::Position GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = 35,
    .y   = -100,
    .tag = GUIRectangleBase::Position::Tag::BOTTOM_RIGHT_CORNER
  };
  constexpr GUIRectangleBase::Position EXPECTED_GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = 35,
    .y   = 0,
    .tag = GUIRectangleBase::Position::Tag::BOTTOM_RIGHT_CORNER
  };
  guiRectangleBaseDescription.position = GUI_RECTANGLE_BASE_POSITION;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getVisiblePartPosition(GUIRectangleBase::Position::Tag::BOTTOM_RIGHT_CORNER),
    Eq(EXPECTED_GUI_RECTANGLE_BASE_POSITION));
}

TEST_F(AGUIRectangleBase, GetVisiblePartPositionWithAnyTagReturnsYPositionEqualToFrameBufferHeightIfPixelIsOutOfScreenFromBelow)
{
  constexpr GUIRectangleBase::Position GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = 0,
    .y   = 500,
    .tag = GUIRectangleBase::Position::Tag::CENTER
  };
  const GUIRectangleBase::Position EXPECTED_GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = 0,
    .y   = static_cast<int16_t>(guiRectangleFrameBuffer.getHeight()),
    .tag = GUIRectangleBase::Position::Tag::CENTER
  };
  guiRectangleBaseDescription.position = GUI_RECTANGLE_BASE_POSITION;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getVisiblePartPosition(GUIRectangleBase::Position::Tag::CENTER),
    Eq(EXPECTED_GUI_RECTANGLE_BASE_POSITION));
}
