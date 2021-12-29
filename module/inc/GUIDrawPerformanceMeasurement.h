#ifndef GUI_DRAW_PERFORMANCE_MEASUREMENT_H
#define GUI_DRAW_PERFORMANCE_MEASUREMENT_H

#include "IPerformanceMeasurement.h"
#include "IGUIObject.h"
#include "SysTick.h"


class GUIDrawPerformanceMeasurement : public IPerformanceMeasurement
{
public:

  GUIDrawPerformanceMeasurement(SysTick &sysTick);

  struct GUIDrawPerformanceMeasurementConfig
  {
    IGUIObject *guiObjectPtr;
    IGUIObject::DrawHardware drawHardware;
  };

  ErrorCode init(const GUIDrawPerformanceMeasurementConfig &config);

  ErrorCode execute(void) override;

  ErrorCode getExecutionTime(uint64_t &executionTimeInUs) const override;

private:

  enum class InitializationState : uint8_t
  {
    NOT_INITIALIZED = 0u,
    EXECUTION_TIME_IS_NOT_YET_ESTIMATED,
    EXECUTION_TIME_IS_AVAILABLE
  };

  bool isInitialized(void) const;
  bool isExecutionTimeAvailable(void) const;

  InitializationState m_initializationState = InitializationState::NOT_INITIALIZED;

  uint64_t m_executionTimeInUs = 0u;

  IGUIObject *m_guiObjectPtr = nullptr;

  IGUIObject::DrawHardware m_drawHardware;

  //! Reference to SysTick
  SysTick &m_sysTick;
};

#endif // #ifndef GUI_DRAW_PERFORMANCE_MEASUREMENT_H