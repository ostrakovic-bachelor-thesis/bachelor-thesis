#include "PowerControl.h"
#include "MemoryUtility.h"
#include "MemoryAccess.h"
#include "DriverTest.h"
#include "ResetControlMock.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"


using namespace ::testing;


class APowerControl : public DriverTest
{
public:

  //! Based on real reset values for PWR registers (source STM32L4R9 reference manual)
  static constexpr uint32_t PWR_CR1_RESET_VALUE   = 0x00000200;
  static constexpr uint32_t PWR_CR2_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t PWR_CR3_RESET_VALUE   = 0x00008000;
  static constexpr uint32_t PWR_CR4_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t PWR_SR1_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t PWR_SR2_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t PWR_SCR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t PWR_PUCRA_RESET_VALUE = 0x00000000;
  static constexpr uint32_t PWR_PDCRA_RESET_VALUE = 0x00000000;
  static constexpr uint32_t PWR_PUCRB_RESET_VALUE = 0x00000000;
  static constexpr uint32_t PWR_PDCRB_RESET_VALUE = 0x00000000;
  static constexpr uint32_t PWR_PUCRC_RESET_VALUE = 0x00000000;
  static constexpr uint32_t PWR_PDCRC_RESET_VALUE = 0x00000000;
  static constexpr uint32_t PWR_PUCRD_RESET_VALUE = 0x00000000;
  static constexpr uint32_t PWR_PDCRD_RESET_VALUE = 0x00000000;
  static constexpr uint32_t PWR_PUCRE_RESET_VALUE = 0x00000000;
  static constexpr uint32_t PWR_PDCRE_RESET_VALUE = 0x00000000;
  static constexpr uint32_t PWR_PUCRF_RESET_VALUE = 0x00000000;
  static constexpr uint32_t PWR_PDCRF_RESET_VALUE = 0x00000000;
  static constexpr uint32_t PWR_PUCRG_RESET_VALUE = 0x00000000;
  static constexpr uint32_t PWR_PDCRG_RESET_VALUE = 0x00000000;
  static constexpr uint32_t PWR_PUCRH_RESET_VALUE = 0x00000000;
  static constexpr uint32_t PWR_PDCRH_RESET_VALUE = 0x00000000;
  static constexpr uint32_t PWR_PUCRI_RESET_VALUE = 0x00000000;
  static constexpr uint32_t PWR_PDCRI_RESET_VALUE = 0x00000000;
  static constexpr uint32_t PWR_CR5_RESET_VALUE   = 0x00000100;

  PWR_TypeDef virtualPWRPeripheral;
  NiceMock<ResetControlMock> resetControlMock;
  PowerControl virtualPowerControl = PowerControl(&virtualPWRPeripheral, &resetControlMock);

  uint32_t m_pvmo2Counter;
  uint32_t m_vosfCounter;

  void setupSR2RegisterReadings(void);
  void setDynamicVoltageScalingRangeToHighPerformanceNormal(PWR_TypeDef &virtualPWRPeripheral);
  void setDynamicVoltageScalingRangeToHighPerformanceBoost(PWR_TypeDef &virtualPWRPeripheral);
  void setDynamicVoltageScalingRangeToLowPower(PWR_TypeDef &virtualPWRPeripheral);
  void assertThatLowPowerVoltageScalingRangeIsSelected(PowerControl &powerControl);
  void assertThatHighPerformanceNormalVoltageScalingRangeIsSelected(PowerControl &powerControl);
  void assertThatHighPerformanceBoostVoltageScalingRangeIsSelected(PowerControl &powerControl);

  void SetUp() override;
  void TearDown() override;
};

