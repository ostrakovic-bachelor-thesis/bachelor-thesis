
#include "DSIHost.h"
#include "DriverTest.h"
#include "ClockControlMock.h"
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

  static constexpr uint32_t NANOSECONDS_IN_MICROSECOND = 1000u;
  static constexpr uint32_t NANOSECONDS_IN_SECOND      = 1000000000u;
  static constexpr uint32_t BITS_IN_BYTE               = 8u;
  static constexpr uint32_t FREQ_HZ_TO_MHZ_DIVIDER     = 1000000u;

  static constexpr DSIHost::VirtualChannelID RANDOM_VIRTUAL_CHANNEL_ID = DSIHost::VirtualChannelID::CHANNEL_3;
  static constexpr uint8_t RANDOM_PARAM_VALUE_1 = 0x8F;
  static constexpr uint8_t RANDOM_PARAM_VALUE_2 = 0x3A;
  static constexpr uint8_t RANDOM_DCS_COMMAND   = 0x11;

  DSI_TypeDef virtualDSIHostPeripheral;
  NiceMock<ClockControlMock> clockControlMock;
  NiceMock<ResetControlMock> resetControlMock;
  DSIHost virtualDSIHost = DSIHost(&virtualDSIHostPeripheral, &clockControlMock, &resetControlMock);
  DSIHost::DSIHostConfig dsiHostConfig;

  uint32_t m_isDPHYRegulatorReadyCounter;
  uint32_t m_isDPHYPLLReadyCounter;

  void setHSEClockFrequency(uint32_t clockFrequency);
  void setGetHSEClockFrequencyErrorCode(ClockControl::ErrorCode errorCode);
  void setDSIPHYClockFrequencyTo800MHz(void);
  void setDSIPHYClockFrequencyTo500MHz(void);
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

  dsiHostConfig.pllConfig.inputClockDivider    = 2u;
  dsiHostConfig.pllConfig.inputClockMultiplier = 100u;
  dsiHostConfig.pllConfig.outputClockDivider   = 1u;

  setupWISRRegisterReadings();
}

void ADSIHost::TearDown()
{
  DriverTest::TearDown();
}

void ADSIHost::setHSEClockFrequency(uint32_t clockFrequency)
{
  clockControlMock.setReturnClockFrequency(clockFrequency);
}

void ADSIHost::setGetHSEClockFrequencyErrorCode(ClockControl::ErrorCode errorCode)
{
  clockControlMock.setReturnErrorCode(errorCode);
}

void ADSIHost::setDSIPHYClockFrequencyTo800MHz(void)
{
  constexpr uint32_t HSE_CLOCK_FREQ = 16000000u;
  setHSEClockFrequency(HSE_CLOCK_FREQ);
  constexpr uint32_t DPHY_PLL_INPUT_CLOCK_DIVIDER    = 2u;
  constexpr uint32_t DPHY_PLL_INPUT_CLOCK_MULTIPLIER = 100u;
  constexpr uint32_t DPHY_PLL_OUTPUT_CLOCK_DIVIDER   = 1u;

  dsiHostConfig.pllConfig.inputClockDivider    = DPHY_PLL_INPUT_CLOCK_DIVIDER;
  dsiHostConfig.pllConfig.inputClockMultiplier = DPHY_PLL_INPUT_CLOCK_MULTIPLIER;
  dsiHostConfig.pllConfig.outputClockDivider   = DPHY_PLL_OUTPUT_CLOCK_DIVIDER;
}

void ADSIHost::setDSIPHYClockFrequencyTo500MHz(void)
{
  constexpr uint32_t HSE_CLOCK_FREQ = 16000000u;
  setHSEClockFrequency(HSE_CLOCK_FREQ);
  constexpr uint32_t DPHY_PLL_INPUT_CLOCK_DIVIDER    = 4u;
  constexpr uint32_t DPHY_PLL_INPUT_CLOCK_MULTIPLIER = 125u;
  constexpr uint32_t DPHY_PLL_OUTPUT_CLOCK_DIVIDER   = 1u;

  dsiHostConfig.pllConfig.inputClockDivider    = DPHY_PLL_INPUT_CLOCK_DIVIDER;
  dsiHostConfig.pllConfig.inputClockMultiplier = DPHY_PLL_INPUT_CLOCK_MULTIPLIER;
  dsiHostConfig.pllConfig.outputClockDivider   = DPHY_PLL_OUTPUT_CLOCK_DIVIDER;
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

TEST_F(ADSIHost, InitSetsUIX4BitsInWPCR0RegisterAccordingToHighSpeedModeClockFrequency)
{
  constexpr uint32_t HIGH_SPEED_MODE_CLOCK_FREQ = 500000000u;
  setDSIPHYClockFrequencyTo500MHz();
  constexpr uint32_t DSIHOST_WPCR0_UIX4_POSITION = 0u;
  constexpr uint32_t DSIHOST_WPCR0_UIX4_SIZE     = 6u;
  constexpr uint32_t EXPECTED_DSIHOST_WPCR0_UIX4_VALUE
    = 4 * NANOSECONDS_IN_MICROSECOND / (HIGH_SPEED_MODE_CLOCK_FREQ / FREQ_HZ_TO_MHZ_DIVIDER);
  auto bitsValueMatcher =
    BitsHaveValue(DSIHOST_WPCR0_UIX4_POSITION, DSIHOST_WPCR0_UIX4_SIZE, EXPECTED_DSIHOST_WPCR0_UIX4_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.WPCR[0]), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.WPCR[0], bitsValueMatcher);
}

TEST_F(ADSIHost, InitSetsTXECKDIVBitsInCCRRegisterToAchieveTxEscapeClockFrequencyTheClosestPossibleTo20MHz)
{
  constexpr uint32_t HIGH_SPEED_MODE_CLOCK_FREQ = 500000000u;
  setDSIPHYClockFrequencyTo500MHz();
  constexpr uint32_t TARGET_FREQ_MHZ = 20u;
  constexpr uint32_t DSIHOST_CCR_TXECKDIV_POSITION = 0u;
  constexpr uint32_t DSIHOST_CCR_TXECKDIV_SIZE     = 8u;
  constexpr uint32_t EXPECTED_DSIHOST_CCR_TXECKDIV_VALUE
    = (((HIGH_SPEED_MODE_CLOCK_FREQ / FREQ_HZ_TO_MHZ_DIVIDER) / BITS_IN_BYTE) + TARGET_FREQ_MHZ - 1u) / TARGET_FREQ_MHZ;
  auto bitsValueMatcher =
    BitsHaveValue(DSIHOST_CCR_TXECKDIV_POSITION, DSIHOST_CCR_TXECKDIV_SIZE, EXPECTED_DSIHOST_CCR_TXECKDIV_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.CCR), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.CCR, bitsValueMatcher);
}

TEST_F(ADSIHost, GetDSIPHYClockFrequencyReturnsFrequencyValueAccordingToPLLConfigParamsForwardedToInitFunction)
{
  constexpr uint32_t HSE_CLOCK_FREQ = 16000000u;
  setHSEClockFrequency(HSE_CLOCK_FREQ);
  constexpr uint32_t DPHY_PLL_INPUT_CLOCK_DIVIDER    = 4u;
  constexpr uint32_t DPHY_PLL_INPUT_CLOCK_MULTIPLIER = 125u;
  constexpr uint32_t DPHY_PLL_OUTPUT_CLOCK_DIVIDER   = 1u;
  constexpr uint32_t EXPECTED_DSY_PHY_CLOCK_FREQ =
    ((HSE_CLOCK_FREQ / DPHY_PLL_INPUT_CLOCK_DIVIDER) * DPHY_PLL_INPUT_CLOCK_MULTIPLIER) * DPHY_PLL_OUTPUT_CLOCK_DIVIDER;
  dsiHostConfig.pllConfig.inputClockDivider    = DPHY_PLL_INPUT_CLOCK_DIVIDER;
  dsiHostConfig.pllConfig.inputClockMultiplier = DPHY_PLL_INPUT_CLOCK_MULTIPLIER;
  dsiHostConfig.pllConfig.outputClockDivider   = DPHY_PLL_OUTPUT_CLOCK_DIVIDER;
  virtualDSIHost.init(dsiHostConfig);
  expectNoRegisterToChange();

  uint32_t dsiPhyClockFreq;
  const DSIHost::ErrorCode errorCode = virtualDSIHost.getDSIPHYClockFrequency(dsiPhyClockFreq);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(dsiPhyClockFreq, EXPECTED_DSY_PHY_CLOCK_FREQ);
}

