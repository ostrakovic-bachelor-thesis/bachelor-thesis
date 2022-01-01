#include "GUIImage.h"
#include "FrameBuffer.h"
#include "DMA2DMock.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


class AGUIImage : public Test
{
public:
  NiceMock<DMA2DMock> dma2dMock;
  FrameBuffer<50u, 50u, IFrameBuffer::ColorFormat::RGB888> guiImageFrameBuffer;
  GUIImage guiImage = GUIImage(dma2dMock, guiImageFrameBuffer);
  GUIImage::ImageDescription guiImageDescription;

  uint8_t m_initFameBufferColorRed;
  uint8_t m_initFameBufferColorGreen;
  uint8_t m_initFameBufferColorBlue;

  bool m_isDMA2DCopyBitmapConfigOk;
  bool m_isDMA2DBlendBitmapConfigOk;

  static constexpr uint16_t TEST_RGB888_BITMAP_WIDTH  = 20u;
  static constexpr uint16_t TEST_RGB888_BITMAP_HEIGHT = 20u;
  static constexpr GUIImage::ColorFormat TEST_RGB888_BITMAP_COLOR_FORMAT = GUIImage::ColorFormat::RGB888;
  static constexpr uint16_t TEST_RGB888_BITMAP_COLOR_FORMAT_SIZE = 3u;
  uint8_t m_testRGB888Bitmap[TEST_RGB888_BITMAP_HEIGHT][TEST_RGB888_BITMAP_WIDTH * TEST_RGB888_BITMAP_COLOR_FORMAT_SIZE];

  static constexpr uint16_t TEST_ARGB8888_BITMAP_WIDTH  = 15u;
  static constexpr uint16_t TEST_ARGB8888_BITMAP_HEIGHT = 25u;
  static constexpr GUIImage::ColorFormat TEST_ARGB8888_BITMAP_COLOR_FORMAT = GUIImage::ColorFormat::ARGB8888;
  static constexpr uint16_t TEST_ARGB8888_BITMAP_COLOR_FORMAT_SIZE = 4u;
  uint8_t m_testARGB8888Bitmap[TEST_ARGB8888_BITMAP_HEIGHT][TEST_ARGB8888_BITMAP_WIDTH * TEST_ARGB8888_BITMAP_COLOR_FORMAT_SIZE];

  void initTestBitmaps(void);
  void setDefaultFrameBufferColor(IFrameBuffer &frameBuffer);
  void assertThatGUIImageWithRGB888BitmapIsDrawnCorrectlyOntoFrameBufferWithRGB888ColorFormat(GUIImage &guiImage);
  void assertThatGUIImageWithARGB8888BitmapIsDrawnCorrectlyOntoFrameBufferWithRGB888ColorFormat(GUIImage &guiImage);
  void assertThatStateOfFrameBufferIsNotChanged(const GUIImage &guiImage);
  void expectThatDMA2DCopyBitmapWillBeCalledWithAppropriateConfigParams(GUIImage &guiImage);
  void expectThatDMA2DBlendBitmapWillBeCalledWithAppropriateConfigParams(GUIImage &guiImage);
  void expectThatDMA2DCopyBitmapWillDisplayOnlyVisiblePartOfGUIImage(GUIImage &guiImage);
  void expectThatDMA2DBlendBitmapWillDisplayOnlyVisiblePartOfGUIImage(GUIImage &guiImage);
  void assertThatDMA2DCopyBitmapConfigParamsWereOk(void);
  void assertThatDMA2DBlendBitmapConfigParamsWereOk(void);
  void expectThatNoDMA2DOperationWillBeCalled(void);

  void setDMA2DTransferOngoingStateToTrue(void);

  void SetUp() override;

private:
  void assertThatGUIImageWithRGB888BitmapIsDrawnOntoFrameBufferWithRGB888ColorFormat(GUIImage &guiImage);
  void assertThatGUIImageWithARGB8888BitmapIsDrawnOntoFrameBufferWithRGB888ColorFormat(GUIImage &guiImage);
  void assertThatPixelsInFrameBufferOutsideGUIImageDrawnAreaAreNotChanged(GUIImage &guiImage);
};

void AGUIImage::SetUp()
{
  m_initFameBufferColorRed   = 50u;
  m_initFameBufferColorGreen = 150u;
  m_initFameBufferColorBlue  = 250u;

  guiImageDescription.baseDescription.dimension =
  {
    .width  = 40,
    .height = 40
  };
  guiImageDescription.baseDescription.position =
  {
    .x = 5,
    .y = 5
  };

  m_isDMA2DCopyBitmapConfigOk  = true;
  m_isDMA2DBlendBitmapConfigOk = true;

  initTestBitmaps();
  setDefaultFrameBufferColor(guiImageFrameBuffer);
}

void AGUIImage::initTestBitmaps(void)
{
  for (uint32_t row = 0u; row < TEST_RGB888_BITMAP_HEIGHT; ++row)
  {
    for (uint32_t column = 0u; column < TEST_RGB888_BITMAP_WIDTH; ++column)
    {
      m_testRGB888Bitmap[row][TEST_RGB888_BITMAP_COLOR_FORMAT_SIZE * column]     = static_cast<uint8_t>((2 * row + 3 * column) % 256);
      m_testRGB888Bitmap[row][TEST_RGB888_BITMAP_COLOR_FORMAT_SIZE * column + 1] = static_cast<uint8_t>((3 * row + 7 *column) % 256);
      m_testRGB888Bitmap[row][TEST_RGB888_BITMAP_COLOR_FORMAT_SIZE * column + 2] = static_cast<uint8_t>((5 * row + 11 *column) % 256);
    }
  }

  for (uint32_t row = 0u; row < TEST_ARGB8888_BITMAP_HEIGHT; ++row)
  {
    for (uint32_t column = 0u; column < TEST_ARGB8888_BITMAP_WIDTH; ++column)
    {
      m_testARGB8888Bitmap[row][TEST_ARGB8888_BITMAP_COLOR_FORMAT_SIZE * column]     = static_cast<uint8_t>((5 * row + 11 *column) % 256);
      m_testARGB8888Bitmap[row][TEST_ARGB8888_BITMAP_COLOR_FORMAT_SIZE * column + 1] = static_cast<uint8_t>((2 * row + 3 * column) % 256);
      m_testARGB8888Bitmap[row][TEST_ARGB8888_BITMAP_COLOR_FORMAT_SIZE * column + 2] = static_cast<uint8_t>((3 * row + 7 *column) % 256);
      m_testARGB8888Bitmap[row][TEST_ARGB8888_BITMAP_COLOR_FORMAT_SIZE * column + 3] = 128u;
    }
  }
}

void AGUIImage::setDefaultFrameBufferColor(IFrameBuffer &frameBuffer)
{
  uint8_t *frameBufferPtr = reinterpret_cast<uint8_t*>(frameBuffer.getPointer());

  for (uint32_t i = 0u; i < frameBuffer.getSize();)
  {
    frameBufferPtr[i++] = m_initFameBufferColorBlue;
    frameBufferPtr[i++] = m_initFameBufferColorGreen;
    frameBufferPtr[i++] = m_initFameBufferColorRed;
  }
}

