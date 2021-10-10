#ifndef I2C_MOCK_H
#define I2C_MOCK_H

#include "I2C.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


class I2CMock : public I2C
{
public:

  I2CMock():
    I2C(nullptr, nullptr, nullptr)
  {}

  virtual ~I2CMock() = default;

  // mock methods
  MOCK_METHOD(ErrorCode, init, (const I2CConfig &), (override));
  MOCK_METHOD(ErrorCode, write, (uint16_t, const void *, uint32_t), (override));
  MOCK_METHOD(ErrorCode, read, (uint16_t, void *, uint32_t), (override));
  MOCK_METHOD(ErrorCode, writeMemory, (uint16_t, uint8_t, const void *, uint32_t), (override));
  MOCK_METHOD(ErrorCode, readMemory, (uint16_t, uint8_t, void *, uint32_t), (override));
  MOCK_METHOD(bool, isTransactionOngoing, (), (const, override));
  MOCK_METHOD(void, IRQHandler, (), (override));
};

#endif // #ifndef I2C_MOCK_H