#include "USART.h"
#include "MemoryAccess.h"
#include "MemoryUtility.h"
#include "RegisterUtility.h"
#include <cstddef>


const uint32_t USART::s_prescaler[] =
{
  [0b0000] = 1u,
  [0b0001] = 2u,
  [0b0010] = 4u,
  [0b0011] = 6u,
  [0b0100] = 8u,
  [0b0101] = 10u,
  [0b0110] = 12u,
  [0b0111] = 16u,
  [0b1000] = 32u,
  [0b1001] = 64u,
  [0b1010] = 128u,
  [0b1011] = 256u,
  [0b1100] = 256u,
  [0b1101] = 256u,
  [0b1110] = 256u,
  [0b1111] = 256u
};

const USART::CSRegisterMapping USART::s_interruptCSRegisterMapping[static_cast<uint8_t>(Interrupt::COUNT)] =
{
  [static_cast<uint8_t>(USART::Interrupt::FIFO_THRESHOLD)] =
  {
    .registerOffset = offsetof(USART_TypeDef, CR3),
    .bitPosition = 23u,
  },

  [static_cast<uint8_t>(USART::Interrupt::TRANSMISSION_COMPLETE)] =
  {
    .registerOffset = offsetof(USART_TypeDef, CR1),
    .bitPosition = 6u,
  },
};

const USART::CSRegisterMapping USART::s_interruptStatusFlagsRegisterMapping[static_cast<uint8_t>(Flag::COUNT)] =
{
  [static_cast<uint8_t>(USART::Flag::TX_FIFO_NOT_FULL)] =
  {
    .registerOffset = offsetof(USART_TypeDef, ISR),
    .bitPosition = 7u,
  },

  [static_cast<uint8_t>(USART::Flag::IS_TRANMISSION_COMPLETED)] =
  {
    .registerOffset = offsetof(USART_TypeDef, ISR),
    .bitPosition = 6u,
  },
};


USART::USART(USART_TypeDef *USARTPeripheralPtr, ClockControl *clockControlPtr, ResetControl *resetControlPtr):
  m_USARTPeripheralPtr(USARTPeripheralPtr),
  m_clockControlPtr(clockControlPtr),
  m_resetControlPtr(resetControlPtr),
  m_isTxTransactionCompleted(true)
{}

USART::ErrorCode USART::init(const USARTConfig &usartConfig)
{
  if (not isUSARTConfigurationValid(usartConfig))
  {
    return ErrorCode::USART_CONFIG_PARAM_INVALID_VALUE;
  }

  ErrorCode errorCode = enablePeripheralClock();
  if (ErrorCode::OK != errorCode)
  {
    return errorCode;
  }

  disableUSART();

  uint32_t registerValueCR1 = MemoryAccess::getRegisterValue(&(m_USARTPeripheralPtr->CR1));
  uint32_t registerValueCR2 = MemoryAccess::getRegisterValue(&(m_USARTPeripheralPtr->CR2));

  enableFIFOMode(registerValueCR1);
  setFrameFormat(registerValueCR1, usartConfig.frameFormat);
  setOversampling(registerValueCR1, usartConfig.oversampling);
  setParity(registerValueCR1, usartConfig.parity);

  setStopBits(registerValueCR2, usartConfig.stopBits);

  MemoryAccess::setRegisterValue(&(m_USARTPeripheralPtr->CR1), registerValueCR1);
  MemoryAccess::setRegisterValue(&(m_USARTPeripheralPtr->CR2), registerValueCR2);

  errorCode = setBaudrate(usartConfig.baudrate, usartConfig.oversampling);

  enableUSART();

  return errorCode;
}

