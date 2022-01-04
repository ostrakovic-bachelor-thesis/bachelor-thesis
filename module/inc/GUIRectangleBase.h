#ifndef GUI_RECTANGLE_BASE_H
#define GUI_RECTANGLE_BASE_H

#include "IGUIObject.h"
#include "FrameBuffer.h"


class GUIRectangleBase : public IGUIObject
{
public:

  GUIRectangleBase(IFrameBuffer &frameBuffer);

  struct GUIRectangleBaseDescription
  {
    Dimension dimension;
    Position position;
  };

  void init(const GUIRectangleBaseDescription &rectangleDescription);

  IFrameBuffer& getFrameBuffer(void) override;
  const IFrameBuffer& getFrameBuffer(void) const;
  void setFrameBuffer(IFrameBuffer &frameBuffer) override;

  inline uint16_t getWidth(void) const override
  {
    return m_rectangleBaseDescription.dimension.width;
  }

  inline uint16_t getHeight(void) const override
  {
    return m_rectangleBaseDescription.dimension.height;
  }

  inline Dimension getDimension(void) const override
  {
    return m_rectangleBaseDescription.dimension;
  }

  Position getPosition(Position::Tag positionTag) const override;
  void moveToPosition(const Position &position) override;

  void draw(DrawHardware drawHardware) override;
  bool isDrawCompleted(void) const override;

  uint16_t getVisiblePartWidth(void) const;
  uint16_t getVisiblePartHeight(void) const;
  Dimension getVisiblePartDimension(void) const;

  Position getVisiblePartPosition(Position::Tag positionTag) const;

protected:

  Position getPositionTopLeftCorner(void) const;
  Position getPositionTopRightCorner(void) const;
  Position getPositionBottomLeftCorner(void) const;
  Position getPositionBottomRightCorner(void) const;
  Position getPositionCenter(void) const;

  static void recalculatePositionToBeTopLeftCorner(GUIRectangleBaseDescription &rectangleDescription);

  template <typename T>
  static T saturateValue(T value, T valueLowerLimit, T valueUpperLimit);

  IFrameBuffer *m_frameBufferPtr;

  GUIRectangleBaseDescription m_rectangleBaseDescription;
};

template <typename T>
T GUIRectangleBase::saturateValue(T value, T valueLowerLimit, T valueUpperLimit)
{
  if (value < valueLowerLimit)
  {
    return valueLowerLimit;
  }
  else if (value > valueUpperLimit)
  {
    return valueUpperLimit;
  }
  else
  {
    return value;
  }
}

#endif // #ifndef GUI_RECTANGLE_BASE_H