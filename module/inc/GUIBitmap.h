#ifndef GUI_BITMAP_H
#define GUI_BITMAP_H

#include "GUIRectangleBase.h"
#include "IFrameBuffer.h"
#include "DMA2D.h"
#include <cstdint>


class GUIBitmap : public GUIRectangleBase
{
public:

  GUIBitmap(DMA2D &dma2d, IFrameBuffer &frameBuffer);

  struct BitmapDescription
  {
    GUIRectangleBaseDescription baseDescription;
    const void *bitmapPtr;
  };

  void init(const BitmapDescription &bitmapDescription);

  const void* getBitmapPtr(void) const;

private:

  const void *m_bitmapPtr;

  //! Reference to DMA2D
  DMA2D &m_dma2d;
};

#endif // #ifndef GUI_BITMAP_H