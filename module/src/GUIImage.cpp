#include "GUIImage.h"

#include <iostream>

GUIImage::GUIImage(DMA2D &dma2d, IFrameBuffer &frameBuffer):
  GUIRectangleBase(frameBuffer),
  m_dma2d(dma2d)
{}

GUIImage::ErrorCode GUIImage::init(const ImageDescription &imageDescription)
{
  if (not isFrameBufferColorFormatSupported())
  {
    return ErrorCode::UNSUPPORTED_FBUFF_COLOR_FORMAT;
  }

  GUIRectangleBase::init(imageDescription.baseDescription);
  m_bitmapDescription = imageDescription.bitmapDescription;

  return ErrorCode::OK;
}

void GUIImage::draw(DrawHardware drawHardware)
{
  switch (drawHardware)
  {
    case DrawHardware::DMA2D:
      //drawDMA2D();
      break;

    case DrawHardware::CPU:
    default:
      drawCPU();
      break;
  }
}

void GUIImage::drawCPU(void)
{
  switch (m_bitmapDescription.colorFormat)
  {
    case ColorFormat::ARGB8888:
      break;

    case ColorFormat::RGB888:
      drawCPUFromBitmapRGB888ToFrameBufferRGB888();
      break;

    default:
      // do nothing
      break;
  }
}

void GUIImage::drawCPUFromBitmapRGB888ToFrameBufferRGB888(void)
{
  constexpr uint8_t PIXEL_SIZE = 3u;

  const Position fbuffStartPosition = getVisiblePartPosition(Position::Tag::TOP_LEFT_CORNER);
  const Position fbuffEndPosition   = getVisiblePartPosition(Position::Tag::BOTTOM_RIGHT_CORNER);
  const uint32_t fbuffRowWidth          = PIXEL_SIZE * m_frameBuffer.getWidth();
  const uint32_t fbuffColumnStartOffset = PIXEL_SIZE * fbuffStartPosition.x;
  const uint32_t fbuffColumnEndOffset   = PIXEL_SIZE * fbuffEndPosition.x;
  uint8_t *frameBufferPtr = reinterpret_cast<uint8_t*>(m_frameBuffer.getPointer());

  const uint32_t bitmapRowWidth = PIXEL_SIZE * m_rectangleBaseDescription.dimension.width;
  const uint8_t *bitmapPtr = reinterpret_cast<const uint8_t*>(m_bitmapDescription.bitmapPtr);

  uint16_t bitmapRowIdx = 0u;
  for (uint16_t fbuffRowIdx = fbuffStartPosition.y; fbuffRowIdx <= fbuffEndPosition.y; ++fbuffRowIdx, ++bitmapRowIdx)
  {
    const uint32_t fbuffColumnStartIdx = fbuffRowIdx * fbuffRowWidth + fbuffColumnStartOffset;
    const uint32_t fbuffColumnEndIdx   = fbuffRowIdx * fbuffRowWidth + fbuffColumnEndOffset;
    uint16_t bitmapColumnIdx = bitmapRowIdx * bitmapRowWidth;
    for (uint32_t fbuffColumnIdx = fbuffColumnStartIdx; fbuffColumnIdx <= fbuffColumnEndIdx;)
    {
      frameBufferPtr[fbuffColumnIdx++] = bitmapPtr[bitmapColumnIdx++];
      frameBufferPtr[fbuffColumnIdx++] = bitmapPtr[bitmapColumnIdx++];
      frameBufferPtr[fbuffColumnIdx++] = bitmapPtr[bitmapColumnIdx++];
    }
  }
}

inline bool GUIImage::isFrameBufferColorFormatSupported(void) const
{
  return (IFrameBuffer::ColorFormat::RGB888 == m_frameBuffer.getColorFormat());
}