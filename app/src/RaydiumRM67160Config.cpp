#include "RaydiumRM67160Config.h"

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