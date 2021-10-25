
#include "DSIHost.h"
#include "DriverTest.h"
#include "ResetControlMock.h"
#include "MemoryUtility.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"


using namespace ::testing;


class ADSIHost : public DriverTest
{
public:

  //! Based on real reset values for DSI Host registers (source STM32L4R9 reference manual)
  static constexpr uint32_t DSIHOST_VR_RESET_VALUE      = 0x3133302A;
  static constexpr uint32_t DSIHOST_CR_RESET_VALUE      = 0x00000000;
  static constexpr uint32_t DSIHOST_CCR_RESET_VALUE     = 0x00000000;
  static constexpr uint32_t DSIHOST_LVCIDR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t DSIHOST_LCOLCR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t DSIHOST_LPCR_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DSIHOST_LPMCR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t DSIHOST_PCR_RESET_VALUE     = 0x00000000;
  static constexpr uint32_t DSIHOST_GVCIDR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t DSIHOST_MCR_RESET_VALUE     = 0x00000001;
  static constexpr uint32_t DSIHOST_VMCR_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DSIHOST_VPCR_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DSIHOST_VCCR_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DSIHOST_VNPCR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t DSIHOST_VHSACR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t DSIHOST_VHBPCR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t DSIHOST_VLCR_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DSIHOST_VVSACR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t DSIHOST_VVBPCR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t DSIHOST_VVFPCR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t DSIHOST_VVACR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t DSIHOST_LCCR_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DSIHOST_CMCR_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DSIHOST_GHCR_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DSIHOST_GPDR_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DSIHOST_GPSR_RESET_VALUE    = 0x00000015;
  static constexpr uint32_t DSIHOST_TCCRX_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t DSIHOST_CLCR_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DSIHOST_CLTCR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t DSIHOST_DLTCR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t DSIHOST_PCTLR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t DSIHOST_PCONFR_RESET_VALUE  = 0x00000001;
  static constexpr uint32_t DSIHOST_PUCR_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DSIHOST_PTTCR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t DSIHOST_PSR_RESET_VALUE     = 0x00001528;
  static constexpr uint32_t DSIHOST_ISRX_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DSIHOST_IERX_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DSIHOST_FIRX_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DSIHOST_VSCR_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DSIHOST_LCVCIDR_RESET_VALUE = 0x00000000;
  static constexpr uint32_t DSIHOST_LCCCR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t DSIHOST_LPMCCR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t DSIHOST_VMCCR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t DSIHOST_VPCCR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t DSIHOST_VCCCR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t DSIHOST_VNPCCR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t DSIHOST_VHSACCR_RESET_VALUE = 0x00000000;
  static constexpr uint32_t DSIHOST_VHBPCCR_RESET_VALUE = 0x00000000;
  static constexpr uint32_t DSIHOST_VLCCR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t DSIHOST_VVSACCR_RESET_VALUE = 0x00000000;
  static constexpr uint32_t DSIHOST_VVBPCCR_RESET_VALUE = 0x00000000;
  static constexpr uint32_t DSIHOST_VVFPCCR_RESET_VALUE = 0x00000000;
  static constexpr uint32_t DSIHOST_VVACCR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t DSIHOST_WCFGR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t DSIHOST_WCR_RESET_VALUE     = 0x00000000;
  static constexpr uint32_t DSIHOST_WIER_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DSIHOST_WISR_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DSIHOST_WIFCR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t DSIHOST_WPCRX_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t DSIHOST_WRPCR_RESET_VALUE   = 0x00000000;

  DSI_TypeDef virtualDSIHostPeripheral;
  NiceMock<ResetControlMock> resetControlMock;
  DSIHost virtualDSIHost = DSIHost(&virtualDSIHostPeripheral, &resetControlMock);
  DSIHost::DSIHostConfig dsiHostConfig;

  uint32_t m_isDPHYRegulatorReadyCounter;
  uint32_t m_isDPHYPLLReadyCounter;

  void setupWISRRegisterReadings(void);
  void expectDPHYRegulatorToBeReadyBeforeWrittingToAnyBitOfDSIHostPeripheralOtherThanREGENInWRPCR(void);
  void expectDPHYPLLToBeReadyBeforeWrittingToAnyBitOfDSIHostPeripheralOtherThanREGENInWRPCR(void);

  void SetUp() override;
  void TearDown() override;
};

