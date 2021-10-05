#include "I2C.h"
#include "MemoryAccess.h"
#include "MemoryUtility.h"
#include "RegisterUtility.h"
#include <cstddef>


const I2C::CSRegisterMapping I2C::s_interruptCSRegisterMapping[static_cast<uint8_t>(Interrupt::COUNT)] =
{
  [static_cast<uint8_t>(I2C::Interrupt::TRANSMIT)] =
  {
    .registerOffset = offsetof(I2C_TypeDef, CR1),
    .bitPosition = 1u,
  },

  [static_cast<uint8_t>(I2C::Interrupt::RECEIVE)] =
  {
    .registerOffset = offsetof(I2C_TypeDef, CR1),
    .bitPosition = 2u,
  },

  [static_cast<uint8_t>(I2C::Interrupt::STOP_DETECTION)] =
  {
    .registerOffset = offsetof(I2C_TypeDef, CR1),
    .bitPosition = 5u,
  },

  [static_cast<uint8_t>(I2C::Interrupt::TRANSFER_COMPLETE)] =
  {
    .registerOffset = offsetof(I2C_TypeDef, CR1),
    .bitPosition = 6u,
  },

};

const I2C::CSRegisterMapping I2C::s_interruptStatusFlagsRegisterMapping[static_cast<uint8_t>(Flag::COUNT)] =
{
  [static_cast<uint8_t>(I2C::Flag::IS_TXDR_REGISTER_EMPTY)] =
  {
    .registerOffset = offsetof(I2C_TypeDef, ISR),
    .bitPosition = 0u,
  },

  [static_cast<uint8_t>(I2C::Flag::DATA_TO_TXDR_MUST_BE_WRITTEN)] =
  {
    .registerOffset = offsetof(I2C_TypeDef, ISR),
    .bitPosition = 1u,
  },

  [static_cast<uint8_t>(I2C::Flag::RXDR_NOT_EMPTY)] =
  {
    .registerOffset = offsetof(I2C_TypeDef, ISR),
    .bitPosition = 2u,
  },

  [static_cast<uint8_t>(I2C::Flag::IS_STOP_DETECTED)] =
  {
    .registerOffset = offsetof(I2C_TypeDef, ISR),
    .bitPosition = 5u,
  },

  [static_cast<uint8_t>(I2C::Flag::IS_TRANSFER_COMPLETED)] =
  {
    .registerOffset = offsetof(I2C_TypeDef, ISR),
    .bitPosition = 6u,
  },

};


I2C::I2C(I2C_TypeDef *I2CPeripheralPtr, ClockControl *clockControlPtr, ResetControl *resetControlPtr):
  m_I2CPeripheralPtr(I2CPeripheralPtr),
  m_clockControlPtr(clockControlPtr),
  m_resetControlPtr(resetControlPtr),
  m_isTransactionCompleted(true)
{}

I2C::ErrorCode I2C::init(const I2CConfig &i2cConfig)
{
  ErrorCode errorCode = enablePeripheralClock();
  if (ErrorCode::OK != errorCode)
  {
    return errorCode;
  }

  disableI2C();

  uint32_t registerValueCR1 = MemoryAccess::getRegisterValue(&(m_I2CPeripheralPtr->CR1));
  uint32_t registerValueCR2 = MemoryAccess::getRegisterValue(&(m_I2CPeripheralPtr->CR2));

  disableAnalogNoiseFilter(registerValueCR1);
  disableDigitalNoiseFilter(registerValueCR1);
  disableClockStretching(registerValueCR1);

  setAddressingMode(registerValueCR2, i2cConfig.addressingMode);
  enableAutoEndMode(registerValueCR2);
  disableReloadMode(registerValueCR2);

  MemoryAccess::setRegisterValue(&(m_I2CPeripheralPtr->CR1), registerValueCR1);
  MemoryAccess::setRegisterValue(&(m_I2CPeripheralPtr->CR2), registerValueCR2);

  errorCode = setupSCLClockTiming(i2cConfig.clockFrequencySCL);

  enableI2C();

  return errorCode;
}

