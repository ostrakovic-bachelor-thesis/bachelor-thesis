#include "GUIDrawPerformanceMeasurement.h"


GUIDrawPerformanceMeasurement::GUIDrawPerformanceMeasurement(SysTick &sysTick):
  m_sysTick(sysTick)
{}

GUIDrawPerformanceMeasurement::ErrorCode
GUIDrawPerformanceMeasurement::init(const GUIDrawPerformanceMeasurementConfig &config)
{
  if (nullptr == config.guiObjectPtr)
  {
    return ErrorCode::NULL_POINTER_ERROR;
  }

  m_initializationState = InitializationState::EXECUTION_TIME_IS_NOT_YET_ESTIMATED;
  m_guiObjectPtr = config.guiObjectPtr;
  m_drawHardware = config.drawHardware;

  return ErrorCode::OK;
}

GUIDrawPerformanceMeasurement::ErrorCode GUIDrawPerformanceMeasurement::execute(void)
{
  if (not isInitialized())
  {
    return ErrorCode::NOT_INITIALIZED;
  }

  const uint64_t timestamp = m_sysTick.getTicks();
  m_guiObjectPtr->draw(m_drawHardware);
  while (not m_guiObjectPtr->isDrawCompleted());
  m_executionTimeInUs = m_sysTick.getElapsedTimeInUs(timestamp);

  m_initializationState = InitializationState::EXECUTION_TIME_IS_AVAILABLE;

  return ErrorCode::OK;
}

GUIDrawPerformanceMeasurement::ErrorCode
GUIDrawPerformanceMeasurement::getExecutionTime(uint64_t &executionTimeInUs) const
{
  if (not isInitialized())
  {
    return ErrorCode::NOT_INITIALIZED;
  }

  if (not isExecutionTimeAvailable())
  {
    return ErrorCode::EXECUTION_TIME_IS_NOT_YET_ESTIMATED;
  }

  executionTimeInUs = m_executionTimeInUs;

  return ErrorCode::OK;
}

inline bool GUIDrawPerformanceMeasurement::isInitialized(void) const
{
  return InitializationState::NOT_INITIALIZED != m_initializationState;
}

inline bool GUIDrawPerformanceMeasurement::isExecutionTimeAvailable(void) const
{
  return InitializationState::EXECUTION_TIME_IS_AVAILABLE == m_initializationState;
}