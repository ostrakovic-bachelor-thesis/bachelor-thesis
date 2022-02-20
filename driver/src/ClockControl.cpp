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

ClockControl::ErrorCode ClockControl::enableClock(Clock clock)
{
  ErrorCode errorCode = ErrorCode::OK;

  if (Clock::HSE == clock)
  {
    enableHSEClock();
    while (not isHSEClockReady());
  }
  else
  {
    errorCode = ErrorCode::CLOCK_CAN_NOT_BE_DIRECTLY_ENABLED;;
  }

  return errorCode;
}

ClockControl::ErrorCode ClockControl::setClockSource(Clock clock, Clock clockSource)
{
  ErrorCode errorCode = ErrorCode::OK;

  // TODO ErrorCode errorCode = (this->*s_setClockSource[static_cast<uint8_t>(clock)])(clockSource);
  if (Clock::PLL == clock)
  {
    errorCode = setPLLClockSource(clockSource);
  }
  else if (Clock::SYSTEM_CLOCK == clock)
  {
    errorCode = setSystemClockSource(clockSource);
  }

  return errorCode;
}


ClockControl::ErrorCode ClockControl::configurePLL(const PLLConfiguration &pllConfig)
{
  turnOffPLL();

  uint32_t registerValuePLLCFGR = MemoryAccess::getRegisterValue(&(m_RCCPeripheralPtr->PLLCFGR));

  setPLLInputClockDivider(registerValuePLLCFGR, pllConfig.inputClockDivider);
  setPLLInputClockMultiplier(registerValuePLLCFGR, pllConfig.inputClockMultiplier);
  setPLLOutputClockPDivider(registerValuePLLCFGR, pllConfig.outputClockPDivider);
  setPLLOutputClockQDivider(registerValuePLLCFGR, pllConfig.outputClockQDivider);
  setPLLOutputClockRDivider(registerValuePLLCFGR, pllConfig.outputClockRDivider);

  if (pllConfig.enableOutputClockP)
  {
    enableOutputClockP(registerValuePLLCFGR);
  }
  else
  {
    disableOutputClockP(registerValuePLLCFGR);
  }

  if (pllConfig.enableOutputClockQ)
  {
    enableOutputClockQ(registerValuePLLCFGR);
  }
  else
  {
    disableOutputClockQ(registerValuePLLCFGR);
  }

  if (pllConfig.enableOutputClockR)
  {
    enableOutputClockR(registerValuePLLCFGR);
  }
  else
  {
    disableOutputClockR(registerValuePLLCFGR);
  }

  MemoryAccess::setRegisterValue(&(m_RCCPeripheralPtr->PLLCFGR), registerValuePLLCFGR);

  turnOnPLL();

  return ErrorCode::OK;
}

