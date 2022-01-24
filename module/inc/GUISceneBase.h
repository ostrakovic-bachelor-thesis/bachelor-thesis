#ifndef GUI_SCENE_BASE_H
#define GUI_SCENE_BASE_H

#include "IGUIScene.h"
#include "IArrayList.h"
#include "IFrameBuffer.h"


namespace GUI
{
  class SceneBase : public IScene
  {
  public:

    struct GUIObjectInfo
    {
      IObject *guiObjectPtr;
      uint32_t zIndex;
    };

    using Iterator = GUIObjectInfo*;

    SceneBase(IArrayList<GUIObjectInfo> &guiObjectInfoList, IFrameBuffer &frameBuffer);

    inline IFrameBuffer& getFrameBuffer(void) override
    {
      return m_frameBuffer;
    }

    inline const IFrameBuffer& getFrameBuffer(void) const override
    {
      return m_frameBuffer;
    }

    uint32_t getCapacity(void) const override;

    uint32_t getObjectsCount(void) const override;

    bool isEmpty(void) const override;

    IObject* getObject(uint32_t zIndex) override;

    ErrorCode addObject(IObject *guiObjectPtr, uint32_t zIndex) override;

    Iterator getBeginIterator(void);

    Iterator getEndIterator(void);

  private:

    ErrorCode insertGUIObjectInfoIntoList(const GUIObjectInfo &guiObjectInfo);

    static ErrorCode mapToErrorCode(IArrayListBase::ErrorCode errorCode);

    IArrayList<GUIObjectInfo> &m_guiObjectInfoList;

    IFrameBuffer &m_frameBuffer;
  };
}

#endif // #ifndef GUI_SCENE_BASE_H