void AGUIImage::assertThatGUIImageWithRGB888BitmapIsDrawnCorrectlyOntoFrameBufferWithRGB888ColorFormat(GUIImage &guiImage)
{
  assertThatGUIImageWithRGB888BitmapIsDrawnOntoFrameBufferWithRGB888ColorFormat(guiImage);
  assertThatPixelsInFrameBufferOutsideGUIImageDrawnAreaAreNotChanged(guiImage);
}

void AGUIImage::assertThatGUIImageWithRGB888BitmapIsDrawnOntoFrameBufferWithRGB888ColorFormat(GUIImage &guiImage)
{
  constexpr uint8_t PIXEL_SIZE = 3u;

  const uint8_t *frameBufferPtr   = reinterpret_cast<uint8_t*>(guiImage.getFrameBuffer().getPointer());
  const uint32_t frameBufferWidth = guiImage.getFrameBuffer().getWidth();
  const uint16_t frameBufferColumnStart = guiImage.getVisiblePartPosition(GUIImage::Position::Tag::TOP_LEFT_CORNER).x;
  const uint16_t frameBufferRowStart    = guiImage.getVisiblePartPosition(GUIImage::Position::Tag::TOP_LEFT_CORNER).y;
  const uint16_t frameBufferColumnEnd = guiImage.getVisiblePartPosition(GUIImage::Position::Tag::BOTTOM_RIGHT_CORNER).x;
  const uint16_t frameBufferRowEnd    = guiImage.getVisiblePartPosition(GUIImage::Position::Tag::BOTTOM_RIGHT_CORNER).y;

  const uint8_t *bitmapPtr = reinterpret_cast<const uint8_t*>(guiImage.getBitmapPtr());
  const uint32_t bitmapWidth = guiImage.getBitmapDimension().width;
  const uint16_t bitmapColumnStart = guiImage.getBitmapVisiblePartCopyPosition().x;
  const uint16_t bitmapRowStart    = guiImage.getBitmapVisiblePartCopyPosition().y;

  uint32_t bitmapRow = bitmapRowStart;
  for (uint32_t frameBufferRow = frameBufferRowStart; frameBufferRow < frameBufferRowEnd; ++frameBufferRow, ++bitmapRow)
  {
    uint32_t bitmapColumn = bitmapColumnStart;
    for (uint32_t frameBufferColumn = frameBufferColumnStart; frameBufferColumn < frameBufferColumnEnd; ++frameBufferColumn, ++bitmapColumn)
    {
      uint32_t frameBufferPixelIndex = PIXEL_SIZE * (frameBufferRow * frameBufferWidth + frameBufferColumn);
      uint32_t bitmapPixelIndex = PIXEL_SIZE * (bitmapRow * bitmapWidth + bitmapColumn);

      ASSERT_THAT(frameBufferPtr[frameBufferPixelIndex],     bitmapPtr[bitmapPixelIndex]);
      ASSERT_THAT(frameBufferPtr[frameBufferPixelIndex + 1], bitmapPtr[bitmapPixelIndex + 1]);
      ASSERT_THAT(frameBufferPtr[frameBufferPixelIndex + 2], bitmapPtr[bitmapPixelIndex + 2]);
    }
  }
}

void AGUIImage::assertThatGUIImageWithARGB8888BitmapIsDrawnCorrectlyOntoFrameBufferWithRGB888ColorFormat(GUIImage &guiImage)
{
  assertThatGUIImageWithARGB8888BitmapIsDrawnOntoFrameBufferWithRGB888ColorFormat(guiImage);
  assertThatPixelsInFrameBufferOutsideGUIImageDrawnAreaAreNotChanged(guiImage);
}

void AGUIImage::assertThatGUIImageWithARGB8888BitmapIsDrawnOntoFrameBufferWithRGB888ColorFormat(GUIImage &guiImage)
{
  constexpr uint8_t FRAME_BUFFER_PIXEL_SIZE = 3u;
  constexpr uint8_t BITMAP_PIXEL_SIZE       = 4u;

  const uint8_t *frameBufferPtr   = reinterpret_cast<uint8_t*>(guiImage.getFrameBuffer().getPointer());
  const uint32_t frameBufferWidth = guiImage.getFrameBuffer().getWidth();
  const uint16_t frameBufferColumnStart = guiImage.getVisiblePartPosition(GUIImage::Position::Tag::TOP_LEFT_CORNER).x;
  const uint16_t frameBufferRowStart    = guiImage.getVisiblePartPosition(GUIImage::Position::Tag::TOP_LEFT_CORNER).y;
  const uint16_t frameBufferColumnEnd = guiImage.getVisiblePartPosition(GUIImage::Position::Tag::BOTTOM_RIGHT_CORNER).x;
  const uint16_t frameBufferRowEnd    = guiImage.getVisiblePartPosition(GUIImage::Position::Tag::BOTTOM_RIGHT_CORNER).y;

  const uint8_t *bitmapPtr = reinterpret_cast<const uint8_t*>(guiImage.getBitmapPtr());
  const uint32_t bitmapWidth = guiImage.getBitmapDimension().width;
  const uint16_t bitmapColumnStart = guiImage.getBitmapVisiblePartCopyPosition().x;
  const uint16_t bitmapRowStart    = guiImage.getBitmapVisiblePartCopyPosition().y;

  uint32_t bitmapRow = bitmapRowStart;
  for (uint32_t frameBufferRow = frameBufferRowStart; frameBufferRow < frameBufferRowEnd; ++frameBufferRow, ++bitmapRow)
  {
    uint32_t bitmapColumn = bitmapColumnStart;
    for (uint32_t frameBufferColumn = frameBufferColumnStart; frameBufferColumn < frameBufferColumnEnd; ++frameBufferColumn, ++bitmapColumn)
    {
      uint32_t frameBufferPixelIndex = FRAME_BUFFER_PIXEL_SIZE * (frameBufferRow * frameBufferWidth + frameBufferColumn);
      uint32_t bitmapPixelIndex = BITMAP_PIXEL_SIZE * (bitmapRow * bitmapWidth + bitmapColumn);

      const uint32_t alpha = bitmapPtr[bitmapPixelIndex + 3];

      const uint8_t expectedRedColor   = (alpha * bitmapPtr[bitmapPixelIndex + 2] + (255u - alpha) * m_initFameBufferColorRed)   / 255u;
      const uint8_t expectedGreenColor = (alpha * bitmapPtr[bitmapPixelIndex + 1] + (255u - alpha) * m_initFameBufferColorGreen) / 255u;
      const uint8_t expectedBlueColor  = (alpha * bitmapPtr[bitmapPixelIndex]     + (255u - alpha) * m_initFameBufferColorBlue)  / 255u;

      ASSERT_THAT(frameBufferPtr[frameBufferPixelIndex],     expectedBlueColor);
      ASSERT_THAT(frameBufferPtr[frameBufferPixelIndex + 1], expectedGreenColor);
      ASSERT_THAT(frameBufferPtr[frameBufferPixelIndex + 2], expectedRedColor);
    }
  }
}

