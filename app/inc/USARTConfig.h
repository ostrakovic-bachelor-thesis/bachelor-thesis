#ifndef USART_CONFIG_H
#define USART_CONFIG_H

#include "USART.h"


USART::USARTConfig g_usart2Config =
{
  .frameFormat  = USART::FrameFormat::BITS_8_WITHOUT_PARITY,
  .oversampling = USART::Oversampling::OVERSAMPLING_16,
  .stopBits     = USART::StopBits::BIT_1_0,
  .baudrate     = USART::Baudrate::BAUDRATE_115200
};

#endif // #ifndef USART_CONFIG_H