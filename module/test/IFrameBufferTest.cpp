#include "IFrameBuffer.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


TEST(IFrameBuffer, GetColorFormatPixelSizeReturnsSizeOfAPixelInBytesForTheForwardedColorFormat)
{
  ASSERT_THAT(IFrameBuffer::getColorFormatPixelSize(IFrameBuffer::ColorFormat::ARGB8888), Eq(4u));
  ASSERT_THAT(IFrameBuffer::getColorFormatPixelSize(IFrameBuffer::ColorFormat::RGB888), Eq(3u));
}