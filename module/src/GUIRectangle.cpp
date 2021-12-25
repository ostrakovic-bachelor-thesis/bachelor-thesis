#include "GUIRectangle.h"


GUIRectangle::GUIRectangle(DMA2D &dma2d, IFrameBuffer &frameBuffer):
  GUIRectangleBase(frameBuffer),
  m_dma2d(dma2d)
{}

void GUIRectangle::init(const RectangleDescription &rectangleDescription)
{
  GUIRectangleBase::init(rectangleDescription.baseDescription);
  m_color = rectangleDescription.color;
  m_fillRectangleConfig = buildFillRectangleConfig(m_rectangleBaseDescription, m_color, m_frameBuffer);
}

GUIRectangle::Color GUIRectangle::getColor(void) const
{
  return m_color;
}

void GUIRectangle::moveToPosition(const Position &position)
{
  GUIRectangleBase::moveToPosition(position);

  const Position visiblePartPosition = getVisiblePartPosition(Position::Tag::TOP_LEFT_CORNER);

  m_fillRectangleConfig.position.x = visiblePartPosition.x;
  m_fillRectangleConfig.position.y = visiblePartPosition.y;
  m_fillRectangleConfig.dimension.width  = getVisiblePartWidth();
  m_fillRectangleConfig.dimension.height = getVisiblePartHeight();
}

void GUIRectangle::draw(DrawHardware drawHardware)
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

DMA2D::FillRectangleConfig GUIRectangle::buildFillRectangleConfig(
  const GUIRectangleBaseDescription &rectangleBaseDescription,
  Color rectangleColor,
  IFrameBuffer &frameBuffer)
{
  DMA2D::FillRectangleConfig fillRectangleConfig =
  {
    .color =
    {
      .alpha = 0u,
      .red   = rectangleColor.red,
      .green = rectangleColor.green,
      .blue  = rectangleColor.blue,
    },
    .destinationBufferConfig =
    {
      .colorFormat     = mapToDMA2DOutputColorFormat(frameBuffer.getColorFormat()),
      .bufferDimension =
      {
        .width  = frameBuffer.getWidth(),
        .height = frameBuffer.getHeight(),
      },
      .bufferPtr = frameBuffer.getPointer(),
    },
  };

  const Position visiblePartPosition = getVisiblePartPosition(Position::Tag::TOP_LEFT_CORNER);

  m_fillRectangleConfig.position.x = visiblePartPosition.x;
  m_fillRectangleConfig.position.y = visiblePartPosition.y;
  fillRectangleConfig.dimension.width  = getVisiblePartWidth();
  fillRectangleConfig.dimension.height = getVisiblePartHeight();

  return fillRectangleConfig;
}