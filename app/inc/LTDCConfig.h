#ifndef LTDC_CONFIG_H
#define LTDC_CONFIG_H

#include "LTDC.h"
#include "AppFrameBuffer.h"


LTDC::LTDCConfig g_ltdcConfig =
{
  .hsyncWidth            = 1u,
  .vsyncWidth            = 1u,
  .horizontalBackPorch   = 1u,
  .verticalBackPorch     = 1u,
  .displayWidth          = 390u,
  .displayHeight         = 390u,
  .horizontalFrontPorch  = 1u,
  .verticalFrontPorch    = 1u,
  .hsyncPolarity         = LTDC::Polarity::ACTIVE_LOW,
  .vsyncPolarity         = LTDC::Polarity::ACTIVE_LOW,
  .notDataEnablePolarity = LTDC::Polarity::ACTIVE_LOW,
  .pixelClockPolarity    = LTDC::Polarity::ACTIVE_LOW,
  .backgroundColor       =
  {
    .alpha = 0u,
    .red   = 0u,
    .green = 0u,
    .blue  = 0u,
  },
};

LTDC::LTDCLayerConfig g_ltdcLayer1Config =
{
  .alpha        = 255u,
  .defaultColor =
  {
    .alpha = 0u,
    .red   = 0u,
    .green = 0u,
    .blue  = 0u,
  },
  .currentLayerBlendingFactor   = LTDC::BlendingFactor::PIXEL_ALPHA_X_CONST_ALPHA,
  .subjacentLayerBlendingFactor = LTDC::BlendingFactor::PIXEL_ALPHA_X_CONST_ALPHA,
  .frameBufferConfig            =
  {
    .colorFormat     = LTDC::ColorFormat::RGB888,
    .bufferDimension =
    {
      .width  = 390u,
      .height = 390u,
    },
    .bufferPtr = g_frameBuffer.getPointer(),
  },
};

#endif // #ifndef LTDC_CONFIG_H