void APowerControl::SetUp()
{
  DriverTest::SetUp();

  m_pvmo2Counter = 1u;
  m_vosfCounter  = 1u;

  // set values of virtual PWR peripheral to reset values
  virtualPWRPeripheral.CR1   = PWR_CR1_RESET_VALUE;
  virtualPWRPeripheral.CR2   = PWR_CR2_RESET_VALUE;
  virtualPWRPeripheral.CR3   = PWR_CR3_RESET_VALUE;
  virtualPWRPeripheral.CR4   = PWR_CR4_RESET_VALUE;
  virtualPWRPeripheral.SR1   = PWR_SR1_RESET_VALUE;
  virtualPWRPeripheral.SR2   = PWR_SR2_RESET_VALUE;
  virtualPWRPeripheral.SCR   = PWR_SCR_RESET_VALUE;
  virtualPWRPeripheral.PUCRA = PWR_PUCRA_RESET_VALUE;
  virtualPWRPeripheral.PDCRA = PWR_PDCRA_RESET_VALUE;
  virtualPWRPeripheral.PUCRB = PWR_PUCRB_RESET_VALUE;
  virtualPWRPeripheral.PDCRB = PWR_PDCRB_RESET_VALUE;
  virtualPWRPeripheral.PUCRC = PWR_PUCRC_RESET_VALUE;
  virtualPWRPeripheral.PDCRC = PWR_PDCRC_RESET_VALUE;
  virtualPWRPeripheral.PUCRD = PWR_PUCRD_RESET_VALUE;
  virtualPWRPeripheral.PDCRD = PWR_PDCRD_RESET_VALUE;
  virtualPWRPeripheral.PUCRE = PWR_PUCRE_RESET_VALUE;
  virtualPWRPeripheral.PDCRE = PWR_PDCRE_RESET_VALUE;
  virtualPWRPeripheral.PUCRF = PWR_PUCRF_RESET_VALUE;
  virtualPWRPeripheral.PDCRF = PWR_PDCRF_RESET_VALUE;
  virtualPWRPeripheral.PUCRG = PWR_PUCRG_RESET_VALUE;
  virtualPWRPeripheral.PDCRG = PWR_PDCRG_RESET_VALUE;
  virtualPWRPeripheral.PUCRH = PWR_PUCRH_RESET_VALUE;
  virtualPWRPeripheral.PDCRH = PWR_PDCRH_RESET_VALUE;
  virtualPWRPeripheral.PUCRI = PWR_PUCRI_RESET_VALUE;
  virtualPWRPeripheral.PDCRI = PWR_PDCRI_RESET_VALUE;
  virtualPWRPeripheral.CR5   = PWR_CR5_RESET_VALUE;
}

void APowerControl::TearDown()
{
  DriverTest::TearDown();
}

void APowerControl::setupSR2RegisterReadings(void)
{
  constexpr uint32_t PWR_SR2_VOSF_POSITION  = 10u;
  constexpr uint32_t PWR_SR2_PVMO2_POSITION = 13u;

  virtualPWRPeripheral.SR2 =
    expectedRegVal(PWR_SR2_RESET_VALUE, PWR_SR2_PVMO2_POSITION, 1u, (((m_pvmo2Counter++) % 100u) != 0u));
  virtualPWRPeripheral.SR2 =
    expectedRegVal(virtualPWRPeripheral.SR2, PWR_SR2_VOSF_POSITION, 1u, (((m_vosfCounter++) % 100u) != 0u));

  ON_CALL(memoryAccessHook, getRegisterValue(&(virtualPWRPeripheral.SR2)))
    .WillByDefault([&](volatile const uint32_t *registerPtr)
    {
      virtualPWRPeripheral.SR2 =
        expectedRegVal(PWR_SR2_RESET_VALUE, PWR_SR2_PVMO2_POSITION, 1u, (((m_pvmo2Counter++) % 100u) != 0u));
      virtualPWRPeripheral.SR2 =
        expectedRegVal(virtualPWRPeripheral.SR2, PWR_SR2_VOSF_POSITION, 1u, (((m_vosfCounter++) % 100u) != 0u));

      return virtualPWRPeripheral.SR2;
    });
}

