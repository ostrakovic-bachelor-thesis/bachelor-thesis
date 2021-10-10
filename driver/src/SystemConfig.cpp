#include "SystemConfig.h"
#include "RegisterUtility.h"


SystemConfig::SystemConfig(SYSCFG_TypeDef *sysCfgPeripheralPtr, ResetControl *resetControlPtr):
  m_sysCfgPeripheralPtr(sysCfgPeripheralPtr),
  m_resetControlPtr(resetControlPtr)
{}

SystemConfig::ErrorCode SystemConfig::init(void)
{
  ErrorCode errorCode = enablePeripheralClock();

  return errorCode;
}

SystemConfig::ErrorCode SystemConfig::mapGPIOToEXTILine(EXTILine extiLine, GPIOPort gpioPort)
{
  if (not isGPIOPortValid(gpioPort, extiLine))
  {
    return SystemConfig::ErrorCode::INVALID_GPIO_PORT;
  }

  setGPIOToEXTILineMapping(extiLine, gpioPort);

  return ErrorCode::OK;
}

inline SystemConfig::ErrorCode SystemConfig::enablePeripheralClock(void)
{
  ResetControl::ErrorCode errorCode = m_resetControlPtr->enablePeripheralClock(getPeripheralTag());

  return (ResetControl::ErrorCode::OK == errorCode) ? ErrorCode::OK : ErrorCode::CAN_NOT_TURN_ON_PERIPHERAL_CLOCK;
}

void SystemConfig::setGPIOToEXTILineMapping(EXTILine extiLine, GPIOPort gpioPort)
{
  constexpr uint32_t SYSCFG_EXTIX_CONFIG_PER_EXTICR_REG = 4u;
  constexpr uint32_t SYSCFG_EXTICR_EXTIX_SIZE = 4u;
  const uint32_t registerIndex = static_cast<uint32_t>(extiLine) /  SYSCFG_EXTIX_CONFIG_PER_EXTICR_REG;
  const uint32_t position = (static_cast<uint32_t>(extiLine) % SYSCFG_EXTICR_EXTIX_SIZE) * SYSCFG_EXTICR_EXTIX_SIZE;

  RegisterUtility<uint32_t>::setBitsInRegister(
    &(m_sysCfgPeripheralPtr->EXTICR[registerIndex]),
    position,
    SYSCFG_EXTICR_EXTIX_SIZE,
    static_cast<uint32_t>(gpioPort));
}

bool SystemConfig::isGPIOPortValid(GPIOPort gpioPort, EXTILine extiLine)
{
  return not ((static_cast<uint8_t>(EXTILine::EXTI12) <= static_cast<uint8_t>(extiLine)) &&
              (static_cast<uint8_t>(GPIOPort::GPIOI)  == static_cast<uint8_t>(gpioPort)));
}