#include "RCC.h"
#include "MemoryAccess.h"
#include "MemoryUtility.h"


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

RCC::ErrorCode RCC::isClockEnabled(Peripheral peripheral, bool &isPeripheralEnabled) const
{
  auto mappingPtr = findEnableClockRegisterMapping(peripheral);
  if (nullptr == mappingPtr)
  {
    return ErrorCode::NOT_IMPLEMENTED;
  }
  
  bool isBitChecked = isBitSetInRegister(mappingPtr->enableRegister, mappingPtr->enableBitPosition, isPeripheralEnabled);
  if (not isBitChecked)
  {
    return ErrorCode::INTERNAL;
  }

  return ErrorCode::OK;
}

bool RCC::setBitInRegister(Register rccRegister, uint32_t bitInRegister)
{
  bool isBitSet = false;

  volatile void *registerPtr = getPointerToRegister(rccRegister);
  if (nullptr != registerPtr)
  {
    MemoryUtility::setBitInRegister(registerPtr, bitInRegister);
    isBitSet = true;
  }

  return isBitSet;
}

bool RCC::resetBitInRegister(Register rccRegister, uint32_t bitInRegister)
{
  bool isBitSet = false;

  volatile void *registerPtr = getPointerToRegister(rccRegister);
  if (nullptr != registerPtr)
  {
    MemoryUtility::resetBitInRegister(registerPtr, bitInRegister);
    isBitSet = true;
  }

  return isBitSet;
}

bool RCC::isBitSetInRegister(Register rccRegister, uint32_t bitInRegister, bool &isBitSet) const
{
  bool isBitChecked = false;

  volatile void *registerPtr = getPointerToRegister(rccRegister);
  if (nullptr != registerPtr)
  {
    isBitSet = MemoryUtility::isBitSetInRegister(registerPtr, bitInRegister);
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

volatile void* RCC::getPointerToRegister(Register rccRegister) const
{
  volatile void *registerPtr;

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

