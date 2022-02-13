#include "FT3267Config.h"


FT3267::FT3267Config g_ft3267Config =
{
  .peripheralAddress                = 0x38,
  .performPowerOn                   = false,
  .setFT3267ResetLineToLowCallback  = nullptr,
  .setFT3267ResetLineToHighCallback = nullptr,
  .mapFromTouchScreenToDisplayCoordinatesFunc =
    [](FT3267::TouchPosition touchPosition) -> FT3267::TouchPosition
    {
      constexpr uint16_t SCREEN_CENTER_X = 390u / 2u;
      constexpr uint16_t SCREEN_CENTER_Y = 390u / 2u;
      constexpr float SIN_45_DEG = 0.707107f;
      constexpr float COS_45_DEG = 0.707107f;

      // translate coordinates to the center of the screen
      int16_t x = touchPosition.x - SCREEN_CENTER_X;
      int16_t y = touchPosition.y - SCREEN_CENTER_Y;

      // apply rotation of 45deg
      int16_t xRot = x * COS_45_DEG - y * SIN_45_DEG;
      int16_t yRot = x * SIN_45_DEG + y * COS_45_DEG;

      // revert initial translation
      return
      {
        .x = static_cast<uint16_t>(xRot + SCREEN_CENTER_X),
        .y = static_cast<uint16_t>(yRot + SCREEN_CENTER_Y)
      };
    }
};