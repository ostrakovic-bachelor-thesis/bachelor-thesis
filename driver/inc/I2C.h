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
    OK                                        = 0u,
    SCL_CLOCK_TIMING_SETUP_PROBLEM            = 1u,
    WANTED_OUTPUT_CLOCK_PERIOD_TOO_SHORT      = 2u,
    WANTED_OUTPUT_CLOCK_PERIOD_NOT_ACHIEVABLE = 3u,
    BUSY                                      = 4u
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

  ErrorCode write(uint16_t slaveAddress, const void *messagePtr, uint32_t messageLen);

  ErrorCode read(uint16_t slaveAddress, void *messagePtr, uint32_t messageLen);

  bool isTransactionOngoing(void) const;

  void IRQHandler(void);

private:

  enum class Interrupt : uint8_t
  {
    TRANSMIT = 0u,
    RECEIVE,
    STOP_DETECTION,
    TRANSFER_COMPLETE,

    COUNT
  };

  //! Interrupt and status flags
  enum class Flag : uint8_t
  {
    IS_TXDR_REGISTER_EMPTY = 0u,
    DATA_TO_TXDR_MUST_BE_WRITTEN,
    RXDR_NOT_EMPTY,
    IS_STOP_DETECTED,
    IS_TRANSFER_COMPLETED,

    COUNT
  };

  //! Constrol/status operation to register mapping
  struct CSRegisterMapping
  {
    uint32_t registerOffset;
    uint8_t  bitPosition;
  };

  constexpr static uint32_t NANOSECONDS_IN_SECOND = 1000000000u;

  constexpr static uint32_t MAXIMUM_CLOCK_RISING_TIME  = 1000u; // 1000ns
  constexpr static uint32_t MAXIMUM_CLOCK_FALLING_TIME = 300u;  // 300ns
  constexpr static uint32_t MINIMUM_DATA_HOLD_TIME     = 0u;    // 0 ns
  constexpr static uint32_t MAXIMUM_DATA_HOLD_TIME     = 3450u; // 3450ns
  constexpr static uint32_t MAXIMUM_DATA_VALID_TIME    = 3450u; // 3450ns
  constexpr static uint32_t MINIMUM_DATA_SETUP_TIME    = 250u;  // 250ns

  //! Analog filter is disabled by default, because of that minimum delay is 0ns
  constexpr static uint32_t MINIMUM_DELAY_ANALOG_FILTER = 0u;
  //! Analog filter is disabled by default, because of that maximum delay is 0ns
  constexpr static uint32_t MAXIMUM_DELAY_ANALOG_FILTER = 0u;

  //! Digital filter is disabled by default
  constexpr static uint32_t DNF = 0u;

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

  bool startTransaction(void);
  void endTransaction(void);

  void flushTXDR(void);

  void enableInterrupt(Interrupt interrupt);
  void disableInterrupt(Interrupt interrupt);
  bool isInterruptEnabled(Interrupt interrupt) const;
  void clearFlag(Flag flag);
  bool isFlagSet(Flag flag) const;

  void writeData(uint8_t data);
  void readData(uint8_t &storeLocation);

  ErrorCode getInputClockFrequency(uint32_t &inputClockFrequency);

  static void disableAnalogNoiseFilter(uint32_t &registerValueCR1);
  static void disableDigitalNoiseFilter(uint32_t &registerValueCR1);
  static void disableClockStretching(uint32_t &registerValueCR1);

  static void setAddressingMode(uint32_t &registerValueCR2, AddressingMode addressingMode);
  static void enableAutoEndMode(uint32_t &registerValueCR2);
  static void disableReloadMode(uint32_t &registerValueCR2);
  static void setNumberOfBytesToTransfer(uint32_t &registerValueCR2, uint32_t numberOfBytes);
  static void setSlaveAddress(uint32_t &registerValueCR2, uint16_t slaveAddress);
  static void setSlaveAddress7Bits(uint32_t &registerValueCR2, uint16_t slaveAddress);
  static void setSlaveAddress10Bits(uint32_t &registerValueCR2, uint16_t slaveAddress);
  static bool isAddressingMode10Bits(uint32_t &registerValueCR2);
  static void setTransferDirectionToWrite(uint32_t &registerValueCR2);
  static void setTransferDirectionToRead(uint32_t &registerValueCR2);
  static void setStartTransactionFlag(uint32_t &registerValueCR2);
  static void clearStopFlag(uint32_t &registerValueCR2);

  static uint32_t getMinimumSDADEL(uint32_t inputClockPeriod, uint32_t internalClockPeriod);
  static uint32_t getMaximumSDADEL(uint32_t inputClockPeriod, uint32_t internalClockPeriod);
  static uint32_t getMinimumSCLDEL(uint32_t inputClockPeriod);
  static uint32_t getMaximumSCLDEL(void);
  static uint32_t getMinimumSCLL(uint32_t internalClockPeriod);
  static uint32_t getMinimumSCLH(uint32_t internalClockPeriod);
  static uint32_t getMaximumSCLL(void);
  static uint32_t getMaximumSCLH(void);
  static uint32_t getSync1Period(uint32_t inputClockPeriod);
  static uint32_t getSync2Period(uint32_t inputClockPeriod);

  static uint32_t getOutputClockPeriod(const TimingRegisterConfig &timingRegisterConfig, uint32_t inputClockPeriod);

  static ErrorCode findTimingRegisterConfig(
    TimingRegisterConfig &timingRegisterConfig,
    uint32_t inputClockPeriod,
    uint32_t wantedOutputClockPeriod);

  static ErrorCode
  setClockLowAndHighTimerPeriod(TimingRegisterConfig &timingRegisterConfig, uint32_t wantedOutputClockPeriod, uint32_t inputClockPeriod);

  static ErrorCode
  setDataSetupAndHoldTimePeriod(TimingRegisterConfig &timingRegisterConfig, uint32_t inputClockPeriod);

  ErrorCode setupSCLClockTiming(uint32_t clockFrequencySCL);
  void setTimingRegister(const TimingRegisterConfig &timingRegisterConfig);

  //! TODO
  static const CSRegisterMapping s_interruptCSRegisterMapping[static_cast<uint8_t>(Interrupt::COUNT)];

  //! TODO
  static const CSRegisterMapping s_interruptStatusFlagsRegisterMapping[static_cast<uint8_t>(Flag::COUNT)];

  //! Pointer to I2C peripheral
  I2C_TypeDef *m_I2CPeripheralPtr;

  //! Pointer to Clock Control module
  ClockControl *m_clockControlPtr;

  //! Pointer to message to transmit/receive
  void *m_messagePtr;

  //! Size of message to transmit/receive
  uint32_t m_messageLen;

  //! Position of next data chunk to transmit / to where next received data chunk should be stored
  uint32_t m_messagePos;

  //! Is transmit/receive transaction completed
  bool m_isTransactionCompleted;

};

#endif // #ifndef I2C_H