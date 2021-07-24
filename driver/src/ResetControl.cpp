#include "ResetControl.h"
#include "MemoryAccess.h"
#include "RegisterUtility.h"


const ResetControl::EnablePeripheralClockRegisterMapping ResetControl::s_enablePeripheralClockMapping[] =
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

  { .peripheral = Peripheral::USART1,  .enableRegister = Register::APB2ENR,  .enableBitPosition = 14u },
  { .peripheral = Peripheral::USART2,  .enableRegister = Register::APB1ENR1, .enableBitPosition = 17u },
  { .peripheral = Peripheral::USART3,  .enableRegister = Register::APB1ENR1, .enableBitPosition = 18u },
  { .peripheral = Peripheral::UART4,   .enableRegister = Register::APB1ENR1, .enableBitPosition = 19u },
  { .peripheral = Peripheral::UART5,   .enableRegister = Register::APB1ENR1, .enableBitPosition = 20u },
  { .peripheral = Peripheral::LPUART1, .enableRegister = Register::APB1ENR2, .enableBitPosition = 0u },
};


ResetControl::ResetControl(RCC_TypeDef *RCCPeripheralPtr):
  m_RCCPeripheralPtr(RCCPeripheralPtr)
{}

ResetControl::ErrorCode ResetControl::enablePeripheralClock(Peripheral peripheral)
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

ResetControl::ErrorCode ResetControl::disablePeripheralClock(Peripheral peripheral)
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

ResetControl::ErrorCode ResetControl::isPeripheralClockEnabled(Peripheral peripheral, bool &isClockEnabled) const
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

bool ResetControl::setBitInRegister(Register rccRegister, uint8_t bitInRegister)
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

bool ResetControl::resetBitInRegister(Register rccRegister, uint8_t bitInRegister)
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

bool ResetControl::isBitSetInRegister(Register rccRegister, uint8_t bitInRegister, bool &isBitSet) const
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

const ResetControl::EnablePeripheralClockRegisterMapping*
ResetControl::findEnableClockRegisterMapping(Peripheral peripheral) const
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

volatile uint32_t* ResetControl::getPointerToRegister(Register rccRegister) const
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