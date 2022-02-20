#include "FlashController.h"
#include "MemoryUtility.h"
#include "MemoryAccess.h"
#include "DriverTest.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"


using namespace ::testing;


class AFlashController : public DriverTest
{
public:

  //! Based on real reset values for FLASH registers (source STM32L4R9 reference manual)
  static constexpr uint32_t FLASH_ACR_RESET_VALUE      = 0x00000600;
  static constexpr uint32_t FLASH_PDKEYR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t FLASH_KEYR_RESET_VALUE     = 0x00000000;
  static constexpr uint32_t FLASH_OPTKEYR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t FLASH_SR_RESET_VALUE       = 0x00000000;
  static constexpr uint32_t FLASH_CR_RESET_VALUE       = 0xC0000000;
  static constexpr uint32_t FLASH_ECCR_RESET_VALUE     = 0x00000000;
  static constexpr uint32_t FLASH_OPTR_RESET_VALUE     = 0xFFEFF8AA;
  static constexpr uint32_t FLASH_PCROP1SR_RESET_VALUE = 0xFFFFFFFF;
  static constexpr uint32_t FLASH_PCROP1ER_RESET_VALUE = 0x0FF00000;
  static constexpr uint32_t FLASH_WRP1AR_RESET_VALUE   = 0xFF00FF00;
  static constexpr uint32_t FLASH_WRP1BR_RESET_VALUE   = 0xFF00FF00;
  static constexpr uint32_t FLASH_PCROP2SR_RESET_VALUE = 0xFFFFFFFF;
  static constexpr uint32_t FLASH_PCROP2ER_RESET_VALUE = 0x0FF00000;
  static constexpr uint32_t FLASH_WRP2AR_RESET_VALUE   = 0xFF00FF00;
  static constexpr uint32_t FLASH_WRP2BR_RESET_VALUE   = 0xFF00FF00;
  static constexpr uint32_t FLASH_CFGR_RESET_VALUE     = 0x00000000;

  FLASH_TypeDef virtualFLASHPeripheral;
  FlashController virtualFlashController = FlashController(&virtualFLASHPeripheral);

  void SetUp() override;
  void TearDown() override;
};

void AFlashController::SetUp()
{
  DriverTest::SetUp();

  // set values of virtual FLASH peripheral to reset values
  virtualFLASHPeripheral.ACR      = FLASH_ACR_RESET_VALUE;
  virtualFLASHPeripheral.PDKEYR   = FLASH_PDKEYR_RESET_VALUE;
  virtualFLASHPeripheral.KEYR     = FLASH_KEYR_RESET_VALUE;
  virtualFLASHPeripheral.OPTKEYR  = FLASH_OPTKEYR_RESET_VALUE;
  virtualFLASHPeripheral.SR       = FLASH_SR_RESET_VALUE;
  virtualFLASHPeripheral.CR       = FLASH_CR_RESET_VALUE;
  virtualFLASHPeripheral.ECCR     = FLASH_ECCR_RESET_VALUE;
  virtualFLASHPeripheral.OPTR     = FLASH_OPTR_RESET_VALUE;
  virtualFLASHPeripheral.PCROP1SR = FLASH_PCROP1SR_RESET_VALUE;
  virtualFLASHPeripheral.PCROP1ER = FLASH_PCROP1ER_RESET_VALUE;
  virtualFLASHPeripheral.WRP1AR   = FLASH_WRP1AR_RESET_VALUE;
  virtualFLASHPeripheral.WRP1BR   = FLASH_WRP1BR_RESET_VALUE;
  virtualFLASHPeripheral.PCROP2SR = FLASH_PCROP2SR_RESET_VALUE;
  virtualFLASHPeripheral.PCROP2ER = FLASH_PCROP2ER_RESET_VALUE;
  virtualFLASHPeripheral.WRP2AR   = FLASH_WRP2AR_RESET_VALUE;
  virtualFLASHPeripheral.WRP2BR   = FLASH_WRP2BR_RESET_VALUE;
  virtualFLASHPeripheral.CFGR     = FLASH_CFGR_RESET_VALUE;
}

void AFlashController::TearDown()
{
  DriverTest::TearDown();
}


TEST_F(AFlashController, GetPeripheralTagReturnsPointerToUnderlayingFLASHPeripheralCastedToPeripheralType)
{
  ASSERT_THAT(virtualFlashController.getPeripheralTag(),
    Eq(static_cast<Peripheral>(reinterpret_cast<uintptr_t>(&virtualFLASHPeripheral))));
}

TEST_F(AFlashController, SetFlashAccessLatency)
{
  constexpr uint32_t FLASH_ACR_LATENCY_POSITION = 0u;
  constexpr uint32_t FLASH_ACR_LATENCY_SIZE = 4u;
  constexpr uint32_t EXPECTED_FLASH_ACR_LATENCY_VALUE = static_cast<uint32_t>(FlashController::Latency::WAIT_STATE_3);
  auto bitsValueMatcher =
    BitsHaveValue(FLASH_ACR_LATENCY_POSITION, FLASH_ACR_LATENCY_SIZE, EXPECTED_FLASH_ACR_LATENCY_VALUE);
  expectRegisterSetOnlyOnce(&(virtualFLASHPeripheral.ACR), bitsValueMatcher);

  virtualFlashController.setFlashAccessLatency(FlashController::Latency::WAIT_STATE_3);
}

