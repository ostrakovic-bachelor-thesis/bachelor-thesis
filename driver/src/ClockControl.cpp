#include "ClockControl.h"
#include "MemoryAccess.h"
#include "RegisterUtility.h"


const uint32_t ClockControl::s_msiClockFreq[] =
{
  [0b0000] =   100000u, // 100 kHz
  [0b0001] =   200000u, // 200 kHz
  [0b0010] =   400000u, // 400 kHz
  [0b0011] =   800000u, // 800 kHz
  [0b0100] =  1000000u, // 1 MHz
  [0b0101] =  2000000u, // 2 MHz
  [0b0110] =  4000000u, // 4 MHz
  [0b0111] =  8000000u, // 8 MHz
  [0b1000] = 16000000u, // 16 MHz
  [0b1001] = 24000000u, // 24 MHz
  [0b1010] = 32000000u, // 32 MHz
  [0b1011] = 48000000u  // 48 MHz
};

const uint32_t ClockControl::s_AHBPrescaler[] =
{
  [0b0000] = 1u,
  [0b0001] = 1u,
  [0b0010] = 1u,
  [0b0011] = 1u,
  [0b0100] = 1u,
  [0b0101] = 1u,
  [0b0110] = 1u,
  [0b0111] = 1u,
  [0b1000] = 2u,
  [0b1001] = 4u,
  [0b1010] = 8u,
  [0b1011] = 16u,
  [0b1100] = 32u,
  [0b1101] = 64u,
  [0b1110] = 128u,
  [0b1111] = 256u
};

const uint32_t ClockControl::s_APBxPrescaler[] =
{
  [0b000] = 1u,
  [0b001] = 1u,
  [0b010] = 1u,
  [0b011] = 1u,
  [0b100] = 2u,
  [0b101] = 4u,
  [0b110] = 8u,
  [0b111] = 16u
};

const ClockControl::GetClockFrequencyFunction_t ClockControl::s_i2cClockFrequency[] =
{
  [0b00] = &ClockControl::getAPB1ClockFrequency,
  [0b01] = &ClockControl::getSystemClockFrequency,
  [0b10] = &ClockControl::getHSIClockFrequency,
};

const ClockControl::GetClockFrequencyFunction_t ClockControl::s_usart1ClockFrequency[] =
{
  [0b00] = &ClockControl::getAPB2ClockFrequency,
  [0b01] = &ClockControl::getSystemClockFrequency,
  [0b10] = &ClockControl::getHSIClockFrequency,
  [0b11] = &ClockControl::getLSEClockFrequency
};

const ClockControl::GetClockFrequencyFunction_t ClockControl::s_usartClockFrequency[] =
{
  [0b00] = &ClockControl::getAPB1ClockFrequency,
  [0b01] = &ClockControl::getSystemClockFrequency,
  [0b10] = &ClockControl::getHSIClockFrequency,
  [0b11] = &ClockControl::getLSEClockFrequency
};

const ClockControl::GetClockFrequencyFunction_t ClockControl::s_PLLInputClockFrequency[] =
{
  [0b00] = &ClockControl::getNoClockFrequency,
  [0b01] = &ClockControl::getMSIClockFrequency,
  [0b10] = &ClockControl::getHSIClockFrequency,
  [0b11] = &ClockControl::getHSEClockFrequency
};

const ClockControl::GetClockFrequencyFunction_t ClockControl::s_systemClockFrequency[] =
{
  [0b00] = &ClockControl::getMSIClockFrequency,
  [0b01] = &ClockControl::getHSIClockFrequency,
  [0b10] = &ClockControl::getHSEClockFrequency,
  [0b11] = &ClockControl::getPLLClockFrequency
};

const ClockControl::GetClockFrequencyFunction_t ClockControl::s_clockFrequency[] =
{
  [static_cast<uint8_t>(Clock::LSE)]          = &ClockControl::getLSEClockFrequency,
  [static_cast<uint8_t>(Clock::MSI)]          = &ClockControl::getMSIClockFrequency,
  [static_cast<uint8_t>(Clock::HSI)]          = &ClockControl::getHSIClockFrequency,
  [static_cast<uint8_t>(Clock::HSE)]          = &ClockControl::getHSEClockFrequency,
  [static_cast<uint8_t>(Clock::PLL)]          = &ClockControl::getPLLClockFrequency,
  [static_cast<uint8_t>(Clock::PLLSAI2)]      = &ClockControl::getNoClockFrequency,
  [static_cast<uint8_t>(Clock::SYSTEM_CLOCK)] = &ClockControl::getSystemClockFrequency,
  [static_cast<uint8_t>(Clock::AHB)]          = &ClockControl::getAHBClockFrequency,
  [static_cast<uint8_t>(Clock::APB1)]         = &ClockControl::getAPB1ClockFrequency,
  [static_cast<uint8_t>(Clock::APB2)]         = &ClockControl::getAPB2ClockFrequency,
  [static_cast<uint8_t>(Clock::NO_CLOCK)]     = &ClockControl::getNoClockFrequency
};