TEST_F(ADSIHost, InitFailsIfGettingFrequencyOfHSEClockFails)
{
  setGetHSEClockFrequencyErrorCode(ClockControl::ErrorCode::INVALID_CLOCK_SOURCE);
  expectNoRegisterToChange();

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::CAN_NOT_GET_HSE_CLOCK_FREQUENCY));
}

TEST_F(ADSIHost, InitSetsHS2LPTIMEBitsInCLTCRRegisterAccordingToClockLaneHighSpeedToLowPowerTime)
{
  constexpr uint32_t DSI_PHY_PLL_CLOCK_FREQ = 800000000u;
  setDSIPHYClockFrequencyTo800MHz();
  constexpr uint32_t CLOCK_LANE_HIGH_SPEED_TO_LOW_POWER_TIME = 200u;
  constexpr uint32_t DSIHOST_CLTCR_HS2LP_TIME_POSITION = 16u;
  constexpr uint32_t DSIHOST_CLTCR_HS2LP_TIME_SIZE     = 10u;
  constexpr uint32_t EXPECTED_DSIHOST_CLTCR_HS2LP_TIME_VALUE =
    CLOCK_LANE_HIGH_SPEED_TO_LOW_POWER_TIME / (NANOSECONDS_IN_SECOND / (DSI_PHY_PLL_CLOCK_FREQ / BITS_IN_BYTE));
  auto bitsValueMatcher = BitsHaveValue(DSIHOST_CLTCR_HS2LP_TIME_POSITION,
    DSIHOST_CLTCR_HS2LP_TIME_SIZE,
    EXPECTED_DSIHOST_CLTCR_HS2LP_TIME_VALUE);
  dsiHostConfig.dsiPhyTiming.clockLaneHighSpeedToLowPowerTime = CLOCK_LANE_HIGH_SPEED_TO_LOW_POWER_TIME;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.CLTCR), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.CLTCR, bitsValueMatcher);
}

TEST_F(ADSIHost, InitSetsLP2HSTIMEBitsInCLTCRRegisterAccordingToClockLaneLowPowerToHighSpeedTime)
{
  constexpr uint32_t DSI_PHY_PLL_CLOCK_FREQ = 800000000u;
  setDSIPHYClockFrequencyTo800MHz();
  constexpr uint32_t CLOCK_LANE_LOW_POWER_TO_HIGH_SPEED_TIME = 180u;
  constexpr uint32_t DSIHOST_CLTCR_LP2HS_TIME_POSITION = 0u;
  constexpr uint32_t DSIHOST_CLTCR_LP2HS_TIME_SIZE     = 10u;
  constexpr uint32_t EXPECTED_DSIHOST_CLTCR_LP2HS_TIME_VALUE =
    CLOCK_LANE_LOW_POWER_TO_HIGH_SPEED_TIME / (NANOSECONDS_IN_SECOND / (DSI_PHY_PLL_CLOCK_FREQ / BITS_IN_BYTE));
  auto bitsValueMatcher = BitsHaveValue(DSIHOST_CLTCR_LP2HS_TIME_POSITION,
    DSIHOST_CLTCR_LP2HS_TIME_SIZE,
    EXPECTED_DSIHOST_CLTCR_LP2HS_TIME_VALUE);
  dsiHostConfig.dsiPhyTiming.clockLaneLowPowerToHighSpeedTime = CLOCK_LANE_LOW_POWER_TO_HIGH_SPEED_TIME;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.CLTCR), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.CLTCR, bitsValueMatcher);
}

TEST_F(ADSIHost, InitSetsHS2LPTIMEBitsInDLTCRRegisterAccordingToDataLaneHighSpeedToLowPowerTime)
{
  constexpr uint32_t DSI_PHY_PLL_CLOCK_FREQ = 800000000u;
  setDSIPHYClockFrequencyTo800MHz();
  constexpr uint32_t DATA_LANE_HIGH_SPEED_TO_LOW_POWER_TIME = 120u;
  constexpr uint32_t DSIHOST_DLTCR_HS2LP_TIME_POSITION = 24u;
  constexpr uint32_t DSIHOST_DLTCR_HS2LP_TIME_SIZE     = 8u;
  constexpr uint32_t EXPECTED_DSIHOST_DLTCR_HS2LP_TIME_VALUE =
    DATA_LANE_HIGH_SPEED_TO_LOW_POWER_TIME / (NANOSECONDS_IN_SECOND / (DSI_PHY_PLL_CLOCK_FREQ / BITS_IN_BYTE));
  auto bitsValueMatcher = BitsHaveValue(DSIHOST_DLTCR_HS2LP_TIME_POSITION,
    DSIHOST_DLTCR_HS2LP_TIME_SIZE,
    EXPECTED_DSIHOST_DLTCR_HS2LP_TIME_VALUE);
  dsiHostConfig.dsiPhyTiming.dataLaneHighSpeedToLowPowerTime = DATA_LANE_HIGH_SPEED_TO_LOW_POWER_TIME;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.DLTCR), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.DLTCR, bitsValueMatcher);
}

TEST_F(ADSIHost, InitSetsLP2HSTIMEBitsInDLTCRRegisterAccordingToDataLaneLowPowerToHighSpeedTime)
{
  constexpr uint32_t DSI_PHY_PLL_CLOCK_FREQ = 800000000u;
  setDSIPHYClockFrequencyTo800MHz();
  constexpr uint32_t DATA_LANE_LOW_POWER_TO_HIGH_SPEED_TIME = 150u;
  constexpr uint32_t DSIHOST_DLTCR_LP2HS_TIME_POSITION = 16u;
  constexpr uint32_t DSIHOST_DLTCR_LP2HS_TIME_SIZE     = 8u;
  constexpr uint32_t EXPECTED_DSIHOST_DLTCR_LP2HS_TIME_VALUE =
    DATA_LANE_LOW_POWER_TO_HIGH_SPEED_TIME / (NANOSECONDS_IN_SECOND / (DSI_PHY_PLL_CLOCK_FREQ / BITS_IN_BYTE));
  auto bitsValueMatcher = BitsHaveValue(DSIHOST_DLTCR_LP2HS_TIME_POSITION,
    DSIHOST_DLTCR_LP2HS_TIME_SIZE,
    EXPECTED_DSIHOST_DLTCR_LP2HS_TIME_VALUE);
  dsiHostConfig.dsiPhyTiming.dataLaneLowPowerToHighSpeedTime = DATA_LANE_LOW_POWER_TO_HIGH_SPEED_TIME;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.DLTCR), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.DLTCR, bitsValueMatcher);
}

TEST_F(ADSIHost, InitSetsSWTIMEBitsInPCONFRRegisterAccordingToStopWaitTime)
{
  constexpr uint32_t DSI_PHY_PLL_CLOCK_FREQ = 800000000u;
  setDSIPHYClockFrequencyTo800MHz();
  constexpr uint32_t STOP_WAIT_TIME = 70u;
  constexpr uint32_t DSIHOST_PCONFR_SW_TIME_POSITION = 8u;
  constexpr uint32_t DSIHOST_PCONFR_SW_TIME_SIZE     = 8u;
  constexpr uint32_t EXPECTED_DSIHOST_PCONFR_SW_TIME_VALUE =
    STOP_WAIT_TIME / (NANOSECONDS_IN_SECOND / (DSI_PHY_PLL_CLOCK_FREQ / BITS_IN_BYTE));
  auto bitsValueMatcher = BitsHaveValue(DSIHOST_PCONFR_SW_TIME_POSITION,
    DSIHOST_PCONFR_SW_TIME_SIZE,
    EXPECTED_DSIHOST_PCONFR_SW_TIME_VALUE);
  dsiHostConfig.dsiPhyTiming.stopWaitTime = STOP_WAIT_TIME;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.PCONFR), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.PCONFR, bitsValueMatcher);
}

TEST_F(ADSIHost, InitSetsTimeoutClockDivisionFactorToOneBySettingTOCKDIVBitsInCCRRegister)
{
  constexpr uint32_t DSIHOST_CCR_TOCKDIV_POSITION = 8u;
  constexpr uint32_t DSIHOST_CCR_TOCKDIV_SIZE     = 8u;
  constexpr uint32_t EXPECTED_DSIHOST_CCR_TOCKDIV_VALUE = 1u;
  auto bitsValueMatcher = BitsHaveValue(DSIHOST_CCR_TOCKDIV_POSITION,
    DSIHOST_CCR_TOCKDIV_SIZE,
    EXPECTED_DSIHOST_CCR_TOCKDIV_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.CCR), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.CCR, bitsValueMatcher);
}

