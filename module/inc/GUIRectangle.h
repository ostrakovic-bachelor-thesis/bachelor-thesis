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

  void setFrameBuffer(IFrameBuffer &frameBuffer) override;

  void moveToPosition(const Position &position) override;

  void draw(DrawHardware drawHardware) override;

  bool isDrawCompleted(void) const override;

  Color getColor(void) const;

private:

  void drawCPU(void);
  void drawDMA2D(void);

  bool isRectangleVisibleOnTheScreen(void) const;

  void buildFillRectangleConfig(void);

  static DMA2D::OutputColorFormat mapToDMA2DOutputColorFormat(IFrameBuffer::ColorFormat colorFormat);
  static DMA2D::Position mapToDMA2DPosition(Position position);
  static DMA2D::Dimension mapToDMA2DDimension(Dimension dimension);
  static DMA2D::Dimension mapToDMA2DDimension(IFrameBuffer::Dimension dimension);
  static DMA2D::Color mapToDMA2DColor(Color color);

  Color m_color;

  DrawHardware m_lastTransactionDrawHardware;

  DMA2D::FillRectangleConfig m_fillRectangleConfig;

  //! Reference to DMA2D
  DMA2D &m_dma2d;
};

#endif // #ifndef GUI_RECTANGLE_H