I2C::ErrorCode I2C::write(uint16_t slaveAddress, const void *messagePtr, uint32_t messageLen)
{
  ErrorCode errorCode = ErrorCode::OK;

  if (startTransaction())
  {
    m_transactionTag = TransactionTag::WRITE;

    m_messagePtr = const_cast<void*>(messagePtr);
    m_messageLen = messageLen;
    m_messagePos = 0u;

    uint32_t registerValueCR2 = MemoryAccess::getRegisterValue(&(m_I2CPeripheralPtr->CR2));

    enableAutoEndMode(registerValueCR2);
    setNumberOfBytesToTransfer(registerValueCR2, messageLen);
    setSlaveAddress(registerValueCR2, slaveAddress);
    setTransferDirectionToWrite(registerValueCR2);
    setStartTransactionFlag(registerValueCR2);
    clearStopFlag(registerValueCR2);

    MemoryAccess::setRegisterValue(&(m_I2CPeripheralPtr->CR2), registerValueCR2);

    enableInterrupts(Interrupt::TRANSMIT, Interrupt::STOP_DETECTION);
  }
  else
  {
    errorCode = ErrorCode::BUSY;
  }

  return errorCode;
}

I2C::ErrorCode I2C::read(uint16_t slaveAddress, void *messagePtr, uint32_t messageLen)
{
  ErrorCode errorCode = ErrorCode::OK;

  if (startTransaction())
  {
    m_transactionTag = TransactionTag::READ;

    m_messagePtr = messagePtr;
    m_messageLen = messageLen;
    m_messagePos = 0u;

    uint32_t registerValueCR2 = MemoryAccess::getRegisterValue(&(m_I2CPeripheralPtr->CR2));

    enableAutoEndMode(registerValueCR2);
    setNumberOfBytesToTransfer(registerValueCR2, messageLen);
    setSlaveAddress(registerValueCR2, slaveAddress);
    setTransferDirectionToRead(registerValueCR2);
    setStartTransactionFlag(registerValueCR2);
    clearStopFlag(registerValueCR2);

    MemoryAccess::setRegisterValue(&(m_I2CPeripheralPtr->CR2), registerValueCR2);

    enableInterrupts(Interrupt::RECEIVE, Interrupt::STOP_DETECTION);
  }
  else
  {
    errorCode = ErrorCode::BUSY;
  }

  return errorCode;
}

I2C::ErrorCode
I2C::readMemory(uint16_t slaveAddress, uint8_t memoryAddress, void *messagePtr, uint32_t messageLen)
{
  ErrorCode errorCode = ErrorCode::OK;

  if (startTransaction())
  {
    m_transactionTag = TransactionTag::READ_MEMORY;

    m_messagePtr = messagePtr;
    m_messageLen = messageLen;
    m_messagePos = 0u;

    uint32_t registerValueCR2 = MemoryAccess::getRegisterValue(&(m_I2CPeripheralPtr->CR2));

    disableAutoEndMode(registerValueCR2);
    disableReloadMode(registerValueCR2);
    setNumberOfBytesToTransfer(registerValueCR2, sizeof(uint8_t));
    setSlaveAddress(registerValueCR2, slaveAddress);
    setTransferDirectionToWrite(registerValueCR2);
    setStartTransactionFlag(registerValueCR2);
    clearStopFlag(registerValueCR2);

    MemoryAccess::setRegisterValue(&(m_I2CPeripheralPtr->CR2), registerValueCR2);

    enableInterrupts(Interrupt::STOP_DETECTION, Interrupt::TRANSFER_COMPLETE, Interrupt::TRANSMIT, Interrupt::RECEIVE);
  }
  else
  {
    errorCode = ErrorCode::BUSY;
  }

  return errorCode;
}

I2C::ErrorCode
I2C::writeMemory(uint16_t slaveAddress, uint8_t memoryAddress, const void *messagePtr, uint32_t messageLen)
{
  ErrorCode errorCode = ErrorCode::OK;

  if (startTransaction())
  {
    m_transactionTag = TransactionTag::WRITE_MEMORY;

    uint32_t registerValueCR2 = MemoryAccess::getRegisterValue(&(m_I2CPeripheralPtr->CR2));

    disableAutoEndMode(registerValueCR2);
    enableReloadMode(registerValueCR2);
    setNumberOfBytesToTransfer(registerValueCR2, sizeof(uint8_t));
    setSlaveAddress(registerValueCR2, slaveAddress);
    setTransferDirectionToWrite(registerValueCR2);
    setStartTransactionFlag(registerValueCR2);
    clearStopFlag(registerValueCR2);

    MemoryAccess::setRegisterValue(&(m_I2CPeripheralPtr->CR2), registerValueCR2);

    enableInterrupts(Interrupt::STOP_DETECTION, Interrupt::TRANSFER_COMPLETE, Interrupt::TRANSMIT);
  }
  else
  {
    errorCode = ErrorCode::BUSY;
  }

  return errorCode;
}

