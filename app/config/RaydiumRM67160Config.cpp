#include "RaydiumRM67160Config.h"
#include "MFXSTM32L152Config.h"
#include "MFXSTM32L152.h"

extern MFXSTM32L152 g_mfx;

void enableDSI3V3Callback(void);
void enableDSI1V8Callback(void);
void setDSIResetLineToLowCallback(void);
void setDSIResetLineToHighCallback(void);


RaydiumRM67160::RaydiumRM67160Config g_rm67160Config =
{
  .enableDSI3V3Callback          = enableDSI3V3Callback,
  .enableDSI1V8Callback          = enableDSI1V8Callback,
  .setDSIResetLineToLowCallback  = setDSIResetLineToLowCallback,
  .setDSIResetLineToHighCallback = setDSIResetLineToHighCallback,
  .startColumnAddress            = 4u,
  .endColumnAddress              = 393u,
  .startRowAddress               = 0u,
  .endRowAddress                 = 389u,
  .defaultBrightness             = 120u,
};

void enableDSI3V3Callback(void)
{
  MFXSTM32L152::ErrorCode errorCode = g_mfx.configureGPIOPin(MFXSTM32L152::GPIOPin::PIN8, g_mfxGPIOPin8Config);
  if (MFXSTM32L152::ErrorCode::OK != errorCode)
  {
    // panic();
  }

  errorCode = g_mfx.setGPIOPinState(MFXSTM32L152::GPIOPin::PIN8, MFXSTM32L152::GPIOPinState::LOW);
  if (MFXSTM32L152::ErrorCode::OK != errorCode)
  {
    // panic();
  }
}

void enableDSI1V8Callback(void)
{
  MFXSTM32L152::ErrorCode errorCode = g_mfx.configureGPIOPin(MFXSTM32L152::GPIOPin::PIN18, g_mfxGPIOPin18Config);
  if (MFXSTM32L152::ErrorCode::OK != errorCode)
  {
    // panic();
  }

  errorCode = g_mfx.setGPIOPinState(MFXSTM32L152::GPIOPin::PIN18, MFXSTM32L152::GPIOPinState::LOW);
  if (MFXSTM32L152::ErrorCode::OK != errorCode)
  {
    // panic();
  }
}

void setDSIResetLineToLowCallback(void)
{
  MFXSTM32L152::ErrorCode errorCode = g_mfx.configureGPIOPin(MFXSTM32L152::GPIOPin::PIN10, g_mfxGPIOPin10Config);
  if (MFXSTM32L152::ErrorCode::OK != errorCode)
  {
    // panic();
  }

  errorCode = g_mfx.setGPIOPinState(MFXSTM32L152::GPIOPin::PIN10, MFXSTM32L152::GPIOPinState::LOW);
  if (MFXSTM32L152::ErrorCode::OK != errorCode)
  {
    // panic();
  }
}


void setDSIResetLineToHighCallback(void)
{
  //MFXSTM32L152::ErrorCode errorCode = g_mfx.configureGPIOPin(MFXSTM32L152::GPIOPin::PIN10, g_mfxGPIOPin10Config);
  //if (MFXSTM32L152::ErrorCode::OK != errorCode)
  //{
  //  panic();
  //}

  MFXSTM32L152::ErrorCode errorCode =
    g_mfx.setGPIOPinState(MFXSTM32L152::GPIOPin::PIN10, MFXSTM32L152::GPIOPinState::HIGH);
  if (MFXSTM32L152::ErrorCode::OK != errorCode)
  {
    // panic();
  }
}