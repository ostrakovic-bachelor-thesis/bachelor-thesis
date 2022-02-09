#ifndef I_GUI_OBJECT_H
#define I_GUI_OBJECT_H

#include "IGUIObjectBase.h"
#include "IGUIDrawable.h"
#include "IGUIMovable.h"
#include "IGUIShape.h"


namespace GUI
{
  class IObject : public IObjectBase, public IDrawable, public IMovable, public IShape
  {
  public:
    virtual ~IObject() = default;
  };
}

#endif // #ifndef I_GUI_OBJECT_H