ClockControl::ClockControl(RCC_TypeDef *RCCPeripheralPtr):
  m_RCCPeripheralPtr(RCCPeripheralPtr)
{}

ClockControl::ErrorCode ClockControl::setClockSource(Clock clock, Clock clockSource)
{
  // TODO ErrorCode errorCode = (this->*s_setClockSource[static_cast<uint8_t>(clock)])(clockSource);
  ErrorCode errorCode = setPLLClockSource(clockSource);

  return errorCode;
}

ClockControl::ErrorCode ClockControl::configurePLL(Clock pllClock, const PLLConfiguration &pllConfig)
{
  ErrorCode errorCode = configurePLLSAI2(pllConfig);

  return errorCode;
}

ClockControl::ErrorCode
ClockControl::getClockFrequency(Clock clock, uint32_t &clockFrequency) const
{
  clockFrequency = (this->*(s_clockFrequency[static_cast<uint8_t>(clock)]))();
  return ErrorCode::OK;
}

ClockControl::ErrorCode
ClockControl::getClockFrequency(Peripheral peripheral, uint32_t &clockFrequency) const
{
  ErrorCode errorCode = ErrorCode::OK;

  switch (peripheral)
  {
    case Peripheral::USART1:
    {
      clockFrequency = getUSART1ClockFrequency();
    }
    break;

    case Peripheral::USART2:
    {
      clockFrequency = getUSART2ClockFrequency();
    }
    break;

    case Peripheral::I2C1:
    {
      clockFrequency = getI2C1ClockFrequency();
    }
    break;

    default:
    {
      errorCode = ErrorCode::NOT_IMPLEMENTED_FOR_PERIPHERAL;
    }
    break;
  }

  return errorCode;
}

uint32_t ClockControl::getNoClockFrequency(void) const
{
  return 0u; // 0 Hz
}

uint32_t ClockControl::getHSIClockFrequency(void) const
{
  return 16000000u; // 16 MHz
}

uint32_t ClockControl::getHSEClockFrequency(void) const
{
  return 16000000u; // 16 MHz
}

uint32_t ClockControl::getLSEClockFrequency(void) const
{
  return 32768u; // 32.768 kHz
}

uint32_t ClockControl::getMSIClockFrequency(void) const
{
  uint32_t msiClockFreqIndex;

  if (isMSIClockFrequencyDefinedInCR())
  {
    msiClockFreqIndex = getMsiClockFreqencyIndexFromCR();
  }
  else
  {
    msiClockFreqIndex = getMsiClockFreqencyIndexFromCSR();
  }

  return s_msiClockFreq[msiClockFreqIndex];
}

bool ClockControl::isMSIClockFrequencyDefinedInCR(void) const
{
  constexpr uint8_t MSIRGSEL_POSITION = 3u;
  constexpr uint8_t MSIRGSEL_NUM_OF_BITS = 1u;

  uint32_t isClockFrequencyDefinedInCR = RegisterUtility<uint32_t>::getBitsInRegister(
      &(m_RCCPeripheralPtr->CR),
      MSIRGSEL_POSITION,
      MSIRGSEL_NUM_OF_BITS);

  return (0u != isClockFrequencyDefinedInCR);
}

uint32_t ClockControl::getMsiClockFreqencyIndexFromCR(void) const
{
  constexpr uint8_t MSIRANGE_POSITION = 4u;
  constexpr uint8_t MSIRANGE_NUM_OF_BITS = 4u;

  uint32_t msiClockFreqIndex = RegisterUtility<uint32_t>::getBitsInRegister(
    &(m_RCCPeripheralPtr->CR),
    MSIRANGE_POSITION,
    MSIRANGE_NUM_OF_BITS);

  return msiClockFreqIndex;
}

