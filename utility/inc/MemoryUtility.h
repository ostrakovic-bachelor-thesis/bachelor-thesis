#ifndef MEMORY_UTILITY_H
#define MEMORY_UTILITY_H

#include <cstdint>


/**
 * @brief   Utility class designed for manipulating with values of bits.
 * @details Template parameter T should be set to appropriate type according to the
 *          size of value on which manipulations will be performed.
 *
 * @tparam T - Should be: uint8_t, uint16_t, uint32_t or uint64_t.
 */
template <typename T>
class MemoryUtility
{
public:

  //! Max allowed value for bit parameter.
  static constexpr uint8_t MAX_ALLOWED_BIT = static_cast<uint8_t>(8u * sizeof(T));

  /**
   * @brief Static method sets bit 'bit' in 'value'.
   *
   * @param[in] value - Value in which bit will be set.
   * @param[in] bit   - Bit to set in register.
   * @return Value with bit 'bit' set.
   */
  static T setBit(T value, uint8_t bit);

  /**
   * @brief Static method resets bit 'bit' in 'value'.
   *
   * @param[in] value - Value in which bit will be reset.
   * @param[in] bit   - Bit to reset in register.
   * @return Value with bit 'bit' reset.
   */
  static T resetBit(T value, uint8_t bit);

  /**
   * @brief Static method checks is bit 'bit' set in 'value'.
   *
   * @param[in] value - Value for which check will be performed.
   * @param[in] bit   - Bit to check in 'value'.
   * @return true if bit is set in 'value', false otherwise.
   */
  static bool isBitSet(T value, uint8_t bit);

  /**
   * @brief   Static method sets {startBits ... startBits + numberOfBits - 1} bits in 'value',
   *          to the value 'bitsValue'.
   * @details If you want to change value of bits which are out of scope of register, then
   *          this method will do nothing. 'bitsValue' will be masked, so that only first
   *          'numberOfBits' may be set.
   *
   * @param[in] value        - Value in which bits will be set.
   * @param[in] startBit     - Bit starting from which 'bitsValue' will be set/inserted in 'value'.
   * @param[in] numberOfBits - Size of 'bitsValue' in bits.
   * @param[in] bitsValue    - Value to set/insert.
   */
  static T setBits(T value, uint8_t startBit, uint8_t numberOfBits, T bitsValue);

  /**
   * @brief   Static method gets {startBits ... startBits + numberOfBits - 1} bits from value.
   * @details If you want to read value of bits which are out of scope of register, then
   *          this method will return 0u.
   *
   * @param[in] value        - Value from which bits will be read.
   * @param[in] startBit     - Bit starting from which value will be get.
   * @param[in] numberOfBits - Size of value in bits.
   * @return Value of bits on wanted position in 'value'.
   */
  static T getBits(T value, uint8_t startBit, uint8_t numberOfBits);

private:

  /**
   * @brief Static method returns mask in which first 'numberOfBits' are set to 1, and others are 0.
   *
   * @param[in] numberOfBits - Number of bits which will be set in mask.
   * @return Mask in which first 'numberOfBits' bits are set, and others are 0.
   */
  static inline T mask(uint8_t numberOfBits)
  {
    return (1u << numberOfBits) - 1u;
  }

};

template <typename T>
T MemoryUtility<T>::setBit(T value, uint8_t bit)
{
  return (MAX_ALLOWED_BIT > bit) ? (value | (1u << bit)) : value;
}

template <typename T>
T MemoryUtility<T>::resetBit(T value, uint8_t bit)
{
  return (MAX_ALLOWED_BIT > bit) ? (value & ~(1u << bit)) : value;
}

template <typename T>
bool MemoryUtility<T>::isBitSet(T value, uint8_t bit)
{
  return (MAX_ALLOWED_BIT > bit) ? ((value & (1u << bit)) == (1u << bit)) : false;
}

template <typename T>
T MemoryUtility<T>::setBits(T value, uint8_t startBit, uint8_t numberOfBits, T bitsValue)
{
  T retval = value;

  if (MAX_ALLOWED_BIT >= (startBit + numberOfBits))
  {
    retval = (value & ~(mask(numberOfBits) << startBit)) | ((bitsValue & mask(numberOfBits)) << startBit);
  }

  return retval;
}

template <typename T>
T MemoryUtility<T>::getBits(T value, uint8_t startBit, uint8_t numberOfBits)
{
  T retval = 0u;

  if (MAX_ALLOWED_BIT >= (startBit + numberOfBits))
  {
    retval = (value >> startBit) & mask(numberOfBits);
  }

  return retval;
}


#endif