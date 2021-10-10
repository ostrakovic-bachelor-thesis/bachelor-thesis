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

  void setupSR2RegisterReadings(void);

  void SetUp() override;
  void TearDown() override;
};

void APowerControl::SetUp()
{
  DriverTest::SetUp();

  m_pvmo2Counter = 1u;

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
  ON_CALL(memoryAccessHook, getRegisterValue(&(virtualPWRPeripheral.SR2)))
    .WillByDefault([&](volatile const uint32_t *registerPtr)
    {
      constexpr uint32_t PWR_SR2_PVMO2_POSITION = 13u;
      virtualPWRPeripheral.SR2 =
        expectedRegVal(PWR_SR2_RESET_VALUE, PWR_SR2_PVMO2_POSITION, 1u, (((m_pvmo2Counter++) % 100u) != 0u));

      return virtualPWRPeripheral.SR2;
    });
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
