#ifndef REGISTER_UTILITY_H
#define REGISTER_UTILITY_H

#include "MemoryAccess.h"
#include "MemoryUtility.h"
#include <cstdint>


/**
 * @brief   Utility class designed for manipulating with values of bits in registers.
 * @details Template parameter T should be set to appropriate type according to the
 *          size of register on which manipulations will be performed.
 *
 * @tparam T - Should be: uint8_t, uint16_t, uint32_t or uint64_t.
 */
template <typename T>
class RegisterUtility
{
public:

  //! Max allowed value for bit parameter.
  static constexpr uint8_t MAX_ALLOWED_BIT = 8u * sizeof(T);

  /**
   * @brief Static method sets bit 'bit' in register, pointed by 'registerPtr'.
   *
   * @param[in,out] registerPtr - Pointer to register.
   * @param[in]     bit         - Bit to set in register.
   */
  static void setBitInRegister(volatile T *registerPtr, uint8_t bit);


  /**
   * @brief Static method resets bit 'bit' in register, pointed by 'registerPtr'.
   *
   * @param[in,out] registerPtr - Pointer to register.
   * @param[in]     bit         - Bit to reset in register.
   */
  static void resetBitInRegister(volatile T *registerPtr, uint8_t bit);

  /**
   * @brief Static method checks is bit 'bit' set in register, pointed by 'registerPtr'.
   *
   * @param[in] registerPtr - Pointer to register.
   * @param[in] bit         - Bit to set in register.
   * @return true if bit is set in register, false otherwise.
   */
  static bool isBitSetInRegister(volatile const T *registerPtr, uint8_t bit);

  /**
   * @brief   Static method sets {startBits ... startBits + numberOfBits - 1} bits in register,
   *          pointed by 'registerPtr', to value 'value'.
   * @details If you want to change value of bits which are out of scope of register, then
   *          this method will do nothing. Value will be masked, so that only first 'numberOfBits'
   *          may be set.
   *
   * @param[in,out] registerPtr  - Pointer to register.
   * @param[in]     startBit     - Bit starting from which value will be set/inserted.
   * @param[in]     numberOfBits - Size of value in bits.
   * @param[in]     value        - Value to set/insert.
   */
  static void setBitsInRegister(volatile T *registerPtr, uint8_t startBit, uint8_t numberOfBits, T value);

 /**
   * @brief   Static method gets {startBits ... startBits + numberOfBits - 1} bits in register,
   *          pointed by 'registerPtr'.
   * @details If you want to read value of bits which are out of scope of register, then
   *          this method will return 0u.
   *
   * @param[in] registerPtr  - Pointer to register.
   * @param[in] startBit     - Bit starting from which value will be get.
   * @param[in] numberOfBits - Size of value in bits.
   * @return Value of bits on wanted position in the register.
   */
  static T getBitsInRegister(volatile const T *registerPtr, uint8_t startBit, uint8_t numberOfBits);

};

template <typename T>
void RegisterUtility<T>::setBitInRegister(volatile T *registerPtr, uint8_t bit)
{
  if (bit < MAX_ALLOWED_BIT)
  {
    const T registerValue = MemoryAccess::getRegisterValue(registerPtr);
    T newRegisterValue    = MemoryUtility<T>::setBit(registerValue, bit);
    MemoryAccess::setRegisterValue(registerPtr, newRegisterValue);
  }
}

template <typename T>
void RegisterUtility<T>::resetBitInRegister(volatile T *registerPtr, uint8_t bit)
{
  if (bit < MAX_ALLOWED_BIT)
  {
    const T registerValue = MemoryAccess::getRegisterValue(registerPtr);
    T newRegisterValue    = MemoryUtility<T>::resetBit(registerValue, bit);
    MemoryAccess::setRegisterValue(registerPtr, newRegisterValue);
  }
}

template <typename T>
bool RegisterUtility<T>::isBitSetInRegister(volatile const T *registerPtr, uint8_t bit)
{
  const T registerValue = MemoryAccess::getRegisterValue(registerPtr);
  return MemoryUtility<T>::isBitSet(registerValue, bit);
}

template <typename T>
void RegisterUtility<T>::setBitsInRegister(volatile T *registerPtr, uint8_t startBit, uint8_t numberOfBits, T value)
{
  if ((0u != numberOfBits) && (startBit + numberOfBits) <= MAX_ALLOWED_BIT)
  {
    const T registerValue = MemoryAccess::getRegisterValue(registerPtr);
    T newRegisterValue    = MemoryUtility<T>::setBits(registerValue, startBit, numberOfBits, value);
    MemoryAccess::setRegisterValue(registerPtr, newRegisterValue);
  }
}

template <typename T>
T RegisterUtility<T>::getBitsInRegister(volatile const T *registerPtr, uint8_t startBit, uint8_t numberOfBits)
{
  const T registerValue = MemoryAccess::getRegisterValue(registerPtr);
  return MemoryUtility<T>::getBits(registerValue, startBit, numberOfBits);
}

#endif // #ifndef REGISTER_UTILITY_H