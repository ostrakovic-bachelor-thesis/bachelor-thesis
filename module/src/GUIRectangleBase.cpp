#include "GUIRectangleBase.h"


GUIRectangleBase::GUIRectangleBase(IFrameBuffer &frameBuffer):
  m_frameBuffer(frameBuffer),
  m_rectangleBaseDescription{
    .dimension = {
      .width  = 0u,
      .height = 0u
    },
    .position = {
      .x   = 0,
      .y   = 0,
      .tag = Position::Tag::TOP_LEFT_CORNER
    }}
{}

void GUIRectangleBase::init(const GUIRectangleBaseDescription &rectangleDescription)
{
  m_rectangleBaseDescription = rectangleDescription;
  recalculatePositionToBeTopLeftCorner(m_rectangleBaseDescription);
}

GUIRectangleBase::Position GUIRectangleBase::getPosition(Position::Tag positionTag) const
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

void GUIRectangleBase::moveToPosition(const Position &position)
{
  m_rectangleBaseDescription.position = position;
  recalculatePositionToBeTopLeftCorner(m_rectangleBaseDescription);
}

void GUIRectangleBase::draw(DrawHardware drawHardware)
{
  // do nothing
}

uint16_t GUIRectangleBase::getVisiblePartWidth(void) const
{
  const int16_t frameBufferWidth = static_cast<int16_t>(m_frameBuffer.getWidth());
  const Position topLeftCornerPosition = getPosition(Position::Tag::TOP_LEFT_CORNER);
  const Position bottomRightCornerPosition = getPosition(Position::Tag::BOTTOM_RIGHT_CORNER);
  int16_t visiblePartWidth = static_cast<int16_t>(m_rectangleBaseDescription.dimension.width);

  if (0 > topLeftCornerPosition.x)
  {
    visiblePartWidth += topLeftCornerPosition.x;
  }
  else if (frameBufferWidth <= bottomRightCornerPosition.x)
  {
    visiblePartWidth = frameBufferWidth - topLeftCornerPosition.x;
  }

  visiblePartWidth = saturateValue(visiblePartWidth, static_cast<int16_t>(0), frameBufferWidth);

  return static_cast<uint16_t>(visiblePartWidth);
}

uint16_t GUIRectangleBase::getVisiblePartHeight(void) const
{
  const int16_t frameBufferHeight = static_cast<int16_t>(m_frameBuffer.getHeight());
  const Position topLeftCornerPosition = getPosition(Position::Tag::TOP_LEFT_CORNER);
  const Position bottomRightCornerPosition = getPosition(Position::Tag::BOTTOM_RIGHT_CORNER);
  int16_t visiblePartHeight = static_cast<int16_t>(m_rectangleBaseDescription.dimension.height);

  if (0 > topLeftCornerPosition.y)
  {
    visiblePartHeight += topLeftCornerPosition.y;
  }
  else if (frameBufferHeight <= bottomRightCornerPosition.y)
  {
    visiblePartHeight = frameBufferHeight - topLeftCornerPosition.y;
  }

  visiblePartHeight = saturateValue(visiblePartHeight, static_cast<int16_t>(0), frameBufferHeight);

  return static_cast<uint16_t>(visiblePartHeight);
}

GUIRectangleBase::Dimension GUIRectangleBase::getVisiblePartDimension(void) const
{
  return
  {
    .width  = getVisiblePartWidth(),
    .height = getVisiblePartHeight()
  };
}

GUIRectangleBase::Position GUIRectangleBase::getVisiblePartPosition(Position::Tag positionTag) const
{
  Position position = getPosition(positionTag);

  position.x = saturateValue(position.x, static_cast<int16_t>(0), static_cast<int16_t>(m_frameBuffer.getWidth()));
  position.y = saturateValue(position.y, static_cast<int16_t>(0), static_cast<int16_t>(m_frameBuffer.getHeight()));

  return position;
}

inline GUIRectangleBase::Position GUIRectangleBase::getPositionTopLeftCorner(void) const
{
  return m_rectangleBaseDescription.position;
}

inline GUIRectangleBase::Position GUIRectangleBase::getPositionTopRightCorner(void) const
{
  const int16_t x = m_rectangleBaseDescription.position.x + static_cast<int16_t>(m_rectangleBaseDescription.dimension.width) - 1;
  const int16_t y = m_rectangleBaseDescription.position.y;

  return { .x = x, .y = y, .tag = Position::Tag::TOP_RIGHT_CORNER };
}

inline GUIRectangleBase::Position GUIRectangleBase::getPositionBottomLeftCorner(void) const
{
  const int16_t x = m_rectangleBaseDescription.position.x;
  const int16_t y = m_rectangleBaseDescription.position.y + static_cast<int16_t>(m_rectangleBaseDescription.dimension.height) - 1;

  return { .x = x, .y = y, .tag = Position::Tag::BOTTOM_LEFT_CORNER };
}

inline GUIRectangleBase::Position GUIRectangleBase::getPositionBottomRightCorner(void) const
{
  const int16_t x = m_rectangleBaseDescription.position.x + static_cast<int16_t>(m_rectangleBaseDescription.dimension.width) - 1;
  const int16_t y = m_rectangleBaseDescription.position.y + static_cast<int16_t>(m_rectangleBaseDescription.dimension.height) - 1;

  return { .x = x, .y = y, .tag = Position::Tag::BOTTOM_RIGHT_CORNER };
}

inline GUIRectangleBase::Position GUIRectangleBase::getPositionCenter(void) const
{
  const int16_t x = m_rectangleBaseDescription.position.x + static_cast<int16_t>(m_rectangleBaseDescription.dimension.width / 2);
  const int16_t y = m_rectangleBaseDescription.position.y + static_cast<int16_t>(m_rectangleBaseDescription.dimension.height / 2);

  return { .x = x, .y = y, .tag = Position::Tag::CENTER };
}

void GUIRectangleBase::recalculatePositionToBeTopLeftCorner(GUIRectangleBaseDescription &rectangleDescription)
{
  switch (rectangleDescription.position.tag)
  {
    case Position::Tag::CENTER:
      rectangleDescription.position.x -= (rectangleDescription.dimension.width / 2u);
      rectangleDescription.position.y -= (rectangleDescription.dimension.height / 2u);
      break;

    case Position::Tag::TOP_LEFT_CORNER:
      // leave it as it is
      break;

    case Position::Tag::TOP_RIGHT_CORNER:
      rectangleDescription.position.x -= (rectangleDescription.dimension.width - 1u);
      break;

    case Position::Tag::BOTTOM_LEFT_CORNER:
      rectangleDescription.position.y -= (rectangleDescription.dimension.height - 1u);
      break;

    case Position::Tag::BOTTOM_RIGHT_CORNER:
      rectangleDescription.position.x -= (rectangleDescription.dimension.width - 1u);
      rectangleDescription.position.y -= (rectangleDescription.dimension.height - 1u);
      break;

    default:
      // leave it as it is
      break;
  }

  rectangleDescription.position.tag = Position::Tag::TOP_LEFT_CORNER;
}