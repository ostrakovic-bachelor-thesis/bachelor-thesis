#include "GUIRectangle.h"


GUI::Rectangle::Rectangle(DMA2D &dma2d, SysTick &sysTick, IFrameBuffer &frameBuffer):
  RectangleBase(sysTick, frameBuffer),
  m_dma2d(dma2d)
{}

void GUI::Rectangle::init(const RectangleDescription &rectangleDescription)
{
  RectangleBase::init(rectangleDescription.baseDescription);
  m_color = rectangleDescription.color;
  buildFillRectangleConfig();
}

void GUI::Rectangle::setFrameBuffer(IFrameBuffer &frameBuffer)
{
  RectangleBase::setFrameBuffer(frameBuffer);
  buildFillRectangleConfig();
}

GUI::Color GUI::Rectangle::getColor(void) const
{
  return m_color;
}

void GUI::Rectangle::moveToPosition(const Position &position)
{
  RectangleBase::moveToPosition(position);
  m_fillRectangleConfig.position  = mapToDMA2DPosition(getVisiblePartPosition(Position::Tag::TOP_LEFT_CORNER));
  m_fillRectangleConfig.dimension = mapToDMA2DDimension(getVisiblePartDimension());
}

void GUI::Rectangle::drawCPU(void)
{
  const Position startPosition = getVisiblePartPosition(Position::Tag::TOP_LEFT_CORNER);
  const Position endPosition   = getVisiblePartPosition(Position::Tag::BOTTOM_RIGHT_CORNER);
  const uint32_t rowWidth      = m_frameBufferPtr->getSize() / m_frameBufferPtr->getHeight();
  const uint8_t pixelSize      = IFrameBuffer::getColorFormatPixelSize(m_frameBufferPtr->getColorFormat());
  const uint32_t columnStartOffset = pixelSize * startPosition.x;
  const uint32_t columnEndOffset   = pixelSize * endPosition.x;

  uint8_t *frameBufferPtr = reinterpret_cast<uint8_t*>(m_frameBufferPtr->getPointer());

  for (uint16_t rowIdx = startPosition.y; rowIdx <= endPosition.y; ++rowIdx)
  {
    const uint32_t columnStartIdx = rowIdx * rowWidth + columnStartOffset;
    const uint32_t columnEndIdx   = rowIdx * rowWidth + columnEndOffset;
    for (uint32_t columnIdx = columnStartIdx; columnIdx <= columnEndIdx;)
    {
      frameBufferPtr[columnIdx++] = m_color.blue;
      frameBufferPtr[columnIdx++] = m_color.green;
      frameBufferPtr[columnIdx++] = m_color.red;
    }
  }
}

void GUI::Rectangle::drawDMA2D(void)
{
  m_dma2d.fillRectangle(m_fillRectangleConfig);
}

void GUI::Rectangle::buildFillRectangleConfig(void)
{
  m_fillRectangleConfig =
  {
    .color     = mapToDMA2DColor(m_color),
    .dimension = mapToDMA2DDimension(getVisiblePartDimension()),
    .position  = mapToDMA2DPosition(getVisiblePartPosition(Position::Tag::TOP_LEFT_CORNER)),
    .destinationBufferConfig =
    {
      .colorFormat     = mapToDMA2DOutputColorFormat(m_frameBufferPtr->getColorFormat()),
      .bufferDimension = mapToDMA2DDimension(m_frameBufferPtr->getDimension()),
      .bufferPtr       = m_frameBufferPtr->getPointer(),
    },
    .drawCompletedCallback =
    {
      .functionPtr = callbackDMA2DDrawCompleted,
      .argument    = this,
    }
  };
}

DMA2D::OutputColorFormat GUI::Rectangle::mapToDMA2DOutputColorFormat(IFrameBuffer::ColorFormat colorFormat)
{
  switch (colorFormat)
  {
    case IFrameBuffer::ColorFormat::ARGB8888:
      return DMA2D::OutputColorFormat::ARGB8888;

    case IFrameBuffer::ColorFormat::RGB888:
    default:
      return DMA2D::OutputColorFormat::RGB888;
  }
}

DMA2D::Position GUI::Rectangle::mapToDMA2DPosition(Position position)
{
  return
  {
    .x = static_cast<uint16_t>(position.x),
    .y = static_cast<uint16_t>(position.y)
  };
}

DMA2D::Dimension GUI::Rectangle::mapToDMA2DDimension(Dimension dimension)
{
  return
  {
    .width  = dimension.width,
    .height = dimension.height
  };
}

DMA2D::Dimension GUI::Rectangle::mapToDMA2DDimension(IFrameBuffer::Dimension dimension)
{
  return
  {
    .width  = dimension.width,
    .height = dimension.height
  };
}

DMA2D::Color GUI::Rectangle::mapToDMA2DColor(Color color)
{
  return
  {
    .alpha = 0u,
    .red   = color.red,
    .green = color.green,
    .blue  = color.blue
  };
}