TEST_F(ADSIHost, InitSetsHighSpeedTransmissionTimeoutToZeroBySettingHSTXTOCNTBitsInTCCR0Register)
{
  constexpr uint32_t DSIHOST_TCCR0_HSTX_TOCNT_POSITION = 16u;
  constexpr uint32_t DSIHOST_TCCR0_HSTX_TOCNT_SIZE     = 16u;
  constexpr uint32_t EXPECTED_DSIHOST_TCCR0_HSTX_TOCNT_VALUE = 0u;
  auto bitsValueMatcher = BitsHaveValue(DSIHOST_TCCR0_HSTX_TOCNT_POSITION,
    DSIHOST_TCCR0_HSTX_TOCNT_SIZE,
    EXPECTED_DSIHOST_TCCR0_HSTX_TOCNT_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.TCCR[0]), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.TCCR[0], bitsValueMatcher);
}

TEST_F(ADSIHost, InitSetsLowPowerReceptionTimeoutToZeroBySettingLPRXTOCNTBitsInTCCR0Register)
{
  constexpr uint32_t DSIHOST_TCCR0_LPRX_TOCNT_POSITION = 0u;
  constexpr uint32_t DSIHOST_TCCR0_LPRX_TOCNT_SIZE     = 16u;
  constexpr uint32_t EXPECTED_DSIHOST_TCCR0_LPRX_TOCNT_VALUE = 0u;
  auto bitsValueMatcher = BitsHaveValue(DSIHOST_TCCR0_LPRX_TOCNT_POSITION,
    DSIHOST_TCCR0_LPRX_TOCNT_SIZE,
    EXPECTED_DSIHOST_TCCR0_LPRX_TOCNT_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.TCCR[0]), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.TCCR[0], bitsValueMatcher);
}

TEST_F(ADSIHost, InitSetsHighSpeedReadTimeoutToZeroBySettingHSRDTOCNTBitsInTCCR1Register)
{
  constexpr uint32_t DSIHOST_TCCR1_HSRD_TOCNT_POSITION = 0u;
  constexpr uint32_t DSIHOST_TCCR1_HSRD_TOCNT_SIZE     = 16u;
  constexpr uint32_t EXPECTED_DSIHOST_TCCR1_HSRD_TOCNT_VALUE = 0u;
  auto bitsValueMatcher = BitsHaveValue(DSIHOST_TCCR1_HSRD_TOCNT_POSITION,
    DSIHOST_TCCR1_HSRD_TOCNT_SIZE,
    EXPECTED_DSIHOST_TCCR1_HSRD_TOCNT_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.TCCR[1]), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.TCCR[1], bitsValueMatcher);
}

TEST_F(ADSIHost, InitSetsLowPowerReadTimeoutToZeroBySettingLPRDTOCNTBitsInTCCR2Register)
{
  constexpr uint32_t DSIHOST_TCCR2_LPRD_TOCNT_POSITION = 0u;
  constexpr uint32_t DSIHOST_TCCR2_LPRD_TOCNT_SIZE     = 16u;
  constexpr uint32_t EXPECTED_DSIHOST_TCCR2_LPRD_TOCNT_VALUE = 0u;
  auto bitsValueMatcher = BitsHaveValue(DSIHOST_TCCR2_LPRD_TOCNT_POSITION,
    DSIHOST_TCCR2_LPRD_TOCNT_SIZE,
    EXPECTED_DSIHOST_TCCR2_LPRD_TOCNT_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.TCCR[2]), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.TCCR[2], bitsValueMatcher);
}

TEST_F(ADSIHost, InitSetsHighSpeedWriteTimeoutToZeroBySettingHSWRTOCNTBitsInTCCR3Register)
{
  constexpr uint32_t DSIHOST_TCCR3_HSWR_TOCNT_POSITION = 0u;
  constexpr uint32_t DSIHOST_TCCR3_HSWR_TOCNT_SIZE     = 16u;
  constexpr uint32_t EXPECTED_DSIHOST_TCCR3_HSWR_TOCNT_VALUE = 0u;
  auto bitsValueMatcher = BitsHaveValue(DSIHOST_TCCR3_HSWR_TOCNT_POSITION,
    DSIHOST_TCCR3_HSWR_TOCNT_SIZE,
    EXPECTED_DSIHOST_TCCR3_HSWR_TOCNT_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.TCCR[3]), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.TCCR[3], bitsValueMatcher);
}

TEST_F(ADSIHost, InitDisablesPrespModeBySettingToZeroPMBitInTCCR3Register)
{
  constexpr uint32_t DSIHOST_TCCR3_PM_POSITION = 24u;
  constexpr uint32_t EXPECTED_DSIHOST_TCCR3_PM_VALUE = 0u;
  auto bitValueMatcher = BitHasValue(DSIHOST_TCCR3_PM_POSITION, EXPECTED_DSIHOST_TCCR3_PM_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.TCCR[3]), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.TCCR[3], bitValueMatcher);
}

TEST_F(ADSIHost, InitSetsLowPowerWriteTimeoutToZeroBySettingLSWRTOCNTBitsInTCCR4Register)
{
  constexpr uint32_t DSIHOST_TCCR4_LSWR_TOCNT_POSITION = 0u;
  constexpr uint32_t DSIHOST_TCCR4_LSWR_TOCNT_SIZE     = 16u;
  constexpr uint32_t EXPECTED_DSIHOST_TCCR4_LSWR_TOCNT_VALUE = 0u;
  auto bitsValueMatcher = BitsHaveValue(DSIHOST_TCCR4_LSWR_TOCNT_POSITION,
    DSIHOST_TCCR4_LSWR_TOCNT_SIZE,
    EXPECTED_DSIHOST_TCCR4_LSWR_TOCNT_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.TCCR[4]), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.TCCR[4], bitsValueMatcher);
}

TEST_F(ADSIHost, InitSetsBusTurnAroundTimeoutToZeroBySettingBTATOCNTBitsInTCCR5Register)
{
  constexpr uint32_t DSIHOST_TCCR5_BTA_TOCNT_POSITION = 0u;
  constexpr uint32_t DSIHOST_TCCR5_BTA_TOCNT_SIZE     = 16u;
  constexpr uint32_t EXPECTED_DSIHOST_TCCR5_BTA_TOCNT_VALUE = 0u;
  auto bitsValueMatcher = BitsHaveValue(DSIHOST_TCCR5_BTA_TOCNT_POSITION,
    DSIHOST_TCCR5_BTA_TOCNT_SIZE,
    EXPECTED_DSIHOST_TCCR5_BTA_TOCNT_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.TCCR[5]), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.TCCR[5], bitsValueMatcher);
}

TEST_F(ADSIHost, InitSetsCRCRXEBitInPCRRegisterAccordingToEnableCRCReceptionConfigParam)
{
  constexpr uint32_t DSIHOST_PCR_CRCRXE_POSITION = 4u;
  constexpr uint32_t EXPECTED_DSIHOST_PCR_CRCRXE_VALUE = 1u;
  auto bitValueMatcher = BitHasValue(DSIHOST_PCR_CRCRXE_POSITION, EXPECTED_DSIHOST_PCR_CRCRXE_VALUE);
  dsiHostConfig.flowControlConfig.enableCRCReception = true;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.PCR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.PCR, bitValueMatcher);
}

TEST_F(ADSIHost, InitSetsECCRXEBitInPCRRegisterAccordingToEnableECCReceptionConfigParam)
{
  constexpr uint32_t DSIHOST_PCR_ECCRXE_POSITION = 3u;
  constexpr uint32_t EXPECTED_DSIHOST_PCR_ECCRXE_VALUE = 1u;
  auto bitValueMatcher = BitHasValue(DSIHOST_PCR_ECCRXE_POSITION, EXPECTED_DSIHOST_PCR_ECCRXE_VALUE);
  dsiHostConfig.flowControlConfig.enableECCReception = true;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.PCR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.PCR, bitValueMatcher);
}

TEST_F(ADSIHost, InitSetsBTAEBitInPCRRegisterAccordingToEnableBusTurnAroundConfigParam)
{
  constexpr uint32_t DSIHOST_PCR_BTAE_POSITION = 2u;
  constexpr uint32_t EXPECTED_DSIHOST_PCR_BTAE_VALUE = 1u;
  auto bitValueMatcher = BitHasValue(DSIHOST_PCR_BTAE_POSITION, EXPECTED_DSIHOST_PCR_BTAE_VALUE);
  dsiHostConfig.flowControlConfig.enableBusTurnAround = true;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.PCR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.PCR, bitValueMatcher);
}

TEST_F(ADSIHost, InitSetsETRXEBitInPCRRegisterAccordingToEnableEoTpReceptionConfigParam)
{
  constexpr uint32_t DSIHOST_PCR_ETRXE_POSITION = 1u;
  constexpr uint32_t EXPECTED_DSIHOST_PCR_ETRXE_VALUE = 0u;
  auto bitValueMatcher = BitHasValue(DSIHOST_PCR_ETRXE_POSITION, EXPECTED_DSIHOST_PCR_ETRXE_VALUE);
  dsiHostConfig.flowControlConfig.enableEoTpReception = false;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.PCR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.PCR, bitValueMatcher);
}