void ADSIHost::SetUp()
{
  DriverTest::SetUp();

  m_isDPHYRegulatorReadyCounter = 0u;
  m_isDPHYPLLReadyCounter       = 0u;

  // based on real reset values of DSI Host registers (source STM32L4R9 reference manual)
  virtualDSIHostPeripheral.VR      = DSIHOST_VR_RESET_VALUE;
  virtualDSIHostPeripheral.CR      = DSIHOST_CR_RESET_VALUE;
  virtualDSIHostPeripheral.CCR     = DSIHOST_CCR_RESET_VALUE;
  virtualDSIHostPeripheral.LVCIDR  = DSIHOST_LVCIDR_RESET_VALUE;
  virtualDSIHostPeripheral.LCOLCR  = DSIHOST_LCOLCR_RESET_VALUE;
  virtualDSIHostPeripheral.LPCR    = DSIHOST_LPCR_RESET_VALUE;
  virtualDSIHostPeripheral.LPMCR   = DSIHOST_LPMCR_RESET_VALUE;
  virtualDSIHostPeripheral.PCR     = DSIHOST_PCR_RESET_VALUE;
  virtualDSIHostPeripheral.GVCIDR  = DSIHOST_GVCIDR_RESET_VALUE;
  virtualDSIHostPeripheral.MCR     = DSIHOST_MCR_RESET_VALUE;
  virtualDSIHostPeripheral.VMCR    = DSIHOST_VMCR_RESET_VALUE;
  virtualDSIHostPeripheral.VPCR    = DSIHOST_VPCR_RESET_VALUE;
  virtualDSIHostPeripheral.VCCR    = DSIHOST_VCCR_RESET_VALUE;
  virtualDSIHostPeripheral.VNPCR   = DSIHOST_VNPCR_RESET_VALUE;
  virtualDSIHostPeripheral.VHSACR  = DSIHOST_VHSACR_RESET_VALUE;
  virtualDSIHostPeripheral.VHBPCR  = DSIHOST_VHBPCR_RESET_VALUE;
  virtualDSIHostPeripheral.VLCR    = DSIHOST_VLCR_RESET_VALUE;
  virtualDSIHostPeripheral.VVSACR  = DSIHOST_VVSACR_RESET_VALUE;
  virtualDSIHostPeripheral.VVBPCR  = DSIHOST_VVBPCR_RESET_VALUE;
  virtualDSIHostPeripheral.VVFPCR  = DSIHOST_VVFPCR_RESET_VALUE;
  virtualDSIHostPeripheral.VVACR   = DSIHOST_VVACR_RESET_VALUE;
  virtualDSIHostPeripheral.LCCR    = DSIHOST_LCCR_RESET_VALUE;
  virtualDSIHostPeripheral.CMCR    = DSIHOST_CMCR_RESET_VALUE;
  virtualDSIHostPeripheral.GHCR    = DSIHOST_GHCR_RESET_VALUE;
  virtualDSIHostPeripheral.GPDR    = DSIHOST_GPDR_RESET_VALUE;
  virtualDSIHostPeripheral.GPSR    = DSIHOST_GPSR_RESET_VALUE;
  virtualDSIHostPeripheral.TCCR[0] = DSIHOST_TCCRX_RESET_VALUE;
  virtualDSIHostPeripheral.TCCR[1] = DSIHOST_TCCRX_RESET_VALUE;
  virtualDSIHostPeripheral.TCCR[2] = DSIHOST_TCCRX_RESET_VALUE;
  virtualDSIHostPeripheral.TCCR[3] = DSIHOST_TCCRX_RESET_VALUE;
  virtualDSIHostPeripheral.TCCR[4] = DSIHOST_TCCRX_RESET_VALUE;
  virtualDSIHostPeripheral.TCCR[5] = DSIHOST_TCCRX_RESET_VALUE;
  virtualDSIHostPeripheral.CLCR    = DSIHOST_CLCR_RESET_VALUE;
  virtualDSIHostPeripheral.CLTCR   = DSIHOST_CLTCR_RESET_VALUE;
  virtualDSIHostPeripheral.DLTCR   = DSIHOST_DLTCR_RESET_VALUE;
  virtualDSIHostPeripheral.PCTLR   = DSIHOST_PCTLR_RESET_VALUE;
  virtualDSIHostPeripheral.PCONFR  = DSIHOST_PCONFR_RESET_VALUE;
  virtualDSIHostPeripheral.PUCR    = DSIHOST_PUCR_RESET_VALUE;
  virtualDSIHostPeripheral.PTTCR   = DSIHOST_PTTCR_RESET_VALUE;
  virtualDSIHostPeripheral.PSR     = DSIHOST_PSR_RESET_VALUE;
  virtualDSIHostPeripheral.ISR[0]  = DSIHOST_ISRX_RESET_VALUE;
  virtualDSIHostPeripheral.ISR[1]  = DSIHOST_ISRX_RESET_VALUE;
  virtualDSIHostPeripheral.IER[0]  = DSIHOST_IERX_RESET_VALUE;
  virtualDSIHostPeripheral.IER[1]  = DSIHOST_IERX_RESET_VALUE;
  virtualDSIHostPeripheral.FIR[0]  = DSIHOST_FIRX_RESET_VALUE;
  virtualDSIHostPeripheral.FIR[1]  = DSIHOST_FIRX_RESET_VALUE;
  virtualDSIHostPeripheral.VSCR    = DSIHOST_VSCR_RESET_VALUE;
  virtualDSIHostPeripheral.LCVCIDR = DSIHOST_LCVCIDR_RESET_VALUE;
  virtualDSIHostPeripheral.LCCCR   = DSIHOST_LCCCR_RESET_VALUE;
  virtualDSIHostPeripheral.LPMCCR  = DSIHOST_LPMCCR_RESET_VALUE;
  virtualDSIHostPeripheral.VMCCR   = DSIHOST_VMCCR_RESET_VALUE;
  virtualDSIHostPeripheral.VPCCR   = DSIHOST_VPCCR_RESET_VALUE;
  virtualDSIHostPeripheral.VCCCR   = DSIHOST_VCCCR_RESET_VALUE;
  virtualDSIHostPeripheral.VNPCCR  = DSIHOST_VNPCCR_RESET_VALUE;
  virtualDSIHostPeripheral.VHSACCR = DSIHOST_VHSACCR_RESET_VALUE;
  virtualDSIHostPeripheral.VHBPCCR = DSIHOST_VHBPCCR_RESET_VALUE;
  virtualDSIHostPeripheral.VLCCR   = DSIHOST_VLCCR_RESET_VALUE;
  virtualDSIHostPeripheral.VVSACCR = DSIHOST_VVSACCR_RESET_VALUE;
  virtualDSIHostPeripheral.VVBPCCR = DSIHOST_VVBPCCR_RESET_VALUE;
  virtualDSIHostPeripheral.VVFPCCR = DSIHOST_VVFPCCR_RESET_VALUE;
  virtualDSIHostPeripheral.VVACCR  = DSIHOST_VVACCR_RESET_VALUE;
  virtualDSIHostPeripheral.WCFGR   = DSIHOST_WCFGR_RESET_VALUE;
  virtualDSIHostPeripheral.WCR     = DSIHOST_WCR_RESET_VALUE;
  virtualDSIHostPeripheral.WIER    = DSIHOST_WIER_RESET_VALUE;
  virtualDSIHostPeripheral.WISR    = DSIHOST_WISR_RESET_VALUE;
  virtualDSIHostPeripheral.WIFCR   = DSIHOST_WIFCR_RESET_VALUE;
  virtualDSIHostPeripheral.WPCR[0] = DSIHOST_WPCRX_RESET_VALUE;
  virtualDSIHostPeripheral.WPCR[1] = DSIHOST_WPCRX_RESET_VALUE;
  virtualDSIHostPeripheral.WPCR[2] = DSIHOST_WPCRX_RESET_VALUE;
  virtualDSIHostPeripheral.WPCR[3] = DSIHOST_WPCRX_RESET_VALUE;
  virtualDSIHostPeripheral.WPCR[4] = DSIHOST_WPCRX_RESET_VALUE;
  virtualDSIHostPeripheral.WRPCR   = DSIHOST_WRPCR_RESET_VALUE;

  setupWISRRegisterReadings();
}

