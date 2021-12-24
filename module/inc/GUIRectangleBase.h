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
    uint16_t width;
    uint16_t height;
    Position position;
  };

  void init(const GUIRectangleBaseDescription &rectangleDescription);

  inline uint16_t getWidth(void) const override
  {
    return m_rectangleBaseDescription.width;
  }

  inline uint16_t getHeight(void) const override
  {
    return m_rectangleBaseDescription.height;
  }

  Position getPosition(Position::Tag positionTag) const override;

  void moveToPosition(const Position &position) override;

  void draw(DrawHardware drawHardware) override;

  inline IFrameBuffer& getFrameBuffer(void)
  {
    return m_frameBuffer;
  }

  inline const IFrameBuffer& getFrameBuffer(void) const
  {
    return m_frameBuffer;
  }

  uint16_t getVisiblePartWidth(void) const;

protected:

  Position getPositionTopLeftCorner(void) const;
  Position getPositionTopRightCorner(void) const;
  Position getPositionBottomLeftCorner(void) const;
  Position getPositionBottomRightCorner(void) const;
  Position getPositionCenter(void) const;

  static void recalculatePositionToBeTopLeftCorner(GUIRectangleBaseDescription &rectangleDescription);

  IFrameBuffer &m_frameBuffer;

  GUIRectangleBaseDescription m_rectangleBaseDescription;


};

#endif // #ifndef GUI_RECTANGLE_BASE_H