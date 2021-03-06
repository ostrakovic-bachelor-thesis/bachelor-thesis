#ifndef GUI_IMAGE_H
#define GUI_IMAGE_H

#include "GUIRectangleBase.h"
#include "IFrameBuffer.h"
#include "DMA2D.h"
#include <cstdint>


namespace GUI
{
  class Image : public RectangleBase
  {
  public:

    Image(DMA2D &dma2d, SysTick &sysTick, IFrameBuffer &frameBuffer);

    struct BitmapDescription
    {
      ColorFormat colorFormat;
      Dimension dimension;
      Position copyPosition;
      const void *bitmapPtr;
    };

    struct ImageDescription
    {
      RectangleBaseDescription baseDescription;
      BitmapDescription bitmapDescription;
    };

    ErrorCode init(const ImageDescription &imageDescription);

    void setFrameBuffer(IFrameBuffer &frameBuffer) override;

    void moveToPosition(const Position &position) override;

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

    void drawCPU(void) override;
    void drawDMA2D(void) override;

    void drawDMA2DFromBitmapRGB888ToFrameBufferRGB888(void);
    void drawDMA2DFromBitmapARGB8888ToFrameBufferRGB888(void);

    void drawCPUFromBitmapRGB888ToFrameBufferRGB888(void);
    void drawCPUFromBitmapARGB8888ToFrameBufferRGB888(void);

    bool isFrameBufferColorFormatSupported(void) const;
    bool isImageVisibleOnTheScreen(void) const;

    void buildCopyBitmapConfig(void);
    void buildBlendBitmapConfig(void);

    static DMA2D::Position mapToDMA2DPosition(Position position);
    static DMA2D::Dimension mapToDMA2DDimension(Dimension dimension);
    static DMA2D::Dimension mapToDMA2DDimension(IFrameBuffer::Dimension dimension);

    BitmapDescription m_bitmapDescription;

    DrawHardware m_lastTransactionDrawHardware;

    DMA2D::CopyBitmapConfig m_copyBitmapConfig;

    DMA2D::BlendBitmapConfig m_blendBitmapConfig;

    //! Reference to DMA2D
    DMA2D &m_dma2d;
  };
}

#endif // #ifndef GUI_IMAGE_H