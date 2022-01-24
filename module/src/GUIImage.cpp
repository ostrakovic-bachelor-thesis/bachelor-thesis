#include "GUIImage.h"


GUI::Image::Image(DMA2D &dma2d, SysTick &sysTick, IFrameBuffer &frameBuffer):
  RectangleBase(sysTick, frameBuffer),
  m_dma2d(dma2d)
{}

GUI::ErrorCode GUI::Image::init(const ImageDescription &imageDescription)
{
  if (not isFrameBufferColorFormatSupported())
  {
    return ErrorCode::UNSUPPORTED_FBUFF_COLOR_FORMAT;
  }

  RectangleBase::init(imageDescription.baseDescription);
  m_bitmapDescription = imageDescription.bitmapDescription;
  buildCopyBitmapConfig();
  buildBlendBitmapConfig();

  return ErrorCode::OK;
}

void GUI::Image::setFrameBuffer(IFrameBuffer &frameBuffer)
{
  RectangleBase::setFrameBuffer(frameBuffer);
  buildCopyBitmapConfig();
  buildBlendBitmapConfig();
}

void GUI::Image::moveToPosition(const Position &position)
{
  RectangleBase::moveToPosition(position);

  const DMA2D::Dimension visiblePartDimension = mapToDMA2DDimension(getVisiblePartDimension());
  const DMA2D::Position bitmapVisiblePartCopyPosition = mapToDMA2DPosition(getBitmapVisiblePartCopyPosition());
  const DMA2D::Position imageVisiblePartPosition = mapToDMA2DPosition(getVisiblePartPosition(GUI::Position::Tag::TOP_LEFT_CORNER));

  m_copyBitmapConfig.dimension                    = visiblePartDimension;
  m_copyBitmapConfig.sourceRectanglePosition      = bitmapVisiblePartCopyPosition;
  m_copyBitmapConfig.destinationRectanglePosition = imageVisiblePartPosition;

  m_blendBitmapConfig.dimension                    = visiblePartDimension;
  m_blendBitmapConfig.foregroundRectanglePosition  = bitmapVisiblePartCopyPosition;
  m_blendBitmapConfig.backgroundRectanglePosition  = imageVisiblePartPosition;
  m_blendBitmapConfig.destinationRectanglePosition = imageVisiblePartPosition;
}

GUI::Position GUI::Image::getBitmapVisiblePartCopyPosition(void) const
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

void GUI::Image::drawDMA2D(void)
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

void GUI::Image::drawCPU(void)
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

void GUI::Image::drawDMA2DFromBitmapRGB888ToFrameBufferRGB888(void)
{
  m_dma2d.copyBitmap(m_copyBitmapConfig);
}

void GUI::Image::drawDMA2DFromBitmapARGB8888ToFrameBufferRGB888(void)
{
  m_dma2d.blendBitmap(m_blendBitmapConfig);
}

void GUI::Image::drawCPUFromBitmapRGB888ToFrameBufferRGB888(void)
{
  constexpr uint32_t PIXEL_SIZE = 3u;

  const Position fbuffStartPosition = getVisiblePartPosition(Position::Tag::TOP_LEFT_CORNER);
  const Position fbuffEndPosition   = getVisiblePartPosition(Position::Tag::BOTTOM_RIGHT_CORNER);
  const uint32_t fbuffRowWidth          = PIXEL_SIZE * m_frameBufferPtr->getWidth();
  const uint32_t fbuffColumnStartOffset = PIXEL_SIZE * fbuffStartPosition.x;
  const uint32_t fbuffColumnEndOffset   = PIXEL_SIZE * fbuffEndPosition.x;
  uint8_t *frameBufferPtr = reinterpret_cast<uint8_t*>(m_frameBufferPtr->getPointer());

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

void GUI::Image::drawCPUFromBitmapARGB8888ToFrameBufferRGB888(void)
{
  constexpr uint32_t FRAME_BUFFER_PIXEL_SIZE = 3u;
  constexpr uint32_t BITMAP_PIXEL_SIZE       = 4u;

  const Position fbuffStartPosition = getVisiblePartPosition(Position::Tag::TOP_LEFT_CORNER);
  const Position fbuffEndPosition   = getVisiblePartPosition(Position::Tag::BOTTOM_RIGHT_CORNER);
  const uint32_t fbuffRowWidth          = FRAME_BUFFER_PIXEL_SIZE * m_frameBufferPtr->getWidth();
  const uint32_t fbuffColumnStartOffset = FRAME_BUFFER_PIXEL_SIZE * fbuffStartPosition.x;
  const uint32_t fbuffColumnEndOffset   = FRAME_BUFFER_PIXEL_SIZE * fbuffEndPosition.x;
  uint8_t *frameBufferPtr = reinterpret_cast<uint8_t*>(m_frameBufferPtr->getPointer());

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

inline bool GUI::Image::isFrameBufferColorFormatSupported(void) const
{
  return (IFrameBuffer::ColorFormat::RGB888 == m_frameBufferPtr->getColorFormat());
}

bool GUI::Image::isImageVisibleOnTheScreen(void) const
{
  const Dimension visiblePartDimension = getVisiblePartDimension();

  return (0u != visiblePartDimension.width) && (0u != visiblePartDimension.height);
}

void GUI::Image::buildCopyBitmapConfig(void)
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
    .destinationRectanglePosition = mapToDMA2DPosition(getVisiblePartPosition(GUI::Position::Tag::TOP_LEFT_CORNER)),
    .destinationBufferConfig =
    {
      .colorFormat     = DMA2D::OutputColorFormat::RGB888,
      .bufferDimension = mapToDMA2DDimension(m_frameBufferPtr->getDimension()),
      .bufferPtr       = m_frameBufferPtr->getPointer()
    },
    .drawCompletedCallback =
    {
      .functionPtr = callbackDMA2DDrawCompleted,
      .argument    = this
    }
  };
}

void GUI::Image::buildBlendBitmapConfig(void)
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
    .backgroundRectanglePosition = mapToDMA2DPosition(getVisiblePartPosition(GUI::Position::Tag::TOP_LEFT_CORNER)),
    .backgroundBufferConfig =
    {
      .colorFormat     = DMA2D::InputColorFormat::RGB888,
      .bufferDimension = mapToDMA2DDimension(m_frameBufferPtr->getDimension()),
      .bufferPtr       = m_frameBufferPtr->getPointer()
    },
    .destinationRectanglePosition = mapToDMA2DPosition(getVisiblePartPosition(GUI::Position::Tag::TOP_LEFT_CORNER)),
    .destinationBufferConfig =
    {
      .colorFormat     = DMA2D::OutputColorFormat::RGB888,
      .bufferDimension = mapToDMA2DDimension(m_frameBufferPtr->getDimension()),
      .bufferPtr       = m_frameBufferPtr->getPointer()
    },
    .drawCompletedCallback =
    {
      .functionPtr = callbackDMA2DDrawCompleted,
      .argument    = this
    }
  };
}

DMA2D::Position GUI::Image::mapToDMA2DPosition(Position position)
{
  return
  {
    .x = static_cast<uint16_t>(position.x),
    .y = static_cast<uint16_t>(position.y)
  };
}

DMA2D::Dimension GUI::Image::mapToDMA2DDimension(IFrameBuffer::Dimension dimension)
{
  return
  {
    .width  = dimension.width,
    .height = dimension.height
  };
}

DMA2D::Dimension GUI::Image::mapToDMA2DDimension(Dimension dimension)
{
  return
  {
    .width  = dimension.width,
    .height = dimension.height
  };
}