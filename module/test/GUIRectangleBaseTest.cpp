#include "GUIRectangleBase.h"
#include "FrameBuffer.h"
#include "GUIRectangleBaseMock.h"
#include "SysTickMock.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


class AGUIRectangleBase : public Test
{
public:
  AGUIRectangleBase():
    guiRectangleBase(sysTickMock, guiRectangleFrameBuffer)
  {}

  NiceMock<SysTickMock> sysTickMock;
  FrameBuffer<50u, 50u, IFrameBuffer::ColorFormat::RGB888> guiRectangleFrameBuffer;
  NiceMock<GUIRectangleBaseMock> guiRectangleBase;
  GUI::RectangleBase::RectangleBaseDescription guiRectangleBaseDescription;

  static constexpr uint64_t DRAW_OPERATION_DURATION_IN_US = 4300u;
  static constexpr GUI::Position GUI_RECTANGLE_PARTIALLY_OUT_OF_SCREEN_POSITION =
  {
    .x   = -10,
    .y   = -20,
    .tag = GUI::Position::Tag::TOP_LEFT_CORNER
  };
  static constexpr GUI::Position GUI_RECTANGLE_COMPLETELY_OUT_OF_SCREEN_POSITION =
  {
    .x   = 0,
    .y   = 500,
    .tag = GUI::Position::Tag::TOP_LEFT_CORNER
  };


  GUI::RectangleBase::CallbackDescription callbackDescription;
  bool m_isCallbackCalled;
  uint64_t m_sysTickFunctionCallCounter;

  void setupSysTickReadings(uint64_t drawOperationExecutionTimeInUs);
  void assertThatCallbackIsCalled(void);
  void assertThatCallbackIsNotCalled(void);

  void SetUp() override;
};

constexpr GUI::Position AGUIRectangleBase::GUI_RECTANGLE_PARTIALLY_OUT_OF_SCREEN_POSITION;

void AGUIRectangleBase::SetUp()
{
  m_isCallbackCalled           = false;
  m_sysTickFunctionCallCounter = 0u;

  guiRectangleBaseDescription.dimension =
  {
    .width  = 10u,
    .height = 10u
  };
  guiRectangleBaseDescription.position =
  {
    .x   = 0u,
    .y   = 0u,
    .tag = GUI::Position::Tag::TOP_LEFT_CORNER
  };

  callbackDescription =
  {
    .functionPtr = [](void *argument) { *reinterpret_cast<bool*>(argument) = true; },
    .argument = &m_isCallbackCalled
  };
}

void AGUIRectangleBase::setupSysTickReadings(uint64_t drawOperationExecutionTimeInUs)
{
  EXPECT_CALL(sysTickMock, getTicks())
    .WillRepeatedly([&] ()
    {
      return (m_sysTickFunctionCallCounter++);
    });

  EXPECT_CALL(sysTickMock, getElapsedTimeInUs(_))
    .WillRepeatedly([&, drawOperationExecutionTimeInUs] (uint64_t timestamp)
    {
      return ((m_sysTickFunctionCallCounter++) - timestamp) * drawOperationExecutionTimeInUs;
    });
}

void AGUIRectangleBase::assertThatCallbackIsCalled(void)
{
  ASSERT_THAT(m_isCallbackCalled, Eq(true));
}

void AGUIRectangleBase::assertThatCallbackIsNotCalled(void)
{
  ASSERT_THAT(m_isCallbackCalled, Eq(false));
}


TEST_F(AGUIRectangleBase, GetWidthReturnsZeroWhenItIsCalledOnUninitializedObject)
{
  ASSERT_THAT(guiRectangleBase.getWidth(), Eq(0u));
}

TEST_F(AGUIRectangleBase, GetWidthReturnsWidthSpecifiedAtInit)
{
  constexpr uint16_t EXPECTED_GUI_RECTANGLE_BASE_WIDTH = 100u;
  guiRectangleBaseDescription.dimension.width = EXPECTED_GUI_RECTANGLE_BASE_WIDTH;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getWidth(), Eq(EXPECTED_GUI_RECTANGLE_BASE_WIDTH));
}

TEST_F(AGUIRectangleBase, GetHeightReturnsZeroWhenItIsCalledOnUninitializedObject)
{
  ASSERT_THAT(guiRectangleBase.getHeight(), Eq(0u));
}

TEST_F(AGUIRectangleBase, GetHeightReturnsHeightSpecifiedAtInit)
{
  constexpr uint16_t EXPECTED_GUI_RECTANGLE_BASE_HEIGHT = 150u;
  guiRectangleBaseDescription.dimension.height = EXPECTED_GUI_RECTANGLE_BASE_HEIGHT;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getHeight(), Eq(EXPECTED_GUI_RECTANGLE_BASE_HEIGHT));
}

TEST_F(AGUIRectangleBase, GetDimensionReturnsDimensionWithWidthAndHeightSetToZerosWhenItIsCalledOnUninitializedObject)
{
  const GUI::Dimension dimension = guiRectangleBase.getDimension();

  ASSERT_THAT(dimension.width, Eq(0u));
  ASSERT_THAT(dimension.height, Eq(0u));
}

TEST_F(AGUIRectangleBase, GetDimensionReturnsGUIRectangleDimensionsSpecifiedAtInit)
{
  constexpr GUI::Dimension EXPECTED_GUI_RECTANGLE_BASE_DIMENSION =
  {
    .width  = 100u,
    .height = 150u
  };
  guiRectangleBaseDescription.dimension = EXPECTED_GUI_RECTANGLE_BASE_DIMENSION;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getDimension(), Eq(EXPECTED_GUI_RECTANGLE_BASE_DIMENSION));
}

