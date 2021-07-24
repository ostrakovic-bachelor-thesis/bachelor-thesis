#include "InterruptController.h"
#include "MemoryUtility.h"
#include "MemoryAccess.h"
#include "DriverTest.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <algorithm>


using namespace ::testing;


class AnInterruptController : public DriverTest
{
public:

  static constexpr uint32_t NVIC_ISER_RESET_VALUE  = 0x00000000u;
  static constexpr uint32_t NVIC_ICER_RESET_VALUE  = 0x00000000u;
  static constexpr uint32_t NVIC_ISPR_RESET_VALUE  = 0x00000000u;
  static constexpr uint32_t NVIC_ICPR_RESET_VALUE  = 0x00000000u;
  static constexpr uint32_t NVIC_IABR_RESET_VALUE  = 0x00000000u;
  static constexpr uint8_t  NVIC_IP_RESET_VALUE    = 0x00u;
  static constexpr uint32_t NVIC_STIR_RESET_VALUE  = 0x00000000u;

  static constexpr uint8_t SCB_SHP_RESET_VALUE = 0x00u;

  static constexpr uint8_t  NUMBER_OF_BITS_IN_UINT8_T         = 8u;
  static constexpr uint32_t NUMBER_OF_BITS_IN_UINT32_T        = 32u;
  static constexpr uint32_t RANDOM_VALID_INTERRUPT_PRIORITY   = 3u;
  static constexpr uint32_t RANDOM_INVALID_INTERRUPT_PRIORITY = 20u;
  static constexpr int32_t  RANDOM_MCU_SPECIFIC_IRQ_NUMBER    = EXTI0_IRQn;
  static constexpr int32_t  RANDOM_CPU_EXCEPTION_IRQ_NUMBER   = SysTick_IRQn;
  static constexpr int32_t  RANDOM_VALID_IRQ_NUMBER           = UART4_IRQn;
  static constexpr int32_t  RANDOM_INVALID_IRQ_NUMBER         = -20;

  NVIC_Type virtualNVIC;
  SCB_Type virtualSCB =
  {
    .CPUID = 0xABCDABCD
  };
  InterruptController virtualInterruptController = InterruptController(&virtualNVIC, &virtualSCB);

  void SetUp() override;
  void TearDown() override;
};

void AnInterruptController::SetUp()
{
  DriverTest::SetUp();

  std::for_each(virtualNVIC.ISER,
    virtualNVIC.ISER + sizeof(virtualNVIC.ISER) / sizeof(virtualNVIC.ISER[0]),
    [] (volatile uint32_t &regValue) { regValue = NVIC_ISER_RESET_VALUE; });

  std::for_each(virtualNVIC.ICER,
    virtualNVIC.ICER + sizeof(virtualNVIC.ICER) / sizeof(virtualNVIC.ICER[0]),
    [] (volatile uint32_t &regValue) { regValue = NVIC_ICER_RESET_VALUE; });

 std::for_each(virtualNVIC.ISPR,
    virtualNVIC.ISPR + sizeof(virtualNVIC.ISPR) / sizeof(virtualNVIC.ISPR[0]),
    [] (volatile uint32_t &regValue) { regValue = NVIC_ISPR_RESET_VALUE; });

  std::for_each(virtualNVIC.ICPR,
    virtualNVIC.ICPR + sizeof(virtualNVIC.ICPR) / sizeof(virtualNVIC.ICPR[0]),
    [] (volatile uint32_t &regValue) { regValue = NVIC_ICPR_RESET_VALUE; });

  std::for_each(virtualNVIC.IABR,
    virtualNVIC.IABR + sizeof(virtualNVIC.IABR) / sizeof(virtualNVIC.IABR[0]),
    [] (volatile uint32_t &regValue) { regValue = NVIC_IABR_RESET_VALUE; });

  std::for_each(virtualNVIC.IP,
    virtualNVIC.IP + sizeof(virtualNVIC.IP) / sizeof(virtualNVIC.IP[0]),
    [] (volatile uint8_t &regValue) { regValue = NVIC_IP_RESET_VALUE; });

  virtualNVIC.STIR = NVIC_STIR_RESET_VALUE;

  std::for_each(virtualSCB.SHP,
    virtualSCB.SHP + sizeof(virtualSCB.SHP) / sizeof(virtualSCB.SHP[0]),
    [] (volatile uint8_t &regValue) { regValue = SCB_SHP_RESET_VALUE; });
}

void AnInterruptController::TearDown()
{
  DriverTest::TearDown();
}


