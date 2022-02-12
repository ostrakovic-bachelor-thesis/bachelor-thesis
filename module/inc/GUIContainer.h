#ifndef GUI_CONTAINER_H
#define GUI_CONTAINER_H

#include "IGUIContainer.h"
#include "IArrayList.h"
#include "IGUIObject.h"
#include "IFrameBuffer.h"


namespace GUI
{
  class Container : public IContainer
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

    Iterator getBeginIterator(void);
    Iterator getEndIterator(void);

    IFrameBuffer& getFrameBuffer(void) override;
    const IFrameBuffer& getFrameBuffer(void) const override;
    void setFrameBuffer(IFrameBuffer &frameBuffer) override;

    Position getPosition(Position::Tag positionTag) const override;

    uint32_t getCapacity(void) const override;
    uint32_t getSize(void) const override;
    bool isEmpty(void) const override;

    IObject* getObject(uint32_t zIndex) override;
    ErrorCode addObject(IObject *objectPtr, uint32_t zIndex) override;

    void draw(DrawHardware drawHardware) override;
    bool isDrawCompleted(void) const override;
    ErrorCode getDrawingTime(DrawHardware drawHardware, uint64_t &drawingTimeInUs) const override;

    void registerDrawCompletedCallback(const CallbackDescription &callbackDescription) override;
    void unregisterDrawCompletedCallback(void) override;

    IObject* getEventTarget(const TouchEvent &touchEvent) override;
    void dispatchEvent(TouchEvent &touchEvent) override;

  private:

    bool doesGUIObjectContainAnyOfTouchPoints(
      const IObject &guiObject,
      const IArrayList<Point> &touchPoints);

    Position getPositionTopLeftCorner(void) const;
    Position getPositionTopRightCorner(void) const;
    Position getPositionBottomLeftCorner(void) const;
    Position getPositionBottomRightCorner(void) const;
    Position getPositionCenter(void) const;

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

    IFrameBuffer *m_frameBufferPtr;

    Iterator m_currentDrawingObjectIterator;

    CallbackDescription m_drawCompletedCallback;

    bool m_isDrawingCompleted = true;

    DrawHardware m_drawHardwareInUsage = DrawHardware::CPU;
  };
}

#endif // #ifndef GUI_CONTAINER_H