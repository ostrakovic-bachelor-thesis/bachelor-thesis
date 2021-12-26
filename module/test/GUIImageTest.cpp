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
  DMA2DMock dma2dMock;
  FrameBuffer<50u, 50u, IFrameBuffer::ColorFormat::RGB888> guiImageFrameBuffer;
  FrameBuffer<50u, 50u, IFrameBuffer::ColorFormat::RGB888> guiImageFrameBufferBeforeDrawing;
  GUIImage guiImage = GUIImage(dma2dMock, guiImageFrameBuffer);
  GUIImage::ImageDescription guiImageDescription;

  bool m_isDMA2DCopyBitmapConfigOk;

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
  void assertThatGUIImageWithRGB888BitmapIsDrawnOntoFrameBufferWithRGB888ColorFormat(GUIImage &guiImage);
  void assertThatGUIImageWithARGB8888BitmapIsDrawnOntoFrameBufferWithRGB888ColorFormat(
    GUIImage &guiImage,
    const IFrameBuffer &frameBufferBeforeDrawing);
  void expectThatDMA2DCopyBitmapWillBeCalledWithAppropriateConfigParams(GUIImage &guiImage);
  void assertThatDMA2DCopyBitmapConfigParamsWereOk(void);

  void SetUp() override;
};

void AGUIImage::SetUp()
{
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

  m_isDMA2DCopyBitmapConfigOk = true;

  initTestBitmaps();
  setDefaultFrameBufferColor(guiImageFrameBuffer);
  setDefaultFrameBufferColor(guiImageFrameBufferBeforeDrawing);
}

void AGUIImage::initTestBitmaps(void)
{
  for (uint32_t row = 0u; row < TEST_RGB888_BITMAP_HEIGHT; ++row)
  {
    for (uint32_t column = 0u; column < TEST_RGB888_BITMAP_WIDTH; ++column)
    {
      m_testRGB888Bitmap[row][TEST_RGB888_BITMAP_COLOR_FORMAT_SIZE * column]     = 128u;
      m_testRGB888Bitmap[row][TEST_RGB888_BITMAP_COLOR_FORMAT_SIZE * column + 1] = static_cast<uint8_t>((row + column) % 256);
      m_testRGB888Bitmap[row][TEST_RGB888_BITMAP_COLOR_FORMAT_SIZE * column + 2] = 128u;
    }
  }

  for (uint32_t row = 0u; row < TEST_ARGB8888_BITMAP_HEIGHT; ++row)
  {
    for (uint32_t column = 0u; column < TEST_ARGB8888_BITMAP_WIDTH; ++column)
    {
      m_testARGB8888Bitmap[row][TEST_ARGB8888_BITMAP_COLOR_FORMAT_SIZE * column]     = 255u;
      m_testARGB8888Bitmap[row][TEST_ARGB8888_BITMAP_COLOR_FORMAT_SIZE * column + 1] = 255u;
      m_testARGB8888Bitmap[row][TEST_ARGB8888_BITMAP_COLOR_FORMAT_SIZE * column + 2] = 255u;
      m_testARGB8888Bitmap[row][TEST_ARGB8888_BITMAP_COLOR_FORMAT_SIZE * column + 3] = 128u;
    }
  }
}