bool I2C::isTransactionOngoing(void) const
{
  constexpr uint32_t I2C_ISR_BUSY_POSITION = 15u;
  const bool isBusyFlagSet =
    RegisterUtility<uint32_t>::isBitSetInRegister(&(m_I2CPeripheralPtr->ISR), I2C_ISR_BUSY_POSITION);

  return isBusyFlagSet || (not m_isTransactionCompleted);
}

void I2C::IRQHandler(void)
{
  if (isInterruptEnabled(Interrupt::STOP_DETECTION) && isFlagSet(Flag::IS_STOP_DETECTED))
  {
    disableInterrupts(Interrupt::TRANSFER_COMPLETE, Interrupt::STOP_DETECTION, Interrupt::TRANSMIT, Interrupt::RECEIVE);
    clearFlag(Flag::IS_STOP_DETECTED);
    flushTXDR();
    endTransaction();
  }

  if (isInterruptEnabled(Interrupt::TRANSFER_COMPLETE) && isFlagSet(Flag::IS_TRANSFER_COMPLETED))
  {
    uint32_t registerValueCR2 = MemoryAccess::getRegisterValue(&(m_I2CPeripheralPtr->CR2));

    if (TransactionTag::READ_MEMORY == m_transactionTag)
    {
      enableAutoEndMode(registerValueCR2);
      setNumberOfBytesToTransfer(registerValueCR2, m_messageLen);
      setTransferDirectionToRead(registerValueCR2);
    }

    if (TransactionTag::WRITE_MEMORY == m_transactionTag)
    {
      enableAutoEndMode(registerValueCR2);
      disableReloadMode(registerValueCR2);
      setNumberOfBytesToTransfer(registerValueCR2, m_messageLen);
    }

    MemoryAccess::setRegisterValue(&(m_I2CPeripheralPtr->CR2), registerValueCR2);
  }

  if (isInterruptEnabled(Interrupt::TRANSMIT) && isFlagSet(Flag::DATA_TO_TXDR_MUST_BE_WRITTEN))
  {
    if (m_messagePos < m_messageLen)
    {
      writeData(reinterpret_cast<const uint8_t*>(m_messagePtr)[m_messagePos++]);
    }
  }

  if (isInterruptEnabled(Interrupt::RECEIVE) && isFlagSet(Flag::RXDR_NOT_EMPTY))
  {
    if (m_messagePos < m_messageLen)
    {
      readData(reinterpret_cast<uint8_t*>(m_messagePtr)[m_messagePos++]);
    }
  }
}

inline void I2C::enableInterrupts(uint32_t &registerValueCR1, Interrupt interrupt)
{
  const auto bitPosition = s_interruptCSRegisterMapping[static_cast<uint8_t>(interrupt)].bitPosition;
  registerValueCR1 = MemoryUtility<uint32_t>::setBit(registerValueCR1, bitPosition);
}

template<typename... Args>
inline void I2C::enableInterrupts(uint32_t &registerValueCR1, Interrupt interrupt, Args... args)
{
  enableInterrupts(registerValueCR1, interrupt);
  enableInterrupts(registerValueCR1, args...);
}

template<typename... Args>
void I2C::enableInterrupts(Args... args)
{
  uint32_t registerValueCR1 = MemoryAccess::getRegisterValue(&(m_I2CPeripheralPtr->CR1));
  enableInterrupts(registerValueCR1, args...);
  MemoryAccess::setRegisterValue(&(m_I2CPeripheralPtr->CR1), registerValueCR1);
}

inline void I2C::disableInterrupts(uint32_t &registerValueCR1, Interrupt interrupt)
{
  const auto bitPosition = s_interruptCSRegisterMapping[static_cast<uint8_t>(interrupt)].bitPosition;
  registerValueCR1 = MemoryUtility<uint32_t>::resetBit(registerValueCR1, bitPosition);
}

template<typename... Args>
inline void I2C::disableInterrupts(uint32_t &registerValueCR1, Interrupt interrupt, Args... args)
{
  disableInterrupts(registerValueCR1, interrupt);
  disableInterrupts(registerValueCR1, args...);
}

template<typename... Args>
void I2C::disableInterrupts(Args... args)
{
  uint32_t registerValueCR1 = MemoryAccess::getRegisterValue(&(m_I2CPeripheralPtr->CR1));
  disableInterrupts(registerValueCR1, args...);
  MemoryAccess::setRegisterValue(&(m_I2CPeripheralPtr->CR1), registerValueCR1);
}

