#include "DMA2D.h"
#include "MemoryAccess.h"
#include "RegisterUtility.h"
#include <cstddef>


const DMA2D::CSRegisterMapping DMA2D::s_interruptCSRegisterMapping[static_cast<uint8_t>(Interrupt::COUNT)] =
{
  [static_cast<uint8_t>(DMA2D::Interrupt::TRANSFER_COMPLETE)] =
  {
    .registerOffset = offsetof(DMA2D_TypeDef, CR),
    .bitPosition = 9u,
  }
};

const DMA2D::CSRegisterMapping DMA2D::s_interruptStatusFlagsRegisterMapping[static_cast<uint8_t>(Flag::COUNT)] =
{
  [static_cast<uint8_t>(DMA2D::Flag::IS_TRANSFER_COMPLETED)] =
  {
    .registerOffset = offsetof(DMA2D_TypeDef, ISR),
    .bitPosition = 1u,
  },
};


DMA2D::DMA2D(DMA2D_TypeDef *DMA2DPtr):
  m_DMA2DPtr(DMA2DPtr),
  m_isTransferCompleted(true)
{}

DMA2D::ErrorCode DMA2D::init(void)
{
  if (isTransferOngoing())
  {
    return ErrorCode::BUSY;

  }

  uint32_t registerValueCR = MemoryAccess::getRegisterValue(&(m_DMA2DPtr->CR));

  setLineOffsetModeToBytes(registerValueCR);

  MemoryAccess::setRegisterValue(&(m_DMA2DPtr->CR), registerValueCR);

  return ErrorCode::OK;
}

DMA2D::ErrorCode DMA2D::fillRectangle(const FillRectangleConfig &fillRectangleConfig)
{
  ErrorCode errorCode = checkFillRectangleConfig(fillRectangleConfig);

  if (ErrorCode::OK == errorCode)
  {
    bool isTransferStarted = startTransfer();
    if (not isTransferStarted)
    {
      errorCode = ErrorCode::BUSY;
    }
  }

  if (ErrorCode::OK == errorCode)
  {
    setMode(Mode::REGISTER_TO_MEMORY);

    uint32_t registerValueOPFCCR = MemoryAccess::getRegisterValue(&(m_DMA2DPtr->OPFCCR));
    setOutputColorFormat(registerValueOPFCCR, fillRectangleConfig.outputColorFormat);
    setRedBlueSwap(registerValueOPFCCR, fillRectangleConfig.outputColorFormat);
    MemoryAccess::setRegisterValue(&(m_DMA2DPtr->OPFCCR), registerValueOPFCCR);

    setOutputColor(fillRectangleConfig.outputColorFormat,
      fillRectangleConfig.color);
    setOutputMemoryAddress(fillRectangleConfig.frameBufferPtr,
      fillRectangleConfig.frameBufferDimension,
      fillRectangleConfig.position,
      fillRectangleConfig.outputColorFormat);
    setOutputLineOffset(fillRectangleConfig.frameBufferDimension,
      fillRectangleConfig.rectangleDimension,
      fillRectangleConfig.outputColorFormat);
    setOutputDimension(fillRectangleConfig.rectangleDimension);

    enableInterrupt(Interrupt::TRANSFER_COMPLETE);
    startDMA2D();
  }

  return errorCode;
}

void DMA2D::IRQHandler(void)
{
  if (isInterruptEnabled(Interrupt::TRANSFER_COMPLETE) && isFlagSet(Flag::IS_TRANSFER_COMPLETED))
  {
    disableInterrupt(Interrupt::TRANSFER_COMPLETE);
    clearFlag(Flag::IS_TRANSFER_COMPLETED);
    endTransfer();
  }
}

bool DMA2D::startTransfer(void)
{
  bool isTransacationStarted = false;

  if (not isTransferOngoing())
  {
    m_isTransferCompleted = false;
    isTransacationStarted = true;
  }

  return isTransacationStarted;
}

inline void DMA2D::endTransfer(void)
{
  m_isTransferCompleted = true;
}

inline void DMA2D::setLineOffsetModeToBytes(uint32_t &registerValueCR)
{
  constexpr uint8_t DMA2D_CR_LOM_POSITION = 6u;
  registerValueCR = MemoryUtility<uint32_t>::setBit(registerValueCR, DMA2D_CR_LOM_POSITION);
}