TEST_F(ADSIHost, InitSetsETTXEBitInPCRRegisterAccordingToEnableEoTpTransmissionConfigParam)
{
  constexpr uint32_t DSIHOST_PCR_ETTXE_POSITION = 0u;
  constexpr uint32_t EXPECTED_DSIHOST_PCR_ETTXE_VALUE = 0u;
  auto bitValueMatcher = BitHasValue(DSIHOST_PCR_ETTXE_POSITION, EXPECTED_DSIHOST_PCR_ETTXE_VALUE);
  dsiHostConfig.flowControlConfig.enableEoTpTransmission = false;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.PCR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.PCR, bitValueMatcher);
}

TEST_F(ADSIHost, InitSetsMRDPSBitInCMCRRegisterAccordingToMaximumReadPacketSizeTransmissionType)
{
  constexpr uint32_t DSIHOST_CMCR_MRDPS_POSITION = 24u;
  constexpr uint32_t EXPECTED_DSIHOST_CMCR_MRDPS_VALUE = 1u;
  auto bitValueMatcher = BitHasValue(DSIHOST_CMCR_MRDPS_POSITION, EXPECTED_DSIHOST_CMCR_MRDPS_VALUE);
  dsiHostConfig.dbiInterfaceConfig.transmissionTypeMaximumReadPacketSize = DSIHost::TransmissionType::LOW_POWER;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.CMCR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.CMCR, bitValueMatcher);
}

TEST_F(ADSIHost, InitSetsDLWTXBitInCMCRRegisterAccordingToDCSLongWriteTransmissionType)
{
  constexpr uint32_t DSIHOST_CMCR_DLWTX_POSITION = 19u;
  constexpr uint32_t EXPECTED_DSIHOST_CMCR_DLWTX_VALUE = 1u;
  auto bitValueMatcher = BitHasValue(DSIHOST_CMCR_DLWTX_POSITION, EXPECTED_DSIHOST_CMCR_DLWTX_VALUE);
  dsiHostConfig.dbiInterfaceConfig.transmissionTypeDCSLongWrite = DSIHost::TransmissionType::LOW_POWER;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.CMCR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.CMCR, bitValueMatcher);
}

TEST_F(ADSIHost, InitSetsDSR0TXBitInCMCRRegisterAccordingToDCSShortReadZeroParamTransmissionType)
{
  constexpr uint32_t DSIHOST_CMCR_DSR0TX_POSITION = 18u;
  constexpr uint32_t EXPECTED_DSIHOST_CMCR_DSR0TX_VALUE = 1u;
  auto bitValueMatcher = BitHasValue(DSIHOST_CMCR_DSR0TX_POSITION, EXPECTED_DSIHOST_CMCR_DSR0TX_VALUE);
  dsiHostConfig.dbiInterfaceConfig.transmissionTypeDCSShortReadZeroParam = DSIHost::TransmissionType::LOW_POWER;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.CMCR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.CMCR, bitValueMatcher);
}

TEST_F(ADSIHost, InitSetsDSW1TXBitInCMCRRegisterAccordingToDCSShortReadOneParamTransmissionType)
{
  constexpr uint32_t DSIHOST_CMCR_DSW1TX_POSITION = 17u;
  constexpr uint32_t EXPECTED_DSIHOST_CMCR_DSW1TX_VALUE = 1u;
  auto bitValueMatcher = BitHasValue(DSIHOST_CMCR_DSW1TX_POSITION, EXPECTED_DSIHOST_CMCR_DSW1TX_VALUE);
  dsiHostConfig.dbiInterfaceConfig.transmissionTypeDCSShortReadOneParam = DSIHost::TransmissionType::LOW_POWER;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.CMCR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.CMCR, bitValueMatcher);
}

TEST_F(ADSIHost, InitSetsDSW0TXBitInCMCRRegisterAccordingToDCSShortWriteZeroParamTransmissionType)
{
  constexpr uint32_t DSIHOST_CMCR_DSW0TX_POSITION = 16u;
  constexpr uint32_t EXPECTED_DSIHOST_CMCR_DSW0TX_VALUE = 1u;
  auto bitValueMatcher = BitHasValue(DSIHOST_CMCR_DSW0TX_POSITION, EXPECTED_DSIHOST_CMCR_DSW0TX_VALUE);
  dsiHostConfig.dbiInterfaceConfig.transmissionTypeDCSShortWriteZeroParam = DSIHost::TransmissionType::LOW_POWER;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.CMCR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.CMCR, bitValueMatcher);
}

TEST_F(ADSIHost, InitSetsGLWTXBitInCMCRRegisterAccordingToGenericLongWriteTransmissionType)
{
  constexpr uint32_t DSIHOST_CMCR_GLWTX_POSITION = 14u;
  constexpr uint32_t EXPECTED_DSIHOST_CMCR_GLWTX_VALUE = 1u;
  auto bitValueMatcher = BitHasValue(DSIHOST_CMCR_GLWTX_POSITION, EXPECTED_DSIHOST_CMCR_GLWTX_VALUE);
  dsiHostConfig.dbiInterfaceConfig.transmissionTypeGenericLongWrite = DSIHost::TransmissionType::LOW_POWER;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.CMCR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.CMCR, bitValueMatcher);
}

TEST_F(ADSIHost, InitSetsGSR2TXBitInCMCRRegisterAccordingToGenericShortReadTwoParamTransmissionType)
{
  constexpr uint32_t DSIHOST_CMCR_GSR2TX_POSITION = 13u;
  constexpr uint32_t EXPECTED_DSIHOST_CMCR_GSR2TX_VALUE = 1u;
  auto bitValueMatcher = BitHasValue(DSIHOST_CMCR_GSR2TX_POSITION, EXPECTED_DSIHOST_CMCR_GSR2TX_VALUE);
  dsiHostConfig.dbiInterfaceConfig.transmissionTypeGenericShortReadTwoParam = DSIHost::TransmissionType::LOW_POWER;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.CMCR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.CMCR, bitValueMatcher);
}

TEST_F(ADSIHost, InitSetsGSR1TXBitInCMCRRegisterAccordingToGenericShortReadOneParamTransmissionType)
{
  constexpr uint32_t DSIHOST_CMCR_GSR1TX_POSITION = 12u;
  constexpr uint32_t EXPECTED_DSIHOST_CMCR_GSR1TX_VALUE = 1u;
  auto bitValueMatcher = BitHasValue(DSIHOST_CMCR_GSR1TX_POSITION, EXPECTED_DSIHOST_CMCR_GSR1TX_VALUE);
  dsiHostConfig.dbiInterfaceConfig.transmissionTypeGenericShortReadOneParam = DSIHost::TransmissionType::LOW_POWER;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.CMCR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.CMCR, bitValueMatcher);
}

TEST_F(ADSIHost, InitSetsGSR0TXBitInCMCRRegisterAccordingToGenericShortReadZeroParamTransmissionType)
{
  constexpr uint32_t DSIHOST_CMCR_GSR0TX_POSITION = 11u;
  constexpr uint32_t EXPECTED_DSIHOST_CMCR_GSR0TX_VALUE = 1u;
  auto bitValueMatcher = BitHasValue(DSIHOST_CMCR_GSR0TX_POSITION, EXPECTED_DSIHOST_CMCR_GSR0TX_VALUE);
  dsiHostConfig.dbiInterfaceConfig.transmissionTypeGenericShortReadZeroParam = DSIHost::TransmissionType::LOW_POWER;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.CMCR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.CMCR, bitValueMatcher);
}

TEST_F(ADSIHost, InitSetsGSW2TXBitInCMCRRegisterAccordingToGenericShortWriteTwoParamTransmissionType)
{
  constexpr uint32_t DSIHOST_CMCR_GSW2TX_POSITION = 10u;
  constexpr uint32_t EXPECTED_DSIHOST_CMCR_GSW2TX_VALUE = 1u;
  auto bitValueMatcher = BitHasValue(DSIHOST_CMCR_GSW2TX_POSITION, EXPECTED_DSIHOST_CMCR_GSW2TX_VALUE);
  dsiHostConfig.dbiInterfaceConfig.transmissionTypeGenericShortWriteTwoParam = DSIHost::TransmissionType::LOW_POWER;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.CMCR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.CMCR, bitValueMatcher);
}

