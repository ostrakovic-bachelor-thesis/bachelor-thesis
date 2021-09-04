#ifndef MEMORY_ACCESS_H
#define MEMORY_ACCESS_H

#ifdef UNIT_TEST
#include "MemoryAccessHook.h"
#endif // #ifdef UNIT_TEST

#include <cstdint>


class MemoryAccess
{
public:

  /**
   * @brief Static method sets register, pointed by 'registerPtr', to value 'value'.
   *
   * @param[in] registerPtr - Pointer to register.
   * @param[in] value       - Value to which register will be set.
   */
  static inline void setRegisterValue(volatile uint32_t *registerPtr, uint32_t value)
  {
#ifdef UNIT_TEST
    if (nullptr != s_memoryAccessHookPtr)
    {
      s_memoryAccessHookPtr->setRegisterValue(registerPtr, value);
    }
#endif // #ifdef UNIT_TEST
    *registerPtr = value;
  }

  /**
   * @brief Static method sets register, pointed by 'registerPtr', to value 'value'.
   *
   * @param[in] registerPtr - Pointer to register.
   * @param[in] value       - Value to which register will be set.
   */
  static inline void setRegisterValue(volatile uint16_t *registerPtr, uint16_t value)
  {
#ifdef UNIT_TEST
    if (nullptr != s_memoryAccessHookPtr)
    {
      s_memoryAccessHookPtr->setRegisterValue(registerPtr, value);
    }
#endif // #ifdef UNIT_TEST
    *registerPtr = value;
  }

  /**
   * @brief Static method sets register, pointed by 'registerPtr', to value 'value'.
   *
   * @param[in] registerPtr - Pointer to register.
   * @param[in] value       - Value to which register will be set.
   */
  static inline void setRegisterValue(volatile uint8_t *registerPtr, uint8_t value)
  {
#ifdef UNIT_TEST
    if (nullptr != s_memoryAccessHookPtr)
    {
      s_memoryAccessHookPtr->setRegisterValue(registerPtr, value);
    }
#endif // #ifdef UNIT_TEST
    *registerPtr = value;
  }

  /**
   * @brief Static method gets value of register, pointed by 'registerPtr'.
   *
   * @param[in] registerPtr - Pointer to register.
   * @return Register value.
   */
  static inline uint32_t getRegisterValue(volatile const uint32_t *registerPtr)
  {
#ifdef UNIT_TEST
    if (nullptr != s_memoryAccessHookPtr)
    {
      s_memoryAccessHookPtr->getRegisterValue(registerPtr);
    }
#endif // #ifdef UNIT_TEST
    return *registerPtr;
  }

  /**
   * @brief Static method gets value of register, pointed by 'registerPtr'.
   *
   * @param[in] registerPtr - Pointer to register.
   * @return Register value.
   */
  static inline uint16_t getRegisterValue(volatile const uint16_t *registerPtr)
  {
#ifdef UNIT_TEST
    if (nullptr != s_memoryAccessHookPtr)
    {
      s_memoryAccessHookPtr->getRegisterValue(registerPtr);
    }
#endif // #ifdef UNIT_TEST
    return *registerPtr;
  }

  /**
   * @brief Static method gets value of register, pointed by 'registerPtr'.
   *
   * @param[in] registerPtr - Pointer to register.
   * @return Register value.
   */
  static inline uint8_t getRegisterValue(volatile const uint8_t *registerPtr)
  {
#ifdef UNIT_TEST
    if (nullptr != s_memoryAccessHookPtr)
    {
      s_memoryAccessHookPtr->getRegisterValue(registerPtr);
    }
#endif // #ifdef UNIT_TEST
    return *registerPtr;
  }

#ifdef UNIT_TEST
  /**
   * @brief   Method sets memory access hook.
   * @details It is used only for purpose of driver testing, to verify expectations about setting
   *          registers to correct values.
   *
   * @param[in] memoryAccessHookPtr - Pointer to memory access hook.
   */
  static inline void setMemoryAccessHook(NiceMock<MemoryAccessHook> *memoryAccessHookPtr)
  {
    s_memoryAccessHookPtr = memoryAccessHookPtr;
  }

private:
  //! Pointer to memory access hook, used to verify set and get register value expectations
  static NiceMock<MemoryAccessHook> *s_memoryAccessHookPtr;
#endif // #ifdef UNIT_TEST

};

#endif // MEMORY_ACCESS_H