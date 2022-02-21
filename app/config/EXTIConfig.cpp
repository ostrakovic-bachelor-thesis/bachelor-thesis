#include "EXTIConfig.h"
#include "MFXSTM32L152.h"

extern MFXSTM32L152 g_mfx;


EXTI::EXTIConfig g_extiLine1Config =
{
  .isInterruptMasked = false,
  .interruptTrigger  = EXTI::InterruptTrigger::RISING_EDGE,
  .interruptCallback = [] (EXTI::EXTILine extiLine)
    {
      MFXSTM32L152::ErrorCode errorCode = g_mfx.runtimeTask();
      if (MFXSTM32L152::ErrorCode::OK != errorCode)
      {
        // panic();
      }
    }
};