bool I2C::isInterruptEnabled(Interrupt interrupt) const
{
  const auto registerOffset = s_interruptCSRegisterMapping[static_cast<uint8_t>(interrupt)].registerOffset;
  const auto bitPosition = s_interruptCSRegisterMapping[static_cast<uint8_t>(interrupt)].bitPosition;
  const uint32_t *registerPtr =
    reinterpret_cast<uint32_t*>((reinterpret_cast<uintptr_t>(m_I2CPeripheralPtr) + registerOffset));

  const uint32_t registerValue = MemoryAccess::getRegisterValue(registerPtr);
  return MemoryUtility<uint32_t>::isBitSet(registerValue, bitPosition);
}

bool I2C::isFlagSet(Flag flag) const
{
  const auto bitPosition = s_interruptStatusFlagsRegisterMapping[static_cast<uint8_t>(flag)].bitPosition;

  const uint32_t registerValue = MemoryAccess::getRegisterValue(&(m_I2CPeripheralPtr->ISR));
  return MemoryUtility<uint32_t>::isBitSet(registerValue, bitPosition);
}

inline void I2C::clearFlag(Flag flag)
{
  const auto bitPosition = s_interruptStatusFlagsRegisterMapping[static_cast<uint8_t>(flag)].bitPosition;
  RegisterUtility<uint32_t>::setBitInRegister(&(m_I2CPeripheralPtr->ICR), bitPosition);
}

bool I2C::startTransaction(void)
{
  bool isTxTransacationStarted = false;

  if (not isTransactionOngoing())
  {
    m_isTransactionCompleted = false;
    isTxTransacationStarted  = true;
  }

  return isTxTransacationStarted;
}

inline void I2C::endTransaction(void)
{
  m_isTransactionCompleted = true;
}

inline void I2C::disableI2C(void)
{
  constexpr uint32_t I2C_CR1_PE_POSITION = 0u;
  RegisterUtility<uint32_t>::resetBitInRegister(&(m_I2CPeripheralPtr->CR1), I2C_CR1_PE_POSITION);
}

inline void I2C::enableI2C(void)
{
  constexpr uint32_t I2C_CR1_PE_POSITION = 0u;
  RegisterUtility<uint32_t>::setBitInRegister(&(m_I2CPeripheralPtr->CR1), I2C_CR1_PE_POSITION);
}

inline void I2C::disableAnalogNoiseFilter(uint32_t &registerValueCR1)
{
  constexpr uint32_t I2C_CR1_ANFOFF_POSITION = 12u;
  registerValueCR1 = MemoryUtility<uint32_t>::resetBit(registerValueCR1, I2C_CR1_ANFOFF_POSITION);
}

inline void I2C::disableDigitalNoiseFilter(uint32_t &registerValueCR1)
{
  constexpr uint32_t I2C_CR1_DNF_POSITION = 8u;
  constexpr uint32_t I2C_CR1_DNF_NUM_OF_BITS = 4u;
  constexpr uint32_t I2C_CR1_DNF_DISABLED_VALUE = 0x0;

  registerValueCR1 = MemoryUtility<uint32_t>::setBits(
    registerValueCR1,
    I2C_CR1_DNF_POSITION,
    I2C_CR1_DNF_NUM_OF_BITS,
    I2C_CR1_DNF_DISABLED_VALUE);
}

inline void I2C::disableClockStretching(uint32_t &registerValueCR1)
{
  constexpr uint32_t I2C_CR1_NOSTRETCH_POSITION = 17u;
  registerValueCR1 = MemoryUtility<uint32_t>::setBit(registerValueCR1, I2C_CR1_NOSTRETCH_POSITION);
}

inline void I2C::setAddressingMode(uint32_t &registerValueCR2, AddressingMode addressingMode)
{
  constexpr uint32_t I2C_CR2_ADDR10_POSITION = 11u;
  constexpr uint32_t I2C_CR2_ADDR10_NUM_OF_BITS = 1u;

  registerValueCR2 = MemoryUtility<uint32_t>::setBits(
    registerValueCR2,
    I2C_CR2_ADDR10_POSITION,
    I2C_CR2_ADDR10_NUM_OF_BITS,
    static_cast<uint32_t>(addressingMode));
}

inline void I2C::enableAutoEndMode(uint32_t &registerValueCR2)
{
  constexpr uint32_t I2C_CR2_AUTOEND_POSITION = 25u;
  registerValueCR2 = MemoryUtility<uint32_t>::setBit(registerValueCR2, I2C_CR2_AUTOEND_POSITION);
}

inline void I2C::disableAutoEndMode(uint32_t &registerValueCR2)
{
  constexpr uint32_t I2C_CR2_AUTOEND_POSITION = 25u;
  registerValueCR2 = MemoryUtility<uint32_t>::resetBit(registerValueCR2, I2C_CR2_AUTOEND_POSITION);
}

