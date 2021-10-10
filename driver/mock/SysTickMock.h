#ifndef SYS_TICK_MOCK_H
#define SYS_TICK_MOCK_H

#include "SysTick.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


class SysTickMock : public SysTick
{
public:

  SysTickMock():
    SysTick(nullptr, nullptr, nullptr)
  {}

  virtual ~SysTickMock() = default;

  // mock methods
  MOCK_METHOD(ErrorCode, init, (const SysTickConfig &), (override));
  MOCK_METHOD(uint64_t, getTicks, (), (const, override));
  MOCK_METHOD(uint32_t, getTicksPerSecond, (), (const, override));
  MOCK_METHOD(uint64_t, getElapsedTimeInMs, (uint64_t), (const, override));
  MOCK_METHOD(void, IRQHandler, (), (override));
};

#endif // #ifndef SYS_TICK_MOCK_H