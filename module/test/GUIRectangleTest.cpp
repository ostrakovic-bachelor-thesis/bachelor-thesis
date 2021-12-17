#include "GUIRectangle.h"
#include "FrameBuffer.h"
#include "DMA2DMock.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


class AGUIRectangle : public Test
{
public:
  DMA2DMock dma2dMock;
  FrameBuffer<50u, 50u, IFrameBuffer::ColorFormat::RGB888> guiRectangleFrameBuffer;
  GUIRectangle guiRectangle = GUIRectangle(dma2dMock, guiRectangleFrameBuffer);
  GUIRectangle::RectangleDescription guiRectangleDescription;

  GUIRectangle::Color m_initFameBufferColor;
  bool m_isDMA2DFillRectangleConfigOk;

  void assertThatGUIRectangleIsDrawnInFrameBuffer(GUIRectangle &guiRectangle);
  void assertThatPixelsInFrameBufferOutsideDrawnGUIRectangleAreNotChanged(GUIRectangle &guiRectangle);
  void expectThatDMA2DFillRectangleWillBeCalledWithAppropriateConfigParams(const GUIRectangle &guiRectangle);
  void assertThatDMA2DFillRectangleConfigParamsWereOk(void);

  void setDefaultFrameBufferColor(GUIRectangle::Color color);

  void SetUp() override;
};

void AGUIRectangle::SetUp()
{
  m_initFameBufferColor.red   = 0u;
  m_initFameBufferColor.green = 0u;
  m_initFameBufferColor.blue  = 0u;

  m_isDMA2DFillRectangleConfigOk = true;

  setDefaultFrameBufferColor(m_initFameBufferColor);
}

void AGUIRectangle::setDefaultFrameBufferColor(GUIRectangle::Color color)
{
  uint8_t *frameBufferPtr = reinterpret_cast<uint8_t*>(guiRectangle.getFrameBuffer().getPointer());

  for (uint32_t i = 0u; i < guiRectangleFrameBuffer.getSize();)
  {
    frameBufferPtr[i++] = color.blue;
    frameBufferPtr[i++] = color.green;
    frameBufferPtr[i++] = color.red;
  }
}

void AGUIRectangle::assertThatGUIRectangleIsDrawnInFrameBuffer(GUIRectangle &guiRectangle)
{
  const uint16_t columnStart = guiRectangle.getPosition().x;
  const uint16_t rowStart    = guiRectangle.getPosition().y;
  uint16_t columnEnd = guiRectangle.getPosition().x + guiRectangle.getWidth();
  uint16_t rowEnd    = guiRectangle.getPosition().y + guiRectangle.getHeight();

  if (guiRectangle.getFrameBuffer().getWidth() < columnEnd)
  {
    columnEnd = guiRectangle.getFrameBuffer().getWidth();
  }

  if (guiRectangle.getFrameBuffer().getHeight() < rowEnd)
  {
    rowEnd = guiRectangle.getFrameBuffer().getHeight();
  }

  const uint8_t *frameBufferPtr  = reinterpret_cast<uint8_t*>(guiRectangle.getFrameBuffer().getPointer());
  const uint32_t frameBufferWidth = guiRectangle.getFrameBuffer().getWidth();
  const uint8_t pixelSize = IFrameBuffer::getColorFormatPixelSize(guiRectangle.getFrameBuffer().getColorFormat());

  for (uint32_t row = rowStart; row < rowEnd; ++row)
  {
    for (uint32_t column = columnStart; column < columnEnd; ++column)
    {
      ASSERT_THAT(frameBufferPtr[pixelSize * (row * frameBufferWidth + column)],     guiRectangle.getColor().red);
      ASSERT_THAT(frameBufferPtr[pixelSize * (row * frameBufferWidth + column) + 1], guiRectangle.getColor().green);
      ASSERT_THAT(frameBufferPtr[pixelSize * (row * frameBufferWidth + column) + 2], guiRectangle.getColor().blue);
    }
  }
}

