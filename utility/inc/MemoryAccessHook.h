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
  MOCK_METHOD(void, setRegisterValue, (volatile void *, uint32_t));
  MOCK_METHOD(uint32_t, getRegisterValue, (volatile const void *));

};

#endif // MEMORY_ACCESS_HOOK_H