void AGUIImage::setDefaultFrameBufferColor(IFrameBuffer &frameBuffer)
{
  uint8_t *frameBufferPtr = reinterpret_cast<uint8_t*>(frameBuffer.getPointer());

  for (uint32_t i = 0u; i < frameBuffer.getSize();)
  {
    frameBufferPtr[i++] = 50u;
    frameBufferPtr[i++] = 100u;
    frameBufferPtr[i++] = 200u;
  }
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
  const uint16_t bitmapColumnStart = guiImage.getBitmapCopyPosition().x;
  const uint16_t bitmapRowStart    = guiImage.getBitmapCopyPosition().y;

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

void AGUIImage::assertThatGUIImageWithARGB8888BitmapIsDrawnOntoFrameBufferWithRGB888ColorFormat(
  GUIImage &guiImage,
  const IFrameBuffer &frameBufferBeforeDrawing)
{
  constexpr uint8_t FRAME_BUFFER_PIXEL_SIZE = 3u;
  constexpr uint8_t BITMAP_PIXEL_SIZE       = 4u;

  const uint8_t *frameBufferBeforeDrawingPtr = reinterpret_cast<const uint8_t*>(frameBufferBeforeDrawing.getPointer());
  const uint8_t *frameBufferPtr   = reinterpret_cast<uint8_t*>(guiImage.getFrameBuffer().getPointer());
  const uint32_t frameBufferWidth = guiImage.getFrameBuffer().getWidth();
  const uint16_t frameBufferColumnStart = guiImage.getVisiblePartPosition(GUIImage::Position::Tag::TOP_LEFT_CORNER).x;
  const uint16_t frameBufferRowStart    = guiImage.getVisiblePartPosition(GUIImage::Position::Tag::TOP_LEFT_CORNER).y;
  const uint16_t frameBufferColumnEnd = guiImage.getVisiblePartPosition(GUIImage::Position::Tag::BOTTOM_RIGHT_CORNER).x;
  const uint16_t frameBufferRowEnd    = guiImage.getVisiblePartPosition(GUIImage::Position::Tag::BOTTOM_RIGHT_CORNER).y;

  const uint8_t *bitmapPtr = reinterpret_cast<const uint8_t*>(guiImage.getBitmapPtr());
  const uint32_t bitmapWidth = guiImage.getBitmapDimension().width;
  const uint16_t bitmapColumnStart = guiImage.getBitmapCopyPosition().x;
  const uint16_t bitmapRowStart    = guiImage.getBitmapCopyPosition().y;

  uint32_t bitmapRow = bitmapRowStart;
  for (uint32_t frameBufferRow = frameBufferRowStart; frameBufferRow < frameBufferRowEnd; ++frameBufferRow, ++bitmapRow)
  {
    uint32_t bitmapColumn = bitmapColumnStart;
    for (uint32_t frameBufferColumn = frameBufferColumnStart; frameBufferColumn < frameBufferColumnEnd; ++frameBufferColumn, ++bitmapColumn)
    {
      uint32_t frameBufferPixelIndex = FRAME_BUFFER_PIXEL_SIZE * (frameBufferRow * frameBufferWidth + frameBufferColumn);
      uint32_t bitmapPixelIndex = BITMAP_PIXEL_SIZE * (bitmapRow * bitmapWidth + bitmapColumn);

      const uint32_t alpha = bitmapPtr[bitmapPixelIndex + 3];
      const uint8_t expectedRedColor   = (alpha * bitmapPtr[bitmapPixelIndex + 2] + (255u - alpha) * frameBufferBeforeDrawingPtr[frameBufferPixelIndex + 2]) / 255u;
      const uint8_t expectedGreenColor = (alpha * bitmapPtr[bitmapPixelIndex + 1] + (255u - alpha) * frameBufferBeforeDrawingPtr[frameBufferPixelIndex + 1]) / 255u;
      const uint8_t expectedBlueColor  = (alpha * bitmapPtr[bitmapPixelIndex]     + (255u - alpha) * frameBufferBeforeDrawingPtr[frameBufferPixelIndex])     / 255u;

      ASSERT_THAT(frameBufferPtr[frameBufferPixelIndex],     expectedBlueColor);
      ASSERT_THAT(frameBufferPtr[frameBufferPixelIndex + 1], expectedGreenColor);
      ASSERT_THAT(frameBufferPtr[frameBufferPixelIndex + 2], expectedRedColor);
    }
  }
}

void AGUIImage::expectThatDMA2DCopyBitmapWillBeCalledWithAppropriateConfigParams(GUIImage &guiImage)
{
  EXPECT_CALL(dma2dMock, copyBitmap(_))
  .Times(1u)
  .WillOnce([=](const DMA2D::CopyBitmapConfig &copyBitmapConfig) -> DMA2D::ErrorCode
  {
    m_isDMA2DCopyBitmapConfigOk = (copyBitmapConfig.dimension.width == guiImage.getWidth());
    m_isDMA2DCopyBitmapConfigOk = (copyBitmapConfig.dimension.height == guiImage.getHeight());

    m_isDMA2DCopyBitmapConfigOk = (copyBitmapConfig.sourceRectanglePosition.x == guiImage.getBitmapCopyPosition().x);
    m_isDMA2DCopyBitmapConfigOk = (copyBitmapConfig.sourceRectanglePosition.y == guiImage.getBitmapCopyPosition().y);

    m_isDMA2DCopyBitmapConfigOk = (copyBitmapConfig.sourceBufferConfig.colorFormat == DMA2D::InputColorFormat::RGB888);
    m_isDMA2DCopyBitmapConfigOk = (copyBitmapConfig.sourceBufferConfig.bufferDimension.width == guiImage.getBitmapDimension().width);
    m_isDMA2DCopyBitmapConfigOk = (copyBitmapConfig.sourceBufferConfig.bufferDimension.height == guiImage.getBitmapDimension().height);
    m_isDMA2DCopyBitmapConfigOk = (copyBitmapConfig.sourceBufferConfig.bufferPtr == guiImage.getBitmapPtr());

    m_isDMA2DCopyBitmapConfigOk = (copyBitmapConfig.destinationRectanglePosition.x == guiImage.getPosition(GUIImage::Position::Tag::TOP_LEFT_CORNER).x);
    m_isDMA2DCopyBitmapConfigOk = (copyBitmapConfig.destinationRectanglePosition.y == guiImage.getPosition(GUIImage::Position::Tag::TOP_LEFT_CORNER).y);

    m_isDMA2DCopyBitmapConfigOk = (copyBitmapConfig.destinationBufferConfig.colorFormat == DMA2D::OutputColorFormat::RGB888);
    m_isDMA2DCopyBitmapConfigOk = (copyBitmapConfig.destinationBufferConfig.bufferDimension.width == guiImage.getFrameBuffer().getWidth());
    m_isDMA2DCopyBitmapConfigOk = (copyBitmapConfig.destinationBufferConfig.bufferDimension.height == guiImage.getFrameBuffer().getHeight());
    m_isDMA2DCopyBitmapConfigOk = (copyBitmapConfig.destinationBufferConfig.bufferPtr == guiImage.getFrameBuffer().getPointer());

    return DMA2D::ErrorCode::OK;
  });
}

void AGUIImage::assertThatDMA2DCopyBitmapConfigParamsWereOk(void)
{
  ASSERT_THAT(m_isDMA2DCopyBitmapConfigOk, Eq(true));
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

TEST_F(AGUIImage, DrawWithCPUDrawsRGB888BitmapOnAssociatedRGB888FrameBuffer)
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

  assertThatGUIImageWithRGB888BitmapIsDrawnOntoFrameBufferWithRGB888ColorFormat(guiImage);
}

TEST_F(AGUIImage, DrawWithCPUDrawsARGB8888BitmapOnAssociatedRGB888FrameBuffer)
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

  assertThatGUIImageWithARGB8888BitmapIsDrawnOntoFrameBufferWithRGB888ColorFormat(
    guiImage,
    guiImageFrameBufferBeforeDrawing);
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