inline bool DMA2D::isTransferOngoing(void) const
{
  constexpr uint8_t DMA2D_CR_START_POSITION = 0u;
  const uint32_t registerValue = MemoryAccess::getRegisterValue(&(m_DMA2DPtr->CR));
  const bool isStartBitSet = MemoryUtility<uint32_t>::isBitSet(registerValue, DMA2D_CR_START_POSITION);

  return isStartBitSet || (not m_isTransferCompleted);
}

inline void DMA2D::setMode(Mode mode)
{
  constexpr uint8_t DMA2D_CR_MODE_POSITION    = 16u;
  constexpr uint8_t DMA2D_CR_MODE_NUM_OF_BITS = 3u;

  RegisterUtility<uint32_t>::setBitsInRegister(
    &(m_DMA2DPtr->CR),
    DMA2D_CR_MODE_POSITION,
    DMA2D_CR_MODE_NUM_OF_BITS,
    static_cast<uint32_t>(mode));
}

inline void DMA2D::setOutputColorFormat(uint32_t &registerValueOPFCCR, OutputColorFormat outputColorFormat)
{
  constexpr uint8_t DMA2D_OPFCCR_CM_POSITION    = 0u;
  constexpr uint8_t DMA2D_OPFCCR_CM_NUM_OF_BITS = 3u;

  registerValueOPFCCR = MemoryUtility<uint32_t>::setBits(
    registerValueOPFCCR,
    DMA2D_OPFCCR_CM_POSITION,
    DMA2D_OPFCCR_CM_NUM_OF_BITS,
    static_cast<uint32_t>(outputColorFormat));
}

inline void DMA2D::setRedBlueSwap(uint32_t &registerValueOPFCCR, OutputColorFormat outputColorFormat)
{
  constexpr uint8_t DMA2D_OPFCCR_RBS_POSITION    = 21u;
  constexpr uint8_t DMA2D_OPFCCR_RBS_NUM_OF_BITS = 1u;

  registerValueOPFCCR = MemoryUtility<uint32_t>::setBits(
    registerValueOPFCCR,
    DMA2D_OPFCCR_RBS_POSITION,
    DMA2D_OPFCCR_RBS_NUM_OF_BITS,
    (static_cast<uint32_t>(outputColorFormat) >> 3));
}

inline void DMA2D::setOutputMemoryAddress(
  void *frameBufferPtr,
  Dimension frameBufferDimension,
  Position position,
  OutputColorFormat outputColorFormat)
{
  uint32_t registerValueOMAR = reinterpret_cast<uintptr_t>(frameBufferPtr) +
    getPixelSize(outputColorFormat) * (position.x + position.y * frameBufferDimension.width);
  MemoryAccess::setRegisterValue(&(m_DMA2DPtr->OMAR), registerValueOMAR);
}

inline void DMA2D::setOutputLineOffset(
  Dimension frameBufferDimension,
  Dimension rectangleDimension,
  OutputColorFormat outputColorFormat)
{
  uint32_t registerValueOOR =
    getPixelSize(outputColorFormat) * (frameBufferDimension.width - rectangleDimension.width);
  MemoryAccess::setRegisterValue(&(m_DMA2DPtr->OOR), registerValueOOR);
}

void DMA2D::setOutputDimension(Dimension rectangleDimension)
{
  constexpr uint8_t DMA2D_NLR_NL_POSITION = 0u;
  constexpr uint8_t DMA2D_NLR_PL_POSITION = 16;
  constexpr uint8_t DMA2D_NLR_NL_NUM_OF_BITS = 16u;
  constexpr uint8_t DMA2D_NLR_PL_NUM_OF_BITS = 14u;

  uint32_t registerValueNLR = 0u;

  registerValueNLR = MemoryUtility<uint32_t>::setBits(
    registerValueNLR,
    DMA2D_NLR_NL_POSITION,
    DMA2D_NLR_NL_NUM_OF_BITS,
    static_cast<uint32_t>(rectangleDimension.height));

  registerValueNLR = MemoryUtility<uint32_t>::setBits(
    registerValueNLR,
    DMA2D_NLR_PL_POSITION,
    DMA2D_NLR_PL_NUM_OF_BITS,
    static_cast<uint32_t>(rectangleDimension.width));

  MemoryAccess::setRegisterValue(&(m_DMA2DPtr->NLR), registerValueNLR);
}

void DMA2D::startDMA2D(void)
{
  constexpr uint8_t DMA2D_CR_START_POSITION = 0u;
  RegisterUtility<uint32_t>::setBitInRegister(&(m_DMA2DPtr->CR), DMA2D_CR_START_POSITION);
}

