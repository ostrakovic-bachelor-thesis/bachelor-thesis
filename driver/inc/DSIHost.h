#ifndef DSI_HOST_H
#define DSI_HOST_H

#include "stm32l4r9xx.h"
#include "ResetControl.h"
#include "Peripheral.h"


class DSIHost
{
public:

  DSIHost(DSI_TypeDef *DSIHostPeripheralPtr, ResetControl *resetControlPtr);

  //! This enum class represents errors which can happen during method calls
  enum class ErrorCode : uint8_t
  {
    OK                               = 0u,
    CAN_NOT_TURN_ON_PERIPHERAL_CLOCK = 1u,
  };

  struct PLLConfig
  {
    uint8_t inputClockDivider;
    uint8_t inputClockMultiplier;
    uint8_t outputClockDivider;
  };

  struct DSIHostConfig
  {
    PLLConfig pllConfig;
    uint8_t numberOfDataLanes;
  };

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode init(const DSIHostConfig &dsiHostConfig);

  inline Peripheral getPeripheralTag(void) const
  {
    return static_cast<Peripheral>(reinterpret_cast<uintptr_t>(const_cast<DSI_TypeDef*>(m_DSIHostPeripheralPtr)));
  }

private:

  ErrorCode enablePeripheralClock(void);

  void enableDPHYRegulator(void);
  void requestTurningOnDPHYRegulator(void);
  bool isDPHYRegulatorReady(void) const;

  void setPLLInputClockDivider(uint32_t &registerValueWRPCR, uint8_t inputClockDivider);
  void setPLLInputClockMultiplier(uint32_t &registerValueWRPCR, uint8_t inputClockMultiplier);
  void setPLLOutputClockDivider(uint32_t &registerValueWRPCR, uint8_t outputClockDivider);

  void enableDPHYPLL(void);
  void requestTurningOnDPHYPLL(void);
  bool isDPHYPLLReady(void) const;

  void setNumberOfDataLanes(uint8_t numberOfDataLanes);

  void enableDPHYClockLaneModuleAndDigitalSection(void);
  void enableDPHYClockLaneModule(uint32_t &registerValuePCTLR);
  void enableDPHYDigitalSection(uint32_t &registerValuePCTLR);

  void disableAutomaticClockLaneControl(void);

  //! Pointer to DSI Host peripheral
  DSI_TypeDef *m_DSIHostPeripheralPtr;

  //! Pointer to Reset Control module
  ResetControl *m_resetControlPtr;
};

#endif // #ifndef DSI_HOST_H