#include "RCC.h"
#include "MemoryAccess.h"
#include "RegisterUtility.h"


const RCC::EnablePeripheralClockRegisterMapping RCC::s_enablePeripheralClockMapping[] =
{
  { .peripheral = Peripheral::GPIOA, .enableRegister = Register::AHB2ENR, .enableBitPosition = 0u },
  { .peripheral = Peripheral::GPIOB, .enableRegister = Register::AHB2ENR, .enableBitPosition = 1u },
  { .peripheral = Peripheral::GPIOC, .enableRegister = Register::AHB2ENR, .enableBitPosition = 2u },
  { .peripheral = Peripheral::GPIOD, .enableRegister = Register::AHB2ENR, .enableBitPosition = 3u },
  { .peripheral = Peripheral::GPIOE, .enableRegister = Register::AHB2ENR, .enableBitPosition = 4u },
  { .peripheral = Peripheral::GPIOF, .enableRegister = Register::AHB2ENR, .enableBitPosition = 5u },
  { .peripheral = Peripheral::GPIOG, .enableRegister = Register::AHB2ENR, .enableBitPosition = 6u },
  { .peripheral = Peripheral::GPIOH, .enableRegister = Register::AHB2ENR, .enableBitPosition = 7u },
  { .peripheral = Peripheral::GPIOI, .enableRegister = Register::AHB2ENR, .enableBitPosition = 8u },
};

const uint32_t RCC::s_msiClockFreq[] =
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


RCC::RCC(RCC_TypeDef *RCCPeripheralPtr):
  m_RCCPeripheralPtr(RCCPeripheralPtr)
{}

RCC::ErrorCode RCC::enablePeripheralClock(Peripheral peripheral)
{
  auto mappingPtr = findEnableClockRegisterMapping(peripheral);
  if (nullptr == mappingPtr)
  {
    return ErrorCode::NOT_IMPLEMENTED;
  }

  bool isBitSet = setBitInRegister(mappingPtr->enableRegister, mappingPtr->enableBitPosition);
  if (not isBitSet)
  {
    return ErrorCode::INTERNAL;
  }

  return ErrorCode::OK;
}

RCC::ErrorCode RCC::disablePeripheralClock(Peripheral peripheral)
{
  auto mappingPtr = findEnableClockRegisterMapping(peripheral);
  if (nullptr == mappingPtr)
  {
    return ErrorCode::NOT_IMPLEMENTED;
  }
  
  bool isBitReset = resetBitInRegister(mappingPtr->enableRegister, mappingPtr->enableBitPosition);
  if (not isBitReset)
  {
    return ErrorCode::INTERNAL;
  }

  return ErrorCode::OK;
}

RCC::ErrorCode RCC::isPeripheralClockEnabled(Peripheral peripheral, bool &isClockEnabled) const
{
  auto mappingPtr = findEnableClockRegisterMapping(peripheral);
  if (nullptr == mappingPtr)
  {
    return ErrorCode::NOT_IMPLEMENTED;
  }
  
  bool isBitChecked = isBitSetInRegister(mappingPtr->enableRegister, mappingPtr->enableBitPosition, isClockEnabled);
  if (not isBitChecked)
  {
    return ErrorCode::INTERNAL;
  }

  return ErrorCode::OK;
}

uint32_t RCC::getHSIClockFrequency(void) const
{  
  return 16000000u; // 16 MHz
}

uint32_t RCC::getHSEClockFrequency(void) const
{
  return 16000000u; // 16 MHz
}

uint32_t RCC::getMSIClockFrequency(void) const
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

bool RCC::setBitInRegister(Register rccRegister, uint8_t bitInRegister)
{
  bool isBitSet = false;

  volatile uint32_t *registerPtr = getPointerToRegister(rccRegister);
  if (nullptr != registerPtr)
  {
    RegisterUtility<uint32_t>::setBitInRegister(registerPtr, bitInRegister);
    isBitSet = true;
  }

  return isBitSet;
}

bool RCC::resetBitInRegister(Register rccRegister, uint8_t bitInRegister)
{
  bool isBitSet = false;

  volatile uint32_t *registerPtr = getPointerToRegister(rccRegister);
  if (nullptr != registerPtr)
  {
    RegisterUtility<uint32_t>::resetBitInRegister(registerPtr, bitInRegister);
    isBitSet = true;
  }

  return isBitSet;
}

bool RCC::isBitSetInRegister(Register rccRegister, uint8_t bitInRegister, bool &isBitSet) const
{
  bool isBitChecked = false;

  volatile uint32_t *registerPtr = getPointerToRegister(rccRegister);
  if (nullptr != registerPtr)
  {
    isBitSet = RegisterUtility<uint32_t>::isBitSetInRegister(registerPtr, bitInRegister);
    isBitChecked = true;
  }

  return isBitChecked;
}

const RCC::EnablePeripheralClockRegisterMapping* RCC::findEnableClockRegisterMapping(Peripheral peripheral) const
{
  for (auto i = 0u; i < sizeof(s_enablePeripheralClockMapping) / sizeof(EnablePeripheralClockRegisterMapping); ++i)
  {
    if (s_enablePeripheralClockMapping[i].peripheral == peripheral)
    {
      return &s_enablePeripheralClockMapping[i];
    }
  }

  return nullptr;
}

