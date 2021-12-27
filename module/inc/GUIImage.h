#ifndef GUI_IMAGE_H
#define GUI_IMAGE_H

#include "GUIRectangleBase.h"
#include "IFrameBuffer.h"
#include "DMA2D.h"
#include <cstdint>


class GUIImage : public GUIRectangleBase
{
public:

  GUIImage(DMA2D &dma2d, IFrameBuffer &frameBuffer);

  enum class ColorFormat : uint8_t
  {
    ARGB8888 = 0u,
    RGB888   = 1u,
  };

  struct BitmapDescription
  {
    ColorFormat colorFormat;
    Dimension dimension;
    Position copyPosition;
    const void *bitmapPtr;
  };

  struct ImageDescription
  {
    GUIRectangleBaseDescription baseDescription;
    BitmapDescription bitmapDescription;
  };

  ErrorCode init(const ImageDescription &imageDescription);

  void draw(DrawHardware drawHardware) override;

  inline ColorFormat getBitmapColorFormat(void) const
  {
    return m_bitmapDescription.colorFormat;
  }

  inline Dimension getBitmapDimension(void) const
  {
    return m_bitmapDescription.dimension;
  }

  inline Position getBitmapCopyPosition(void) const
  {
    return m_bitmapDescription.copyPosition;
  }

  Position getBitmapVisiblePartCopyPosition(void) const;

  const void* getBitmapPtr(void) const
  {
    return m_bitmapDescription.bitmapPtr;
  }

private:

  void drawDMA2D(void);
  void drawDMA2DFromBitmapRGB888ToFrameBufferRGB888(void);
  void drawDMA2DFromBitmapARGB8888ToFrameBufferRGB888(void);

  void drawCPU(void);
  void drawCPUFromBitmapRGB888ToFrameBufferRGB888(void);
  void drawCPUFromBitmapARGB8888ToFrameBufferRGB888(void);

  bool isFrameBufferColorFormatSupported(void) const;

  static DMA2D::Position mapToDMA2DPosition(Position position);
  static DMA2D::Dimension mapToDMA2DDimension(Dimension dimension);

  BitmapDescription m_bitmapDescription;

  //! Reference to DMA2D
  DMA2D &m_dma2d;
};

#endif // #ifndef GUI_IMAGE_H