void AGUIImage::assertThatPixelsInFrameBufferOutsideGUIImageDrawnAreaAreNotChanged(GUIImage &guiImage)
{
  const uint16_t columnStart = guiImage.getVisiblePartPosition(GUIImage::Position::Tag::TOP_LEFT_CORNER).x;
  const uint16_t rowStart    = guiImage.getVisiblePartPosition(GUIImage::Position::Tag::TOP_LEFT_CORNER).y;
  const uint16_t columnEnd = guiImage.getVisiblePartPosition(GUIImage::Position::Tag::BOTTOM_RIGHT_CORNER).x;
  const uint16_t rowEnd    = guiImage.getVisiblePartPosition(GUIImage::Position::Tag::BOTTOM_RIGHT_CORNER).y;

  const uint8_t *frameBufferPtr = reinterpret_cast<const uint8_t*>(guiImage.getFrameBuffer().getPointer());
  const uint32_t frameBufferWidth = guiImage.getFrameBuffer().getWidth();
  const uint8_t pixelSize = IFrameBuffer::getColorFormatPixelSize(guiImage.getFrameBuffer().getColorFormat());

  for (uint32_t row = 0u; row < guiImage.getFrameBuffer().getHeight(); ++row)
  {
    for (uint32_t column = 0u; column < guiImage.getFrameBuffer().getWidth(); ++column)
    {
      if ((rowStart <= row) && (rowEnd >= row) & (columnStart == column))
      {
        column = columnEnd;
      }
      else
      {
        ASSERT_THAT(frameBufferPtr[pixelSize * (row * frameBufferWidth + column)],     m_initFameBufferColorBlue);
        ASSERT_THAT(frameBufferPtr[pixelSize * (row * frameBufferWidth + column) + 1], m_initFameBufferColorGreen);
        ASSERT_THAT(frameBufferPtr[pixelSize * (row * frameBufferWidth + column) + 2], m_initFameBufferColorRed);
      }
    }
  }
}

void AGUIImage::assertThatStateOfFrameBufferIsNotChanged(const GUIImage &guiImage)
{
  const uint8_t *frameBufferPtr  = reinterpret_cast<const uint8_t*>(guiImage.getFrameBuffer().getPointer());
  const uint32_t frameBufferSize = guiImage.getFrameBuffer().getSize();

  for (uint32_t i = 0u; i < frameBufferSize;)
  {
    ASSERT_THAT(frameBufferPtr[i++], m_initFameBufferColorBlue);
    ASSERT_THAT(frameBufferPtr[i++], m_initFameBufferColorGreen);
    ASSERT_THAT(frameBufferPtr[i++], m_initFameBufferColorRed);
  }
}

void AGUIImage::expectThatDMA2DCopyBitmapWillBeCalledWithAppropriateConfigParams(GUIImage &guiImage)
{
  EXPECT_CALL(dma2dMock, copyBitmap(_))
  .Times(1u)
  .WillOnce([=](const DMA2D::CopyBitmapConfig &copyBitmapConfig) -> DMA2D::ErrorCode
  {
    m_isDMA2DCopyBitmapConfigOk &= (copyBitmapConfig.dimension.width == guiImage.getWidth());
    m_isDMA2DCopyBitmapConfigOk &= (copyBitmapConfig.dimension.height == guiImage.getHeight());

    m_isDMA2DCopyBitmapConfigOk &= (copyBitmapConfig.sourceRectanglePosition.x == guiImage.getBitmapCopyPosition().x);
    m_isDMA2DCopyBitmapConfigOk &= (copyBitmapConfig.sourceRectanglePosition.y == guiImage.getBitmapCopyPosition().y);

    m_isDMA2DCopyBitmapConfigOk &= (copyBitmapConfig.sourceBufferConfig.colorFormat == DMA2D::InputColorFormat::RGB888);
    m_isDMA2DCopyBitmapConfigOk &= (copyBitmapConfig.sourceBufferConfig.bufferDimension.width == guiImage.getBitmapDimension().width);
    m_isDMA2DCopyBitmapConfigOk &= (copyBitmapConfig.sourceBufferConfig.bufferDimension.height == guiImage.getBitmapDimension().height);
    m_isDMA2DCopyBitmapConfigOk &= (copyBitmapConfig.sourceBufferConfig.bufferPtr == guiImage.getBitmapPtr());

    m_isDMA2DCopyBitmapConfigOk &= (copyBitmapConfig.destinationRectanglePosition.x == guiImage.getPosition(GUIImage::Position::Tag::TOP_LEFT_CORNER).x);
    m_isDMA2DCopyBitmapConfigOk &= (copyBitmapConfig.destinationRectanglePosition.y == guiImage.getPosition(GUIImage::Position::Tag::TOP_LEFT_CORNER).y);

    m_isDMA2DCopyBitmapConfigOk &= (copyBitmapConfig.destinationBufferConfig.colorFormat == DMA2D::OutputColorFormat::RGB888);
    m_isDMA2DCopyBitmapConfigOk &= (copyBitmapConfig.destinationBufferConfig.bufferDimension.width == guiImage.getFrameBuffer().getWidth());
    m_isDMA2DCopyBitmapConfigOk &= (copyBitmapConfig.destinationBufferConfig.bufferDimension.height == guiImage.getFrameBuffer().getHeight());
    m_isDMA2DCopyBitmapConfigOk &= (copyBitmapConfig.destinationBufferConfig.bufferPtr == guiImage.getFrameBuffer().getPointer());

    return DMA2D::ErrorCode::OK;
  });
}