void AGUIRectangle::assertThatPixelsInFrameBufferOutsideDrawnGUIRectangleAreNotChanged(GUIRectangle &guiRectangle)
{
  const uint16_t columnStart = guiRectangle.getPosition().x;
  const uint16_t rowStart    = guiRectangle.getPosition().y;
  uint16_t columnEnd = guiRectangle.getPosition().x + guiRectangle.getWidth();
  uint16_t rowEnd    = guiRectangle.getPosition().y + guiRectangle.getHeight();

  if (guiRectangle.getFrameBuffer().getWidth() < columnEnd)
  {
    columnEnd = guiRectangle.getFrameBuffer().getWidth();
  }

  if (guiRectangle.getFrameBuffer().getHeight() < rowEnd)
  {
    rowEnd = guiRectangle.getFrameBuffer().getHeight();
  }

  const uint8_t *frameBufferPtr  = reinterpret_cast<uint8_t*>(guiRectangle.getFrameBuffer().getPointer());
  const uint32_t frameBufferWidth = guiRectangle.getFrameBuffer().getWidth();
  const uint8_t pixelSize = IFrameBuffer::getColorFormatPixelSize(guiRectangle.getFrameBuffer().getColorFormat());


  for (uint32_t row = 0u; row < guiRectangle.getFrameBuffer().getHeight(); ++row)
  {
    for (uint32_t column = 0u; column < guiRectangle.getFrameBuffer().getWidth(); ++column)
    {
      if ((rowStart <= row) && (rowEnd > row) & (columnStart == column))
      {
        column = columnEnd - 1u;
      }
      else
      {
        ASSERT_THAT(frameBufferPtr[pixelSize * (row * frameBufferWidth + column)],     m_initFameBufferColor.red);
        ASSERT_THAT(frameBufferPtr[pixelSize * (row * frameBufferWidth + column) + 1], m_initFameBufferColor.green);
        ASSERT_THAT(frameBufferPtr[pixelSize * (row * frameBufferWidth + column) + 2], m_initFameBufferColor.blue);
      }
    }
  }
}

void
AGUIRectangle::expectThatDMA2DFillRectangleWillBeCalledWithAppropriateConfigParams(const GUIRectangle &guiRectangle)
{
  EXPECT_CALL(dma2dMock, fillRectangle(_))
    .Times(1u)
    .WillOnce([=](const DMA2D::FillRectangleConfig &fillRectangleConfig) -> DMA2D::ErrorCode
    {
      m_isDMA2DFillRectangleConfigOk = (fillRectangleConfig.color.alpha == 0u);
      m_isDMA2DFillRectangleConfigOk = (fillRectangleConfig.color.red   == guiRectangle.getColor().red);
      m_isDMA2DFillRectangleConfigOk = (fillRectangleConfig.color.green == guiRectangle.getColor().green);
      m_isDMA2DFillRectangleConfigOk = (fillRectangleConfig.color.blue  == guiRectangle.getColor().blue);

      m_isDMA2DFillRectangleConfigOk = (fillRectangleConfig.dimension.width == guiRectangle.getWidth());
      m_isDMA2DFillRectangleConfigOk = (fillRectangleConfig.dimension.height == guiRectangle.getHeight());

      m_isDMA2DFillRectangleConfigOk = (fillRectangleConfig.position.x == guiRectangle.getPosition().x);
      m_isDMA2DFillRectangleConfigOk = (fillRectangleConfig.position.y == guiRectangle.getPosition().y);

      m_isDMA2DFillRectangleConfigOk =
        (fillRectangleConfig.destinationBufferConfig.colorFormat == DMA2D::OutputColorFormat::RGB888);
      m_isDMA2DFillRectangleConfigOk =
        (fillRectangleConfig.destinationBufferConfig.bufferDimension.width == guiRectangle.getFrameBuffer().getWidth());
      m_isDMA2DFillRectangleConfigOk =
        (fillRectangleConfig.destinationBufferConfig.bufferDimension.height == guiRectangle.getFrameBuffer().getHeight());
      m_isDMA2DFillRectangleConfigOk =
        (fillRectangleConfig.destinationBufferConfig.bufferPtr == guiRectangle.getFrameBuffer().getPointer());

      return DMA2D::ErrorCode::OK;
    });
}

