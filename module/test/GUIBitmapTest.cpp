#include "GUIBitmap.h"
#include "FrameBuffer.h"
#include "DMA2DMock.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


class AGUIBitmap : public Test
{
public:
  DMA2DMock dma2dMock;
  FrameBuffer<50u, 50u, IFrameBuffer::ColorFormat::RGB888> guiBitmapFrameBuffer;
  GUIBitmap guiBitmap = GUIBitmap(dma2dMock, guiBitmapFrameBuffer);
  GUIBitmap::BitmapDescription guiBitmapDescription;

  void SetUp() override;
};

void AGUIBitmap::SetUp()
{
  guiBitmapDescription.baseDescription.width  = 40;
  guiBitmapDescription.baseDescription.height = 40;
  guiBitmapDescription.baseDescription.position.x = 5;
  guiBitmapDescription.baseDescription.position.y = 5;
}


TEST_F(AGUIBitmap, GetBitmapReturnsPointerToAssociatedBitmap)
{
  const uint32_t fakeBitmap[10][10] = { 0 };
  guiBitmapDescription.bitmapPtr = reinterpret_cast<const void*>(&fakeBitmap);
  guiBitmap.init(guiBitmapDescription);

  ASSERT_THAT(guiBitmap.getBitmapPtr(), Eq(reinterpret_cast<const void*>(&fakeBitmap)));
}