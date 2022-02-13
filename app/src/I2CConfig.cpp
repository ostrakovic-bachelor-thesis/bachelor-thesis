#include "I2CConfig.h"


I2C::I2CConfig g_i2c1Config =
{
  .addressingMode    = I2C::AddressingMode::ADDRESS_7_BITS,
  .clockFrequencySCL = 100000u // 100 kHzs
};