void AGUIImage::expectThatDMA2DBlendBitmapWillBeCalledWithAppropriateConfigParams(GUIImage &guiImage)
{
  EXPECT_CALL(dma2dMock, blendBitmap(_))
  .Times(1u)
  .WillOnce([=](const DMA2D::BlendBitmapConfig &blendBitmapConfig) -> DMA2D::ErrorCode
  {
    m_isDMA2DBlendBitmapConfigOk &= (blendBitmapConfig.dimension.width == guiImage.getWidth());
    m_isDMA2DBlendBitmapConfigOk &= (blendBitmapConfig.dimension.height == guiImage.getHeight());

    m_isDMA2DBlendBitmapConfigOk &= (blendBitmapConfig.foregroundRectanglePosition.x == guiImage.getBitmapCopyPosition().x);
    m_isDMA2DBlendBitmapConfigOk &= (blendBitmapConfig.foregroundRectanglePosition.y == guiImage.getBitmapCopyPosition().y);

    m_isDMA2DBlendBitmapConfigOk &= (blendBitmapConfig.foregroundBufferConfig.colorFormat == DMA2D::InputColorFormat::ARGB8888);
    m_isDMA2DBlendBitmapConfigOk &= (blendBitmapConfig.foregroundBufferConfig.bufferDimension.width == guiImage.getBitmapDimension().width);
    m_isDMA2DBlendBitmapConfigOk &= (blendBitmapConfig.foregroundBufferConfig.bufferDimension.height == guiImage.getBitmapDimension().height);
    m_isDMA2DBlendBitmapConfigOk &= (blendBitmapConfig.foregroundBufferConfig.bufferPtr == guiImage.getBitmapPtr());

    m_isDMA2DBlendBitmapConfigOk &= (blendBitmapConfig.backgroundRectanglePosition.x == guiImage.getPosition(GUIImage::Position::Tag::TOP_LEFT_CORNER).x);
    m_isDMA2DBlendBitmapConfigOk &= (blendBitmapConfig.backgroundRectanglePosition.y == guiImage.getPosition(GUIImage::Position::Tag::TOP_LEFT_CORNER).y);

    m_isDMA2DBlendBitmapConfigOk &= (blendBitmapConfig.backgroundBufferConfig.colorFormat == DMA2D::InputColorFormat::RGB888);
    m_isDMA2DBlendBitmapConfigOk &= (blendBitmapConfig.backgroundBufferConfig.bufferDimension.width == guiImage.getFrameBuffer().getWidth());
    m_isDMA2DBlendBitmapConfigOk &= (blendBitmapConfig.backgroundBufferConfig.bufferDimension.height == guiImage.getFrameBuffer().getHeight());
    m_isDMA2DBlendBitmapConfigOk &= (blendBitmapConfig.backgroundBufferConfig.bufferPtr == guiImage.getFrameBuffer().getPointer());

    m_isDMA2DBlendBitmapConfigOk &= (blendBitmapConfig.destinationRectanglePosition.x == guiImage.getPosition(GUIImage::Position::Tag::TOP_LEFT_CORNER).x);
    m_isDMA2DBlendBitmapConfigOk &= (blendBitmapConfig.destinationRectanglePosition.y == guiImage.getPosition(GUIImage::Position::Tag::TOP_LEFT_CORNER).y);

    m_isDMA2DBlendBitmapConfigOk &= (blendBitmapConfig.destinationBufferConfig.colorFormat == DMA2D::OutputColorFormat::RGB888);
    m_isDMA2DBlendBitmapConfigOk &= (blendBitmapConfig.destinationBufferConfig.bufferDimension.width == guiImage.getFrameBuffer().getWidth());
    m_isDMA2DBlendBitmapConfigOk &= (blendBitmapConfig.destinationBufferConfig.bufferDimension.height == guiImage.getFrameBuffer().getHeight());
    m_isDMA2DBlendBitmapConfigOk &= (blendBitmapConfig.destinationBufferConfig.bufferPtr == guiImage.getFrameBuffer().getPointer());

    return DMA2D::ErrorCode::OK;
  });
}

void AGUIImage::expectThatDMA2DCopyBitmapWillDisplayOnlyVisiblePartOfGUIImage(GUIImage &guiImage)
{
  EXPECT_CALL(dma2dMock, copyBitmap(_))
  .Times(1u)
  .WillOnce([=](const DMA2D::CopyBitmapConfig &copyBitmapConfig) -> DMA2D::ErrorCode
  {
    const uint16_t expectedWidth  = guiImage.getVisiblePartWidth();
    const uint16_t expectedHeight = guiImage.getVisiblePartHeight();
    const int16_t expectedFbuffXPos = guiImage.getVisiblePartPosition(GUIImage::Position::Tag::TOP_LEFT_CORNER).x;
    const int16_t expectedFbuffYPos = guiImage.getVisiblePartPosition(GUIImage::Position::Tag::TOP_LEFT_CORNER).y;
    const int16_t expectedBitmapXPos = guiImage.getBitmapVisiblePartCopyPosition().x;
    const int16_t expectedBitmapYPos = guiImage.getBitmapVisiblePartCopyPosition().y;

    m_isDMA2DCopyBitmapConfigOk &= (copyBitmapConfig.dimension.width == expectedWidth);
    m_isDMA2DCopyBitmapConfigOk &= (copyBitmapConfig.dimension.height == expectedHeight);

    m_isDMA2DCopyBitmapConfigOk &= (copyBitmapConfig.sourceRectanglePosition.x == expectedBitmapXPos);
    m_isDMA2DCopyBitmapConfigOk &= (copyBitmapConfig.sourceRectanglePosition.y == expectedBitmapYPos);

    m_isDMA2DCopyBitmapConfigOk &= (copyBitmapConfig.destinationRectanglePosition.x == expectedFbuffXPos);
    m_isDMA2DCopyBitmapConfigOk &= (copyBitmapConfig.destinationRectanglePosition.y == expectedFbuffYPos);

    return DMA2D::ErrorCode::OK;
  });
}

void AGUIImage::expectThatDMA2DBlendBitmapWillDisplayOnlyVisiblePartOfGUIImage(GUIImage &guiImage)
{
  EXPECT_CALL(dma2dMock, blendBitmap(_))
  .Times(1u)
  .WillOnce([=](const DMA2D::BlendBitmapConfig &blendBitmapConfig) -> DMA2D::ErrorCode
  {
    const uint16_t expectedWidth  = guiImage.getVisiblePartWidth();
    const uint16_t expectedHeight = guiImage.getVisiblePartHeight();
    const int16_t expectedFbuffXPos = guiImage.getVisiblePartPosition(GUIImage::Position::Tag::TOP_LEFT_CORNER).x;
    const int16_t expectedFbuffYPos = guiImage.getVisiblePartPosition(GUIImage::Position::Tag::TOP_LEFT_CORNER).y;
    const int16_t expectedBitmapXPos = guiImage.getBitmapVisiblePartCopyPosition().x;
    const int16_t expectedBitmapYPos = guiImage.getBitmapVisiblePartCopyPosition().y;

    m_isDMA2DBlendBitmapConfigOk &= (blendBitmapConfig.dimension.width == expectedWidth);
    m_isDMA2DBlendBitmapConfigOk &= (blendBitmapConfig.dimension.height == expectedHeight);

    m_isDMA2DBlendBitmapConfigOk &= (blendBitmapConfig.foregroundRectanglePosition.x == expectedBitmapXPos);
    m_isDMA2DBlendBitmapConfigOk &= (blendBitmapConfig.foregroundRectanglePosition.y == expectedBitmapYPos);

    m_isDMA2DBlendBitmapConfigOk &= (blendBitmapConfig.backgroundRectanglePosition.x == expectedFbuffXPos);
    m_isDMA2DBlendBitmapConfigOk &= (blendBitmapConfig.backgroundRectanglePosition.y == expectedFbuffYPos);

    m_isDMA2DBlendBitmapConfigOk &= (blendBitmapConfig.destinationRectanglePosition.x == expectedFbuffXPos);
    m_isDMA2DBlendBitmapConfigOk &= (blendBitmapConfig.destinationRectanglePosition.y == expectedFbuffYPos);

    return DMA2D::ErrorCode::OK;
  });
}

void AGUIImage::assertThatDMA2DCopyBitmapConfigParamsWereOk(void)
{
  ASSERT_THAT(m_isDMA2DCopyBitmapConfigOk, Eq(true));
}

void AGUIImage::assertThatDMA2DBlendBitmapConfigParamsWereOk(void)
{
  ASSERT_THAT(m_isDMA2DBlendBitmapConfigOk, Eq(true));
}

void AGUIImage::expectThatNoDMA2DOperationWillBeCalled(void)
{
  EXPECT_CALL(dma2dMock, fillRectangle(_))
    .Times(0u);

  EXPECT_CALL(dma2dMock, copyBitmap(_))
    .Times(0u);

  EXPECT_CALL(dma2dMock, blendBitmap(_))
    .Times(0u);
}

