#ifndef MFXSTM32L152_H
#define MFXSTM32L152_H

#include "I2C.h"
#include "GPIO.h"
#include "SysTick.h"


class MFXSTM32L152
{
public:

  typedef void (*CallbackFunc)(void*);

  MFXSTM32L152(I2C *I2CPtr, SysTick *sysTickPtr);

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ~MFXSTM32L152() = default;

  //! This enum class represents errors which can happen during method calls
  enum class ErrorCode : uint8_t
  {
    OK       = 0u,
    BUSY     = 1u,
    INTERNAL = 2u
  };

  struct MFXSTM32L152Config
  {
    uint16_t peripheralAddress; //!< MFXSTM32L152 I2C address
    GPIO *wakeUpPinGPIOPortPtr; //!< MFXSTM32L152 wakeup pin GPIO port
    GPIO::Pin wakeUpPin;        //!< MFXSTM32L152 wakeup pin
  };

  enum class Interrupt : uint8_t
  {
    GPIO              = 0u, //!< GPIO interrupt
    IDD               = 1u, //!< IDD interrupt
    ERROR             = 2u, //!< Error interrupt
    TS_TOUCH_DETECTED = 3u, //!< TS touch detected interrupt
    TS_FIFO_NOT_EMPTY = 4u, //!< TS FIFO not empty interrupt
    TS_FIFO_THRESHOLD = 5u, //!< TS FIFO threshold reached interrupt
    TS_FIFO_FULL      = 6u, //!< TS FIFO full interrupt
    TS_FIFO_OVERFLOW  = 7u  //!< TS FIFO overflow interrupt
  };

  //! This enum class represents MFXSTM32L152 GPIO pin.
  enum class GPIOPin : uint8_t
  {
    PIN0  = 0u,
    PIN1  = 1u,
    PIN2  = 2u,
    PIN3  = 3u,
    PIN4  = 4u,
    PIN5  = 5u,
    PIN6  = 6u,
    PIN7  = 7u,
    PIN8  = 8u,
    PIN9  = 9u,
    PIN10 = 10u,
    PIN11 = 11u,
    PIN12 = 12u,
    PIN13 = 13u,
    PIN14 = 14u,
    PIN15 = 15u,
    PIN16 = 16u,
    PIN17 = 17u,
    PIN18 = 18u,
    PIN19 = 19u,
    PIN20 = 20u,
    PIN21 = 21u,
    PIN22 = 22u,
    PIN23 = 23u
  };

  //! This enum class represents state of GPIO pin.
  enum class GPIOPinState : uint8_t
  {
    LOW  = 0b0, //< GPIO pin connected to GND
    HIGH = 0b1  //< GPIO pin connected to VDD
  };

  /**
   * This enum class represents mode of GPIO pin.
   *
   * MFXSTM32L152 GPIO pins can be in one of the next 3 modes:
   *  a) INPUT     - TODO input mode,
   *  b) OUTPUT    - TODO output mode,
   *  c) INTERRUPT - TODO interrupt mode,
   */
  enum class GPIOPinMode : uint8_t
  {
    INPUT     = 0b00,
    OUTPUT    = 0b01,
    INTERRUPT = 0b10
  };

  /**
   * This enum class represents type of output configuration.
   * Output pin can be in push-pull or open-drain configuration.
   */
  enum class GPIOOutputType : uint8_t
  {
    PUSH_PULL  = 0b0,
    OPEN_DRAIN = 0b1
  };

  /**
   * This enum class represents pull resistor configuration of GPIO pin.
   *
   * Each GPIO pin has 2 internal resistors, one connected to VDD and one connected
   * to GND. You can choose to use of one them, or not to use them at all.
   *
   * They can be helpul, because in some cases (when wanted resistance is same as their)
   * you can use them instead of using an external resistor, when can save you some space
   * on PCB and reduce number of components at your product.
   */
  enum class GPIOPullConfig : uint8_t
  {
    PULL_DOWN = 0b00,
    PULL_UP   = 0b01,
    NO_PULL   = 0b10
  };

  enum class GPIOInterruptTrigger : uint8_t
  {
    LOW_LEVEL    = 0b00,
    HIGH_LEVEL   = 0b10,
    RISING_EDGE  = 0b11,
    FALLING_EDGE = 0b01
  };

  enum class IRQPinPolarity : uint8_t
  {
    LOW  = 0b0,
    HIGH = 0b1
  };

  /**
   * This structure contains information needed for pin configuration. Depending on the mode,
   * all or only some configuration parameters from the structure will be used:
   *
   *  a) INPUT     - PinMode, PullConfig
   *  b) OUTPUT    - PinMode, PullConfig, OutputType
   *  c) INTERRUPT - PinMode, PullConfig, OutputType
   */
  struct GPIOPinConfiguration
  {
    GPIOPinMode mode;
    GPIOOutputType outputType;
    GPIOPullConfig pullConfig;
    GPIOInterruptTrigger interruptTrigger;
  };

