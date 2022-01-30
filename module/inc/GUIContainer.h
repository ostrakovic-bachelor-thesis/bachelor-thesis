#ifndef GUI_CONTAINER_H
#define GUI_CONTAINER_H

#include "IGUIDrawable.h"
#include "IArrayList.h"
#include "IGUIObject.h"
#include "IFrameBuffer.h"


namespace GUI
{
  class Container : public IDrawable
  {
  public:

    struct ObjectInfo
    {
      IObject *objectPtr;
      uint32_t zIndex;
    };

    Container(IArrayList<ObjectInfo> &objectInfoList, IFrameBuffer &frameBuffer);

    class Iterator
    {
    public:
      Iterator(IArrayList<ObjectInfo>::Iterator objectInfoListIterator);

      bool operator==(const Iterator &other) const;
      bool operator!=(const Iterator &other) const;

      IObject* operator*();

      IObject* operator++(int);

      uint32_t getZIndex(void) const;

    private:

      IArrayList<ObjectInfo>::Iterator m_objectInfoListIterator;
    };

    IFrameBuffer& getFrameBuffer(void);

    uint32_t getCapacity(void) const;
    uint32_t getSize(void) const;
    bool isEmpty(void) const;

    IObject* getObject(uint32_t zIndex);
    ErrorCode addObject(IObject *objectPtr, uint32_t zIndex);


    Iterator getBeginIterator(void);
    Iterator getEndIterator(void);

    void draw(DrawHardware drawHardware) override;
    bool isDrawCompleted(void) const override;
    ErrorCode getDrawingTime(DrawHardware drawHardware, uint64_t &drawingTimeInUs) const override;

    void registerDrawCompletedCallback(const CallbackDescription &callbackDescription) override;
    void unregisterDrawCompletedCallback(void) override;

  private:

    void drawDMA2D(void);
    void drawCPU(void);

    void startDrawingTransaction(DrawHardware drawHardware);
    void endDrawingTransaction(void);

    void callDrawCompletedCallbackIfRegistered(void);

    bool startDrawingOfTheNextObjectWithDMA2D(void);

    IObject* findObjectAtZIndex(uint32_t zIndex) const;

    ErrorCode insertObjectInfoIntoList(const ObjectInfo &objectInfo);

    static ErrorCode mapToErrorCode(IArrayListBase::ErrorCode errorCode);

    static void objectDrawingCompletedCallback(void *guiContainerPtr);

    IArrayList<ObjectInfo> &m_objectInfoList;

    IFrameBuffer &m_frameBuffer;

    Iterator m_currentDrawingObjectIterator;

    CallbackDescription m_drawCompletedCallback;

    bool m_isDrawingCompleted = true;

    DrawHardware m_drawHardwareInUsage = DrawHardware::CPU;
  };
}

#endif // #ifndef GUI_CONTAINER_H