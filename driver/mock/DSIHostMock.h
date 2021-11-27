#ifndef DSI_HOST_MOCK_H
#define DSI_HOST_MOCK_H

#include "DSIHost.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


class DSIHostMock : public DSIHost
{
public:

  DSIHostMock():
    DSIHost(nullptr, nullptr, nullptr)
  {}

  virtual ~DSIHostMock() = default;

  // mock methods
  MOCK_METHOD(ErrorCode, init, (const DSIHostConfig &), (override));
  MOCK_METHOD(ErrorCode, genericShortWrite, (VirtualChannelID), (override));
  MOCK_METHOD(ErrorCode, genericShortWrite, (VirtualChannelID, uint8_t), (override));
  MOCK_METHOD(ErrorCode, genericShortWrite, (VirtualChannelID, uint8_t, uint8_t), (override));
  MOCK_METHOD(ErrorCode, dcsShortWrite, (VirtualChannelID, uint8_t), (override));
  MOCK_METHOD(ErrorCode, dcsShortWrite, (VirtualChannelID, uint8_t, uint8_t), (override));
  MOCK_METHOD(ErrorCode, genericLongWrite, (VirtualChannelID, const void *, uint16_t), (override));
  MOCK_METHOD(ErrorCode, dcsLongWrite, (VirtualChannelID, uint8_t, const void *, uint16_t), (override));
  MOCK_METHOD(ErrorCode, getDSIPHYClockFrequency, (uint32_t &), (override));
};

#endif // #ifndef DSI_HOST_MOCK_H