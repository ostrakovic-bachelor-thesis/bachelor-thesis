#include "GUIContainer.h"


GUI::Container::Container(IArrayList<ObjectInfo> &objectInfoList, IFrameBuffer &frameBuffer):
  m_objectInfoList(objectInfoList),
  m_frameBufferPtr(&frameBuffer),
  m_currentDrawingObjectIterator(m_objectInfoList.getEndIterator())
{}

IFrameBuffer& GUI::Container::getFrameBuffer(void)
{
  return *m_frameBufferPtr;
}

const IFrameBuffer& GUI::Container::getFrameBuffer(void) const
{
  return *m_frameBufferPtr;
}

void GUI::Container::setFrameBuffer(IFrameBuffer &frameBuffer)
{
  m_frameBufferPtr = &frameBuffer;

  for (auto it = getBeginIterator(); it != getEndIterator(); it++)
  {
    (*it)->setFrameBuffer(frameBuffer);
  }
}

GUI::Position GUI::Container::getPosition(Position::Tag positionTag) const
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

uint32_t GUI::Container::getCapacity(void) const
{
  return m_objectInfoList.getCapacity();
}

uint32_t GUI::Container::getSize(void) const
{
  return m_objectInfoList.getSize();
}

bool GUI::Container::isEmpty(void) const
{
  return m_objectInfoList.isEmpty();
}

GUI::IObject* GUI::Container::getObject(uint32_t zIndex)
{
  return findObjectAtZIndex(zIndex);
}

GUI::ErrorCode GUI::Container::addObject(IObject *objectPtr, uint32_t zIndex)
{
  ErrorCode errorCode = ErrorCode::OK;

  if (nullptr == objectPtr)
  {
    errorCode = ErrorCode::ARGUMENT_NULL_POINTER;
  }

  if (ErrorCode::OK == errorCode)
  {
    ObjectInfo objectInfo =
    {
      .objectPtr = objectPtr,
      .zIndex    = zIndex
    };

    errorCode = insertObjectInfoIntoList(objectInfo);
  }

  if (ErrorCode::OK == errorCode)
  {
    const IDrawable::CallbackDescription callbackDescription =
    {
      .functionPtr = objectDrawingCompletedCallback,
      .argument    = this
    };

    objectPtr->setFrameBuffer(getFrameBuffer());
    objectPtr->registerDrawCompletedCallback(callbackDescription);
  }

  return errorCode;
}

GUI::Container::Iterator GUI::Container::getBeginIterator(void)
{
  return Iterator(m_objectInfoList.getBeginIterator());
}

GUI::Container::Iterator GUI::Container::getEndIterator(void)
{
  return Iterator(m_objectInfoList.getEndIterator());
}

