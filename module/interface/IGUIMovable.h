#ifndef I_GUI_MOVABLE_H
#define I_GUI_MOVABLE_H

#include "GUICommon.h"


namespace GUI
{
  class IMovable
  {
  public:
    virtual ~IMovable() = default;

    virtual void moveToPosition(const Position &position) = 0;
  };
}

#endif // #ifndef I_GUI_MOVABLE_H