void ADSIHost::TearDown()
{
  DriverTest::TearDown();
}

void ADSIHost::setupWISRRegisterReadings(void)
{
  ON_CALL(memoryAccessHook, getRegisterValue(&(virtualDSIHostPeripheral.WISR)))
    .WillByDefault([&](volatile const uint32_t *registerPtr)
    {
      constexpr uint32_t DSIHOST_WISR_RRS_POSITION = 12u;
      constexpr uint32_t DSIHOST_WISR_PLLLS_POSITION = 8u;
      const bool isDPHYRegulatorReady = (10u < m_isDPHYRegulatorReadyCounter);
      const bool isDPHYPLLReady = (30u < m_isDPHYPLLReadyCounter);

      ++m_isDPHYRegulatorReadyCounter;
      ++m_isDPHYPLLReadyCounter;

      virtualDSIHostPeripheral.WISR =
        expectedRegVal(virtualDSIHostPeripheral.WISR, DSIHOST_WISR_RRS_POSITION, 1u, isDPHYRegulatorReady);
      virtualDSIHostPeripheral.WISR =
        expectedRegVal(virtualDSIHostPeripheral.WISR, DSIHOST_WISR_PLLLS_POSITION, 1u, isDPHYPLLReady);

      return virtualDSIHostPeripheral.WISR;
    });
}

