#ifndef I_GUI_CONTAINER_H
#define I_GUI_CONTAINER_H

#include "IGUIDrawable.h"
#include "IGUIObject.h"
#include "IGUIObjectBase.h"
#include "GUITouchEvent.h"
#include <cstdint>


namespace GUI
{
  class IContainer : public IObjectBase, public IDrawable
  {
  public:
    virtual ~IContainer() = default;

    virtual uint32_t getCapacity(void) const = 0;
    virtual uint32_t getSize(void) const = 0;
    virtual bool isEmpty(void) const = 0;

    virtual IObject* getObject(uint32_t zIndex) = 0;
    virtual ErrorCode addObject(IObject *objectPtr, uint32_t zIndex) = 0;

    virtual IObject* getEventTarget(const TouchEvent &touchEvent) = 0;
    virtual void dispatchEvent(TouchEvent &touchEvent) = 0;
  };
}

#endif // #ifndef I_GUI_CONTAINER_H