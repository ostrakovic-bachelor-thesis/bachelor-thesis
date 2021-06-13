#include "MemoryAccess.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace ::testing;


TEST(MemoryAccess, SetsRegisterValueProperly)
{
  constexpr uint32_t RANDOM_VALUE = 0xFA904B72;
  volatile uint32_t virtualRegister;

  MemoryAccess::setRegisterValue(&virtualRegister, RANDOM_VALUE);

  ASSERT_THAT(virtualRegister, Eq(RANDOM_VALUE));
}

TEST(MemoryAccess, GetsRegisterValueProperly)
{
  constexpr uint32_t RANDOM_VALUE = 0xFA904B72;
  volatile uint32_t virtualRegister = RANDOM_VALUE;

  ASSERT_THAT(MemoryAccess::getRegisterValue(&virtualRegister), Eq(RANDOM_VALUE));
}