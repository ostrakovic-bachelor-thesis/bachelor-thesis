#ifndef DRIVER_TEST_H
#define DRIVER_TEST_H

#include "MemoryUtility.h"
#include "MemoryAccess.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"


using namespace ::testing;


MATCHER_P2(BitHasValue, bit, value, "") 
{
  return ((arg >> bit) & 0x1) == (value & 0x1);
}

MATCHER_P3(BitsHaveValue, startBit, numberOfBit, value, "") 
{
  const uint32_t mask = 0xFFFFFFFFu >> (sizeof(uint32_t) * 8u - numberOfBit);
  return ((arg >> startBit) & mask) == (value & mask);
}


class DriverTest : public Test
{
public:

  //! Needed to verify statement for setting and getting registers value
  NiceMock<MemoryAccessHook> memoryAccessHook;

  uint32_t expectedRegVal(uint32_t initialRegVal, uint32_t position, uint32_t valueSize, uint32_t value);
  void expectRegisterSetOnlyOnce(volatile uint32_t *registerPtr, uint32_t registerValue);
  void expectRegisterNotToChange(volatile uint32_t *registerPtr);
  void expectNoRegisterToChange(void);
  
  template<typename Matcher>
  void expectRegisterSetOnlyOnce(volatile uint32_t *registerPtr, Matcher matcher);

  void SetUp() override;
  void TearDown() override;
};

template<typename Matcher>
void DriverTest::expectRegisterSetOnlyOnce(volatile uint32_t *registerPtr, Matcher matcher)
{
  EXPECT_CALL(memoryAccessHook, setRegisterValue(registerPtr, _))
    .WillOnce([&](volatile void *registerPtr, uint32_t registerValue) {
      ASSERT_THAT(registerValue, matcher);
    });

  EXPECT_CALL(memoryAccessHook, setRegisterValue(Not(registerPtr), _))
    .Times(AnyNumber()); 
}

#endif // #ifndef DRIVER_TEST_H