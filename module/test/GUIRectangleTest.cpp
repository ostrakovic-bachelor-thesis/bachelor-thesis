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

  void assertThatGUIRectangleIsDrawnOntoFrameBuffer(GUIRectangle &guiRectangle);
  void assertThatPixelsInFrameBufferOutsideDrawnGUIRectangleAreNotChanged(GUIRectangle &guiRectangle);
  void expectThatDMA2DFillRectangleWillBeCalledWithAppropriateConfigParams(const GUIRectangle &guiRectangle);
  void expectThatDMA2DFillRectangleWillDisplayOnlyVisiblePartOfGUIRectangle(const GUIRectangle &guiRectangle);
  void assertThatDMA2DFillRectangleConfigParamsWereOk(void);
  void expectThatDMA2DFillRectangleWillNotBeCalled(void);

  void setDefaultFrameBufferColor(GUIRectangle::Color color);

  void SetUp() override;
};

void AGUIRectangle::SetUp()
{
  guiRectangleDescription.baseDescription.dimension =
  {
    .width  = 40,
    .height = 40
  };
  guiRectangleDescription.baseDescription.position =
  {
    .x = 5,
    .y = 5
  };

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

void AGUIRectangle::assertThatGUIRectangleIsDrawnOntoFrameBuffer(GUIRectangle &guiRectangle)
{
  const uint16_t columnStart = guiRectangle.getVisiblePartPosition(GUIRectangle::Position::Tag::TOP_LEFT_CORNER).x;
  const uint16_t rowStart    = guiRectangle.getVisiblePartPosition(GUIRectangle::Position::Tag::TOP_LEFT_CORNER).y;
  const uint16_t columnEnd = guiRectangle.getVisiblePartPosition(GUIRectangle::Position::Tag::BOTTOM_RIGHT_CORNER).x;
  const uint16_t rowEnd    = guiRectangle.getVisiblePartPosition(GUIRectangle::Position::Tag::BOTTOM_RIGHT_CORNER).y;

  const uint8_t *frameBufferPtr  = reinterpret_cast<uint8_t*>(guiRectangle.getFrameBuffer().getPointer());
  const uint32_t frameBufferWidth = guiRectangle.getFrameBuffer().getWidth();
  const uint8_t pixelSize = IFrameBuffer::getColorFormatPixelSize(guiRectangle.getFrameBuffer().getColorFormat());

  for (uint32_t row = rowStart; row < rowEnd; ++row)
  {
    for (uint32_t column = columnStart; column < columnEnd; ++column)
    {
      ASSERT_THAT(frameBufferPtr[pixelSize * (row * frameBufferWidth + column)],     guiRectangle.getColor().blue);
      ASSERT_THAT(frameBufferPtr[pixelSize * (row * frameBufferWidth + column) + 1], guiRectangle.getColor().green);
      ASSERT_THAT(frameBufferPtr[pixelSize * (row * frameBufferWidth + column) + 2], guiRectangle.getColor().red);
    }
  }
}

void AGUIRectangle::assertThatPixelsInFrameBufferOutsideDrawnGUIRectangleAreNotChanged(GUIRectangle &guiRectangle)
{
  const uint16_t columnStart = guiRectangle.getVisiblePartPosition(GUIRectangle::Position::Tag::TOP_LEFT_CORNER).x;
  const uint16_t rowStart    = guiRectangle.getVisiblePartPosition(GUIRectangle::Position::Tag::TOP_LEFT_CORNER).y;
  const uint16_t columnEnd = guiRectangle.getVisiblePartPosition(GUIRectangle::Position::Tag::BOTTOM_RIGHT_CORNER).x;
  const uint16_t rowEnd    = guiRectangle.getVisiblePartPosition(GUIRectangle::Position::Tag::BOTTOM_RIGHT_CORNER).y;

  const uint8_t *frameBufferPtr  = reinterpret_cast<uint8_t*>(guiRectangle.getFrameBuffer().getPointer());
  const uint32_t frameBufferWidth = guiRectangle.getFrameBuffer().getWidth();
  const uint8_t pixelSize = IFrameBuffer::getColorFormatPixelSize(guiRectangle.getFrameBuffer().getColorFormat());

  for (uint32_t row = 0u; row < guiRectangle.getFrameBuffer().getHeight(); ++row)
  {
    for (uint32_t column = 0u; column < guiRectangle.getFrameBuffer().getWidth(); ++column)
    {
      if ((rowStart <= row) && (rowEnd >= row) & (columnStart == column))
      {
        column = columnEnd;
      }
      else
      {
        ASSERT_THAT(frameBufferPtr[pixelSize * (row * frameBufferWidth + column)],     m_initFameBufferColor.blue);
        ASSERT_THAT(frameBufferPtr[pixelSize * (row * frameBufferWidth + column) + 1], m_initFameBufferColor.green);
        ASSERT_THAT(frameBufferPtr[pixelSize * (row * frameBufferWidth + column) + 2], m_initFameBufferColor.red);
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
      m_isDMA2DFillRectangleConfigOk &= (fillRectangleConfig.color.alpha == 0u);
      m_isDMA2DFillRectangleConfigOk &= (fillRectangleConfig.color.red   == guiRectangle.getColor().red);
      m_isDMA2DFillRectangleConfigOk &= (fillRectangleConfig.color.green == guiRectangle.getColor().green);
      m_isDMA2DFillRectangleConfigOk &= (fillRectangleConfig.color.blue  == guiRectangle.getColor().blue);

      m_isDMA2DFillRectangleConfigOk &= (fillRectangleConfig.dimension.width == guiRectangle.getWidth());
      m_isDMA2DFillRectangleConfigOk &= (fillRectangleConfig.dimension.height == guiRectangle.getHeight());

      m_isDMA2DFillRectangleConfigOk &= (fillRectangleConfig.position.x == guiRectangle.getPosition(GUIRectangle::Position::Tag::TOP_LEFT_CORNER).x);
      m_isDMA2DFillRectangleConfigOk &= (fillRectangleConfig.position.y == guiRectangle.getPosition(GUIRectangle::Position::Tag::TOP_LEFT_CORNER).y);

      m_isDMA2DFillRectangleConfigOk &=
        (fillRectangleConfig.destinationBufferConfig.colorFormat == DMA2D::OutputColorFormat::RGB888);
      m_isDMA2DFillRectangleConfigOk &=
        (fillRectangleConfig.destinationBufferConfig.bufferDimension.width == guiRectangle.getFrameBuffer().getWidth());
      m_isDMA2DFillRectangleConfigOk &=
        (fillRectangleConfig.destinationBufferConfig.bufferDimension.height == guiRectangle.getFrameBuffer().getHeight());
      m_isDMA2DFillRectangleConfigOk &=
        (fillRectangleConfig.destinationBufferConfig.bufferPtr == guiRectangle.getFrameBuffer().getPointer());

      return DMA2D::ErrorCode::OK;
    });
}

void
AGUIRectangle::expectThatDMA2DFillRectangleWillDisplayOnlyVisiblePartOfGUIRectangle(const GUIRectangle &guiRectangle)
{
  EXPECT_CALL(dma2dMock, fillRectangle(_))
    .Times(1u)
    .WillOnce([=](const DMA2D::FillRectangleConfig &fillRectangleConfig) -> DMA2D::ErrorCode
    {
      const uint16_t expectedWidth  = guiRectangle.getVisiblePartWidth();
      const uint16_t expectedHeight = guiRectangle.getVisiblePartHeight();
      const int16_t expectedXPos = guiRectangle.getVisiblePartPosition(GUIRectangle::Position::Tag::TOP_LEFT_CORNER).x;
      const int16_t expectedYPos = guiRectangle.getVisiblePartPosition(GUIRectangle::Position::Tag::TOP_LEFT_CORNER).y;

      m_isDMA2DFillRectangleConfigOk = (fillRectangleConfig.dimension.width == expectedWidth);
      m_isDMA2DFillRectangleConfigOk = (fillRectangleConfig.dimension.height == expectedHeight);

      m_isDMA2DFillRectangleConfigOk = (fillRectangleConfig.position.x == expectedXPos);
      m_isDMA2DFillRectangleConfigOk = (fillRectangleConfig.position.y == expectedYPos);

      return DMA2D::ErrorCode::OK;
    });
}

void AGUIRectangle::expectThatDMA2DFillRectangleWillNotBeCalled(void)
{
  EXPECT_CALL(dma2dMock, fillRectangle(_))
    .Times(0u);
}

void AGUIRectangle::assertThatDMA2DFillRectangleConfigParamsWereOk(void)
{
  ASSERT_THAT(m_isDMA2DFillRectangleConfigOk, Eq(true));
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

TEST_F(AGUIRectangle, DrawWithCPUDrawsRectangleOnAssociatedFrameBuffer)
{
  guiRectangleDescription.baseDescription.position =
  {
    .x   = 10,
    .y   = 10,
    .tag = GUIRectangle::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangleDescription.baseDescription.dimension =
  {
    .width  = 20u,
    .height = 20u
  };
  guiRectangleDescription.color =
  {
    .red   = 100u,
    .green = 200u,
    .blue  = 100u
  };
  guiRectangle.init(guiRectangleDescription);

  guiRectangle.draw(IGUIObject::DrawHardware::CPU);

  assertThatGUIRectangleIsDrawnOntoFrameBuffer(guiRectangle);
}

TEST_F(AGUIRectangle, DrawWithCPUDoesNotChangeValueOfAnyPixelOutsideGivenRectangle)
{
  guiRectangleDescription.baseDescription.position =
  {
    .x   = 15,
    .y   = 10,
    .tag = GUIRectangle::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangleDescription.baseDescription.dimension =
  {
    .width  = 15u,
    .height = 25u
  };
  guiRectangleDescription.color =
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
  guiRectangleDescription.baseDescription.position =
  {
    .x   = 0,
    .y   = 0,
    .tag = GUIRectangle::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangleDescription.baseDescription.dimension =
  {
    .width  = 45u,
    .height = 10u
  };
  guiRectangleDescription.color =
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

TEST_F(AGUIRectangle, DrawWithDMA2DDisplaysOnlyVisiblePartOfRectangleIfAPartOfIsOutOfTheScreenAkaFrameBuffer)
{
  guiRectangleDescription.baseDescription.position =
  {
    .x   = -10,
    .y   = -10,
    .tag = GUIRectangle::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangleDescription.baseDescription.dimension =
  {
    .width  = 45u,
    .height = 55u
  };
  guiRectangle.init(guiRectangleDescription);
  expectThatDMA2DFillRectangleWillDisplayOnlyVisiblePartOfGUIRectangle(guiRectangle);

  guiRectangle.draw(IGUIObject::DrawHardware::DMA2D);

  assertThatDMA2DFillRectangleConfigParamsWereOk();
}

TEST_F(AGUIRectangle, DrawWithDMA2DDisplaysOnlyVisiblePartOfRectangleIfItIsMovedToBePartiallyOutOfTheScreen)
{
  constexpr GUIRectangle::Position NEW_GUI_RECTANGLE_POSITION =
  {
    .x   = 40,
    .y   = 40,
    .tag = GUIRectangle::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangle.init(guiRectangleDescription);
  guiRectangle.moveToPosition(NEW_GUI_RECTANGLE_POSITION);
  expectThatDMA2DFillRectangleWillDisplayOnlyVisiblePartOfGUIRectangle(guiRectangle);

  guiRectangle.draw(IGUIObject::DrawHardware::DMA2D);

  assertThatDMA2DFillRectangleConfigParamsWereOk();
}

TEST_F(AGUIRectangle, DrawWithCPUDisplaysOnlyVisiblePartOfRectangleIfItIsMovedToBePartiallyOutOfTheScreen)
{
  constexpr GUIRectangle::Position NEW_GUI_RECTANGLE_POSITION =
  {
    .x   = -20,
    .y   = -20,
    .tag = GUIRectangle::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangle.init(guiRectangleDescription);
  guiRectangle.moveToPosition(NEW_GUI_RECTANGLE_POSITION);

  guiRectangle.draw(IGUIObject::DrawHardware::CPU);

  assertThatGUIRectangleIsDrawnOntoFrameBuffer(guiRectangle);
}

TEST_F(AGUIRectangle, DrawWithDMA2DDoesNotTriggerDMA2DFillRectangleOperationIfGUIRectangleIsCompletelyOutOfTheScreen)
{
  guiRectangleDescription.baseDescription.position =
  {
    .x   = -400,
    .y   = 40,
    .tag = GUIRectangle::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangle.init(guiRectangleDescription);
  expectThatDMA2DFillRectangleWillNotBeCalled();

  guiRectangle.draw(IGUIObject::DrawHardware::DMA2D);
}