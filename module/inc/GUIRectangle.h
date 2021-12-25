#ifndef GUI_RECTANGLE_H
#define GUI_RECTANGLE_H

#include "GUIRectangleBase.h"
#include "IFrameBuffer.h"
#include "DMA2D.h"
#include <cstdint>


class GUIRectangle : public GUIRectangleBase
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
    GUIRectangleBaseDescription baseDescription;
    Color                       color;
  };

  void init(const RectangleDescription &rectangleDescription);

  void moveToPosition(const Position &position) override;

  void draw(DrawHardware drawHardware) override;

  Color getColor(void) const;

private:

  void drawCPU(void);
  void drawDMA2D(void);

  static DMA2D::OutputColorFormat mapToDMA2DOutputColorFormat(IFrameBuffer::ColorFormat colorFormat);

  static DMA2D::Position mapToDMA2DPosition(Position position);

  DMA2D::FillRectangleConfig buildFillRectangleConfig(
    const GUIRectangleBaseDescription &rectangleBaseDescription,
    Color rectangleColor,
    IFrameBuffer &frameBuffer);

  Color m_color;

  DMA2D::FillRectangleConfig m_fillRectangleConfig;

  //! Reference to DMA2D
  DMA2D &m_dma2d;
};

#endif // #ifndef GUI_RECTANGLE_H