TEST_F(AGUIRectangleBase, GetPositionWithAnyTagReturnsPositionWithBothXAndYSetToZerosWhenItIsCalledOnUninitializedObject)
{
  const GUI::Position position = guiRectangleBase.getPosition(GUI::Position::Tag::CENTER);

  ASSERT_THAT(position.x, Eq(0));
  ASSERT_THAT(position.y, Eq(0));
}

TEST_F(AGUIRectangleBase, GetPositionWithAnyTagReturnsPositionWithThatTag)
{
  const GUI::Position position =
    guiRectangleBase.getPosition(GUI::Position::Tag::BOTTOM_LEFT_CORNER);

  ASSERT_THAT(position.tag, Eq(GUI::Position::Tag::BOTTOM_LEFT_CORNER));
}

TEST_F(AGUIRectangleBase, GetPositionTopLeftCornerReturnsExactSameTopLeftCornerPositionSpecifiedAtTheInit)
{
  constexpr GUI::Position EXPECTED_GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = 90,
    .y   = 140,
    .tag = GUI::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangleBaseDescription.position = EXPECTED_GUI_RECTANGLE_BASE_POSITION;
  guiRectangleBase.init(guiRectangleBaseDescription);

  const GUI::Position position =
    guiRectangleBase.getPosition(GUI::Position::Tag::TOP_LEFT_CORNER);

  ASSERT_THAT(position, Eq(EXPECTED_GUI_RECTANGLE_BASE_POSITION));
}

TEST_F(AGUIRectangleBase, GetPositionTopLeftCornerReturnsCorrectValueIfTopRightCornerPositionIsSpecifiedAtTheInit)
{
  constexpr uint16_t GUI_RECTANGLE_BASE_WIDTH = 20;
  constexpr GUI::Position GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = 49,
    .y   = 50,
    .tag = GUI::Position::Tag::TOP_RIGHT_CORNER,
  };
  constexpr GUI::Position EXPECTED_GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = GUI_RECTANGLE_BASE_POSITION.x - GUI_RECTANGLE_BASE_WIDTH + 1,
    .y   = GUI_RECTANGLE_BASE_POSITION.y,
    .tag = GUI::Position::Tag::TOP_LEFT_CORNER,
  };
  guiRectangleBaseDescription.position        = GUI_RECTANGLE_BASE_POSITION;
  guiRectangleBaseDescription.dimension.width = GUI_RECTANGLE_BASE_WIDTH;
  guiRectangleBase.init(guiRectangleBaseDescription);

   const GUI::Position position =
    guiRectangleBase.getPosition(GUI::Position::Tag::TOP_LEFT_CORNER);

  ASSERT_THAT(position, Eq(EXPECTED_GUI_RECTANGLE_BASE_POSITION));
}

TEST_F(AGUIRectangleBase, GetPositionTopLeftCornerReturnsCorrectValueIfBottomLeftCornerPositionIsSpecifiedAtTheInit)
{
  constexpr uint16_t GUI_RECTANGLE_BASE_HEIGHT = 25;
  constexpr GUI::Position GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = 180,
    .y   = 74,
    .tag = GUI::Position::Tag::BOTTOM_LEFT_CORNER,
  };
  constexpr GUI::Position EXPECTED_GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = GUI_RECTANGLE_BASE_POSITION.x,
    .y   = GUI_RECTANGLE_BASE_POSITION.y - GUI_RECTANGLE_BASE_HEIGHT + 1,
    .tag = GUI::Position::Tag::TOP_LEFT_CORNER,
  };
  guiRectangleBaseDescription.position         = GUI_RECTANGLE_BASE_POSITION;
  guiRectangleBaseDescription.dimension.height = GUI_RECTANGLE_BASE_HEIGHT;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getPosition(GUI::Position::Tag::TOP_LEFT_CORNER),
    Eq(EXPECTED_GUI_RECTANGLE_BASE_POSITION));
}

TEST_F(AGUIRectangleBase, GetPositionTopLeftCornerReturnsCorrectValueIfCenterPositionIsSpecifiedAtTheInit)
{
  constexpr uint16_t GUI_RECTANGLE_BASE_WIDTH  = 25;
  constexpr uint16_t GUI_RECTANGLE_BASE_HEIGHT = 40;
  constexpr GUI::Position GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = 100,
    .y   = 100,
    .tag = GUI::Position::Tag::CENTER,
  };
  constexpr GUI::Position EXPECTED_GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = GUI_RECTANGLE_BASE_POSITION.x - (GUI_RECTANGLE_BASE_WIDTH / 2),
    .y   = GUI_RECTANGLE_BASE_POSITION.y - (GUI_RECTANGLE_BASE_HEIGHT / 2),
    .tag = GUI::Position::Tag::TOP_LEFT_CORNER,
  };
  guiRectangleBaseDescription.position = GUI_RECTANGLE_BASE_POSITION;
  guiRectangleBaseDescription.dimension =
  {
    .width  = GUI_RECTANGLE_BASE_WIDTH,
    .height = GUI_RECTANGLE_BASE_HEIGHT
  };
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getPosition(GUI::Position::Tag::TOP_LEFT_CORNER),
    Eq(EXPECTED_GUI_RECTANGLE_BASE_POSITION));
}

