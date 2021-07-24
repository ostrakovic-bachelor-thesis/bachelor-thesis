#include "RegisterUtility.h"
#include "MemoryAccess.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace ::testing;


class TheRegisterUtility : public Test
{
public:

  static constexpr uint32_t BIT_IN_ALLOWED_SCOPE     = 15u;
  static constexpr uint32_t BIT_OUT_OF_ALLOWED_SCOPE = 33u;
  static constexpr uint32_t REGISTER_ALL_BITS_SET    = ~0u;

  NiceMock<MemoryAccessHook> memoryAccessHook;
  volatile uint32_t virtualRegister;

  void SetUp() override
  {
    MemoryAccess::setMemoryAccessHook(&memoryAccessHook);
  }

  void TearDown() override
  {
    MemoryAccess::setMemoryAccessHook(nullptr);
  }
};


TEST_F(TheRegisterUtility, SetsBitInRegisterProperly)
{
  virtualRegister = 0u;
  EXPECT_CALL(memoryAccessHook, setRegisterValue(&virtualRegister, 1u << BIT_IN_ALLOWED_SCOPE))
    .Times(1u);

  RegisterUtility<uint32_t>::setBitInRegister(&virtualRegister, BIT_IN_ALLOWED_SCOPE);

  ASSERT_THAT(virtualRegister, Eq(1u << BIT_IN_ALLOWED_SCOPE));
}

TEST_F(TheRegisterUtility, SetBitInRegisterDoesNotChangeValueIfBitIsOutOfAllowedScope)
{
  virtualRegister = 0u;
  EXPECT_CALL(memoryAccessHook, setRegisterValue(Matcher<volatile uint32_t*>(_), _))
    .Times(0u);

  RegisterUtility<uint32_t>::setBitInRegister(&virtualRegister, BIT_OUT_OF_ALLOWED_SCOPE);

  ASSERT_THAT(virtualRegister, Eq(0u));
}

TEST_F(TheRegisterUtility, ResetsBitInRegisterProperly)
{
  virtualRegister = 1u << BIT_IN_ALLOWED_SCOPE;
  EXPECT_CALL(memoryAccessHook, setRegisterValue(&virtualRegister, 0u))
    .Times(1u);

  RegisterUtility<uint32_t>::resetBitInRegister(&virtualRegister, BIT_IN_ALLOWED_SCOPE);

  ASSERT_THAT(virtualRegister, Eq(0u));
}

TEST_F(TheRegisterUtility, ResetBitInRegisterDoesNotChangeValueIfBitIsOutOfAllowedScope)
{
  virtualRegister = 1u << BIT_IN_ALLOWED_SCOPE;
  EXPECT_CALL(memoryAccessHook, setRegisterValue(Matcher<volatile uint32_t*>(_), _))
    .Times(0u);

  RegisterUtility<uint32_t>::resetBitInRegister(&virtualRegister, BIT_OUT_OF_ALLOWED_SCOPE);

  ASSERT_THAT(virtualRegister, Eq(1u << BIT_IN_ALLOWED_SCOPE));
}

TEST_F(TheRegisterUtility, ChecksIsBitSetProperly)
{
  virtualRegister = 1 << BIT_IN_ALLOWED_SCOPE;
  EXPECT_CALL(memoryAccessHook, getRegisterValue(&virtualRegister))
    .Times(2u);

  ASSERT_THAT(RegisterUtility<uint32_t>::isBitSetInRegister(&virtualRegister, 0u), Eq(false));
  ASSERT_THAT(RegisterUtility<uint32_t>::isBitSetInRegister(&virtualRegister, BIT_IN_ALLOWED_SCOPE), Eq(true));
}

TEST_F(TheRegisterUtility, ReturnsFalseIfBitIsOutOfAllowedScope)
{
  virtualRegister = REGISTER_ALL_BITS_SET;

  ASSERT_THAT(RegisterUtility<uint32_t>::isBitSetInRegister(&virtualRegister, BIT_OUT_OF_ALLOWED_SCOPE), Eq(false));
}

