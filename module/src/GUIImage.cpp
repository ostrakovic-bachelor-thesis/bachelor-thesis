#include "GUIImage.h"


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
      drawDMA2D();
      break;

    case DrawHardware::CPU:
    default:
      drawCPU();
      break;
  }
}

void GUIImage::drawDMA2D(void)
{
  switch (m_bitmapDescription.colorFormat)
  {
    case ColorFormat::ARGB8888:
      // do nothing
      break;

    case ColorFormat::RGB888:
      drawDMA2DFromBitmapRGB888ToFrameBufferRGB888();
      break;

    default:
      // do nothing
      break;
  }
}

void GUIImage::drawCPU(void)
{
  switch (m_bitmapDescription.colorFormat)
  {
    case ColorFormat::ARGB8888:
      drawCPUFromBitmapARGB8888ToFrameBufferRGB888();
      break;

    case ColorFormat::RGB888:
      drawCPUFromBitmapRGB888ToFrameBufferRGB888();
      break;

    default:
      // do nothing
      break;
  }
}

void GUIImage::drawDMA2DFromBitmapRGB888ToFrameBufferRGB888(void)
{
  DMA2D::CopyBitmapConfig copyBitmapConfig =
  {
    .dimension =
    {
      .width  = m_rectangleBaseDescription.dimension.width,
      .height = m_rectangleBaseDescription.dimension.height
    },
    .sourceRectanglePosition =
    {
      .x = static_cast<uint16_t>(m_bitmapDescription.copyPosition.x),
      .y = static_cast<uint16_t>(m_bitmapDescription.copyPosition.y)
    },
    .sourceBufferConfig =
    {
      .colorFormat = DMA2D::InputColorFormat::RGB888,
      .bufferDimension =
      {
        .width  = m_bitmapDescription.dimension.width,
        .height = m_bitmapDescription.dimension.height
      },
      .bufferPtr = m_bitmapDescription.bitmapPtr
    },
    .destinationRectanglePosition =
    {
      .x = static_cast<uint16_t>(m_rectangleBaseDescription.position.x),
      .y = static_cast<uint16_t>(m_rectangleBaseDescription.position.y)
    },
    .destinationBufferConfig =
    {
      .colorFormat = DMA2D::OutputColorFormat::RGB888,
      .bufferDimension =
      {
        .width  = m_frameBuffer.getWidth(),
        .height = m_frameBuffer.getHeight()
      },
      .bufferPtr = m_frameBuffer.getPointer()
    }
  };

  m_dma2d.copyBitmap(copyBitmapConfig);
}

void GUIImage::drawCPUFromBitmapRGB888ToFrameBufferRGB888(void)
{
  constexpr uint32_t PIXEL_SIZE = 3u;

  const Position fbuffStartPosition = getVisiblePartPosition(Position::Tag::TOP_LEFT_CORNER);
  const Position fbuffEndPosition   = getVisiblePartPosition(Position::Tag::BOTTOM_RIGHT_CORNER);
  const uint32_t fbuffRowWidth          = PIXEL_SIZE * m_frameBuffer.getWidth();
  const uint32_t fbuffColumnStartOffset = PIXEL_SIZE * fbuffStartPosition.x;
  const uint32_t fbuffColumnEndOffset   = PIXEL_SIZE * fbuffEndPosition.x;
  uint8_t *frameBufferPtr = reinterpret_cast<uint8_t*>(m_frameBuffer.getPointer());

  const uint32_t bitmapRowWidth          = PIXEL_SIZE * m_bitmapDescription.dimension.width;
  const uint32_t bitmapColumnStartOffset = PIXEL_SIZE * m_bitmapDescription.copyPosition.x;
  const uint8_t *bitmapPtr = reinterpret_cast<const uint8_t*>(m_bitmapDescription.bitmapPtr);

  uint16_t bitmapRowIdx = m_bitmapDescription.copyPosition.y;
  for (uint16_t fbuffRowIdx = fbuffStartPosition.y; fbuffRowIdx <= fbuffEndPosition.y; ++fbuffRowIdx, ++bitmapRowIdx)
  {
    const uint32_t fbuffColumnStartIdx = fbuffRowIdx * fbuffRowWidth + fbuffColumnStartOffset;
    const uint32_t fbuffColumnEndIdx   = fbuffRowIdx * fbuffRowWidth + fbuffColumnEndOffset;
    uint32_t bitmapColumnIdx = bitmapRowIdx * bitmapRowWidth + bitmapColumnStartOffset;
    for (uint32_t fbuffColumnIdx = fbuffColumnStartIdx; fbuffColumnIdx <= fbuffColumnEndIdx;)
    {
      frameBufferPtr[fbuffColumnIdx++] = bitmapPtr[bitmapColumnIdx++];
      frameBufferPtr[fbuffColumnIdx++] = bitmapPtr[bitmapColumnIdx++];
      frameBufferPtr[fbuffColumnIdx++] = bitmapPtr[bitmapColumnIdx++];
    }
  }
}

