#ifndef I2C_CONFIG_H
#define I2C_CONFIG_H

#include "I2C.h"


I2C::I2CConfig g_i2c1Config =
{
  .addressingMode    = I2C::AddressingMode::ADDRESS_7_BITS,
  .clockFrequencySCL = 100000u // 100 kHzs
};

#endif // #ifndef I2C_CONFIG_H