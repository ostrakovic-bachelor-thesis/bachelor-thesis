#ifndef GUI_RECTANGLE_H
#define GUI_RECTANGLE_H

#include "GUIRectangleBase.h"
#include "IFrameBuffer.h"
#include "DMA2D.h"
#include "SysTick.h"
#include <cstdint>


namespace GUI
{
  class Rectangle : public RectangleBase
  {
  public:

    Rectangle(DMA2D &dma2d, SysTick &sysTick, IFrameBuffer &frameBuffer);

    struct RectangleDescription
    {
      RectangleBaseDescription baseDescription;
      Color                    color;
    };

    void init(const RectangleDescription &rectangleDescription);

    void setFrameBuffer(IFrameBuffer &frameBuffer) override;

    void moveToPosition(const Position &position) override;

    Color getColor(void) const;

  private:

    void drawCPU(void) override;
    void drawDMA2D(void) override;

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
}

#endif // #ifndef GUI_RECTANGLE_H
