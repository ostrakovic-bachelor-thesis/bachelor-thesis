#include "GUIRectangle.h"


GUIRectangle::GUIRectangle(DMA2D &dma2d, IFrameBuffer &frameBuffer):
  GUIRectangleBase(frameBuffer),
  m_dma2d(dma2d)
{}

void GUIRectangle::init(const RectangleDescription &rectangleDescription)
{
  GUIRectangleBase::init(rectangleDescription.baseDescription);
  m_color = rectangleDescription.color;
  buildFillRectangleConfig();
}

GUIRectangle::Color GUIRectangle::getColor(void) const
{
  return m_color;
}

void GUIRectangle::moveToPosition(const Position &position)
{
  GUIRectangleBase::moveToPosition(position);

  m_fillRectangleConfig.position  = mapToDMA2DPosition(getVisiblePartPosition(Position::Tag::TOP_LEFT_CORNER));
  m_fillRectangleConfig.dimension = mapToDMA2DDimension(getVisiblePartDimension());
}

void GUIRectangle::draw(DrawHardware drawHardware)
{
  if (isRectangleVisibleOnTheScreen())
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
}

bool GUIRectangle::Color::operator==(const Color &color) const
{
  return (color.red == red) && (color.green == green) && (color.blue == blue);
}

void GUIRectangle::drawCPU(void)
{
  const Position startPosition = getVisiblePartPosition(Position::Tag::TOP_LEFT_CORNER);
  const Position endPosition   = getVisiblePartPosition(Position::Tag::BOTTOM_RIGHT_CORNER);
  const uint32_t rowWidth      = m_frameBuffer.getSize() / m_frameBuffer.getHeight();
  const uint8_t pixelSize      = IFrameBuffer::getColorFormatPixelSize(m_frameBuffer.getColorFormat());
  const uint32_t columnStartOffset = pixelSize * startPosition.x;
  const uint32_t columnEndOffset   = pixelSize * endPosition.x;

  uint8_t *frameBufferPtr = reinterpret_cast<uint8_t*>(m_frameBuffer.getPointer());

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

void GUIRectangle::drawDMA2D(void)
{
  m_dma2d.fillRectangle(m_fillRectangleConfig);
}

bool GUIRectangle::isRectangleVisibleOnTheScreen(void) const
{
  const Dimension visiblePartDimension = getVisiblePartDimension();

  return (0u != visiblePartDimension.width) && (0u != visiblePartDimension.height);
}

void GUIRectangle::buildFillRectangleConfig(void)
{
  m_fillRectangleConfig =
  {
    .color     = mapToDMA2DColor(m_color),
    .dimension = mapToDMA2DDimension(getVisiblePartDimension()),
    .position  = mapToDMA2DPosition(getVisiblePartPosition(Position::Tag::TOP_LEFT_CORNER)),
    .destinationBufferConfig =
    {
      .colorFormat     = mapToDMA2DOutputColorFormat(m_frameBuffer.getColorFormat()),
      .bufferDimension = mapToDMA2DDimension(m_frameBuffer.getDimension()),
      .bufferPtr       = m_frameBuffer.getPointer(),
    },
  };
}

DMA2D::OutputColorFormat GUIRectangle::mapToDMA2DOutputColorFormat(IFrameBuffer::ColorFormat colorFormat)
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

DMA2D::Position GUIRectangle::mapToDMA2DPosition(Position position)
{
  return
  {
    .x = static_cast<uint16_t>(position.x),
    .y = static_cast<uint16_t>(position.y)
  };
}

DMA2D::Dimension GUIRectangle::mapToDMA2DDimension(Dimension dimension)
{
  return
  {
    .width  = dimension.width,
    .height = dimension.height
  };
}

DMA2D::Dimension GUIRectangle::mapToDMA2DDimension(IFrameBuffer::Dimension dimension)
{
  return
  {
    .width  = dimension.width,
    .height = dimension.height
  };
}

DMA2D::Color GUIRectangle::mapToDMA2DColor(Color color)
{
  return
  {
    .alpha = 0u,
    .red   = color.red,
    .green = color.green,
    .blue  = color.blue
  };
}