TEST_F(AGUIRectangleBase, GetPositionTopLeftCornerReturnsCorrectValueIfBottomRightCornerPositionIsSpecifiedAtTheInit)
{
  constexpr uint16_t GUI_RECTANGLE_BASE_WIDTH  = 49;
  constexpr uint16_t GUI_RECTANGLE_BASE_HEIGHT = 49;
  constexpr GUI::Position GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = 99,
    .y   = 99,
    .tag = GUI::Position::Tag::BOTTOM_RIGHT_CORNER,
  };
  constexpr GUI::Position EXPECTED_GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = GUI_RECTANGLE_BASE_POSITION.x - GUI_RECTANGLE_BASE_WIDTH + 1,
    .y   = GUI_RECTANGLE_BASE_POSITION.y - GUI_RECTANGLE_BASE_HEIGHT + 1,
    .tag = GUI::Position::Tag::TOP_LEFT_CORNER,
  };
  guiRectangleBaseDescription.position  = GUI_RECTANGLE_BASE_POSITION;
  guiRectangleBaseDescription.dimension =
  {
    .width  = GUI_RECTANGLE_BASE_WIDTH,
    .height = GUI_RECTANGLE_BASE_HEIGHT
  };
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getPosition(GUI::Position::Tag::TOP_LEFT_CORNER),
    Eq(EXPECTED_GUI_RECTANGLE_BASE_POSITION));
}

TEST_F(AGUIRectangleBase, GetPositionTopRightCornerReturnsCorrectValueNoMatterWhichPositionTagIsSpecifiedAtTheInit)
{
  constexpr uint16_t GUI_RECTANGLE_BASE_WIDTH = 100;
  constexpr GUI::Position GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = 50,
    .y   = 50,
    .tag = GUI::Position::Tag::TOP_LEFT_CORNER,
  };
  constexpr GUI::Position EXPECTED_GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = GUI_RECTANGLE_BASE_POSITION.x + GUI_RECTANGLE_BASE_WIDTH - 1,
    .y   = GUI_RECTANGLE_BASE_POSITION.y,
    .tag = GUI::Position::Tag::TOP_RIGHT_CORNER,
  };
  guiRectangleBaseDescription.position        = GUI_RECTANGLE_BASE_POSITION;
  guiRectangleBaseDescription.dimension.width = GUI_RECTANGLE_BASE_WIDTH;
  guiRectangleBase.init(guiRectangleBaseDescription);

   const GUI::Position position =
    guiRectangleBase.getPosition(GUI::Position::Tag::TOP_RIGHT_CORNER);

  ASSERT_THAT(position, Eq(EXPECTED_GUI_RECTANGLE_BASE_POSITION));
}

TEST_F(AGUIRectangleBase, GetPositionBottomLeftCornerReturnsCorrectValueNoMatterWhichPositionTagIsSpecifiedAtTheInit)
{
  constexpr uint16_t GUI_RECTANGLE_BASE_WIDTH  = 50;
  constexpr uint16_t GUI_RECTANGLE_BASE_HEIGHT = 25;
  constexpr GUI::Position GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = 75,
    .y   = 75,
    .tag = GUI::Position::Tag::CENTER,
  };
  constexpr GUI::Position EXPECTED_GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = GUI_RECTANGLE_BASE_POSITION.x - (GUI_RECTANGLE_BASE_WIDTH / 2),
    .y   = GUI_RECTANGLE_BASE_POSITION.y - (GUI_RECTANGLE_BASE_HEIGHT / 2) + GUI_RECTANGLE_BASE_HEIGHT - 1,
    .tag = GUI::Position::Tag::BOTTOM_LEFT_CORNER,
  };
  guiRectangleBaseDescription.position  = GUI_RECTANGLE_BASE_POSITION;
  guiRectangleBaseDescription.dimension =
  {
    .width  = GUI_RECTANGLE_BASE_WIDTH,
    .height = GUI_RECTANGLE_BASE_HEIGHT
  };
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getPosition(GUI::Position::Tag::BOTTOM_LEFT_CORNER),
    Eq(EXPECTED_GUI_RECTANGLE_BASE_POSITION));
}

TEST_F(AGUIRectangleBase, GetPositionBottomRightCornerReturnsCorrectValueNoMatterWhichPositionTagIsSpecifiedAtTheInit)
{
  constexpr uint16_t GUI_RECTANGLE_BASE_WIDTH = 50;
  constexpr GUI::Position GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = 100,
    .y   = 100,
    .tag = GUI::Position::Tag::BOTTOM_LEFT_CORNER,
  };
  constexpr GUI::Position EXPECTED_GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = GUI_RECTANGLE_BASE_POSITION.x + GUI_RECTANGLE_BASE_WIDTH - 1,
    .y   = GUI_RECTANGLE_BASE_POSITION.y,
    .tag = GUI::Position::Tag::BOTTOM_RIGHT_CORNER,
  };
  guiRectangleBaseDescription.position        = GUI_RECTANGLE_BASE_POSITION;
  guiRectangleBaseDescription.dimension.width = GUI_RECTANGLE_BASE_WIDTH;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getPosition(GUI::Position::Tag::BOTTOM_RIGHT_CORNER),
    Eq(EXPECTED_GUI_RECTANGLE_BASE_POSITION));
}