USART::ErrorCode USART::write(const void *messagePtr, uint32_t messageLen)
{
  ErrorCode errorCode = ErrorCode::OK;

  if (startTxTransaction())
  {
    m_txMessagePtr = messagePtr;
    m_txMessageLen = messageLen;
    m_txMessagePos = 0u;

    disableInterrupt(Interrupt::TRANSMISSION_COMPLETE);
    enableInterrupt(Interrupt::FIFO_THRESHOLD);
    enableTransmitter();
  }
  else
  {
    errorCode = ErrorCode::BUSY;
  }

  return errorCode;
}

bool USART::startTxTransaction(void)
{
  bool isTxTransacationStarted = false;

  if (true == m_isTxTransactionCompleted)
  {
    m_isTxTransactionCompleted = false;
    isTxTransacationStarted    = true;
  }

  return isTxTransacationStarted;
}

inline void USART::endTxTransaction(void)
{
  m_isTxTransactionCompleted = true;
}

void USART::IRQHandler(void)
{
  if (isInterruptEnabled(Interrupt::TRANSMISSION_COMPLETE) && isFlagSet(Flag::IS_TRANMISSION_COMPLETED))
  {
    disableInterrupt(Interrupt::TRANSMISSION_COMPLETE);
    disableTransmitter();
    endTxTransaction();
  }

  if (isWriteTransactionOngoing())
  {
    while (isInterruptEnabled(Interrupt::FIFO_THRESHOLD) && isFlagSet(Flag::TX_FIFO_NOT_FULL))
    {
      if (m_txMessagePos != m_txMessageLen)
      {
        writeData(reinterpret_cast<const uint8_t*>(m_txMessagePtr)[m_txMessagePos++]);
      }
      else
      {
        disableInterrupt(Interrupt::FIFO_THRESHOLD);
        enableInterrupt(Interrupt::TRANSMISSION_COMPLETE);
      }
    }
  }
}

void USART::enableFIFOMode(uint32_t &registerValueCR1)
{
  constexpr uint32_t USART_CR1_FIFOEN_POSITION = 29u;
  registerValueCR1 = MemoryUtility<uint32_t>::setBit(registerValueCR1, USART_CR1_FIFOEN_POSITION);
}

void USART::setFrameFormat(uint32_t &registerValueCR1, FrameFormat frameFormat)
{
  constexpr uint32_t USART_CR1_PCE_POSITION = 10u;
  constexpr uint32_t USART_CR1_M0_POSITION  = 12u;
  constexpr uint32_t USART_CR1_M1_POSITION  = 28u;

  if (MemoryUtility<uint8_t>::isBitSet(static_cast<uint8_t>(frameFormat), 2u))
  {
    registerValueCR1 = MemoryUtility<uint32_t>::setBit(registerValueCR1, USART_CR1_PCE_POSITION);
  }
  else
  {
    registerValueCR1 = MemoryUtility<uint32_t>::resetBit(registerValueCR1, USART_CR1_PCE_POSITION);
  }

  if (MemoryUtility<uint8_t>::isBitSet(static_cast<uint8_t>(frameFormat), 0u))
  {
    registerValueCR1 = MemoryUtility<uint32_t>::setBit(registerValueCR1, USART_CR1_M0_POSITION);
  }
  else
  {
    registerValueCR1 = MemoryUtility<uint32_t>::resetBit(registerValueCR1, USART_CR1_M0_POSITION);
  }

  if (MemoryUtility<uint8_t>::isBitSet(static_cast<uint8_t>(frameFormat), 1u))
  {
    registerValueCR1 = MemoryUtility<uint32_t>::setBit(registerValueCR1, USART_CR1_M1_POSITION);
  }
  else
  {
    registerValueCR1 = MemoryUtility<uint32_t>::resetBit(registerValueCR1, USART_CR1_M1_POSITION);
  }
}

void USART::setOversampling(uint32_t &registerValueCR1, Oversampling oversampling)
{
  constexpr uint8_t OVERSAMPLING_NUM_OF_BITS = 1u;
  constexpr uint8_t USART_CR1_OVER8_POSITION = 15u;

  registerValueCR1 = MemoryUtility<uint32_t>::setBits(
    registerValueCR1,
    USART_CR1_OVER8_POSITION,
    OVERSAMPLING_NUM_OF_BITS,
    static_cast<uint32_t>(oversampling));
}

