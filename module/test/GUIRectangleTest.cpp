#include "GUIRectangle.h"
#include "FrameBuffer.h"
#include "DMA2DMock.h"
#include "SysTickMock.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


class AGUIRectangle : public Test
{
public:
  NiceMock<DMA2DMock> dma2dMock;
  NiceMock<SysTickMock> sysTickMock;
  FrameBuffer<50u, 50u, IFrameBuffer::ColorFormat::RGB888> guiRectangleFrameBuffer;
  FrameBuffer<50u, 50u, IFrameBuffer::ColorFormat::RGB888> guiRectangleFrameBufferSnapshot;
  GUI::Rectangle guiRectangle = GUI::Rectangle(dma2dMock, sysTickMock, guiRectangleFrameBuffer);
  GUI::Rectangle::RectangleDescription guiRectangleDescription;

  GUI::Color m_initFrameBufferColor;
  bool m_isDMA2DFillRectangleConfigOk;
  bool m_isDMA2DFillRectangleCallbackOk;

  void assertThatGUIRectangleIsDrawnCorrectlyOntoFrameBuffer(const GUI::Rectangle &guiRectangle);
  void assertThatStateOfFrameBufferIsNotChanged(const GUI::Rectangle &guiRectangle);
  void assertThatDMA2DFillRectangleDrawCompletedCallbackWasOk(void);
  void expectThatDMA2DFillRectangleWillBeCalledWithAppropriateConfigParams(const GUI::Rectangle &guiRectangle);
  void expectThatDMA2DFillRectangleWillDisplayOnlyVisiblePartOfGUIRectangle(const GUI::Rectangle &guiRectangle);
  void assertThatDMA2DFillRectangleConfigParamsWereOk(void);
  void expectThatNoDMA2DOperationWillBeTriggered(void);
  void expectThatDMA2DFillRectangleWillBeCalledWithAppropriateDrawCompletedCallback(
    DMA2D::CallbackFunc callbackFunc,
    void *callbackArgument);

  void setDefaultFrameBufferColor(GUI::Color color);
  void setDMA2DTransferOngoingStateToTrue(void);
  void setDMA2DTransferOngoingStateToFalse(void);

  void SetUp() override;

private:
  void assertThatGUIRectangleIsDrawnOntoFrameBuffer(const GUI::Rectangle &guiRectangle);
  void assertThatPixelsInFrameBufferOutsideGUIRectangleDrawnAreaAreNotChanged(const GUI::Rectangle &guiRectangle);
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
    .x   = 5,
    .y   = 5,
    .tag = GUI::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangleDescription.color =
  {
    .red   = 55u,
    .green = 210u,
    .blue  = 145u
  };

  m_initFrameBufferColor.red   = 0u;
  m_initFrameBufferColor.green = 0u;
  m_initFrameBufferColor.blue  = 0u;

  m_isDMA2DFillRectangleConfigOk   = true;
  m_isDMA2DFillRectangleCallbackOk = true;

  setDefaultFrameBufferColor(m_initFrameBufferColor);
}

void AGUIRectangle::setDefaultFrameBufferColor(GUI::Color color)
{
  uint8_t *frameBufferPtr = reinterpret_cast<uint8_t*>(guiRectangle.getFrameBuffer().getPointer());

  for (uint32_t i = 0u; i < guiRectangleFrameBuffer.getSize();)
  {
    frameBufferPtr[i++] = color.blue;
    frameBufferPtr[i++] = color.green;
    frameBufferPtr[i++] = color.red;
  }
}

void AGUIRectangle::setDMA2DTransferOngoingStateToTrue(void)
{
  ON_CALL(dma2dMock, isTransferOngoing())
    .WillByDefault([&](void)
    {
      return true;
    });
}

void AGUIRectangle::setDMA2DTransferOngoingStateToFalse(void)
{
  ON_CALL(dma2dMock, isTransferOngoing())
    .WillByDefault([&](void)
    {
      return false;
    });
}

void AGUIRectangle::assertThatGUIRectangleIsDrawnCorrectlyOntoFrameBuffer(const GUI::Rectangle &guiRectangle)
{
  assertThatGUIRectangleIsDrawnOntoFrameBuffer(guiRectangle);
  assertThatPixelsInFrameBufferOutsideGUIRectangleDrawnAreaAreNotChanged(guiRectangle);
}

