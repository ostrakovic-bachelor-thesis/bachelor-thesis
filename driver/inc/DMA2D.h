#ifndef DMA2D_H
#define DMA2D_H

#include "stm32l4r9xx.h"
#include "Peripheral.h"
#include <cstdint>


class DMA2D
{
public:

  DMA2D(DMA2D_TypeDef *DMA2DPtr);

  //! This enum class represents errors which can happen during method calls
  enum class ErrorCode : uint8_t
  {
    OK   = 0u,
    BUSY = 1u
  };

  enum class OutputColorFormat : uint8_t
  {
    ARGB8888 = 0b0000,
    RGB888   = 0b0001,
    RGB565   = 0b0010,
    ARGB1555 = 0b0011,
    ARGB4444 = 0b0100,
    ABGR8888 = 0b1000,
    BGR888   = 0b1001,
    BGR565   = 0b1010,
    ABGR1555 = 0b1011,
    ABGR4444 = 0b1100,
  };

  struct Color
  {
    uint8_t alpha;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
  };

  struct Position
  {
    uint16_t x;
    uint16_t y;
  };

  struct Dimension
  {
    uint16_t width;
    uint16_t height;
  };

  struct FillRectangleConfig
  {
    OutputColorFormat outputColorFormat;
    Color color;
    Position position;
    Dimension rectangleDimension;
    Dimension frameBufferDimension;
    void *frameBufferPtr;
  };

  ErrorCode init(void);

  ErrorCode fillRectangle(const FillRectangleConfig &fillRectangleConfig);

  bool isTransactionOngoing(void) const;

  void IRQHandler(void);

private:

  enum class Mode : uint8_t
  {
    //! Memory to memory (foreground fetch only)
    MEMORY_TO_MEMORY = 0b000,
    //! Memory to memory with pixel format conversion (FG/foreground fetch only with PFC active)
    MEMORY_TO_MEMORY_PFC = 0b001,
    //! Memory to memory with blending (FG/foreground and BG/background fetch with PFC and blending active)
    MEMORY_TO_MEMORY_WITH_BLENDING = 0b010,
    //! Register to memory (no FG/foreground nor BG/background fetch, only output stage active)
    REGISTER_TO_MEMORY = 0b011,
    //! Memory to memory with blending and fixed color foreground (BG/background fetch only with FG and BG PFC active)
    MEMORY_TO_MEMORY_WITH_BLENDING_AND_FIX_COLOR_FG = 0b100,
    //! Memory to memory with blending and fixed color background (FG/foreground fetch only with FG and BG PFC active)
    MEMORY_TO_MEMORY_WITH_BLENDING_AND_FIX_COLOR_BG = 0b101
  };

  enum class Interrupt : uint8_t
  {
    TRANSFER_COMPLETE = 0u,

    COUNT
  };

  //! Interrupt and status flags
  enum class Flag : uint8_t
  {
    IS_TRANSFER_COMPLETED = 0,

    COUNT
  };

  //! Constrol/status operation to register mapping
  struct CSRegisterMapping
  {
    uint32_t registerOffset;
    uint8_t  bitPosition;
  };

  void setMode(Mode mode);

  static void setLineOffsetModeToBytes(uint32_t &registerValueCR);

  static void setOutputColorFormat(uint32_t &registerValueOPFCCR, OutputColorFormat outputColorFormat);

  static void setRedBlueSwap(uint32_t &registerValueOPFCCR, OutputColorFormat outputColorFormat);

  static uint8_t getPixelSize(OutputColorFormat outputColorFormat);

  template<uint32_t t_alphaSize, uint32_t t_redSize, uint32_t t_greenSize, uint32_t t_blueSize>
  void setOutputColor(Color color);

  void setOutputColor(OutputColorFormat outputColorFormat, Color color);

  void setOutputMemoryAddress(
    void *frameBufferPtr,
    Dimension frameBufferDimension,
    Position position,
    OutputColorFormat outputColorFormat);

  void setOutputLineOffset(
    Dimension frameBufferDimension,
    Dimension rectangleDimension,
    OutputColorFormat outputColorFormat);

  void setOutputDimension(Dimension rectangleDimension);

  void startDMA2D(void);

  bool startTransaction(void);
  void endTransaction(void);

  void enableInterrupt(Interrupt interrupt);
  void disableInterrupt(Interrupt interrupt);
  bool isInterruptEnabled(Interrupt interrupt) const;

  bool isFlagSet(Flag flag) const;
  void clearFlag(Flag flag);

  //! TODO
  static const CSRegisterMapping s_interruptCSRegisterMapping[static_cast<uint8_t>(Interrupt::COUNT)];

  //! TODO
  static const CSRegisterMapping s_interruptStatusFlagsRegisterMapping[static_cast<uint8_t>(Flag::COUNT)];

  //! TODO
  DMA2D_TypeDef *m_DMA2DPtr;

  //! Is DMA2D transaction completed
  bool m_isTransactionCompleted;

};

#endif // #ifndef DMA2D_H