ClockControl::ErrorCode ClockControl::configurePLL(const PLLSAI2Configuration &pllSAI2Config)
{
  turnOffPLLSAI2();

  uint32_t registerValuePLLSAI2CFGR = MemoryAccess::getRegisterValue(&(m_RCCPeripheralPtr->PLLSAI2CFGR));

  setPLLInputClockDivider(registerValuePLLSAI2CFGR, pllSAI2Config.inputClockDivider);
  setPLLInputClockMultiplier(registerValuePLLSAI2CFGR, pllSAI2Config.inputClockMultiplier);
  setPLLOutputClockPDivider(registerValuePLLSAI2CFGR, pllSAI2Config.outputClockPDivider);
  setPLLOutputClockQDivider(registerValuePLLSAI2CFGR, pllSAI2Config.outputClockQDivider);
  setPLLOutputClockRDivider(registerValuePLLSAI2CFGR, pllSAI2Config.outputClockRDivider);

  if (pllSAI2Config.enableOutputClockP)
  {
    enableOutputClockP(registerValuePLLSAI2CFGR);
  }
  else
  {
    disableOutputClockP(registerValuePLLSAI2CFGR);
  }

  if (pllSAI2Config.enableOutputClockQ)
  {
    enableOutputClockQ(registerValuePLLSAI2CFGR);
  }
  else
  {
    disableOutputClockQ(registerValuePLLSAI2CFGR);
  }

  if (pllSAI2Config.enableOutputClockR)
  {
    enableOutputClockR(registerValuePLLSAI2CFGR);
  }
  else
  {
    disableOutputClockR(registerValuePLLSAI2CFGR);
  }

  MemoryAccess::setRegisterValue(&(m_RCCPeripheralPtr->PLLSAI2CFGR), registerValuePLLSAI2CFGR);

  setLTDCClockDivider(pllSAI2Config.ltdcClockDivider);

  turnOnPLLSAI2();

  return ErrorCode::OK;
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

ClockControl::ErrorCode ClockControl::setSystemClockSource(Clock clockSource)
{
  ErrorCode errorCode = requestUsingGivenClockAsSystemClockSource(clockSource);

  if (ErrorCode::OK == errorCode)
  {
    while (clockSource != getSystemClockSource());
  }

  return errorCode;
}

ClockControl::ErrorCode ClockControl::requestUsingGivenClockAsSystemClockSource(Clock clockSource)
{
  constexpr uint32_t RCC_CFGR_SW_POSITION = 0u;
  constexpr uint32_t RCC_CFGR_SW_SIZE     = 2u;
  uint8_t systemClockSourceRaw;

  ErrorCode errorCode = mapToSystemClockSourceRaw(clockSource, systemClockSourceRaw);
  if (ErrorCode::OK == errorCode)
  {
    RegisterUtility<uint32_t>::setBitsInRegister(
      &(m_RCCPeripheralPtr->CFGR),
      RCC_CFGR_SW_POSITION,
      RCC_CFGR_SW_SIZE,
      static_cast<uint32_t>(systemClockSourceRaw));
  }

  return errorCode;
}

ClockControl::Clock ClockControl::getSystemClockSource(void) const
{
  constexpr uint32_t RCC_CFGR_SWS_POSITION = 2u;
  constexpr uint32_t RCC_CFGR_SWS_SIZE     = 2u;

  uint32_t systemClockSourceRaw = RegisterUtility<uint32_t>::getBitsInRegister(
      &(m_RCCPeripheralPtr->CFGR),
      RCC_CFGR_SWS_POSITION,
      RCC_CFGR_SWS_SIZE);

  return mapToSystemClockSource(systemClockSourceRaw);
}

ClockControl::ErrorCode ClockControl::mapToPLLClockSource(Clock clockSource, uint8_t &pllClockSource)
{
  ErrorCode errorCode = ErrorCode::OK;

  switch (clockSource)
  {
    case Clock::NO_CLOCK:
    {
      pllClockSource = 0b00;
    }
    break;

    case Clock::MSI:
    {
      pllClockSource = 0b01;
    }
    break;

    case Clock::HSI:
    {
      pllClockSource = 0b10;
    }
    break;

    case Clock::HSE:
    {
      pllClockSource = 0b11;
    }
    break;

    default:
    {
      errorCode = ErrorCode::INVALID_CLOCK_SOURCE;
    }
    break;
  }

  return errorCode;
}

ClockControl::ErrorCode ClockControl::mapToSystemClockSourceRaw(Clock clockSource, uint8_t &systemClockSourceRaw)
{
  ErrorCode errorCode = ErrorCode::OK;

  switch (clockSource)
  {
    case Clock::MSI:
    {
      systemClockSourceRaw = 0b00;
    }
    break;

    case Clock::HSI:
    {
      systemClockSourceRaw = 0b01;
    }
    break;

    case Clock::HSE:
    {
      systemClockSourceRaw = 0b10;
    }
    break;

    case Clock::PLL:
    {
      systemClockSourceRaw = 0b11;
    }
    break;

    default:
    {
      errorCode = ErrorCode::INVALID_CLOCK_SOURCE;
    }
    break;
  }

  return errorCode;
}

ClockControl::Clock ClockControl::mapToSystemClockSource(uint32_t systemClockSourceRaw)
{
  if (0b00 == systemClockSourceRaw)
  {
    return Clock::MSI;
  }
  else if (0b01 == systemClockSourceRaw)
  {
    return Clock::HSI;
  }
  else if (0b10 == systemClockSourceRaw)
  {
    return Clock::HSE;
  }
  else if (0b11 == systemClockSourceRaw)
  {
    return Clock::PLL;
  }
  else
  {
    return Clock::NO_CLOCK;
  }
}

void ClockControl::setPLLInputClockDivider(uint32_t &registerValuePLLXCFGR, uint8_t inputClockDivider)
{
  constexpr uint32_t RCC_PLLXCFGR_PLLM_POSITION = 4u;
  constexpr uint32_t RCC_PLLXCFGR_PLLM_SIZE     = 4u;

  registerValuePLLXCFGR = MemoryUtility<uint32_t>::setBits(
    registerValuePLLXCFGR,
    RCC_PLLXCFGR_PLLM_POSITION,
    RCC_PLLXCFGR_PLLM_SIZE,
    static_cast<uint32_t>(inputClockDivider - 1u));
}

void ClockControl::setPLLInputClockMultiplier(uint32_t &registerValuePLLXCFGR, uint8_t inputClockMultiplier)
{
  constexpr uint32_t RCC_PLLXCFGR_PLLN_POSITION = 8u;
  constexpr uint32_t RCC_PLLXCFGR_PLLN_SIZE     = 7u;

  registerValuePLLXCFGR = MemoryUtility<uint32_t>::setBits(
    registerValuePLLXCFGR,
    RCC_PLLXCFGR_PLLN_POSITION,
    RCC_PLLXCFGR_PLLN_SIZE,
    static_cast<uint32_t>(inputClockMultiplier));
}

void ClockControl::setPLLOutputClockRDivider(uint32_t &registerValuePLLXCFGR, uint8_t outputClockRDivider)
{
  constexpr uint32_t RCC_PLLXCFGR_PLLR_POSITION = 25u;
  constexpr uint32_t RCC_PLLXCFGR_PLLR_SIZE     = 2u;

  registerValuePLLXCFGR = MemoryUtility<uint32_t>::setBits(
    registerValuePLLXCFGR,
    RCC_PLLXCFGR_PLLR_POSITION,
    RCC_PLLXCFGR_PLLR_SIZE,
    static_cast<uint32_t>((outputClockRDivider >> 1u) - 1u));
}

void ClockControl::setPLLOutputClockQDivider(uint32_t &registerValuePLLXCFGR, uint8_t outputClockQDivider)
{
  constexpr uint32_t RCC_PLLXCFGR_PLLQ_POSITION = 21u;
  constexpr uint32_t RCC_PLLXCFGR_PLLQ_SIZE     = 2u;

  registerValuePLLXCFGR = MemoryUtility<uint32_t>::setBits(
    registerValuePLLXCFGR,
    RCC_PLLXCFGR_PLLQ_POSITION,
    RCC_PLLXCFGR_PLLQ_SIZE,
    static_cast<uint32_t>((outputClockQDivider >> 1u) - 1u));
}

void ClockControl::setPLLOutputClockPDivider(uint32_t &registerValuePLLXCFGR, uint8_t outputClockPDivider)
{
  constexpr uint32_t RCC_PLLXCFGR_PLLPDIV_POSITION = 27u;
  constexpr uint32_t RCC_PLLXCFGR_PLLPDIV_SIZE     = 5u;

  registerValuePLLXCFGR = MemoryUtility<uint32_t>::setBits(
    registerValuePLLXCFGR,
    RCC_PLLXCFGR_PLLPDIV_POSITION,
    RCC_PLLXCFGR_PLLPDIV_SIZE,
    static_cast<uint32_t>(outputClockPDivider));
}

void ClockControl::enableOutputClockP(uint32_t &registerValuePLLXCFGR)
{
  constexpr uint32_t RCC_PLLXCFGR_PLLPEN_POSITION = 16u;

  registerValuePLLXCFGR = MemoryUtility<uint32_t>::setBit(
    registerValuePLLXCFGR,
    RCC_PLLXCFGR_PLLPEN_POSITION);
}

void ClockControl::disableOutputClockP(uint32_t &registerValuePLLXCFGR)
{
  constexpr uint32_t RCC_PLLXCFGR_PLLPEN_POSITION = 16u;

  registerValuePLLXCFGR = MemoryUtility<uint32_t>::resetBit(
    registerValuePLLXCFGR,
    RCC_PLLXCFGR_PLLPEN_POSITION);
}

void ClockControl::enableOutputClockQ(uint32_t &registerValuePLLXCFGR)
{
  constexpr uint32_t RCC_PLLXCFGR_PLLQEN_POSITION = 20u;

  registerValuePLLXCFGR = MemoryUtility<uint32_t>::setBit(
    registerValuePLLXCFGR,
    RCC_PLLXCFGR_PLLQEN_POSITION);
}

void ClockControl::disableOutputClockQ(uint32_t &registerValuePLLXCFGR)
{
  constexpr uint32_t RCC_PLLXCFGR_PLLQEN_POSITION = 20u;

  registerValuePLLXCFGR = MemoryUtility<uint32_t>::resetBit(
    registerValuePLLXCFGR,
    RCC_PLLXCFGR_PLLQEN_POSITION);
}

void ClockControl::enableOutputClockR(uint32_t &registerValuePLLXCFGR)
{
  constexpr uint32_t RCC_PLLXCFGR_PLLREN_POSITION = 24u;

  registerValuePLLXCFGR = MemoryUtility<uint32_t>::setBit(
    registerValuePLLXCFGR,
    RCC_PLLXCFGR_PLLREN_POSITION);
}

void ClockControl::disableOutputClockR(uint32_t &registerValuePLLXCFGR)
{
  constexpr uint32_t RCC_PLLXCFGR_PLLREN_POSITION = 24u;

  registerValuePLLXCFGR = MemoryUtility<uint32_t>::resetBit(
    registerValuePLLXCFGR,
    RCC_PLLXCFGR_PLLREN_POSITION);
}

void ClockControl::setLTDCClockDivider(uint8_t ltdcClockDivider)
{
  constexpr uint32_t RCC_CCIPR2_PLLSAI2DIVR_POSITION = 16u;
  constexpr uint32_t RCC_CCIPR2_PLLSAI2DIVR_SIZE     = 2u;

  RegisterUtility<uint32_t>::setBitsInRegister(
    &(m_RCCPeripheralPtr->CCIPR2),
    RCC_CCIPR2_PLLSAI2DIVR_POSITION,
    RCC_CCIPR2_PLLSAI2DIVR_SIZE,
    static_cast<uint32_t>((ltdcClockDivider + 2u) / 5u));
}

void ClockControl::turnOffPLLSAI2(void)
{
  requestTurningOffPLLSAI2();

  while (isPLLSAI2TurnedOn());
}

void ClockControl::turnOnPLLSAI2(void)
{
  requestTurningOnPLLSAI2();

  while (not isPLLSAI2TurnedOn());
}

inline void ClockControl::requestTurningOffPLLSAI2(void)
{
  constexpr uint32_t RCC_CR_PLLSAI2ON_POSITION = 28u;
  RegisterUtility<uint32_t>::resetBitInRegister(&(m_RCCPeripheralPtr->CR), RCC_CR_PLLSAI2ON_POSITION);
}

inline void ClockControl::requestTurningOnPLLSAI2(void)
{
  constexpr uint32_t RCC_CR_PLLSAI2ON_POSITION = 28u;
  RegisterUtility<uint32_t>::setBitInRegister(&(m_RCCPeripheralPtr->CR), RCC_CR_PLLSAI2ON_POSITION);
}

inline bool ClockControl::isPLLSAI2TurnedOn(void) const
{
  constexpr uint32_t RCC_CR_PLLSAI2RDY_POSITION = 29u;
  return RegisterUtility<uint32_t>::isBitSetInRegister(&(m_RCCPeripheralPtr->CR), RCC_CR_PLLSAI2RDY_POSITION);
}

void ClockControl::enableHSEClock(void)
{
  constexpr uint32_t RCC_CR_HSEON_POSITION = 16u;
  RegisterUtility<uint32_t>::setBitInRegister(&(m_RCCPeripheralPtr->CR), RCC_CR_HSEON_POSITION);
}

bool ClockControl::isHSEClockReady(void) const
{
  constexpr uint32_t RCC_CR_HSERDY_POSITION = 17u;
  return RegisterUtility<uint32_t>::isBitSetInRegister(&(m_RCCPeripheralPtr->CR), RCC_CR_HSERDY_POSITION);
}

inline void ClockControl::requestTurningOffPLL(void)
{
  constexpr uint32_t RCC_CR_PLLON_POSITION = 24u;
  RegisterUtility<uint32_t>::resetBitInRegister(&(m_RCCPeripheralPtr->CR), RCC_CR_PLLON_POSITION);
}

inline void ClockControl::requestTurningOnPLL(void)
{
  constexpr uint32_t RCC_CR_PLLON_POSITION = 24u;
  RegisterUtility<uint32_t>::setBitInRegister(&(m_RCCPeripheralPtr->CR), RCC_CR_PLLON_POSITION);
}

inline bool ClockControl::isPLLTurnedOn(void) const
{
  constexpr uint32_t RCC_CR_PLLRDY_POSITION = 25u;
  return RegisterUtility<uint32_t>::isBitSetInRegister(&(m_RCCPeripheralPtr->CR), RCC_CR_PLLRDY_POSITION);
}

void ClockControl::turnOffPLL(void)
{
  requestTurningOffPLL();
  while (isPLLTurnedOn());
}

void ClockControl::turnOnPLL(void)
{
  requestTurningOnPLL();
  while (not isPLLTurnedOn());
}