uint32_t ClockControl::getMsiClockFreqencyIndexFromCSR(void) const
{
  constexpr uint8_t MSISRANGE_POSITION = 8u;
  constexpr uint8_t MSISRANGE_NUM_OF_BITS = 4u;

  uint32_t msiClockFreqIndex = RegisterUtility<uint32_t>::getBitsInRegister(
    &(m_RCCPeripheralPtr->CSR),
    MSISRANGE_POSITION,
    MSISRANGE_NUM_OF_BITS);

  return msiClockFreqIndex;
}

uint32_t ClockControl::getPLLInputClockFrequency(void) const
{
  constexpr uint8_t PLLSRC_POSITION = 0u;
  constexpr uint8_t PLLSRC_NUM_OF_BITS = 2u;

  uint32_t PLLClockSourceIndex = RegisterUtility<uint32_t>::getBitsInRegister(
    &(m_RCCPeripheralPtr->PLLCFGR),
    PLLSRC_POSITION,
    PLLSRC_NUM_OF_BITS);

  return (this->*(s_PLLInputClockFrequency[PLLClockSourceIndex]))();
}

uint32_t ClockControl::getPLLClockFrequency(void) const
{
  constexpr uint32_t PLLN_POSITION = 8u;
  constexpr uint32_t PLLN_NUM_OF_BITS = 6u;
  constexpr uint32_t PLLM_POSITION = 4u;
  constexpr uint32_t PLLM_NUM_OF_BITS = 4u;
  constexpr uint32_t PLLR_POSITION = 25u;
  constexpr uint32_t PLLR_NUM_OF_BITS = 2u;
  const uint32_t PLLCFGR = MemoryAccess::getRegisterValue(&(m_RCCPeripheralPtr->PLLCFGR));
  const uint32_t PLLN = MemoryUtility<uint32_t>::getBits(PLLCFGR, PLLN_POSITION, PLLN_NUM_OF_BITS);
  const uint32_t PLLM = MemoryUtility<uint32_t>::getBits(PLLCFGR, PLLM_POSITION, PLLM_NUM_OF_BITS) + 1u;
  const uint32_t PLLR = (MemoryUtility<uint32_t>::getBits(PLLCFGR, PLLR_POSITION, PLLR_NUM_OF_BITS) + 1u) << 1u;

  const uint32_t PLLVco = (getPLLInputClockFrequency() / PLLM) * PLLN;

  return PLLVco / PLLR;
}

uint32_t ClockControl::getSystemClockFrequency(void) const
{
  constexpr uint8_t SYS_CLOCK_SWITCH_STATUS_POSITION = 2u;
  constexpr uint8_t SYS_CLOCK_SWITCH_STATUS_NUM_OF_BITS = 2u;

  uint32_t systemClockSourceIndex = RegisterUtility<uint32_t>::getBitsInRegister(
    &(m_RCCPeripheralPtr->CFGR),
    SYS_CLOCK_SWITCH_STATUS_POSITION,
    SYS_CLOCK_SWITCH_STATUS_NUM_OF_BITS);

  return (this->*(s_systemClockFrequency[systemClockSourceIndex]))();
}

uint32_t ClockControl::getAHBClockFrequency(void) const
{
  constexpr uint8_t HPRE_POSITION = 4u;
  constexpr uint8_t HPRE_NUM_OF_BITS = 4u;

  const uint32_t AHBPrescalerIndex = RegisterUtility<uint32_t>::getBitsInRegister(
    &(m_RCCPeripheralPtr->CFGR),
    HPRE_POSITION,
    HPRE_NUM_OF_BITS);

  return getSystemClockFrequency() / s_AHBPrescaler[AHBPrescalerIndex];
}

uint32_t ClockControl::getAPB1ClockFrequency(void) const
{
  constexpr uint8_t PPRE1_POSITION = 8u;
  constexpr uint8_t PPRE1_NUM_OF_BITS = 3u;

  const uint32_t APB1PrescalerIndex = RegisterUtility<uint32_t>::getBitsInRegister(
    &(m_RCCPeripheralPtr->CFGR),
    PPRE1_POSITION,
    PPRE1_NUM_OF_BITS);

  return getAHBClockFrequency() / s_APBxPrescaler[APB1PrescalerIndex];
}

uint32_t ClockControl::getAPB2ClockFrequency(void) const
{
  constexpr uint8_t PPRE2_POSITION = 11u;
  constexpr uint8_t PPRE2_NUM_OF_BITS = 3u;

  const uint32_t APB2PrescalerIndex = RegisterUtility<uint32_t>::getBitsInRegister(
    &(m_RCCPeripheralPtr->CFGR),
    PPRE2_POSITION,
    PPRE2_NUM_OF_BITS);

  return getAHBClockFrequency() / s_APBxPrescaler[APB2PrescalerIndex];
}