void USART::setParity(uint32_t &registerValueCR1, Parity parity)
{
  constexpr uint8_t PARITY_NUM_OF_BITS = 1u;
  constexpr uint8_t USART_CR1_PS_POSITION = 9u;

  registerValueCR1 = MemoryUtility<uint32_t>::setBits(
    registerValueCR1,
    USART_CR1_PS_POSITION,
    PARITY_NUM_OF_BITS,
    static_cast<uint32_t>(parity));
}

void USART::setStopBits(uint32_t &registerValueCR2, StopBits stopBits)
{
  constexpr uint8_t STOP_BITS_NUM_OF_BITS = 2u;
  constexpr uint8_t USART_CR2_STOP_POSITION = 12u;

  registerValueCR2 = MemoryUtility<uint32_t>::setBits(
    registerValueCR2,
    USART_CR2_STOP_POSITION,
    STOP_BITS_NUM_OF_BITS,
    static_cast<uint32_t>(stopBits));
}

inline void USART::disableUSART(void)
{
  constexpr uint8_t USART_CR1_UE_POSITION = 0u;
  RegisterUtility<uint32_t>::resetBitInRegister(&(m_USARTPeripheralPtr->CR1), USART_CR1_UE_POSITION);
}

inline void USART::enableUSART(void)
{
  constexpr uint8_t USART_CR1_UE_POSITION = 0u;
  RegisterUtility<uint32_t>::setBitInRegister(&(m_USARTPeripheralPtr->CR1), USART_CR1_UE_POSITION);
}

inline void USART::enableTransmitter(void)
{
  constexpr uint8_t USART_CR1_TE_POSITION = 3u;
  RegisterUtility<uint32_t>::setBitInRegister(&(m_USARTPeripheralPtr->CR1), USART_CR1_TE_POSITION);
}

inline void USART::disableTransmitter(void)
{
  constexpr uint8_t USART_CR1_TE_POSITION = 3u;
  RegisterUtility<uint32_t>::resetBitInRegister(&(m_USARTPeripheralPtr->CR1), USART_CR1_TE_POSITION);
}

void USART::enableInterrupt(Interrupt interrupt)
{
  const auto registerOffset = s_interruptCSRegisterMapping[static_cast<uint8_t>(interrupt)].registerOffset;
  const auto bitPosition = s_interruptCSRegisterMapping[static_cast<uint8_t>(interrupt)].bitPosition;
  uint32_t *registerPtr =
    reinterpret_cast<uint32_t*>((reinterpret_cast<uintptr_t>(m_USARTPeripheralPtr) + registerOffset));

  RegisterUtility<uint32_t>::setBitInRegister(registerPtr, bitPosition);
}

void USART::disableInterrupt(Interrupt interrupt)
{
  const auto registerOffset = s_interruptCSRegisterMapping[static_cast<uint8_t>(interrupt)].registerOffset;
  const auto bitPosition = s_interruptCSRegisterMapping[static_cast<uint8_t>(interrupt)].bitPosition;
  uint32_t *registerPtr =
    reinterpret_cast<uint32_t*>((reinterpret_cast<uintptr_t>(m_USARTPeripheralPtr) + registerOffset));

  RegisterUtility<uint32_t>::resetBitInRegister(registerPtr, bitPosition);
}

bool USART::isInterruptEnabled(Interrupt interrupt) const
{
  const auto registerOffset = s_interruptCSRegisterMapping[static_cast<uint8_t>(interrupt)].registerOffset;
  const auto bitPosition = s_interruptCSRegisterMapping[static_cast<uint8_t>(interrupt)].bitPosition;
  const uint32_t *registerPtr =
    reinterpret_cast<uint32_t*>((reinterpret_cast<uintptr_t>(m_USARTPeripheralPtr) + registerOffset));

  const uint32_t registerValue = MemoryAccess::getRegisterValue(registerPtr);
  return MemoryUtility<uint32_t>::isBitSet(registerValue, bitPosition);
}

