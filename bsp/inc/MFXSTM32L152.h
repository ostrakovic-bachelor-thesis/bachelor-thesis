#ifndef MFXSTM32L152_H
#define MFXSTM32L152_H

#include "I2C.h"


class MFXSTM32L152
{
public:

  MFXSTM32L152(I2C *I2CPtr);

private:

  //! Pointer to I2C, used to communicate with MFXSTM32L152
  I2C *m_I2CPtr;
};

#endif // #ifndef MFXSTM32L152_H