void APowerControl::setDynamicVoltageScalingRangeToHighPerformanceNormal(PWR_TypeDef &virtualPWRPeripheral)
{
  constexpr uint32_t PWR_CR1_VOS_POSITION     = 9u;
  constexpr uint32_t PWR_CR1_VOS_SIZE         = 2u;
  constexpr uint32_t PWR_CR1_VOS_VALUE_RANGE1 = 0x1;
  constexpr uint32_t PWR_CR5_R1MODE_POSITION  = 8u;
  constexpr uint32_t PWR_CR5_R1MODE_VALUE     = 0x1;
  virtualPWRPeripheral.CR1 =
    expectedRegVal(virtualPWRPeripheral.CR1, PWR_CR1_VOS_POSITION, PWR_CR1_VOS_SIZE, PWR_CR1_VOS_VALUE_RANGE1);
  virtualPWRPeripheral.CR5 =
    expectedRegVal(virtualPWRPeripheral.CR5, PWR_CR5_R1MODE_POSITION, 1u, PWR_CR5_R1MODE_VALUE);
}

void APowerControl::setDynamicVoltageScalingRangeToHighPerformanceBoost(PWR_TypeDef &virtualPWRPeripheral)
{
  constexpr uint32_t PWR_CR1_VOS_POSITION     = 9u;
  constexpr uint32_t PWR_CR1_VOS_SIZE         = 2u;
  constexpr uint32_t PWR_CR1_VOS_VALUE_RANGE1 = 0x1;
  constexpr uint32_t PWR_CR5_R1MODE_POSITION  = 8u;
  constexpr uint32_t PWR_CR5_R1MODE_VALUE     = 0x0;
  virtualPWRPeripheral.CR1 =
    expectedRegVal(virtualPWRPeripheral.CR1, PWR_CR1_VOS_POSITION, PWR_CR1_VOS_SIZE, PWR_CR1_VOS_VALUE_RANGE1);
  virtualPWRPeripheral.CR5 =
    expectedRegVal(virtualPWRPeripheral.CR5, PWR_CR5_R1MODE_POSITION, 1u, PWR_CR5_R1MODE_VALUE);
}

void APowerControl::setDynamicVoltageScalingRangeToLowPower(PWR_TypeDef &virtualPWRPeripheral)
{
  constexpr uint32_t PWR_CR1_VOS_POSITION     = 9u;
  constexpr uint32_t PWR_CR1_VOS_SIZE         = 2u;
  constexpr uint32_t PWR_CR1_VOS_VALUE_RANGE2 = 0x2;
  virtualPWRPeripheral.CR1 =
    expectedRegVal(virtualPWRPeripheral.CR1, PWR_CR1_VOS_POSITION, PWR_CR1_VOS_SIZE, PWR_CR1_VOS_VALUE_RANGE2);
}

void APowerControl::assertThatLowPowerVoltageScalingRangeIsSelected(PowerControl &powerControl)
{
  const PowerControl::DynamicVoltageScalingRange dynamicVoltageScalingRange =
    powerControl.getDynamicVoltageScalingRange();
  ASSERT_THAT(dynamicVoltageScalingRange, Eq(PowerControl::DynamicVoltageScalingRange::LOW_POWER));
}

void APowerControl::assertThatHighPerformanceNormalVoltageScalingRangeIsSelected(PowerControl &powerControl)
{
  const PowerControl::DynamicVoltageScalingRange dynamicVoltageScalingRange =
    powerControl.getDynamicVoltageScalingRange();
  ASSERT_THAT(dynamicVoltageScalingRange, Eq(PowerControl::DynamicVoltageScalingRange::HIGH_PERFORMANCE_NORMAL));
}

void APowerControl::assertThatHighPerformanceBoostVoltageScalingRangeIsSelected(PowerControl &powerControl)
{
  const PowerControl::DynamicVoltageScalingRange dynamicVoltageScalingRange =
    powerControl.getDynamicVoltageScalingRange();
  ASSERT_THAT(dynamicVoltageScalingRange, Eq(PowerControl::DynamicVoltageScalingRange::HIGH_PERFORMANCE_BOOST));
}


TEST_F(APowerControl, GetPeripheralTagReturnsPointerToUnderlayingPWRPeripheralCastedToPeripheralType)
{
  ASSERT_THAT(virtualPowerControl.getPeripheralTag(),
    Eq(static_cast<Peripheral>(reinterpret_cast<uintptr_t>(&virtualPWRPeripheral))));
}

