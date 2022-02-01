#ifndef MFXSTM32L152_MOCK_H
#define MFXSTM32L152_MOCK_H

#include "MFXSTM32L152.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


class MFXSTM32L152Mock : public MFXSTM32L152
{
public:

  MFXSTM32L152Mock():
    MFXSTM32L152(nullptr, nullptr)
  {}

  virtual ~MFXSTM32L152Mock() = default;

  // mock methods
  MOCK_METHOD(ErrorCode, init, (MFXSTM32L152Config &), (override));
  MOCK_METHOD(ErrorCode, wakeUp, (), (override));
  MOCK_METHOD(ErrorCode, configureGPIOPin, (GPIOPin, const GPIOPinConfiguration &), (override));
  MOCK_METHOD(ErrorCode, configureIRQPin, (const IRQPinConfiguration &), (override));
  MOCK_METHOD(ErrorCode, setGPIOPinState, (GPIOPin, GPIOPinState), (override));
  MOCK_METHOD(ErrorCode, getGPIOPinState, (GPIOPin, GPIOPinState &), (override));
  MOCK_METHOD(ErrorCode, enableGPIO, (), (override));
  MOCK_METHOD(ErrorCode, enableInterrupt, (Interrupt), (override));
  MOCK_METHOD(ErrorCode, getID, (uint8_t &), (override));
  MOCK_METHOD(ErrorCode, registerGPIOInterruptCallback, (GPIOPin, CallbackFunc, void *), (override));
  MOCK_METHOD(ErrorCode, runtimeTask, (), (override));
};

#endif // #ifndef MFXSTM32L152_MOCK_H