#include "RaydiumRM67160.h"


RaydiumRM67160::RaydiumRM67160(SysTick *sysTickPtr):
  m_sysTickPtr(sysTickPtr)
{}

RaydiumRM67160::ErrorCode RaydiumRM67160::init(const RaydiumRM67160Config &raydiumRM67160Config)
{
  powerOnDisplay(raydiumRM67160Config);

  return ErrorCode::OK;
}

void RaydiumRM67160::powerOnDisplay(const RaydiumRM67160Config &raydiumRM67160Config)
{
  constexpr uint64_t WAIT_BEFORE_ENABLE_DSI_3V3_PERIOD_MS        = 5u;
  constexpr uint64_t WAIT_BEFORE_ENABLE_DSI_1V8_PERIOD_MS        = 1u;
  constexpr uint64_t WAIT_BEFORE_SET_DSI_RESET_TO_HIGH_PERIOD_MS = 15u;
  constexpr uint64_t WAIT_FOR_RESET_TO_COMPLETE_PERIOD_MS        = 120u;

  raydiumRM67160Config.setDSIResetLineToLowCallback();

  waitMs(WAIT_BEFORE_ENABLE_DSI_3V3_PERIOD_MS);

  raydiumRM67160Config.enableDSI3V3Callback();

  waitMs(WAIT_BEFORE_ENABLE_DSI_1V8_PERIOD_MS);

  raydiumRM67160Config.enableDSI1V8Callback();

  waitMs(WAIT_BEFORE_SET_DSI_RESET_TO_HIGH_PERIOD_MS);

  raydiumRM67160Config.setDSIResetLineToHighCallback();

  waitMs(WAIT_FOR_RESET_TO_COMPLETE_PERIOD_MS);
}

void RaydiumRM67160::waitMs(uint64_t periodToWaitInMs)
{
  const uint32_t timestamp = m_sysTickPtr->getTicks();

  while (m_sysTickPtr->getElapsedTimeInMs(timestamp) < periodToWaitInMs);
}