TEST_F(APowerControl, InitTurnsOnPWRPeripheralClock)
{
  resetControlMock.setReturnErrorCode(ResetControl::ErrorCode::OK);
  EXPECT_CALL(resetControlMock, enablePeripheralClock(virtualPowerControl.getPeripheralTag()))
    .Times(1u);

  const PowerControl::ErrorCode errorCode = virtualPowerControl.init();

  ASSERT_THAT(errorCode, Eq(PowerControl::ErrorCode::OK));
}

TEST_F(APowerControl, InitFailsIfTurningOnOfPWRPeripheralClockFail)
{
  resetControlMock.setReturnErrorCode(ResetControl::ErrorCode::INTERNAL);
  EXPECT_CALL(resetControlMock, enablePeripheralClock(virtualPowerControl.getPeripheralTag()))
    .Times(1u);

  const PowerControl::ErrorCode errorCode = virtualPowerControl.init();

  ASSERT_THAT(errorCode, Eq(PowerControl::ErrorCode::CAN_NOT_TURN_ON_PERIPHERAL_CLOCK));
}

TEST_F(APowerControl, EnablePowerSupplyVDDIO2EnablesVDDIO2MonitoringBySettingPVME2BitInCR2Register)
{
  setupSR2RegisterReadings();
  constexpr uint32_t PWR_CR2_PVME2_POSITION = 5u;
  constexpr uint32_t EXPECTED_PWR_CR2_PVME2_VALUE = 0x1;
  auto bitValueMatcher =
    BitHasValue(PWR_CR2_PVME2_POSITION, EXPECTED_PWR_CR2_PVME2_VALUE);
  expectSpecificRegisterSetToBeCalledFirst(&(virtualPWRPeripheral.CR2), bitValueMatcher);

  virtualPowerControl.enablePowerSupplyVDDIO2();

  ASSERT_THAT(virtualPWRPeripheral.CR2, bitValueMatcher);
}

TEST_F(APowerControl, EnablePowerSupplyVDDIO2SetsIOSVBitInCR2RegisterWhenPVMO2BitInSR2RegisterBecomesSet)
{
  setupSR2RegisterReadings();
  constexpr uint32_t PWR_CR2_IOSV_POSITION = 9u;
  constexpr uint32_t EXPECTED_PWR_CR2_IOSV_VALUE = 0x1;
  auto bitValueMatcher =
    BitHasValue(PWR_CR2_IOSV_POSITION, EXPECTED_PWR_CR2_IOSV_VALUE);
  expectSpecificRegisterSetToBeCalledLast(&(virtualPWRPeripheral.CR2), bitValueMatcher);

  const PowerControl::ErrorCode errorCode = virtualPowerControl.enablePowerSupplyVDDIO2();

  ASSERT_THAT(errorCode, Eq(PowerControl::ErrorCode::OK));
  ASSERT_THAT(virtualPWRPeripheral.CR2, bitValueMatcher);
}

TEST_F(APowerControl, GetDynamicVoltageScalingRangeReturnsLowPowerIfVOSBitsInCR1RegisterAreSetToRange2)
{
  setDynamicVoltageScalingRangeToLowPower(virtualPWRPeripheral);

  const PowerControl::DynamicVoltageScalingRange dynamicVoltageScalingRange =
    virtualPowerControl.getDynamicVoltageScalingRange();

  ASSERT_THAT(dynamicVoltageScalingRange, Eq(PowerControl::DynamicVoltageScalingRange::LOW_POWER));
}

TEST_F(APowerControl, GetDynamicVoltageScalingRangeReturnsHighPerformanceNormalIfVOSBitsInCR1AreSetToRange1AndR1MODEBitInCR5IsOne)
{
  setDynamicVoltageScalingRangeToHighPerformanceNormal(virtualPWRPeripheral);

  const PowerControl::DynamicVoltageScalingRange dynamicVoltageScalingRange =
    virtualPowerControl.getDynamicVoltageScalingRange();

  ASSERT_THAT(dynamicVoltageScalingRange, Eq(PowerControl::DynamicVoltageScalingRange::HIGH_PERFORMANCE_NORMAL));
}

