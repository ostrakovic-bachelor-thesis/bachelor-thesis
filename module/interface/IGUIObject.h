#ifndef I_GUI_OBJECT_H
#define I_GUI_OBJECT_H

#include "GUICommon.h"
#include "IGUIObjectBase.h"
#include "IGUIDrawable.h"
#include "IGUIMovable.h"
#include "IFrameBuffer.h"
#include <cstdint>


namespace GUI
{
  class IObject : public IObjectBase, public IDrawable, public IMovable
  {
  public:
    virtual ~IObject() = default;

    virtual uint16_t getWidth(void) const = 0;
    virtual uint16_t getHeight(void) const = 0;
    virtual Dimension getDimension(void) const = 0;
  };
}

#endif // #ifndef I_GUI_OBJECT_H