bool DMA2D::isFlagSet(Flag flag) const
{
  const auto bitPosition = s_interruptStatusFlagsRegisterMapping[static_cast<uint8_t>(flag)].bitPosition;

  const uint32_t registerValue = MemoryAccess::getRegisterValue(&(m_DMA2DPtr->ISR));
  return MemoryUtility<uint32_t>::isBitSet(registerValue, bitPosition);
}

void DMA2D::clearFlag(Flag flag)
{
  const auto bitPosition = s_interruptStatusFlagsRegisterMapping[static_cast<uint8_t>(flag)].bitPosition;

  RegisterUtility<uint32_t>::resetBitInRegister(&(m_DMA2DPtr->ISR), bitPosition);
}

void DMA2D::enableInterrupt(Interrupt interrupt)
{
  const auto registerOffset = s_interruptCSRegisterMapping[static_cast<uint8_t>(interrupt)].registerOffset;
  const auto bitPosition = s_interruptCSRegisterMapping[static_cast<uint8_t>(interrupt)].bitPosition;
  uint32_t *registerPtr =
    reinterpret_cast<uint32_t*>((reinterpret_cast<uintptr_t>(m_DMA2DPtr) + registerOffset));

  RegisterUtility<uint32_t>::setBitInRegister(registerPtr, bitPosition);
}

void DMA2D::disableInterrupt(Interrupt interrupt)
{
  const auto registerOffset = s_interruptCSRegisterMapping[static_cast<uint8_t>(interrupt)].registerOffset;
  const auto bitPosition = s_interruptCSRegisterMapping[static_cast<uint8_t>(interrupt)].bitPosition;
  uint32_t *registerPtr =
    reinterpret_cast<uint32_t*>((reinterpret_cast<uintptr_t>(m_DMA2DPtr) + registerOffset));

  RegisterUtility<uint32_t>::resetBitInRegister(registerPtr, bitPosition);
}

bool DMA2D::isInterruptEnabled(Interrupt interrupt) const
{
  const auto registerOffset = s_interruptCSRegisterMapping[static_cast<uint8_t>(interrupt)].registerOffset;
  const auto bitPosition = s_interruptCSRegisterMapping[static_cast<uint8_t>(interrupt)].bitPosition;
  const uint32_t *registerPtr =
    reinterpret_cast<uint32_t*>((reinterpret_cast<uintptr_t>(m_DMA2DPtr) + registerOffset));

  const uint32_t registerValue = MemoryAccess::getRegisterValue(registerPtr);
  return MemoryUtility<uint32_t>::isBitSet(registerValue, bitPosition);
}

DMA2D::ErrorCode DMA2D::checkFillRectangleConfig(const FillRectangleConfig &fillRectangleConfig)
{
  const Color maximumColorValue = getMaximumColorValue(fillRectangleConfig.outputColorFormat);

  if ((maximumColorValue.alpha < fillRectangleConfig.color.alpha) ||
      (maximumColorValue.red   < fillRectangleConfig.color.red)   ||
      (maximumColorValue.green < fillRectangleConfig.color.green) ||
      (maximumColorValue.blue  < fillRectangleConfig.color.blue))
  {
    return ErrorCode::COLOR_VALUE_OUT_OF_RANGE;
  }

  return ErrorCode::OK;
}

DMA2D::Color DMA2D::getMaximumColorValue(OutputColorFormat outputColorFormat)
{
  DMA2D::Color maxColorValue;

  switch (outputColorFormat)
  {
    case DMA2D::OutputColorFormat::ARGB8888:
    case DMA2D::OutputColorFormat::ABGR8888:
    {
      maxColorValue =
      {
        .alpha = 255u,
        .red   = 255u,
        .green = 255u,
        .blue  = 255u
      };
    }
    break;

    case DMA2D::OutputColorFormat::RGB888:
    case DMA2D::OutputColorFormat::BGR888:
    {
      maxColorValue =
      {
        .alpha = 0u,
        .red   = 255u,
        .green = 255u,
        .blue  = 255u
      };
    }
    break;

    case DMA2D::OutputColorFormat::RGB565:
    case DMA2D::OutputColorFormat::BGR565:
    {
      maxColorValue =
      {
        .alpha = 0u,
        .red   = 31u,
        .green = 63u,
        .blue  = 31u
      };
    }
    break;

    case DMA2D::OutputColorFormat::ARGB1555:
    case DMA2D::OutputColorFormat::ABGR1555:
    {
      maxColorValue =
      {
        .alpha = 1u,
        .red   = 31u,
        .green = 31u,
        .blue  = 31u
      };
    }
    break;

    case DMA2D::OutputColorFormat::ARGB4444:
    case DMA2D::OutputColorFormat::ABGR4444:
    {
      maxColorValue =
      {
        .alpha = 15u,
        .red   = 15u,
        .green = 15u,
        .blue  = 15u
      };
    }
    break;

    default:
    {
      maxColorValue =
      {
        .alpha = 0u,
        .red   = 0u,
        .green = 0u,
        .blue  = 0u
      };
    }
    break;
  }

  return maxColorValue;
}