TEST_F(APowerControl, GetDynamicVoltageScalingRangeReturnsHighPerformanceBoostlIfVOSBitsInCR1AreSetToRange1AndR1MODEBitInCR5IsZero)
{
  setDynamicVoltageScalingRangeToHighPerformanceBoost(virtualPWRPeripheral);

  const PowerControl::DynamicVoltageScalingRange dynamicVoltageScalingRange =
    virtualPowerControl.getDynamicVoltageScalingRange();

  ASSERT_THAT(dynamicVoltageScalingRange, Eq(PowerControl::DynamicVoltageScalingRange::HIGH_PERFORMANCE_BOOST));
}

TEST_F(APowerControl, SetDynamicVoltageScalingRangeToLowPowerSetsVOSBitsInCR1RegisterToRange2Value)
{
  constexpr uint32_t PWR_CR1_VOS_POSITION = 9u;
  constexpr uint32_t PWR_CR1_VOS_SIZE = 2u;
  constexpr uint32_t PWR_CR1_VOS_VALUE_RANGE2 = 0x2;
  auto bitValueMatcher =
    BitsHaveValue(PWR_CR1_VOS_POSITION, PWR_CR1_VOS_SIZE, PWR_CR1_VOS_VALUE_RANGE2);
  expectRegisterSetOnlyOnce(&(virtualPWRPeripheral.CR1), bitValueMatcher);
  doesNotExpectRegisterSet(&(virtualPWRPeripheral.CR5), _);

  const PowerControl::ErrorCode errorCode =
    virtualPowerControl.setDynamicVoltageScalingRange(PowerControl::DynamicVoltageScalingRange::LOW_POWER);

  ASSERT_THAT(errorCode, Eq(PowerControl::ErrorCode::OK));
  assertThatLowPowerVoltageScalingRangeIsSelected(virtualPowerControl);
}

TEST_F(APowerControl, SetDynamicVoltageScalingRangeToHighPerformanceNormalDoesNothingIfItIsAlreadyInThatRange)
{
  setDynamicVoltageScalingRangeToHighPerformanceNormal(virtualPWRPeripheral);
  expectNoRegisterToChange();

  const PowerControl::ErrorCode errorCode =
    virtualPowerControl.setDynamicVoltageScalingRange(PowerControl::DynamicVoltageScalingRange::HIGH_PERFORMANCE_NORMAL);

  ASSERT_THAT(errorCode, Eq(PowerControl::ErrorCode::OK));
  assertThatHighPerformanceNormalVoltageScalingRangeIsSelected(virtualPowerControl);
}

TEST_F(APowerControl, SetDynamicVoltageScalingRangeToHighPerformanceBoostDoesNothingIfItIsAlreadyInThatRange)
{
  setDynamicVoltageScalingRangeToHighPerformanceBoost(virtualPWRPeripheral);
  expectNoRegisterToChange();

  const PowerControl::ErrorCode errorCode =
    virtualPowerControl.setDynamicVoltageScalingRange(PowerControl::DynamicVoltageScalingRange::HIGH_PERFORMANCE_BOOST);

  ASSERT_THAT(errorCode, Eq(PowerControl::ErrorCode::OK));
  assertThatHighPerformanceBoostVoltageScalingRangeIsSelected(virtualPowerControl);
}

TEST_F(APowerControl, SetDynamicVoltageScalingRangeToHighPerformanceNormalJustSetsR1MODEBitInCR5ToOneIfItIsInHighPerformanceBoostRange)
{
  constexpr uint32_t PWR_CR5_R1MODE_POSITION = 8u;
  constexpr uint32_t PWR_CR5_R1MODE_VALUE    = 0x1;
  auto bitValueMatcher = BitHasValue(PWR_CR5_R1MODE_POSITION, PWR_CR5_R1MODE_VALUE);
  setDynamicVoltageScalingRangeToHighPerformanceBoost(virtualPWRPeripheral);
  expectRegisterSetOnlyOnce(&(virtualPWRPeripheral.CR5), bitValueMatcher);
  doesNotExpectRegisterSet(&(virtualPWRPeripheral.CR1), _);

  const PowerControl::ErrorCode errorCode =
    virtualPowerControl.setDynamicVoltageScalingRange(PowerControl::DynamicVoltageScalingRange::HIGH_PERFORMANCE_NORMAL);

  ASSERT_THAT(errorCode, Eq(PowerControl::ErrorCode::OK));
  assertThatHighPerformanceNormalVoltageScalingRangeIsSelected(virtualPowerControl);
}

