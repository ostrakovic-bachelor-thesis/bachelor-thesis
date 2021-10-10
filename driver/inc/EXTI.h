#ifndef EXTI_H
#define EXTI_H

#include "stm32l4r9xx.h"
#include "Peripheral.h"


class EXTI
{
public:

  EXTI(EXTI_TypeDef *EXTIPeripheralPtr);

  //! This enum class represents errors which can happen during method calls
  enum class ErrorCode : uint8_t
  {
    OK = 0u
  };

  enum class EXTILine : uint8_t
  {
    LINE0  = 0u,
    LINE1  = 1u,
    LINE2  = 2u,
    LINE3  = 3u,
    LINE4  = 4u,
    LINE5  = 5u,
    LINE6  = 6u,
    LINE7  = 7u,
    LINE8  = 8u,
    LINE9  = 9u,
    LINE10 = 10u,
    LINE11 = 11u,
    LINE12 = 12u,
    LINE13 = 13u,
    LINE14 = 14u,
    LINE15 = 15u,
    LINE16 = 16u,
    LINE17 = 17u,
    LINE18 = 18u,
    LINE19 = 19u,
    LINE20 = 20u,
    LINE21 = 21u,
    LINE22 = 22u,
    LINE23 = 23u,
    LINE24 = 24u,
    LINE25 = 25u,
    LINE26 = 26u,
    LINE27 = 27u,
    LINE28 = 28u,
    LINE29 = 29u,
    LINE30 = 30u,
    LINE31 = 31u,
    LINE32 = 32u,
    LINE33 = 33u,
    LINE34 = 34u,
    LINE35 = 35u,
    LINE36 = 36u,
    LINE37 = 37u,
    LINE38 = 38u,
    LINE39 = 39u,
    LINE40 = 40u
  };

  struct EXTIConfig
  {
    bool isInterruptMasked;
  };

  ErrorCode configureEXITLine(EXTILine line, EXTIConfig config);

private:

  EXTI_TypeDef *m_EXTIPeripheralPtr;
};

#endif // #ifndef EXTI_H