uint8_t DMA2D::getPixelSize(OutputColorFormat outputColorFormat)
{
  uint8_t pixelSize;

  switch (outputColorFormat)
  {
    case OutputColorFormat::ARGB8888:
    case OutputColorFormat::ABGR8888:
    {
      pixelSize = 4u;
    }
    break;

    case OutputColorFormat::RGB888:
    case OutputColorFormat::BGR888:
    {
      pixelSize = 3u;
    }
    break;

    case OutputColorFormat::RGB565:
    case OutputColorFormat::BGR565:
    case OutputColorFormat::ARGB1555:
    case OutputColorFormat::ABGR1555:
    case OutputColorFormat::ARGB4444:
    case OutputColorFormat::ABGR4444:
    {
      pixelSize = 2u;
    }
    break;

    default:
    {
      pixelSize = 0u;
    }
    break;
  }

  return pixelSize;
}

void DMA2D::setOutputColor(OutputColorFormat outputColorFormat, Color color)
{
  switch (outputColorFormat)
  {
    case OutputColorFormat::ARGB8888:
    case OutputColorFormat::ABGR8888:
    {
      setOutputColor<8,8,8,8>(color);
    }
    break;

    case OutputColorFormat::RGB888:
    case OutputColorFormat::BGR888:
    {
      setOutputColor<0,8,8,8>(color);
    }
    break;

    case OutputColorFormat::RGB565:
    case OutputColorFormat::BGR565:
    {
      setOutputColor<0,5,6,5>(color);
    }
    break;

    case OutputColorFormat::ARGB1555:
    case OutputColorFormat::ABGR1555:
    {
      setOutputColor<1,5,5,5>(color);
    }
    break;

    case OutputColorFormat::ARGB4444:
    case OutputColorFormat::ABGR4444:
    {
      setOutputColor<4,4,4,4>(color);
    }
    break;

    default:
    {
      // do nothing
    }
    break;
  }
}

template<uint32_t t_alphaSize, uint32_t t_redSize, uint32_t t_greenSize, uint32_t t_blueSize>
void DMA2D::setOutputColor(Color color)
{
  constexpr uint8_t DMA2D_OCOLR_BLUE_POSITION  = 0u;
  constexpr uint8_t DMA2D_OCOLR_GREEN_POSITION = DMA2D_OCOLR_BLUE_POSITION + t_blueSize;
  constexpr uint8_t DMA2D_OCOLR_RED_POSITION   = DMA2D_OCOLR_GREEN_POSITION + t_greenSize;
  constexpr uint8_t DMA2D_OCOLR_ALPHA_POSITION = DMA2D_OCOLR_RED_POSITION + t_redSize;

  uint32_t registerValueOCOLR = 0u;

  registerValueOCOLR = MemoryUtility<uint32_t>::setBits(
    registerValueOCOLR,
    DMA2D_OCOLR_BLUE_POSITION,
    t_blueSize,
    static_cast<uint32_t>(color.blue));

  registerValueOCOLR = MemoryUtility<uint32_t>::setBits(
    registerValueOCOLR,
    DMA2D_OCOLR_GREEN_POSITION,
    t_greenSize,
    static_cast<uint32_t>(color.green));

  registerValueOCOLR = MemoryUtility<uint32_t>::setBits(
    registerValueOCOLR,
    DMA2D_OCOLR_RED_POSITION,
    t_redSize,
    static_cast<uint32_t>(color.red));

  registerValueOCOLR = MemoryUtility<uint32_t>::setBits(
    registerValueOCOLR,
    DMA2D_OCOLR_ALPHA_POSITION,
    t_alphaSize,
    static_cast<uint32_t>(color.alpha));

  MemoryAccess::setRegisterValue(&(m_DMA2DPtr->OCOLR), registerValueOCOLR);
}