TEST_F(AnInterruptController, setInterruptProrityInIPRegisterOfNVICForMCUSpecificInterrupts)
{
  const uint8_t expectedIPValue = RANDOM_VALID_INTERRUPT_PRIORITY << (NUMBER_OF_BITS_IN_UINT8_T - __NVIC_PRIO_BITS);
  expectRegisterSetOnlyOnce(&(virtualNVIC.IP[RANDOM_MCU_SPECIFIC_IRQ_NUMBER]), expectedIPValue);

  const InterruptController::ErrorCode errorCode =
    virtualInterruptController.setInterruptPriority(RANDOM_MCU_SPECIFIC_IRQ_NUMBER, RANDOM_VALID_INTERRUPT_PRIORITY);

  ASSERT_THAT(errorCode, Eq(InterruptController::ErrorCode::OK));
  ASSERT_THAT(virtualNVIC.IP[RANDOM_MCU_SPECIFIC_IRQ_NUMBER], Eq(expectedIPValue));
}

TEST_F(AnInterruptController, setInterruptProrityInSHPRegisterOfSCBForProcessorExceptions)
{
  const uint8_t expectedSHPValue = RANDOM_VALID_INTERRUPT_PRIORITY << (NUMBER_OF_BITS_IN_UINT8_T - __NVIC_PRIO_BITS);
  expectRegisterSetOnlyOnce(&(virtualSCB.SHP[12u - RANDOM_CPU_EXCEPTION_IRQ_NUMBER]), expectedSHPValue);

  const InterruptController::ErrorCode errorCode =
    virtualInterruptController.setInterruptPriority(RANDOM_CPU_EXCEPTION_IRQ_NUMBER, RANDOM_VALID_INTERRUPT_PRIORITY);

  ASSERT_THAT(errorCode, Eq(InterruptController::ErrorCode::OK));
  ASSERT_THAT(virtualSCB.SHP[12u - RANDOM_CPU_EXCEPTION_IRQ_NUMBER], Eq(expectedSHPValue));
}

TEST_F(AnInterruptController, setInterruptProrityFailsIfIRQNumberIsOutOfAllowedRange)
{
  expectNoRegisterToChange();

  const InterruptController::ErrorCode errorCode =
    virtualInterruptController.setInterruptPriority(RANDOM_INVALID_IRQ_NUMBER, RANDOM_VALID_INTERRUPT_PRIORITY);

  ASSERT_THAT(errorCode, Eq(InterruptController::ErrorCode::IRQ_NUMBER_OUT_OF_RANGE));
}

TEST_F(AnInterruptController, setInterruptProrityFailsIfInterruptPriorityIsOutOfAllowedRange)
{
  expectNoRegisterToChange();

  const InterruptController::ErrorCode errorCode =
    virtualInterruptController.setInterruptPriority(RANDOM_VALID_IRQ_NUMBER, RANDOM_INVALID_INTERRUPT_PRIORITY);

  ASSERT_THAT(errorCode, Eq(InterruptController::ErrorCode::INTERRUPT_PRIORITY_OUT_OF_RANGE));
}

TEST_F(AnInterruptController, enableInterruptSetsCorrespondingBitInISERRegisterOfNVICForMCUSpecificInterrupts)
{
  constexpr uint32_t ISER_INDEX = RANDOM_MCU_SPECIFIC_IRQ_NUMBER / NUMBER_OF_BITS_IN_UINT32_T;
  constexpr uint32_t ISER_BIT_POSITION = RANDOM_MCU_SPECIFIC_IRQ_NUMBER % NUMBER_OF_BITS_IN_UINT32_T;
  const uint32_t expectedISERValue = MemoryUtility<uint32_t>::setBit(NVIC_ISER_RESET_VALUE, ISER_BIT_POSITION);
  expectRegisterSetOnlyOnce(&(virtualNVIC.ISER[ISER_INDEX]), expectedISERValue);

  const InterruptController::ErrorCode errorCode =
    virtualInterruptController.enableInterrupt(RANDOM_MCU_SPECIFIC_IRQ_NUMBER);

  ASSERT_THAT(errorCode, Eq(InterruptController::ErrorCode::OK));
}

TEST_F(AnInterruptController, enableInterruptSucceedForForProcessorExceptionsWithoutChangingAnyRegister)
{
  expectNoRegisterToChange();

  const InterruptController::ErrorCode errorCode =
    virtualInterruptController.enableInterrupt(RANDOM_CPU_EXCEPTION_IRQ_NUMBER);

  ASSERT_THAT(errorCode, Eq(InterruptController::ErrorCode::OK));
}

TEST_F(AnInterruptController, enableInterruptFailsIfInterruptPriorityIsOutOfAllowedRange)
{
  expectNoRegisterToChange();

  const InterruptController::ErrorCode errorCode =
    virtualInterruptController.enableInterrupt(RANDOM_INVALID_IRQ_NUMBER);

  ASSERT_THAT(errorCode, Eq(InterruptController::ErrorCode::IRQ_NUMBER_OUT_OF_RANGE));
}