#include "SystemConfig.h"
#include "MemoryUtility.h"
#include "MemoryAccess.h"
#include "DriverTest.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"


using namespace ::testing;


class ASystemConfig : public DriverTest
{
public:

  //! Based on real reset values for SYSCFG registers
  static constexpr uint32_t SYSCFG_MEMRMP_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t SYSCFG_CFGR1_RESET_VALUE   = 0x7C000001;
  static constexpr uint32_t SYSCFG_EXTICR1_RESET_VALUE = 0x00000000;
  static constexpr uint32_t SYSCFG_EXTICR2_RESET_VALUE = 0x00000000;
  static constexpr uint32_t SYSCFG_EXTICR3_RESET_VALUE = 0x00000000;
  static constexpr uint32_t SYSCFG_EXTICR4_RESET_VALUE = 0x00000000;
  static constexpr uint32_t SYSCFG_SCSR_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t SYSCFG_CFGR2_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t SYSCFG_SWPR_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t SYSCFG_SKR_RESET_VALUE     = 0x00000000;
  static constexpr uint32_t SYSCFG_SWPR2_RESET_VALUE   = 0x00000000;

  SYSCFG_TypeDef virtualSYSCFGPeripheral;
  SystemConfig virtualSystemConfig = SystemConfig(&virtualSYSCFGPeripheral);

  void SetUp() override;
  void TearDown() override;
};

void ASystemConfig::SetUp()
{
  DriverTest::SetUp();

  // set values of virtual SYSCFG peripheral registers to reset values
  virtualSYSCFGPeripheral.MEMRMP    = SYSCFG_MEMRMP_RESET_VALUE;
  virtualSYSCFGPeripheral.CFGR1     = SYSCFG_CFGR1_RESET_VALUE;
  virtualSYSCFGPeripheral.EXTICR[0] = SYSCFG_EXTICR1_RESET_VALUE;
  virtualSYSCFGPeripheral.EXTICR[1] = SYSCFG_EXTICR2_RESET_VALUE;
  virtualSYSCFGPeripheral.EXTICR[2] = SYSCFG_EXTICR3_RESET_VALUE;
  virtualSYSCFGPeripheral.EXTICR[3] = SYSCFG_EXTICR4_RESET_VALUE;
  virtualSYSCFGPeripheral.SCSR      = SYSCFG_SCSR_RESET_VALUE;
  virtualSYSCFGPeripheral.CFGR2     = SYSCFG_CFGR2_RESET_VALUE;
  virtualSYSCFGPeripheral.SWPR      = SYSCFG_SWPR_RESET_VALUE;
  virtualSYSCFGPeripheral.SKR       = SYSCFG_SKR_RESET_VALUE;
  virtualSYSCFGPeripheral.SWPR2     = SYSCFG_SWPR2_RESET_VALUE;
}

void ASystemConfig::TearDown()
{
  DriverTest::TearDown();
}

TEST_F(ASystemConfig, MapGPIOToEXTILineDefinesForGivenEXTILineWithWhichGPIOPortItShouldBeInterconnected)
{
  constexpr uint32_t EXTIX_CONFIG_PER_EXTICR_REG = 4u;
  constexpr uint32_t SYSCFG_EXTICR_REG_IDX =
    static_cast<uint32_t>(SystemConfig::EXTILine::EXTI9) / EXTIX_CONFIG_PER_EXTICR_REG;
  constexpr uint32_t SYSCFG_EXTICR_EXTIX_SIZE = 4u;
  constexpr uint32_t SYSCFG_EXTICR_EXTIX_POSITION =
    (static_cast<uint32_t>(SystemConfig::EXTILine::EXTI9) % SYSCFG_EXTICR_EXTIX_SIZE) * SYSCFG_EXTICR_EXTIX_SIZE;
  constexpr uint32_t EXPECTED_SYSCFG_EXTICR_EXTIX_VALUE = static_cast<uint32_t>(SystemConfig::GPIOPort::GPIOD);
  auto bitValueMatcher =
    BitsHaveValue(SYSCFG_EXTICR_EXTIX_POSITION, SYSCFG_EXTICR_EXTIX_SIZE, EXPECTED_SYSCFG_EXTICR_EXTIX_VALUE);
  expectRegisterSetOnlyOnce(&(virtualSYSCFGPeripheral.EXTICR[SYSCFG_EXTICR_REG_IDX]), bitValueMatcher);

  const SystemConfig::ErrorCode errorCode =
    virtualSystemConfig.mapGPIOToEXTILine(SystemConfig::EXTILine::EXTI9, SystemConfig::GPIOPort::GPIOD);

  ASSERT_THAT(errorCode, Eq(SystemConfig::ErrorCode::OK));
  ASSERT_THAT(virtualSYSCFGPeripheral.EXTICR[SYSCFG_EXTICR_REG_IDX], bitValueMatcher);
}