TEST_F(ADSIHost, InitSetsGSW1TXBitInCMCRRegisterAccordingTGenericShortWriteOneParamTransmissionType)
{
  constexpr uint32_t DSIHOST_CMCR_GSW1TX_POSITION = 9u;
  constexpr uint32_t EXPECTED_DSIHOST_CMCR_GSW1TX_VALUE = 1u;
  auto bitValueMatcher = BitHasValue(DSIHOST_CMCR_GSW1TX_POSITION, EXPECTED_DSIHOST_CMCR_GSW1TX_VALUE);
  dsiHostConfig.dbiInterfaceConfig.transmissionTypeGenericShortWriteOneParam = DSIHost::TransmissionType::LOW_POWER;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.CMCR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.CMCR, bitValueMatcher);
}

TEST_F(ADSIHost, InitSetsGSW0TXBitInCMCRRegisterAccordingTGenericShortWriteZeroParamTransmissionType)
{
  constexpr uint32_t DSIHOST_CMCR_GSW0TX_POSITION = 8u;
  constexpr uint32_t EXPECTED_DSIHOST_CMCR_GSW0TX_VALUE = 1u;
  auto bitValueMatcher = BitHasValue(DSIHOST_CMCR_GSW0TX_POSITION, EXPECTED_DSIHOST_CMCR_GSW0TX_VALUE);
  dsiHostConfig.dbiInterfaceConfig.transmissionTypeGenericShortWriteZeroParam = DSIHost::TransmissionType::LOW_POWER;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.CMCR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.CMCR, bitValueMatcher);
}

TEST_F(ADSIHost, InitSetsAREBitInCMCRRegisterAccordingToEnableAcknowledgeRequestConfigParam)
{
  constexpr uint32_t DSIHOST_CMCR_ARE_POSITION = 1u;
  constexpr uint32_t EXPECTED_DSIHOST_CMCR_ARE_VALUE = 1u;
  auto bitValueMatcher = BitHasValue(DSIHOST_CMCR_ARE_POSITION, EXPECTED_DSIHOST_CMCR_ARE_VALUE);
  dsiHostConfig.dbiInterfaceConfig.enableAcknowledgeRequest = true;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.CMCR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.CMCR, bitValueMatcher);
}

TEST_F(ADSIHost, InitSetsTEAREBitInCMCRRegisterAccordingToEnableTearingEffectAcknowledgeRequestConfigParam)
{
  constexpr uint32_t DSIHOST_CMCR_TEARE_POSITION = 0u;
  constexpr uint32_t EXPECTED_DSIHOST_CMCR_TEARE_VALUE = 1u;
  auto bitValueMatcher = BitHasValue(DSIHOST_CMCR_TEARE_POSITION, EXPECTED_DSIHOST_CMCR_TEARE_VALUE);
  dsiHostConfig.dbiInterfaceConfig.enableTearingEffectAcknowledgeRequest = true;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.CMCR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.CMCR, bitValueMatcher);
}

TEST_F(ADSIHost, InitSetsVirtualChannelIdBySettingVCIDBitsInLCVCIDRRegister)
{
  constexpr uint8_t VIRTUAL_CHANNEL_ID = 2u;
  constexpr uint32_t DSIHOST_LCVCIDR_VCID_POSITION = 0u;
  constexpr uint32_t DSIHOST_LCVCIDR_VCID_SIZE     = 2u;
  constexpr uint32_t EXPECTED_DSIHOST_LCVCIDR_VCID_VALUE = VIRTUAL_CHANNEL_ID;
  auto bitsValueMatcher =
    BitsHaveValue(DSIHOST_LCVCIDR_VCID_POSITION, DSIHOST_LCVCIDR_VCID_SIZE, EXPECTED_DSIHOST_LCVCIDR_VCID_VALUE);
  dsiHostConfig.virtualChannelId = VIRTUAL_CHANNEL_ID;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.LCVCIDR), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.LCVCIDR, bitsValueMatcher);
}

TEST_F(ADSIHost, InitSetsLTDCColorCodingBySettingCOLCBitsInLCOLCRRegisterAccordingToLTDCColorCodingConfigParam)
{
  constexpr uint32_t DSIHOST_LCOLCR_COLC_POSITION = 0u;
  constexpr uint32_t DSIHOST_LCOLCR_COLC_SIZE     = 4u;
  constexpr uint32_t EXPECTED_DSIHOST_LCOLCR_COLC_VALUE = 0b0101;
  auto bitsValueMatcher =
    BitsHaveValue(DSIHOST_LCOLCR_COLC_POSITION, DSIHOST_LCOLCR_COLC_SIZE, EXPECTED_DSIHOST_LCOLCR_COLC_VALUE);
  dsiHostConfig.ltdcColorCoding = DSIHost::LTDCColorCoding::RGB888;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.LCOLCR), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.LCOLCR, bitsValueMatcher);
}

TEST_F(ADSIHost, InitEnablesLooselyPacketVariantBySettingLPEBitInLCOLCRRegisterIfLTDCColorCodingIsRGB666)
{
  constexpr uint32_t DSIHOST_LCOLCR_LPE_POSITION = 8u;
  constexpr uint32_t EXPECTED_DSIHOST_LCOLCR_LPE_VALUE = 0x1;
  auto bitValueMatcher = BitHasValue(DSIHOST_LCOLCR_LPE_POSITION, EXPECTED_DSIHOST_LCOLCR_LPE_VALUE);
  dsiHostConfig.ltdcColorCoding = DSIHost::LTDCColorCoding::RGB666_IN_BLOCK;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.LCOLCR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.LCOLCR, bitValueMatcher);
}

TEST_F(ADSIHost, InitSetsColorMultiplexingBySettingCOLMUXBitsInWCFGRRegisterAccordingToLTDCColorCodingConfigParam)
{
  constexpr uint32_t DSIHOST_WCFGR_COLMUX_POSITION = 1u;
  constexpr uint32_t DSIHOST_WCFGR_COLMUX_SIZE     = 3u;
  constexpr uint32_t EXPECTED_DSIHOST_WCFGR_COLMUX_VALUE = 0b101;
  auto bitsValueMatcher =
    BitsHaveValue(DSIHOST_WCFGR_COLMUX_POSITION, DSIHOST_WCFGR_COLMUX_SIZE, EXPECTED_DSIHOST_WCFGR_COLMUX_VALUE);
  dsiHostConfig.ltdcColorCoding = DSIHost::LTDCColorCoding::RGB888;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.WCFGR), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.WCFGR, bitsValueMatcher);
}

TEST_F(ADSIHost, InitSetsVSyncPolarityBySettingVSPOLBitInWCFGRRegisterAccordingToVSyncPolarityConfigParam)
{
  constexpr uint32_t DSIHOST_WCFGR_VSPOL_POSITION = 7u;
  constexpr uint32_t EXPECTED_DSIHOST_WCFGR_VSPOL_VALUE = 0x1;
  auto bitValueMatcher = BitHasValue(DSIHOST_WCFGR_VSPOL_POSITION, EXPECTED_DSIHOST_WCFGR_VSPOL_VALUE);
  dsiHostConfig.vsyncLtdcHaltPolarity = DSIHost::VSyncLTDCHaltPolarity::FALLING_EDGE;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.WCFGR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.WCFGR, bitValueMatcher);
}

TEST_F(ADSIHost, InitSetsHSPBitInLPCRRegisterAccordingToHSYNCPolarity)
{
  constexpr uint32_t DSIHOST_LPCR_HSP_POSITION = 2u;
  constexpr uint32_t EXPECTED_DSIHOST_LPCR_HSP_VALUE = 0x1;
  auto bitValueMatcher = BitHasValue(DSIHOST_LPCR_HSP_POSITION, EXPECTED_DSIHOST_LPCR_HSP_VALUE);
  dsiHostConfig.hsyncPolarity = DSIHost::SignalPolarity::ACTIVE_LOW;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.LPCR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.LPCR, bitValueMatcher);
}

TEST_F(ADSIHost, InitSetsDEPBitInLPCRRegisterAccordingToDataEnablePolarity)
{
  constexpr uint32_t DSIHOST_LPCR_DEP_POSITION = 0u;
  constexpr uint32_t EXPECTED_DSIHOST_LPCR_DEP_VALUE = 0x1;
  auto bitValueMatcher = BitHasValue(DSIHOST_LPCR_DEP_POSITION, EXPECTED_DSIHOST_LPCR_DEP_VALUE);
  dsiHostConfig.dataEnablePolarity = DSIHost::SignalPolarity::ACTIVE_LOW;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.LPCR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.LPCR, bitValueMatcher);
}

