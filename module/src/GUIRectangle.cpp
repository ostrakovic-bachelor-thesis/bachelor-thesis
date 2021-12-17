#include "GUIRectangle.h"


GUIRectangle::GUIRectangle(DMA2D &dma2d, IFrameBuffer &frameBuffer):
  m_frameBuffer(frameBuffer),
  m_dma2d(dma2d)
{}

void GUIRectangle::init(const RectangleDescription &rectangleDescription)
{
  m_rectangleDescription = rectangleDescription;
  recalculatePositionToBeTopLeftCorner(m_rectangleDescription);
  m_fillRectangleConfig = buildFillRectangleConfig(m_rectangleDescription, m_frameBuffer);
}

uint16_t GUIRectangle::getWidth(void) const
{
  return m_rectangleDescription.width;
}

uint16_t GUIRectangle::getHeight(void) const
{
  return m_rectangleDescription.height;
}

GUIRectangle::Color GUIRectangle::getColor(void) const
{
  return m_rectangleDescription.color;
}

GUIRectangle::Position GUIRectangle::getPosition(Position::Tag positionTag) const
{
  switch (positionTag)
  {
    case Position::Tag::TOP_LEFT_CORNER:
      return getPositionTopLeftCorner();

    case Position::Tag::TOP_RIGHT_CORNER:
      return getPositionTopRightCorner();

    case Position::Tag::BOTTOM_LEFT_CORNER:
      return getPositionBottomLeftCorner();

    case Position::Tag::BOTTOM_RIGHT_CORNER:
      return getPositionBottomRightCorner();

    case Position::Tag::CENTER:
    default:
      return getPositionCenter();
  }
}

void GUIRectangle::moveToPosition(const Position &position)
{
  m_rectangleDescription.position = position;
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
  return getPosition(Position::Tag::TOP_LEFT_CORNER);
}

GUIRectangle::Position GUIRectangle::getDrawingEndPosition(void) const
{
  Position endPosition = getPosition(Position::Tag::BOTTOM_RIGHT_CORNER);

  if (getFrameBuffer().getWidth() <= endPosition.x)
  {
    endPosition.x = getFrameBuffer().getWidth() - 1u;
  }

  if (getFrameBuffer().getHeight() <= endPosition.y)
  {
    endPosition.y = getFrameBuffer().getHeight() - 1u;
  }

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
      frameBufferPtr[columnIdx++] = m_rectangleDescription.color.blue;
      frameBufferPtr[columnIdx++] = m_rectangleDescription.color.green;
      frameBufferPtr[columnIdx++] = m_rectangleDescription.color.red;
    }
  }
}

void GUIRectangle::drawDMA2D(void)
{
  m_dma2d.fillRectangle(m_fillRectangleConfig);
}

GUIRectangle::Position GUIRectangle::getPositionTopLeftCorner(void) const
{
  return m_rectangleDescription.position;
}

GUIRectangle::Position GUIRectangle::getPositionTopRightCorner(void) const
{
  const int16_t x = m_rectangleDescription.position.x + static_cast<int16_t>(m_rectangleDescription.width) - 1;
  const int16_t y = m_rectangleDescription.position.y;

  return { .x = x, .y = y, .tag = Position::Tag::TOP_RIGHT_CORNER };
}

GUIRectangle::Position GUIRectangle::getPositionBottomLeftCorner(void) const
{
  const int16_t x = m_rectangleDescription.position.x;
  const int16_t y = m_rectangleDescription.position.y + static_cast<int16_t>(m_rectangleDescription.height) - 1;

  return { .x = x, .y = y, .tag = Position::Tag::BOTTOM_LEFT_CORNER };
}

GUIRectangle::Position GUIRectangle::getPositionBottomRightCorner(void) const
{
  const int16_t x = m_rectangleDescription.position.x + static_cast<int16_t>(m_rectangleDescription.width) - 1;
  const int16_t y = m_rectangleDescription.position.y + static_cast<int16_t>(m_rectangleDescription.height) - 1;

  return { .x = x, .y = y, .tag = Position::Tag::BOTTOM_RIGHT_CORNER };
}

GUIRectangle::Position GUIRectangle::getPositionCenter(void) const
{
  const int16_t x = m_rectangleDescription.position.x + static_cast<int16_t>(m_rectangleDescription.width / 2);
  const int16_t y = m_rectangleDescription.position.y + static_cast<int16_t>(m_rectangleDescription.height / 2);

  return { .x = x, .y = y, .tag = Position::Tag::CENTER };
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

DMA2D::FillRectangleConfig
GUIRectangle::buildFillRectangleConfig(const RectangleDescription &rectangleDescription, IFrameBuffer &frameBuffer)
{
  DMA2D::FillRectangleConfig fillRectangleConfig =
  {
    .color =
    {
      .alpha = 0u,
      .red   = rectangleDescription.color.red,
      .green = rectangleDescription.color.green,
      .blue  = rectangleDescription.color.blue,
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

  fillRectangleConfig.position.x = static_cast<uint16_t>(rectangleDescription.position.x);
  fillRectangleConfig.position.y = static_cast<uint16_t>(rectangleDescription.position.y);
  fillRectangleConfig.dimension.width  = rectangleDescription.width;
  fillRectangleConfig.dimension.height = rectangleDescription.height;

  return fillRectangleConfig;
}

void GUIRectangle::recalculatePositionToBeTopLeftCorner(GUIRectangle::RectangleDescription &rectangleDescription)
{
  switch (rectangleDescription.position.tag)
  {
    case Position::Tag::CENTER:
      rectangleDescription.position.x -= (rectangleDescription.width / 2u);
      rectangleDescription.position.y -= (rectangleDescription.height / 2u);
      break;

    case Position::Tag::TOP_LEFT_CORNER:
      // leave it as it is
      break;

    case Position::Tag::TOP_RIGHT_CORNER:
      rectangleDescription.position.x -= (rectangleDescription.width - 1u);
      break;

    case Position::Tag::BOTTOM_LEFT_CORNER:
      rectangleDescription.position.y -= (rectangleDescription.height - 1u);
      break;

    case Position::Tag::BOTTOM_RIGHT_CORNER:
      rectangleDescription.position.x -= (rectangleDescription.width - 1u);
      rectangleDescription.position.y -= (rectangleDescription.height - 1u);
      break;

    default:
      // leave it as it is
      break;
  }

  rectangleDescription.position.tag = Position::Tag::TOP_LEFT_CORNER;
}