inline void I2C::enableReloadMode(uint32_t &registerValueCR2)
{
  constexpr uint32_t I2C_CR2_RELOAD_POSITION = 24u;
  registerValueCR2 = MemoryUtility<uint32_t>::setBit(registerValueCR2, I2C_CR2_RELOAD_POSITION);
}

inline void I2C::disableReloadMode(uint32_t &registerValueCR2)
{
  constexpr uint32_t I2C_CR2_RELOAD_POSITION = 24u;
  registerValueCR2 = MemoryUtility<uint32_t>::resetBit(registerValueCR2, I2C_CR2_RELOAD_POSITION);
}

void I2C::setSlaveAddress(uint32_t &registerValueCR2, uint16_t slaveAddress)
{
  if (isAddressingMode10Bits(registerValueCR2))
  {
    setSlaveAddress10Bits(registerValueCR2, slaveAddress);
  }
  else
  {
    setSlaveAddress7Bits(registerValueCR2, slaveAddress);
  }
}

inline void I2C::setSlaveAddress7Bits(uint32_t &registerValueCR2, uint16_t slaveAddress)
{
  constexpr uint32_t I2C_CR2_SADD_POSITION = 1u;
  constexpr uint32_t I2C_CR2_SADD_NUM_OF_BITS = 7u;

  registerValueCR2 = MemoryUtility<uint32_t>::setBits(
    registerValueCR2,
    I2C_CR2_SADD_POSITION,
    I2C_CR2_SADD_NUM_OF_BITS,
    slaveAddress);
}

inline void I2C::setSlaveAddress10Bits(uint32_t &registerValueCR2, uint16_t slaveAddress)
{
  constexpr uint32_t I2C_CR2_SADD_POSITION = 0u;
  constexpr uint32_t I2C_CR2_SADD_NUM_OF_BITS = 10u;

  registerValueCR2 = MemoryUtility<uint32_t>::setBits(
    registerValueCR2,
    I2C_CR2_SADD_POSITION,
    I2C_CR2_SADD_NUM_OF_BITS,
    slaveAddress);
}

inline void I2C::writeData(uint8_t data)
{
  MemoryAccess::setRegisterValue(&(m_I2CPeripheralPtr->TXDR), data);
}

inline void I2C::readData(uint8_t &storeLocation)
{
  storeLocation = static_cast<uint8_t>(MemoryAccess::getRegisterValue(&(m_I2CPeripheralPtr->RXDR)));
}

inline bool I2C::isAddressingMode10Bits(uint32_t &registerValueCR2)
{
  constexpr uint32_t I2C_CR2_ADDR10_POSITION = 11u;
  return MemoryUtility<uint32_t>::isBitSet(registerValueCR2, I2C_CR2_ADDR10_POSITION);
}

void I2C::setNumberOfBytesToTransfer(uint32_t &registerValueCR2, uint32_t numberOfBytes)
{
  constexpr uint32_t I2C_CR2_NBYTES_POSITION = 16u;
  constexpr uint32_t I2C_CR2_NBYTES_NUM_OF_BITS = 8u;

  registerValueCR2 = MemoryUtility<uint32_t>::setBits(
    registerValueCR2,
    I2C_CR2_NBYTES_POSITION,
    I2C_CR2_NBYTES_NUM_OF_BITS,
    numberOfBytes);
}

void I2C::setTransferDirectionToWrite(uint32_t &registerValueCR2)
{
  constexpr uint32_t I2C_CR2_RD_WRN_POSITION = 10u;
  registerValueCR2 = MemoryUtility<uint32_t>::resetBit(registerValueCR2, I2C_CR2_RD_WRN_POSITION);
}

void I2C::setTransferDirectionToRead(uint32_t &registerValueCR2)
{
  constexpr uint32_t I2C_CR2_RD_WRN_POSITION = 10u;
  registerValueCR2 = MemoryUtility<uint32_t>::setBit(registerValueCR2, I2C_CR2_RD_WRN_POSITION);
}

void I2C::setStartTransactionFlag(uint32_t &registerValueCR2)
{
  constexpr uint32_t I2C_CR2_START_POSITION = 13u;
  registerValueCR2 = MemoryUtility<uint32_t>::setBit(registerValueCR2, I2C_CR2_START_POSITION);
}

void I2C::clearStopFlag(uint32_t &registerValueCR2)
{
  constexpr uint32_t I2C_CR2_STOP_POSITION = 14u;
  registerValueCR2 = MemoryUtility<uint32_t>::resetBit(registerValueCR2, I2C_CR2_STOP_POSITION);
}