TEST_F(APowerControl, SetDynamicVoltageScalingRangeToHighPerformanceBoostJustSetsR1MODEBitInCR5ToZeroIfItIsInHighPerformanceNormalRange)
{
  constexpr uint32_t PWR_CR5_R1MODE_POSITION = 8u;
  constexpr uint32_t PWR_CR5_R1MODE_VALUE    = 0x0;
  auto bitValueMatcher = BitHasValue(PWR_CR5_R1MODE_POSITION, PWR_CR5_R1MODE_VALUE);
  setDynamicVoltageScalingRangeToHighPerformanceNormal(virtualPWRPeripheral);
  expectRegisterSetOnlyOnce(&(virtualPWRPeripheral.CR5), bitValueMatcher);
  doesNotExpectRegisterSet(&(virtualPWRPeripheral.CR1), _);

  const PowerControl::ErrorCode errorCode =
    virtualPowerControl.setDynamicVoltageScalingRange(PowerControl::DynamicVoltageScalingRange::HIGH_PERFORMANCE_BOOST);

  ASSERT_THAT(errorCode, Eq(PowerControl::ErrorCode::OK));
  assertThatHighPerformanceBoostVoltageScalingRangeIsSelected(virtualPowerControl);
}

TEST_F(APowerControl, SetDynamicVoltageScalingRangeToHighPerformanceNormalFirstSetsR1MODEBitInCR5ToOneIfItIsInLowPowerRange)
{
  constexpr uint32_t PWR_CR5_R1MODE_POSITION = 8u;
  constexpr uint32_t PWR_CR5_R1MODE_VALUE    = 0x1;
  auto bitValueMatcher = BitHasValue(PWR_CR5_R1MODE_POSITION, PWR_CR5_R1MODE_VALUE);
  setDynamicVoltageScalingRangeToLowPower(virtualPWRPeripheral);
  expectSpecificRegisterSetToBeCalledFirst(&(virtualPWRPeripheral.CR5), bitValueMatcher);

  const PowerControl::ErrorCode errorCode =
    virtualPowerControl.setDynamicVoltageScalingRange(PowerControl::DynamicVoltageScalingRange::HIGH_PERFORMANCE_NORMAL);

  ASSERT_THAT(errorCode, Eq(PowerControl::ErrorCode::OK));
  assertThatHighPerformanceNormalVoltageScalingRangeIsSelected(virtualPowerControl);
}

TEST_F(APowerControl, SetDynamicVoltageScalingRangeToHighPerformanceBoostFirstSetsR1MODEBitInCR5ToZeroIfItIsInLowPowerRange)
{
  constexpr uint32_t PWR_CR5_R1MODE_POSITION = 8u;
  constexpr uint32_t PWR_CR5_R1MODE_VALUE    = 0x0;
  auto bitValueMatcher = BitHasValue(PWR_CR5_R1MODE_POSITION, PWR_CR5_R1MODE_VALUE);
  setDynamicVoltageScalingRangeToLowPower(virtualPWRPeripheral);
  expectSpecificRegisterSetToBeCalledFirst(&(virtualPWRPeripheral.CR5), bitValueMatcher);

  const PowerControl::ErrorCode errorCode =
    virtualPowerControl.setDynamicVoltageScalingRange(PowerControl::DynamicVoltageScalingRange::HIGH_PERFORMANCE_BOOST);

  ASSERT_THAT(errorCode, Eq(PowerControl::ErrorCode::OK));
  assertThatHighPerformanceBoostVoltageScalingRangeIsSelected(virtualPowerControl);
}