volatile uint32_t* RCC::getPointerToRegister(Register rccRegister) const
{
  volatile uint32_t *registerPtr;

  switch (rccRegister)
  {
    case Register::AHB1ENR:
    {
      registerPtr = &(m_RCCPeripheralPtr->AHB1ENR);
    }
    break;

    case Register::AHB2ENR:
    {
      registerPtr = &(m_RCCPeripheralPtr->AHB2ENR);
    }
    break;

    case Register::AHB3ENR:
    {
      registerPtr = &(m_RCCPeripheralPtr->AHB3ENR);
    }
    break;

    case Register::APB1ENR1:
    {
      registerPtr = &(m_RCCPeripheralPtr->APB1ENR1);
    }
    break;

    case Register::APB1ENR2:
    {
      registerPtr = &(m_RCCPeripheralPtr->APB1ENR2);
    }
    break;

    case Register::APB2ENR:
    {
      registerPtr = &(m_RCCPeripheralPtr->APB2ENR);
    }
    break;

    default:
    {
      registerPtr = nullptr;
    }
    break;
  }

  return registerPtr;
}

RCC::ClockSource RCC::getSystemClockSource(void) const
{
  constexpr uint8_t SYS_CLOCK_SWITCH_STATUS_POSITION = 2u;
  constexpr uint8_t SYS_CLOCK_SWITCH_STATUS_NUM_OF_BITS = 2u;

  uint32_t systemClockSource = RegisterUtility<uint32_t>::getBitsInRegister(
    &(m_RCCPeripheralPtr->CFGR), 
    SYS_CLOCK_SWITCH_STATUS_POSITION, 
    SYS_CLOCK_SWITCH_STATUS_NUM_OF_BITS);

  return static_cast<ClockSource>(systemClockSource);
}

bool RCC::isMSIClockFrequencyDefinedInCR(void) const
{
  constexpr uint8_t MSIRGSEL_POSITION = 3u;
  constexpr uint8_t MSIRGSEL_NUM_OF_BITS = 1u;

  uint32_t isClockFrequencyDefinedInCR = RegisterUtility<uint32_t>::getBitsInRegister(
      &(m_RCCPeripheralPtr->CR), 
      MSIRGSEL_POSITION, 
      MSIRGSEL_NUM_OF_BITS);

  return (0u != isClockFrequencyDefinedInCR);
}

uint32_t RCC::getMsiClockFreqencyIndexFromCR(void) const
{
  constexpr uint8_t MSIRANGE_POSITION = 4u;
  constexpr uint8_t MSIRANGE_NUM_OF_BITS = 4u;

  uint32_t msiClockFreqIndex = RegisterUtility<uint32_t>::getBitsInRegister(
    &(m_RCCPeripheralPtr->CR), 
    MSIRANGE_POSITION, 
    MSIRANGE_NUM_OF_BITS);

  return msiClockFreqIndex;
}

uint32_t RCC::getMsiClockFreqencyIndexFromCSR(void) const
{
  constexpr uint8_t MSISRANGE_POSITION = 8u;
  constexpr uint8_t MSISRANGE_NUM_OF_BITS = 4u;

  uint32_t msiClockFreqIndex = RegisterUtility<uint32_t>::getBitsInRegister(
    &(m_RCCPeripheralPtr->CSR), 
    MSISRANGE_POSITION, 
    MSISRANGE_NUM_OF_BITS);

  return msiClockFreqIndex;
}

uint32_t RCC::getPLLInputClockFrequency(void) const
{
  constexpr uint8_t PLLSRC_POSITION = 0u;
  constexpr uint8_t PLLSRC_NUM_OF_BITS = 2u;

  uint32_t PLLClockSource = RegisterUtility<uint32_t>::getBitsInRegister(
    &(m_RCCPeripheralPtr->PLLCFGR), 
    PLLSRC_POSITION, 
    PLLSRC_NUM_OF_BITS);

  uint32_t PLLInputClockFrequency;

  switch (PLLClockSource)
  {
    // no clock
    case 0b00:
    {
      PLLInputClockFrequency = 0u;
    }
    break;
    
    // MSI clock
    case 0b01:
    {
      PLLInputClockFrequency = getMSIClockFrequency();
    }
    break;
    
    // HSI clock
    case 0b10:
    {
      PLLInputClockFrequency = getHSIClockFrequency();
    }
    break;
   
    // HSE clock
    case 0b11:
    {
      PLLInputClockFrequency = getHSEClockFrequency();
    }
    break;
   
    // invalid case
    default:
    {
      PLLInputClockFrequency = 0u;
    }
  }

  return PLLInputClockFrequency;
}

uint32_t RCC::getPLLClockFrequency(void) const
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

uint32_t RCC::getSystemClockFrequency(void) const
{
  uint32_t systemClockFrequency;

  switch (getSystemClockSource())
  {
    case ClockSource::HSE:
    {
      systemClockFrequency = getHSEClockFrequency();
    }
    break;

    case ClockSource::HSI:
    {
      systemClockFrequency = getHSIClockFrequency();
    }
    break;

    case ClockSource::MSI:
    {
      systemClockFrequency = getMSIClockFrequency();
    }
    break;

    case ClockSource::PLL:
    {
      systemClockFrequency = getPLLClockFrequency();
    }
    break;

    default:
    {
      systemClockFrequency = 0u;
    }
    break;
  };
  
  return systemClockFrequency;
}