  struct IRQPinConfiguration
  {
    GPIOOutputType outputType;
    IRQPinPolarity polarity;
  };

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode init(MFXSTM32L152Config &mfxstm32l152Config);

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode wakeUp(void);

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode configureGPIOPin(GPIOPin pin, const GPIOPinConfiguration &pinConfiguration);

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode configureIRQPin(const IRQPinConfiguration &irqPinConfiguration);

  /**
   * @brief Metod sets GPIO pin state.
   *
   * @param[in] pin   - GPIO pin which state will be set.
   * @param[in] state - State to which pin will be set. For more info @see ::GPIO::PinState
   * @return ::ErrorCode::OK is returned in the case of success. In the case of failure,
   *         it returns corresponding error code from ::ErrorCode.
   */
#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode setGPIOPinState(GPIOPin pin, GPIOPinState state);

  /**
   * @brief Metod gets GPIO pin state.
   *
   * @param[in]  pin   - GPIO pin which state will be get.
   * @param[out] state - Reference to variable in which pin state will be written.
   * @return ::ErrorCode::OK is returned in the case of success. In the case of failure,
   *         it returns corresponding error code from ::ErrorCode.
   */
#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode getGPIOPinState(GPIOPin pin, GPIOPinState &state);

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode enableGPIO(void);

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode enableInterrupt(Interrupt interrupt);

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode getID(uint8_t &id);

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode registerGPIOInterruptCallback(GPIOPin pin, CallbackFunc callback, void *callbackArgument);

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode runtimeTask(void);

private:

  static constexpr uint8_t SIZE_OF_MFXSTM32L152_REGISTER_IN_BITS = 8u;

  enum class RegisterAddress : uint8_t
  {
    ID               = 0x00u, //!< ID register (READ)
    FIRMWARE_VERSION = 0x01u, //!< Firmware version register (READ)
    IRQ_PENDING      = 0x08u, //!< IRQ pending register (READ)
    GPIO_IRQ_PENDING = 0x0Cu, //!< GPIO IRQ pending register (READ)
    GPIO_STATE       = 0x10u, //!< GPIO state register (READ)
    SYSTEM_CONTROL   = 0x40u, //!< System control register (READ/WRITE)
    IRQ_PIN_CONFIG   = 0x41u, //!< IRQ pin configuration register (WRITE)
    IRQ_SRC_EN       = 0x42u, //!< IRQ source enable register (READ/WRITE)
    IRQ_ACK          = 0x44u, //!< IRQ interrupt acknowledge register (WRITE)
    GPIO_IRQ_EN      = 0x48u, //!< GPIO interrupt enable register (READ/WRITE)
    GPIO_IRQ_EVT     = 0x4Cu, //!< GPIO interrupt event type register (READ/WRITE)
    GPIO_IRQ_ATYPE   = 0x50u, //!< GPIO interrupt activity type register (READ/WRITE)
    GPIO_IRQ_ACK     = 0x54u, //!< GPIO interrupt acknowledge register (WRITE)
    GPIO_DIRECTION   = 0x60u, //!< GPIO direction register (READ/WRITE)
    GPIO_TYPE        = 0x64u, //!< GPIO type register (READ/WRITE)
    GPIO_PUPD        = 0x68u, //!< GPIO PUPD register (READ/WRITE)
    GPIO_SET_HIGH    = 0x6Cu, //!< GPIO set pin state to HIGH register (WRITE)
    GPIO_SET_LOW     = 0x70u, //!< GPIO set pin state to LOW register (WRITE)
  };

  //! All possible valid values of ID
  enum class ID : uint8_t
  {
    ID_1 = 0x7Bu,
    ID_2 = 0x79u
  };

  //! System control (TODO)
  enum class SystemControl : uint8_t
  {
    GPIO_ENABLE           = 0u, //!< GPIO enable
    TS_ENABLE             = 1u, //!< TS enable
    IDD_ENABLE            = 2u, //!< IDD enable
    ALTERNATE_GPIO_ENABLE = 3u, //!< Alternate GPIO enable
    STANDY                = 6u, //!< Put in Standy mode
    SWRST                 = 7u, //!< Soft reset
  };

  struct Interrupts
  {
    inline bool isInterruptPending(Interrupt interrupt) const
    {
      return MemoryUtility<uint8_t>::isBitSet(interruptPendingBitField, static_cast<uint8_t>(interrupt));
    }

    uint8_t interruptPendingBitField;
  };

  struct GPIOInterrupts
  {
    static constexpr uint8_t NUMBER_OF_PENDING_INTERRUPT_BITFIELDS = 3u;