TEST_F(APowerControl, SetDynamicVoltageScalingRangeToHighPerformanceNormalSetsVOSBitsInCR1ToRange1IfItIsInLowPowerRange)
{
  constexpr uint32_t PWR_CR1_VOS_POSITION = 9u;
  constexpr uint32_t PWR_CR1_VOS_SIZE = 2u;
  constexpr uint32_t PWR_CR1_VOS_VALUE_RANGE1 = 0x1;
  auto bitsValueMatcher = BitsHaveValue(PWR_CR1_VOS_POSITION, PWR_CR1_VOS_SIZE, PWR_CR1_VOS_VALUE_RANGE1);
  setDynamicVoltageScalingRangeToLowPower(virtualPWRPeripheral);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualPWRPeripheral.CR1), bitsValueMatcher);

  const PowerControl::ErrorCode errorCode =
    virtualPowerControl.setDynamicVoltageScalingRange(PowerControl::DynamicVoltageScalingRange::HIGH_PERFORMANCE_NORMAL);

  ASSERT_THAT(errorCode, Eq(PowerControl::ErrorCode::OK));
  assertThatHighPerformanceNormalVoltageScalingRangeIsSelected(virtualPowerControl);
}

TEST_F(APowerControl, SetDynamicVoltageScalingRangeToHighPerformanceBoostSetsVOSBitsInCR1ToRange1IfItIsInLowPowerRange)
{
  constexpr uint32_t PWR_CR1_VOS_POSITION = 9u;
  constexpr uint32_t PWR_CR1_VOS_SIZE = 2u;
  constexpr uint32_t PWR_CR1_VOS_VALUE_RANGE1 = 0x1;
  auto bitsValueMatcher = BitsHaveValue(PWR_CR1_VOS_POSITION, PWR_CR1_VOS_SIZE, PWR_CR1_VOS_VALUE_RANGE1);
  setDynamicVoltageScalingRangeToLowPower(virtualPWRPeripheral);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualPWRPeripheral.CR1), bitsValueMatcher);

  const PowerControl::ErrorCode errorCode =
    virtualPowerControl.setDynamicVoltageScalingRange(PowerControl::DynamicVoltageScalingRange::HIGH_PERFORMANCE_BOOST);

  ASSERT_THAT(errorCode, Eq(PowerControl::ErrorCode::OK));
  assertThatHighPerformanceBoostVoltageScalingRangeIsSelected(virtualPowerControl);
}

TEST_F(APowerControl, SetDynamicVoltageScalingRangeWhenSwitchingFromLowPowerToHighPerformanceNormalWaitsForVOSFInSR2ToBecomeZero)
{
  constexpr uint32_t PWR_SR2_VOSF_POSITION = 10u;
  constexpr uint32_t EXPECTED_PWR_SR2_VOSF_VALUE = 0x0;
  setupSR2RegisterReadings();
  setDynamicVoltageScalingRangeToLowPower(virtualPWRPeripheral);
  auto bitValueMatcher = BitHasValue(PWR_SR2_VOSF_POSITION, EXPECTED_PWR_SR2_VOSF_VALUE);

  virtualPowerControl.setDynamicVoltageScalingRange(PowerControl::DynamicVoltageScalingRange::HIGH_PERFORMANCE_NORMAL);

  ASSERT_THAT(virtualPWRPeripheral.SR2, bitValueMatcher);
}

TEST_F(APowerControl, SetDynamicVoltageScalingRangeWhenSwitchingFromLowPowerToHighPerformanceBoostWaitsForVOSFInSR2ToBecomeZero)
{
  constexpr uint32_t PWR_SR2_VOSF_POSITION = 10u;
  constexpr uint32_t EXPECTED_PWR_SR2_VOSF_VALUE = 0x0;
  setupSR2RegisterReadings();
  setDynamicVoltageScalingRangeToLowPower(virtualPWRPeripheral);
  auto bitValueMatcher = BitHasValue(PWR_SR2_VOSF_POSITION, EXPECTED_PWR_SR2_VOSF_VALUE);

  virtualPowerControl.setDynamicVoltageScalingRange(PowerControl::DynamicVoltageScalingRange::HIGH_PERFORMANCE_BOOST);

  ASSERT_THAT(virtualPWRPeripheral.SR2, bitValueMatcher);
}