TEST_F(AGUIRectangleBase, GetPositionCenterReturnsCorrectValueNoMatterWhichPositionTagIsSpecifiedAtTheInit)
{
  constexpr uint16_t GUI_RECTANGLE_BASE_WIDTH  = 50;
  constexpr uint16_t GUI_RECTANGLE_BASE_HEIGHT = 55;
  constexpr GUI::Position GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = 100,
    .y   = 100,
    .tag = GUI::Position::Tag::TOP_RIGHT_CORNER,
  };
  constexpr GUI::Position EXPECTED_GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = GUI_RECTANGLE_BASE_POSITION.x - (GUI_RECTANGLE_BASE_WIDTH - 1) + (GUI_RECTANGLE_BASE_WIDTH / 2),
    .y   = GUI_RECTANGLE_BASE_POSITION.y + (GUI_RECTANGLE_BASE_HEIGHT / 2),
    .tag = GUI::Position::Tag::CENTER,
  };
  guiRectangleBaseDescription.position  = GUI_RECTANGLE_BASE_POSITION;
  guiRectangleBaseDescription.dimension =
  {
    .width  = GUI_RECTANGLE_BASE_WIDTH,
    .height = GUI_RECTANGLE_BASE_HEIGHT
  };
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getPosition(GUI::Position::Tag::CENTER),
    Eq(EXPECTED_GUI_RECTANGLE_BASE_POSITION));
}

TEST_F(AGUIRectangleBase, GetFrameBufferReturnsReferenceToTheFrameBufferAssociatedWithGUIRectangleBase)
{
  FrameBuffer<50, 50, IFrameBuffer::ColorFormat::RGB888> frameBuffer;
  GUIRectangleBaseMock guiRectangleBase(sysTickMock, frameBuffer);

  ASSERT_EQ(guiRectangleBase.getFrameBuffer(), frameBuffer);
}

TEST_F(AGUIRectangleBase, SetFrameBufferSetsNewFrameBufferWhichWillBeUsedByGUIRectangleBase)
{
  FrameBuffer<30, 40, IFrameBuffer::ColorFormat::RGB888> newFrameBuffer;
  guiRectangleBase.setFrameBuffer(newFrameBuffer);

  ASSERT_EQ(guiRectangleBase.getFrameBuffer(), newFrameBuffer);
}

TEST_F(AGUIRectangleBase, MoveToPositionMovesGUIRectangleBaseToGivenPosition)
{
  constexpr GUI::Position EXPECTED_GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = 250,
    .y   = 450,
    .tag = GUI::Position::Tag::CENTER
  };
  guiRectangleBase.init(guiRectangleBaseDescription);

  guiRectangleBase.moveToPosition(EXPECTED_GUI_RECTANGLE_BASE_POSITION);

  ASSERT_THAT(guiRectangleBase.getPosition(GUI::Position::Tag::CENTER),
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
    .tag = GUI::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangleBaseDescription.dimension.width = GUI_RECTANGLE_BASE_WIDTH;
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
    .tag = GUI::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangleBaseDescription.dimension.width = GUI_RECTANGLE_BASE_WIDTH;
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
    .tag = GUI::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangleBaseDescription.dimension.width = GUI_RECTANGLE_BASE_WIDTH;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getVisiblePartWidth(), Eq(EXPECTED_VISIBLE_PART_WIDTH));
}

TEST_F(AGUIRectangleBase, GetVisiblePartWidthIsEqualToFrameBufferWidthWhenGUIRectangleIsOutOfScreenFromBothSidesSimultaneously)
{
  constexpr int16_t GUI_RECTANGLE_BASE_XPOS   = -10;
  constexpr uint16_t GUI_RECTANGLE_BASE_WIDTH = 500u;
  const uint16_t EXPECTED_VISIBLE_PART_WIDTH  = guiRectangleFrameBuffer.getWidth();
  guiRectangleBaseDescription.position =
  {
    .x   = GUI_RECTANGLE_BASE_XPOS,
    .y   = 110,
    .tag = GUI::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangleBaseDescription.dimension.width = GUI_RECTANGLE_BASE_WIDTH;
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
    .tag = GUI::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangleBaseDescription.dimension.width = GUI_RECTANGLE_BASE_WIDTH;
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
    .tag = GUI::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangleBaseDescription.dimension.height = GUI_RECTANGLE_BASE_HEIGHT;
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
    .tag = GUI::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangleBaseDescription.dimension.height = GUI_RECTANGLE_BASE_HEIGHT;
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
    .tag = GUI::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangleBaseDescription.dimension.height = GUI_RECTANGLE_BASE_HEIGHT;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getVisiblePartHeight(), Eq(EXPECTED_VISIBLE_PART_HEIGHT));
}

TEST_F(AGUIRectangleBase, GetVisiblePartHeightIsEqualToFrameBufferHeightWhenGUIRectangleIsOutOfScreenFromBothSidesSimultaneously)
{
  constexpr int16_t GUI_RECTANGLE_BASE_YPOS    = -10;
  constexpr uint16_t GUI_RECTANGLE_BASE_HEIGHT = 400u;
  const uint16_t EXPECTED_VISIBLE_PART_HEIGHT  = guiRectangleFrameBuffer.getHeight();
  guiRectangleBaseDescription.position =
  {
    .x   = 30,
    .y   = GUI_RECTANGLE_BASE_YPOS,
    .tag = GUI::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangleBaseDescription.dimension.height = GUI_RECTANGLE_BASE_HEIGHT;
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
    .tag = GUI::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangleBaseDescription.dimension.height = GUI_RECTANGLE_BASE_HEIGHT;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getVisiblePartHeight(), Eq(EXPECTED_VISIBLE_PART_HEIGHT));
}

