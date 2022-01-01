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
  buildCopyBitmapConfig();
  buildBlendBitmapConfig();

  return ErrorCode::OK;
}

void GUIImage::moveToPosition(const Position &position)
{
  GUIRectangleBase::moveToPosition(position);

  const DMA2D::Dimension visiblePartDimension = mapToDMA2DDimension(getVisiblePartDimension());
  const DMA2D::Position bitmapVisiblePartCopyPosition = mapToDMA2DPosition(getBitmapVisiblePartCopyPosition());
  const DMA2D::Position imageVisiblePartPosition = mapToDMA2DPosition(getVisiblePartPosition(GUIImage::Position::Tag::TOP_LEFT_CORNER));

  m_copyBitmapConfig.dimension                    = visiblePartDimension;
  m_copyBitmapConfig.sourceRectanglePosition      = bitmapVisiblePartCopyPosition;
  m_copyBitmapConfig.destinationRectanglePosition = imageVisiblePartPosition;

  m_blendBitmapConfig.dimension                    = visiblePartDimension;
  m_blendBitmapConfig.foregroundRectanglePosition  = bitmapVisiblePartCopyPosition;
  m_blendBitmapConfig.backgroundRectanglePosition  = imageVisiblePartPosition;
  m_blendBitmapConfig.destinationRectanglePosition = imageVisiblePartPosition;
}

GUIImage::Position GUIImage::getBitmapVisiblePartCopyPosition(void) const
{
  const Position topLeftCornerPosition = getPosition(Position::Tag::TOP_LEFT_CORNER);
  Position visiblePartCopyPosition = m_bitmapDescription.copyPosition;

  if (0 > topLeftCornerPosition.x)
  {
    visiblePartCopyPosition.x -= topLeftCornerPosition.x;
  }

  if (0 > topLeftCornerPosition.y)
  {
    visiblePartCopyPosition.y -= topLeftCornerPosition.y;
  }

  visiblePartCopyPosition.x = saturateValue(
    visiblePartCopyPosition.x,
    m_bitmapDescription.copyPosition.x,
    static_cast<int16_t>(m_bitmapDescription.dimension.width));

  visiblePartCopyPosition.y = saturateValue(
    visiblePartCopyPosition.y,
    m_bitmapDescription.copyPosition.y,
    static_cast<int16_t>(m_bitmapDescription.dimension.height));

  return visiblePartCopyPosition;
}

