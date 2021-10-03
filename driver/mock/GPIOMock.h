#ifndef GPIO_MOCK_H
#define GPIO_MOCK_H

#include "GPIO.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


class GPIOMock : public GPIO
{
public:

  GPIOMock():
    GPIO(nullptr)
  {}

  virtual ~GPIOMock() = default;

  // mock methods
  MOCK_METHOD(ErrorCode, configurePin, (Pin, const PinConfiguration &), (override));
  MOCK_METHOD(ErrorCode, setPinState, (Pin, PinState), (override));
  MOCK_METHOD(ErrorCode, getPinState, (Pin, PinState &), (override, const));
  MOCK_METHOD(ErrorCode, getPinMode, (Pin, PinMode &), (override, const));
};

#endif // #ifndef GPIO_MOCK_H