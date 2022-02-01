#ifndef FT3267_MOCK_H
#define FT3267_MOCK_H

#include "FT3267.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


class FT3267Mock : public FT3267
{
public:

  FT3267Mock():
    FT3267(nullptr, nullptr)
  {}

  virtual ~FT3267Mock() = default;

  // mock methods
  MOCK_METHOD(ErrorCode, init, (FT3267Config &), (override));
  MOCK_METHOD(ErrorCode, getID, (uint8_t &), (override));
  MOCK_METHOD(ErrorCode, getFirmwareID, (uint8_t &), (override));
  MOCK_METHOD(ErrorCode, registerTouchEventCallback, (TouchEventCallbackFunc, void *), (override));
  MOCK_METHOD(ErrorCode, runtimeTask, (), (override));
};

#endif // #ifndef FT3267_MOCK_H