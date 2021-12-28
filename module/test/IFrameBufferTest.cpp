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

TEST(IFrameBufferDimension, IsEqualToAnotherIFrameBufferDimensionOnlyIfTheirWidthAndHeightAreTheSame)
{
  const IFrameBuffer::Dimension dimension1 =
  {
    .width  = 250,
    .height = 130
  };
  const IFrameBuffer::Dimension dimension2 =
  {
    .width  = 250,
    .height = 130
  };

  ASSERT_THAT(dimension1.width, Eq(dimension2.width));
  ASSERT_THAT(dimension1.height, Eq(dimension2.height));
  ASSERT_THAT(dimension1, Eq(dimension2));
}

TEST(IFrameBufferDimension, IsNotEqualToAnotherIFrameBufferDimensionIfTheirWidthsAreNotTheSame)
{
  const IFrameBuffer::Dimension dimension1 =
  {
    .width  = 300,
    .height = 130
  };
  const IFrameBuffer::Dimension dimension2 =
  {
    .width  = 250,
    .height = 130
  };

  ASSERT_THAT(dimension1.width, Ne(dimension2.width));
  ASSERT_THAT(dimension1.height, Eq(dimension2.height));
  ASSERT_THAT(dimension1, Ne(dimension2));
}

TEST(IFrameBufferDimension, IsNotEqualToAnotherIFrameBufferDimensionIfTheirHeightsAreNotTheSame)
{
  const IFrameBuffer::Dimension dimension1 =
  {
    .width  = 110,
    .height = 90
  };
  const IFrameBuffer::Dimension dimension2 =
  {
    .width  = 110,
    .height = 200
  };

  ASSERT_THAT(dimension1.width, Eq(dimension2.width));
  ASSERT_THAT(dimension1.height, Ne(dimension2.height));
  ASSERT_THAT(dimension1, Ne(dimension2));
}