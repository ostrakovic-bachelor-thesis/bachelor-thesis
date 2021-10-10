#include "SystemConfig.h"
#include "RegisterUtility.h"


SystemConfig::SystemConfig(SYSCFG_TypeDef *sysCfgPeripheralPtr):
  m_sysCfgPeripheralPtr(sysCfgPeripheralPtr)
{}

SystemConfig::ErrorCode SystemConfig::mapGPIOToEXTILine(EXTILine extiLine, GPIOPort gpioPort)
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

  return ErrorCode::OK;
}
