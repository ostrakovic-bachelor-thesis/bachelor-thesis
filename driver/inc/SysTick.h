#ifndef SYS_TICK_H
#define SYS_TICK_H

#include "stm32l4r9xx.h"
#include "Peripheral.h"
#include "ClockControl.h"
#include "CoreHardware.h"
#include "InterruptController.h"
#include <cstdint>


class SysTick
{
public:

  SysTick(SysTick_Type *sysTickPtr, ClockControl *clockControlPtr, InterruptController *InterruptControllerPtr);

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ~SysTick() = default;

  //! This enum class represents errors which can happen during method calls
  enum class ErrorCode : uint8_t
  {
    OK                               = 0u,
    TICKS_PER_SECOND_INVALID_VALUE   = 1u,
    RELOAD_VALUE_CALCULATION_PROBLEM = 2u,
    RELOAD_VALUE_OUT_OF_RANGE        = 3u
  };

  struct SysTickConfig
  {
    uint32_t ticksPerSecond;
    bool enableInterrupt;
    bool enableOnInit;
  };

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode init(const SysTickConfig& sysTickConfig);

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  inline uint64_t getTicks(void) const
  {
    return m_ticks;
  }

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  inline uint32_t getTicksPerSecond(void) const
  {
    return m_ticksPerSecond;
  }

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  uint64_t getElapsedTimeInMs(uint64_t timestamp) const;

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  void IRQHandler(void);

#ifdef UNIT_TEST
  /**
   * @brief Method gets raw pointer to underlaying SysTick core hardware instance.
   *
   * @return Pointer to underlaying SysTick core hardware instance.
   */
  inline void* getRawPointer(void) const
  {
    return reinterpret_cast<void*>(m_sysTickPtr);
  }
#endif // #ifdef UNIT_TEST

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

  //! TODO
  InterruptController *m_interruptControllerPtr;

  //! TODO
  uint64_t m_ticks;

  //! TODO
  uint32_t m_ticksPerSecond;
};

#endif // #ifndef SYS_TICK_H