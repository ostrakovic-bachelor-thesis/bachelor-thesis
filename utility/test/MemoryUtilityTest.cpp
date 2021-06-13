#include "MemoryUtility.h"
#include "MemoryAccess.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace ::testing;


class TheMemoryUtility : public Test
{
public:

  static constexpr uint32_t BIT_IN_ALLOWED_SCOPE     = 15u;
  static constexpr uint32_t BIT_OUT_OF_ALLOWED_SCOPE = 33u;
  static constexpr uint32_t ALL_BITS_SET             = ~0u;

  NiceMock<MemoryAccessHook> memoryAccessHook;
  
  void SetUp() override
  {
    MemoryAccess::setMemoryAccessHook(&memoryAccessHook);
  }

  void TearDown() override
  {
    MemoryAccess::setMemoryAccessHook(nullptr);
  }
};


TEST_F(TheMemoryUtility, SetsBitInValueProperly)
{
  ASSERT_THAT(MemoryUtility<uint32_t>::setBit(0u, BIT_IN_ALLOWED_SCOPE), Eq(1u << BIT_IN_ALLOWED_SCOPE));
}

TEST_F(TheMemoryUtility, SetBitReturnsForwardedValueIfBitIsOutOfAllowedScope)
{
  ASSERT_THAT(MemoryUtility<uint32_t>::setBit(0u, BIT_OUT_OF_ALLOWED_SCOPE), Eq(0u));
}

TEST_F(TheMemoryUtility, ResetsBitInValueProperly)
{
  ASSERT_THAT(MemoryUtility<uint32_t>::resetBit(1u << BIT_IN_ALLOWED_SCOPE, BIT_IN_ALLOWED_SCOPE), Eq(0u));
}

TEST_F(TheMemoryUtility, ResetBitReturnsForwardedValueIfBitIsOutOfAllowedScope)
{
  ASSERT_THAT(MemoryUtility<uint32_t>::resetBit(ALL_BITS_SET, BIT_OUT_OF_ALLOWED_SCOPE), Eq(ALL_BITS_SET));
}

TEST_F(TheMemoryUtility, ChecksIsBitSetInValueProperly)
{
  ASSERT_THAT(MemoryUtility<uint32_t>::isBitSet(0u, BIT_IN_ALLOWED_SCOPE), Eq(false));
  ASSERT_THAT(MemoryUtility<uint32_t>::isBitSet(1 << BIT_IN_ALLOWED_SCOPE, BIT_IN_ALLOWED_SCOPE), Eq(true));
}

TEST_F(TheMemoryUtility, CheckIsBitSetReturnsFalseIfBitIsOutOfAllowedScope)
{
  ASSERT_THAT(MemoryUtility<uint32_t>::isBitSet(ALL_BITS_SET, BIT_OUT_OF_ALLOWED_SCOPE), Eq(false));
}

TEST_F(TheMemoryUtility, SetsSubsetOfBitsInForwadedValueToWantedValue)
{
  static constexpr uint32_t START_BIT    = 20u;
  static constexpr uint32_t NUM_OF_BITS  = 4u;
  static constexpr uint32_t VALUE        = 0x5;
  static constexpr uint32_t INITIAL_VALUE  = 0xFFFFFFFF;
  static constexpr uint32_t EXPECTED_VALUE = 0xFF5FFFFF;

  ASSERT_THAT(MemoryUtility<uint32_t>::setBits(INITIAL_VALUE, START_BIT, NUM_OF_BITS, VALUE), Eq(EXPECTED_VALUE));
}

TEST_F(TheMemoryUtility, SetBitsMasksAllBitsInValueExceptFirstNumOfBits)
{
  static constexpr uint32_t START_BIT   = 4u;
  static constexpr uint32_t NUM_OF_BITS = 8u;
  static constexpr uint32_t VALUE       = 0xA0C;
  static constexpr uint32_t INITIAL_VALUE  = 0xFFFF0FFF;
  static constexpr uint32_t EXPECTED_VALUE = 0xFFFF00CF;

  ASSERT_THAT(MemoryUtility<uint32_t>::setBits(INITIAL_VALUE, START_BIT, NUM_OF_BITS, VALUE), Eq(EXPECTED_VALUE));
}

TEST_F(TheMemoryUtility, SetBitsDoesNotChangeValueIfBitsToChangeAreOutOfAllowedScope)
{
  static constexpr uint32_t START_BIT   = 30u;
  static constexpr uint32_t NUM_OF_BITS = 4u;
  static constexpr uint32_t VALUE       = 0x5;
  static constexpr uint32_t FORWARDED_VALUE = ALL_BITS_SET;

  ASSERT_THAT(MemoryUtility<uint32_t>::setBits(FORWARDED_VALUE, START_BIT, NUM_OF_BITS, VALUE), Eq(FORWARDED_VALUE));
}

TEST_F(TheMemoryUtility, GetsValueOfSubsetOfBitsInForwardedValue)
{
  static constexpr uint32_t START_BIT   = 16u;
  static constexpr uint32_t NUM_OF_BITS = 8u;
  static constexpr uint32_t FORWARDED_VALUE = 0x00FA0000;
  static constexpr uint32_t EXPECTED_VALUE  = 0xFA;

  ASSERT_THAT(MemoryUtility<uint32_t>::getBits(FORWARDED_VALUE, START_BIT, NUM_OF_BITS), Eq(EXPECTED_VALUE));
}

TEST_F(TheMemoryUtility, GetBitsReturnsZeroIfBitsToGetAreOutOfAllowedScope)
{
  static constexpr uint32_t START_BIT   = 30u;
  static constexpr uint32_t NUM_OF_BITS = 4u;
  static constexpr uint32_t FORWARDED_VALUE = ALL_BITS_SET;

  ASSERT_THAT(MemoryUtility<uint32_t>::getBits(FORWARDED_VALUE, START_BIT, NUM_OF_BITS), Eq(0u));
}