TEST_F(AGUIRectangleBase, GetVisiblePartDimensionReturnsWidthAndHeightOfVisiblePartPackedInDimensionStructure)
{
  constexpr uint16_t GUI_RECTANGLE_BASE_WIDTH  = 40u;
  constexpr uint16_t GUI_RECTANGLE_BASE_HEIGHT = 40u;
  guiRectangleBaseDescription.position =
  {
    .x   = -static_cast<int16_t>(GUI_RECTANGLE_BASE_WIDTH / 2u),
    .y   = static_cast<int16_t>(guiRectangleFrameBuffer.getHeight() - (GUI_RECTANGLE_BASE_HEIGHT / 2u)),
    .tag = GUI::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangleBaseDescription.dimension.height = GUI_RECTANGLE_BASE_WIDTH;
  guiRectangleBaseDescription.dimension.height = GUI_RECTANGLE_BASE_HEIGHT;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getVisiblePartDimension().width, Eq(guiRectangleBase.getVisiblePartWidth()));
  ASSERT_THAT(guiRectangleBase.getVisiblePartDimension().height, Eq(guiRectangleBase.getVisiblePartHeight()));
}

TEST_F(AGUIRectangleBase, GetVisiblePartAreaReturnsVisiblePartWidthMultipliedByVisiblePartHeight)
{
  guiRectangleBase.init(guiRectangleBaseDescription);
  const uint64_t expectedVisiblePartArea =
    static_cast<uint64_t>(guiRectangleBase.getVisiblePartWidth()) *
    static_cast<uint64_t>(guiRectangleBase.getVisiblePartHeight());

  ASSERT_THAT(guiRectangleBase.getVisiblePartArea(), Eq(expectedVisiblePartArea));
}

TEST_F(AGUIRectangleBase, GetVisiblePartPositionWithAnyTagReturnsThatPositionIfItIsVisibleInTheFrameBuffer)
{
  constexpr GUI::Position EXPECTED_GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = 30,
    .y   = 40,
    .tag = GUI::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangleBaseDescription.position = EXPECTED_GUI_RECTANGLE_BASE_POSITION;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getVisiblePartPosition(GUI::Position::Tag::TOP_LEFT_CORNER),
    Eq(EXPECTED_GUI_RECTANGLE_BASE_POSITION));
}

TEST_F(AGUIRectangleBase, GetVisiblePartPositionWithAnyTagReturnsXPositionEqualToZeroIfItIsOutOfScreenFromLeftSide)
{
  constexpr GUI::Position GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = -30,
    .y   = 40,
    .tag = GUI::Position::Tag::TOP_RIGHT_CORNER
  };
  constexpr GUI::Position EXPECTED_GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = 0,
    .y   = 40,
    .tag = GUI::Position::Tag::TOP_RIGHT_CORNER
  };
  guiRectangleBaseDescription.position = GUI_RECTANGLE_BASE_POSITION;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getVisiblePartPosition(GUI::Position::Tag::TOP_RIGHT_CORNER),
    Eq(EXPECTED_GUI_RECTANGLE_BASE_POSITION));
}

TEST_F(AGUIRectangleBase, GetVisiblePartPositionWithAnyTagReturnsXPositionSetToLastColumnInFrameBufferIfItIsOutOfScreenFromRightSide)
{
  constexpr GUI::Position GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = 500,
    .y   = 10,
    .tag = GUI::Position::Tag::BOTTOM_LEFT_CORNER
  };
  const GUI::Position EXPECTED_GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = static_cast<int16_t>(guiRectangleFrameBuffer.getWidth() - 1u),
    .y   = 10,
    .tag = GUI::Position::Tag::BOTTOM_LEFT_CORNER
  };

  guiRectangleBaseDescription.position = GUI_RECTANGLE_BASE_POSITION;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getVisiblePartPosition(GUI::Position::Tag::BOTTOM_LEFT_CORNER),
    Eq(EXPECTED_GUI_RECTANGLE_BASE_POSITION));
}

TEST_F(AGUIRectangleBase, GetVisiblePartPositionWithAnyTagReturnsYPositionEqualToZeroIfItIsOutOfScreenFromAbove)
{
  constexpr GUI::Position GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = 35,
    .y   = -100,
    .tag = GUI::Position::Tag::BOTTOM_RIGHT_CORNER
  };
  constexpr GUI::Position EXPECTED_GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = 35,
    .y   = 0,
    .tag = GUI::Position::Tag::BOTTOM_RIGHT_CORNER
  };
  guiRectangleBaseDescription.position = GUI_RECTANGLE_BASE_POSITION;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getVisiblePartPosition(GUI::Position::Tag::BOTTOM_RIGHT_CORNER),
    Eq(EXPECTED_GUI_RECTANGLE_BASE_POSITION));
}

TEST_F(AGUIRectangleBase, GetVisiblePartPositionWithAnyTagReturnsYPositionSetToLastRowInFrameBufferIfItIsOutOfScreenFromBelow)
{
  constexpr GUI::Position GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = 0,
    .y   = 500,
    .tag = GUI::Position::Tag::CENTER
  };
  const GUI::Position EXPECTED_GUI_RECTANGLE_BASE_POSITION =
  {
    .x   = 0,
    .y   = static_cast<int16_t>(guiRectangleFrameBuffer.getHeight() - 1u),
    .tag = GUI::Position::Tag::CENTER
  };
  guiRectangleBaseDescription.position = GUI_RECTANGLE_BASE_POSITION;
  guiRectangleBase.init(guiRectangleBaseDescription);

  ASSERT_THAT(guiRectangleBase.getVisiblePartPosition(GUI::Position::Tag::CENTER),
    Eq(EXPECTED_GUI_RECTANGLE_BASE_POSITION));
}

