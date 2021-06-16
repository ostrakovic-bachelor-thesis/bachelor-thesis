#ifndef RCC_H
#define RCC_H

#include "stm32l4r9xx.h"
#include "MemoryAccess.h"
#include "MemoryUtility.h"
#include <Peripheral.h>
#include <cstdint>

#ifdef RCC
#undef RCC
#endif // #ifdef RCC


class RCC
{
public:

  /**
   * @brief Constructor of RCC class instance.
   * 
   * @param[in] - Pointer to a RCC peripheral instance.
   */
  RCC(RCC_TypeDef *RCCPeripheralPtr);

  //! This enum class represents errors which can happen during method calls
  enum class ErrorCode : uint8_t
  {
    OK              = 0u,
    NOT_IMPLEMENTED = 1u,
    INTERNAL        = 2u, //!< Indicates problem with the implementation, should never happen (NON-TESTABLE)
  };

  // TODO
  enum class ClockSource : uint8_t
  {
    MSI = 0b00,
    HSI = 0b01,
    HSE = 0b10,
    PLL = 0b11,
    NO_CLOCK
  };

  /**
   * @brief Method enables peripheral clock.
   *
   * @param[in] peripheral - Peripheral which clock will be enabled.
   * @return ::ErrorCode::OK is returned in the case of success. In the case of failure, 
   *         it returns corresponding error code from ::ErrorCode.
   */
  ErrorCode enablePeripheralClock(Peripheral peripheral);

  /**
   * @brief Method disables peripheral clock.
   *
   * @param[in] peripheral - Peripheral which clock will be disabled.
   * @return ::ErrorCode::OK is returned in the case of success. In the case of failure, 
   *         it returns corresponding error code from ::ErrorCode.
   */
  ErrorCode disablePeripheralClock(Peripheral peripheral);

  /**
   * @brief Method checks is peripheral clock enabled.
   *
   * @param[in]  peripheral     - Peripheral which clock will checked.
   * @param[out] isClockEnabled - Reference to variable in which state of enabling will be written.
   * @return ::ErrorCode::OK is returned in the case of success. In the case of failure, 
   *         it returns corresponding error code from ::ErrorCode.
   */
  ErrorCode isPeripheralClockEnabled(Peripheral peripheral, bool &isClockEnabled) const;

  /**
   * TODO
   */
  ClockSource getSystemClockSource(void) const;
  
  /**
   * TODO
   */
  uint32_t getHSIClockFrequency(void) const;

  /**
   * TODO
   */
  uint32_t getHSEClockFrequency(void) const;
  
  /**
   * TODO
   */
  uint32_t getMSIClockFrequency(void) const;

  /**
   * TODO
   */
  uint32_t getPLLClockFrequency(void) const;

  /**
   * TODO
   */
  uint32_t getSystemClockFrequency(void) const;

private:

  //
  enum class MSIClockFrequency
  {
    FREQ_100KHZ = 0b0000,
    FREQ_200KHZ = 0b0001,
    FREQ_400KHZ = 0b0010,
    FREQ_800KHZ = 0b0011,
    FREQ_1MHZ   = 0b0100,
    FREQ_2MHZ   = 0b0101,
    FREQ_4MHZ   = 0b0110,
    FREQ_8MHZ   = 0b0111,
    FREQ_16MHZ  = 0b1000,
    FREQ_24MHZ  = 0b1001,
    FREQ_32MHZ  = 0b1010,
    FREQ_48MHZ  = 0b1011
  };

  enum class Register : uint8_t
  {
    AHB1ENR,
    AHB2ENR,
    AHB3ENR,
    APB1ENR1,
    APB1ENR2,
    APB2ENR,
  };

  struct EnablePeripheralClockRegisterMapping
  {
    Peripheral peripheral;
    Register   enableRegister;
    uint8_t    enableBitPosition;
  };

  const EnablePeripheralClockRegisterMapping* findEnableClockRegisterMapping(Peripheral peripheral) const;

  volatile uint32_t* getPointerToRegister(Register register) const;
  
  bool setBitInRegister(Register register, uint8_t bitInRegister);

  bool resetBitInRegister(Register rccRegister, uint8_t bitInRegister);

  bool isBitSetInRegister(Register rccRegister, uint8_t bitInRegister, bool &isBitSet) const;

  bool isMSIClockFrequencyDefinedInCR(void) const;

  uint32_t getMsiClockFreqencyIndexFromCR(void) const;

  uint32_t getMsiClockFreqencyIndexFromCSR(void) const;

  uint32_t getPLLInputClockFrequency(void) const;

  static const EnablePeripheralClockRegisterMapping s_enablePeripheralClockMapping[];

  static const uint32_t s_msiClockFreq[];

  //! Pointer to RCC peripheral
  RCC_TypeDef *m_RCCPeripheralPtr;

};

#endif // #ifndef RCC_H