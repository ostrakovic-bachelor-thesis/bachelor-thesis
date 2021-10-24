#ifndef CLOCK_CONTROL_H
#define CLOCK_CONTROL_H

#include "stm32l4r9xx.h"
#include "MemoryAccess.h"
#include "MemoryUtility.h"
#include "Peripheral.h"
#include <cstdint>


class ClockControl
{
public:

  /**
   * @brief Constructor of ClockControl class instance.
   *
   * @param[in] - Pointer to a RCC peripheral instance.
   */
  ClockControl(RCC_TypeDef *RCCPeripheralPtr);

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ~ClockControl() = default;

  //! This enum class represents errors which can happen during method calls
  enum class ErrorCode : uint8_t
  {
    OK                             = 0u,
    INVALID_CLOCK_SOURCE           = 1u,
    NOT_IMPLEMENTED_FOR_PERIPHERAL = 2u
  };

  // TODO
  enum class Clock : uint8_t
  {
    LSE = 0u,     //!<
    MSI,          //!<
    HSI,          //!<
    HSE,          //!<
    PLL,          //!<
    PLLSAI2,      //!<
    SYSTEM_CLOCK, //!<
    AHB,          //!<
    APB1,         //!<
    APB2,         //!<
    NO_CLOCK      //!<
  };

  struct PLLSAI2Configuration
  {
    uint8_t inputClockDivider;
    uint8_t inputClockMultiplier;
    uint8_t outputClockPDivider;
    uint8_t outputClockQDivider;
    uint8_t outputClockRDivider;
    uint8_t ltdcClockDivider;
    bool    enableOutputClockP;
    bool    enableOutputClockQ;
    bool    enableOutputClockR;
  };

  /**
   * TODO
   */
#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode setClockSource(Clock clock, Clock clockSource);

  /**
   * TODO
   */
#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode configurePLL(const PLLSAI2Configuration &pllSAI2Config);

  /**
   * TODO
   */
#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode getClockFrequency(Clock clock, uint32_t &clockFrequency) const;

  /**
   * TODO
   */
#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode getClockFrequency(Peripheral peripheral, uint32_t &clockFrequency) const;

#ifdef UNIT_TEST
  /**
   * @brief Method gets raw pointer to underlaying RCC peripheral instance.
   *
   * @return Pointer to underlaying RCC peripheral instance.
   */
  inline void* getRawPointer(void) const
  {
    return reinterpret_cast<void*>(m_RCCPeripheralPtr);
  }
#endif // #ifdef UNIT_TEST

private:

  // TODO
  typedef uint32_t (ClockControl::*GetClockFrequencyFunction_t)(void) const;

  bool isMSIClockFrequencyDefinedInCR(void) const;
  uint32_t getMsiClockFreqencyIndexFromCR(void) const;
  uint32_t getMsiClockFreqencyIndexFromCSR(void) const;

  uint32_t getHSIClockFrequency(void) const;
  uint32_t getHSEClockFrequency(void) const;
  uint32_t getMSIClockFrequency(void) const;
  uint32_t getLSEClockFrequency(void) const;
  uint32_t getPLLClockFrequency(void) const;
  uint32_t getSystemClockFrequency(void) const;
  uint32_t getAHBClockFrequency(void) const;
  uint32_t getAPB1ClockFrequency(void) const;
  uint32_t getAPB2ClockFrequency(void) const;
  uint32_t getPLLInputClockFrequency(void) const;
  uint32_t getNoClockFrequency(void) const;

  uint32_t getUSART1ClockFrequency(void) const;
  uint32_t getUSART2ClockFrequency(void) const;
  uint32_t getI2C1ClockFrequency(void) const;

  ErrorCode setPLLClockSource(Clock clockSource);

  ErrorCode mapToPLLClockSource(Clock clockSource, uint8_t &pllClockSource);

  void setPLLInputClockDivider(uint32_t &registerValuePLLSAI2CFGR, uint8_t inputClockDivider);
  void setPLLInputClockMultiplier(uint32_t &registerValuePLLSAI2CFGR, uint8_t inputClockMultiplier);
  void setPLLOutputClockRDivider(uint32_t &registerValuePLLSAI2CFGR, uint8_t outputClockRDivider);
  void setPLLOutputClockQDivider(uint32_t &registerValuePLLSAI2CFGR, uint8_t outputClockQDivider);
  void setPLLOutputClockPDivider(uint32_t &registerValuePLLSAI2CFGR, uint8_t outputClockPDivider);
  void enableOutputClockP(uint32_t &registerValuePLLSAI2CFGR);
  void disableOutputClockP(uint32_t &registerValuePLLSAI2CFGR);
  void enableOutputClockQ(uint32_t &registerValuePLLSAI2CFGR);
  void disableOutputClockQ(uint32_t &registerValuePLLSAI2CFGR);
  void enableOutputClockR(uint32_t &registerValuePLLSAI2CFGR);
  void disableOutputClockR(uint32_t &registerValuePLLSAI2CFGR);
  void setLTDCClockDivider(uint8_t ltdcClockDivider);

  void turnOffPLLSAI2(void);
  void turnOnPLLSAI2(void);
  void requestTurningOffPLLSAI2(void);
  void requestTurningOnPLLSAI2(void);
  bool isPLLSAI2TurnedOn(void) const;

  static const uint32_t s_msiClockFreq[];

  static const uint32_t s_AHBPrescaler[];

  static const uint32_t s_APBxPrescaler[];

  static const GetClockFrequencyFunction_t s_i2cClockFrequency[];

  static const GetClockFrequencyFunction_t s_usart1ClockFrequency[];

  static const GetClockFrequencyFunction_t s_usartClockFrequency[];

  static const GetClockFrequencyFunction_t s_PLLInputClockFrequency[];

  static const GetClockFrequencyFunction_t s_systemClockFrequency[];

  static const GetClockFrequencyFunction_t s_clockFrequency[];

  //! Pointer to RCC peripheral
  RCC_TypeDef *m_RCCPeripheralPtr;

};

#endif // #ifndef CLOCK_CONTROL_H