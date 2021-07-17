#ifndef CLOCK_CONTROL_MOCK_H
#define CLOCK_CONTROL_MOCK_H

#include "ClockControl.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstdint>


using namespace ::testing;


class ClockControlMock : public ClockControl
{
public:

  ClockControlMock():
    ClockControl(nullptr)
  {
    ON_CALL(*this, getClockFrequency(Matcher<Peripheral>(_), _))
    .WillByDefault(Invoke([&] (Peripheral peripheral, uint32_t &clockFrequency)
    {
      clockFrequency = m_clockFrequency;
      return m_errorCode;
    }));

    ON_CALL(*this, getClockFrequency(Matcher<ClockSource>(_), _))
    .WillByDefault(Invoke([&] (ClockSource clockSource, uint32_t &clockFrequency)
    {
      clockFrequency = m_clockFrequency;
      return m_errorCode;
    }));
  }
  
  virtual ~ClockControlMock() = default;

  inline void setReturnClockFrequency(uint32_t clockFrequency)
  {
    m_clockFrequency = clockFrequency;
  }

  inline void setReturnErrorCode(ErrorCode errorCode)
  {
    m_errorCode = errorCode;
  }
   
  // Mock methods
  MOCK_METHOD(ErrorCode, getClockFrequency, (ClockSource, uint32_t &), (const, override));
  MOCK_METHOD(ErrorCode, getClockFrequency, (Peripheral, uint32_t &), (const, override));

private:
  
  uint32_t m_clockFrequency = 16000000u; // 16 MHz

  ErrorCode m_errorCode = ErrorCode::OK;
};

#endif // #ifndef CLOCK_CONTROL_MOCK_H