void AGUIRectangle::assertThatGUIRectangleIsDrawnOntoFrameBuffer(const GUI::Rectangle &guiRectangle)
{
  const uint16_t columnStart = guiRectangle.getVisiblePartPosition(GUI::Position::Tag::TOP_LEFT_CORNER).x;
  const uint16_t rowStart    = guiRectangle.getVisiblePartPosition(GUI::Position::Tag::TOP_LEFT_CORNER).y;
  const uint16_t columnEnd = guiRectangle.getVisiblePartPosition(GUI::Position::Tag::BOTTOM_RIGHT_CORNER).x;
  const uint16_t rowEnd    = guiRectangle.getVisiblePartPosition(GUI::Position::Tag::BOTTOM_RIGHT_CORNER).y;

  const uint8_t *frameBufferPtr  = reinterpret_cast<const uint8_t*>(guiRectangle.getFrameBuffer().getPointer());
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

void AGUIRectangle::assertThatPixelsInFrameBufferOutsideGUIRectangleDrawnAreaAreNotChanged(const GUI::Rectangle &guiRectangle)
{
  const uint16_t columnStart = guiRectangle.getVisiblePartPosition(GUI::Position::Tag::TOP_LEFT_CORNER).x;
  const uint16_t rowStart    = guiRectangle.getVisiblePartPosition(GUI::Position::Tag::TOP_LEFT_CORNER).y;
  const uint16_t columnEnd = guiRectangle.getVisiblePartPosition(GUI::Position::Tag::BOTTOM_RIGHT_CORNER).x;
  const uint16_t rowEnd    = guiRectangle.getVisiblePartPosition(GUI::Position::Tag::BOTTOM_RIGHT_CORNER).y;

  const uint8_t *frameBufferPtr = reinterpret_cast<const uint8_t*>(guiRectangle.getFrameBuffer().getPointer());
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
        ASSERT_THAT(frameBufferPtr[pixelSize * (row * frameBufferWidth + column)],     m_initFrameBufferColor.blue);
        ASSERT_THAT(frameBufferPtr[pixelSize * (row * frameBufferWidth + column) + 1], m_initFrameBufferColor.green);
        ASSERT_THAT(frameBufferPtr[pixelSize * (row * frameBufferWidth + column) + 2], m_initFrameBufferColor.red);
      }
    }
  }
}

void AGUIRectangle::assertThatStateOfFrameBufferIsNotChanged(const GUI::Rectangle &guiRectangle)
{
  const uint8_t *frameBufferPtr = reinterpret_cast<const uint8_t*>(guiRectangle.getFrameBuffer().getPointer());
  const uint32_t frameBufferWidth = guiRectangle.getFrameBuffer().getWidth();
  const uint8_t pixelSize = IFrameBuffer::getColorFormatPixelSize(guiRectangle.getFrameBuffer().getColorFormat());

  for (uint32_t row = 0u; row < guiRectangle.getFrameBuffer().getHeight(); ++row)
  {
    for (uint32_t column = 0u; column < guiRectangle.getFrameBuffer().getWidth(); ++column)
    {
      ASSERT_THAT(frameBufferPtr[pixelSize * (row * frameBufferWidth + column)],     m_initFrameBufferColor.blue);
      ASSERT_THAT(frameBufferPtr[pixelSize * (row * frameBufferWidth + column) + 1], m_initFrameBufferColor.green);
      ASSERT_THAT(frameBufferPtr[pixelSize * (row * frameBufferWidth + column) + 2], m_initFrameBufferColor.red);
    }
  }
}