void I2C::flushTXDR(void)
{
  constexpr uint8_t DUMMY_VALUE = 0x0;
  constexpr uint32_t I2C_ISR_TXE_POSITION = 0u;

  if (isFlagSet(Flag::DATA_TO_TXDR_MUST_BE_WRITTEN))
  {
    writeData(DUMMY_VALUE);
  }

  if (not isFlagSet(Flag::IS_TXDR_REGISTER_EMPTY))
  {
    RegisterUtility<uint32_t>::setBitInRegister(&(m_I2CPeripheralPtr->ISR), I2C_ISR_TXE_POSITION);
  }
}

uint32_t I2C::getMinimumSDADEL(uint32_t inputClockPeriod, uint32_t internalClockPeriod)
{
  const uint32_t minuend = MAXIMUM_CLOCK_FALLING_TIME + MINIMUM_DATA_HOLD_TIME;
  const uint32_t subtrahend = MINIMUM_DELAY_ANALOG_FILTER - ((DNF + 3u) * inputClockPeriod);

  return (minuend > subtrahend) ? ((minuend - subtrahend) / internalClockPeriod) : 0u;
}

uint32_t I2C::getMaximumSDADEL(uint32_t inputClockPeriod, uint32_t internalClockPeriod)
{
  constexpr uint32_t SDADEL_NUM_OF_BITS = 4u;
  constexpr uint32_t SDADEL_MAX_VALUE = (1u << SDADEL_NUM_OF_BITS) - 1u;

  const uint32_t maximumSDADEL =
    (MAXIMUM_DATA_VALID_TIME - MAXIMUM_CLOCK_RISING_TIME - MAXIMUM_DELAY_ANALOG_FILTER - ((DNF + 4u) * inputClockPeriod)) / internalClockPeriod;

  return (maximumSDADEL <= SDADEL_MAX_VALUE) ? maximumSDADEL : SDADEL_MAX_VALUE;
}

uint32_t I2C::getMinimumSCLDEL(uint32_t internalClockPeriod)
{
  return (MAXIMUM_CLOCK_RISING_TIME + MINIMUM_DATA_SETUP_TIME) / internalClockPeriod - 1u;
}

uint32_t I2C::getMaximumSCLDEL(void)
{
  constexpr uint32_t SCLDEL_NUM_OF_BITS = 4u;
  constexpr uint32_t SCLDEL_MAX_VALUE = (1u << SCLDEL_NUM_OF_BITS) - 1u;

  return SCLDEL_MAX_VALUE;
}

uint32_t I2C::getMinimumSCLL(uint32_t internalClockPeriod)
{
  constexpr uint32_t MINIMUM_LOW_PERIOD_OF_SCL = 4700u; // ns
  return (MINIMUM_LOW_PERIOD_OF_SCL / internalClockPeriod);
}

uint32_t I2C::getMinimumSCLH(uint32_t internalClockPeriod)
{
  constexpr uint32_t MINIMUM_HIGH_PERIOD_OF_SCL = 4000u; // ns
  return (MINIMUM_HIGH_PERIOD_OF_SCL / internalClockPeriod);
}

uint32_t I2C::getMaximumSCLL(void)
{
  constexpr uint32_t SCLL_NUM_OF_BITS = 8u;
  constexpr uint32_t SCLL_MAX_VALUE = (1u << SCLL_NUM_OF_BITS) - 1u;

  return SCLL_MAX_VALUE;
}

uint32_t I2C::getMaximumSCLH(void)
{
  constexpr uint32_t SCLH_NUM_OF_BITS = 8u;
  constexpr uint32_t SCLH_MAX_VALUE = (1u << SCLH_NUM_OF_BITS) - 1u;

  return SCLH_MAX_VALUE;
}

uint32_t I2C::getSync1Period(uint32_t inputClockPeriod)
{
  return (MAXIMUM_CLOCK_FALLING_TIME * 6u / 10u) + MAXIMUM_DELAY_ANALOG_FILTER + ((DNF + 3u) * inputClockPeriod);
}

uint32_t I2C::getSync2Period(uint32_t inputClockPeriod)
{
  return (MAXIMUM_CLOCK_RISING_TIME * 6u / 10u) + MAXIMUM_DELAY_ANALOG_FILTER + ((DNF + 3u) * inputClockPeriod);
}

