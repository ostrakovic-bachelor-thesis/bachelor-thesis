#include "USART.h"
#include "MemoryUtility.h"
#include "MemoryAccess.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"


using namespace ::testing;


class AnUSART : public Test
{
public:

  //! Based on real reset values for RCC register (soruce STM32L4R9 reference manual)
  static constexpr uint32_t USART_CR1_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t USART_CR2_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t USART_CR3_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t USART_BRR_RESET_VALUE   = 0x00000400;
  static constexpr uint16_t USART_GTPR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t USART_RTOR_RESET_VALUE  = 0x00000000;
  static constexpr uint16_t USART_RQR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t USART_ISR_RESET_VALUE   = 0x000000C0;
  static constexpr uint32_t USART_ICR_RESET_VALUE   = 0x00000000;
  static constexpr uint16_t USART_RDR_RESET_VALUE   = 0x00000000;
  static constexpr uint16_t USART_TDR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t USART_PRESC_RESET_VALUE = 0x00000000;

  //! Needed to verify statement for setting and getting registers value
  NiceMock<MemoryAccessHook> memoryAccessHook;

  USART_TypeDef virtualUSARTPeripheral;
  USART virtualUSART = USART(&virtualUSARTPeripheral);
  USART::USARTConfig usartConfig;

  uint32_t expectedRegVal(uint32_t initialRegVal, uint32_t position, uint32_t valueSize, uint32_t value);
  void expectRegisterSetOnlyOnce(volatile uint32_t *registerPtr, uint32_t registerValue);
  void expectRegisterNotToChange(volatile uint32_t *registerPtr);
  void expectNoRegisterToChange(void);

  void SetUp() override;
  void TearDown() override;
};

uint32_t AnUSART::expectedRegVal(uint32_t initialRegVal, uint32_t position, uint32_t valueSize, uint32_t value)
{
  const uint32_t startBit = position * valueSize;
  const uint32_t mask = 0xFFFFFFFFu >> (sizeof(uint32_t) * 8u - valueSize);

  return (initialRegVal & ~(mask << startBit)) | ((value & mask) << startBit);
}

void AnUSART::expectRegisterSetOnlyOnce(volatile uint32_t *registerPtr, uint32_t registerValue)
{
  EXPECT_CALL(memoryAccessHook, setRegisterValue(registerPtr, registerValue))
    .Times(1u);
  EXPECT_CALL(memoryAccessHook, setRegisterValue(Not(registerPtr), _))
    .Times(AnyNumber()); 
}

void AnUSART::expectRegisterNotToChange(volatile uint32_t *registerPtr)
{
  EXPECT_CALL(memoryAccessHook, setRegisterValue(registerPtr, _))
    .Times(0u);
  EXPECT_CALL(memoryAccessHook, setRegisterValue(Not(registerPtr), _))
    .Times(AnyNumber()); 
}

void AnUSART::expectNoRegisterToChange(void)
{
  EXPECT_CALL(memoryAccessHook, setRegisterValue(_, _))
    .Times(0u);
}

void AnUSART::SetUp()
{
  // set values of virtual RCC peripheral to reset values
  virtualUSARTPeripheral.CR1   = USART_CR1_RESET_VALUE;
  virtualUSARTPeripheral.CR2   = USART_CR2_RESET_VALUE;
  virtualUSARTPeripheral.CR3   = USART_CR3_RESET_VALUE;
  virtualUSARTPeripheral.BRR   = USART_BRR_RESET_VALUE;
  virtualUSARTPeripheral.GTPR  = USART_GTPR_RESET_VALUE;
  virtualUSARTPeripheral.RTOR  = USART_RTOR_RESET_VALUE;
  virtualUSARTPeripheral.RQR   = USART_RQR_RESET_VALUE;
  virtualUSARTPeripheral.ISR   = USART_ISR_RESET_VALUE;
  virtualUSARTPeripheral.ICR   = USART_ICR_RESET_VALUE;
  virtualUSARTPeripheral.RDR   = USART_RDR_RESET_VALUE;
  virtualUSARTPeripheral.TDR   = USART_TDR_RESET_VALUE;
  virtualUSARTPeripheral.PRESC = USART_PRESC_RESET_VALUE;

  MemoryAccess::setMemoryAccessHook(&memoryAccessHook);
}

void AnUSART::TearDown()
{
  MemoryAccess::setMemoryAccessHook(nullptr);
}


TEST_F(AnUSART, InitSetsWordLengthBitsInCR1AccordingToChoosenWordLength)
{
  constexpr uint32_t USART_CR1_M0_POSITION = 12u;
  constexpr uint32_t USART_CR1_M1_POSITION = 28u;
  constexpr uint32_t EXPECTED_USART_CR1_M0_VALUE = 1u;
  constexpr uint32_t EXPECTED_USART_CR1_M1_VALUE = 0u;
  const uint32_t expectedCR1Value = 
    expectedRegVal(USART_CR1_RESET_VALUE, USART_CR1_M0_POSITION, 1u, EXPECTED_USART_CR1_M0_VALUE) |
    expectedRegVal(USART_CR1_RESET_VALUE, USART_CR1_M1_POSITION, 1u, EXPECTED_USART_CR1_M1_VALUE);
  usartConfig.wordLength = USART::WordLength::BITS_9;
  expectRegisterSetOnlyOnce(&(virtualUSARTPeripheral.CR1), expectedCR1Value);

  const USART::ErrorCode errorCode = virtualUSART.init(usartConfig);

  ASSERT_THAT(errorCode, Eq(USART::ErrorCode::OK));
  ASSERT_THAT(virtualUSARTPeripheral.CR1, Eq(expectedCR1Value));
}