TEST_F(AGUIRectangleBase, DrawWithCPUDrawHardwareCallsDrawCPUMethod)
{
  guiRectangleBase.init(guiRectangleBaseDescription);
  EXPECT_CALL(guiRectangleBase, drawCPU())
    .Times(1u);

  guiRectangleBase.draw(GUI::DrawHardware::CPU);
}

TEST_F(AGUIRectangleBase, DrawWithDMA2DDrawHardwareCallsDrawDMA2DMethod)
{
  guiRectangleBase.init(guiRectangleBaseDescription);
  EXPECT_CALL(guiRectangleBase, drawDMA2D())
    .Times(1u);

  guiRectangleBase.draw(GUI::DrawHardware::DMA2D);
}

TEST_F(AGUIRectangleBase, DrawWithCPUDrawHardwareDoesNotCallDrawDMA2DMethod)
{
  guiRectangleBase.init(guiRectangleBaseDescription);
  EXPECT_CALL(guiRectangleBase, drawDMA2D())
    .Times(0u);

  guiRectangleBase.draw(GUI::DrawHardware::CPU);
}

TEST_F(AGUIRectangleBase, DrawWithDMA2DDrawHardwareDoesNotCallDrawCPUMethod)
{
  guiRectangleBase.init(guiRectangleBaseDescription);
  EXPECT_CALL(guiRectangleBase, drawCPU())
    .Times(0u);

  guiRectangleBase.draw(GUI::DrawHardware::DMA2D);
}

TEST_F(AGUIRectangleBase, DrawWithCPUDrawHardwarDoesNotCallDrawCPUMethodIfRectangleIsCompletelyOutOfTheScreen)
{
  guiRectangleBaseDescription.position = GUI_RECTANGLE_COMPLETELY_OUT_OF_SCREEN_POSITION;
  guiRectangleBase.init(guiRectangleBaseDescription);
  EXPECT_CALL(guiRectangleBase, drawCPU())
    .Times(0u);

  guiRectangleBase.draw(GUI::DrawHardware::CPU);
}

TEST_F(AGUIRectangleBase, DrawWithDMA2DDrawHardwarDoesNotCallDrawDMA2DMethodIfRectangleIsCompletelyOutOfTheScreen)
{
  guiRectangleBaseDescription.position = GUI_RECTANGLE_COMPLETELY_OUT_OF_SCREEN_POSITION;
  guiRectangleBase.init(guiRectangleBaseDescription);
  EXPECT_CALL(guiRectangleBase, drawDMA2D())
    .Times(0u);

  guiRectangleBase.draw(GUI::DrawHardware::DMA2D);
}

TEST_F(AGUIRectangleBase, DrawWithCPUDrawHardwareCallsSysTickGetTicksBeforeDrawCPUMethodIsCalled)
{
  guiRectangleBase.init(guiRectangleBaseDescription);
  {
    InSequence expectCallsInSequence;

    EXPECT_CALL(sysTickMock, getTicks())
      .Times(1u);
    EXPECT_CALL(guiRectangleBase, drawCPU())
      .Times(1u);
  }

  guiRectangleBase.draw(GUI::DrawHardware::CPU);
}

TEST_F(AGUIRectangleBase, DrawWithDMA2DDrawHardwareCallsSysTickGetTicksBeforeDrawDMA2DMethodIsCalled)
{
  guiRectangleBase.init(guiRectangleBaseDescription);
  {
    InSequence expectCallsInSequence;

    EXPECT_CALL(sysTickMock, getTicks())
      .Times(1u);
    EXPECT_CALL(guiRectangleBase, drawDMA2D())
      .Times(1u);
  }

  guiRectangleBase.draw(GUI::DrawHardware::DMA2D);
}

TEST_F(AGUIRectangleBase, DrawWithCPUDrawHardwareCallsSysTickGetElapsedTimeInUsAfterDrawCPUMethodIsCalled)
{
  guiRectangleBase.init(guiRectangleBaseDescription);
  {
    InSequence expectCallsInSequence;

    EXPECT_CALL(guiRectangleBase, drawCPU())
      .Times(1u);
    EXPECT_CALL(sysTickMock, getElapsedTimeInUs(_))
      .Times(1u);
  }

  guiRectangleBase.draw(GUI::DrawHardware::CPU);
}

TEST_F(AGUIRectangleBase, GetDrawingTimeFailsIfCalledBeforeRectangleIsDrawnAtTheLeastOnceWithTheGivenDrawHardware)
{
  uint64_t drawingTimeInUs;
  GUI::ErrorCode errorCode = guiRectangleBase.getDrawingTime(GUI::DrawHardware::DMA2D, drawingTimeInUs);

  ASSERT_THAT(errorCode, Eq(GUI::ErrorCode::MEASUREMENT_NOT_AVAILABLE));
}

TEST_F(AGUIRectangleBase, GetDrawingTimeSucceedIfDrawWithTheGivenDrawHardwareIsCalledAtTheLeastOnce)
{
  guiRectangleBase.init(guiRectangleBaseDescription);
  guiRectangleBase.draw(GUI::DrawHardware::CPU);

  uint64_t drawingTimeInUs;
  GUI::ErrorCode errorCode = guiRectangleBase.getDrawingTime(GUI::DrawHardware::CPU, drawingTimeInUs);

  ASSERT_THAT(errorCode, Eq(GUI::ErrorCode::OK));
}

