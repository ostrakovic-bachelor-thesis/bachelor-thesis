#ifndef DMA2D_H
#define DMA2D_H

#include "stm32l4r9xx.h"
#include "ResetControl.h"
#include "Peripheral.h"
#include <cstdint>


class DMA2D
{
public:

  DMA2D(DMA2D_TypeDef *DMA2DPeripheralPtr, ResetControl *resetControlPtr);

  //! This enum class represents errors which can happen during method calls
  enum class ErrorCode : uint8_t
  {
    OK                               = 0u,
    BUSY                             = 1u,
    COLOR_VALUE_OUT_OF_RANGE         = 2u,
    CAN_NOT_TURN_ON_PERIPHERAL_CLOCK = 3u
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

  enum class InputColorFormat : uint8_t
  {
    ARGB8888 = 0b00000,
    RGB888   = 0b00001,
    RGB565   = 0b00010,
    ARGB1555 = 0b00011,
    ARGB4444 = 0b00100,
    L8       = 0b00101,
    AL44     = 0b00110,
    AL88     = 0b00111,
    L4       = 0b01000,
    A8       = 0b01001,
    A4       = 0b01010,
    ABGR8888 = 0b10000,
    BGR888   = 0b10001,
    BGR565   = 0b10010,
    ABGR1555 = 0b10011,
    ABGR4444 = 0b10100
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

  struct InputBufferConfiguration
  {
    InputColorFormat colorFormat;
    Dimension bufferDimension;
    const void *bufferPtr;
  };

  struct OutputBufferConfiguration
  {
    OutputColorFormat colorFormat;
    Dimension bufferDimension;
    void *bufferPtr;
  };

  struct FillRectangleConfig
  {
    Color color;
    Dimension dimension;
    Position position;
    OutputBufferConfiguration destinationBufferConfig;
  };

  struct CopyBitmapConfig
  {
    Dimension dimension;
    Position sourceRectanglePosition;
    InputBufferConfiguration sourceBufferConfig;
    Position destinationRectanglePosition;
    OutputBufferConfiguration destinationBufferConfig;
  };

  struct BlendBitmapConfig
  {
    Dimension dimension;
    Position foregroundRectanglePosition;
    InputBufferConfiguration foregroundBufferConfig;
    Position backgroundRectanglePosition;
    InputBufferConfiguration backgroundBufferConfig;
    Position destinationRectanglePosition;
    OutputBufferConfiguration destinationBufferConfig;
  };

  ErrorCode init(void);

  ErrorCode fillRectangle(const FillRectangleConfig &fillRectangleConfig);

  ErrorCode copyBitmap(const CopyBitmapConfig &copyBitmapConfig);

  ErrorCode blendBitmap(const BlendBitmapConfig &blendBitmapConfig);

  bool isTransferOngoing(void) const;

  void IRQHandler(void);

  inline Peripheral getPeripheralTag(void) const
  {
    return static_cast<Peripheral>(reinterpret_cast<uintptr_t>(const_cast<DMA2D_TypeDef*>(m_DMA2DPeripheralPtr)));
  }

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

  void configureOutputStage(
    OutputColorFormat colorFormat,
    Dimension transactionRectangleDimension,
    Position position,
    Dimension bufferDimension,
    void *bufferPtr);

  void configureForegroundInputStage(
    InputColorFormat colorFormat,
    Dimension transactionRectangleDimension,
    Position position,
    Dimension bufferDimension,
    const void *bufferPtr);

  void configureBackgroundInputStage(
    InputColorFormat colorFormat,
    Dimension transactionRectangleDimension,
    Position position,
    Dimension bufferDimension,
    const void *bufferPtr);

  void setMode(Mode mode);

  static void setLineOffsetModeToBytes(uint32_t &registerValueCR);

  template<typename ColorFormat, uint32_t t_colorFormatSize>
  static void setColorFormat(uint32_t &registerValuePFCCR, ColorFormat colorFormat);

  template<typename ColorFormat, uint32_t t_colorFormatSize>
  static void setColorRedBlueSwap(uint32_t &registerValuePFCCR, ColorFormat colorFormat);

  template<typename ColorFormat, typename MemberReference>
  void setMemoryAddress(
    MemberReference memoryAddressRegister,
    void *bufferPtr,
    Dimension bufferDimension,
    Position position,
    ColorFormat colorFormat);

  template<typename ColorFormat, typename MemberReference>
  void setLineOffset(
    MemberReference lineOffsetRegister,
    Dimension bufferDimension,
    Dimension rectangleDimension,
    ColorFormat colorFormat);

  void setTransactionRectangleDimension(Dimension transactionRectangleDimension);

  static uint8_t getPixelSize(OutputColorFormat outputColorFormat);
  static uint8_t getPixelSize(InputColorFormat inputColorFormat);

  static ErrorCode checkFillRectangleConfig(const FillRectangleConfig &fillRectangleConfig);

  static Color getMaximumColorValue(OutputColorFormat outputColorFormat);

  template<uint32_t t_alphaSize, uint32_t t_redSize, uint32_t t_greenSize, uint32_t t_blueSize>
  void setOutputColor(Color color);

  void setOutputColor(OutputColorFormat outputColorFormat, Color color);

  void startDMA2D(void);

  bool startTransfer(void);
  void endTransfer(void);

  void enableInterrupt(Interrupt interrupt);
  void disableInterrupt(Interrupt interrupt);
  bool isInterruptEnabled(Interrupt interrupt) const;

  bool isFlagSet(Flag flag) const;
  void clearFlag(Flag flag);

  ErrorCode enablePeripheralClock(void);

  //! TODO
  static const CSRegisterMapping s_interruptCSRegisterMapping[static_cast<uint8_t>(Interrupt::COUNT)];

  //! TODO
  static const CSRegisterMapping s_interruptStatusFlagsRegisterMapping[static_cast<uint8_t>(Flag::COUNT)];

  //! TODO
  DMA2D_TypeDef *m_DMA2DPeripheralPtr;

  //! Pointer to Reset Control module
  ResetControl *m_resetControlPtr;

  //! Is DMA2D transfer completed
  bool m_isTransferCompleted;

};

#endif // #ifndef DMA2D_H