void GUIImage::draw(DrawHardware drawHardware)
{
  if (isImageVisibleOnTheScreen())
  {
    m_lastTransactionDrawHardware = drawHardware;

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
}

bool GUIImage::isDrawCompleted(void) const
{
  if (DrawHardware::DMA2D == m_lastTransactionDrawHardware)
  {
    return not m_dma2d.isTransferOngoing();
  }
  else
  {
    return true;
  }
}

void GUIImage::drawDMA2D(void)
{
  switch (m_bitmapDescription.colorFormat)
  {
    case ColorFormat::ARGB8888:
      drawDMA2DFromBitmapARGB8888ToFrameBufferRGB888();
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
  m_dma2d.copyBitmap(m_copyBitmapConfig);
}

void GUIImage::drawDMA2DFromBitmapARGB8888ToFrameBufferRGB888(void)
{
  m_dma2d.blendBitmap(m_blendBitmapConfig);
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

  const Position bitmapCopyPosition      = getBitmapVisiblePartCopyPosition();
  const uint32_t bitmapRowWidth          = PIXEL_SIZE * m_bitmapDescription.dimension.width;
  const uint32_t bitmapColumnStartOffset = PIXEL_SIZE * bitmapCopyPosition.x;
  const uint8_t *bitmapPtr = reinterpret_cast<const uint8_t*>(m_bitmapDescription.bitmapPtr);

  uint16_t bitmapRowIdx = bitmapCopyPosition.y;
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

  const Position bitmapCopyPosition      = getBitmapVisiblePartCopyPosition();
  const uint32_t bitmapRowWidth          = BITMAP_PIXEL_SIZE * m_bitmapDescription.dimension.width;
  const uint32_t bitmapColumnStartOffset = BITMAP_PIXEL_SIZE * bitmapCopyPosition.x;
  const uint8_t *bitmapPtr = reinterpret_cast<const uint8_t*>(m_bitmapDescription.bitmapPtr);

  uint16_t bitmapRowIdx = bitmapCopyPosition.y;
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

bool GUIImage::isImageVisibleOnTheScreen(void) const
{
  const Dimension visiblePartDimension = getVisiblePartDimension();

  return (0u != visiblePartDimension.width) && (0u != visiblePartDimension.height);
}

void GUIImage::buildCopyBitmapConfig(void)
{
  m_copyBitmapConfig =
  {
    .dimension = mapToDMA2DDimension(getVisiblePartDimension()),
    .sourceRectanglePosition = mapToDMA2DPosition(getBitmapVisiblePartCopyPosition()),
    .sourceBufferConfig =
    {
      .colorFormat     = DMA2D::InputColorFormat::RGB888,
      .bufferDimension = mapToDMA2DDimension(m_bitmapDescription.dimension),
      .bufferPtr       = m_bitmapDescription.bitmapPtr
    },
    .destinationRectanglePosition = mapToDMA2DPosition(getVisiblePartPosition(GUIImage::Position::Tag::TOP_LEFT_CORNER)),
    .destinationBufferConfig =
    {
      .colorFormat     = DMA2D::OutputColorFormat::RGB888,
      .bufferDimension = mapToDMA2DDimension(m_frameBuffer.getDimension()),
      .bufferPtr       = m_frameBuffer.getPointer()
    }
  };
}

void GUIImage::buildBlendBitmapConfig(void)
{
  m_blendBitmapConfig =
  {
    .dimension = mapToDMA2DDimension(getVisiblePartDimension()),
    .foregroundRectanglePosition = mapToDMA2DPosition(getBitmapVisiblePartCopyPosition()),
    .foregroundBufferConfig =
    {
      .colorFormat     = DMA2D::InputColorFormat::ARGB8888,
      .bufferDimension = mapToDMA2DDimension(m_bitmapDescription.dimension),
      .bufferPtr       = m_bitmapDescription.bitmapPtr
    },
    .backgroundRectanglePosition = mapToDMA2DPosition(getVisiblePartPosition(GUIImage::Position::Tag::TOP_LEFT_CORNER)),
    .backgroundBufferConfig =
    {
      .colorFormat     = DMA2D::InputColorFormat::RGB888,
      .bufferDimension = mapToDMA2DDimension(m_frameBuffer.getDimension()),
      .bufferPtr       = m_frameBuffer.getPointer()
    },
    .destinationRectanglePosition = mapToDMA2DPosition(getVisiblePartPosition(GUIImage::Position::Tag::TOP_LEFT_CORNER)),
    .destinationBufferConfig =
    {
      .colorFormat     = DMA2D::OutputColorFormat::RGB888,
      .bufferDimension = mapToDMA2DDimension(m_frameBuffer.getDimension()),
      .bufferPtr       = m_frameBuffer.getPointer()
    }
  };
}

DMA2D::Position GUIImage::mapToDMA2DPosition(Position position)
{
  return
  {
    .x = static_cast<uint16_t>(position.x),
    .y = static_cast<uint16_t>(position.y)
  };
}

DMA2D::Dimension GUIImage::mapToDMA2DDimension(IFrameBuffer::Dimension dimension)
{
  return
  {
    .width  = dimension.width,
    .height = dimension.height
  };
}

DMA2D::Dimension GUIImage::mapToDMA2DDimension(Dimension dimension)
{
  return
  {
    .width  = dimension.width,
    .height = dimension.height
  };
}