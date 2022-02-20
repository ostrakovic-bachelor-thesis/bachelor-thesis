#include "FlashController.h"
#include "RegisterUtility.h"


FlashController::FlashController(FLASH_TypeDef *FLASHPeripheralPtr):
  m_FLASHPeripheralPtr(FLASHPeripheralPtr)
{}

void FlashController::setFlashAccessLatency(Latency latency)
{
  constexpr uint32_t FLASH_ACR_LATENCY_POSITION    = 0u;
  constexpr uint32_t FLASH_ACR_LATENCY_NUM_OF_BITS = 4u;

  RegisterUtility<uint32_t>::setBitsInRegister(
    &(m_FLASHPeripheralPtr->ACR),
    FLASH_ACR_LATENCY_POSITION,
    FLASH_ACR_LATENCY_NUM_OF_BITS,
    static_cast<uint32_t>(latency));
}