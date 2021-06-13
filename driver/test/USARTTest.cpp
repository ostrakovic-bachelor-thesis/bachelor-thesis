#include "USART.h"
#include "MemoryUtility.h"
#include "MemoryAccess.h"
#include "DriverTest.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"


using namespace ::testing;


class AnUSART : public DriverTest
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

  USART_TypeDef virtualUSARTPeripheral;
  USART virtualUSART = USART(&virtualUSARTPeripheral);
  USART::USARTConfig usartConfig;

  void SetUp() override;
  void TearDown() override;
};

void AnUSART::SetUp()
{
  DriverTest::SetUp();

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
}

void AnUSART::TearDown()
{
  DriverTest::TearDown();
}


TEST_F(AnUSART, InitSetsWordLengthBitsInCR1AccordingToChoosenWordLength)
{
  constexpr uint32_t USART_CR1_M0_POSITION = 12u;
  constexpr uint32_t USART_CR1_M1_POSITION = 28u;
  constexpr uint32_t EXPECTED_USART_CR1_M0_VALUE = 1u;
  constexpr uint32_t EXPECTED_USART_CR1_M1_VALUE = 0u;
  auto bitValueMatcher = 
    AllOf(BitHasValue(USART_CR1_M0_POSITION, EXPECTED_USART_CR1_M0_VALUE), 
          BitHasValue(USART_CR1_M1_POSITION, EXPECTED_USART_CR1_M1_VALUE));
  usartConfig.wordLength = USART::WordLength::BITS_9;
  expectRegisterSetOnlyOnce(&(virtualUSARTPeripheral.CR1), bitValueMatcher);

  const USART::ErrorCode errorCode = virtualUSART.init(usartConfig);

  ASSERT_THAT(errorCode, Eq(USART::ErrorCode::OK));
  ASSERT_THAT(virtualUSARTPeripheral.CR1, bitValueMatcher);
}

