#ifndef I_GUI_DRAWABLE_H
#define I_GUI_DRAWABLE_H

#include "GUICommon.h"


namespace GUI
{
  class IDrawable
  {
  public:
    typedef void (*CallbackFunc)(void*);

    virtual ~IDrawable() = default;

    struct CallbackDescription
    {
      CallbackFunc functionPtr;
      void *argument;
    };

    virtual void draw(DrawHardware drawHardware) = 0;
    virtual bool isDrawCompleted(void) const = 0;
    virtual ErrorCode getDrawingTime(DrawHardware drawHardware, uint64_t &drawingTimeInUs) const = 0;

    virtual void registerDrawCompletedCallback(const CallbackDescription &callbackDescription) = 0;
    virtual void unregisterDrawCompletedCallback(void) = 0;
  };
}

#endif // #ifndef I_GUI_DRAWABLE_H