bool USART::isFlagSet(Flag flag) const
{
  const auto bitPosition = s_interruptStatusFlagsRegisterMapping[static_cast<uint8_t>(flag)].bitPosition;

  const uint32_t registerValue = MemoryAccess::getRegisterValue(&(m_USARTPeripheralPtr->ISR));
  return MemoryUtility<uint32_t>::isBitSet(registerValue, bitPosition);
}

USART::ErrorCode USART::setBaudrate(Baudrate baudrate, Oversampling oversampling)
{
  uint32_t inputClockFrequency = 0u;

  const auto clockControlErrorCode =  m_clockControlPtr->getClockFrequency(
    static_cast<Peripheral>(reinterpret_cast<uintptr_t>(m_USARTPeripheralPtr)),
    inputClockFrequency);
  if (ClockControl::ErrorCode::OK != clockControlErrorCode)
  {
    return ErrorCode::BAUDRATE_SETUP_PROBLEM;
  }

  uint32_t BRRValue;
  uint32_t PRESCValue;

  bool isSuccessful = findBRRAndPRESCValue(static_cast<uint32_t>(baudrate), inputClockFrequency, oversampling, BRRValue, PRESCValue);
  if (not isSuccessful)
  {
    return ErrorCode::BAUDRATE_SETUP_PROBLEM;
  }

  setPRESCRegister(PRESCValue);
  setBRRRegister(BRRValue);

  return ErrorCode::OK;
}

bool USART::findBRRAndPRESCValue(
  uint32_t baudrate,
  uint32_t inputClockFrequency,
  Oversampling oversampling,
  uint32_t &BRRValue,
  uint32_t &PRESCValue)
{
  bool isBaudrateFind = false;

  for (uint32_t prescalerIndex = 0u; prescalerIndex < sizeof(s_prescaler) / sizeof(s_prescaler[0]); ++prescalerIndex)
  {
    PRESCValue = prescalerIndex;
    BRRValue   = findBRR(baudrate, inputClockFrequency, s_prescaler[prescalerIndex], oversampling);
    if (BRR_INVALID_VALUE != BRRValue)
    {
      isBaudrateFind = true;
      break;
    }
  }

  return isBaudrateFind;
}

uint32_t USART::findBRR(uint32_t baudrate, uint32_t inputClockFrequency, uint32_t prescaler, Oversampling oversampling)
{
  const uint32_t clockMultiplier = (Oversampling::OVERSAMPLING_8 == oversampling) ? 2u : 1u;
  const uint32_t clockFrequency  = (clockMultiplier * inputClockFrequency) / prescaler;

  uint32_t usartDiv = (clockFrequency + (baudrate / 2u)) / baudrate ;
  if (Oversampling::OVERSAMPLING_8 == oversampling)
  {
    usartDiv -= (usartDiv % 2);
  }

  if (not isValidUsartDiv(usartDiv))
  {
    return BRR_INVALID_VALUE;
  }

  const uint32_t generatedBaudrate = clockFrequency / usartDiv;
  const float baudrateDeviation = 100.0f * (static_cast<float>(baudrate) - static_cast<float>(generatedBaudrate)) / baudrate;

  if (not isBaudrateDeviationWithinTolerance(baudrateDeviation))
  {
    return BRR_INVALID_VALUE;
  }

  return usartDivToBRR(usartDiv, oversampling);
}

inline bool USART::isValidUsartDiv(uint32_t value)
{
  return (0x10u <= value) && (0xFFFFu >= value);
}

inline bool USART::isBaudrateDeviationWithinTolerance(float baudrateDeviation)
{
  return (1.82f >= baudrateDeviation) && (-1.82f <= baudrateDeviation);
}