void GUI::Container::draw(DrawHardware drawHardware)
{
  if (not isEmpty())
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
        endDrawingTransaction();
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

bool GUI::Container::isDrawCompleted(void) const
{
  return m_isDrawingCompleted;
}

GUI::ErrorCode GUI::Container::getDrawingTime(DrawHardware drawHardware, uint64_t &drawingTimeInUs) const
{
  ErrorCode errorCode = ErrorCode::OK;
  uint64_t containerDrawingTimeInUs = 0u;
  uint64_t objectDrawingTimeInUs;

  for (auto it = m_objectInfoList.getBeginIterator(); it != m_objectInfoList.getEndIterator(); ++it)
  {
    if (ErrorCode::OK == errorCode)
    {
      errorCode = it->objectPtr->getDrawingTime(drawHardware, objectDrawingTimeInUs);
      containerDrawingTimeInUs += objectDrawingTimeInUs;
    }
  }

  drawingTimeInUs = containerDrawingTimeInUs;

  return errorCode;
}

void GUI::Container::registerDrawCompletedCallback(const CallbackDescription &callbackDescription)
{
  m_drawCompletedCallback = callbackDescription;
}

void GUI::Container::unregisterDrawCompletedCallback(void)
{
  m_drawCompletedCallback =
  {
    .functionPtr = nullptr,
    .argument    = nullptr
  };
}

GUI::IObject* GUI::Container::getEventTarget(const TouchEvent &touchEvent)
{
  GUI::IObject *eventTargetPtr = nullptr;

  if (not touchEvent.getTouchPoints().isEmpty())
  {
    for (auto it = getBeginIterator(); it != getEndIterator(); it++)
    {
      if (doesGUIObjectContainAnyOfTouchPoints(**it, touchEvent.getTouchPoints()))
      {
        eventTargetPtr = *it;
      }
    }
  }

  return eventTargetPtr;
}

void GUI::Container::dispatchEvent(TouchEvent &touchEvent)
{
  for (auto it = getBeginIterator(); it != getEndIterator(); it++)
  {
    if (touchEvent.getEventTargetObject() == (*it))
    {
      (*it)->notify(touchEvent);
    }
  }
}

bool GUI::Container::doesGUIObjectContainAnyOfTouchPoints(
  const IObject &guiObject,
  const IArrayList<Point> &touchPoints)
{
  for (auto it = touchPoints.getBeginIterator(); it != touchPoints.getEndIterator(); it++)
  {
    if (guiObject.doesContainPoint(*it))
    {
      return true;
    }
  }

  return false;
}

GUI::Position GUI::Container::getPositionTopLeftCorner(void) const
{
  return
  {
    .x   = 0,
    .y   = 0,
    .tag = GUI::Position::Tag::TOP_LEFT_CORNER
  };
}

GUI::Position GUI::Container::getPositionTopRightCorner(void) const
{
  return
  {
    .x   = static_cast<int16_t>(getFrameBuffer().getWidth()),
    .y   = 0,
    .tag = GUI::Position::Tag::TOP_RIGHT_CORNER
  };
}

GUI::Position GUI::Container::getPositionBottomLeftCorner(void) const
{
  return
  {
    .x   = 0,
    .y   = static_cast<int16_t>(getFrameBuffer().getHeight()),
    .tag = GUI::Position::Tag::BOTTOM_LEFT_CORNER
  };
}

GUI::Position GUI::Container::getPositionBottomRightCorner(void) const
{
  return
  {
    .x   = static_cast<int16_t>(getFrameBuffer().getWidth()),
    .y   = static_cast<int16_t>(getFrameBuffer().getHeight()),
    .tag = GUI::Position::Tag::BOTTOM_RIGHT_CORNER
  };
}

GUI::Position GUI::Container::getPositionCenter(void) const
{
  return
  {
    .x   = static_cast<int16_t>(getFrameBuffer().getWidth() / 2u),
    .y   = static_cast<int16_t>(getFrameBuffer().getHeight() / 2u),
    .tag = GUI::Position::Tag::CENTER
  };
}

GUI::IObject* GUI::Container::findObjectAtZIndex(uint32_t zIndex) const
{
  IObject *objectPtr = nullptr;

  for (auto it = m_objectInfoList.getBeginIterator(); it != m_objectInfoList.getEndIterator(); ++it)
  {
    if (zIndex == it->zIndex)
    {
      objectPtr = it->objectPtr;
      break;
    }
  }

  return objectPtr;
}

GUI::ErrorCode GUI::Container::insertObjectInfoIntoList(const ObjectInfo &objectInfo)
{
  ErrorCode errorCode = ErrorCode::OK;

  auto iterator = m_objectInfoList.getBeginIterator();
  while ((m_objectInfoList.getEndIterator() != iterator) && (iterator->zIndex < objectInfo.zIndex))
  {
    ++iterator;
  }

  if (m_objectInfoList.getEndIterator() == iterator)
  {
    errorCode = mapToErrorCode(m_objectInfoList.addElement(objectInfo));
  }
  else if (objectInfo.zIndex == iterator->zIndex)
  {
    errorCode = ErrorCode::Z_INDEX_ALREADY_IN_USAGE;
  }
  else
  {
    errorCode = mapToErrorCode(m_objectInfoList.addElement(iterator - m_objectInfoList.getBeginIterator(), objectInfo));
  }

  return errorCode;
}

void GUI::Container::drawDMA2D(void)
{
  m_currentDrawingObjectIterator = getBeginIterator();
  (*m_currentDrawingObjectIterator)->draw(DrawHardware::DMA2D);
}

void GUI::Container::drawCPU(void)
{
  m_currentDrawingObjectIterator = getBeginIterator();
  while (getEndIterator() != m_currentDrawingObjectIterator)
  {
    (m_currentDrawingObjectIterator++)->draw(DrawHardware::CPU);
  }
}

void GUI::Container::startDrawingTransaction(DrawHardware drawHardware)
{
  m_isDrawingCompleted  = false;
  m_drawHardwareInUsage = drawHardware;
}

void GUI::Container::endDrawingTransaction(void)
{
  m_isDrawingCompleted = true;
}

void GUI::Container::callDrawCompletedCallbackIfRegistered(void)
{
  if (nullptr != m_drawCompletedCallback.functionPtr)
  {
    m_drawCompletedCallback.functionPtr(m_drawCompletedCallback.argument);
  }
}

GUI::ErrorCode GUI::Container::mapToErrorCode(IArrayListBase::ErrorCode errorCode)
{
  switch (errorCode)
  {
    case IArrayListBase::ErrorCode::CONTAINER_FULL_ERROR:
      return ErrorCode::CONTAINER_FULL_ERROR;

    case IArrayListBase::ErrorCode::OK:
    case IArrayListBase::ErrorCode::OUT_OF_RANGE_ERROR:
    case IArrayListBase::ErrorCode::NULL_POINTER_ERROR:
    default:
      return ErrorCode::OK;
  }
}

bool GUI::Container::startDrawingOfTheNextObjectWithDMA2D(void)
{
  bool isDrawingStartedSuccessfully = false;

  m_currentDrawingObjectIterator++;
  if (getEndIterator() != m_currentDrawingObjectIterator)
  {
    (*(m_currentDrawingObjectIterator))->draw(DrawHardware::DMA2D);
    isDrawingStartedSuccessfully = true;
  }

  return isDrawingStartedSuccessfully;
}

void GUI::Container::objectDrawingCompletedCallback(void *guiContainerPtr)
{
  GUI::Container *containerPtr = reinterpret_cast<GUI::Container*>(guiContainerPtr);

  if (nullptr != containerPtr)
  {
    if (DrawHardware::DMA2D == containerPtr->m_drawHardwareInUsage)
    {
      bool isDrawingStartedSuccessfully = containerPtr->startDrawingOfTheNextObjectWithDMA2D();
      if (not isDrawingStartedSuccessfully)
      {
        containerPtr->endDrawingTransaction();
        containerPtr->callDrawCompletedCallbackIfRegistered();
      }
    }
  }
}

GUI::Container::Iterator::Iterator(IArrayList<ObjectInfo>::Iterator objectInfoListIterator):
  m_objectInfoListIterator(objectInfoListIterator)
{}

bool GUI::Container::Iterator::operator==(const Iterator &other) const
{
  return m_objectInfoListIterator == other.m_objectInfoListIterator;
}

bool GUI::Container::Iterator::operator!=(const Iterator &other) const
{
  return not (other == *this);
}

GUI::IObject* GUI::Container::Iterator::operator*()
{
  return m_objectInfoListIterator->objectPtr;
}

GUI::IObject* GUI::Container::Iterator::operator++(int)
{
  return (m_objectInfoListIterator++)->objectPtr;
}

uint32_t GUI::Container::Iterator::getZIndex(void) const
{
  return m_objectInfoListIterator->zIndex;
}