TEST_F(AGUIRectangleBase, GetDrawingTimeForCPUDrawHardwareGetsTheDurationOfTheLastDrawingIfRectangleIsNotMoved)
{
  setupSysTickReadings(DRAW_OPERATION_DURATION_IN_US);
  guiRectangleBase.init(guiRectangleBaseDescription);
  guiRectangleBase.draw(GUI::DrawHardware::CPU);

  uint64_t drawingTimeInUs = 0u;
  GUI::ErrorCode errorCode = guiRectangleBase.getDrawingTime(GUI::DrawHardware::CPU, drawingTimeInUs);

  ASSERT_THAT(drawingTimeInUs, DRAW_OPERATION_DURATION_IN_US);
  ASSERT_THAT(errorCode, Eq(GUI::ErrorCode::OK));
}

TEST_F(AGUIRectangleBase, GetDrawingTimeForCPUDrawHardwareScalesTheLastDrawingTimeIfValueOfVisiblePartAreaHasBeenChanged)
{
  setupSysTickReadings(DRAW_OPERATION_DURATION_IN_US);
  guiRectangleBase.init(guiRectangleBaseDescription);
  const uint64_t oldVisiblePartArea = guiRectangleBase.getVisiblePartArea();
  guiRectangleBase.draw(GUI::DrawHardware::CPU);
  guiRectangleBase.moveToPosition(GUI_RECTANGLE_PARTIALLY_OUT_OF_SCREEN_POSITION);
  const uint64_t newVisiblePartArea = guiRectangleBase.getVisiblePartArea();

  uint64_t drawingTimeInUs = 0u;
  GUI::ErrorCode errorCode = guiRectangleBase.getDrawingTime(GUI::DrawHardware::CPU, drawingTimeInUs);

  ASSERT_THAT(drawingTimeInUs, DRAW_OPERATION_DURATION_IN_US * newVisiblePartArea / oldVisiblePartArea);
  ASSERT_THAT(errorCode, Eq(GUI::ErrorCode::OK));
}

TEST_F(AGUIRectangleBase, DMA2DDrawCompletedCallbackWillNotCauseFaultIfNullptrIsProvided)
{
  GUI::RectangleBase::callbackDMA2DDrawCompleted(nullptr);

  SUCCEED();
}

TEST_F(AGUIRectangleBase, DMA2DDrawCompletedCallbackCallsSysTickGetElapsedTimeInUs)
{
  EXPECT_CALL(sysTickMock, getElapsedTimeInUs(_))
    .Times(1u);

  GUI::RectangleBase::callbackDMA2DDrawCompleted(&guiRectangleBase);
}

TEST_F(AGUIRectangleBase, DrawWithDMA2DIsNotFinishedIfDMA2DDrawCompleteCallbackIsNotCalled)
{
  guiRectangleBase.init(guiRectangleBaseDescription);
  guiRectangleBase.draw(GUI::DrawHardware::DMA2D);

  uint64_t drawingTimeInUs;
  GUI::ErrorCode errorCode = guiRectangleBase.getDrawingTime(GUI::DrawHardware::DMA2D, drawingTimeInUs);

  ASSERT_THAT(errorCode, Eq(GUI::ErrorCode::MEASUREMENT_NOT_AVAILABLE));
}

TEST_F(AGUIRectangleBase, GetDrawingTimeForDMA2DGetsElapsedTimeBetweenDMA2DTransactionIsStartedAndDrawCompletedCallbackIsCalled)
{
  setupSysTickReadings(DRAW_OPERATION_DURATION_IN_US);
  guiRectangleBase.init(guiRectangleBaseDescription);
  guiRectangleBase.draw(GUI::DrawHardware::DMA2D);
  GUI::RectangleBase::callbackDMA2DDrawCompleted(&guiRectangleBase);

  uint64_t drawingTimeInUs = 0u;
  GUI::ErrorCode errorCode = guiRectangleBase.getDrawingTime(GUI::DrawHardware::DMA2D, drawingTimeInUs);

  ASSERT_THAT(drawingTimeInUs, DRAW_OPERATION_DURATION_IN_US);
  ASSERT_THAT(errorCode, Eq(GUI::ErrorCode::OK));
}

TEST_F(AGUIRectangleBase, GetDrawingTimeForDMA2DDrawHardwareScalesTheLastDrawingTimeIfValueOfVisiblePartAreaHasBeenChanged)
{
  setupSysTickReadings(DRAW_OPERATION_DURATION_IN_US);
  guiRectangleBase.init(guiRectangleBaseDescription);
  const uint64_t oldVisiblePartArea = guiRectangleBase.getVisiblePartArea();
  guiRectangleBase.draw(GUI::DrawHardware::DMA2D);
  GUI::RectangleBase::callbackDMA2DDrawCompleted(&guiRectangleBase);
  guiRectangleBase.moveToPosition(GUI_RECTANGLE_PARTIALLY_OUT_OF_SCREEN_POSITION);
  const uint64_t newVisiblePartArea = guiRectangleBase.getVisiblePartArea();

  uint64_t drawingTimeInUs = 0u;
  GUI::ErrorCode errorCode = guiRectangleBase.getDrawingTime(GUI::DrawHardware::DMA2D, drawingTimeInUs);

  ASSERT_THAT(drawingTimeInUs, DRAW_OPERATION_DURATION_IN_US * newVisiblePartArea / oldVisiblePartArea);
  ASSERT_THAT(errorCode, Eq(GUI::ErrorCode::OK));
}

TEST_F(AGUIRectangleBase, IsDrawCompletedReturnsImmediatelyTrueAfterDrawingWithCPU)
{
  guiRectangleBase.init(guiRectangleBaseDescription);
  guiRectangleBase.draw(GUI::DrawHardware::CPU);

  ASSERT_THAT(guiRectangleBase.isDrawCompleted(), Eq(true));
}

