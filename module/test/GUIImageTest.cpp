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
  GUIImage guiImage = GUIImage(dma2dMock, guiImageFrameBuffer);
  GUIImage::ImageDescription guiImageDescription;

  static constexpr uint16_t TEST_BITMAP_WIDTH  = 20u;
  static constexpr uint16_t TEST_BITMAP_HEIGHT = 20u;
  static constexpr GUIImage::ColorFormat TEST_BITMAP_COLOR_FORMAT = GUIImage::ColorFormat::RGB888;
  static constexpr uint16_t TEST_BITMAP_COLOR_FORMAT_SIZE = 3u;
  uint8_t m_testBitmap[TEST_BITMAP_WIDTH][TEST_BITMAP_HEIGHT * TEST_BITMAP_COLOR_FORMAT_SIZE];

  void initTestBitmap(void);
  void assertThatGUIImageWithRGB888BitmapIsDrawnOntoFrameBufferWithRGB888ColorFormat(GUIImage &guiImage);

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

  initTestBitmap();
}

void AGUIImage::initTestBitmap(void)
{
  for (uint32_t row = 0u; row < TEST_BITMAP_HEIGHT; ++row)
  {
    for (uint32_t column = 0u; column < TEST_BITMAP_WIDTH; ++column)
    {
      m_testBitmap[row][TEST_BITMAP_COLOR_FORMAT_SIZE * column]     = 128u;
      m_testBitmap[row][TEST_BITMAP_COLOR_FORMAT_SIZE * column + 1] = static_cast<uint8_t>((row + column) % 256);
      m_testBitmap[row][TEST_BITMAP_COLOR_FORMAT_SIZE * column + 2] = 128u;
    }
  }
}

void AGUIImage::assertThatGUIImageWithRGB888BitmapIsDrawnOntoFrameBufferWithRGB888ColorFormat(GUIImage &guiImage)
{
  const uint8_t *frameBufferPtr      = reinterpret_cast<uint8_t*>(guiImage.getFrameBuffer().getPointer());
  const uint32_t frameBufferWidth    = guiImage.getFrameBuffer().getWidth();
  const uint8_t frameBufferPixelSize = IFrameBuffer::getColorFormatPixelSize(guiImage.getFrameBuffer().getColorFormat());
  const uint16_t frameBufferColumnStart = guiImage.getVisiblePartPosition(GUIImage::Position::Tag::TOP_LEFT_CORNER).x;
  const uint16_t frameBufferRowStart    = guiImage.getVisiblePartPosition(GUIImage::Position::Tag::TOP_LEFT_CORNER).y;
  const uint16_t frameBufferColumnEnd = guiImage.getVisiblePartPosition(GUIImage::Position::Tag::BOTTOM_RIGHT_CORNER).x;
  const uint16_t frameBufferRowEnd    = guiImage.getVisiblePartPosition(GUIImage::Position::Tag::BOTTOM_RIGHT_CORNER).y;

  const uint8_t *bitmapPtr = reinterpret_cast<const uint8_t*>(guiImage.getBitmapPtr());
  const uint32_t bitmapWidth = guiImage.getWidth();
  const uint8_t bitmapPixelSize = 3u; // GUIImage::getColorFormatPixelSize(guiImage.getColorFormat());



  for (uint32_t frameBufferRow = frameBufferRowStart, bitmapRow = 0u; frameBufferRow < frameBufferRowEnd; ++frameBufferRow, ++bitmapRow)
  {
    for (uint32_t frameBufferColumn = frameBufferColumnStart, bitmapColumn = 0u; frameBufferColumn < frameBufferColumnEnd; ++frameBufferColumn, ++bitmapColumn)
    {
      uint32_t frameBufferPixelIndex = frameBufferPixelSize * (frameBufferRow * frameBufferWidth + frameBufferColumn);
      uint32_t bitmapPixelIndex = bitmapPixelSize * (bitmapRow * bitmapWidth + bitmapColumn);

      ASSERT_THAT(frameBufferPtr[frameBufferPixelIndex],     bitmapPtr[bitmapPixelIndex]);
      ASSERT_THAT(frameBufferPtr[frameBufferPixelIndex + 1], bitmapPtr[bitmapPixelIndex + 1]);
      ASSERT_THAT(frameBufferPtr[frameBufferPixelIndex + 2], bitmapPtr[bitmapPixelIndex + 2]);
    }
  }
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
  guiImageDescription.bitmapDescription.bitmapPtr = reinterpret_cast<const void*>(&m_testBitmap);
  guiImage.init(guiImageDescription);

  ASSERT_THAT(guiImage.getBitmapPtr(), Eq(reinterpret_cast<const void*>(&m_testBitmap)));
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

// TODO
TEST_F(AGUIImage, DrawWithCPUDrawsBitmapOnAssociatedFrameBuffer)
{
  guiImageDescription.baseDescription =
  {
    .dimension =
    {
      .width  = TEST_BITMAP_WIDTH / 2u,
      .height = TEST_BITMAP_HEIGHT / 2u,
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
    .colorFormat = TEST_BITMAP_COLOR_FORMAT,
    .dimension =
    {
      .width  = TEST_BITMAP_WIDTH,
      .height = TEST_BITMAP_HEIGHT
    },
    .copyPosition =
    {
      .x   = TEST_BITMAP_WIDTH / 4u,
      .y   = TEST_BITMAP_WIDTH / 4u,
      .tag = GUIImage::Position::Tag::TOP_LEFT_CORNER
    },
    .bitmapPtr = reinterpret_cast<const void*>(&m_testBitmap)
  };
  guiImage.init(guiImageDescription);

  guiImage.draw(IGUIObject::DrawHardware::CPU);

  assertThatGUIImageWithRGB888BitmapIsDrawnOntoFrameBufferWithRGB888ColorFormat(guiImage);
}