void AGUIImage::setDMA2DTransferOngoingStateToTrue(void)
{
  ON_CALL(dma2dMock, isTransferOngoing())
    .WillByDefault([&](void)
    {
      return true;
    });
}


TEST_F(AGUIImage, InitFailsIfGivenFrameBufferColorFormatIsNotSupported)
{
  FrameBuffer<1u, 1u, IFrameBuffer::ColorFormat::ARGB8888> frameBufferWithUnsupportedColorFormat;
  GUIImage guiImage = GUIImage(dma2dMock, frameBufferWithUnsupportedColorFormat);

  const GUIImage::ErrorCode errorCode = guiImage.init(guiImageDescription);

  ASSERT_THAT(errorCode, Eq(GUIImage::ErrorCode::UNSUPPORTED_FBUFF_COLOR_FORMAT));
}

TEST_F(AGUIImage, GetBitmapReturnsPointerToAssociatedBitmap)
{
  guiImageDescription.bitmapDescription.bitmapPtr = reinterpret_cast<const void*>(&m_testRGB888Bitmap);
  guiImage.init(guiImageDescription);

  ASSERT_THAT(guiImage.getBitmapPtr(), Eq(reinterpret_cast<const void*>(&m_testRGB888Bitmap)));
}

TEST_F(AGUIImage, GetBitmapColorFormatReturnsBitmapColorFormatSpecifiedAtTheInit)
{
  constexpr GUIImage::ColorFormat EXPECTED_GUI_IMAGE_BITMAP_COLOR_FORMAT = GUIImage::ColorFormat::RGB888;
  guiImageDescription.bitmapDescription.colorFormat = EXPECTED_GUI_IMAGE_BITMAP_COLOR_FORMAT;
  guiImage.init(guiImageDescription);

  ASSERT_THAT(guiImage.getBitmapColorFormat(), Eq(EXPECTED_GUI_IMAGE_BITMAP_COLOR_FORMAT));
}

TEST_F(AGUIImage, GetBitmapDimensionReturnsBitmapDimensionsSpecifiedAtTheInit)
{
  constexpr GUIImage::Dimension EXPECTED_GUI_IMAGE_BITMAP_DIMENSION =
  {
    .width  = 120u,
    .height = 140u
  };
  guiImageDescription.bitmapDescription.dimension = EXPECTED_GUI_IMAGE_BITMAP_DIMENSION;
  guiImage.init(guiImageDescription);

  ASSERT_THAT(guiImage.getBitmapDimension(), Eq(EXPECTED_GUI_IMAGE_BITMAP_DIMENSION));
}

TEST_F(AGUIImage, GetBitmapCopyPositionReturnsBitmapCopyPositionSpecifiedAtTheInit)
{
  constexpr GUIImage::Position EXPECTED_GUI_IMAGE_BITMAP_COPY_POSITION =
  {
    .x   = 10u,
    .y   = 20u,
    .tag = GUIImage::Position::Tag::TOP_LEFT_CORNER
  };
  guiImageDescription.bitmapDescription.copyPosition = EXPECTED_GUI_IMAGE_BITMAP_COPY_POSITION;
  guiImage.init(guiImageDescription);

  ASSERT_THAT(guiImage.getBitmapCopyPosition(), Eq(EXPECTED_GUI_IMAGE_BITMAP_COPY_POSITION));
}

TEST_F(AGUIImage, GetBitmapVisiblePartCopyPositionReturnsBitmapCopyPositionSpecifiedAtTheInitIfImageIsNotOutOfTheScreenAlongXAxis)
{
  constexpr GUIImage::Position EXPECTED_GUI_IMAGE_BITMAP_VISIBLE_PART_COPY_POSITION =
  {
    .x   = 10u,
    .y   = 20u,
    .tag = GUIImage::Position::Tag::TOP_LEFT_CORNER
  };
  guiImageDescription.baseDescription.position =
  {
    .x   = 10,
    .y   = 10,
    .tag = GUIRectangleBase::Position::Tag::TOP_LEFT_CORNER
  };
  guiImageDescription.bitmapDescription.copyPosition = EXPECTED_GUI_IMAGE_BITMAP_VISIBLE_PART_COPY_POSITION;
  guiImage.init(guiImageDescription);

  ASSERT_THAT(guiImage.getBitmapVisiblePartCopyPosition(), Eq(EXPECTED_GUI_IMAGE_BITMAP_VISIBLE_PART_COPY_POSITION));
}

TEST_F(AGUIImage, GetBitmapVisiblePartCopyPositionReturnsCorrectlyXAxisPositionWhenImageIsPartiallyOutOfScreenFromLeftSide)
{
  constexpr int16_t GUI_IMAGE_XPOS = -10;
  constexpr int16_t BITMAP_COPY_POSITION_XPOS = 10;
  constexpr uint16_t EXPECTED_BITMAP_VISIBLE_PART_COPY_POSITION_XPOS =
    static_cast<uint16_t>(BITMAP_COPY_POSITION_XPOS - GUI_IMAGE_XPOS);
  guiImageDescription.baseDescription.position =
  {
    .x   = GUI_IMAGE_XPOS,
    .y   = 10,
    .tag = GUIRectangleBase::Position::Tag::TOP_LEFT_CORNER
  };
  guiImageDescription.bitmapDescription.copyPosition =
  {
    .x   = BITMAP_COPY_POSITION_XPOS,
    .y   = 5,
    .tag = GUIRectangleBase::Position::Tag::TOP_LEFT_CORNER
  };

  guiImage.init(guiImageDescription);

  ASSERT_THAT(guiImage.getBitmapVisiblePartCopyPosition().x, Eq(EXPECTED_BITMAP_VISIBLE_PART_COPY_POSITION_XPOS));
}

TEST_F(AGUIImage, GetBitmapVisiblePartCopyPositionReturnsXAxisPositionEqualsToBitmapWidthWhenImageIsCompletelyOutOfScreenFromLeftSide)
{
  constexpr int16_t GUI_IMAGE_XPOS = -500;
  constexpr int16_t BITMAP_COPY_POSITION_XPOS = 10;
  constexpr uint16_t BITMAP_WIDTH = 20u;
  constexpr uint16_t EXPECTED_BITMAP_VISIBLE_PART_COPY_POSITION_XPOS = BITMAP_WIDTH;
  guiImageDescription.baseDescription.position =
  {
    .x   = GUI_IMAGE_XPOS,
    .y   = 5,
    .tag = GUIRectangleBase::Position::Tag::TOP_LEFT_CORNER
  };
  guiImageDescription.bitmapDescription.copyPosition =
  {
    .x   = BITMAP_COPY_POSITION_XPOS,
    .y   = 15,
    .tag = GUIRectangleBase::Position::Tag::TOP_LEFT_CORNER
  };
   guiImageDescription.bitmapDescription.dimension.width = BITMAP_WIDTH;

  guiImage.init(guiImageDescription);

  ASSERT_THAT(guiImage.getBitmapVisiblePartCopyPosition().x, Eq(EXPECTED_BITMAP_VISIBLE_PART_COPY_POSITION_XPOS));
}