void
AGUIRectangle::expectThatDMA2DFillRectangleWillBeCalledWithAppropriateConfigParams(const GUI::Rectangle &guiRectangle)
{
  EXPECT_CALL(dma2dMock, fillRectangle(_))
    .Times(1u)
    .WillOnce([=](const DMA2D::FillRectangleConfig &fillRectangleConfig) -> DMA2D::ErrorCode
    {
      const uint16_t expectedWidth  = guiRectangle.getVisiblePartWidth();
      const uint16_t expectedHeight = guiRectangle.getVisiblePartHeight();
      const int16_t expectedXPos = guiRectangle.getVisiblePartPosition(GUI::Position::Tag::TOP_LEFT_CORNER).x;
      const int16_t expectedYPos = guiRectangle.getVisiblePartPosition(GUI::Position::Tag::TOP_LEFT_CORNER).y;

      m_isDMA2DFillRectangleConfigOk &= (fillRectangleConfig.color.alpha == 0u);
      m_isDMA2DFillRectangleConfigOk &= (fillRectangleConfig.color.red   == guiRectangle.getColor().red);
      m_isDMA2DFillRectangleConfigOk &= (fillRectangleConfig.color.green == guiRectangle.getColor().green);
      m_isDMA2DFillRectangleConfigOk &= (fillRectangleConfig.color.blue  == guiRectangle.getColor().blue);

      m_isDMA2DFillRectangleConfigOk = (fillRectangleConfig.dimension.width == expectedWidth);
      m_isDMA2DFillRectangleConfigOk = (fillRectangleConfig.dimension.height == expectedHeight);

      m_isDMA2DFillRectangleConfigOk = (fillRectangleConfig.position.x == expectedXPos);
      m_isDMA2DFillRectangleConfigOk = (fillRectangleConfig.position.y == expectedYPos);

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
AGUIRectangle::expectThatDMA2DFillRectangleWillDisplayOnlyVisiblePartOfGUIRectangle(const GUI::Rectangle &guiRectangle)
{
  EXPECT_CALL(dma2dMock, fillRectangle(_))
    .Times(1u)
    .WillOnce([=](const DMA2D::FillRectangleConfig &fillRectangleConfig) -> DMA2D::ErrorCode
    {
      const uint16_t expectedWidth  = guiRectangle.getVisiblePartWidth();
      const uint16_t expectedHeight = guiRectangle.getVisiblePartHeight();
      const int16_t expectedXPos = guiRectangle.getVisiblePartPosition(GUI::Position::Tag::TOP_LEFT_CORNER).x;
      const int16_t expectedYPos = guiRectangle.getVisiblePartPosition(GUI::Position::Tag::TOP_LEFT_CORNER).y;

      m_isDMA2DFillRectangleConfigOk = (fillRectangleConfig.dimension.width == expectedWidth);
      m_isDMA2DFillRectangleConfigOk = (fillRectangleConfig.dimension.height == expectedHeight);

      m_isDMA2DFillRectangleConfigOk = (fillRectangleConfig.position.x == expectedXPos);
      m_isDMA2DFillRectangleConfigOk = (fillRectangleConfig.position.y == expectedYPos);

      return DMA2D::ErrorCode::OK;
    });
}

void AGUIRectangle::expectThatNoDMA2DOperationWillBeTriggered(void)
{
  EXPECT_CALL(dma2dMock, fillRectangle(_))
    .Times(0u);

  EXPECT_CALL(dma2dMock, copyBitmap(_))
    .Times(0u);

  EXPECT_CALL(dma2dMock, blendBitmap(_))
    .Times(0u);
}

void AGUIRectangle::expectThatDMA2DFillRectangleWillBeCalledWithAppropriateDrawCompletedCallback(
  DMA2D::CallbackFunc callbackFunc,
  void *callbackArgument)
{
  EXPECT_CALL(dma2dMock, fillRectangle(_))
    .Times(1u)
    .WillOnce([=](const DMA2D::FillRectangleConfig &fillRectangleConfig) -> DMA2D::ErrorCode
    {
      m_isDMA2DFillRectangleCallbackOk &=
        (fillRectangleConfig.drawCompletedCallback.functionPtr == callbackFunc);
     m_isDMA2DFillRectangleCallbackOk &=
        (fillRectangleConfig.drawCompletedCallback.argument == callbackArgument);

      return DMA2D::ErrorCode::OK;
    });
}

void AGUIRectangle::assertThatDMA2DFillRectangleConfigParamsWereOk(void)
{
  ASSERT_THAT(m_isDMA2DFillRectangleConfigOk, Eq(true));
}

void AGUIRectangle::assertThatDMA2DFillRectangleDrawCompletedCallbackWasOk(void)
{
  ASSERT_THAT(m_isDMA2DFillRectangleCallbackOk, Eq(true));
}


TEST_F(AGUIRectangle, GetColorReturnsColorSpecifiedAtInit)
{
  constexpr GUI::Color EXPECTED_GUI_RECTANGLE_COLOR =
  {
    .red   = 255u,
    .green = 192u,
    .blue  = 111u
  };
  guiRectangleDescription.color = EXPECTED_GUI_RECTANGLE_COLOR;
  guiRectangle.init(guiRectangleDescription);

  ASSERT_THAT(guiRectangle.getColor(), Eq(EXPECTED_GUI_RECTANGLE_COLOR));
}

TEST_F(AGUIRectangle, DrawWithCPUDrawsRectangleCorrectlyOntoAssociatedFrameBuffer)
{
  guiRectangleDescription.baseDescription.position =
  {
    .x   = 10,
    .y   = 10,
    .tag = GUI::Position::Tag::TOP_LEFT_CORNER
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

  guiRectangle.draw(GUI::DrawHardware::CPU);

  assertThatGUIRectangleIsDrawnCorrectlyOntoFrameBuffer(guiRectangle);
}

TEST_F(AGUIRectangle, DrawWithDMA2DTriggersDMA2DFillRectangleOperationWithAppropriateConfigParams)
{
  guiRectangleDescription.baseDescription.position =
  {
    .x   = 0,
    .y   = 0,
    .tag = GUI::Position::Tag::TOP_LEFT_CORNER
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

  guiRectangle.draw(GUI::DrawHardware::DMA2D);

  assertThatDMA2DFillRectangleConfigParamsWereOk();
}

TEST_F(AGUIRectangle, DrawWithCPUDrawsOnlyVisiblePixelsIfPartOfRectangleIsOutOfTheScreenAkaFrameBuffer)
{
  guiRectangleDescription.baseDescription.position =
  {
    .x   = 40,
    .y   = -15,
    .tag = GUI::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangleDescription.baseDescription.dimension =
  {
    .width  = 45u,
    .height = 55u
  };
  guiRectangle.init(guiRectangleDescription);

  guiRectangle.draw(GUI::DrawHardware::CPU);

  assertThatGUIRectangleIsDrawnCorrectlyOntoFrameBuffer(guiRectangle);
}

TEST_F(AGUIRectangle, DrawWithDMA2DDrawsOnlyVisiblePixelsIfPartOfRectangleIsOutOfTheScreenAkaFrameBuffer)
{
  guiRectangleDescription.baseDescription.position =
  {
    .x   = -20,
    .y   = 35,
    .tag = GUI::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangleDescription.baseDescription.dimension =
  {
    .width  = 45u,
    .height = 55u
  };
  guiRectangle.init(guiRectangleDescription);
  expectThatDMA2DFillRectangleWillDisplayOnlyVisiblePartOfGUIRectangle(guiRectangle);

  guiRectangle.draw(GUI::DrawHardware::DMA2D);

  assertThatDMA2DFillRectangleConfigParamsWereOk();
}

TEST_F(AGUIRectangle, DrawWithCPUDrawsOnlyVisiblePixelsAfterRectangleIsMovedPartiallyOutOfTheScreen)
{
  constexpr GUI::Position NEW_GUI_RECTANGLE_POSITION =
  {
    .x   = -20,
    .y   = -20,
    .tag = GUI::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangle.init(guiRectangleDescription);
  guiRectangle.moveToPosition(NEW_GUI_RECTANGLE_POSITION);

  guiRectangle.draw(GUI::DrawHardware::CPU);

  assertThatGUIRectangleIsDrawnCorrectlyOntoFrameBuffer(guiRectangle);
}

TEST_F(AGUIRectangle, DrawWithDMA2DDrawsOnlyVisiblePixelsAfterRectangleIsMovedPartiallyOutOfTheScreen)
{
  constexpr GUI::Position NEW_GUI_RECTANGLE_POSITION =
  {
    .x   = 40,
    .y   = 40,
    .tag = GUI::Position::Tag::TOP_LEFT_CORNER
  };
  guiRectangle.init(guiRectangleDescription);
  guiRectangle.moveToPosition(NEW_GUI_RECTANGLE_POSITION);
  expectThatDMA2DFillRectangleWillDisplayOnlyVisiblePartOfGUIRectangle(guiRectangle);

  guiRectangle.draw(GUI::DrawHardware::DMA2D);

  assertThatDMA2DFillRectangleConfigParamsWereOk();
}

TEST_F(AGUIRectangle, DrawWithDMA2DTriggersDMA2DFillRectangleOperationWithAppropriateConfigParamsAfterSubstitutionOfFrameBuffer)
{
  FrameBuffer<40u, 60u, IFrameBuffer::ColorFormat::RGB888> newFrameBuffer;
  guiRectangle.init(guiRectangleDescription);
  guiRectangle.setFrameBuffer(newFrameBuffer);
  expectThatDMA2DFillRectangleWillBeCalledWithAppropriateConfigParams(guiRectangle);

  guiRectangle.draw(GUI::DrawHardware::DMA2D);

  assertThatDMA2DFillRectangleConfigParamsWereOk();
}

TEST_F(AGUIRectangle, DrawWithDMA2DSetsFunctionCallbackDMA2DDrawCompletedToBeFillRectangleTransactionCompletedCallback)
{
  guiRectangle.init(guiRectangleDescription);
  expectThatDMA2DFillRectangleWillBeCalledWithAppropriateDrawCompletedCallback(
    GUI::Rectangle::callbackDMA2DDrawCompleted,
    reinterpret_cast<void*>(&guiRectangle));

  guiRectangle.draw(GUI::DrawHardware::DMA2D);

  assertThatDMA2DFillRectangleDrawCompletedCallbackWasOk();
}