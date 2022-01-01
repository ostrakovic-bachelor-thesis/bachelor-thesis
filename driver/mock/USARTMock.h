#ifndef USART_MOCK_H
#define USART_MOCK_H

#include "SysTick.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


class USARTMock : public USART
{
public:

  USARTMock():
    USART(nullptr, nullptr, nullptr)
  {}

  virtual ~USARTMock() = default;

  // mock methods
  MOCK_METHOD(ErrorCode, init, (const USARTConfig &), (override));
  MOCK_METHOD(ErrorCode, write, (const void *, uint32_t), (override));
  MOCK_METHOD(bool, isWriteTransactionOngoing, (), (const, override));
  MOCK_METHOD(void, IRQHandler, (), (override));
};

#endif // #ifndef USART_MOCK_H