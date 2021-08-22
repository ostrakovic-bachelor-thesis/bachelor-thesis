#include "I2C.h"
#include "MemoryAccess.h"
#include "MemoryUtility.h"
#include "RegisterUtility.h"


I2C::I2C(I2C_TypeDef *I2CPeripheralPtr):
  m_I2CPeripheralPtr(I2CPeripheralPtr)
{}

I2C::ErrorCode I2C::init(void)
{
  disableI2C();

  enableI2C();

  return ErrorCode::OK;
}

inline void I2C::disableI2C(void)
{
  constexpr uint32_t I2C_CR1_PE_POSITION = 0u;
  RegisterUtility<uint32_t>::resetBitInRegister(&(m_I2CPeripheralPtr->CR1), I2C_CR1_PE_POSITION);
}

inline void I2C::enableI2C(void)
{
  constexpr uint32_t I2C_CR1_PE_POSITION = 0u;
  RegisterUtility<uint32_t>::setBitInRegister(&(m_I2CPeripheralPtr->CR1), I2C_CR1_PE_POSITION);
}