TEST_F(ADSIHost, InitSetsVSPBitInLPCRRegisterAccordingToVSYNCPolarity)
{
  constexpr uint32_t DSIHOST_LPCR_VSP_POSITION = 1u;
  constexpr uint32_t EXPECTED_DSIHOST_LPCR_VSP_VALUE = 0x1;
  auto bitValueMatcher = BitHasValue(DSIHOST_LPCR_VSP_POSITION, EXPECTED_DSIHOST_LPCR_VSP_VALUE);
  dsiHostConfig.vsyncPolarity = DSIHost::SignalPolarity::ACTIVE_LOW;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.LPCR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.LPCR, bitValueMatcher);
}

TEST_F(ADSIHost, InitSetsDSIModeToAdaptedCommandModeBySettingToOneDSIMBitInWCFGRRegister)
{
  constexpr uint32_t DSIHOST_WCFGR_DSIM_POSITION = 0u;
  constexpr uint32_t EXPECTED_DSIHOST_WCFGR_DSIM_VALUE = 0x1;
  auto bitValueMatcher = BitHasValue(DSIHOST_WCFGR_DSIM_POSITION, EXPECTED_DSIHOST_WCFGR_DSIM_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.WCFGR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.WCFGR, bitValueMatcher);
}

TEST_F(ADSIHost, InitSetsCommandModeToAdaptedCommandModeBySettingToOneCMDMBitInMCRRRegister)
{
  constexpr uint32_t DSIHOST_MCR_CMDM_POSITION = 0u;
  constexpr uint32_t EXPECTED_DSIHOST_MCR_CMDM_VALUE = 0x1;
  auto bitValueMatcher = BitHasValue(DSIHOST_MCR_CMDM_POSITION, EXPECTED_DSIHOST_MCR_CMDM_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.MCR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.MCR, bitValueMatcher);
}

TEST_F(ADSIHost, InitSetsCMDSIZEBitsInLCCRRegisterAccordingToMaximumLTDCWriteMemoryCommandSize)
{
  constexpr uint32_t DSIHOST_LCCR_CMDSIZE_POSITION = 0u;
  constexpr uint32_t DSIHOST_LCCR_CMDSIZE_SIZE     = 16u;
  constexpr uint32_t EXPECTED_DSIHOST_LCCR_CMDSIZE_VALUE = 390u;
  auto bitValueMatcher =
    BitsHaveValue(DSIHOST_LCCR_CMDSIZE_POSITION, DSIHOST_LCCR_CMDSIZE_SIZE, EXPECTED_DSIHOST_LCCR_CMDSIZE_VALUE);
  dsiHostConfig.maxLTDCWriteMemoryCmdSize = 390u;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.LCCR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.LCCR, bitValueMatcher);
}

TEST_F(ADSIHost, InitSetsTearingEffectSourceBySettingTESRCBitInWCFGRRegister)
{
  constexpr uint32_t DSIHOST_WCFGR_TESRC_POSITION = 4u;
  constexpr uint32_t EXPECTED_DSIHOST_WCFGR_TESRC_VALUE = 0x1;
  auto bitValueMatcher = BitHasValue(DSIHOST_WCFGR_TESRC_POSITION, EXPECTED_DSIHOST_WCFGR_TESRC_VALUE);
  dsiHostConfig.tearingEffectSource = DSIHost::TearingEffectSource::EXTERNAL_PIN;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.WCFGR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.WCFGR, bitValueMatcher);
}

TEST_F(ADSIHost, InitSetsTearingEffectPolarityBySettingTEBitInWCFGRRegisterAccordingToTearingEffectPolarityConfigParam)
{
  constexpr uint32_t DSIHOST_WCFGR_TE_POSITION = 5u;
  constexpr uint32_t EXPECTED_DSIHOST_WCFGR_TE_VALUE = 0x1;
  auto bitValueMatcher = BitHasValue(DSIHOST_WCFGR_TE_POSITION, EXPECTED_DSIHOST_WCFGR_TE_VALUE);
  dsiHostConfig.tearingEffectPolarity = DSIHost::TearingEffectPolarity::FALLING_EDGE;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.WCFGR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.WCFGR, bitValueMatcher);
}

TEST_F(ADSIHost, InitEnablesAutoRefreshModeBySettingARBitInWCFGRRegisterAccordingToEnableAutoRefreshModeConfigParam)
{
  constexpr uint32_t DSIHOST_WCFGR_AR_POSITION = 6u;
  constexpr uint32_t EXPECTED_DSIHOST_WCFGR_AR_VALUE = 0x1;
  auto bitValueMatcher = BitHasValue(DSIHOST_WCFGR_AR_POSITION, EXPECTED_DSIHOST_WCFGR_AR_VALUE);
  dsiHostConfig.enableAutoRefreshMode = true;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.WCFGR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.WCFGR, bitValueMatcher);
}

TEST_F(ADSIHost, InitDisablesAllDSIHostInterruptsBySettingToZeroIER0Register)
{
  constexpr uint32_t ALL_INTERRUPTS_DISABLED = 0x00000000u;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.IER[0]), ALL_INTERRUPTS_DISABLED);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.IER[0], Eq(ALL_INTERRUPTS_DISABLED));
}

TEST_F(ADSIHost, InitDisablesAllDSIHostInterruptsBySettingToZeroIER1Register)
{
  constexpr uint32_t ALL_INTERRUPTS_DISABLED = 0x00000000u;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.IER[1]), ALL_INTERRUPTS_DISABLED);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.IER[1], Eq(ALL_INTERRUPTS_DISABLED));
}

TEST_F(ADSIHost, InitEnablesDSIHostBySettingENBitInCRRegister)
{
  constexpr uint32_t DSIHOST_CR_EN_POSITION = 0u;
  constexpr uint32_t EXPECTED_DSIHOST_CR_EN_VALUE = 0x1;
  auto bitValueMatcher = BitHasValue(DSIHOST_CR_EN_POSITION, EXPECTED_DSIHOST_CR_EN_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.CR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.CR, bitValueMatcher);
}

TEST_F(ADSIHost, InitEnablesDSIWrapperBySettingDSIENBitInWCRRegister)
{
  constexpr uint32_t DSIHOST_WCR_DSIEN_POSITION = 3u;
  constexpr uint32_t EXPECTED_DSIHOST_WCR_DSIEN_VALUE = 0x1;
  auto bitValueMatcher = BitHasValue(DSIHOST_WCR_DSIEN_POSITION, EXPECTED_DSIHOST_WCR_DSIEN_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.WCR), bitValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.init(dsiHostConfig);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.WCR, bitValueMatcher);
}

TEST_F(ADSIHost, GenericShortWriteWithZeroParamsSetsDTBitsInGHCRRegisterToAppropriateValue)
{
  constexpr uint8_t GENERIC_SHORT_WRITE_ZERO_PARAM_DATA_TYPE = 0x3;
  constexpr uint32_t DSIHOST_GHCR_DT_POSITION = 0u;
  constexpr uint32_t DSIHOST_GHCR_DT_SIZE     = 6u;
  constexpr uint32_t EXPECTED_DSIHOST_GHCR_DT_VALUE = GENERIC_SHORT_WRITE_ZERO_PARAM_DATA_TYPE;
  auto bitsValueMatcher =
    BitsHaveValue(DSIHOST_GHCR_DT_POSITION, DSIHOST_GHCR_DT_SIZE, EXPECTED_DSIHOST_GHCR_DT_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.GHCR), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.genericShortWrite(RANDOM_VIRTUAL_CHANNEL_ID);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.GHCR, bitsValueMatcher);
}

TEST_F(ADSIHost, GenericShortWriteWithZeroParamsSetsVCIDBitsInGHCRRegisterAccordingToVirtualChannelID)
{
  constexpr uint32_t DSIHOST_GHCR_VCID_POSITION = 6u;
  constexpr uint32_t DSIHOST_GHCR_VCID_SIZE     = 2u;
  constexpr uint32_t EXPECTED_DSIHOST_GHCR_VCID_VALUE = 2u;
  auto bitsValueMatcher =
    BitsHaveValue(DSIHOST_GHCR_VCID_POSITION, DSIHOST_GHCR_VCID_SIZE, EXPECTED_DSIHOST_GHCR_VCID_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.GHCR), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.genericShortWrite(DSIHost::VirtualChannelID::CHANNEL_2);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.GHCR, bitsValueMatcher);
}

TEST_F(ADSIHost, GenericShortWriteWithZeroParamsSetsWCLSBBitsInGHCRRegisterToZero)
{
  constexpr uint32_t DSIHOST_GHCR_WCLSB_POSITION = 8u;
  constexpr uint32_t DSIHOST_GHCR_WCLSB_SIZE     = 8u;
  constexpr uint32_t EXPECTED_DSIHOST_GHCR_WCLSB_VALUE = 0u;
  auto bitsValueMatcher =
    BitsHaveValue(DSIHOST_GHCR_WCLSB_POSITION, DSIHOST_GHCR_WCLSB_SIZE, EXPECTED_DSIHOST_GHCR_WCLSB_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.GHCR), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.genericShortWrite(RANDOM_VIRTUAL_CHANNEL_ID);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.GHCR, bitsValueMatcher);
}

