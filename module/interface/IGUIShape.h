#ifndef I_GUI_SHAPE_H
#define I_GUI_SHAPE_H

#include "GUICommon.h"


namespace GUI
{
  class IShape
  {
  public:
    virtual ~IShape() = default;

    virtual uint16_t getWidth(void) const = 0;
    virtual uint16_t getHeight(void) const = 0;
    virtual Dimension getDimension(void) const = 0;

    virtual bool doesContainPoint(Point point) const = 0;
  };
}

#endif // #ifndef I_GUI_SHAPE_H