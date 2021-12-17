#include "IGUIObject.h"
#include "IFrameBuffer.h"
#include "DMA2D.h"
#include <cstdint>


class GUIRectangle : public IGUIObject
{
public:

  GUIRectangle(DMA2D &dma2d, IFrameBuffer &frameBuffer);

  struct Color
  {
    bool operator==(const Color &color) const;

    uint8_t red;
    uint8_t green;
    uint8_t blue;
  };

  struct RectangleDescription
  {
    uint16_t width;
    uint16_t height;
    Color    color;
    Position position;
  };

  void init(const RectangleDescription &rectangleDescription);

  uint16_t getWidth(void) const override;

  uint16_t getHeight(void) const override;

  Position getPosition(Position::Tag positionTag = Position::Tag::TOP_LEFT_CORNER) const override;

  void draw(DrawHardware drawHardware) override;

  IFrameBuffer& getFrameBuffer(void)
  {
    return m_frameBuffer;
  }

  const IFrameBuffer& getFrameBuffer(void) const
  {
    return m_frameBuffer;
  }

  Color getColor(void) const;

  void moveToPosition(const Position &position);

private:

  Position getDrawingStartPosition(void) const;
  Position getDrawingEndPosition(void) const;

  void drawCPU(void);
  void drawDMA2D(void);

  Position getPositionTopLeftCorner(void) const;
  Position getPositionTopRightCorner(void) const;
  Position getPositionBottomLeftCorner(void) const;
  Position getPositionBottomRightCorner(void) const;
  Position getPositionCenter(void) const;

  static DMA2D::OutputColorFormat mapToDMA2DOutputColorFormat(IFrameBuffer::ColorFormat colorFormat);

  static DMA2D::FillRectangleConfig
  buildFillRectangleConfig(const RectangleDescription &rectangleDescription, IFrameBuffer &frameBuffer);

  static void recalculatePositionToBeTopLeftCorner(GUIRectangle::RectangleDescription &rectangleDescription);

  RectangleDescription m_rectangleDescription;

  DMA2D::FillRectangleConfig m_fillRectangleConfig;

  IFrameBuffer &m_frameBuffer;

  //! Reference to DMA2D
  DMA2D &m_dma2d;
};
