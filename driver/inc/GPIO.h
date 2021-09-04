#ifndef GPIO_H
#define GPIO_H

#include "stm32l4r9xx.h"
#include <cstdint>


class GPIO
{
public:

  /**
   * @brief Constructor of GPIO class instance.
   *
   * @param[in] - Pointer to a GPIO port.
   */
  GPIO(GPIO_TypeDef *GPIOPortPtr);

  //! This enum class represents errors which can happen during method calls
  enum class ErrorCode : uint8_t
  {
    OK                             = 0u,
    INVALID_PIN_VALUE              = 1u,
    PIN_CONFIG_PARAM_INVALID_VALUE = 2u,
    PIN_MODE_IS_NOT_APPROPRIATE    = 3u,
    INVALID_PIN_STATE_VALUE        = 4u,
  };

  //! This enum class represents GPIO pin.
  enum class Pin : uint8_t
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
    PIN15 = 15u
  };

  //! This enum class represents state of GPIO pin.
  enum class PinState : uint8_t
  {
    LOW  = 0b0, //< GPIO pin connected to GND
    HIGH = 0b1  //< GPIO pin connected to VDD
  };

  /**
   * This enum class represents mode of GPIO pin.
   *
   * STM32 GPIO pins can be in 4 states:
   *  a) INPUT  - input mode, GPIO is driven directly from GPIO peripheral instance
   *  b) OUTPUT - output mode, GPIO is driven directly from GPIO peripheral instance
   *  c) AF     - alternate function mode, GPIO is redirected to other peripherals,
   *              which operates with digital inputs/outputs (for example UART and SPI)
   *  d) ANALOG - analog mode, GPIO is redirected to other peripherals, which operates
   *              with analog inputs/outputs. (for example ADC and DAC)
   */
  enum class PinMode : uint8_t
  {
    INPUT  = 0b00,
    OUTPUT = 0b01,
    AF     = 0b10,
    ANALOG = 0b11
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
  enum class PullConfig : uint8_t
  {
    NO_PULL   = 0b00,
    PULL_UP   = 0b01,
    PULL_DOWN = 0b10
  };

  /**
   * This enum class represents maximum speed of transitions between states of GPIO output.
   * Therefore, it limits maximum frequency of switching between GPIO states which can be achieved.
   *
   * For more quantitative information about this, please read device datasheet.
   */
  enum class OutputSpeed : uint8_t
  {
    LOW       = 0b00,
    MEDIUM    = 0b01,
    HIGH      = 0b10,
    VERY_HIGH = 0b11
  };

  /**
   * This enum class represents type of output configuration.
   * Pin can be in push-pull or open-drain type configuration.
   */
  enum class OutputType : uint8_t
  {
    PUSH_PULL  = 0b0,
    OPEN_DRAIN = 0b1
  };

  /**
   * This enum class represents alternate function which is assigned to GPIO pin.
   *
   * It will be to complex and expensive for producer of MCU to route each GPIO pin to each
   * peripheral inputs/outputs, mostly due large number of GPIO pins. They decided to reduce
   * routing of GPIO pin up to 16 different peripheral's inputs/outputs. More information about
   * routing between pins and peripheral's inputs/outputs can be found in device datasheet. When
   * you want to route some peripheral input/output to GPIO pin, you will first observe table provided in
   * device datasheet and then find out a list of GPIO pin to which this peripheral's input/output can be
   * routed. In this table, you will also find out alternate function number, which then will be used in
   * GPIO configuration process to set up that routing.
   */
  enum class AlternateFunction : uint8_t
  {
    AF0  = 0,
    AF1  = 1,
    AF2  = 2,
    AF3  = 3,
    AF4  = 4,
    AF5  = 5,
    AF6  = 6,
    AF7  = 7,
    AF8  = 8,
    AF9  = 9,
    AF10 = 10,
    AF11 = 11,
    AF12 = 12,
    AF13 = 13,
    AF14 = 14,
    AF15 = 15
  };

  /**
   * This structure contains information needed for pin configuration. Depending on the mode,
   * all or only some configuration paramters from the structure will be used:
   *
   *  a) INPUT  - PinMode, PullConfig
   *  b) OUTPUT - PinMode, PullConfig, OutputType, OutputSpeed
   *  c) AF     - PinMode, PullConfig, OutputType, OutputSpeed, AlternateFunction
   *  d) ANALOG - PinMode, PullConfig (PullConfig::NO_PULL has to be used)
   */
  struct PinConfiguration
  {
    PinMode mode;
    PullConfig pullConfig;
    OutputSpeed outputSpeed;
    OutputType outputType;
    AlternateFunction alternateFunction;
  };

  /**
   * @brief Method performs pin configuration.
   *
   * @param[in] pin              - GPIO pin which will be configured.
   * @param[in] pinConfiguration - Pin configuration. For more info @see ::GPIO::PinConfiguration
   * @return ::ErrorCode::OK is returned in the case of success. In the case of failure,
   *         it returns corresponding error code from ::ErrorCode.
   */
  ErrorCode configurePin(Pin pin, const PinConfiguration &pinConfiguration);

  /**
   * @brief Metod sets pin state.
   *
   * @param[in] pin   - GPIO pin which state will be set.
   * @param[in] state - State to which pin will be set. For more info @see ::GPIO::PinState
   * @return ::ErrorCode::OK is returned in the case of success. In the case of failure,
   *         it returns corresponding error code from ::ErrorCode.
   */
  ErrorCode setPinState(Pin pin, PinState state);

  /**
   * @brief Metod gets pin state.
   *
   * @param[in]  pin   - GPIO pin which state will be get.
   * @param[out] state - Reference to variable in which pin state will be written.
   * @return ::ErrorCode::OK is returned in the case of success. In the case of failure,
   *         it returns corresponding error code from ::ErrorCode.
   */
  ErrorCode getPinState(Pin pin, PinState &state) const;

  /**
   * @brief Metod gets pin mode.
   *
   * @param[in]  pin   - GPIO pin which mode will be get.
   * @param[out] state - Reference to variable in which pin mode will be written.
   * @return ::ErrorCode::OK is returned in the case of success. In the case of failure,
   *         it returns corresponding error code from ::ErrorCode.
   */
  ErrorCode getPinMode(Pin pin, PinMode &pinMode) const;

#ifdef UNIT_TEST
  /**
   * @brief Method gets raw pointer to underlaying GPIO peripheral instance.
   *
   * @return Pointer to underlaying GPIO peripheral instance.
   */
  inline void* getRawPointer(void) const
  {
    return reinterpret_cast<void*>(m_GPIOPortPtr);
  }
#endif // #ifdef UNIT_TEST

private:

  void setPinMode(Pin pin, PinMode mode);

  void setPullConfig(Pin pin, PullConfig pullConfig);

  void setOutputSpeed(Pin pin, OutputSpeed outputSpeed);

  void setOutputType(Pin pin, OutputType outputType);

  void setAlternateFunction(Pin pin, AlternateFunction alternateFunction);

  void setOutputPinState(Pin pin, PinState state);

  PinMode readPinMode(Pin pin) const;

  PinState readOutputPinState(Pin pin) const;

  PinState readInputPinState(Pin pin) const;

  static bool isPinConfigurationValid(const PinConfiguration &pinConfiguration);

  static bool isPinValueInValidRangeOfValues(Pin pin);

  static bool isModeInValidRangeOfValues(PinMode mode);

  static bool isPullConfigInValidRangeOfValues(PullConfig pullConfig);

  static bool isOutputSpeedInValidRangeOfValues(OutputSpeed outputSpeed);

  static bool isOutputTypeInValidRangeOfValues(OutputType outputType);

  static bool isAlternateFunctionInValidRangeOfValues(AlternateFunction alternateFunction);

  static bool isPinStateInValidRangeOfValues(PinState pinState);


  //! Pointer to GPIO port
  GPIO_TypeDef *m_GPIOPortPtr;
};

#endif // #ifndef GPIO_H