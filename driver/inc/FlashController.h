#ifndef FLASH_CONTROLLER_H
#define FLASH_CONTROLLER_H

#include "Peripheral.h"


class FlashController
{
public:

  FlashController(FLASH_TypeDef *FLASHPeripheralPtr);

  enum class Latency : uint8_t
  {
    WAIT_STATE_0 = 0u,
    WAIT_STATE_1 = 1u,
    WAIT_STATE_2 = 2u,
    WAIT_STATE_3 = 3u,
    WAIT_STATE_4 = 4u,
    WAIT_STATE_5 = 5u
  };

  void setFlashAccessLatency(Latency latency);

  inline Peripheral getPeripheralTag(void) const
  {
    return static_cast<Peripheral>(reinterpret_cast<uintptr_t>(const_cast<FLASH_TypeDef*>(m_FLASHPeripheralPtr)));
  }

private:

  FLASH_TypeDef *m_FLASHPeripheralPtr;
};

#endif // #ifndef FLASH_CONTROLLER_H