    inline bool isGPIOPinInterruptPending(GPIOPin pin) const
    {
      return MemoryUtility<uint8_t>::isBitSet(
        interruptPendingBitField[static_cast<uint8_t>(pin) / SIZE_OF_MFXSTM32L152_REGISTER_IN_BITS],
        (static_cast<uint8_t>(pin) % SIZE_OF_MFXSTM32L152_REGISTER_IN_BITS));
    }

    uint8_t interruptPendingBitField[NUMBER_OF_PENDING_INTERRUPT_BITFIELDS];
  };

  RegisterAddress mapToRegisterAddress(RegisterAddress baseRegisterAddress, GPIOPin pin);

  ErrorCode setGPIOPinDirection(GPIOPin pin, GPIOPinMode mode);
  ErrorCode setGPIOPinType(GPIOPin pin, GPIOOutputType outputType);
  ErrorCode setGPIOPinType(GPIOPin pin, GPIOPullConfig pullConfig);
  ErrorCode setGPIOPullConfig(GPIOPin pin, GPIOPullConfig pullConfig);
  ErrorCode setGPIOPinInterruptEventType(GPIOPin pin, GPIOInterruptTrigger interruptTrigger);
  ErrorCode setGPIOPinInterruptActivityType(GPIOPin pin, GPIOInterruptTrigger interruptTrigger);

  ErrorCode configureGPIOPinToInputDirection(GPIOPin pin);
  ErrorCode configureGPIOPinToOutputDirection(GPIOPin pin);

  ErrorCode configureGPIOPinToOpenDrainOutputType(GPIOPin pin);
  ErrorCode configureGPIOPinToPushPullOutputType(GPIOPin pin);

  ErrorCode configureGPIOPinToNoPullInputType(GPIOPin pin);
  ErrorCode configureGPIOPinToPullInputType(GPIOPin pin);

  ErrorCode configureGPIOPullConfigToPullUp(GPIOPin pin);
  ErrorCode configureGPIOPullConfigToPullDown(GPIOPin pin);

  ErrorCode setGPIOPinInterruptEventTypeToEdge(GPIOPin pin);
  ErrorCode setGPIOPinInterruptEventTypeToLevel(GPIOPin pin);

  ErrorCode setGPIOPinInterruptActivitTypeToHighLevelOrRisingEdge(GPIOPin pin);
  ErrorCode setGPIOPinInterruptActivitTypeToLowLevelOrFallingEdge(GPIOPin pin);

  ErrorCode enableGPIOPinInterrupt(GPIOPin pin);
  ErrorCode disableGPIOPinInterrupt(GPIOPin pin);

  ErrorCode setGPIOPinStateToHigh(GPIOPin pin);
  ErrorCode setGPIOPinStateToLow(GPIOPin pin);

  ErrorCode getPendingInterrupts(Interrupts &interrupts);
  ErrorCode clearPendingInterrupts(const Interrupts &interrupts);

  ErrorCode getPendingGPIOInterrupts(GPIOInterrupts &gpioInterrupts);
  ErrorCode clearPendingGPIOInterrupts(const GPIOInterrupts &gpioInterrupts);

  void callAllPendgingGPIOInterruptsCallbacks(const GPIOInterrupts &gpioInterrupts);

  void waitMs(uint64_t periodToWaitInMs);

  static ErrorCode mapToErrorCode(I2C::ErrorCode i2cErrorCode);

  void setBitsInRegister(uint8_t &registerValue, uint8_t bitPosition);
  template<typename... Args>
  void setBitsInRegister(uint8_t &registerValue, uint8_t bitPosition, Args... args);
  template<typename... Args>
  ErrorCode setBitsInRegister(RegisterAddress registerAddress, Args... args);

  void resetBitsInRegister(uint8_t &registerValue, uint8_t bitPosition);
  template<typename... Args>
  void resetBitsInRegister(uint8_t &registerValue, uint8_t bitPosition, Args... args);
  template<typename... Args>
  ErrorCode resetBitsInRegister(RegisterAddress registerAddress, Args... arg);

  ErrorCode readRegister(uint8_t registerAddress, void *messagePtr, uint32_t messageLen);
  ErrorCode writeRegister(uint8_t registerAddress, const void *messagePtr, uint32_t messageLen);

  //! TODO add doxy comment
  CallbackFunc m_callback[24]  = { nullptr };
  void *m_callbackArgument[24] = { nullptr };

  //! MFXSTM32L152 configuration
  MFXSTM32L152Config m_configuration;

  //! Pointer to I2C, used to communicate with MFXSTM32L152
  I2C *m_I2CPtr;

  //! Pointer to SysTick
  SysTick *m_sysTickPtr;
};

#endif // #ifndef MFXSTM32L152_H