#ifndef FT3267_H
#define FT3267_H

#include "I2C.h"
#include "SysTick.h"


class FT3267
{
public:

  static constexpr uint8_t MAX_NUMBER_OF_TOUCHES = 2u;

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

  enum class TouchEvent : uint8_t
  {
    PRESS_DOWN = 0b00,
    LIFT_UP    = 0b01,
    CONTACT    = 0b10,
    NO_EVENT   = 0b11
  };

  struct TouchPosition
  {
    uint16_t x;
    uint16_t y;
  };

  struct TouchPoint
  {
    TouchPosition position;
    TouchEvent event;
  };

  struct TouchEventInfo
  {
    uint8_t touchCount;
    TouchPoint touchPoints[MAX_NUMBER_OF_TOUCHES];
  };

  typedef void (*TouchEventCallbackFunc)(void*, TouchEventInfo);

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

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode registerTouchEventCallback(TouchEventCallbackFunc callback, void *callbackArgument);

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode runtimeTask(void);

private:

  static constexpr uint8_t BITS_IN_BYTE = 8u;

  enum class RegisterAddress : uint8_t
  {
    TOUCH_DATA_STATUS   = 0x02u, //!< Touch data status register (READ)
    TOUCH_POINT_1_INFO  = 0x03u, //!< Touch point 1 information register(s) (READ)
    TOUCH_POINT_2_INFO  = 0x09u, //!< Touch point 2 information register(s) (READ)
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

  ErrorCode getTouchCount(uint8_t &touchCount);
  ErrorCode getTouchPoint1(TouchPoint &touchPoint);
  ErrorCode getTouchPoint2(TouchPoint &touchPoint);

  ErrorCode readRegister(RegisterAddress registerAddress, void *messagePtr, uint32_t messageLen);
  ErrorCode writeRegister(RegisterAddress registerAddress, const void *messagePtr, uint32_t messageLen);
  ErrorCode mapToErrorCode(I2C::ErrorCode i2cErrorCode);

  void waitMs(uint64_t periodToWaitInMs);

  static uint16_t getTouchPositionXCoordinate(uint8_t touchPointXInfoRegisterVal[4]);
  static uint16_t getTouchPositionYCoordinate(uint8_t touchPointXInfoRegisterVal[4]);
  static TouchEvent getTouchEvent(uint8_t touchPointXInfoRegisterVal[4]);

  //! TODO add doxy comment
  TouchEventCallbackFunc m_callback  = nullptr;
  void *m_callbackArgument = nullptr;

  //! MFXSTM32L152 configuration
  FT3267Config m_configuration;

  //! Pointer to I2C, used to communicate with FT3267
  I2C *m_I2CPtr;

  //! Pointer to SysTick
  SysTick *m_sysTickPtr;
};

#endif // #ifndef FT3267_H