TEST_F(AGUIImage, GetBitmapVisiblePartCopyPositionReturnsCorrectlyYAxisPositionWhenImageIsPartiallyOutOfScreenFromAbove)
{
  constexpr int16_t GUI_IMAGE_YPOS = -10;
  constexpr int16_t BITMAP_COPY_POSITION_YPOS = 20;
  constexpr uint16_t EXPECTED_BITMAP_VISIBLE_PART_COPY_POSITION_YPOS =
    static_cast<uint16_t>(BITMAP_COPY_POSITION_YPOS - GUI_IMAGE_YPOS);
  guiImageDescription.baseDescription.position =
  {
    .x   = 15,
    .y   = GUI_IMAGE_YPOS,
    .tag = GUIRectangleBase::Position::Tag::TOP_LEFT_CORNER
  };
  guiImageDescription.bitmapDescription.copyPosition =
  {
    .x   = 5,
    .y   = BITMAP_COPY_POSITION_YPOS,
    .tag = GUIRectangleBase::Position::Tag::TOP_LEFT_CORNER
  };

  guiImage.init(guiImageDescription);

  ASSERT_THAT(guiImage.getBitmapVisiblePartCopyPosition().y, Eq(EXPECTED_BITMAP_VISIBLE_PART_COPY_POSITION_YPOS));
}

TEST_F(AGUIImage, GetBitmapVisiblePartCopyPositionReturnsYAxisPositionEqualsToBitmapHeightWhenImageIsCompletelyOutOfScreenFromAbove)
{
  constexpr int16_t GUI_IMAGE_YPOS = -500;
  constexpr int16_t BITMAP_COPY_POSITION_YPOS = 10;
  constexpr uint16_t BITMAP_HEIGHT = 30u;
  constexpr uint16_t EXPECTED_BITMAP_VISIBLE_PART_COPY_POSITION_YPOS = BITMAP_HEIGHT;
  guiImageDescription.baseDescription.position =
  {
    .x   = 15,
    .y   = GUI_IMAGE_YPOS,
    .tag = GUIRectangleBase::Position::Tag::TOP_LEFT_CORNER
  };
  guiImageDescription.bitmapDescription.copyPosition =
  {
    .x   = 0,
    .y   = BITMAP_COPY_POSITION_YPOS,
    .tag = GUIRectangleBase::Position::Tag::TOP_LEFT_CORNER
  };
   guiImageDescription.bitmapDescription.dimension.height = BITMAP_HEIGHT;

  guiImage.init(guiImageDescription);

  ASSERT_THAT(guiImage.getBitmapVisiblePartCopyPosition().y, Eq(EXPECTED_BITMAP_VISIBLE_PART_COPY_POSITION_YPOS));
}

TEST_F(AGUIImage, DrawWithCPUDrawsRGB888BitmapOntoAssociatedRGB888FrameBufferCorrectly)
{
  guiImageDescription.baseDescription =
  {
    .dimension =
    {
      .width  = TEST_RGB888_BITMAP_WIDTH / 2u,
      .height = TEST_RGB888_BITMAP_HEIGHT / 2u,
    },
    .position =
    {
      .x   = 10,
      .y   = 10,
      .tag = GUIImage::Position::Tag::TOP_LEFT_CORNER
    }
  };
  guiImageDescription.bitmapDescription =
  {
    .colorFormat = TEST_RGB888_BITMAP_COLOR_FORMAT,
    .dimension =
    {
      .width  = TEST_RGB888_BITMAP_WIDTH,
      .height = TEST_RGB888_BITMAP_HEIGHT
    },
    .copyPosition =
    {
      .x   = TEST_RGB888_BITMAP_WIDTH / 4u,
      .y   = TEST_RGB888_BITMAP_HEIGHT / 4u,
      .tag = GUIImage::Position::Tag::TOP_LEFT_CORNER
    },
    .bitmapPtr = reinterpret_cast<const void*>(&m_testRGB888Bitmap)
  };
  guiImage.init(guiImageDescription);

  guiImage.draw(IGUIObject::DrawHardware::CPU);

  assertThatGUIImageWithRGB888BitmapIsDrawnCorrectlyOntoFrameBufferWithRGB888ColorFormat(guiImage);
}

TEST_F(AGUIImage, DrawWithCPUDrawsARGB8888BitmapOntoAssociatedRGB888FrameBufferCorrectly)
{
  guiImageDescription.baseDescription =
  {
    .dimension =
    {
      .width  = TEST_ARGB8888_BITMAP_WIDTH / 2u,
      .height = TEST_ARGB8888_BITMAP_HEIGHT / 2u,
    },
    .position =
    {
      .x   = 10,
      .y   = 10,
      .tag = GUIImage::Position::Tag::TOP_LEFT_CORNER
    }
  };
  guiImageDescription.bitmapDescription =
  {
    .colorFormat = TEST_ARGB8888_BITMAP_COLOR_FORMAT,
    .dimension =
    {
      .width  = TEST_ARGB8888_BITMAP_WIDTH,
      .height = TEST_ARGB8888_BITMAP_HEIGHT
    },
    .copyPosition =
    {
      .x   = TEST_ARGB8888_BITMAP_WIDTH / 2u,
      .y   = TEST_ARGB8888_BITMAP_HEIGHT / 2u,
      .tag = GUIImage::Position::Tag::TOP_LEFT_CORNER
    },
    .bitmapPtr = reinterpret_cast<const void*>(&m_testARGB8888Bitmap)
  };
  guiImage.init(guiImageDescription);

  guiImage.draw(IGUIObject::DrawHardware::CPU);

  assertThatGUIImageWithARGB8888BitmapIsDrawnCorrectlyOntoFrameBufferWithRGB888ColorFormat(guiImage);
}

TEST_F(AGUIImage, DrawWithDMA2DCalledOnImageWithRGB888BitmapTriggersDMA2DCopyBitmapOperationWithAppropriateConfigParams)
{
  guiImageDescription.baseDescription =
  {
    .dimension =
    {
      .width  = TEST_RGB888_BITMAP_WIDTH / 2u,
      .height = TEST_RGB888_BITMAP_HEIGHT / 2u,
    },
    .position =
    {
      .x   = 10,
      .y   = 10,
      .tag = GUIImage::Position::Tag::TOP_LEFT_CORNER
    }
  };
  guiImageDescription.bitmapDescription =
  {
    .colorFormat = TEST_RGB888_BITMAP_COLOR_FORMAT,
    .dimension =
    {
      .width  = TEST_RGB888_BITMAP_WIDTH,
      .height = TEST_RGB888_BITMAP_HEIGHT
    },
    .copyPosition =
    {
      .x   = TEST_RGB888_BITMAP_WIDTH / 4u,
      .y   = TEST_RGB888_BITMAP_HEIGHT / 4u,
      .tag = GUIImage::Position::Tag::TOP_LEFT_CORNER
    },
    .bitmapPtr = reinterpret_cast<const void*>(&m_testRGB888Bitmap)
  };
  guiImage.init(guiImageDescription);
  expectThatDMA2DCopyBitmapWillBeCalledWithAppropriateConfigParams(guiImage);

  guiImage.draw(IGUIObject::DrawHardware::DMA2D);

  assertThatDMA2DCopyBitmapConfigParamsWereOk();
}

