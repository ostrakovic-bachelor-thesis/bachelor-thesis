#include "MemoryUtility.h"
#include "MemoryAccess.h"


const uint32_t MemoryUtility::MASK[33] =
{
  [0]  = 0b0,
  [1]  = 0b1,
  [2]  = 0b11,
  [3]  = 0b111,
  [4]  = 0b1111,
  [5]  = 0b11111,
  [6]  = 0b111111,
  [7]  = 0b1111111,
  [8]  = 0b11111111,
  [9]  = 0b111111111,
  [10] = 0b1111111111,
  [11] = 0b11111111111,
  [12] = 0b111111111111,
  [13] = 0b1111111111111,
  [14] = 0b11111111111111,
  [15] = 0b111111111111111,
  [16] = 0b1111111111111111,
  [17] = 0b11111111111111111,
  [18] = 0b111111111111111111,
  [19] = 0b1111111111111111111,
  [20] = 0b11111111111111111111,
  [21] = 0b111111111111111111111,
  [22] = 0b1111111111111111111111,
  [23] = 0b11111111111111111111111,
  [24] = 0b111111111111111111111111,
  [25] = 0b1111111111111111111111111,
  [26] = 0b11111111111111111111111111,
  [27] = 0b111111111111111111111111111,
  [28] = 0b1111111111111111111111111111,
  [29] = 0b11111111111111111111111111111,
  [30] = 0b111111111111111111111111111111,
  [31] = 0b1111111111111111111111111111111,
  [32] = 0b11111111111111111111111111111111,
};

void MemoryUtility::setBitInRegister(volatile void *registerPtr, uint32_t bit)
{
  if (MAX_ALLOWED_BIT > bit)
  {
    uint32_t registerValue = MemoryAccess::getRegisterValue(registerPtr);
    registerValue |= (1 << bit);
    MemoryAccess::setRegisterValue(registerPtr, registerValue);
  }
}

bool MemoryUtility::isBitSetInRegister(volatile void *registerPtr, uint32_t bit)
{
  bool isBitSet = false;

  if (MAX_ALLOWED_BIT > bit)
  {
    isBitSet = MemoryAccess::getRegisterValue(registerPtr) & (1u << bit);
  }
  
  return isBitSet;
}

void MemoryUtility::setBitsInRegister(volatile void *registerPtr, uint32_t startBit, uint32_t numberOfBits, uint32_t value)
{
  if (MAX_ALLOWED_BIT >= (startBit + numberOfBits))
  {
    uint32_t registerValue = MemoryAccess::getRegisterValue(registerPtr);
    registerValue = (registerValue & ~(MASK[numberOfBits] << startBit)) |
                  ((value & MASK[numberOfBits]) << startBit);
    MemoryAccess::setRegisterValue(registerPtr, registerValue);
  }
}

uint32_t MemoryUtility::getBitsInRegister(volatile const void *registerPtr, uint32_t startBit, uint32_t numberOfBits)
{
  uint32_t retval = 0u;

  if (MAX_ALLOWED_BIT >= (startBit + numberOfBits))
  {
    const uint32_t registerValue = MemoryAccess::getRegisterValue(registerPtr);
    retval = ((registerValue >> startBit) & MASK[numberOfBits]);
  }

  return retval;
}