TEST_F(AGUIRectangleBase, IsDrawCompletedReturnsFalseIfDrawingWithDMA2DButDrawCompleteCallbackHasNotBeenYetCalled)
{
  guiRectangleBase.init(guiRectangleBaseDescription);
  guiRectangleBase.draw(GUI::DrawHardware::DMA2D);

  ASSERT_THAT(guiRectangleBase.isDrawCompleted(), Eq(false));
}

TEST_F(AGUIRectangleBase, IsDrawCompletedReturnsTrueWhenDrawingWithDMA2DAndDrawCompleteCallbackWasCalled)
{
  guiRectangleBase.init(guiRectangleBaseDescription);
  guiRectangleBase.draw(GUI::DrawHardware::DMA2D);
  GUI::RectangleBase::callbackDMA2DDrawCompleted(&guiRectangleBase);

  ASSERT_THAT(guiRectangleBase.isDrawCompleted(), Eq(true));
}

TEST_F(AGUIRectangleBase, IsDrawCompletedReturnsImmediatelyTrueAfterDrawingWithDMA2DIfRectangleIsCompletelyOutOfTheScreen)
{
  guiRectangleBaseDescription.position = GUI_RECTANGLE_COMPLETELY_OUT_OF_SCREEN_POSITION;
  guiRectangleBase.init(guiRectangleBaseDescription);
  guiRectangleBase.draw(GUI::DrawHardware::DMA2D);

  ASSERT_THAT(guiRectangleBase.isDrawCompleted(), Eq(true));
}

TEST_F(AGUIRectangleBase, DrawWithDMA2DDoesNotDirectlyCallRegisteredCallback)
{
  guiRectangleBase.init(guiRectangleBaseDescription);
  guiRectangleBase.registerDrawCompletedCallback(callbackDescription);

  guiRectangleBase.draw(GUI::DrawHardware::DMA2D);

  assertThatCallbackIsNotCalled();
}

TEST_F(AGUIRectangleBase, DMA2DDrawCompletedCallbackDoesNotFailIfDrawCompletedCallbackIsNotRegistered)
{
  guiRectangleBase.init(guiRectangleBaseDescription);

  GUI::RectangleBase::callbackDMA2DDrawCompleted(&guiRectangleBase);

  SUCCEED();
}

TEST_F(AGUIRectangleBase, DMA2DDrawCompletedCallbackCallsRegisteredDrawCompletedCallback)
{
  guiRectangleBase.init(guiRectangleBaseDescription);
  guiRectangleBase.registerDrawCompletedCallback(callbackDescription);

  GUI::RectangleBase::callbackDMA2DDrawCompleted(&guiRectangleBase);

  assertThatCallbackIsCalled();
}

TEST_F(AGUIRectangleBase, DMA2DDrawCompletedCallbackDoesNotCallPreviouslyRegisteredCallbackIfLaterItIsUnregistered)
{
  guiRectangleBase.init(guiRectangleBaseDescription);
  guiRectangleBase.registerDrawCompletedCallback(callbackDescription);
  guiRectangleBase.unregisterDrawCompletedCallback();

  GUI::RectangleBase::callbackDMA2DDrawCompleted(&guiRectangleBase);

  assertThatCallbackIsNotCalled();
}

TEST_F(AGUIRectangleBase, DrawWithCPUDoesNotFailIfDrawCompletedCallbackIsNotRegistered)
{
  guiRectangleBase.init(guiRectangleBaseDescription);

  guiRectangleBase.draw(GUI::DrawHardware::CPU);

  SUCCEED();
}

TEST_F(AGUIRectangleBase, DrawWithCPUCallsRegisteredDrawCompletedCallbackBeforeReturningExecutionToCaller)
{
  guiRectangleBase.init(guiRectangleBaseDescription);
  guiRectangleBase.registerDrawCompletedCallback(callbackDescription);

  guiRectangleBase.draw(GUI::DrawHardware::CPU);

  assertThatCallbackIsCalled();
}

TEST_F(AGUIRectangleBase, DrawWithCPUDoesNotCallPreviouslyRegisteredCallbackIfLaterItIsUnregistered)
{
  guiRectangleBase.init(guiRectangleBaseDescription);
  guiRectangleBase.registerDrawCompletedCallback(callbackDescription);
  guiRectangleBase.unregisterDrawCompletedCallback();

  guiRectangleBase.draw(GUI::DrawHardware::CPU);

  assertThatCallbackIsNotCalled();
}

TEST_F(AGUIRectangleBase, DrawWithCPUCallsRegisteredDrawCompletedCallbackEvenWhenRectangleIsCompletelyOutOfTheScreen)
{
  guiRectangleBaseDescription.position = GUI_RECTANGLE_COMPLETELY_OUT_OF_SCREEN_POSITION;
  guiRectangleBase.init(guiRectangleBaseDescription);
  guiRectangleBase.registerDrawCompletedCallback(callbackDescription);

  guiRectangleBase.draw(GUI::DrawHardware::CPU);

  assertThatCallbackIsCalled();
}

TEST_F(AGUIRectangleBase, DrawWithDMA2DCallsRegisteredDrawCompletedCallbackDirectlyWhenRectangleIsCompletelyOutOfTheScreen)
{
  guiRectangleBaseDescription.position = GUI_RECTANGLE_COMPLETELY_OUT_OF_SCREEN_POSITION;
  guiRectangleBase.init(guiRectangleBaseDescription);
  guiRectangleBase.registerDrawCompletedCallback(callbackDescription);

  guiRectangleBase.draw(GUI::DrawHardware::DMA2D);

  assertThatCallbackIsCalled();
}