inline uint32_t USART::usartDivToBRR(uint32_t usartDiv, Oversampling oversampling)
{
  if (Oversampling::OVERSAMPLING_8 == oversampling)
  {
    usartDiv = ((usartDiv & 0x0000FFF0u) | ((usartDiv & 0x0000000Fu) >> 1));
  }

  return usartDiv;
}

inline void USART::setPRESCRegister(uint32_t PRESCValue)
{
  MemoryAccess::setRegisterValue(&(m_USARTPeripheralPtr->PRESC), PRESCValue);
}

inline void USART::setBRRRegister(uint32_t BRRValue)
{
  MemoryAccess::setRegisterValue(&(m_USARTPeripheralPtr->BRR), BRRValue);
}

inline void USART::writeData(uint8_t data)
{
  MemoryAccess::setRegisterValue(&(m_USARTPeripheralPtr->TDR), data);
}

bool USART::isFrameFormatInValidRangeOfValues(FrameFormat frameFormat)
{
  bool isFrameFormatValid = false;

  switch (frameFormat)
  {
    case FrameFormat::BITS_6_WITH_PARITY:
    case FrameFormat::BITS_7_WITH_PARITY:
    case FrameFormat::BITS_8_WITH_PARITY:
    case FrameFormat::BITS_7_WITHOUT_PARITY:
    case FrameFormat::BITS_8_WITHOUT_PARITY:
    case FrameFormat::BITS_9_WITHOUT_PARITY:
    {
      isFrameFormatValid = true;
    }
    break;

    default:
    {
      isFrameFormatValid = false;
    }
    break;
  }

  return isFrameFormatValid;
}

bool USART::isUSARTConfigurationValid(const USARTConfig &usartConfig)
{
  bool isUSARTConfigValid = true;

  isUSARTConfigValid &= isFrameFormatInValidRangeOfValues(usartConfig.frameFormat);
  isUSARTConfigValid &= isOversamplingInValidRangeOfValues(usartConfig.oversampling);
  isUSARTConfigValid &= isParityInValidRangeOfValues(usartConfig.parity);
  isUSARTConfigValid &= isStopBitsInValidRangeOfValues(usartConfig.stopBits);
  isUSARTConfigValid &= isBaudrateInValidRangeOfValues(usartConfig.baudrate);

  return isUSARTConfigValid;
}

inline bool USART::isOversamplingInValidRangeOfValues(Oversampling oversampling)
{
  return static_cast<uint32_t>(Oversampling::OVERSAMPLING_8) >= static_cast<uint32_t>(oversampling);
}

inline bool USART::isParityInValidRangeOfValues(Parity parity)
{
  return static_cast<uint32_t>(Parity::ODD) >= static_cast<uint32_t>(parity);
}

inline bool USART::isStopBitsInValidRangeOfValues(StopBits stopBits)
{
  return static_cast<uint32_t>(StopBits::BIT_1_5) >= static_cast<uint32_t>(stopBits);
}

bool USART::isBaudrateInValidRangeOfValues(Baudrate baudrate)
{
  bool isBaudrateValid = false;

  switch (baudrate)
  {
    case Baudrate::BAUDRATE_57600:
    case Baudrate::BAUDRATE_115200:
    case Baudrate::BAUDRATE_230400:
    case Baudrate::BAUDRATE_460800:
    case Baudrate::BAUDRATE_921600:
    {
      isBaudrateValid = true;
    }
    break;

    default:
    {
      isBaudrateValid = false;
    }
    break;
  }

  return isBaudrateValid;
}

inline USART::ErrorCode USART::enablePeripheralClock(void)
{
  ResetControl::ErrorCode errorCode = m_resetControlPtr->enablePeripheralClock(getPeripheralTag());

  return (ResetControl::ErrorCode::OK == errorCode) ? ErrorCode::OK : ErrorCode::CAN_NOT_TURN_ON_PERIPHERAL_CLOCK;
}