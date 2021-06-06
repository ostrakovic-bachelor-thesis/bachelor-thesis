#ifndef MEMORY_UTILITY_H
#define MEMORY_UTILITY_H

#include <cstdint>


class MemoryUtility
{
public:

  static constexpr uint32_t MAX_ALLOWED_BIT = 8u * sizeof(uint32_t); 

  /**
   * @brief Static method sets bit 'bit' in register, pointed by 'registerPtr'.
   * 
   * @param[in] registerPtr - Pointer to register.
   * @param[in] bit         - Bit to set in register.
   */
  static void setBitInRegister(volatile void *registerPtr, uint32_t bit);

  /**
   * @brief Static method checks is bit 'bit' set in register, pointed by 'registerPtr'.
   * 
   * @param[in] registerPtr - Pointer to register.
   * @param[in] bit         - Bit to set in register.
   * @return true if bit is set in register, false otherwise.
   */
  static bool isBitSetInRegister(volatile void *registerPtr, uint32_t bit);

  /**
   * @brief   Static method sets {startBits ... startBits + numberOfBits - 1} bits in register,
   *          pointed by 'registerPtr', to value 'value'.
   * @details If you want to change value of bits which are out of scope of register, then
   *          this method will do nothing. Value will be masked, so that only first 'numberOfBits'
   *          may be set.
   * 
   * @param[in] registerPtr  - Pointer to register.
   * @param[in] startBit     - Bit starting from which value will be set/inserted.
   * @param[in] numberOfBits - Size of value in bits.
   * @param[in] value        - Value to set/insert.
   */
  static void setBitsInRegister(volatile void *registerPtr, uint32_t startBit, uint32_t numberOfBits, uint32_t value);

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
  static uint32_t getBitsInRegister(volatile const void *registerPtr, uint32_t startBit, uint32_t numberOfBits);

private:
  
  //! Array with precalculated values of masks for value of n-bits size (n is index to MASK array).
  static const uint32_t MASK[33];
};

#endif // #ifndef MEMORY_UTILITY_H