#include "EXTI.h"
#include "MemoryUtility.h"
#include "MemoryAccess.h"
#include "DriverTest.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"


using namespace ::testing;


class AnEXTI : public DriverTest
{
public:

  //! Based on real reset values for EXTI registers
  static constexpr uint32_t EXTI_IMR1_RESET_VALUE   = 0xFF820000;
  static constexpr uint32_t EXTI_EMR1_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t EXTI_RTSR1_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t EXTI_FTSR1_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t EXTI_SWIER1_RESET_VALUE = 0x00000000;
  static constexpr uint32_t EXTI_PR1_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t EXTI_IMR2_RESET_VALUE   = 0x00000187;
  static constexpr uint32_t EXTI_EMR2_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t EXTI_RTSR2_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t EXTI_FTSR2_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t EXTI_SWIER2_RESET_VALUE = 0x00000000;
  static constexpr uint32_t EXTI_PR2_RESET_VALUE    = 0x00000000;

  EXTI_TypeDef virtualEXTIPeripheral;
  EXTI virtualEXTI= EXTI(&virtualEXTIPeripheral);
  EXTI::EXTIConfig extiConfig;

  void SetUp() override;
  void TearDown() override;
};

void AnEXTI::SetUp()
{
  DriverTest::SetUp();

  // set values of virtual EXTI peripheral registers to reset values
  virtualEXTIPeripheral.IMR1   = EXTI_IMR1_RESET_VALUE;
  virtualEXTIPeripheral.EMR1   = EXTI_EMR1_RESET_VALUE;
  virtualEXTIPeripheral.RTSR1  = EXTI_RTSR1_RESET_VALUE;
  virtualEXTIPeripheral.FTSR1  = EXTI_FTSR1_RESET_VALUE;
  virtualEXTIPeripheral.SWIER1 = EXTI_SWIER1_RESET_VALUE;
  virtualEXTIPeripheral.PR1    = EXTI_PR1_RESET_VALUE;
  virtualEXTIPeripheral.IMR2   = EXTI_IMR2_RESET_VALUE;
  virtualEXTIPeripheral.EMR2   = EXTI_EMR2_RESET_VALUE;
  virtualEXTIPeripheral.RTSR2  = EXTI_RTSR2_RESET_VALUE;
  virtualEXTIPeripheral.FTSR2  = EXTI_FTSR2_RESET_VALUE;
  virtualEXTIPeripheral.SWIER2 = EXTI_SWIER2_RESET_VALUE;
  virtualEXTIPeripheral.PR2    = EXTI_PR2_RESET_VALUE;
}

void AnEXTI::TearDown()
{
  DriverTest::TearDown();
}


TEST_F(AnEXTI, ConfigureEXTILineSetsIMRxRegisterValueAccordingToIsInterruptMaskedConfigurationValue)
{
  constexpr uint32_t EXTI_IMR1_IMX_POSITION = static_cast<uint32_t>(EXTI::EXTILine::LINE2);
  constexpr uint32_t EXPECTED_EXTI_IMR1_IMX_VALUE = 1u;
  extiConfig.isInterruptMasked = false;
  auto bitValueMatcher =
    BitHasValue(EXTI_IMR1_IMX_POSITION, EXPECTED_EXTI_IMR1_IMX_VALUE);
  expectSpecificRegisterSetToBeCalledFirst(&(virtualEXTIPeripheral.IMR1), bitValueMatcher);

  const EXTI::ErrorCode errorCode = virtualEXTI.configureEXITLine(EXTI::EXTILine::LINE2, extiConfig);

  ASSERT_THAT(errorCode, Eq(EXTI::ErrorCode::OK));
}