void ADSIHost::expectDPHYRegulatorToBeReadyBeforeWrittingToAnyBitOfDSIHostPeripheralOtherThanREGENInWRPCR(void)
{
  EXPECT_CALL(memoryAccessHook, setRegisterValue(_, Matcher<uint32_t>(_)))
    .WillRepeatedly([&](volatile void *registerPtr, uint32_t registerValue)
    {
      constexpr uint32_t DSIHOST_WRPCR_REGEN_POSITION = 24u;
      constexpr uint32_t DSIHOST_WISR_RRS_POSITION = 12u;
      constexpr uint32_t EXPECTED_DSIHOST_WISR_RRS_VALUE = 0x1;

      if ((registerPtr != &(virtualDSIHostPeripheral.WRPCR)) ||
          ((registerPtr == &(virtualDSIHostPeripheral.WRPCR)) &&
           not MemoryUtility<uint32_t>::isBitSet((virtualDSIHostPeripheral.WRPCR ^ registerValue), DSIHOST_WRPCR_REGEN_POSITION)))
      {
        auto bitValueMatcher = BitHasValue(DSIHOST_WISR_RRS_POSITION, EXPECTED_DSIHOST_WISR_RRS_VALUE);

        ASSERT_THAT(virtualDSIHostPeripheral.WISR, bitValueMatcher);
      }
    });
}

void ADSIHost::expectDPHYPLLToBeReadyBeforeWrittingToAnyBitOfDSIHostPeripheralOtherThanREGENInWRPCR(void)
{
  EXPECT_CALL(memoryAccessHook, setRegisterValue(_, Matcher<uint32_t>(_)))
    .WillRepeatedly([&](volatile void *registerPtr, uint32_t registerValue)
    {
      constexpr uint32_t DSIHOST_WISR_PLLLS_POSITION = 8u;
      constexpr uint32_t EXPECTED_DSIHOST_WISR_PLLLS_VALUE = 0x1;

      if (registerPtr != &(virtualDSIHostPeripheral.WRPCR))
      {
        auto bitValueMatcher = BitHasValue(DSIHOST_WISR_PLLLS_POSITION, EXPECTED_DSIHOST_WISR_PLLLS_VALUE);

        ASSERT_THAT(virtualDSIHostPeripheral.WISR, bitValueMatcher);
      }
    });
}


TEST_F(ADSIHost, GetPeripheralTagReturnsPointerToUnderlayingDSIHostPeripheralCastedToPeripheralType)
{
  ASSERT_THAT(virtualDSIHost.getPeripheralTag(),
    Eq(static_cast<Peripheral>(reinterpret_cast<uintptr_t>(&virtualDSIHostPeripheral))));
}

TEST_F(ADSIHost, InitTurnsOnDSIHostPeripheralClock)
{
  resetControlMock.setReturnErrorCode(ResetControl::ErrorCode::OK);
  EXPECT_CALL(resetControlMock, enablePeripheralClock(virtualDSIHost.getPeripheralTag()))
    .Times(1u);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
}

