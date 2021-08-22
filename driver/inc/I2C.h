#ifndef I2C_H
#define I2C_H

#include "stm32l4r9xx.h"
#include "Peripheral.h"
#include <cstdint>


class I2C
{
public:

  //! This enum class represents errors which can happen during method calls
  enum class ErrorCode : uint8_t
  {
    OK = 0u,
  };

  I2C(I2C_TypeDef *I2CPeripheralPtr);

  ErrorCode init(void);

private:

  void disableI2C(void);

  void enableI2C(void);

  //! Pointer to I2C peripheral
  I2C_TypeDef *m_I2CPeripheralPtr;

};

#endif // #ifndef I2C_H