TEST_F(AGUIImage, DrawWithDMA2DCalledOnImageWithARGB8888BitmapTriggersDMA2DBlendBitmapOperationWithAppropriateConfigParams)
{
  guiImageDescription.baseDescription =
  {
    .dimension =
    {
      .width  = TEST_ARGB8888_BITMAP_WIDTH / 2u,
      .height = TEST_ARGB8888_BITMAP_HEIGHT / 2u,
    },
    .position =
    {
      .x   = 10,
      .y   = 10,
      .tag = GUIImage::Position::Tag::TOP_LEFT_CORNER
    }
  };
  guiImageDescription.bitmapDescription =
  {
    .colorFormat = TEST_ARGB8888_BITMAP_COLOR_FORMAT,
    .dimension =
    {
      .width  = TEST_ARGB8888_BITMAP_WIDTH,
      .height = TEST_ARGB8888_BITMAP_HEIGHT
    },
    .copyPosition =
    {
      .x   = TEST_ARGB8888_BITMAP_WIDTH / 2u,
      .y   = TEST_ARGB8888_BITMAP_HEIGHT / 2u,
      .tag = GUIImage::Position::Tag::TOP_LEFT_CORNER
    },
    .bitmapPtr = reinterpret_cast<const void*>(&m_testARGB8888Bitmap)
  };
  guiImage.init(guiImageDescription);
  expectThatDMA2DBlendBitmapWillBeCalledWithAppropriateConfigParams(guiImage);

  guiImage.draw(IGUIObject::DrawHardware::DMA2D);

  assertThatDMA2DBlendBitmapConfigParamsWereOk();
}

TEST_F(AGUIImage, DrawWithDMA2DCalledOnImageWithRGB888BitmapDisplaysOnlyVisiblePartOfImageIfItIsPartiallyOutOfTheScreen)
{
  guiImageDescription.baseDescription =
  {
    .dimension =
    {
      .width  = TEST_RGB888_BITMAP_WIDTH - 2u,
      .height = TEST_RGB888_BITMAP_HEIGHT - 3u,
    },
    .position =
    {
      .x   = -10,
      .y   = 40,
      .tag = GUIImage::Position::Tag::TOP_LEFT_CORNER
    }
  };
  guiImageDescription.bitmapDescription =
  {
    .colorFormat = TEST_RGB888_BITMAP_COLOR_FORMAT,
    .dimension =
    {
      .width  = TEST_RGB888_BITMAP_WIDTH,
      .height = TEST_RGB888_BITMAP_HEIGHT
    },
    .copyPosition =
    {
      .x   = 2,
      .y   = 3,
      .tag = GUIImage::Position::Tag::TOP_LEFT_CORNER
    },
    .bitmapPtr = reinterpret_cast<const void*>(&m_testRGB888Bitmap)
  };
  guiImage.init(guiImageDescription);
  expectThatDMA2DCopyBitmapWillDisplayOnlyVisiblePartOfGUIImage(guiImage);

   guiImage.draw(IGUIObject::DrawHardware::DMA2D);

  assertThatDMA2DCopyBitmapConfigParamsWereOk();
}

TEST_F(AGUIImage, DrawWithDMA2DCalledOnImageWithARGB8888BitmapDisplaysOnlyVisiblePartOfImageIfItIsPartiallyOutOfTheScreen)
{
  guiImageDescription.baseDescription =
  {
    .dimension =
    {
      .width  = TEST_ARGB8888_BITMAP_WIDTH - 3u,
      .height = TEST_ARGB8888_BITMAP_HEIGHT - 2u,
    },
    .position =
    {
      .x   = -5,
      .y   = 45,
      .tag = GUIImage::Position::Tag::TOP_LEFT_CORNER
    }
  };
  guiImageDescription.bitmapDescription =
  {
    .colorFormat = TEST_ARGB8888_BITMAP_COLOR_FORMAT,
    .dimension =
    {
      .width  = TEST_ARGB8888_BITMAP_WIDTH,
      .height = TEST_ARGB8888_BITMAP_HEIGHT
    },
    .copyPosition =
    {
      .x   = 3,
      .y   = 2,
      .tag = GUIImage::Position::Tag::TOP_LEFT_CORNER
    },
    .bitmapPtr = reinterpret_cast<const void*>(&m_testARGB8888Bitmap)
  };
  guiImage.init(guiImageDescription);
  expectThatDMA2DBlendBitmapWillDisplayOnlyVisiblePartOfGUIImage(guiImage);

   guiImage.draw(IGUIObject::DrawHardware::DMA2D);

  assertThatDMA2DBlendBitmapConfigParamsWereOk();
}

TEST_F(AGUIImage, DrawWithDMA2DCalledAfterImageIsMovedToBePartiallyOutOfTheScreenProducesSameResultAsIfThePositionIsGivenAtInit)
{
  constexpr GUIImage::Position NEW_GUI_IMAGE_POSITION =
  {
    .x   = 40,
    .y   = 40,
    .tag = GUIImage::Position::Tag::TOP_LEFT_CORNER
  };
  guiImageDescription.baseDescription =
  {
    .dimension =
    {
      .width  = TEST_RGB888_BITMAP_WIDTH - 2u,
      .height = TEST_RGB888_BITMAP_HEIGHT - 3u,
    },
    .position =
    {
      .x   = 10,
      .y   = 10,
      .tag = GUIImage::Position::Tag::TOP_LEFT_CORNER
    }
  };
  guiImageDescription.bitmapDescription =
  {
    .colorFormat = TEST_RGB888_BITMAP_COLOR_FORMAT,
    .dimension =
    {
      .width  = TEST_RGB888_BITMAP_WIDTH,
      .height = TEST_RGB888_BITMAP_HEIGHT
    },
    .copyPosition =
    {
      .x   = 2,
      .y   = 3,
      .tag = GUIImage::Position::Tag::TOP_LEFT_CORNER
    },
    .bitmapPtr = reinterpret_cast<const void*>(&m_testRGB888Bitmap)
  };
  guiImage.init(guiImageDescription);
  guiImage.moveToPosition(NEW_GUI_IMAGE_POSITION);
  expectThatDMA2DCopyBitmapWillDisplayOnlyVisiblePartOfGUIImage(guiImage);

  guiImage.draw(IGUIObject::DrawHardware::DMA2D);

  assertThatDMA2DCopyBitmapConfigParamsWereOk();
}

TEST_F(AGUIImage, DrawWithCPUCalledOnImageWithRGB888BitmapDrawsOnlyVisiblePixelsIfImageIsPartiallyOutOfTheScreen)
{
  guiImageDescription.baseDescription =
  {
    .dimension =
    {
      .width  = TEST_RGB888_BITMAP_WIDTH - 2u,
      .height = TEST_RGB888_BITMAP_HEIGHT - 3u,
    },
    .position =
    {
      .x   = -10,
      .y   = 40,
      .tag = GUIImage::Position::Tag::TOP_LEFT_CORNER
    }
  };
  guiImageDescription.bitmapDescription =
  {
    .colorFormat = TEST_RGB888_BITMAP_COLOR_FORMAT,
    .dimension =
    {
      .width  = TEST_RGB888_BITMAP_WIDTH,
      .height = TEST_RGB888_BITMAP_HEIGHT
    },
    .copyPosition =
    {
      .x   = 2,
      .y   = 3,
      .tag = GUIImage::Position::Tag::TOP_LEFT_CORNER
    },
    .bitmapPtr = reinterpret_cast<const void*>(&m_testRGB888Bitmap)
  };
  guiImage.init(guiImageDescription);

  guiImage.draw(IGUIObject::DrawHardware::CPU);

  assertThatGUIImageWithRGB888BitmapIsDrawnCorrectlyOntoFrameBufferWithRGB888ColorFormat(guiImage);
}