TEST_F(ADSIHost, InitFailsIfTurningOnOfDSIHostPeripheralClockFail)
{
  resetControlMock.setReturnErrorCode(ResetControl::ErrorCode::INTERNAL);
  EXPECT_CALL(resetControlMock, enablePeripheralClock(virtualDSIHost.getPeripheralTag()))
    .Times(1u);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::CAN_NOT_TURN_ON_PERIPHERAL_CLOCK));
}

TEST_F(ADSIHost, InitRequestsEnablingOfDPHYRegulatorBySettingToOneREGENBitInWRPCRRegister)
{
  constexpr uint32_t DSIHOST_WRPCR_REGEN_POSITION = 24u;
  constexpr uint32_t EXPECTED_DSIHOST_WRPCR_REGEN_VALUE = 1u;
  auto bitValueMatcher = BitHasValue(DSIHOST_WRPCR_REGEN_POSITION, EXPECTED_DSIHOST_WRPCR_REGEN_VALUE);
  expectSpecificRegisterSetToBeCalledFirst(&(virtualDSIHostPeripheral.WRPCR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.WRPCR, bitValueMatcher);
}

TEST_F(ADSIHost, InitWillWaitUpUntilDPHYRegulatorBecomesReadyBeforeProceedingFurther)
{
  expectDPHYRegulatorToBeReadyBeforeWrittingToAnyBitOfDSIHostPeripheralOtherThanREGENInWRPCR();

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
}

TEST_F(ADSIHost, InitSetsIDFBitsInWRPCRRegisterAccordingToPLLInputClockDivider)
{
  constexpr uint8_t DPHY_PLL_INPUT_CLOCK_DIVIDER = 3u;
  constexpr uint32_t DSIHOST_WRPCR_IDF_POSITION = 11u;
  constexpr uint32_t DSIHOST_WRPCR_IDF_SIZE     = 4u;
  constexpr uint32_t EXPECTED_DSIHOST_WRPCR_IDF_VALUE = DPHY_PLL_INPUT_CLOCK_DIVIDER;
  auto bitsValueMatcher =
    BitsHaveValue(DSIHOST_WRPCR_IDF_POSITION, DSIHOST_WRPCR_IDF_SIZE, EXPECTED_DSIHOST_WRPCR_IDF_VALUE);
  dsiHostConfig.pllConfig.inputClockDivider = DPHY_PLL_INPUT_CLOCK_DIVIDER;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.WRPCR), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.WRPCR, bitsValueMatcher);
}

TEST_F(ADSIHost, InitSetsNDIVBitsInWRPCRRegisterAccordingToPLLInputClockMultiplier)
{
  constexpr uint8_t DPHY_PLL_INPUT_CLOCK_MULTIPLIER = 125u;
  constexpr uint32_t DSIHOST_WRPCR_NDIV_POSITION = 2u;
  constexpr uint32_t DSIHOST_WRPCR_NDIV_SIZE     = 7u;
  constexpr uint32_t EXPECTED_DSIHOST_WRPCR_NDIV_VALUE = DPHY_PLL_INPUT_CLOCK_MULTIPLIER;
  auto bitsValueMatcher =
    BitsHaveValue(DSIHOST_WRPCR_NDIV_POSITION, DSIHOST_WRPCR_NDIV_SIZE, EXPECTED_DSIHOST_WRPCR_NDIV_VALUE);
  dsiHostConfig.pllConfig.inputClockMultiplier = DPHY_PLL_INPUT_CLOCK_MULTIPLIER;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.WRPCR), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.WRPCR, bitsValueMatcher);
}

TEST_F(ADSIHost, InitSetsODFBitsInWRPCRRegisterAccordingToPLLOutputClockDivider)
{
  constexpr uint8_t DPHY_PLL_OUTPUT_CLOCK_DIVIDER = 4u;
  constexpr uint32_t DSIHOST_WRPCR_ODF_POSITION = 16u;
  constexpr uint32_t DSIHOST_WRPCR_ODF_SIZE     = 2u;
  constexpr uint32_t EXPECTED_DSIHOST_WRPCR_ODF_VALUE = (DPHY_PLL_OUTPUT_CLOCK_DIVIDER + 1u) * 2u / 5u;
  auto bitsValueMatcher =
    BitsHaveValue(DSIHOST_WRPCR_ODF_POSITION, DSIHOST_WRPCR_ODF_SIZE, EXPECTED_DSIHOST_WRPCR_ODF_VALUE);
  dsiHostConfig.pllConfig.outputClockDivider = DPHY_PLL_OUTPUT_CLOCK_DIVIDER;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.WRPCR), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.WRPCR, bitsValueMatcher);
}

