#include "SysTick.h"
#include "MemoryAccess.h"
#include "RegisterUtility.h"


SysTick::SysTick(SysTick_Type *sysTickPtr, ClockControl *clockControlPtr, InterruptController *InterruptControllerPtr):
  m_sysTickPtr(sysTickPtr),
  m_clockControlPtr(clockControlPtr),
  m_interruptControllerPtr(InterruptControllerPtr),
  m_ticks(0ull)
{}

SysTick::ErrorCode SysTick::init(const SysTickConfig& sysTickConfig)
{
  uint32_t reloadValue = 0u;

  ErrorCode errorCode = ticksPerSecondToReloadValue(sysTickConfig.ticksPerSecond, reloadValue);
  if (ErrorCode::OK == errorCode)
  {
    setReloadValue(reloadValue);
    setCurrentValue(0u);

    uint32_t registerValueCTRL = 0u;

    setEnableSysTickFlag(registerValueCTRL, sysTickConfig.enableOnInit);
    setEnableInterruptFlag(registerValueCTRL, sysTickConfig.enableInterrupt);
    setClockSourceFlag(registerValueCTRL, true);

    MemoryAccess::setRegisterValue(&(m_sysTickPtr->CTRL), registerValueCTRL);
  }

  return errorCode;
}

void SysTick::IRQHandler(void)
{
  ++m_ticks;
}

SysTick::ErrorCode SysTick::ticksPerSecondToReloadValue(uint32_t ticksPerSecond, uint32_t &reloadValue)
{
  uint32_t systemClockFrequency = 0u;

  const auto clockControlErrorCode =  m_clockControlPtr->getClockFrequency(
    ClockControl::ClockSource::SYSTEM_CLOCK,
    systemClockFrequency);
  if (ClockControl::ErrorCode::OK != clockControlErrorCode)
  {
    return ErrorCode::RELOAD_VALUE_CALCULATION_PROBLEM;
  }

  reloadValue = systemClockFrequency / ticksPerSecond;
  if (not isReloadValueInValidRangeOfValue(reloadValue))
  {
    return ErrorCode::RELOAD_VALUE_OUT_OF_RANGE;
  }

  return ErrorCode::OK;
}

inline void SysTick::setReloadValue(uint32_t reloadValue)
{
  constexpr uint8_t RELOAD_VALUE_NUM_OF_BITS = 24u;
  RegisterUtility<uint32_t>::setBitsInRegister(
    &(m_sysTickPtr->LOAD),
    0u,
    RELOAD_VALUE_NUM_OF_BITS,
    reloadValue);
}

inline void SysTick::setCurrentValue(uint32_t currentValue)
{
  MemoryAccess::setRegisterValue(&(m_sysTickPtr->VAL), currentValue);
}

inline void SysTick::setEnableSysTickFlag(uint32_t &registerValueCTRL, bool enableSysTick)
{
  constexpr uint8_t ENABLE_SYSTICK_NUM_OF_BITS = 1u;
  constexpr uint8_t ENABLE_SYSTICK_POSITION = 0u;

  registerValueCTRL = MemoryUtility<uint32_t>::setBits(
    registerValueCTRL,
    ENABLE_SYSTICK_POSITION,
    ENABLE_SYSTICK_NUM_OF_BITS,
    (enableSysTick ? 1u : 0u));
}

inline void SysTick::setEnableInterruptFlag(uint32_t &registerValueCTRL, bool enableInterrupt)
{
  constexpr uint8_t ENABLE_INTERRUPT_NUM_OF_BITS = 1u;
  constexpr uint8_t ENABLE_INTERRUPT_POSITION = 1u;

  registerValueCTRL = MemoryUtility<uint32_t>::setBits(
    registerValueCTRL,
    ENABLE_INTERRUPT_POSITION,
    ENABLE_INTERRUPT_NUM_OF_BITS,
    (enableInterrupt ? 1u : 0u));
}


inline void SysTick::setClockSourceFlag(uint32_t &registerValueCTRL, bool useCPUClock)
{
  constexpr uint8_t CLOCK_SOURCE_NUM_OF_BITS = 1u;
  constexpr uint8_t CLOCK_SOURCE_POSITION = 2u;

  registerValueCTRL = MemoryUtility<uint32_t>::setBits(
    registerValueCTRL,
    CLOCK_SOURCE_POSITION,
    CLOCK_SOURCE_NUM_OF_BITS,
    (useCPUClock ? 1u : 0u));
}

inline bool SysTick::isReloadValueInValidRangeOfValue(uint32_t reloadValue)
{
  constexpr uint32_t MAX_RELOAD_VALUE = 1 << 24u;

  return reloadValue < MAX_RELOAD_VALUE;
}
