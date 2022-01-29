#ifndef GUI_RECTANGLE_BASE_H
#define GUI_RECTANGLE_BASE_H

#include "IGUIObject.h"
#include "FrameBuffer.h"
#include "SysTick.h"


namespace GUI
{
  class RectangleBase : public IObject
  {
  public:

    RectangleBase(SysTick &sysTick, IFrameBuffer &frameBuffer);

    virtual ~RectangleBase() = default;

    struct RectangleBaseDescription
    {
      Dimension dimension;
      Position position;
    };

    void init(const RectangleBaseDescription &rectangleDescription);

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
    ErrorCode getDrawingTime(DrawHardware drawHardware, uint64_t &drawingTimeInUs) const override;

    void registerDrawCompletedCallback(const CallbackDescription &callbackDescription) override;
    void unregisterDrawCompletedCallback(void) override;

    uint16_t getVisiblePartWidth(void) const;
    uint16_t getVisiblePartHeight(void) const;
    Dimension getVisiblePartDimension(void) const;
    uint64_t getVisiblePartArea(void) const;

    Position getVisiblePartPosition(Position::Tag positionTag) const;

    static void callbackDMA2DDrawCompleted(void *guiRectangleBasePtr);

  protected:

    static constexpr uint8_t DISTINCT_DRAW_HARDWARE_COUNT = 2u;

    struct DrawingDurationInfo
    {
      bool isDrawnAtLeastOnce;
      uint64_t drawStartTimestamp;
      uint64_t lastDrawingDurationInUs;
      uint64_t lastDrawingVisiblePartArea;
    };

    virtual void drawCPU(void) = 0;
    virtual void drawDMA2D(void) = 0;

    Position getPositionTopLeftCorner(void) const;
    Position getPositionTopRightCorner(void) const;
    Position getPositionBottomLeftCorner(void) const;
    Position getPositionBottomRightCorner(void) const;
    Position getPositionCenter(void) const;

    bool isVisibleOnTheScreen(void) const;

    void startDrawingTransaction(DrawHardware drawHardware);
    void endDrawingTransaction(DrawHardware drawHardware);

    uint64_t calculateDrawingTime(const DrawingDurationInfo *drawingDurationInfoPtr) const;

    void callDrawCompletedCallbackIfRegistered(void);

    static void recalculatePositionToBeTopLeftCorner(RectangleBaseDescription &rectangleDescription);

    template <typename T>
    static T saturateValue(T value, T valueLowerLimit, T valueUpperLimit);

    DrawingDurationInfo m_drawingDurationInfo[DISTINCT_DRAW_HARDWARE_COUNT] =
    {
      {
        .isDrawnAtLeastOnce         = false,
        .drawStartTimestamp         = 0u,
        .lastDrawingDurationInUs    = 0u,
        .lastDrawingVisiblePartArea = 0u
      }
    };

    bool m_isDrawingCompleted = true;

    //! Reference to SysTick
    SysTick &m_sysTick;

    IFrameBuffer *m_frameBufferPtr;

    RectangleBaseDescription m_rectangleBaseDescription;

    CallbackDescription m_drawCompletedCallback;
  };
}

template <typename T>
T GUI::RectangleBase::saturateValue(T value, T valueLowerLimit, T valueUpperLimit)
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