void AGUIRectangle::assertThatDMA2DFillRectangleConfigParamsWereOk(void)
{
  ASSERT_THAT(m_isDMA2DFillRectangleConfigOk, Eq(true));
}


TEST_F(AGUIRectangle, GetWidthReturnsWidthSpecifiedAtInit)
{
  constexpr uint16_t EXPECTED_GUI_RECTANGLE_WIDTH = 100u;
  guiRectangleDescription.width = EXPECTED_GUI_RECTANGLE_WIDTH;
  guiRectangle.init(guiRectangleDescription);

  ASSERT_THAT(guiRectangle.getWidth(), Eq(EXPECTED_GUI_RECTANGLE_WIDTH));
}

TEST_F(AGUIRectangle, GetHeightReturnsHeightSpecifiedAtInit)
{
  constexpr uint16_t EXPECTED_GUI_RECTANGLE_HEIGHT = 200u;
  guiRectangleDescription.height = EXPECTED_GUI_RECTANGLE_HEIGHT;
  guiRectangle.init(guiRectangleDescription);

  ASSERT_THAT(guiRectangle.getHeight(), Eq(EXPECTED_GUI_RECTANGLE_HEIGHT));
}

TEST_F(AGUIRectangle, GetColorReturnsColorSpecifiedAtInit)
{
  constexpr GUIRectangle::Color EXPECTED_GUI_RECTANGLE_COLOR =
  {
    .red   = 255u,
    .green = 192u,
    .blue  = 111u
  };
  guiRectangleDescription.color = EXPECTED_GUI_RECTANGLE_COLOR;
  guiRectangle.init(guiRectangleDescription);

  ASSERT_THAT(guiRectangle.getColor(), Eq(EXPECTED_GUI_RECTANGLE_COLOR));
}

