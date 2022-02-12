#include "GUIRectangleBase.h"


GUI::RectangleBase::RectangleBase(SysTick &sysTick, IFrameBuffer &frameBuffer):
  m_sysTick(sysTick),
  m_frameBufferPtr(&frameBuffer),
  m_rectangleBaseDescription{
    .dimension = {
      .width  = 0u,
      .height = 0u
    },
    .position = {
      .x   = 0,
      .y   = 0,
      .tag = Position::Tag::TOP_LEFT_CORNER
    }},
  m_drawCompletedCallback{
    .functionPtr = nullptr,
    .argument    = nullptr
  },
  m_touchEventCallback{
    .functionPtr = nullptr,
    .argument    = nullptr
  }
{}

void GUI::RectangleBase::init(const RectangleBaseDescription &rectangleDescription)
{
  m_rectangleBaseDescription = rectangleDescription;
  recalculatePositionToBeTopLeftCorner(m_rectangleBaseDescription);
}

IFrameBuffer& GUI::RectangleBase::getFrameBuffer(void)
{
  return *m_frameBufferPtr;
}

const IFrameBuffer& GUI::RectangleBase::getFrameBuffer(void) const
{
  return *m_frameBufferPtr;
}

void GUI::RectangleBase::setFrameBuffer(IFrameBuffer &frameBuffer)
{
  m_frameBufferPtr = &frameBuffer;
}

bool GUI::RectangleBase::doesContainPoint(Point point) const
{
  const Position topLeftCorner     = getPosition(Position::Tag::TOP_LEFT_CORNER);
  const Position bottomRightCorner = getPosition(Position::Tag::BOTTOM_RIGHT_CORNER);

  return (point.x >= topLeftCorner.x) && (point.x <= bottomRightCorner.x) &&
         (point.y >= topLeftCorner.y) && (point.y <= bottomRightCorner.y);
}

GUI::Position GUI::RectangleBase::getPosition(Position::Tag positionTag) const
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

void GUI::RectangleBase::moveToPosition(const Position &position)
{
  m_rectangleBaseDescription.position = position;
  recalculatePositionToBeTopLeftCorner(m_rectangleBaseDescription);
}

void GUI::RectangleBase::draw(DrawHardware drawHardware)
{
  if (isVisibleOnTheScreen())
  {
    startDrawingTransaction(drawHardware);

    switch (drawHardware)
    {
      case DrawHardware::DMA2D:
      {
        drawDMA2D();
      }
      break;

      default:
      case DrawHardware::CPU:
      {
        drawCPU();
        endDrawingTransaction(drawHardware);
        callDrawCompletedCallbackIfRegistered();
      }
      break;
    }
  }
  else
  {
    callDrawCompletedCallbackIfRegistered();
  }
}

bool GUI::RectangleBase::isDrawCompleted(void) const
{
  return m_isDrawingCompleted;
}

GUI::ErrorCode GUI::RectangleBase::getDrawingTime(DrawHardware drawHardware, uint64_t &drawingTimeInUs) const
{
  const DrawingDurationInfo *drawingDurationInfoPtr = &m_drawingDurationInfo[static_cast<uint8_t>(drawHardware)];
  ErrorCode errorCode = GUI::ErrorCode::MEASUREMENT_NOT_AVAILABLE;

  if (drawingDurationInfoPtr->isDrawnAtLeastOnce)
  {
    drawingTimeInUs = calculateDrawingTime(drawingDurationInfoPtr);
    errorCode = ErrorCode::OK;
  }

  return errorCode;
}

void GUI::RectangleBase::registerDrawCompletedCallback(const CallbackDescription &callbackDescription)
{
  m_drawCompletedCallback = callbackDescription;
}

void GUI::RectangleBase::unregisterDrawCompletedCallback(void)
{
  m_drawCompletedCallback =
  {
    .functionPtr = nullptr,
    .argument    = nullptr
  };
}

void GUI::RectangleBase::notify(const TouchEvent &touchEvent)
{
  if (nullptr != m_touchEventCallback.functionPtr)
  {
    m_touchEventCallback.functionPtr(m_touchEventCallback.argument, *this, touchEvent);
  }
}

void GUI::RectangleBase::registerTouchEventCallback(const TouchEventCallbackDescription &callbackDescription)
{
  m_touchEventCallback = callbackDescription;
}

void GUI::RectangleBase::unregisterTouchEventCallback(void)
{
  m_touchEventCallback =
  {
    .functionPtr = nullptr,
    .argument    = nullptr
  };
}

