#ifndef I_GUI_SCENE_H
#define I_GUI_SCENE_H

#include "IGUIObject.h"
#include "IFrameBuffer.h"
#include <cstdint>


namespace GUI
{
  class IScene
  {
  public:
    virtual ~IScene() = default;

    virtual IFrameBuffer& getFrameBuffer(void) = 0;
    virtual const IFrameBuffer& getFrameBuffer(void) const = 0;

    virtual uint32_t getCapacity(void) const = 0;
    virtual uint32_t getObjectsCount(void) const = 0;

    virtual bool isEmpty(void) const = 0;

    virtual IObject* getObject(uint32_t zIndex) = 0;

    virtual ErrorCode addObject(IObject *guiObjectPtr, uint32_t zIndex) = 0;
  };
}

#endif // #ifndef I_GUI_SCENE_H