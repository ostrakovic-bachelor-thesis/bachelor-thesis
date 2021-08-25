#include "I2C.h"
#include "MemoryAccess.h"
#include "MemoryUtility.h"
#include "RegisterUtility.h"


I2C::I2C(I2C_TypeDef *I2CPeripheralPtr, ClockControl *clockControlPtr):
  m_I2CPeripheralPtr(I2CPeripheralPtr),
   m_clockControlPtr(clockControlPtr)
{}

I2C::ErrorCode I2C::init(const I2CConfig &i2cConfig)
{
  disableI2C();

  uint32_t registerValueCR2 = MemoryAccess::getRegisterValue(&(m_I2CPeripheralPtr->CR2));

  setAddressingMode(registerValueCR2, i2cConfig.addressingMode);
  enableAutoEndMode(registerValueCR2);

  MemoryAccess::setRegisterValue(&(m_I2CPeripheralPtr->CR2), registerValueCR2);

  ErrorCode errorCode = setupSCLClockTiming(i2cConfig.clockFrequencySCL);

  enableI2C();

  return ErrorCode::OK;
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

void I2C::setAddressingMode(uint32_t &registerValueCR2, AddressingMode addressingMode)
{
  constexpr uint32_t I2C_CR2_ADDR10_POSITION = 11u;
  constexpr uint32_t I2C_CR2_ADDR10_NUM_OF_BITS = 1u;

  registerValueCR2 = MemoryUtility<uint32_t>::setBits(
    registerValueCR2,
    I2C_CR2_ADDR10_POSITION,
    I2C_CR2_ADDR10_NUM_OF_BITS,
    static_cast<uint32_t>(addressingMode));
}

void I2C::enableAutoEndMode(uint32_t &registerValueCR2)
{
  constexpr uint32_t I2C_CR2_AUTOEND_POSITION = 25u;
  registerValueCR2 = MemoryUtility<uint32_t>::setBit(registerValueCR2, I2C_CR2_AUTOEND_POSITION);
}


I2C::ErrorCode I2C::setupSCLClockTiming(uint32_t clockFrequencySCL)
{
  uint32_t inputClockFrequency = 0u;

  const auto clockControlErrorCode =  m_clockControlPtr->getClockFrequency(
    static_cast<Peripheral>(reinterpret_cast<uintptr_t>(m_I2CPeripheralPtr)),
    inputClockFrequency);
  if (ClockControl::ErrorCode::OK != clockControlErrorCode)
  {
    return ErrorCode::SCL_CLOCK_TIMING_SETUP_PROBLEM;
  }

  if((16000000u == inputClockFrequency) && (100000u == clockFrequencySCL))
  {
    TimingRegisterConfig timingRegisterConfig =
    {
      .SCLL   = 0x13,
      .SCLH   = 0xF,
      .SDADEL = 0x2,
      .SCLDEL = 0x4,
      .PRESC  = 0x3
    };

    setTimingRegister(timingRegisterConfig);
  }

  return ErrorCode::OK;
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