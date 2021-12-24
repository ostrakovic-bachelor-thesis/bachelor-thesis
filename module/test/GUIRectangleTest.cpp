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
  void expectThatDMA2DFillRectangleWillDisplayOnlyVisiblePartOfGUIRectangle(const GUIRectangle &guiRectangle);
  void assertThatDMA2DFillRectangleConfigParamsWereOk(void);

  void setDefaultFrameBufferColor(GUIRectangle::Color color);

  static uint16_t getGUIRectangleDisplayWidth(const GUIRectangle &guiRectangle);
  static uint16_t getGUIRectangleDisplayHeight(const GUIRectangle &guiRectangle);
  static int16_t getGUIRectangleDisplayXPos(const GUIRectangle &guiRectangle);
  static int16_t getGUIRectangleDisplayYPos(const GUIRectangle &guiRectangle);

  void SetUp() override;
};

void AGUIRectangle::SetUp()
{
  guiRectangleDescription.baseDescription.width  = 40;
  guiRectangleDescription.baseDescription.height = 40;
  guiRectangleDescription.baseDescription.position.x = 5;
  guiRectangleDescription.baseDescription.position.y = 5;

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
  const uint16_t columnStart = getGUIRectangleDisplayXPos(guiRectangle);
  const uint16_t rowStart    = getGUIRectangleDisplayYPos(guiRectangle);
  const uint16_t columnEnd = getGUIRectangleDisplayXPos(guiRectangle) + getGUIRectangleDisplayWidth(guiRectangle);
  const uint16_t rowEnd    = getGUIRectangleDisplayYPos(guiRectangle) + getGUIRectangleDisplayHeight(guiRectangle);

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
  const uint16_t columnStart = getGUIRectangleDisplayXPos(guiRectangle);
  const uint16_t rowStart    = getGUIRectangleDisplayYPos(guiRectangle);
  const uint16_t columnEnd = getGUIRectangleDisplayXPos(guiRectangle) + getGUIRectangleDisplayWidth(guiRectangle);
  const uint16_t rowEnd    = getGUIRectangleDisplayYPos(guiRectangle) + getGUIRectangleDisplayHeight(guiRectangle);

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
      m_isDMA2DFillRectangleConfigOk = (fillRectangleConfig.color.alpha == 0u);
      m_isDMA2DFillRectangleConfigOk = (fillRectangleConfig.color.red   == guiRectangle.getColor().red);
      m_isDMA2DFillRectangleConfigOk = (fillRectangleConfig.color.green == guiRectangle.getColor().green);
      m_isDMA2DFillRectangleConfigOk = (fillRectangleConfig.color.blue  == guiRectangle.getColor().blue);

      m_isDMA2DFillRectangleConfigOk = (fillRectangleConfig.dimension.width == guiRectangle.getWidth());
      m_isDMA2DFillRectangleConfigOk = (fillRectangleConfig.dimension.height == guiRectangle.getHeight());

      m_isDMA2DFillRectangleConfigOk = (fillRectangleConfig.position.x == guiRectangle.getPosition(GUIRectangle::Position::Tag::TOP_LEFT_CORNER).x);
      m_isDMA2DFillRectangleConfigOk = (fillRectangleConfig.position.y == guiRectangle.getPosition(GUIRectangle::Position::Tag::TOP_LEFT_CORNER).y);

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

void
AGUIRectangle::expectThatDMA2DFillRectangleWillDisplayOnlyVisiblePartOfGUIRectangle(const GUIRectangle &guiRectangle)
{
  EXPECT_CALL(dma2dMock, fillRectangle(_))
    .Times(1u)
    .WillOnce([=](const DMA2D::FillRectangleConfig &fillRectangleConfig) -> DMA2D::ErrorCode
    {
      const uint16_t expectedWidth  = getGUIRectangleDisplayWidth(guiRectangle);
      const uint16_t expectedHeight = getGUIRectangleDisplayHeight(guiRectangle);
      const int16_t expectedXPos = getGUIRectangleDisplayXPos(guiRectangle);
      const int16_t expectedYPos = getGUIRectangleDisplayYPos(guiRectangle);

      m_isDMA2DFillRectangleConfigOk = (fillRectangleConfig.dimension.width == expectedWidth);
      m_isDMA2DFillRectangleConfigOk = (fillRectangleConfig.dimension.height == expectedHeight);

      m_isDMA2DFillRectangleConfigOk = (fillRectangleConfig.position.x == expectedXPos);
      m_isDMA2DFillRectangleConfigOk = (fillRectangleConfig.position.y == expectedYPos);

      return DMA2D::ErrorCode::OK;
    });
}

void AGUIRectangle::assertThatDMA2DFillRectangleConfigParamsWereOk(void)
{
  ASSERT_THAT(m_isDMA2DFillRectangleConfigOk, Eq(true));
}

uint16_t AGUIRectangle::getGUIRectangleDisplayWidth(const GUIRectangle &guiRectangle)
{
  int16_t width = static_cast<int16_t>(guiRectangle.getWidth());

  if (0 > guiRectangle.getPosition(GUIRectangle::Position::Tag::TOP_LEFT_CORNER).x)
  {
    width += guiRectangle.getPosition(GUIRectangle::Position::Tag::TOP_LEFT_CORNER).x;
  }
  else if ((guiRectangle.getPosition(GUIRectangle::Position::Tag::TOP_LEFT_CORNER).x + width) > guiRectangle.getFrameBuffer().getWidth())
  {
    width = guiRectangle.getFrameBuffer().getWidth() - guiRectangle.getPosition(GUIRectangle::Position::Tag::TOP_LEFT_CORNER).x;
  }

  if (width < 0)
  {
    width = 0;
  }

  return static_cast<uint16_t>(width);
}

uint16_t AGUIRectangle::getGUIRectangleDisplayHeight(const GUIRectangle &guiRectangle)
{
  int16_t height = static_cast<int16_t>(guiRectangle.getHeight());

  if (0 > guiRectangle.getPosition(GUIRectangle::Position::Tag::TOP_LEFT_CORNER).y)
  {
    height += guiRectangle.getPosition(GUIRectangle::Position::Tag::TOP_LEFT_CORNER).y;
  }
  else if ((guiRectangle.getPosition(GUIRectangle::Position::Tag::TOP_LEFT_CORNER).y + height) > guiRectangle.getFrameBuffer().getHeight())
  {
    height = guiRectangle.getFrameBuffer().getHeight() - guiRectangle.getPosition(GUIRectangle::Position::Tag::TOP_LEFT_CORNER).y;
  }

  if (height < 0)
  {
    height = 0;
  }

  return static_cast<uint16_t>(height);
}

int16_t AGUIRectangle::getGUIRectangleDisplayXPos(const GUIRectangle &guiRectangle)
{
  if (0 > guiRectangle.getPosition(GUIRectangle::Position::Tag::TOP_LEFT_CORNER).x)
  {
    return 0;
  }
  else if (guiRectangle.getFrameBuffer().getWidth() <= guiRectangle.getPosition(GUIRectangle::Position::Tag::TOP_LEFT_CORNER).x)
  {
    return static_cast<int16_t>(guiRectangle.getFrameBuffer().getWidth()) - 1;
  }
  else
  {
    return guiRectangle.getPosition(GUIRectangle::Position::Tag::TOP_LEFT_CORNER).x;
  }
}

int16_t AGUIRectangle::getGUIRectangleDisplayYPos(const GUIRectangle &guiRectangle)
{
  if (0 > guiRectangle.getPosition(GUIRectangle::Position::Tag::TOP_LEFT_CORNER).y)
  {
    return 0;
  }
  else if (guiRectangle.getFrameBuffer().getHeight() <= guiRectangle.getPosition(GUIRectangle::Position::Tag::TOP_LEFT_CORNER).y)
  {
    return static_cast<int16_t>(guiRectangle.getFrameBuffer().getHeight()) - 1;
  }
  else
  {
    return guiRectangle.getPosition(GUIRectangle::Position::Tag::TOP_LEFT_CORNER).y;
  }
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
  guiRectangleDescription.baseDescription.width  = 20u;
  guiRectangleDescription.baseDescription.height = 20u;
  guiRectangleDescription.color =
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
  guiRectangleDescription.baseDescription.position =
  {
    .x   = 15,
    .y   = 10,
    .tag = GUIRectangle::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangleDescription.baseDescription.width  = 15u;
  guiRectangleDescription.baseDescription.height = 25u;
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
  guiRectangleDescription.baseDescription.width  = 45u;
  guiRectangleDescription.baseDescription.height = 10u;
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
  guiRectangleDescription.baseDescription.width  = 45u;
  guiRectangleDescription.baseDescription.height = 55u;
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

  assertThatGUIRectangleIsDrawnInFrameBuffer(guiRectangle);
}