void GUIImage::drawCPUFromBitmapARGB8888ToFrameBufferRGB888(void)
{
  constexpr uint32_t FRAME_BUFFER_PIXEL_SIZE = 3u;
  constexpr uint32_t BITMAP_PIXEL_SIZE       = 4u;

  const Position fbuffStartPosition = getVisiblePartPosition(Position::Tag::TOP_LEFT_CORNER);
  const Position fbuffEndPosition   = getVisiblePartPosition(Position::Tag::BOTTOM_RIGHT_CORNER);
  const uint32_t fbuffRowWidth          = FRAME_BUFFER_PIXEL_SIZE * m_frameBuffer.getWidth();
  const uint32_t fbuffColumnStartOffset = FRAME_BUFFER_PIXEL_SIZE * fbuffStartPosition.x;
  const uint32_t fbuffColumnEndOffset   = FRAME_BUFFER_PIXEL_SIZE * fbuffEndPosition.x;
  uint8_t *frameBufferPtr = reinterpret_cast<uint8_t*>(m_frameBuffer.getPointer());

  const uint32_t bitmapRowWidth          = BITMAP_PIXEL_SIZE * m_bitmapDescription.dimension.width;
  const uint32_t bitmapColumnStartOffset = BITMAP_PIXEL_SIZE * m_bitmapDescription.copyPosition.x;
  const uint8_t *bitmapPtr = reinterpret_cast<const uint8_t*>(m_bitmapDescription.bitmapPtr);

  uint16_t bitmapRowIdx = m_bitmapDescription.copyPosition.y;
  for (uint16_t fbuffRowIdx = fbuffStartPosition.y; fbuffRowIdx <= fbuffEndPosition.y; ++fbuffRowIdx, ++bitmapRowIdx)
  {
    const uint32_t fbuffColumnStartIdx = fbuffRowIdx * fbuffRowWidth + fbuffColumnStartOffset;
    const uint32_t fbuffColumnEndIdx   = fbuffRowIdx * fbuffRowWidth + fbuffColumnEndOffset;
    uint32_t bitmapColumnIdx = bitmapRowIdx * bitmapRowWidth + bitmapColumnStartOffset;
    for (uint32_t fbuffColumnIdx = fbuffColumnStartIdx; fbuffColumnIdx <= fbuffColumnEndIdx;)
    {
      const uint32_t alpha = bitmapPtr[bitmapColumnIdx + 3];

      frameBufferPtr[fbuffColumnIdx] = (alpha * bitmapPtr[bitmapColumnIdx++] + (255u - alpha) * frameBufferPtr[fbuffColumnIdx]) / 255u;
      ++fbuffColumnIdx;
      frameBufferPtr[fbuffColumnIdx] = (alpha * bitmapPtr[bitmapColumnIdx++] + (255u - alpha) * frameBufferPtr[fbuffColumnIdx]) / 255u;
      ++fbuffColumnIdx;
      frameBufferPtr[fbuffColumnIdx] = (alpha * bitmapPtr[bitmapColumnIdx++] + (255u - alpha) * frameBufferPtr[fbuffColumnIdx]) / 255u;
      ++fbuffColumnIdx;

      // skip alpha
      ++bitmapColumnIdx;
    }
  }
}

inline bool GUIImage::isFrameBufferColorFormatSupported(void) const
{
  return (IFrameBuffer::ColorFormat::RGB888 == m_frameBuffer.getColorFormat());
}