uint32_t ClockControl::getUSART1ClockFrequency(void) const
{
  constexpr uint8_t USART1SEL_POSITION = 0u;
  constexpr uint8_t USART1SEL_NUM_OF_BITS = 2u;

  const uint32_t usart1ClockSourceIndex = RegisterUtility<uint32_t>::getBitsInRegister(
    &(m_RCCPeripheralPtr->CCIPR),
    USART1SEL_POSITION,
    USART1SEL_NUM_OF_BITS);

  return (this->*(s_usart1ClockFrequency[usart1ClockSourceIndex]))();
}

uint32_t ClockControl::getUSART2ClockFrequency(void) const
{
  constexpr uint8_t USART2SEL_POSITION = 2u;
  constexpr uint8_t USART2SEL_NUM_OF_BITS = 2u;

  const uint32_t usart2ClockSourceIndex = RegisterUtility<uint32_t>::getBitsInRegister(
    &(m_RCCPeripheralPtr->CCIPR),
    USART2SEL_POSITION,
    USART2SEL_NUM_OF_BITS);

  return (this->*(s_usartClockFrequency[usart2ClockSourceIndex]))();
}

uint32_t ClockControl::getI2C1ClockFrequency(void) const
{
  constexpr uint8_t I2C1SEL_POSITION = 12u;
  constexpr uint8_t I2C1SEL_NUM_OF_BITS = 2u;

  const uint32_t i2c1ClockSourceIndex = RegisterUtility<uint32_t>::getBitsInRegister(
    &(m_RCCPeripheralPtr->CCIPR),
    I2C1SEL_POSITION,
    I2C1SEL_NUM_OF_BITS);

  return (this->*(s_i2cClockFrequency[i2c1ClockSourceIndex]))();
}

ClockControl::ErrorCode ClockControl::configurePLLSAI2(const PLLConfiguration &pllConfig)
{
  setPLLInputClockDivider(&(m_RCCPeripheralPtr->PLLSAI2CFGR), pllConfig.inputClockDivider);

  return ErrorCode::OK;
}

ClockControl::ErrorCode ClockControl::setPLLClockSource(Clock clockSource)
{
  constexpr uint32_t RCC_PLLCFGR_PLLSRC_POSITION = 0u;
  constexpr uint32_t RCC_PLLCFGR_PLLSRC_SIZE     = 2u;
  uint8_t pllClockSource;

  ErrorCode errorCode = mapToPLLClockSource(clockSource, pllClockSource);
  if (ErrorCode::OK == errorCode)
  {
    RegisterUtility<uint32_t>::setBitsInRegister(
      &(m_RCCPeripheralPtr->PLLCFGR),
      RCC_PLLCFGR_PLLSRC_POSITION,
      RCC_PLLCFGR_PLLSRC_SIZE,
      static_cast<uint32_t>(pllClockSource));
  }

  return errorCode;
}

ClockControl::ErrorCode ClockControl::mapToPLLClockSource(Clock clockSource, uint8_t &pllClockSource)
{
  ErrorCode errorCode = ErrorCode::OK;

  switch (clockSource)
  {
    case Clock::NO_CLOCK:
      pllClockSource = 0b00;
      break;

    case Clock::MSI:
      pllClockSource = 0b01;
      break;

    case Clock::HSI:
      pllClockSource = 0b10;
      break;

    case Clock::HSE:
      pllClockSource = 0b11;
      break;

    default:
      errorCode = ErrorCode::INVALID_CLOCK_SOURCE;
      break;
  }

  return errorCode;
}

void ClockControl::setPLLInputClockDivider(volatile uint32_t *pllConfigRegisterPtr, uint8_t inputClockDivider)
{
  constexpr uint32_t RCC_PLLXCFGR_PLLM_POSITION = 4u;
  constexpr uint32_t RCC_PLLXCFGR_PLLM_SIZE     = 4u;

  RegisterUtility<uint32_t>::setBitsInRegister(
    pllConfigRegisterPtr,
    RCC_PLLXCFGR_PLLM_POSITION,
    RCC_PLLXCFGR_PLLM_SIZE,
    static_cast<uint32_t>(inputClockDivider - 1u));
}