TEST_F(TheRegisterUtility, SetsSubsetOfBitsInRegisterToWantedValue)
{
  static constexpr uint32_t START_BIT   = 20u;
  static constexpr uint32_t NUM_OF_BITS = 4u;
  static constexpr uint32_t VALUE       = 0x5;
  static constexpr uint32_t INITIAL_REGISTER_VALUE  = 0xFFFFFFFF;
  static constexpr uint32_t EXPECTED_REGISTER_VALUE = 0xFF5FFFFF;
  virtualRegister = INITIAL_REGISTER_VALUE;
  EXPECT_CALL(memoryAccessHook, setRegisterValue(&virtualRegister, EXPECTED_REGISTER_VALUE))
    .Times(1u);

  RegisterUtility<uint32_t>::setBitsInRegister(&virtualRegister, START_BIT, NUM_OF_BITS, VALUE);

  ASSERT_THAT(virtualRegister, Eq(EXPECTED_REGISTER_VALUE));
}

TEST_F(TheRegisterUtility, SetsSubsetOfBitsMasksAllBitsInValueExpectFirstNumOfBits)
{
  static constexpr uint32_t START_BIT   = 4u;
  static constexpr uint32_t NUM_OF_BITS = 8u;
  static constexpr uint32_t VALUE       = 0xA0C;
  static constexpr uint32_t INITIAL_REGISTER_VALUE  = 0xFFFF0FFF;
  static constexpr uint32_t EXPECTED_REGISTER_VALUE = 0xFFFF00CF;
  virtualRegister = INITIAL_REGISTER_VALUE;
  EXPECT_CALL(memoryAccessHook, setRegisterValue(&virtualRegister, EXPECTED_REGISTER_VALUE))
    .Times(1u);

  RegisterUtility<uint32_t>::setBitsInRegister(&virtualRegister, START_BIT, NUM_OF_BITS, VALUE);

  ASSERT_THAT(virtualRegister, Eq(EXPECTED_REGISTER_VALUE));
}

TEST_F(TheRegisterUtility, SetsSubsetOfBitsDoesNotChangeValueIfBitsToChangeAreOutOfScope)
{
  static constexpr uint32_t START_BIT   = 30u;
  static constexpr uint32_t NUM_OF_BITS = 4u;
  static constexpr uint32_t VALUE       = 0x5;
  static constexpr uint32_t INITIAL_REGISTER_VALUE = 0xFFFFFFFF;
  virtualRegister = INITIAL_REGISTER_VALUE;
  EXPECT_CALL(memoryAccessHook, setRegisterValue(Matcher<volatile uint32_t*>(_), _))
    .Times(0u);

  RegisterUtility<uint32_t>::setBitsInRegister(&virtualRegister, START_BIT, NUM_OF_BITS, VALUE);

  ASSERT_THAT(virtualRegister, Eq(INITIAL_REGISTER_VALUE));
}

TEST_F(TheRegisterUtility, GetsValueOfSubsetOfBitsInRegister)
{
  static constexpr uint32_t START_BIT   = 16u;
  static constexpr uint32_t NUM_OF_BITS = 8u;
  static constexpr uint32_t INITIAL_REGISTER_VALUE  = 0x00FA0000;
  static constexpr uint32_t EXPECTED_VALUE = 0xFA;
  virtualRegister = INITIAL_REGISTER_VALUE;
  EXPECT_CALL(memoryAccessHook, getRegisterValue(&virtualRegister))
    .Times(1u);

  uint32_t value = RegisterUtility<uint32_t>::getBitsInRegister(&virtualRegister, START_BIT, NUM_OF_BITS);

  ASSERT_THAT(value, Eq(EXPECTED_VALUE));
}

TEST_F(TheRegisterUtility, GetsValueOfSubsetOfBitsInRegisterReturnsZeroIfBitsToGetAreOutOfScope)
{
  static constexpr uint32_t START_BIT   = 30u;
  static constexpr uint32_t NUM_OF_BITS = 4u;
  static constexpr uint32_t INITIAL_REGISTER_VALUE = 0xFFFFFFFF;
  virtualRegister = INITIAL_REGISTER_VALUE;
  EXPECT_CALL(memoryAccessHook, setRegisterValue(Matcher<volatile uint32_t*>(_), _))
    .Times(0u);

  uint32_t value = RegisterUtility<uint32_t>::getBitsInRegister(&virtualRegister, START_BIT, NUM_OF_BITS);

  ASSERT_THAT(value, Eq(0u));
}