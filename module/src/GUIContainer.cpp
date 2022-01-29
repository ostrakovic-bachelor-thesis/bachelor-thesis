#include "GUIContainer.h"


GUI::Container::Container(IArrayList<ObjectInfo> &objectInfoList, IFrameBuffer &frameBuffer):
  m_objectInfoList(objectInfoList),
  m_frameBuffer(frameBuffer)
{}

IFrameBuffer& GUI::Container::getFrameBuffer(void)
{
  return m_frameBuffer;
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
    IDrawable::CallbackDescription callbackDescription =
    {
      .functionPtr = nullptr,
      .argument    = nullptr
    };

    objectPtr->setFrameBuffer(m_frameBuffer);
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
  m_isDrawingCompleted = false;

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
      m_isDrawingCompleted = true;
    }
    break;
  }
}

bool GUI::Container::isDrawCompleted(void) const
{
  return m_isDrawingCompleted;
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
  for (Iterator it = getBeginIterator(); it != getEndIterator(); it++)
  {
    (*it)->draw(DrawHardware::DMA2D);
  }
}

void GUI::Container::drawCPU(void)
{
  for (Iterator it = getBeginIterator(); it != getEndIterator(); it++)
  {
    (*it)->draw(DrawHardware::CPU);
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