TEST_F(AGUIRectangle, GetPositionReturnsSpecifiedPositionAtInitIfObjectIsNotMoved)
{
  constexpr GUIRectangle::Position EXPECTED_GUI_RECTANGLE_POSITION =
  {
    .x   = 250,
    .y   = -10,
    .tag = GUIRectangle::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangleDescription.position = EXPECTED_GUI_RECTANGLE_POSITION;
  guiRectangle.init(guiRectangleDescription);

  ASSERT_THAT(guiRectangle.getPosition(), Eq(EXPECTED_GUI_RECTANGLE_POSITION));
}

TEST_F(AGUIRectangle, GetPositionTopLeftCornerReturnsCorrectValue)
{
  constexpr uint16_t GUI_RECTANGLE_WIDTH = 20;
  constexpr GUIRectangle::Position GUI_RECTANGLE_POSITION =
  {
    .x   = 49,
    .y   = 50,
    .tag = GUIRectangle::Position::Tag::TOP_RIGHT_CORNER,
  };
  constexpr GUIRectangle::Position EXPECTED_GUI_RECTANGLE_POSITION =
  {
    .x   = GUI_RECTANGLE_POSITION.x - GUI_RECTANGLE_WIDTH + 1,
    .y   = GUI_RECTANGLE_POSITION.y,
    .tag = GUIRectangle::Position::Tag::TOP_LEFT_CORNER,
  };
  guiRectangleDescription.position = GUI_RECTANGLE_POSITION;
  guiRectangleDescription.width    = GUI_RECTANGLE_WIDTH;
  guiRectangle.init(guiRectangleDescription);

  ASSERT_THAT(guiRectangle.getPosition(GUIRectangle::Position::Tag::TOP_LEFT_CORNER),
    Eq(EXPECTED_GUI_RECTANGLE_POSITION));
}

TEST_F(AGUIRectangle, GetPositionTopRightCornerReturnsCorrectValue)
{
  constexpr uint16_t GUI_RECTANGLE_WIDTH  = 20;
  constexpr uint16_t GUI_RECTANGLE_HEIGHT = 20;
  constexpr GUIRectangle::Position GUI_RECTANGLE_POSITION =
  {
    .x   = 100,
    .y   = 99,
    .tag = GUIRectangle::Position::Tag::BOTTOM_LEFT_CORNER,
  };
  constexpr GUIRectangle::Position EXPECTED_GUI_RECTANGLE_POSITION =
  {
    .x   = GUI_RECTANGLE_POSITION.x + GUI_RECTANGLE_WIDTH - 1,
    .y   = GUI_RECTANGLE_POSITION.y - GUI_RECTANGLE_HEIGHT + 1,
    .tag = GUIRectangle::Position::Tag::TOP_RIGHT_CORNER,
  };
  guiRectangleDescription.position = GUI_RECTANGLE_POSITION;
  guiRectangleDescription.width    = GUI_RECTANGLE_WIDTH;
  guiRectangleDescription.height   = GUI_RECTANGLE_HEIGHT;
  guiRectangle.init(guiRectangleDescription);

  ASSERT_THAT(guiRectangle.getPosition(GUIRectangle::Position::Tag::TOP_RIGHT_CORNER),
    Eq(EXPECTED_GUI_RECTANGLE_POSITION));
}

TEST_F(AGUIRectangle, GetPositionBottomLeftCornerReturnsCorrectValue)
{
  constexpr uint16_t GUI_RECTANGLE_WIDTH  = 50;
  constexpr uint16_t GUI_RECTANGLE_HEIGHT = 25;
  constexpr GUIRectangle::Position GUI_RECTANGLE_POSITION =
  {
    .x   = 75,
    .y   = 75,
    .tag = GUIRectangle::Position::Tag::CENTER,
  };
  constexpr GUIRectangle::Position EXPECTED_GUI_RECTANGLE_POSITION =
  {
    .x   = GUI_RECTANGLE_POSITION.x - (GUI_RECTANGLE_WIDTH / 2),
    .y   = GUI_RECTANGLE_POSITION.y - (GUI_RECTANGLE_HEIGHT / 2) + GUI_RECTANGLE_HEIGHT - 1,
    .tag = GUIRectangle::Position::Tag::BOTTOM_LEFT_CORNER,
  };
  guiRectangleDescription.position = GUI_RECTANGLE_POSITION;
  guiRectangleDescription.width    = GUI_RECTANGLE_WIDTH;
  guiRectangleDescription.height   = GUI_RECTANGLE_HEIGHT;
  guiRectangle.init(guiRectangleDescription);

  ASSERT_THAT(guiRectangle.getPosition(GUIRectangle::Position::Tag::BOTTOM_LEFT_CORNER),
    Eq(EXPECTED_GUI_RECTANGLE_POSITION));
}

TEST_F(AGUIRectangle, GetPositionBottomRightCornerReturnsCorrectValue)
{
  constexpr uint16_t GUI_RECTANGLE_WIDTH = 50;
  constexpr GUIRectangle::Position GUI_RECTANGLE_POSITION =
  {
    .x   = 100,
    .y   = 100,
    .tag = GUIRectangle::Position::Tag::BOTTOM_LEFT_CORNER,
  };
  constexpr GUIRectangle::Position EXPECTED_GUI_RECTANGLE_POSITION =
  {
    .x   = GUI_RECTANGLE_POSITION.x + GUI_RECTANGLE_WIDTH - 1,
    .y   = GUI_RECTANGLE_POSITION.y,
    .tag = GUIRectangle::Position::Tag::BOTTOM_RIGHT_CORNER,
  };
  guiRectangleDescription.position = GUI_RECTANGLE_POSITION;
  guiRectangleDescription.width    = GUI_RECTANGLE_WIDTH;
  guiRectangle.init(guiRectangleDescription);

  ASSERT_THAT(guiRectangle.getPosition(GUIRectangle::Position::Tag::BOTTOM_RIGHT_CORNER),
    Eq(EXPECTED_GUI_RECTANGLE_POSITION));
}

TEST_F(AGUIRectangle, GetPositionCenterReturnsCorrectValue)
{
  constexpr uint16_t GUI_RECTANGLE_WIDTH  = 50;
  constexpr uint16_t GUI_RECTANGLE_HEIGHT = 55;
  constexpr GUIRectangle::Position GUI_RECTANGLE_POSITION =
  {
    .x   = 100,
    .y   = 100,
    .tag = GUIRectangle::Position::Tag::TOP_LEFT_CORNER,
  };
  constexpr GUIRectangle::Position EXPECTED_GUI_RECTANGLE_POSITION =
  {
    .x   = GUI_RECTANGLE_POSITION.x + (GUI_RECTANGLE_WIDTH / 2),
    .y   = GUI_RECTANGLE_POSITION.y + (GUI_RECTANGLE_HEIGHT / 2),
    .tag = GUIRectangle::Position::Tag::CENTER,
  };
  guiRectangleDescription.position = GUI_RECTANGLE_POSITION;
  guiRectangleDescription.width    = GUI_RECTANGLE_WIDTH;
  guiRectangleDescription.height   = GUI_RECTANGLE_HEIGHT;
  guiRectangle.init(guiRectangleDescription);

  ASSERT_THAT(guiRectangle.getPosition(GUIRectangle::Position::Tag::CENTER),
    Eq(EXPECTED_GUI_RECTANGLE_POSITION));
}

TEST_F(AGUIRectangle, GetFrameBufferReturnsPointerToTheFrameBufferAssociatedWithGUIRectangle)
{
  FrameBuffer<50, 50, IFrameBuffer::ColorFormat::RGB888> frameBuffer;
  GUIRectangle guiRectangle(dma2dMock, frameBuffer);

  ASSERT_EQ(guiRectangle.getFrameBuffer(), frameBuffer);
}

TEST_F(AGUIRectangle, DrawWithCPUDrawsRectangleOnAssociatedFrameBuffer)
{
  guiRectangleDescription.position =
  {
    .x = 10,
    .y = 10
  };
  guiRectangleDescription.width  = 20u;
  guiRectangleDescription.height = 20u,
  guiRectangleDescription.color  =
  {
    .red   = 100u,
    .green = 200u,
    .blue  = 100u
  };
  guiRectangle.init(guiRectangleDescription);

  guiRectangle.draw(IGUIObject::DrawHardware::CPU);

  assertThatGUIRectangleIsDrawnInFrameBuffer(guiRectangle);
}

TEST_F(AGUIRectangle, DrawWithCPUDoesNotChangeValueOfAnyPixelOutsideGivenRectangle)
{
  guiRectangleDescription.position =
  {
    .x = 15,
    .y = 10
  };
  guiRectangleDescription.width  = 15u;
  guiRectangleDescription.height = 25u,
  guiRectangleDescription.color  =
  {
    .red   = 150u,
    .green = 210u,
    .blue  = 40u
  };
  guiRectangle.init(guiRectangleDescription);

  guiRectangle.draw(IGUIObject::DrawHardware::CPU);

  assertThatPixelsInFrameBufferOutsideDrawnGUIRectangleAreNotChanged(guiRectangle);
}

TEST_F(AGUIRectangle, DrawWithDMA2DTriggersDMA2DFillRectangleOperationWithAppropriateConfigParams)
{
  guiRectangleDescription.position =
  {
    .x = 0,
    .y = 0
  };
  guiRectangleDescription.width  = 45u;
  guiRectangleDescription.height = 10u,
  guiRectangleDescription.color  =
  {
    .red   = 50u,
    .green = 250u,
    .blue  = 30u
  };
  guiRectangle.init(guiRectangleDescription);
  expectThatDMA2DFillRectangleWillBeCalledWithAppropriateConfigParams(guiRectangle);

  guiRectangle.draw(IGUIObject::DrawHardware::DMA2D);

  assertThatDMA2DFillRectangleConfigParamsWereOk();
}

TEST_F(AGUIRectangle, MoveToPositionMovesGUIRectangleToGivenPosition)
{
  constexpr GUIRectangle::Position EXPECTED_GUI_RECTANGLE_POSITION =
  {
    .x = 250,
    .y = 450
  };
  guiRectangle.init(guiRectangleDescription);

  guiRectangle.moveToPosition(EXPECTED_GUI_RECTANGLE_POSITION);

  ASSERT_THAT(guiRectangle.getPosition(), Eq(EXPECTED_GUI_RECTANGLE_POSITION));
}