TEST_F(ADSIHost, GenericShortWriteWithZeroParamsSetsWCMSBBitsInGHCRRegisterToZero)
{
  constexpr uint32_t DSIHOST_GHCR_WCMSB_POSITION = 16u;
  constexpr uint32_t DSIHOST_GHCR_WCMSB_SIZE     = 8u;
  constexpr uint32_t EXPECTED_DSIHOST_GHCR_WCMSB_VALUE = 0u;
  auto bitsValueMatcher =
    BitsHaveValue(DSIHOST_GHCR_WCMSB_POSITION, DSIHOST_GHCR_WCMSB_SIZE, EXPECTED_DSIHOST_GHCR_WCMSB_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.GHCR), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.genericShortWrite(RANDOM_VIRTUAL_CHANNEL_ID);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.GHCR, bitsValueMatcher);
}

TEST_F(ADSIHost, GenericShortWriteWithOneParamSetsDTBitsInGHCRRegisterToAppropriateValue)
{
  constexpr uint8_t GENERIC_SHORT_WRITE_ONE_PARAM_DATA_TYPE = 0x13;
  constexpr uint32_t DSIHOST_GHCR_DT_POSITION = 0u;
  constexpr uint32_t DSIHOST_GHCR_DT_SIZE     = 6u;
  constexpr uint32_t EXPECTED_DSIHOST_GHCR_DT_VALUE = GENERIC_SHORT_WRITE_ONE_PARAM_DATA_TYPE;
  auto bitsValueMatcher =
    BitsHaveValue(DSIHOST_GHCR_DT_POSITION, DSIHOST_GHCR_DT_SIZE, EXPECTED_DSIHOST_GHCR_DT_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.GHCR), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode =
    virtualDSIHost.genericShortWrite(RANDOM_VIRTUAL_CHANNEL_ID, RANDOM_PARAM_VALUE_1);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.GHCR, bitsValueMatcher);
}

TEST_F(ADSIHost, GenericShortWriteWithOneParamSetsVCIDBitsInGHCRRegisterAccordingToVirtualChannelID)
{
  constexpr uint32_t DSIHOST_GHCR_VCID_POSITION = 6u;
  constexpr uint32_t DSIHOST_GHCR_VCID_SIZE     = 2u;
  constexpr uint32_t EXPECTED_DSIHOST_GHCR_VCID_VALUE = 3u;
  auto bitsValueMatcher =
    BitsHaveValue(DSIHOST_GHCR_VCID_POSITION, DSIHOST_GHCR_VCID_SIZE, EXPECTED_DSIHOST_GHCR_VCID_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.GHCR), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode =
    virtualDSIHost.genericShortWrite(DSIHost::VirtualChannelID::CHANNEL_3, RANDOM_PARAM_VALUE_1);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.GHCR, bitsValueMatcher);
}

TEST_F(ADSIHost, GenericShortWriteWithOneParamSetsWCLSBBitsInGHCRRegisterAccordingToParameterValue)
{
  constexpr uint8_t PARAMETER_VALUE = 0xC2;
  constexpr uint32_t DSIHOST_GHCR_WCLSB_POSITION = 8u;
  constexpr uint32_t DSIHOST_GHCR_WCLSB_SIZE     = 8u;
  constexpr uint32_t EXPECTED_DSIHOST_GHCR_WCLSB_VALUE = PARAMETER_VALUE;
  auto bitsValueMatcher =
    BitsHaveValue(DSIHOST_GHCR_WCLSB_POSITION, DSIHOST_GHCR_WCLSB_SIZE, EXPECTED_DSIHOST_GHCR_WCLSB_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.GHCR), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode =
    virtualDSIHost.genericShortWrite(RANDOM_VIRTUAL_CHANNEL_ID, PARAMETER_VALUE);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.GHCR, bitsValueMatcher);
}

TEST_F(ADSIHost, GenericShortWriteWithOneParamSetsWCMSBBitsInGHCRRegisterToZero)
{
  constexpr uint32_t DSIHOST_GHCR_WCMSB_POSITION = 16u;
  constexpr uint32_t DSIHOST_GHCR_WCMSB_SIZE     = 8u;
  constexpr uint32_t EXPECTED_DSIHOST_GHCR_WCMSB_VALUE = 0u;
  auto bitsValueMatcher =
    BitsHaveValue(DSIHOST_GHCR_WCMSB_POSITION, DSIHOST_GHCR_WCMSB_SIZE, EXPECTED_DSIHOST_GHCR_WCMSB_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.GHCR), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode =
    virtualDSIHost.genericShortWrite(RANDOM_VIRTUAL_CHANNEL_ID, RANDOM_PARAM_VALUE_1);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.GHCR, bitsValueMatcher);
}

TEST_F(ADSIHost, GenericShortWriteWithTwoParamsSetsDTBitsInGHCRRegisterToAppropriateValue)
{
  constexpr uint8_t GENERIC_SHORT_WRITE_TWO_PARAMS_DATA_TYPE = 0x23;
  constexpr uint32_t DSIHOST_GHCR_DT_POSITION = 0u;
  constexpr uint32_t DSIHOST_GHCR_DT_SIZE     = 6u;
  constexpr uint32_t EXPECTED_DSIHOST_GHCR_DT_VALUE = GENERIC_SHORT_WRITE_TWO_PARAMS_DATA_TYPE;
  auto bitsValueMatcher =
    BitsHaveValue(DSIHOST_GHCR_DT_POSITION, DSIHOST_GHCR_DT_SIZE, EXPECTED_DSIHOST_GHCR_DT_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.GHCR), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode =
    virtualDSIHost.genericShortWrite(RANDOM_VIRTUAL_CHANNEL_ID, RANDOM_PARAM_VALUE_1, RANDOM_PARAM_VALUE_2);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.GHCR, bitsValueMatcher);
}

TEST_F(ADSIHost, GenericShortWriteWithTwoParamsSetsVCIDBitsInGHCRRegisterAccordingToVirtualChannelID)
{
  constexpr uint32_t DSIHOST_GHCR_VCID_POSITION = 6u;
  constexpr uint32_t DSIHOST_GHCR_VCID_SIZE     = 2u;
  constexpr uint32_t EXPECTED_DSIHOST_GHCR_VCID_VALUE = 1u;
  auto bitsValueMatcher =
    BitsHaveValue(DSIHOST_GHCR_VCID_POSITION, DSIHOST_GHCR_VCID_SIZE, EXPECTED_DSIHOST_GHCR_VCID_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.GHCR), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode =
    virtualDSIHost.genericShortWrite(DSIHost::VirtualChannelID::CHANNEL_1, RANDOM_PARAM_VALUE_1, RANDOM_PARAM_VALUE_2);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.GHCR, bitsValueMatcher);
}

TEST_F(ADSIHost, GenericShortWriteWithTwoParamsSetsWCLSBBitsInGHCRRegisterAccordingToParameter1Value)
{
  constexpr uint8_t PARAMETER1_VALUE = 0xAF;
  constexpr uint32_t DSIHOST_GHCR_WCLSB_POSITION = 8u;
  constexpr uint32_t DSIHOST_GHCR_WCLSB_SIZE     = 8u;
  constexpr uint32_t EXPECTED_DSIHOST_GHCR_WCLSB_VALUE = PARAMETER1_VALUE;
  auto bitsValueMatcher =
    BitsHaveValue(DSIHOST_GHCR_WCLSB_POSITION, DSIHOST_GHCR_WCLSB_SIZE, EXPECTED_DSIHOST_GHCR_WCLSB_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.GHCR), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode =
    virtualDSIHost.genericShortWrite(RANDOM_VIRTUAL_CHANNEL_ID, PARAMETER1_VALUE, RANDOM_PARAM_VALUE_2);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.GHCR, bitsValueMatcher);
}

TEST_F(ADSIHost, GenericShortWriteWithTwoParamsSetsWCMSBBitsInGHCRRegisterAccordingToParameter2Value)
{
  constexpr uint8_t PARAMETER2_VALUE = 0x63;
  constexpr uint32_t DSIHOST_GHCR_WCMSB_POSITION = 16u;
  constexpr uint32_t DSIHOST_GHCR_WCMSB_SIZE     = 8u;
  constexpr uint32_t EXPECTED_DSIHOST_GHCR_WCMSB_VALUE = PARAMETER2_VALUE;
  auto bitsValueMatcher =
    BitsHaveValue(DSIHOST_GHCR_WCMSB_POSITION, DSIHOST_GHCR_WCMSB_SIZE, EXPECTED_DSIHOST_GHCR_WCMSB_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.GHCR), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode =
    virtualDSIHost.genericShortWrite(RANDOM_VIRTUAL_CHANNEL_ID, RANDOM_PARAM_VALUE_1, PARAMETER2_VALUE);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.GHCR, bitsValueMatcher);
}

