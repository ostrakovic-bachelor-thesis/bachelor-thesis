#include "USARTConfig.h"


USART::USARTConfig g_usart2Config =
{
  .frameFormat  = USART::FrameFormat::BITS_8_WITHOUT_PARITY,
  .oversampling = USART::Oversampling::OVERSAMPLING_16,
  .stopBits     = USART::StopBits::BIT_1_0,
  .baudrate     = USART::Baudrate::BAUDRATE_115200
};