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

  enum class VirtualChannelID : uint8_t
  {
    CHANNEL_0 = 0u,
    CHANNEL_1 = 1u,
    CHANNEL_2 = 2u,
    CHANNEL_3 = 3u
  };

  enum class TransmissionType : uint8_t
  {
    HIGH_SPEED = 0u,
    LOW_POWER  = 1u
  };

  enum class LTDCColorCoding : uint8_t
  {
    RGB565_IN_BLOCK      = 0b0000,
    RGB565_BYTE_ALIGNED  = 0b0001,
    RGB565_MIDDLE_PLACED = 0b0010,
    RGB666_IN_BLOCK      = 0b0011,
    RGB666_BYTE_ALIGNED  = 0b0100,
    RGB888               = 0b0101
  };

  enum class TearingEffectSource : uint8_t
  {
    DSI_LINK     = 0b0,
    EXTERNAL_PIN = 0b1
  };

  enum class TearingEffectPolarity : uint8_t
  {
    RISING_EDGE  = 0b0,
    FALLING_EDGE = 0b1
  };

  enum class VSyncLTDCHaltPolarity : uint8_t
  {
    FALLING_EDGE = 0b0,
    RISING_EDGE  = 0b1
  };

  enum class SignalPolarity : uint8_t
  {
    ACTIVE_HIGH = 0b0,
    ACTIVE_LOW  = 0b1
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

  struct FlowControlConfig
  {
    bool enableCRCReception;
    bool enableECCReception;
    bool enableBusTurnAround;
    bool enableEoTpReception;
    bool enableEoTpTransmission;
  };

  struct DBIInterfaceConfig
  {
    TransmissionType transmissionTypeMaximumReadPacketSize;
    TransmissionType transmissionTypeDCSLongWrite;
    TransmissionType transmissionTypeDCSShortReadZeroParam;
    TransmissionType transmissionTypeDCSShortReadOneParam;
    TransmissionType transmissionTypeDCSShortWriteZeroParam;
    TransmissionType transmissionTypeGenericLongWrite;
    TransmissionType transmissionTypeGenericShortReadTwoParam;
    TransmissionType transmissionTypeGenericShortReadOneParam;
    TransmissionType transmissionTypeGenericShortReadZeroParam;
    TransmissionType transmissionTypeGenericShortWriteTwoParam;
    TransmissionType transmissionTypeGenericShortWriteOneParam;
    TransmissionType transmissionTypeGenericShortWriteZeroParam;
    bool enableAcknowledgeRequest;
    bool enableTearingEffectAcknowledgeRequest;
  };

  struct DSIHostConfig
  {
    PLLConfig pllConfig;
    uint8_t numberOfDataLanes;
    DSIPHYTiming dsiPhyTiming;
    FlowControlConfig flowControlConfig;
    DBIInterfaceConfig dbiInterfaceConfig;

    uint8_t virtualChannelId;
    LTDCColorCoding ltdcColorCoding;
    VSyncLTDCHaltPolarity vsyncLtdcHaltPolarity;
    SignalPolarity hsyncPolarity;
    SignalPolarity vsyncPolarity;
    SignalPolarity dataEnablePolarity;

    uint16_t maxLTDCWriteMemoryCmdSize;
    TearingEffectSource tearingEffectSource;
    TearingEffectPolarity tearingEffectPolarity;
    bool enableAutoRefreshMode;
  };

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode init(const DSIHostConfig &dsiHostConfig);

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode genericShortWrite(VirtualChannelID virtualChannelId);

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode genericShortWrite(VirtualChannelID virtualChannelId, uint8_t parameter);

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode genericShortWrite(VirtualChannelID virtualChannelId, uint8_t parameter1, uint8_t parameter2);

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode dcsShortWrite(VirtualChannelID virtualChannelId, uint8_t dcsCommand);

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode dcsShortWrite(VirtualChannelID virtualChannelId, uint8_t dcsCommand, uint8_t parameter);

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode genericLongWrite(VirtualChannelID virtualChannelId, const void *dataPtr, uint16_t dataSize);

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode dcsLongWrite(VirtualChannelID virtualChannelId,
    uint8_t dcsCommand,
    const void *dataPtr,
    uint16_t dataSize);

#ifdef UNIT_TEST
  virtual
#endif // #ifdef UNIT_TEST
  ErrorCode startTransferFromLTDC(void);

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
  ErrorCode configureFlowControl(const FlowControlConfig &flowControlConfig);
  ErrorCode configureDBIInterface(const DBIInterfaceConfig &dbiInterfaceConfig);
  ErrorCode configureDSIHostLTDCInterface(const DSIHostConfig &dsiHostConfig);
  ErrorCode configureAdaptedCommandMode(const DSIHostConfig &dsiHostConfig);
  void enableAdaptedCommandMode(void);

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
  void setDCSShortWriteZeroParamTransmissionType(uint32_t &registerValueCMCR, TransmissionType transmissionType);
  void setGenericLongWriteTransmissionType(uint32_t &registerValueCMCR, TransmissionType transmissionType);
  void setGenericShortReadTwoParamTransmissionType(uint32_t &registerValueCMCR, TransmissionType transmissionType);
  void setGenericShortReadOneParamTransmissionType(uint32_t &registerValueCMCR, TransmissionType transmissionType);
  void setGenericShortReadZeroParamTransmissionType(uint32_t &registerValueCMCR, TransmissionType transmissionType);
  void setGenericShortWriteTwoParamTransmissionType(uint32_t &registerValueCMCR, TransmissionType transmissionType);
  void setGenericShortWriteOneParamTransmissionType(uint32_t &registerValueCMCR, TransmissionType transmissionType);
  void setGenericShortWriteZeroParamTransmissionType(uint32_t &registerValueCMCR, TransmissionType transmissionType);

  void enableAcknowledgeRequest(uint32_t &registerValueCMCR);
  void disableAcknowledgeRequest(uint32_t &registerValueCMCR);

  void enableTearingEffectAcknowledgeRequest(uint32_t &registerValueCMCR);
  void disableTearingEffectAcknowledgeRequest(uint32_t &registerValueCMCR);

  void setVirtualChannelID(uint8_t virtualChannelId);
  void setLTDCColorCoding(LTDCColorCoding ltdcColorCoding);

  void setColorMultiplexing(uint32_t &registerValueWCFGR, LTDCColorCoding ltdcColorCoding);
  void setTearingEffectSource(uint32_t &registerValueWCFGR, TearingEffectSource tearingEffectSource);
  void enableAutoRefreshMode(uint32_t &registerValueWCFGR);
  void disableAutoRefreshMode(uint32_t &registerValueWCFGR);
  void setVSyncLTDCHaltPolarity(uint32_t &registerValueWCFGR, VSyncLTDCHaltPolarity vsyncLtdcHaltPolarity);
  void setTearingEffectPolarity(uint32_t &registerValueWCFGR, TearingEffectPolarity tearingEffectPolarity);

  void setDSIModeToAdaptedCommandMode(void);
  void setCommandModeToAdaptedCommandMode(void);

  void setMaximumLTDCWriteMemoryCommandSize(uint16_t maximumLTDCWriteMemoryCommandSize);

  void setHSyncPolarity(uint32_t &registerValueLPCR, SignalPolarity hsyncPolarity);
  void setVSyncPolarity(uint32_t &registerValueLPCR, SignalPolarity vsyncPolarity);
  void setDataEnablePolarity(uint32_t &registerValueLPCR, SignalPolarity dataEnablePolarity);

  void disableAllDSIHostInterrupts(void);

  void enableDSIHost(void);
  void enableDSIWrapper(void);

  void setPacketDataType(uint32_t &registerValueGHCR, uint8_t packetDataType);
  void setVirtualChannelID(uint32_t &registerValueGHCR, VirtualChannelID virtualChannelId);
  void setShortPacketData0(uint32_t &registerValueGHCR, uint8_t data);
  void setShortPacketData1(uint32_t &registerValueGHCR, uint8_t data);
  void setLongPacketDataSize(uint32_t &registerValueGHCR, uint16_t dataSize);

  void writeDataToTransmitInFIFO(uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4);
  void writeDataToTransmitInFIFO(const uint8_t *dataPtr, uint16_t dataSize);
  void writeDCSCommandAndDataToTransmitInFIFO(uint8_t dcsCommand, const uint8_t *dataPtr, uint16_t dataSize);

  bool isCommandFIFOEmpty(void) const;

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