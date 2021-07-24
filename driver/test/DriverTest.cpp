#include "DriverTest.h"


uint32_t DriverTest::expectedRegVal(uint32_t initialRegVal, uint32_t position, uint32_t valueSize, uint32_t value)
{
  const uint32_t startBit = position * valueSize;
  const uint32_t mask = 0xFFFFFFFFu >> (sizeof(uint32_t) * 8u - valueSize);

  return (initialRegVal & ~(mask << startBit)) | ((value & mask) << startBit);
}

void DriverTest::expectRegisterSetOnlyOnce(volatile uint32_t *registerPtr, uint32_t registerValue)
{
  EXPECT_CALL(memoryAccessHook, setRegisterValue(registerPtr, registerValue))
    .Times(1u);
  EXPECT_CALL(memoryAccessHook, setRegisterValue(Not(registerPtr), Matcher<uint32_t>(_)))
    .Times(AnyNumber());
}

void DriverTest::expectRegisterSetOnlyOnce(volatile uint16_t *registerPtr, uint16_t registerValue)
{
  EXPECT_CALL(memoryAccessHook, setRegisterValue(registerPtr, registerValue))
    .Times(1u);
  EXPECT_CALL(memoryAccessHook, setRegisterValue(Not(registerPtr), Matcher<uint16_t>(_)))
    .Times(AnyNumber());
}

void DriverTest::expectRegisterSetOnlyOnce(volatile uint8_t *registerPtr, uint8_t registerValue)
{
  EXPECT_CALL(memoryAccessHook, setRegisterValue(registerPtr, registerValue))
    .Times(1u);
  EXPECT_CALL(memoryAccessHook, setRegisterValue(Not(registerPtr), Matcher<uint8_t>(_)))
    .Times(AnyNumber());
}

void DriverTest::expectRegisterNotToChange(volatile uint32_t *registerPtr)
{
  EXPECT_CALL(memoryAccessHook, setRegisterValue(registerPtr, Matcher<uint32_t>(_)))
    .Times(0u);
  EXPECT_CALL(memoryAccessHook, setRegisterValue(Not(registerPtr), Matcher<uint32_t>(_)))
    .Times(AnyNumber());
}

void DriverTest::expectRegisterNotToChange(volatile uint16_t *registerPtr)
{
  EXPECT_CALL(memoryAccessHook, setRegisterValue(registerPtr, Matcher<uint16_t>(_)))
    .Times(0u);
  EXPECT_CALL(memoryAccessHook, setRegisterValue(Not(registerPtr), Matcher<uint16_t>(_)))
    .Times(AnyNumber());
}

void DriverTest::expectRegisterNotToChange(volatile uint8_t *registerPtr)
{
  EXPECT_CALL(memoryAccessHook, setRegisterValue(registerPtr, Matcher<uint8_t>(_)))
    .Times(0u);
  EXPECT_CALL(memoryAccessHook, setRegisterValue(Not(registerPtr), Matcher<uint8_t>(_)))
    .Times(AnyNumber());
}

void DriverTest::expectNoRegisterToChange(void)
{
  EXPECT_CALL(memoryAccessHook, setRegisterValue(Matcher<volatile uint32_t*>(_), Matcher<uint32_t>(_)))
    .Times(0u);
  EXPECT_CALL(memoryAccessHook, setRegisterValue(Matcher<volatile uint16_t*>(_), Matcher<uint16_t>(_)))
    .Times(0u);
}

void DriverTest::SetUp()
{
  MemoryAccess::setMemoryAccessHook(&memoryAccessHook);
}

void DriverTest::TearDown()
{
  MemoryAccess::setMemoryAccessHook(nullptr);
}