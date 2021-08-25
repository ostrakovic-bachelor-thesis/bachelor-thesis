#ifndef I2C_H
#define I2C_H

#include "stm32l4r9xx.h"
#include "ClockControl.h"
#include "Peripheral.h"
#include <cstdint>


class I2C
{
public:

  I2C(I2C_TypeDef *I2CPeripheralPtr, ClockControl *clockControlPtr);

  //! This enum class represents errors which can happen during method calls
  enum class ErrorCode : uint8_t
  {
    OK                             = 0u,
    SCL_CLOCK_TIMING_SETUP_PROBLEM = 1u,
  };

  enum class AddressingMode : uint8_t
  {
    ADDRESS_7_BITS  = 0b0,
    ADDRESS_10_BITS = 0b1
  };

  struct I2CConfig
  {
    AddressingMode addressingMode;
    uint32_t clockFrequencySCL;
  };

  ErrorCode init(const I2CConfig &i2cConfig);

private:

  struct TimingRegisterConfig
  {
    uint8_t SCLL;
    uint8_t SCLH;
    uint8_t SDADEL;
    uint8_t SCLDEL;
    uint8_t PRESC;
  };

  void enableI2C(void);
  void disableI2C(void);

  static void setAddressingMode(uint32_t &registerValueCR2, AddressingMode addressingMode);
  static void enableAutoEndMode(uint32_t &registerValueCR2);

  ErrorCode setupSCLClockTiming(uint32_t clockFrequencySCL);
  void setTimingRegister(const TimingRegisterConfig &timingRegisterConfig);

  //! Pointer to I2C peripheral
  I2C_TypeDef *m_I2CPeripheralPtr;

  //! Pointer to Clock Control module
  ClockControl *m_clockControlPtr;

};

#endif // #ifndef I2C_H