#ifndef RESET_CONTROL_MOCK_H
#define RESET_CONTROL_MOCK_H

#include "ResetControl.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


class ResetControlMock : public ResetControl
{
public:

  ResetControlMock():
    ResetControl(nullptr)
  {
    ON_CALL(*this, enablePeripheralClock(_))
    .WillByDefault(Invoke([&] (Peripheral peripheral)
    {
      return m_errorCode;
    }));

    ON_CALL(*this, disablePeripheralClock(_))
    .WillByDefault(Invoke([&] (Peripheral peripheral)
    {
      return m_errorCode;
    }));

    ON_CALL(*this, isPeripheralClockEnabled(_, _))
    .WillByDefault(Invoke([&] (Peripheral peripheral, bool &isClockEnabled)
    {
      isClockEnabled = m_isPeripheralClockEnabled;
      return m_errorCode;
    }));
  }

  virtual ~ResetControlMock() = default;

  inline void setReturnValueOfIsPeripheralClockEnabled(bool returnValue)
  {
    m_isPeripheralClockEnabled = returnValue;
  }

  inline void setReturnErrorCode(ErrorCode errorCode)
  {
    m_errorCode = errorCode;
  }

  // mock methods
  MOCK_METHOD(ErrorCode, enablePeripheralClock, (Peripheral), (override));
  MOCK_METHOD(ErrorCode, disablePeripheralClock, (Peripheral), (override));
  MOCK_METHOD(ErrorCode, isPeripheralClockEnabled, (Peripheral, bool &), (const, override));

private:

  bool m_isPeripheralClockEnabled = false;

  ErrorCode m_errorCode = ErrorCode::OK;
};

#endif // #ifndef RESET_CONTROL_MOCK_H