uint32_t I2C::getOutputClockPeriod(const TimingRegisterConfig &timingRegisterConfig, uint32_t inputClockPeriod)
{
  const uint32_t internalClockPeriod = inputClockPeriod * (timingRegisterConfig.PRESC + 1u);
  return getSync1Period(inputClockPeriod) + getSync2Period(inputClockPeriod) + (timingRegisterConfig.SCLL + timingRegisterConfig.SCLH) * internalClockPeriod;
}

I2C::ErrorCode
I2C::setClockLowAndHighTimerPeriod(TimingRegisterConfig &timingRegisterConfig, uint32_t wantedOutputClockPeriod, uint32_t inputClockPeriod)
{
  const uint32_t maximumSCLL = getMaximumSCLL();
  const uint32_t maximumSCLH = getMaximumSCLH();
  const uint32_t internalClockPeriod = inputClockPeriod * (timingRegisterConfig.PRESC + 1u);

  timingRegisterConfig.SCLL  = static_cast<uint8_t>(getMinimumSCLL(internalClockPeriod));
  timingRegisterConfig.SCLH  = static_cast<uint8_t>(getMinimumSCLH(internalClockPeriod));

  if (getOutputClockPeriod(timingRegisterConfig, inputClockPeriod) > wantedOutputClockPeriod)
  {
    return ErrorCode::WANTED_OUTPUT_CLOCK_PERIOD_TOO_SHORT;
  }

  bool isMaximumSCLLReached = false;
  bool isMaximumSCLHReached = false;
  bool increaseSCLL = true;

  while (getOutputClockPeriod(timingRegisterConfig, inputClockPeriod) < wantedOutputClockPeriod)
  {
    if (increaseSCLL == true)
    {
      if (maximumSCLL > timingRegisterConfig.SCLL)
      {
        ++timingRegisterConfig.SCLL;
      }
      else
      {
        isMaximumSCLLReached = true;
      }
    }
    else
    {
      if (maximumSCLH > timingRegisterConfig.SCLH)
      {
        ++timingRegisterConfig.SCLH;
      }
      else
      {
        isMaximumSCLHReached = true;
      }
    }

    if (isMaximumSCLLReached && isMaximumSCLHReached)
    {
      return ErrorCode::WANTED_OUTPUT_CLOCK_PERIOD_NOT_ACHIEVABLE;
    }

    increaseSCLL = not increaseSCLL;
  }

  return ErrorCode::OK;
}

I2C::ErrorCode
I2C::setDataSetupAndHoldTimePeriod(TimingRegisterConfig &timingRegisterConfig, uint32_t inputClockPeriod)
{
  const uint32_t internalClockPeriod = inputClockPeriod * (timingRegisterConfig.PRESC + 1u);

  const uint32_t minimumSDADEL = getMinimumSDADEL(inputClockPeriod, internalClockPeriod);
  const uint32_t minimumSCLDEL = getMinimumSCLDEL(internalClockPeriod);
  const uint32_t maximumSDADEL = getMaximumSDADEL(inputClockPeriod, internalClockPeriod);
  const uint32_t maximumSCLDEL = getMaximumSCLDEL();

  if ((minimumSDADEL > maximumSDADEL) || (minimumSCLDEL > maximumSCLDEL))
  {
    return ErrorCode::WANTED_OUTPUT_CLOCK_PERIOD_NOT_ACHIEVABLE;
  }

  timingRegisterConfig.SDADEL = (7u * minimumSDADEL + 3u * maximumSDADEL) / 10u;
  timingRegisterConfig.SCLDEL = (7u * minimumSCLDEL + 3u * maximumSCLDEL) / 10u;

  return ErrorCode::OK;
}

I2C::ErrorCode I2C::getInputClockFrequency(uint32_t &inputClockFrequency)
{
  ErrorCode errorCode = ErrorCode::OK;

  const auto clockControlErrorCode =  m_clockControlPtr->getClockFrequency(
    static_cast<Peripheral>(reinterpret_cast<uintptr_t>(m_I2CPeripheralPtr)),
    inputClockFrequency);

  if (ClockControl::ErrorCode::OK != clockControlErrorCode)
  {
    errorCode = ErrorCode::SCL_CLOCK_TIMING_SETUP_PROBLEM;
  }

  return errorCode;
}

I2C::ErrorCode I2C::findTimingRegisterConfig(
  TimingRegisterConfig &timingRegisterConfig,
  uint32_t inputClockPeriod,
  uint32_t wantedOutputClockPeriod)
{
  ErrorCode errorCode = ErrorCode::WANTED_OUTPUT_CLOCK_PERIOD_NOT_ACHIEVABLE;

  for (uint32_t PRESC = 3u; (3u >= PRESC) && (ErrorCode::OK != errorCode); --PRESC)
  {
    timingRegisterConfig.PRESC = PRESC;

    errorCode = setClockLowAndHighTimerPeriod(timingRegisterConfig, wantedOutputClockPeriod, inputClockPeriod);

    if (ErrorCode::OK == errorCode)
    {
      errorCode = setDataSetupAndHoldTimePeriod(timingRegisterConfig, inputClockPeriod);
    }
  }

  return errorCode;
}

