#ifndef DSI_HOST_H
#define DSI_HOST_H

#include "stm32l4r9xx.h"
#include "ClockControl.h"
#include "ResetControl.h"
#include "Peripheral.h"


class DSIHost
{
public:

  DSIHost(DSI_TypeDef *DSIHostPeripheralPtr, ClockControl *clockControlPtr, ResetControl *resetControlPtr);

  //! This enum class represents errors which can happen during method calls
  enum class ErrorCode : uint8_t
  {
    OK                               = 0u,
    CAN_NOT_TURN_ON_PERIPHERAL_CLOCK = 1u,
    CAN_NOT_GET_HSE_CLOCK_FREQUENCY  = 2u
  };

  enum class TransmissionType : uint8_t
  {
    HIGH_SPEED = 0u,
    LOW_POWER  = 1u
  };

  struct PLLConfig
  {
    uint8_t inputClockDivider;
    uint8_t inputClockMultiplier;
    uint8_t outputClockDivider;
  };

  struct DSIPHYTiming
  {
    uint16_t clockLaneHighSpeedToLowPowerTime; //!< [ns]
    uint16_t clockLaneLowPowerToHighSpeedTime; //!< [ns]
    uint16_t dataLaneHighSpeedToLowPowerTime;  //!< [ns]
    uint16_t dataLaneLowPowerToHighSpeedTime;  //!< [ns]
    uint16_t stopWaitTime;                     //!< [ns]
  };

  struct DSIHostConfig
  {
    PLLConfig pllConfig;
    uint8_t numberOfDataLanes;
    DSIPHYTiming dsiPhyTiming;

    bool enableCRCReception;
    bool enableECCReception;
    bool enableBusTurnAround;
    bool enableEoTpReception;
    bool enableEoTpTransmission;

    TransmissionType transmissionTypeMaximumReadPacketSize;
    TransmissionType transmissionTypeDCSLongWrite;
    TransmissionType transmissionTypeDCSShortReadZeroParam;
    TransmissionType transmissionTypeDCSShortReadOneParam;
  };

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode init(const DSIHostConfig &dsiHostConfig);

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode getDSIPHYClockFrequency(uint32_t &dsiPhyClockFreq);

  inline Peripheral getPeripheralTag(void) const
  {
    return static_cast<Peripheral>(reinterpret_cast<uintptr_t>(const_cast<DSI_TypeDef*>(m_DSIHostPeripheralPtr)));
  }

private:

  static constexpr uint32_t FREQ_HZ_TO_MHZ_DIVIDER     = 1000000u;
  static constexpr uint32_t NANOSECONDS_IN_MICROSECOND = 1000u;
  static constexpr uint32_t NANOSECONDS_IN_SECOND      = 1000000000u;
  static constexpr uint32_t BITS_IN_BYTE = 8u;

  ErrorCode calculateDSIPHYClockFrequency(const PLLConfig &pllConfig, uint32_t &dsiPhyClockFreq);

  ErrorCode enablePeripheralClock(void);
  ErrorCode configureDPHYPLL(const PLLConfig &pllConfig);
  ErrorCode configureDPHY(const DSIHostConfig &dsiHostConfig);
  ErrorCode configureClocks(void);
  ErrorCode configureDSIPHYTiming(const DSIPHYTiming &dsiPhyTiming);
  ErrorCode configureDSIHostTimeouts(void);

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

  void disableAutomaticClockLaneControl(uint32_t &registerValueCLCR);
  void setClockLaneRunningModeToHighSpeed(uint32_t &registerValueCLCR);

  void setHighSpeedModeClockFrequency(void);
  void setTxEscapeClockFrequency(void);

  void setClockLaneHighSpeedToLowPowerTime(uint32_t clockLaneHighSpeedToLowPowerTime);
  void setClockLaneLowPowerToHighSpeedTime(uint32_t clockLaneLowPowerToHighSpeedTime);
  void setDataLaneHighSpeedToLowPowerTime(uint32_t dataLaneHighSpeedToLowPowerTime);
  void setDataLaneLowPowerToHighSpeedTime(uint32_t dataLaneLowPowerToHighSpeedTime);
  void setStopWaitTimeBeforeGoingToHighSpeed(uint32_t stopWaitTime);

  void setTimeoutClockDivisionFactor(uint8_t timeoutClockDivisionFactor);
  void setHighSpeedTransmissionTimeout(uint8_t highSpeedTransmissionTimeout);
  void setLowPowerReceptionTimeout(uint8_t lowPowerReceptionTimeout);
  void setHighSpeedReadTimeout(uint8_t highSpeedReadTimeout);
  void setLowPowerReadTimeout(uint8_t lowPowerReadTimeout);

  void setHighSpeedWriteTimeout(uint32_t &registerValueTCCR3, uint8_t highSpeedWriteTimeout);
  void disablePrespMode(uint32_t &registerValueTCCR3);

  void setLowPowerWriteTimeout(uint8_t lowPowerWriteTimeout);
  void setBusTurnAroundTimeout(uint8_t busTurnAroundTimeout);

  void enableCRCReception(uint32_t &registerValuePCR);
  void disableCRCReception(uint32_t &registerValuePCR);

  void enableECCReception(uint32_t &registerValuePCR);
  void disableECCReception(uint32_t &registerValuePCR);

  void enableBusTurnAround(uint32_t &registerValuePCR);
  void disableBusTurnAround(uint32_t &registerValuePCR);

  void enableEoTpReception(uint32_t &registerValuePCR);
  void disableEoTpReception(uint32_t &registerValuePCR);

  void enableEoTpTransmission(uint32_t &registerValuePCR);
  void disableEoTpTransmission(uint32_t &registerValuePCR);

  void setMaximumReadPacketSizeTransmissionType(uint32_t &registerValueCMCR, TransmissionType transmissionType);
  void setDCSLongWriteTransmissionType(uint32_t &registerValueCMCR, TransmissionType transmissionType);
  void setDCSShortReadZeroParamTransmissionType(uint32_t &registerValueCMCR, TransmissionType transmissionType);
  void setDCSShortReadOneParamTransmissionType(uint32_t &registerValueCMCR, TransmissionType transmissionType);

  //! Pointer to DSI Host peripheral
  DSI_TypeDef *m_DSIHostPeripheralPtr;

  //! Pointer to Clock Control module
  ClockControl *m_clockControlPtr;

  //! Pointer to Reset Control module
  ResetControl *m_resetControlPtr;

  //! TODO
  uint32_t m_dsiPhyClockFreq;
};

#endif // #ifndef DSI_HOST_H