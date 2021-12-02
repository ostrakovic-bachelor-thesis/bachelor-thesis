#ifndef FT3267_H
#define FT3267_H

#include "I2C.h"
#include "SysTick.h"


class FT3267
{
public:

  typedef void (*CallbackFunc)(void);

  FT3267(I2C *I2CPtr, SysTick *sysTickPtr);

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ~FT3267() = default;

  //! This enum class represents errors which can happen during method calls
  enum class ErrorCode : uint8_t
  {
    OK       = 0u,
    BUSY     = 1u,
    INTERNAL = 2u
  };

  struct FT3267Config
  {
    uint16_t peripheralAddress; //!< FT3267 I2C address
    bool performPowerOn;
    CallbackFunc setFT3267ResetLineToLowCallback;
    CallbackFunc setFT3267ResetLineToHighCallback;
  };

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode init(FT3267Config &ft3267Config);

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode getID(uint8_t &id);

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
ErrorCode getFirmwareID(uint8_t &firmwareId);

private:

  enum class RegisterAddress : uint8_t
  {
    OPERATION_MODE      = 0xA4u, //!< Operation mode register (WRITE)
    FIRMWARE_ID         = 0xA6u, //!< Firmware ID register (READ)
    CHIP_ID             = 0xA8u, //!< CHIP ID register (READ)
    GESTURE_MODE_ENABLE = 0xD0u, //!< Gesture mode enable register (WRITE)
  };

  enum class OperationMode : uint8_t
  {
    POLLING   = 0x0,
    INTERRUPT = 0x1
  };

  void powerOn(const FT3267Config &ft3267Config);

  ErrorCode setOperationMode(OperationMode operationMode);

  ErrorCode disableGestureMode(void);

  ErrorCode readRegister(RegisterAddress registerAddress, void *messagePtr, uint32_t messageLen);
  ErrorCode writeRegister(RegisterAddress registerAddress, const void *messagePtr, uint32_t messageLen);
  ErrorCode mapToErrorCode(I2C::ErrorCode i2cErrorCode);

  void waitMs(uint64_t periodToWaitInMs);

  //! MFXSTM32L152 configuration
  FT3267Config m_configuration;

  //! Pointer to I2C, used to communicate with FT3267
  I2C *m_I2CPtr;

  //! Pointer to SysTick
  SysTick *m_sysTickPtr;
};

#endif // #ifndef FT3267_H