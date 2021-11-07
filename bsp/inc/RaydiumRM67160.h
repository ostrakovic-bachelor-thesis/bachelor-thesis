#ifndef RAYDIUM_RM67160_H
#define RAYDIUM_RM67160_H

#include "SysTick.h"


class RaydiumRM67160
{
public:

  typedef void (*CallbackFunc)(void);

  RaydiumRM67160(SysTick *sysTickPtr);

  //! This enum class represents errors which can happen during method calls
  enum class ErrorCode : uint8_t
  {
    OK = 0u,
  };

  struct RaydiumRM67160Config
  {
    CallbackFunc enableDSI3V3Callback;
    CallbackFunc enableDSI1V8Callback;
    CallbackFunc setDSIResetLineToLowCallback;
    CallbackFunc setDSIResetLineToHighCallback;
  };

  ErrorCode init(const RaydiumRM67160Config &raydiumRM67160Config);

private:

  void powerOnDisplay(const RaydiumRM67160Config &raydiumRM67160Config);

  void waitMs(uint64_t periodToWaitInMs);

  //! Pointer to SysTick
  SysTick *m_sysTickPtr;
};

#endif // #ifndef RAYDIUM_RM67160_H