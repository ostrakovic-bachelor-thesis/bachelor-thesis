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
  
  template<typename GMockMatcher>
  void expectRegisterSetOnlyOnce(volatile uint32_t *registerPtr, GMockMatcher matcher);

  template<typename GMockMatcher>
  void expectSpecificRegisterSetWithNoChangesAfter(volatile uint32_t *registerPtr, GMockMatcher matcher);

  template<typename GMockMatcher>
  void expectSpecificRegisterSetToBeCalledFirst(volatile uint32_t *registerPtr, GMockMatcher matcher);

  template<typename GMockMatcher>
  void expectSpecificRegisterSetToBeCalledLast(volatile uint32_t *registerPtr, GMockMatcher matcher);

  void SetUp() override;
  void TearDown() override;
};

template<typename GMockMatcher>
void DriverTest::expectRegisterSetOnlyOnce(volatile uint32_t *registerPtr, GMockMatcher matcher)
{
  EXPECT_CALL(memoryAccessHook, setRegisterValue(registerPtr, Matcher<uint32_t>(_)))
    .WillOnce([&](volatile void *registerPtr, uint32_t registerValue) {
      ASSERT_THAT(registerValue, matcher);
    });

  EXPECT_CALL(memoryAccessHook, setRegisterValue(Not(registerPtr), Matcher<uint32_t>(_)))
    .Times(AnyNumber()); 
}


template<typename GMockMatcher>
void DriverTest::expectSpecificRegisterSetWithNoChangesAfter(volatile uint32_t *registerPtr, GMockMatcher matcher)
{
  {
    InSequence s;

    EXPECT_CALL(memoryAccessHook, setRegisterValue(registerPtr, _))
      .Times(AnyNumber());

    EXPECT_CALL(memoryAccessHook, setRegisterValue(registerPtr, matcher))
      .Times(AtLeast(1u));
 }

  EXPECT_CALL(memoryAccessHook, setRegisterValue(Not(registerPtr), Matcher<uint32_t>(_)))
    .Times(AnyNumber()); 
}

template<typename GMockMatcher>
void DriverTest::expectSpecificRegisterSetToBeCalledFirst(volatile uint32_t *registerPtr, GMockMatcher matcher)
{
  {
    InSequence s;
    
    EXPECT_CALL(memoryAccessHook, setRegisterValue(registerPtr, matcher))
      .Times(1u);

    EXPECT_CALL(memoryAccessHook, setRegisterValue(registerPtr, _))
      .Times(AnyNumber());
  }

  EXPECT_CALL(memoryAccessHook, setRegisterValue(Not(registerPtr), Matcher<uint32_t>(_)))
    .Times(AnyNumber()); 
}

template<typename GMockMatcher>
void DriverTest::expectSpecificRegisterSetToBeCalledLast(volatile uint32_t *registerPtr, GMockMatcher matcher)
{
  {
    InSequence s;

    EXPECT_CALL(memoryAccessHook, setRegisterValue(registerPtr, _))
      .Times(AnyNumber());
    
    EXPECT_CALL(memoryAccessHook, setRegisterValue(registerPtr, matcher))
      .Times(1u);
  }

  EXPECT_CALL(memoryAccessHook, setRegisterValue(Not(registerPtr), Matcher<uint32_t>(_)))
    .Times(AnyNumber()); 
}

#endif // #ifndef DRIVER_TEST_H