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

  m_fillRectangleConfig.position.x = getVisiblePartPositionX(m_rectangleBaseDescription, m_frameBuffer);
  m_fillRectangleConfig.position.y = getVisiblePartPositionY(m_rectangleBaseDescription, m_frameBuffer);
  m_fillRectangleConfig.dimension.width  = getVisiblePartWidth(m_rectangleBaseDescription, m_frameBuffer);
  m_fillRectangleConfig.dimension.height = getVisiblePartHeight(m_rectangleBaseDescription, m_frameBuffer);
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

inline GUIRectangle::Position GUIRectangle::getDrawingStartPosition(void) const
{
  return
  {
    .x = static_cast<int16_t>(getVisiblePartPositionX(m_rectangleBaseDescription, m_frameBuffer)),
    .y = static_cast<int16_t>(getVisiblePartPositionY(m_rectangleBaseDescription, m_frameBuffer))
  };
}

GUIRectangle::Position GUIRectangle::getDrawingEndPosition(void) const
{
  Position endPosition = getDrawingStartPosition();

  endPosition.x += getVisiblePartWidth(m_rectangleBaseDescription, m_frameBuffer) - 1u;
  endPosition.y += getVisiblePartHeight(m_rectangleBaseDescription, m_frameBuffer) - 1u;

  return endPosition;
}

void GUIRectangle::drawCPU(void)
{
  const Position startPosition = getDrawingStartPosition();
  const Position endPosition   = getDrawingEndPosition();
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

  fillRectangleConfig.position.x = getVisiblePartPositionX(rectangleBaseDescription, frameBuffer);
  fillRectangleConfig.position.y = getVisiblePartPositionY(rectangleBaseDescription, frameBuffer);
  fillRectangleConfig.dimension.width  = getVisiblePartWidth(rectangleBaseDescription, frameBuffer);
  fillRectangleConfig.dimension.height = getVisiblePartHeight(rectangleBaseDescription, frameBuffer);

  return fillRectangleConfig;
}

uint16_t GUIRectangle::getVisiblePartPositionX(
  const GUIRectangleBaseDescription &rectangleBaseDescription,
  const IFrameBuffer &frameBuffer)
{
  if (0 > rectangleBaseDescription.position.x)
  {
    return 0u;
  }
  else if (frameBuffer.getWidth() <= rectangleBaseDescription.position.x)
  {
    return frameBuffer.getWidth() - 1u;
  }
  else
  {
    return static_cast<uint16_t>(rectangleBaseDescription.position.x);
  }
}

uint16_t GUIRectangle::getVisiblePartPositionY(
  const GUIRectangleBaseDescription &rectangleBaseDescription,
  const IFrameBuffer &frameBuffer)
{
  if (0 > rectangleBaseDescription.position.y)
  {
    return 0u;
  }
  else if (frameBuffer.getHeight() <= rectangleBaseDescription.position.y)
  {
    return frameBuffer.getHeight() - 1u;
  }
  else
  {
    return static_cast<uint16_t>(rectangleBaseDescription.position.y);
  }
}

uint16_t GUIRectangle::getVisiblePartWidth(
  const GUIRectangleBaseDescription &rectangleBaseDescription,
  const IFrameBuffer &frameBuffer)
{
  int16_t width = static_cast<int16_t>(rectangleBaseDescription.width);

  if (0 > rectangleBaseDescription.position.x)
  {
    width += rectangleBaseDescription.position.x;
  }
  else if ((rectangleBaseDescription.position.x + width) > frameBuffer.getWidth())
  {
    width = frameBuffer.getWidth() - rectangleBaseDescription.position.x;
  }

  if (width < 0)
  {
    width = 0;
  }

  return static_cast<uint16_t>(width);
}

uint16_t GUIRectangle::getVisiblePartHeight(
  const GUIRectangleBaseDescription &rectangleBaseDescription,
  const IFrameBuffer &frameBuffer)
{
  int16_t height = static_cast<int16_t>(rectangleBaseDescription.height);

  if (0 > rectangleBaseDescription.position.y)
  {
    height += rectangleBaseDescription.position.y;
  }
  else if ((rectangleBaseDescription.position.y + height) > frameBuffer.getHeight())
  {
    height = frameBuffer.getHeight() - rectangleBaseDescription.position.y;
  }

  if (height < 0)
  {
    height = 0;
  }

  return static_cast<uint16_t>(height);
}
