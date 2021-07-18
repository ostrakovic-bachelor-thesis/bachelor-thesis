#ifndef MEMORY_ACCESS_HOOK_H
#define MEMORY_ACCESS_HOOK_H

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>

using ::testing::NiceMock;


class MemoryAccessHook
{
public:
  virtual ~MemoryAccessHook() = default;
   
  // Mock methods
  MOCK_METHOD(void, setRegisterValue, (volatile uint32_t *, uint32_t));
  MOCK_METHOD(void, setRegisterValue, (volatile uint16_t *, uint16_t));
  MOCK_METHOD(void, setRegisterValue, (volatile uint8_t *, uint8_t));
  MOCK_METHOD(uint32_t, getRegisterValue, (volatile const uint32_t *));
  MOCK_METHOD(uint16_t, getRegisterValue, (volatile const uint16_t *));
  MOCK_METHOD(uint8_t, getRegisterValue, (volatile const uint8_t *));

};

#endif // MEMORY_ACCESS_HOOK_H