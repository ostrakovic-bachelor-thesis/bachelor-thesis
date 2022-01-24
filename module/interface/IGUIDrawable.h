#ifndef I_GUI_DRAWABLE_H
#define I_GUI_DRAWABLE_H

#include "GUICommon.h"


namespace GUI
{
  class IDrawable
  {
  public:
    virtual ~IDrawable() = default;

    virtual void draw(DrawHardware drawHardware) = 0;
    virtual bool isDrawCompleted(void) const = 0;
    virtual ErrorCode getDrawingTime(DrawHardware drawHardware, uint64_t &drawingTimeInUs) const = 0;
  };
}

#endif // #ifndef I_GUI_DRAWABLE_H