uint16_t GUI::RectangleBase::getVisiblePartWidth(void) const
{
  const int16_t frameBufferWidth = static_cast<int16_t>(m_frameBufferPtr->getWidth());
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

uint16_t GUI::RectangleBase::getVisiblePartHeight(void) const
{
  const int16_t frameBufferHeight = static_cast<int16_t>(m_frameBufferPtr->getHeight());
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

GUI::Dimension GUI::RectangleBase::getVisiblePartDimension(void) const
{
  return
  {
    .width  = getVisiblePartWidth(),
    .height = getVisiblePartHeight()
  };
}

uint64_t GUI::RectangleBase::getVisiblePartArea(void) const
{
  return getVisiblePartWidth() * getVisiblePartHeight();
}

GUI::Position GUI::RectangleBase::getVisiblePartPosition(Position::Tag positionTag) const
{
  Position position = getPosition(positionTag);

  position.x = saturateValue(position.x, static_cast<int16_t>(0), static_cast<int16_t>(m_frameBufferPtr->getWidth() - 1u));
  position.y = saturateValue(position.y, static_cast<int16_t>(0), static_cast<int16_t>(m_frameBufferPtr->getHeight() - 1u));

  return position;
}

inline GUI::Position GUI::RectangleBase::getPositionTopLeftCorner(void) const
{
  return m_rectangleBaseDescription.position;
}

inline GUI::Position GUI::RectangleBase::getPositionTopRightCorner(void) const
{
  const int16_t x = m_rectangleBaseDescription.position.x + static_cast<int16_t>(m_rectangleBaseDescription.dimension.width) - 1;
  const int16_t y = m_rectangleBaseDescription.position.y;

  return { .x = x, .y = y, .tag = Position::Tag::TOP_RIGHT_CORNER };
}

inline GUI::Position GUI::RectangleBase::getPositionBottomLeftCorner(void) const
{
  const int16_t x = m_rectangleBaseDescription.position.x;
  const int16_t y = m_rectangleBaseDescription.position.y + static_cast<int16_t>(m_rectangleBaseDescription.dimension.height) - 1;

  return { .x = x, .y = y, .tag = Position::Tag::BOTTOM_LEFT_CORNER };
}

inline GUI::Position GUI::RectangleBase::getPositionBottomRightCorner(void) const
{
  const int16_t x = m_rectangleBaseDescription.position.x + static_cast<int16_t>(m_rectangleBaseDescription.dimension.width) - 1;
  const int16_t y = m_rectangleBaseDescription.position.y + static_cast<int16_t>(m_rectangleBaseDescription.dimension.height) - 1;

  return { .x = x, .y = y, .tag = Position::Tag::BOTTOM_RIGHT_CORNER };
}

inline GUI::Position GUI::RectangleBase::getPositionCenter(void) const
{
  const int16_t x = m_rectangleBaseDescription.position.x + static_cast<int16_t>(m_rectangleBaseDescription.dimension.width / 2);
  const int16_t y = m_rectangleBaseDescription.position.y + static_cast<int16_t>(m_rectangleBaseDescription.dimension.height / 2);

  return { .x = x, .y = y, .tag = Position::Tag::CENTER };
}

bool GUI::RectangleBase::isVisibleOnTheScreen(void) const
{
  return (0u != getVisiblePartWidth()) && (0u != getVisiblePartHeight());
}

void GUI::RectangleBase::startDrawingTransaction(DrawHardware drawHardware)
{
  DrawingDurationInfo *drawingDurationInfoPtr = &m_drawingDurationInfo[static_cast<uint8_t>(drawHardware)];

  m_isDrawingCompleted                               = false;
  drawingDurationInfoPtr->drawStartTimestamp         = m_sysTick.getTicks();
  drawingDurationInfoPtr->lastDrawingVisiblePartArea = getVisiblePartArea();
}

void GUI::RectangleBase::endDrawingTransaction(DrawHardware drawHardware)
{
  DrawingDurationInfo *drawingDurationInfoPtr = &m_drawingDurationInfo[static_cast<uint8_t>(drawHardware)];

  drawingDurationInfoPtr->lastDrawingDurationInUs =
    m_sysTick.getElapsedTimeInUs(drawingDurationInfoPtr->drawStartTimestamp);
  drawingDurationInfoPtr->isDrawnAtLeastOnce = true;
  m_isDrawingCompleted                       = true;
}

uint64_t GUI::RectangleBase::calculateDrawingTime(const DrawingDurationInfo *drawingDurationInfoPtr) const
{
  uint64_t drawingTimeInUs =  drawingDurationInfoPtr->lastDrawingDurationInUs;
  drawingTimeInUs *= getVisiblePartArea();
  drawingTimeInUs /= drawingDurationInfoPtr->lastDrawingVisiblePartArea;

  return drawingTimeInUs;
}

void GUI::RectangleBase::callDrawCompletedCallbackIfRegistered(void)
{
  if (nullptr != m_drawCompletedCallback.functionPtr)
  {
    m_drawCompletedCallback.functionPtr(m_drawCompletedCallback.argument);
  }
}

void GUI::RectangleBase::recalculatePositionToBeTopLeftCorner(RectangleBaseDescription &rectangleDescription)
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

void GUI::RectangleBase::callbackDMA2DDrawCompleted(void *guiRectangleBasePtr)
{
  GUI::RectangleBase *rectangleBasePtr = reinterpret_cast<GUI::RectangleBase*>(guiRectangleBasePtr);

  if (nullptr != rectangleBasePtr)
  {
    rectangleBasePtr->endDrawingTransaction(DrawHardware::DMA2D);
    rectangleBasePtr->callDrawCompletedCallbackIfRegistered();
  }
}