#include "GUISceneBase.h"


GUI::SceneBase::SceneBase(IArrayList<GUIObjectInfo> &guiObjectInfoList, IFrameBuffer &frameBuffer):
  m_guiObjectInfoList(guiObjectInfoList),
  m_frameBuffer(frameBuffer)
{}

uint32_t GUI::SceneBase::getCapacity(void) const
{
  return m_guiObjectInfoList.getCapacity();
}

uint32_t GUI::SceneBase::getObjectsCount(void) const
{
  return m_guiObjectInfoList.getSize();
}

bool GUI::SceneBase::isEmpty(void) const
{
  return m_guiObjectInfoList.isEmpty();
}

GUI::IObject* GUI::SceneBase::getObject(uint32_t zIndex)
{
  IObject *guiObjectPtr = nullptr;

  for (auto it = m_guiObjectInfoList.getBeginIterator(); it != m_guiObjectInfoList.getEndIterator(); ++it)
  {
    if (zIndex == it->zIndex)
    {
      guiObjectPtr = it->guiObjectPtr;
      break;
    }
  }

  return guiObjectPtr;
}

GUI::ErrorCode GUI::SceneBase::addObject(IObject *guiObjectPtr, uint32_t zIndex)
{
  GUIObjectInfo guiObjectInfo =
  {
    .guiObjectPtr = guiObjectPtr,
    .zIndex       = zIndex
  };

  ErrorCode errorCode = insertGUIObjectInfoIntoList(guiObjectInfo);

  if (ErrorCode::OK == errorCode)
  {
    guiObjectPtr->setFrameBuffer(m_frameBuffer);
  }

  return errorCode;
}

GUI::SceneBase::Iterator GUI::SceneBase::getBeginIterator(void)
{
  return m_guiObjectInfoList.getBeginIterator();
}

GUI::SceneBase::Iterator GUI::SceneBase::getEndIterator(void)
{
  return m_guiObjectInfoList.getEndIterator();
}

GUI::ErrorCode GUI::SceneBase::insertGUIObjectInfoIntoList(const GUIObjectInfo &guiObjectInfo)
{
  ErrorCode errorCode = ErrorCode::OK;

  auto iterator = m_guiObjectInfoList.getBeginIterator();
  while ((m_guiObjectInfoList.getEndIterator() != iterator) && (iterator->zIndex < guiObjectInfo.zIndex))
  {
    ++iterator;
  }

  if (m_guiObjectInfoList.getEndIterator() == iterator)
  {
    errorCode = mapToErrorCode(m_guiObjectInfoList.addElement(guiObjectInfo));
  }
  else if (guiObjectInfo.zIndex == iterator->zIndex)
  {
    errorCode = ErrorCode::Z_INDEX_ALREADY_IN_USAGE;
  }
  else
  {
    errorCode = mapToErrorCode(m_guiObjectInfoList.addElement(iterator - m_guiObjectInfoList.getBeginIterator(), guiObjectInfo));
  }

  return errorCode;
}

GUI::ErrorCode GUI::SceneBase::mapToErrorCode(IArrayListBase::ErrorCode errorCode)
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