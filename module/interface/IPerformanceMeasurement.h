#ifndef I_PERFORMANCE_MEASUREMENT_H
#define I_PERFORMANCE_MEASUREMENT_H

#include <cstdint>


class IPerformanceMeasurement
{
public:
  virtual ~IPerformanceMeasurement() = default;

  //! This enum class represents errors which can happen during method calls
  enum class ErrorCode : uint8_t
  {
    OK                                  = 0u,
    NOT_INITIALIZED                     = 1u,
    NULL_POINTER_ERROR                  = 2u,
    EXECUTION_TIME_IS_NOT_YET_ESTIMATED = 3u
  };

  virtual ErrorCode execute(void) = 0;

  virtual ErrorCode getExecutionTime(uint64_t &executionTimeInUs) const = 0;
};

#endif // #ifndef I_PERFORMANCE_MEASUREMENT_H