TEST_F(ADSIHost, DCSShortWriteWithZeroParamsSetsDTBitsInGHCRRegisterToAppropriateValue)
{
  constexpr uint8_t DCS_SHORT_WRITE_ZERO_PARAM_DATA_TYPE = 0x05;
  constexpr uint32_t DSIHOST_GHCR_DT_POSITION = 0u;
  constexpr uint32_t DSIHOST_GHCR_DT_SIZE     = 6u;
  constexpr uint32_t EXPECTED_DSIHOST_GHCR_DT_VALUE = DCS_SHORT_WRITE_ZERO_PARAM_DATA_TYPE;
  auto bitsValueMatcher =
    BitsHaveValue(DSIHOST_GHCR_DT_POSITION, DSIHOST_GHCR_DT_SIZE, EXPECTED_DSIHOST_GHCR_DT_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.GHCR), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.dcsShortWrite(RANDOM_VIRTUAL_CHANNEL_ID, RANDOM_DCS_COMMAND);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.GHCR, bitsValueMatcher);
}

TEST_F(ADSIHost, DCSShortWriteWithZeroParamsSetsVCIDBitsInGHCRRegisterAccordingToVirtualChannelID)
{
  constexpr uint32_t DSIHOST_GHCR_VCID_POSITION = 6u;
  constexpr uint32_t DSIHOST_GHCR_VCID_SIZE     = 2u;
  constexpr uint32_t EXPECTED_DSIHOST_GHCR_VCID_VALUE = 2u;
  auto bitsValueMatcher =
    BitsHaveValue(DSIHOST_GHCR_VCID_POSITION, DSIHOST_GHCR_VCID_SIZE, EXPECTED_DSIHOST_GHCR_VCID_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.GHCR), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode =
    virtualDSIHost.dcsShortWrite(DSIHost::VirtualChannelID::CHANNEL_2, RANDOM_DCS_COMMAND);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.GHCR, bitsValueMatcher);
}

TEST_F(ADSIHost, DCSShortWriteWithZeroParamsSetsWCLSBBitsInGHCRRegisterAccordingToDCSCommand)
{
  constexpr uint8_t DCS_COMMAND_VALUE = 0x18;
  constexpr uint32_t DSIHOST_GHCR_WCLSB_POSITION = 8u;
  constexpr uint32_t DSIHOST_GHCR_WCLSB_SIZE     = 8u;
  constexpr uint32_t EXPECTED_DSIHOST_GHCR_WCLSB_VALUE = DCS_COMMAND_VALUE;
  auto bitsValueMatcher =
    BitsHaveValue(DSIHOST_GHCR_WCLSB_POSITION, DSIHOST_GHCR_WCLSB_SIZE, EXPECTED_DSIHOST_GHCR_WCLSB_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.GHCR), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.dcsShortWrite(RANDOM_VIRTUAL_CHANNEL_ID, DCS_COMMAND_VALUE);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.GHCR, bitsValueMatcher);
}

TEST_F(ADSIHost, DCSShortWriteWithZeroParamsSetsWCMSBBitsInGHCRRegisterToZero)
{
  constexpr uint32_t DSIHOST_GHCR_WCMSB_POSITION = 16u;
  constexpr uint32_t DSIHOST_GHCR_WCMSB_SIZE     = 8u;
  constexpr uint32_t EXPECTED_DSIHOST_GHCR_WCMSB_VALUE = 0u;
  auto bitsValueMatcher =
    BitsHaveValue(DSIHOST_GHCR_WCMSB_POSITION, DSIHOST_GHCR_WCMSB_SIZE, EXPECTED_DSIHOST_GHCR_WCMSB_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.GHCR), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode = virtualDSIHost.dcsShortWrite(RANDOM_VIRTUAL_CHANNEL_ID, RANDOM_DCS_COMMAND);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.GHCR, bitsValueMatcher);
}

TEST_F(ADSIHost, DCSShortWriteWithOneParamSetsDTBitsInGHCRRegisterToAppropriateValue)
{
  constexpr uint8_t DCS_SHORT_WRITE_ONE_PARAM_DATA_TYPE = 0x15;
  constexpr uint32_t DSIHOST_GHCR_DT_POSITION = 0u;
  constexpr uint32_t DSIHOST_GHCR_DT_SIZE     = 6u;
  constexpr uint32_t EXPECTED_DSIHOST_GHCR_DT_VALUE = DCS_SHORT_WRITE_ONE_PARAM_DATA_TYPE;
  auto bitsValueMatcher =
    BitsHaveValue(DSIHOST_GHCR_DT_POSITION, DSIHOST_GHCR_DT_SIZE, EXPECTED_DSIHOST_GHCR_DT_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.GHCR), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode =
    virtualDSIHost.dcsShortWrite(RANDOM_VIRTUAL_CHANNEL_ID, RANDOM_DCS_COMMAND, RANDOM_PARAM_VALUE_1);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.GHCR, bitsValueMatcher);
}

TEST_F(ADSIHost, DCSShortWriteWithOneParamSetsVCIDBitsInGHCRRegisterAccordingToVirtualChannelID)
{
  constexpr uint32_t DSIHOST_GHCR_VCID_POSITION = 6u;
  constexpr uint32_t DSIHOST_GHCR_VCID_SIZE     = 2u;
  constexpr uint32_t EXPECTED_DSIHOST_GHCR_VCID_VALUE = 3u;
  auto bitsValueMatcher =
    BitsHaveValue(DSIHOST_GHCR_VCID_POSITION, DSIHOST_GHCR_VCID_SIZE, EXPECTED_DSIHOST_GHCR_VCID_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.GHCR), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode =
    virtualDSIHost.dcsShortWrite(DSIHost::VirtualChannelID::CHANNEL_3, RANDOM_DCS_COMMAND, RANDOM_PARAM_VALUE_1);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.GHCR, bitsValueMatcher);
}

TEST_F(ADSIHost, DCSShortWriteWithOneParamSetsWCLSBBitsInGHCRRegisterAccordingToDCSCommand)
{
  constexpr uint8_t DCS_COMMAND_VALUE = 0x3E;
  constexpr uint32_t DSIHOST_GHCR_WCLSB_POSITION = 8u;
  constexpr uint32_t DSIHOST_GHCR_WCLSB_SIZE     = 8u;
  constexpr uint32_t EXPECTED_DSIHOST_GHCR_WCLSB_VALUE = DCS_COMMAND_VALUE;
  auto bitsValueMatcher =
    BitsHaveValue(DSIHOST_GHCR_WCLSB_POSITION, DSIHOST_GHCR_WCLSB_SIZE, EXPECTED_DSIHOST_GHCR_WCLSB_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.GHCR), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode =
    virtualDSIHost.dcsShortWrite(RANDOM_VIRTUAL_CHANNEL_ID, DCS_COMMAND_VALUE, RANDOM_PARAM_VALUE_1);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.GHCR, bitsValueMatcher);
}

TEST_F(ADSIHost, DCSShortWriteWithOneParamSetsWCMSBBitsInGHCRRegisterAccordingToParameterValue)
{
  constexpr uint8_t PARAMETER_VALUE = 0x2C;
  constexpr uint32_t DSIHOST_GHCR_WCMSB_POSITION = 16u;
  constexpr uint32_t DSIHOST_GHCR_WCMSB_SIZE     = 8u;
  constexpr uint32_t EXPECTED_DSIHOST_GHCR_WCMSB_VALUE = PARAMETER_VALUE;
  auto bitsValueMatcher =
    BitsHaveValue(DSIHOST_GHCR_WCMSB_POSITION, DSIHOST_GHCR_WCMSB_SIZE, EXPECTED_DSIHOST_GHCR_WCMSB_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDSIHostPeripheral.GHCR), bitsValueMatcher);

  const DSIHost::ErrorCode errorCode =
    virtualDSIHost.dcsShortWrite(RANDOM_VIRTUAL_CHANNEL_ID, RANDOM_DCS_COMMAND, PARAMETER_VALUE);

  ASSERT_THAT(errorCode, Eq(DSIHost::ErrorCode::OK));
  ASSERT_THAT(virtualDSIHostPeripheral.GHCR, bitsValueMatcher);
}