TEST_F(ADSIHost, InitRequestsEnablingOfDPHYPLLBySettingToOnePLLENBitInWRPCRRegister)
{
  constexpr uint32_t DSIHOST_WRPCR_PLLEN_POSITION = 0u;
  constexpr uint32_t EXPECTED_DSIHOST_WRPCR_PLLEN_VALUE = 1u;
  auto bitValueMatcher = BitHasValue(DSIHOST_WRPCR_PLLEN_POSITION, EXPECTED_DSIHOST_WRPCR_PLLEN_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.WRPCR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.WRPCR, bitValueMatcher);
}

TEST_F(ADSIHost, InitWillWaitUpUntilDPHYPLLBecomesReadyBeforeProceedingFurther)
{
  expectDPHYPLLToBeReadyBeforeWrittingToAnyBitOfDSIHostPeripheralOtherThanREGENInWRPCR();

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
}

TEST_F(ADSIHost, InitSetsNLBitsInPCONFRRegisterAccordingToNumberOfDataLanes)
{
  constexpr uint8_t DSI_NUMBER_OF_DATA_LANES = 1u;
  constexpr uint32_t DSIHOST_PCONFR_NL_POSITION = 0u;
  constexpr uint32_t DSIHOST_PCONFR_NL_SIZE     = 2u;
  constexpr uint32_t EXPECTED_DSIHOST_PCONFR_NL_VALUE = DSI_NUMBER_OF_DATA_LANES - 1u;
  auto bitsValueMatcher =
    BitsHaveValue(DSIHOST_PCONFR_NL_POSITION, DSIHOST_PCONFR_NL_SIZE, EXPECTED_DSIHOST_PCONFR_NL_VALUE);
  dsiHostConfig.numberOfDataLanes = DSI_NUMBER_OF_DATA_LANES;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.PCONFR), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.PCONFR, bitsValueMatcher);
}

TEST_F(ADSIHost, InitEnablesDPHYClockLaneModuleBySettingCKEBitInPCTLRRegister)
{
  constexpr uint32_t DSIHOST_PCTLR_CKE_POSITION = 2u;
  constexpr uint32_t EXPECTED_DSIHOST_PCTLR_CKE_VALUE = 1u;
  auto bitValueMatcher = BitHasValue(DSIHOST_PCTLR_CKE_POSITION, EXPECTED_DSIHOST_PCTLR_CKE_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.PCTLR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.PCTLR, bitValueMatcher);
}

TEST_F(ADSIHost, InitEnablesDPHYDigitalSectionBySettingDENBitInPCTLRRegister)
{
  constexpr uint32_t DSIHOST_PCTLR_DEN_POSITION = 1u;
  constexpr uint32_t EXPECTED_DSIHOST_PCTLR_DEN_VALUE = 1u;
  auto bitValueMatcher = BitHasValue(DSIHOST_PCTLR_DEN_POSITION, EXPECTED_DSIHOST_PCTLR_DEN_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.PCTLR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.PCTLR, bitValueMatcher);
}

TEST_F(ADSIHost, InitDisablesAutomaticClockLaneControlBySettingToZeroACRBitInCLCRRegister)
{
  constexpr uint32_t DSIHOST_CLCR_ACR_POSITION = 1u;
  constexpr uint32_t EXPECTED_DSIHOST_CLCR_ACR_VALUE = 0u;
  auto bitValueMatcher = BitHasValue(DSIHOST_CLCR_ACR_POSITION, EXPECTED_DSIHOST_CLCR_ACR_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.CLCR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.CLCR, bitValueMatcher);
}

TEST_F(ADSIHost, InitSetsClockLaneRunningModeToHighSpeedBySettingToOneDPCCBitInCLCRRegister)
{
  constexpr uint32_t DSIHOST_CLCR_DPCC_POSITION = 0u;
  constexpr uint32_t EXPECTED_DSIHOST_CLCR_DPCC_VALUE = 1u;
  auto bitValueMatcher = BitHasValue(DSIHOST_CLCR_DPCC_POSITION, EXPECTED_DSIHOST_CLCR_DPCC_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.CLCR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.CLCR, bitValueMatcher);
}