#include "GUIBitmap.h"


GUIBitmap::GUIBitmap(DMA2D &dma2d, IFrameBuffer &frameBuffer):
  GUIRectangleBase(frameBuffer),
  m_dma2d(dma2d)
{}

void GUIBitmap::init(const BitmapDescription &bitmapDescription)
{
  GUIRectangleBase::init(bitmapDescription.baseDescription);
  m_bitmapPtr = bitmapDescription.bitmapPtr;
}

const void* GUIBitmap::getBitmapPtr(void) const
{
  return m_bitmapPtr;
}