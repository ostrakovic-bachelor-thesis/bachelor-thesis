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
  enum class ClockSource : uint8_t
  {
    LSE = 0u,     //!<
    MSI,          //!<
    HSI,          //!<
    HSE,          //!<
    PLL,          //!<
    SYSTEM_CLOCK, //!<
    AHB,          //!<
    APB1,         //!<
    APB2,         //!<
    NO_CLOCK      //!<
  };

  /**
   * TODO
   */
#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode getClockFrequency(ClockSource clockSource, uint32_t &clockFrequency) const;

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

  static const uint32_t s_msiClockFreq[];

  static const uint32_t s_AHBPrescaler[];

  static const uint32_t s_APBxPrescaler[];

  static const GetClockFrequencyFunction_t s_usart1ClockFrequency[];

  static const GetClockFrequencyFunction_t s_PLLInputClockFrequency[];

  static const GetClockFrequencyFunction_t s_systemClockFrequency[];

  static const GetClockFrequencyFunction_t s_clockFrequency[];

  //! Pointer to RCC peripheral
  RCC_TypeDef *m_RCCPeripheralPtr;

};

#endif // #ifndef CLOCK_CONTROL_H