I2C::ErrorCode I2C::setupSCLClockTiming(uint32_t clockFrequencySCL)
{
  uint32_t inputClockFrequency = 0u;
  ErrorCode errorCode = getInputClockFrequency(inputClockFrequency);

  if (ErrorCode::OK == errorCode)
  {
    const uint32_t inputClockPeriod = NANOSECONDS_IN_SECOND / inputClockFrequency;
    const uint32_t wantedOutputClockPeriod = NANOSECONDS_IN_SECOND / clockFrequencySCL;

    TimingRegisterConfig timingRegisterConfig;

    errorCode = findTimingRegisterConfig(timingRegisterConfig, inputClockPeriod, wantedOutputClockPeriod);

    if (ErrorCode::OK == errorCode)
    {
      setTimingRegister(timingRegisterConfig);
    }
  }

  return errorCode;
}

void I2C::setTimingRegister(const TimingRegisterConfig &timingRegisterConfig)
{
  constexpr uint32_t I2C_TIMINGR_SCLL_POSITION      = 0u;
  constexpr uint32_t I2C_TIMINGR_SCLL_NUM_OF_BITS   = 8u;
  constexpr uint32_t I2C_TIMINGR_SCLH_POSITION      = 8u;
  constexpr uint32_t I2C_TIMINGR_SCLH_NUM_OF_BITS   = 8u;
  constexpr uint32_t I2C_TIMINGR_SDADEL_POSITION    = 16u;
  constexpr uint32_t I2C_TIMINGR_SDADEL_NUM_OF_BITS = 4u;
  constexpr uint32_t I2C_TIMINGR_SCLDEL_POSITION    = 20u;
  constexpr uint32_t I2C_TIMINGR_SCLDEL_NUM_OF_BITS = 4u;
  constexpr uint32_t I2C_TIMINGR_PRESC_POSITION     = 28u;
  constexpr uint32_t I2C_TIMINGR_PRESC_NUM_OF_BITS  = 4u;

  uint32_t registerValueTIMINGR = 0u;

  registerValueTIMINGR = MemoryUtility<uint32_t>::setBits(
    registerValueTIMINGR,
    I2C_TIMINGR_SCLL_POSITION,
    I2C_TIMINGR_SCLL_NUM_OF_BITS,
    static_cast<uint32_t>(timingRegisterConfig.SCLL));

  registerValueTIMINGR = MemoryUtility<uint32_t>::setBits(
    registerValueTIMINGR,
    I2C_TIMINGR_SCLH_POSITION,
    I2C_TIMINGR_SCLH_NUM_OF_BITS,
    static_cast<uint32_t>(timingRegisterConfig.SCLH));

  registerValueTIMINGR = MemoryUtility<uint32_t>::setBits(
    registerValueTIMINGR,
    I2C_TIMINGR_SDADEL_POSITION,
    I2C_TIMINGR_SDADEL_NUM_OF_BITS,
    static_cast<uint32_t>(timingRegisterConfig.SDADEL));

  registerValueTIMINGR = MemoryUtility<uint32_t>::setBits(
    registerValueTIMINGR,
    I2C_TIMINGR_SCLDEL_POSITION,
    I2C_TIMINGR_SCLDEL_NUM_OF_BITS,
    static_cast<uint32_t>(timingRegisterConfig.SCLDEL));

  registerValueTIMINGR = MemoryUtility<uint32_t>::setBits(
    registerValueTIMINGR,
    I2C_TIMINGR_PRESC_POSITION,
    I2C_TIMINGR_PRESC_NUM_OF_BITS,
    static_cast<uint32_t>(timingRegisterConfig.PRESC));

  MemoryAccess::setRegisterValue(&(m_I2CPeripheralPtr->TIMINGR), registerValueTIMINGR);
}

  I2C::ErrorCode I2C::enablePeripheralClock(void)
{
  ResetControl::ErrorCode errorCode = m_resetControlPtr->enablePeripheralClock(getPeripheralTag());

  return (ResetControl::ErrorCode::OK == errorCode) ? ErrorCode::OK : ErrorCode::CAN_NOT_TURN_ON_PERIPHERAL_CLOCK;
}