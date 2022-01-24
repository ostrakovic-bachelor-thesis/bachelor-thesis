#ifndef I_GUI_OBJECT_BASE_H
#define I_GUI_OBJECT_BASE_H

#include "GUICommon.h"
#include "IFrameBuffer.h"
#include <cstdint>


namespace GUI
{
  class IObjectBase
  {
  public:
    virtual ~IObjectBase() = default;

    virtual IFrameBuffer& getFrameBuffer(void) = 0;
    virtual const IFrameBuffer& getFrameBuffer(void) const = 0;
    virtual void setFrameBuffer(IFrameBuffer &frameBuffer) = 0;

    virtual Position getPosition(Position::Tag positionTag) const = 0;
  };
}

#endif // #ifndef I_GUI_OBJECT_BASE_H