TEST_F(AGUIImage, DrawWithCPUCalledOnImageWithARGB8888BitmapDisplaysOnlyVisiblePartOfImageIfItIsPartiallyOutOfTheScreen)
{
  guiImageDescription.baseDescription =
  {
    .dimension =
    {
      .width  = TEST_ARGB8888_BITMAP_WIDTH - 3u,
      .height = TEST_ARGB8888_BITMAP_HEIGHT - 2u,
    },
    .position =
    {
      .x   = -10,
      .y   = 40,
      .tag = GUIImage::Position::Tag::TOP_LEFT_CORNER
    }
  };
  guiImageDescription.bitmapDescription =
  {
    .colorFormat = TEST_ARGB8888_BITMAP_COLOR_FORMAT,
    .dimension =
    {
      .width  = TEST_ARGB8888_BITMAP_WIDTH,
      .height = TEST_ARGB8888_BITMAP_HEIGHT
    },
    .copyPosition =
    {
      .x   = 3,
      .y   = 2,
      .tag = GUIImage::Position::Tag::TOP_LEFT_CORNER
    },
    .bitmapPtr = reinterpret_cast<const void*>(&m_testARGB8888Bitmap)
  };
  guiImage.init(guiImageDescription);

  guiImage.draw(IGUIObject::DrawHardware::CPU);

  assertThatGUIImageWithARGB8888BitmapIsDrawnCorrectlyOntoFrameBufferWithRGB888ColorFormat(guiImage);
}

TEST_F(AGUIImage, DrawWithCPUCalledAfterImageIsMovedToBePartiallyOutOfTheScreenProducesSameResultAsIfThePositionIsGivenAtInit)
{
  constexpr GUIImage::Position NEW_GUI_IMAGE_POSITION =
  {
    .x   = 40,
    .y   = 40,
    .tag = GUIImage::Position::Tag::TOP_LEFT_CORNER
  };
  guiImageDescription.baseDescription =
  {
    .dimension =
    {
      .width  = TEST_RGB888_BITMAP_WIDTH - 2u,
      .height = TEST_RGB888_BITMAP_HEIGHT - 3u,
    },
    .position =
    {
      .x   = 10,
      .y   = 10,
      .tag = GUIImage::Position::Tag::TOP_LEFT_CORNER
    }
  };
  guiImageDescription.bitmapDescription =
  {
    .colorFormat = TEST_RGB888_BITMAP_COLOR_FORMAT,
    .dimension =
    {
      .width  = TEST_RGB888_BITMAP_WIDTH,
      .height = TEST_RGB888_BITMAP_HEIGHT
    },
    .copyPosition =
    {
      .x   = 2,
      .y   = 3,
      .tag = GUIImage::Position::Tag::TOP_LEFT_CORNER
    },
    .bitmapPtr = reinterpret_cast<const void*>(&m_testRGB888Bitmap)
  };
  guiImage.init(guiImageDescription);
  guiImage.moveToPosition(NEW_GUI_IMAGE_POSITION);

  guiImage.draw(IGUIObject::DrawHardware::CPU);

  assertThatGUIImageWithRGB888BitmapIsDrawnCorrectlyOntoFrameBufferWithRGB888ColorFormat(guiImage);
}

TEST_F(AGUIImage, DrawWithCPUDoesNotChangeStateOfFrameBufferIfGUIImageIsCompletelyOutOfTheScreen)
{
  guiImageDescription.baseDescription =
  {
    .dimension =
    {
      .width  = TEST_RGB888_BITMAP_WIDTH / 2u,
      .height = TEST_RGB888_BITMAP_HEIGHT / 2u,
    },
    .position =
    {
      .x   = -100,
      .y   = 10,
      .tag = GUIImage::Position::Tag::TOP_LEFT_CORNER
    }
  };
  guiImageDescription.bitmapDescription =
  {
    .colorFormat = TEST_RGB888_BITMAP_COLOR_FORMAT,
    .dimension =
    {
      .width  = TEST_RGB888_BITMAP_WIDTH,
      .height = TEST_RGB888_BITMAP_HEIGHT
    },
    .copyPosition =
    {
      .x   = TEST_RGB888_BITMAP_WIDTH / 4u,
      .y   = TEST_RGB888_BITMAP_HEIGHT / 4u,
      .tag = GUIImage::Position::Tag::TOP_LEFT_CORNER
    },
    .bitmapPtr = reinterpret_cast<const void*>(&m_testRGB888Bitmap)
  };
  guiImage.init(guiImageDescription);

  guiImage.draw(IGUIObject::DrawHardware::CPU);

  assertThatStateOfFrameBufferIsNotChanged(guiImage);
}

TEST_F(AGUIImage, DrawWithDMA2DDoesNotTriggerAnyDMA2DOperationIfGUIImageIsCompletelyOutOfTheScreen)
{
  guiImageDescription.baseDescription =
  {
    .dimension =
    {
      .width  = TEST_RGB888_BITMAP_WIDTH / 2u,
      .height = TEST_RGB888_BITMAP_HEIGHT / 2u,
    },
    .position =
    {
      .x   = -100,
      .y   = 10,
      .tag = GUIImage::Position::Tag::TOP_LEFT_CORNER
    }
  };
  guiImageDescription.bitmapDescription =
  {
    .colorFormat = TEST_RGB888_BITMAP_COLOR_FORMAT,
    .dimension =
    {
      .width  = TEST_RGB888_BITMAP_WIDTH,
      .height = TEST_RGB888_BITMAP_HEIGHT
    },
    .copyPosition =
    {
      .x   = TEST_RGB888_BITMAP_WIDTH / 4u,
      .y   = TEST_RGB888_BITMAP_HEIGHT / 4u,
      .tag = GUIImage::Position::Tag::TOP_LEFT_CORNER
    },
    .bitmapPtr = reinterpret_cast<const void*>(&m_testRGB888Bitmap)
  };
  guiImage.init(guiImageDescription);
  expectThatNoDMA2DOperationWillBeCalled();

  guiImage.draw(IGUIObject::DrawHardware::DMA2D);
}

TEST_F(AGUIImage, IsDrawCompletedReturnsImmediatelyTrueAfterDrawingWithCPU)
{
  guiImage.init(guiImageDescription);
  guiImage.draw(IGUIObject::DrawHardware::CPU);

  ASSERT_THAT(guiImage.isDrawCompleted(), Eq(true));
}

TEST_F(AGUIImage, IsDrawCompletedReturnsFalseIfDMA2DTransferIsStillOngoingWhenDrawingWithDMA2D)
{
  guiImage.init(guiImageDescription);
  guiImage.draw(IGUIObject::DrawHardware::DMA2D);
  setDMA2DTransferOngoingStateToTrue();

  ASSERT_THAT(guiImage.isDrawCompleted(), Eq(false));
}