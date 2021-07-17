#ifndef SYS_TICK_TIMER_H
#define SYS_TICK_TIMER_H

#include "stm32l4r9xx.h"
#include "ClockControl.h"
#include <cstdint>


class SysTickTimer
{
public:

  SysTickTimer(SysTick_Type *sysTickPtr, ClockControl *clockControlPtr);
  
  //! This enum class represents errors which can happen during method calls
  enum class ErrorCode : uint8_t
  {
    OK                               = 0u,
    TICKS_PER_SECOND_INVALID_VALUE   = 1u,
    RELOAD_VALUE_CALCULATION_PROBLEM = 2u,
    RELOAD_VALUE_OUT_OF_RANGE        = 3u
  };

  struct SysTickTimerConfig
  {
    uint32_t ticksPerSecond;
    bool enableInterrupt;
    bool enableOnInit;
  };

  ErrorCode init(const SysTickTimerConfig& sysTickTimerConfig);

private:

  void setEnableSysTickFlag(uint32_t &registerValueCTRL, bool enableSysTick);
  void setEnableInterruptFlag(uint32_t &registerValueCTRL, bool enableInterrupt);
  void setClockSourceFlag(uint32_t &registerValueCTRL, bool useCPUClock);

  void setReloadValue(uint32_t reloadValue);
  void setCurrentValue(uint32_t currentValue);
  ErrorCode ticksPerSecondToReloadValue(uint32_t ticksPerSecond, uint32_t &reloadValue);

  static bool isReloadValueInValidRangeOfValue(uint32_t reloadValue);
  
  //! TODO
  SysTick_Type *m_sysTickPtr;

  //! Pointer to Clock Control module
  ClockControl *m_clockControlPtr;
};

#endif // #ifndef SYS_TICK_TIMER_H