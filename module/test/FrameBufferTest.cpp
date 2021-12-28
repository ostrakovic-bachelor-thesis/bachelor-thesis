#include "FrameBuffer.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


TEST(AFrameBuffer, GetWidthReturnsFrameBufferWidthSpecifiedAsTemplateArgument)
{
  constexpr uint32_t EXPECTED_FRAME_BUFFER_WIDTH = 390u;
  FrameBuffer<EXPECTED_FRAME_BUFFER_WIDTH, 1u, IFrameBuffer::ColorFormat::RGB888> frameBuffer;

  ASSERT_THAT(frameBuffer.getWidth(), Eq(EXPECTED_FRAME_BUFFER_WIDTH));
}

TEST(AFrameBuffer, GetHeightReturnsFrameBufferHeightSpecifiedAsTemplateArgument)
{
  constexpr uint32_t EXPECTED_FRAME_BUFFER_HEIGHT = 340u;
  FrameBuffer<1u, EXPECTED_FRAME_BUFFER_HEIGHT, IFrameBuffer::ColorFormat::RGB888> frameBuffer;

  ASSERT_THAT(frameBuffer.getHeight(), Eq(EXPECTED_FRAME_BUFFER_HEIGHT));
}

TEST(AFrameBuffer, GetDimensionReturnsFrameBufferWidthAndHeightSpecifiedAsTemplateArgumentPackedInDimensionStructure)
{
  constexpr IFrameBuffer::Dimension EXPECTED_FRAME_BUFFER_DIMENSION =
  {
    .width  = 50u,
    .height = 2u,
  };
  FrameBuffer<EXPECTED_FRAME_BUFFER_DIMENSION.width,
    EXPECTED_FRAME_BUFFER_DIMENSION.height,
    IFrameBuffer::ColorFormat::RGB888> frameBuffer;

  ASSERT_THAT(frameBuffer.getDimension(), Eq(EXPECTED_FRAME_BUFFER_DIMENSION));
}

TEST(AFrameBuffer, GetColorFormatReturnsFrameBufferColorFormatSpecifiedAsTemplateArgument)
{
  constexpr IFrameBuffer::ColorFormat EXPECTED_FRAME_BUFFER_COLOR_FORMAT = IFrameBuffer::ColorFormat::ARGB8888;
  FrameBuffer<1u, 1u, EXPECTED_FRAME_BUFFER_COLOR_FORMAT> frameBuffer;

  ASSERT_THAT(frameBuffer.getColorFormat(), Eq(EXPECTED_FRAME_BUFFER_COLOR_FORMAT));
}

TEST(AFrameBuffer, GetSizeReturnsFrameBufferSizeWhichIsProductOfItsWidthHeightAndColorFormatSize)
{
  constexpr uint32_t FRAME_BUFFER_WIDTH       = 390u;
  constexpr uint32_t FRAME_BUFFER_HEIGHT      = 390u;
  constexpr uint32_t COLOR_FORMAT_RGB888_SIZE = 3u;
  constexpr uint32_t EXPECTED_FRAME_BUFFER_SIZE = FRAME_BUFFER_WIDTH * FRAME_BUFFER_HEIGHT * COLOR_FORMAT_RGB888_SIZE;
  FrameBuffer<FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, IFrameBuffer::ColorFormat::RGB888> frameBuffer;

  ASSERT_THAT(frameBuffer.getSize(), Eq(EXPECTED_FRAME_BUFFER_SIZE));
}

TEST(AFrameBuffer, GetPointerReturnsPointerToTheUnderlayingMemoryBlockToWhichWritingShouldNotCauseAnySystemIssue)
{
  FrameBuffer<50, 70, IFrameBuffer::ColorFormat::RGB888> frameBuffer;
  uint8_t *frameBufferPtr = reinterpret_cast<uint8_t*>(frameBuffer.getPointer());

  for (uint32_t i = 0u; i < frameBuffer.getSize(); ++i)
  {
    frameBufferPtr[i] = 0xF5;
  }

  SUCCEED();
}

TEST(AFrameBuffer, DataReadFromTheUnderlayingMemoryBlockAreTheSameOneWrittenInsideIt)
{
  FrameBuffer<30, 80, IFrameBuffer::ColorFormat::ARGB8888> frameBuffer;
  uint8_t *frameBufferPtr = reinterpret_cast<uint8_t*>(frameBuffer.getPointer());
  for (uint32_t i = 0u; i < frameBuffer.getSize(); ++i)
  {
    frameBufferPtr[i] = 0xF3;
  }

  for (uint32_t i = 0u; i < frameBuffer.getSize(); ++i)
  {
    ASSERT_THAT(frameBufferPtr[i] , Eq(0xF3));
  }

  SUCCEED();
}

TEST(AFrameBuffer, IsEqualOnlyToItself)
{
  FrameBuffer<30, 80, IFrameBuffer::ColorFormat::RGB888> frameBuffer1;
  FrameBuffer<30, 80, IFrameBuffer::ColorFormat::RGB888> frameBuffer2;

  ASSERT_TRUE(frameBuffer1 == frameBuffer1);
  ASSERT_TRUE(frameBuffer2 == frameBuffer2);
  ASSERT_FALSE(frameBuffer1 == frameBuffer2);
}