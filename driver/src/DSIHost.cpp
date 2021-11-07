#include "DSIHost.h"
#include "MemoryAccess.h"
#include "MemoryUtility.h"
#include "RegisterUtility.h"


DSIHost::DSIHost(DSI_TypeDef *DSIHostPeripheralPtr, ClockControl *clockControlPtr, ResetControl *resetControlPtr):
  m_DSIHostPeripheralPtr(DSIHostPeripheralPtr),
  m_clockControlPtr(clockControlPtr),
  m_resetControlPtr(resetControlPtr)
{}

DSIHost::ErrorCode DSIHost::init(const DSIHostConfig &dsiHostConfig)
{
  ErrorCode errorCode = enablePeripheralClock();

  if (ErrorCode::OK == errorCode)
  {
    errorCode = configureDPHYPLL(dsiHostConfig.pllConfig);
  }

  if (ErrorCode::OK == errorCode)
  {
    errorCode = configureDPHY(dsiHostConfig);
  }

  if (ErrorCode::OK == errorCode)
  {
    errorCode = configureClocks();
  }

  if (ErrorCode::OK == errorCode)
  {
    errorCode = configureDSIPHYTiming(dsiHostConfig.dsiPhyTiming);
  }

  if (ErrorCode::OK == errorCode)
  {
    errorCode = configureDSIHostTimeouts();
  }

  if (ErrorCode::OK == errorCode)
  {
    errorCode = configureFlowControl(dsiHostConfig.flowControlConfig);
  }

  if (ErrorCode::OK == errorCode)
  {
    errorCode = configureDBIInterface(dsiHostConfig.dbiInterfaceConfig);
  }

  if (ErrorCode::OK == errorCode)
  {
    errorCode = configureDSIHostLTDCInterface(dsiHostConfig);
  }

  if (ErrorCode::OK == errorCode)
  {
    enableAdaptedCommandMode();
    errorCode = configureAdaptedCommandMode(dsiHostConfig);
  }

  if (ErrorCode::OK == errorCode)
  {
    disableAllDSIHostInterrupts();
    enableDSIHost();
    enableDSIWrapper();
  }

  return errorCode;
}

DSIHost::ErrorCode DSIHost::genericShortWrite(VirtualChannelID virtualChannelId)
{
  while (not isCommandFIFOEmpty());

  uint32_t registerValueGHCR = 0u;

  setPacketDataType(registerValueGHCR, 0x3);
  setVirtualChannelID(registerValueGHCR, virtualChannelId);
  setShortPacketData0(registerValueGHCR, 0x0);
  setShortPacketData1(registerValueGHCR, 0x0);

  MemoryAccess::setRegisterValue(&(m_DSIHostPeripheralPtr->GHCR), registerValueGHCR);

  return ErrorCode::OK;
}

DSIHost::ErrorCode DSIHost::genericShortWrite(VirtualChannelID virtualChannelId, uint8_t parameter)
{
  while (not isCommandFIFOEmpty());

  uint32_t registerValueGHCR = 0u;

  setPacketDataType(registerValueGHCR, 0x13);
  setVirtualChannelID(registerValueGHCR, virtualChannelId);
  setShortPacketData0(registerValueGHCR, parameter);
  setShortPacketData1(registerValueGHCR, 0x0);

  MemoryAccess::setRegisterValue(&(m_DSIHostPeripheralPtr->GHCR), registerValueGHCR);

  return ErrorCode::OK;
}

DSIHost::ErrorCode
DSIHost::genericShortWrite(VirtualChannelID virtualChannelId, uint8_t parameter1, uint8_t parameter2)
{
  while (not isCommandFIFOEmpty());

  uint32_t registerValueGHCR = 0u;

  setPacketDataType(registerValueGHCR, 0x23);
  setVirtualChannelID(registerValueGHCR, virtualChannelId);
  setShortPacketData0(registerValueGHCR, parameter1);
  setShortPacketData1(registerValueGHCR, parameter2);

  MemoryAccess::setRegisterValue(&(m_DSIHostPeripheralPtr->GHCR), registerValueGHCR);

  return ErrorCode::OK;
}

DSIHost::ErrorCode DSIHost::dcsShortWrite(VirtualChannelID virtualChannelId, uint8_t dcsCommand)
{
  while (not isCommandFIFOEmpty());

  uint32_t registerValueGHCR = 0u;

  setPacketDataType(registerValueGHCR, 0x05);
  setVirtualChannelID(registerValueGHCR, virtualChannelId);
  setShortPacketData0(registerValueGHCR, dcsCommand);
  setShortPacketData1(registerValueGHCR, 0x0);

  MemoryAccess::setRegisterValue(&(m_DSIHostPeripheralPtr->GHCR), registerValueGHCR);

  return ErrorCode::OK;
}

DSIHost::ErrorCode DSIHost::dcsShortWrite(VirtualChannelID virtualChannelId, uint8_t dcsCommand, uint8_t parameter)
{
  while (not isCommandFIFOEmpty());

  uint32_t registerValueGHCR = 0u;

  setPacketDataType(registerValueGHCR, 0x15);
  setVirtualChannelID(registerValueGHCR, virtualChannelId);
  setShortPacketData0(registerValueGHCR, dcsCommand);
  setShortPacketData1(registerValueGHCR, parameter);

  MemoryAccess::setRegisterValue(&(m_DSIHostPeripheralPtr->GHCR), registerValueGHCR);

  return ErrorCode::OK;
}

DSIHost::ErrorCode DSIHost::genericLongWrite(VirtualChannelID virtualChannelId, const void *dataPtr, uint16_t dataSize)
{
  while (not isCommandFIFOEmpty());

  writeDataToTransmitInFIFO(reinterpret_cast<const uint8_t*>(dataPtr), dataSize);

  uint32_t registerValueGHCR = 0u;

  setPacketDataType(registerValueGHCR, 0x29);
  setVirtualChannelID(registerValueGHCR, virtualChannelId);
  setLongPacketDataSize(registerValueGHCR, dataSize);

  MemoryAccess::setRegisterValue(&(m_DSIHostPeripheralPtr->GHCR), registerValueGHCR);

  return ErrorCode::OK;
}

DSIHost::ErrorCode DSIHost::dcsLongWrite(VirtualChannelID virtualChannelId,
  uint8_t dcsCommand,
  const void *dataPtr,
  uint16_t dataSize)
{
  while (not isCommandFIFOEmpty());

  writeDCSCommandAndDataToTransmitInFIFO(dcsCommand, reinterpret_cast<const uint8_t*>(dataPtr), dataSize);

  uint32_t registerValueGHCR = 0u;

  setPacketDataType(registerValueGHCR, 0x39);
  setVirtualChannelID(registerValueGHCR, virtualChannelId);
  setLongPacketDataSize(registerValueGHCR, dataSize + 1u);

  MemoryAccess::setRegisterValue(&(m_DSIHostPeripheralPtr->GHCR), registerValueGHCR);

  return ErrorCode::OK;
}

DSIHost::ErrorCode DSIHost::configureDPHYPLL(const PLLConfig &pllConfig)
{
  ErrorCode errorCode = calculateDSIPHYClockFrequency(pllConfig, m_dsiPhyClockFreq);
  if (ErrorCode::OK == errorCode)
  {
    enableDPHYRegulator();

    uint32_t registerValueWRPCR = MemoryAccess::getRegisterValue(&(m_DSIHostPeripheralPtr->WRPCR));

    setPLLInputClockDivider(registerValueWRPCR, pllConfig.inputClockDivider);
    setPLLInputClockMultiplier(registerValueWRPCR, pllConfig.inputClockMultiplier);
    setPLLOutputClockDivider(registerValueWRPCR, pllConfig.outputClockDivider);

    MemoryAccess::setRegisterValue(&(m_DSIHostPeripheralPtr->WRPCR), registerValueWRPCR);

    enableDPHYPLL();
  }

  return errorCode;
}

DSIHost::ErrorCode DSIHost::configureDPHY(const DSIHostConfig &dsiHostConfig)
{
  enableDPHYClockLaneModuleAndDigitalSection();

  uint32_t registerValueCLCR = MemoryAccess::getRegisterValue(&(m_DSIHostPeripheralPtr->CLCR));

  disableAutomaticClockLaneControl(registerValueCLCR);
  setClockLaneRunningModeToHighSpeed(registerValueCLCR);

  MemoryAccess::setRegisterValue(&(m_DSIHostPeripheralPtr->CLCR), registerValueCLCR);

  setNumberOfDataLanes(dsiHostConfig.numberOfDataLanes);

  return ErrorCode::OK;
}

inline DSIHost::ErrorCode DSIHost::configureClocks(void)
{
  setHighSpeedModeClockFrequency();
  setTxEscapeClockFrequency();

  return ErrorCode::OK;
}

DSIHost::ErrorCode DSIHost::configureDSIPHYTiming(const DSIPHYTiming &dsiPhyTiming)
{
  setClockLaneHighSpeedToLowPowerTime(dsiPhyTiming.clockLaneHighSpeedToLowPowerTime);
  setClockLaneLowPowerToHighSpeedTime(dsiPhyTiming.clockLaneLowPowerToHighSpeedTime);
  setDataLaneHighSpeedToLowPowerTime(dsiPhyTiming.dataLaneHighSpeedToLowPowerTime);
  setDataLaneLowPowerToHighSpeedTime(dsiPhyTiming.dataLaneLowPowerToHighSpeedTime);
  setStopWaitTimeBeforeGoingToHighSpeed(dsiPhyTiming.stopWaitTime);

  return ErrorCode::OK;
}

DSIHost::ErrorCode DSIHost::configureDSIHostTimeouts(void)
{
  setTimeoutClockDivisionFactor(1u);
  setHighSpeedTransmissionTimeout(0u);
  setLowPowerReceptionTimeout(0u);
  setHighSpeedReadTimeout(0u);
  setLowPowerReadTimeout(0u);

  uint32_t registerValueTCCR3 = MemoryAccess::getRegisterValue(&(m_DSIHostPeripheralPtr->TCCR[3]));

  setHighSpeedWriteTimeout(registerValueTCCR3, 0u);
  disablePrespMode(registerValueTCCR3);

  MemoryAccess::setRegisterValue(&(m_DSIHostPeripheralPtr->TCCR[3]), registerValueTCCR3);

  setLowPowerWriteTimeout(0u);
  setBusTurnAroundTimeout(0u);

  return ErrorCode::OK;
}

DSIHost::ErrorCode DSIHost::configureFlowControl(const FlowControlConfig &flowControlConfig)
{
  uint32_t registerValuePCR = MemoryAccess::getRegisterValue(&(m_DSIHostPeripheralPtr->PCR));

  if (flowControlConfig.enableCRCReception)
  {
    enableCRCReception(registerValuePCR);
  }
  else
  {
    disableCRCReception(registerValuePCR);
  }

  if (flowControlConfig.enableECCReception)
  {
    enableECCReception(registerValuePCR);
  }
  else
  {
    disableECCReception(registerValuePCR);
  }

  if (flowControlConfig.enableBusTurnAround)
  {
    enableBusTurnAround(registerValuePCR);
  }
  else
  {
    disableBusTurnAround(registerValuePCR);
  }

  if (flowControlConfig.enableEoTpReception)
  {
    enableEoTpReception(registerValuePCR);
  }
  else
  {
    disableEoTpReception(registerValuePCR);
  }

  if (flowControlConfig.enableEoTpTransmission)
  {
    enableEoTpTransmission(registerValuePCR);
  }
  else
  {
    disableEoTpTransmission(registerValuePCR);
  }

  MemoryAccess::setRegisterValue(&(m_DSIHostPeripheralPtr->PCR), registerValuePCR);

  return ErrorCode::OK;
}

DSIHost::ErrorCode DSIHost::configureDBIInterface(const DBIInterfaceConfig &dbiInterfaceConfig)
{
  uint32_t registerValueCMCR = MemoryAccess::getRegisterValue(&(m_DSIHostPeripheralPtr->CMCR));

  setMaximumReadPacketSizeTransmissionType(registerValueCMCR, dbiInterfaceConfig.transmissionTypeMaximumReadPacketSize);
  setDCSLongWriteTransmissionType(registerValueCMCR, dbiInterfaceConfig.transmissionTypeDCSLongWrite);
  setDCSShortReadZeroParamTransmissionType(registerValueCMCR, dbiInterfaceConfig.transmissionTypeDCSShortReadZeroParam);
  setDCSShortReadOneParamTransmissionType(registerValueCMCR, dbiInterfaceConfig.transmissionTypeDCSShortReadOneParam);
  setDCSShortWriteZeroParamTransmissionType(registerValueCMCR, dbiInterfaceConfig.transmissionTypeDCSShortWriteZeroParam);
  setGenericLongWriteTransmissionType(registerValueCMCR, dbiInterfaceConfig.transmissionTypeGenericLongWrite);
  setGenericShortReadTwoParamTransmissionType(registerValueCMCR, dbiInterfaceConfig.transmissionTypeGenericShortReadTwoParam);
  setGenericShortReadOneParamTransmissionType(registerValueCMCR, dbiInterfaceConfig.transmissionTypeGenericShortReadOneParam);
  setGenericShortReadZeroParamTransmissionType(registerValueCMCR, dbiInterfaceConfig.transmissionTypeGenericShortReadZeroParam);
  setGenericShortWriteTwoParamTransmissionType(registerValueCMCR, dbiInterfaceConfig.transmissionTypeGenericShortWriteTwoParam);
  setGenericShortWriteOneParamTransmissionType(registerValueCMCR, dbiInterfaceConfig.transmissionTypeGenericShortWriteOneParam);
  setGenericShortWriteZeroParamTransmissionType(registerValueCMCR, dbiInterfaceConfig.transmissionTypeGenericShortWriteZeroParam);

  if (dbiInterfaceConfig.enableAcknowledgeRequest)
  {
    enableAcknowledgeRequest(registerValueCMCR);
  }
  else
  {
    disableAcknowledgeRequest(registerValueCMCR);
  }

  if (dbiInterfaceConfig.enableTearingEffectAcknowledgeRequest)
  {
    enableTearingEffectAcknowledgeRequest(registerValueCMCR);
  }
  else
  {
    disableTearingEffectAcknowledgeRequest(registerValueCMCR);
  }

  MemoryAccess::setRegisterValue(&(m_DSIHostPeripheralPtr->CMCR), registerValueCMCR);

  return ErrorCode::OK;
}

DSIHost::ErrorCode DSIHost::configureDSIHostLTDCInterface(const DSIHostConfig &dsiHostConfig)
{
  setVirtualChannelID(dsiHostConfig.virtualChannelId);
  setLTDCColorCoding(dsiHostConfig.ltdcColorCoding);

  uint32_t registerValueWCFGR = MemoryAccess::getRegisterValue(&(m_DSIHostPeripheralPtr->WCFGR));

  setColorMultiplexing(registerValueWCFGR, dsiHostConfig.ltdcColorCoding);
  setVSyncLTDCHaltPolarity(registerValueWCFGR, dsiHostConfig.vsyncLtdcHaltPolarity);

  MemoryAccess::setRegisterValue(&(m_DSIHostPeripheralPtr->WCFGR), registerValueWCFGR);

  uint32_t registerValueLPCR = MemoryAccess::getRegisterValue(&(m_DSIHostPeripheralPtr->LPCR));

  setHSyncPolarity(registerValueLPCR, dsiHostConfig.hsyncPolarity);
  setVSyncPolarity(registerValueLPCR, dsiHostConfig.vsyncPolarity);
  setDataEnablePolarity(registerValueLPCR, dsiHostConfig.dataEnablePolarity);

  MemoryAccess::setRegisterValue(&(m_DSIHostPeripheralPtr->LPCR), registerValueLPCR);

  return ErrorCode::OK;
}

void DSIHost::enableAdaptedCommandMode(void)
{
  setDSIModeToAdaptedCommandMode();
  setCommandModeToAdaptedCommandMode();
}

DSIHost::ErrorCode DSIHost::configureAdaptedCommandMode(const DSIHostConfig &dsiHostConfig)
{
  setMaximumLTDCWriteMemoryCommandSize(dsiHostConfig.maxLTDCWriteMemoryCmdSize);

  uint32_t registerValueWCFGR = MemoryAccess::getRegisterValue(&(m_DSIHostPeripheralPtr->WCFGR));

  setTearingEffectSource(registerValueWCFGR, dsiHostConfig.tearingEffectSource);
  setTearingEffectPolarity(registerValueWCFGR, dsiHostConfig.tearingEffectPolarity);

  if (dsiHostConfig.enableAutoRefreshMode)
  {
    enableAutoRefreshMode(registerValueWCFGR);
  }
  else
  {
    disableAutoRefreshMode(registerValueWCFGR);
  }

  MemoryAccess::setRegisterValue(&(m_DSIHostPeripheralPtr->WCFGR), registerValueWCFGR);

  return ErrorCode::OK;
}

DSIHost::ErrorCode DSIHost::getDSIPHYClockFrequency(uint32_t &dsiPhyClockFreq)
{
  dsiPhyClockFreq = m_dsiPhyClockFreq;

  return ErrorCode::OK;
}

DSIHost::ErrorCode DSIHost::calculateDSIPHYClockFrequency(const PLLConfig &pllConfig, uint32_t &dsiPhyClockFreq)
{
  uint32_t hseClockFrequency;

  ClockControl::ErrorCode errorCode =
    m_clockControlPtr->getClockFrequency(ClockControl::Clock::HSE, hseClockFrequency);
  if (ClockControl::ErrorCode::OK != errorCode)
  {
    return DSIHost::ErrorCode::CAN_NOT_GET_HSE_CLOCK_FREQUENCY;
  }

  dsiPhyClockFreq = ((hseClockFrequency / pllConfig.inputClockDivider) * pllConfig.inputClockMultiplier) /
    pllConfig.outputClockDivider;

  return ErrorCode::OK;
}

inline DSIHost::ErrorCode DSIHost::enablePeripheralClock(void)
{
  ResetControl::ErrorCode errorCode = m_resetControlPtr->enablePeripheralClock(getPeripheralTag());

  return (ResetControl::ErrorCode::OK == errorCode) ? ErrorCode::OK : ErrorCode::CAN_NOT_TURN_ON_PERIPHERAL_CLOCK;
}

void DSIHost::enableDPHYRegulator(void)
{
  requestTurningOnDPHYRegulator();

  while (not isDPHYRegulatorReady());
}

void DSIHost::requestTurningOnDPHYRegulator(void)
{
  constexpr uint32_t DSIHOST_WRPCR_REGEN_POSITION = 24u;
  RegisterUtility<uint32_t>::setBitInRegister(&(m_DSIHostPeripheralPtr->WRPCR), DSIHOST_WRPCR_REGEN_POSITION);
}

bool DSIHost::isDPHYRegulatorReady(void) const
{
  constexpr uint32_t DSIHOST_WISR_RRS_POSITION = 12u;
  return RegisterUtility<uint32_t>::isBitSetInRegister(&(m_DSIHostPeripheralPtr->WISR), DSIHOST_WISR_RRS_POSITION);
}

void DSIHost::setPLLInputClockDivider(uint32_t &registerValueWRPCR, uint8_t inputClockDivider)
{
  constexpr uint32_t DSIHOST_WRPCR_IDF_POSITION = 11u;
  constexpr uint32_t DSIHOST_WRPCR_IDF_SIZE     = 4u;

  registerValueWRPCR = MemoryUtility<uint32_t>::setBits(
    registerValueWRPCR,
    DSIHOST_WRPCR_IDF_POSITION,
    DSIHOST_WRPCR_IDF_SIZE,
    inputClockDivider);
}

void DSIHost::setPLLInputClockMultiplier(uint32_t &registerValueWRPCR, uint8_t inputClockMultiplier)
{
  constexpr uint32_t DSIHOST_WRPCR_NDIV_POSITION = 2u;
  constexpr uint32_t DSIHOST_WRPCR_NDIV_SIZE     = 7u;

  registerValueWRPCR = MemoryUtility<uint32_t>::setBits(
    registerValueWRPCR,
    DSIHOST_WRPCR_NDIV_POSITION,
    DSIHOST_WRPCR_NDIV_SIZE,
    inputClockMultiplier);
}

void DSIHost::setPLLOutputClockDivider(uint32_t &registerValueWRPCR, uint8_t outputClockDivider)
{
  constexpr uint32_t DSIHOST_WRPCR_ODF_POSITION = 16u;
  constexpr uint32_t DSIHOST_WRPCR_ODF_SIZE     = 2u;

  registerValueWRPCR = MemoryUtility<uint32_t>::setBits(
    registerValueWRPCR,
    DSIHOST_WRPCR_ODF_POSITION,
    DSIHOST_WRPCR_ODF_SIZE,
    (outputClockDivider + 1u) * 2u / 5u);
}

void DSIHost::enableDPHYPLL(void)
{
  requestTurningOnDPHYPLL();

  while (not isDPHYPLLReady());
};

void DSIHost::requestTurningOnDPHYPLL(void)
{
  constexpr uint32_t DSIHOST_WRPCR_PLLEN_POSITION = 0u;
  RegisterUtility<uint32_t>::setBitInRegister(&(m_DSIHostPeripheralPtr->WRPCR), DSIHOST_WRPCR_PLLEN_POSITION);
}

bool DSIHost::isDPHYPLLReady(void) const
{
  constexpr uint32_t DSIHOST_WISR_PLLLS_POSITION = 8u;
  return RegisterUtility<uint32_t>::isBitSetInRegister(&(m_DSIHostPeripheralPtr->WISR), DSIHOST_WISR_PLLLS_POSITION);
}

void DSIHost::setNumberOfDataLanes(uint8_t numberOfDataLanes)
{
  constexpr uint32_t DSIHOST_PCONFR_NL_POSITION = 0u;
  constexpr uint32_t DSIHOST_PCONFR_NL_SIZE     = 2u;

  RegisterUtility<uint32_t>::setBitsInRegister(
    &(m_DSIHostPeripheralPtr->PCONFR),
    DSIHOST_PCONFR_NL_POSITION,
    DSIHOST_PCONFR_NL_SIZE,
    (numberOfDataLanes - 1u));
}

void DSIHost::enableDPHYClockLaneModuleAndDigitalSection(void)
{
  uint32_t registerValuePCTLR = MemoryAccess::getRegisterValue(&(m_DSIHostPeripheralPtr->PCTLR));

  enableDPHYClockLaneModule(registerValuePCTLR);
  enableDPHYDigitalSection(registerValuePCTLR);

  MemoryAccess::setRegisterValue(&(m_DSIHostPeripheralPtr->PCTLR), registerValuePCTLR);
}

void DSIHost::enableDPHYClockLaneModule(uint32_t &registerValuePCTLR)
{
  constexpr uint32_t DSIHOST_PCTLR_CKE_POSITION = 2u;

  registerValuePCTLR = MemoryUtility<uint32_t>::setBit(registerValuePCTLR, DSIHOST_PCTLR_CKE_POSITION);
}

void DSIHost::enableDPHYDigitalSection(uint32_t &registerValuePCTLR)
{
  constexpr uint32_t DSIHOST_PCTLR_DEN_POSITION = 1u;

  registerValuePCTLR = MemoryUtility<uint32_t>::setBit(registerValuePCTLR, DSIHOST_PCTLR_DEN_POSITION);
}

void DSIHost::disableAutomaticClockLaneControl(uint32_t &registerValueCLCR)
{
  constexpr uint32_t DSIHOST_CLCR_ACR_POSITION = 1u;

  registerValueCLCR = MemoryUtility<uint32_t>::resetBit(registerValueCLCR, DSIHOST_CLCR_ACR_POSITION);
}

void DSIHost::setClockLaneRunningModeToHighSpeed(uint32_t &registerValueCLCR)
{
  constexpr uint32_t DSIHOST_CLCR_DPCC_POSITION = 0u;

  registerValueCLCR = MemoryUtility<uint32_t>::setBit(registerValueCLCR, DSIHOST_CLCR_DPCC_POSITION);
}

void DSIHost::setHighSpeedModeClockFrequency(void)
{
  constexpr uint32_t DSIHOST_WPCR0_UIX4_POSITION = 0u;
  constexpr uint32_t DSIHOST_WPCR0_UIX4_SIZE     = 6u;

  const uint32_t uix4Value = 4u * NANOSECONDS_IN_MICROSECOND / (m_dsiPhyClockFreq / FREQ_HZ_TO_MHZ_DIVIDER);

  RegisterUtility<uint32_t>::setBitsInRegister(
    &(m_DSIHostPeripheralPtr->WPCR[0]),
    DSIHOST_WPCR0_UIX4_POSITION,
    DSIHOST_WPCR0_UIX4_SIZE,
    uix4Value);
}

void DSIHost::setTxEscapeClockFrequency(void)
{
  constexpr uint32_t TARGET_FREQ_MHZ = 20u;
  constexpr uint32_t DSIHOST_CCR_TXECKDIV_POSITION = 0u;
  constexpr uint32_t DSIHOST_CCR_TXECKDIV_SIZE     = 8u;

  const uint32_t txEscapeClockDivider =
    (((m_dsiPhyClockFreq / FREQ_HZ_TO_MHZ_DIVIDER) / BITS_IN_BYTE) + TARGET_FREQ_MHZ - 1u) / TARGET_FREQ_MHZ;

  RegisterUtility<uint32_t>::setBitsInRegister(
    &(m_DSIHostPeripheralPtr->CCR),
    DSIHOST_CCR_TXECKDIV_POSITION,
    DSIHOST_CCR_TXECKDIV_SIZE,
    txEscapeClockDivider);
}

void DSIHost::setClockLaneHighSpeedToLowPowerTime(uint32_t clockLaneHighSpeedToLowPowerTime)
{
  constexpr uint32_t DSIHOST_CLTCR_HS2LP_TIME_POSITION = 16u;
  constexpr uint32_t DSIHOST_CLTCR_HS2LP_TIME_SIZE     = 10u;

  RegisterUtility<uint32_t>::setBitsInRegister(
    &(m_DSIHostPeripheralPtr->CLTCR),
    DSIHOST_CLTCR_HS2LP_TIME_POSITION,
    DSIHOST_CLTCR_HS2LP_TIME_SIZE,
    (clockLaneHighSpeedToLowPowerTime / (NANOSECONDS_IN_SECOND / (m_dsiPhyClockFreq / BITS_IN_BYTE))));
}

 void DSIHost::setClockLaneLowPowerToHighSpeedTime(uint32_t clockLaneLowPowerToHighSpeedTime)
 {
  constexpr uint32_t DSIHOST_CLTCR_LP2HS_TIME_POSITION = 0u;
  constexpr uint32_t DSIHOST_CLTCR_LP2HS_TIME_SIZE     = 10u;

  RegisterUtility<uint32_t>::setBitsInRegister(
    &(m_DSIHostPeripheralPtr->CLTCR),
    DSIHOST_CLTCR_LP2HS_TIME_POSITION,
    DSIHOST_CLTCR_LP2HS_TIME_SIZE,
    (clockLaneLowPowerToHighSpeedTime / (NANOSECONDS_IN_SECOND / (m_dsiPhyClockFreq / BITS_IN_BYTE))));
 }

void DSIHost::setDataLaneHighSpeedToLowPowerTime(uint32_t dataLaneHighSpeedToLowPowerTime)
{
  constexpr uint32_t DSIHOST_DLTCR_HS2LP_TIME_POSITION = 24u;
  constexpr uint32_t DSIHOST_DLTCR_HS2LP_TIME_SIZE     = 8u;

  RegisterUtility<uint32_t>::setBitsInRegister(
    &(m_DSIHostPeripheralPtr->DLTCR),
    DSIHOST_DLTCR_HS2LP_TIME_POSITION,
    DSIHOST_DLTCR_HS2LP_TIME_SIZE,
    (dataLaneHighSpeedToLowPowerTime / (NANOSECONDS_IN_SECOND / (m_dsiPhyClockFreq / BITS_IN_BYTE))));
}

void DSIHost::setDataLaneLowPowerToHighSpeedTime(uint32_t dataLaneLowPowerToHighSpeedTime)
{
  constexpr uint32_t DSIHOST_DLTCR_LP2HS_TIME_POSITION = 16u;
  constexpr uint32_t DSIHOST_DLTCR_LP2HS_TIME_SIZE     = 8u;

  RegisterUtility<uint32_t>::setBitsInRegister(
    &(m_DSIHostPeripheralPtr->DLTCR),
    DSIHOST_DLTCR_LP2HS_TIME_POSITION,
    DSIHOST_DLTCR_LP2HS_TIME_SIZE,
    (dataLaneLowPowerToHighSpeedTime / (NANOSECONDS_IN_SECOND / (m_dsiPhyClockFreq / BITS_IN_BYTE))));
}

void DSIHost::setStopWaitTimeBeforeGoingToHighSpeed(uint32_t stopWaitTime)
{
  constexpr uint32_t DSIHOST_PCONFR_SW_TIME_POSITION = 8u;
  constexpr uint32_t DSIHOST_PCONFR_SW_TIME_SIZE     = 8u;

  RegisterUtility<uint32_t>::setBitsInRegister(
    &(m_DSIHostPeripheralPtr->PCONFR),
    DSIHOST_PCONFR_SW_TIME_POSITION,
    DSIHOST_PCONFR_SW_TIME_SIZE,
    (stopWaitTime / (NANOSECONDS_IN_SECOND / (m_dsiPhyClockFreq / BITS_IN_BYTE))));
}

void DSIHost::setTimeoutClockDivisionFactor(uint8_t timeoutClockDivisionFactor)
{
  constexpr uint32_t DSIHOST_CCR_TOCKDIV_POSITION = 8u;
  constexpr uint32_t DSIHOST_CCR_TOCKDIV_SIZE     = 8u;

  RegisterUtility<uint32_t>::setBitsInRegister(
    &(m_DSIHostPeripheralPtr->CCR),
    DSIHOST_CCR_TOCKDIV_POSITION,
    DSIHOST_CCR_TOCKDIV_SIZE,
    timeoutClockDivisionFactor);
}

void DSIHost::setHighSpeedTransmissionTimeout(uint8_t highSpeedTransmissionTimeout)
{
  constexpr uint32_t DSIHOST_TCCR0_HSTX_TOCNT_POSITION = 16u;
  constexpr uint32_t DSIHOST_TCCR0_HSTX_TOCNT_SIZE     = 16u;

  RegisterUtility<uint32_t>::setBitsInRegister(
    &(m_DSIHostPeripheralPtr->TCCR[0]),
    DSIHOST_TCCR0_HSTX_TOCNT_POSITION,
    DSIHOST_TCCR0_HSTX_TOCNT_SIZE,
    highSpeedTransmissionTimeout);
}

void DSIHost::setLowPowerReceptionTimeout(uint8_t lowPowerReceptionTimeout)
{
  constexpr uint32_t DSIHOST_TCCR0_LPRX_TOCNT_POSITION = 0u;
  constexpr uint32_t DSIHOST_TCCR0_LPRX_TOCNT_SIZE     = 16u;

  RegisterUtility<uint32_t>::setBitsInRegister(
    &(m_DSIHostPeripheralPtr->TCCR[0]),
    DSIHOST_TCCR0_LPRX_TOCNT_POSITION,
    DSIHOST_TCCR0_LPRX_TOCNT_SIZE,
    lowPowerReceptionTimeout);
}

void DSIHost::setHighSpeedReadTimeout(uint8_t highSpeedReadTimeout)
{
  constexpr uint32_t DSIHOST_TCCR1_HSRD_TOCNT_POSITION = 0u;
  constexpr uint32_t DSIHOST_TCCR1_HSRD_TOCNT_SIZE     = 16u;

  RegisterUtility<uint32_t>::setBitsInRegister(
    &(m_DSIHostPeripheralPtr->TCCR[1]),
    DSIHOST_TCCR1_HSRD_TOCNT_POSITION,
    DSIHOST_TCCR1_HSRD_TOCNT_SIZE,
    highSpeedReadTimeout);
}

void DSIHost::setLowPowerReadTimeout(uint8_t lowPowerReadTimeout)
{
  constexpr uint32_t DSIHOST_TCCR2_LPRD_TOCNT_POSITION = 0u;
  constexpr uint32_t DSIHOST_TCCR2_LPRD_TOCNT_SIZE     = 16u;

  RegisterUtility<uint32_t>::setBitsInRegister(
    &(m_DSIHostPeripheralPtr->TCCR[2]),
    DSIHOST_TCCR2_LPRD_TOCNT_POSITION,
    DSIHOST_TCCR2_LPRD_TOCNT_SIZE,
    lowPowerReadTimeout);
}

void DSIHost::setHighSpeedWriteTimeout(uint32_t &registerValueTCCR3, uint8_t highSpeedWriteTimeout)
{
  constexpr uint32_t DSIHOST_TCCR3_HSWR_TOCNT_POSITION = 0u;
  constexpr uint32_t DSIHOST_TCCR3_HSWR_TOCNT_SIZE     = 16u;

  registerValueTCCR3 = MemoryUtility<uint32_t>::setBits(
    registerValueTCCR3,
    DSIHOST_TCCR3_HSWR_TOCNT_POSITION,
    DSIHOST_TCCR3_HSWR_TOCNT_SIZE,
    highSpeedWriteTimeout);
}

void DSIHost::disablePrespMode(uint32_t &registerValueTCCR3)
{
  constexpr uint32_t DSIHOST_TCCR3_PM_POSITION = 24u;

  registerValueTCCR3 = MemoryUtility<uint32_t>::resetBit(registerValueTCCR3, DSIHOST_TCCR3_PM_POSITION);
}

void DSIHost::setLowPowerWriteTimeout(uint8_t lowPowerWriteTimeout)
{
  constexpr uint32_t DSIHOST_TCCR4_LSWR_TOCNT_POSITION = 0u;
  constexpr uint32_t DSIHOST_TCCR4_LSWR_TOCNT_SIZE     = 16u;

  RegisterUtility<uint32_t>::setBitsInRegister(
    &(m_DSIHostPeripheralPtr->TCCR[4]),
    DSIHOST_TCCR4_LSWR_TOCNT_POSITION,
    DSIHOST_TCCR4_LSWR_TOCNT_SIZE,
    lowPowerWriteTimeout);
}

void DSIHost::setBusTurnAroundTimeout(uint8_t busTurnAroundTimeout)
{
  constexpr uint32_t DSIHOST_TCCR5_BTA_TOCNT_POSITION = 0u;
  constexpr uint32_t DSIHOST_TCCR5_BTA_TOCNT_SIZE     = 16u;

  RegisterUtility<uint32_t>::setBitsInRegister(
    &(m_DSIHostPeripheralPtr->TCCR[5]),
    DSIHOST_TCCR5_BTA_TOCNT_POSITION,
    DSIHOST_TCCR5_BTA_TOCNT_SIZE,
    busTurnAroundTimeout);
}

inline void DSIHost::enableCRCReception(uint32_t &registerValuePCR)
{
  constexpr uint32_t DSIHOST_PCR_CRCRXE_POSITION = 4u;
  registerValuePCR = MemoryUtility<uint32_t>::setBit(registerValuePCR, DSIHOST_PCR_CRCRXE_POSITION);
}

inline void DSIHost::disableCRCReception(uint32_t &registerValuePCR)
{
  constexpr uint32_t DSIHOST_PCR_CRCRXE_POSITION = 4u;
  registerValuePCR = MemoryUtility<uint32_t>::resetBit(registerValuePCR, DSIHOST_PCR_CRCRXE_POSITION);
}

inline void DSIHost::enableECCReception(uint32_t &registerValuePCR)
{
  constexpr uint32_t DSIHOST_PCR_ECCRXE_POSITION = 3u;
  registerValuePCR = MemoryUtility<uint32_t>::setBit(registerValuePCR, DSIHOST_PCR_ECCRXE_POSITION);
}

inline void DSIHost::disableECCReception(uint32_t &registerValuePCR)
{
  constexpr uint32_t DSIHOST_PCR_ECCRXE_POSITION = 3u;
  registerValuePCR = MemoryUtility<uint32_t>::resetBit(registerValuePCR, DSIHOST_PCR_ECCRXE_POSITION);
}

inline void DSIHost::enableBusTurnAround(uint32_t &registerValuePCR)
{
  constexpr uint32_t DSIHOST_PCR_BTAE_POSITION = 2u;
  registerValuePCR = MemoryUtility<uint32_t>::setBit(registerValuePCR, DSIHOST_PCR_BTAE_POSITION);
}

inline void DSIHost::disableBusTurnAround(uint32_t &registerValuePCR)
{
  constexpr uint32_t DSIHOST_PCR_BTAE_POSITION = 2u;
  registerValuePCR = MemoryUtility<uint32_t>::resetBit(registerValuePCR, DSIHOST_PCR_BTAE_POSITION);
}

inline void DSIHost::enableEoTpReception(uint32_t &registerValuePCR)
{
  constexpr uint32_t DSIHOST_PCR_ETRXE_POSITION = 1u;
  registerValuePCR = MemoryUtility<uint32_t>::setBit(registerValuePCR, DSIHOST_PCR_ETRXE_POSITION);
}

inline void DSIHost::disableEoTpReception(uint32_t &registerValuePCR)
{
  constexpr uint32_t DSIHOST_PCR_ETRXE_POSITION = 1u;
  registerValuePCR = MemoryUtility<uint32_t>::resetBit(registerValuePCR, DSIHOST_PCR_ETRXE_POSITION);
}

inline void DSIHost::enableEoTpTransmission(uint32_t &registerValuePCR)
{
  constexpr uint32_t DSIHOST_PCR_ETTXE_POSITION = 0u;
  registerValuePCR = MemoryUtility<uint32_t>::setBit(registerValuePCR, DSIHOST_PCR_ETTXE_POSITION);
}

inline void DSIHost::disableEoTpTransmission(uint32_t &registerValuePCR)
{
  constexpr uint32_t DSIHOST_PCR_ETTXE_POSITION = 0u;
  registerValuePCR = MemoryUtility<uint32_t>::resetBit(registerValuePCR, DSIHOST_PCR_ETTXE_POSITION);
}

void DSIHost::setMaximumReadPacketSizeTransmissionType(uint32_t &registerValueCMCR, TransmissionType transmissionType)
{
  constexpr uint32_t DSIHOST_CMCR_MRDPS_POSITION    = 24u;
  constexpr uint32_t DSIHOST_CMCR_MRDPS_NUM_OF_BITS = 1u;

  registerValueCMCR = MemoryUtility<uint32_t>::setBits(
    registerValueCMCR,
    DSIHOST_CMCR_MRDPS_POSITION,
    DSIHOST_CMCR_MRDPS_NUM_OF_BITS,
    static_cast<uint32_t>(transmissionType));
}

void DSIHost::setDCSLongWriteTransmissionType(uint32_t &registerValueCMCR, TransmissionType transmissionType)
{
  constexpr uint32_t DSIHOST_CMCR_DLWTX_POSITION    = 19u;
  constexpr uint32_t DSIHOST_CMCR_DLWTX_NUM_OF_BITS = 1u;

  registerValueCMCR = MemoryUtility<uint32_t>::setBits(
    registerValueCMCR,
    DSIHOST_CMCR_DLWTX_POSITION,
    DSIHOST_CMCR_DLWTX_NUM_OF_BITS,
    static_cast<uint32_t>(transmissionType));
}

void DSIHost::setDCSShortReadZeroParamTransmissionType(uint32_t &registerValueCMCR, TransmissionType transmissionType)
{
  constexpr uint32_t DSIHOST_CMCR_DSR0TX_POSITION    = 18u;
  constexpr uint32_t DSIHOST_CMCR_DSR0TX_NUM_OF_BITS = 1u;

  registerValueCMCR = MemoryUtility<uint32_t>::setBits(
    registerValueCMCR,
    DSIHOST_CMCR_DSR0TX_POSITION,
    DSIHOST_CMCR_DSR0TX_NUM_OF_BITS,
    static_cast<uint32_t>(transmissionType));
}

void DSIHost::setDCSShortReadOneParamTransmissionType(uint32_t &registerValueCMCR, TransmissionType transmissionType)
{
  constexpr uint32_t DSIHOST_CMCR_DSW1TX_POSITION = 17u;
  constexpr uint32_t DSIHOST_CMCR_DSW1TX_NUM_OF_BITS = 1u;

  registerValueCMCR = MemoryUtility<uint32_t>::setBits(
    registerValueCMCR,
    DSIHOST_CMCR_DSW1TX_POSITION,
    DSIHOST_CMCR_DSW1TX_NUM_OF_BITS,
    static_cast<uint32_t>(transmissionType));
}

void DSIHost::setDCSShortWriteZeroParamTransmissionType(uint32_t &registerValueCMCR, TransmissionType transmissionType)
{
  constexpr uint32_t DSIHOST_CMCR_DSW0TX_POSITION = 16u;
  constexpr uint32_t DSIHOST_CMCR_DSW0TX_NUM_OF_BITS = 1u;

  registerValueCMCR = MemoryUtility<uint32_t>::setBits(
    registerValueCMCR,
    DSIHOST_CMCR_DSW0TX_POSITION,
    DSIHOST_CMCR_DSW0TX_NUM_OF_BITS,
    static_cast<uint32_t>(transmissionType));
}

void DSIHost::setGenericLongWriteTransmissionType(uint32_t &registerValueCMCR, TransmissionType transmissionType)
{
  constexpr uint32_t DSIHOST_CMCR_GLWTX_POSITION = 14u;
  constexpr uint32_t DSIHOST_CMCR_GLWTX_NUM_OF_BITS = 1u;

  registerValueCMCR = MemoryUtility<uint32_t>::setBits(
    registerValueCMCR,
    DSIHOST_CMCR_GLWTX_POSITION,
    DSIHOST_CMCR_GLWTX_NUM_OF_BITS,
    static_cast<uint32_t>(transmissionType));
}

void
DSIHost::setGenericShortReadTwoParamTransmissionType(uint32_t &registerValueCMCR, TransmissionType transmissionType)
{
  constexpr uint32_t DSIHOST_CMCR_GSR2TX_POSITION = 13u;
  constexpr uint32_t DSIHOST_CMCR_GSR2TX_NUM_OF_BITS = 1u;

  registerValueCMCR = MemoryUtility<uint32_t>::setBits(
    registerValueCMCR,
    DSIHOST_CMCR_GSR2TX_POSITION,
    DSIHOST_CMCR_GSR2TX_NUM_OF_BITS,
    static_cast<uint32_t>(transmissionType));
}

void
DSIHost::setGenericShortReadOneParamTransmissionType(uint32_t &registerValueCMCR, TransmissionType transmissionType)
{
  constexpr uint32_t DSIHOST_CMCR_GSR1TX_POSITION = 12u;
  constexpr uint32_t DSIHOST_CMCR_GSR1TX_NUM_OF_BITS = 1u;

  registerValueCMCR = MemoryUtility<uint32_t>::setBits(
    registerValueCMCR,
    DSIHOST_CMCR_GSR1TX_POSITION,
    DSIHOST_CMCR_GSR1TX_NUM_OF_BITS,
    static_cast<uint32_t>(transmissionType));
}

void
DSIHost::setGenericShortReadZeroParamTransmissionType(uint32_t &registerValueCMCR, TransmissionType transmissionType)
{
  constexpr uint32_t DSIHOST_CMCR_GSR0TX_POSITION = 11u;
  constexpr uint32_t DSIHOST_CMCR_GSR0TX_NUM_OF_BITS = 1u;

  registerValueCMCR = MemoryUtility<uint32_t>::setBits(
    registerValueCMCR,
    DSIHOST_CMCR_GSR0TX_POSITION,
    DSIHOST_CMCR_GSR0TX_NUM_OF_BITS,
    static_cast<uint32_t>(transmissionType));
}

void
DSIHost::setGenericShortWriteTwoParamTransmissionType(uint32_t &registerValueCMCR, TransmissionType transmissionType)
{
  constexpr uint32_t DSIHOST_CMCR_GSW2TX_POSITION = 10u;
  constexpr uint32_t DSIHOST_CMCR_GSW2TX_NUM_OF_BITS = 1u;

  registerValueCMCR = MemoryUtility<uint32_t>::setBits(
    registerValueCMCR,
    DSIHOST_CMCR_GSW2TX_POSITION,
    DSIHOST_CMCR_GSW2TX_NUM_OF_BITS,
    static_cast<uint32_t>(transmissionType));
}

void
DSIHost::setGenericShortWriteOneParamTransmissionType(uint32_t &registerValueCMCR, TransmissionType transmissionType)
{
  constexpr uint32_t DSIHOST_CMCR_GSW1TX_POSITION = 9u;
  constexpr uint32_t DSIHOST_CMCR_GSW1TX_NUM_OF_BITS = 1u;

  registerValueCMCR = MemoryUtility<uint32_t>::setBits(
    registerValueCMCR,
    DSIHOST_CMCR_GSW1TX_POSITION,
    DSIHOST_CMCR_GSW1TX_NUM_OF_BITS,
    static_cast<uint32_t>(transmissionType));
}

void
DSIHost::setGenericShortWriteZeroParamTransmissionType(uint32_t &registerValueCMCR, TransmissionType transmissionType)
{
  constexpr uint32_t DSIHOST_CMCR_GSW0TX_POSITION = 8u;
  constexpr uint32_t DSIHOST_CMCR_GSW0TX_NUM_OF_BITS = 1u;

  registerValueCMCR = MemoryUtility<uint32_t>::setBits(
    registerValueCMCR,
    DSIHOST_CMCR_GSW0TX_POSITION,
    DSIHOST_CMCR_GSW0TX_NUM_OF_BITS,
    static_cast<uint32_t>(transmissionType));
}

inline void DSIHost::enableAcknowledgeRequest(uint32_t &registerValueCMCR)
{
  constexpr uint32_t DSIHOST_CMCR_ARE_POSITION = 1u;
  registerValueCMCR = MemoryUtility<uint32_t>::setBit(registerValueCMCR, DSIHOST_CMCR_ARE_POSITION);
}

inline void DSIHost::disableAcknowledgeRequest(uint32_t &registerValueCMCR)
{
  constexpr uint32_t DSIHOST_CMCR_ARE_POSITION = 1u;
  registerValueCMCR = MemoryUtility<uint32_t>::resetBit(registerValueCMCR, DSIHOST_CMCR_ARE_POSITION);
}

inline void DSIHost::enableTearingEffectAcknowledgeRequest(uint32_t &registerValueCMCR)
{
  constexpr uint32_t DSIHOST_CMCR_TEARE_POSITION = 0u;
  registerValueCMCR = MemoryUtility<uint32_t>::setBit(registerValueCMCR, DSIHOST_CMCR_TEARE_POSITION);
}

inline void DSIHost::disableTearingEffectAcknowledgeRequest(uint32_t &registerValueCMCR)
{
  constexpr uint32_t DSIHOST_CMCR_TEARE_POSITION = 0u;
  registerValueCMCR = MemoryUtility<uint32_t>::resetBit(registerValueCMCR, DSIHOST_CMCR_TEARE_POSITION);
}

void DSIHost::setVirtualChannelID(uint8_t virtualChannelId)
{
  constexpr uint32_t DSIHOST_LCVCIDR_VCID_POSITION = 0u;
  constexpr uint32_t DSIHOST_LCVCIDR_VCID_NUM_OF_BITS = 2u;

  RegisterUtility<uint32_t>::setBitsInRegister(
    &(m_DSIHostPeripheralPtr->LCVCIDR),
    DSIHOST_LCVCIDR_VCID_POSITION,
    DSIHOST_LCVCIDR_VCID_NUM_OF_BITS,
    virtualChannelId);
}

void DSIHost::setLTDCColorCoding(LTDCColorCoding ltdcColorCoding)
{
  constexpr uint32_t DSIHOST_LCOLCR_LPE_POSITION     = 8u;
  constexpr uint32_t DSIHOST_LCOLCR_COLC_POSITION    = 0u;
  constexpr uint32_t DSIHOST_LCOLCR_COLC_NUM_OF_BITS = 4u;

  uint32_t registerValueLCOLCR = 0u;

  registerValueLCOLCR = MemoryUtility<uint32_t>::setBits(
    registerValueLCOLCR,
    DSIHOST_LCOLCR_COLC_POSITION,
    DSIHOST_LCOLCR_COLC_NUM_OF_BITS,
    static_cast<uint32_t>(ltdcColorCoding));

  if ((LTDCColorCoding::RGB666_IN_BLOCK     == ltdcColorCoding) ||
      (LTDCColorCoding::RGB666_BYTE_ALIGNED == ltdcColorCoding))
  {
    registerValueLCOLCR = MemoryUtility<uint32_t>::setBit(registerValueLCOLCR, DSIHOST_LCOLCR_LPE_POSITION);
  }

  MemoryAccess::setRegisterValue(&(m_DSIHostPeripheralPtr->LCOLCR), registerValueLCOLCR);
}

void DSIHost::setColorMultiplexing(uint32_t &registerValueWCFGR, LTDCColorCoding ltdcColorCoding)
{
  constexpr uint32_t DSIHOST_WCFGR_COLMUX_POSITION    = 1u;
  constexpr uint32_t DSIHOST_WCFGR_COLMUX_NUM_OF_BITS = 3u;

  registerValueWCFGR = MemoryUtility<uint32_t>::setBits(
    registerValueWCFGR,
    DSIHOST_WCFGR_COLMUX_POSITION,
    DSIHOST_WCFGR_COLMUX_NUM_OF_BITS,
    static_cast<uint32_t>(ltdcColorCoding));
}

void DSIHost::setTearingEffectSource(uint32_t &registerValueWCFGR, TearingEffectSource tearingEffectSource)
{
  constexpr uint32_t DSIHOST_WCFGR_TESRC_POSITION    = 4u;
  constexpr uint32_t DSIHOST_WCFGR_TESRC_NUM_OF_BITS = 1u;

  registerValueWCFGR = MemoryUtility<uint32_t>::setBits(
    registerValueWCFGR,
    DSIHOST_WCFGR_TESRC_POSITION,
    DSIHOST_WCFGR_TESRC_NUM_OF_BITS,
    static_cast<uint32_t>(tearingEffectSource));
}

void DSIHost::enableAutoRefreshMode(uint32_t &registerValueWCFGR)
{
  constexpr uint32_t DSIHOST_WCFGR_AR_POSITION = 6u;
  registerValueWCFGR = MemoryUtility<uint32_t>::setBit(registerValueWCFGR, DSIHOST_WCFGR_AR_POSITION);
}

void DSIHost::disableAutoRefreshMode(uint32_t &registerValueWCFGR)
{
  constexpr uint32_t DSIHOST_WCFGR_AR_POSITION = 6u;
  registerValueWCFGR = MemoryUtility<uint32_t>::resetBit(registerValueWCFGR, DSIHOST_WCFGR_AR_POSITION);
}

void DSIHost::setVSyncLTDCHaltPolarity(uint32_t &registerValueWCFGR, VSyncLTDCHaltPolarity vsyncLtdcHaltPolarity)
{
  constexpr uint32_t DSIHOST_WCFGR_VSPOL_POSITION    = 7u;
  constexpr uint32_t DSIHOST_WCFGR_VSPOL_NUM_OF_BITS = 1u;

  registerValueWCFGR = MemoryUtility<uint32_t>::setBits(
    registerValueWCFGR,
    DSIHOST_WCFGR_VSPOL_POSITION,
    DSIHOST_WCFGR_VSPOL_NUM_OF_BITS,
    static_cast<uint32_t>(vsyncLtdcHaltPolarity));
}

void DSIHost::setTearingEffectPolarity(uint32_t &registerValueWCFGR, TearingEffectPolarity tearingEffectPolarity)
{
  constexpr uint32_t DSIHOST_WCFGR_TE_POSITION = 5u;
  constexpr uint32_t DSIHOST_WCFGR_TE_NUM_OF_BITS = 1u;

  registerValueWCFGR = MemoryUtility<uint32_t>::setBits(
    registerValueWCFGR,
    DSIHOST_WCFGR_TE_POSITION,
    DSIHOST_WCFGR_TE_NUM_OF_BITS,
    static_cast<uint32_t>(tearingEffectPolarity));
}

void DSIHost::setHSyncPolarity(uint32_t &registerValueLPCR, SignalPolarity hsyncPolarity)
{
  constexpr uint32_t DSIHOST_LPCR_HSP_POSITION    = 2u;
  constexpr uint32_t DSIHOST_LPCR_HSP_NUM_OF_BITS = 1u;

  registerValueLPCR = MemoryUtility<uint32_t>::setBits(
    registerValueLPCR,
    DSIHOST_LPCR_HSP_POSITION,
    DSIHOST_LPCR_HSP_NUM_OF_BITS,
    static_cast<uint32_t>(hsyncPolarity));
}

void DSIHost::setVSyncPolarity(uint32_t &registerValueLPCR, SignalPolarity vsyncPolarity)
{
  constexpr uint32_t DSIHOST_LPCR_VSP_POSITION    = 1u;
  constexpr uint32_t DSIHOST_LPCR_VSP_NUM_OF_BITS = 1u;

  registerValueLPCR = MemoryUtility<uint32_t>::setBits(
    registerValueLPCR,
    DSIHOST_LPCR_VSP_POSITION,
    DSIHOST_LPCR_VSP_NUM_OF_BITS,
    static_cast<uint32_t>(vsyncPolarity));
}

void DSIHost::setDataEnablePolarity(uint32_t &registerValueLPCR, SignalPolarity dataEnablePolarity)
{
  constexpr uint32_t DSIHOST_LPCR_DEP_POSITION    = 0u;
  constexpr uint32_t DSIHOST_LPCR_DEP_NUM_OF_BITS = 1u;

  registerValueLPCR = MemoryUtility<uint32_t>::setBits(
    registerValueLPCR,
    DSIHOST_LPCR_DEP_POSITION,
    DSIHOST_LPCR_DEP_NUM_OF_BITS,
    static_cast<uint32_t>(dataEnablePolarity));
}

void DSIHost::setMaximumLTDCWriteMemoryCommandSize(uint16_t maximumLTDCWriteMemoryCommandSize)
{
  constexpr uint32_t DSIHOST_LCCR_CMDSIZE_POSITION = 0u;
  constexpr uint32_t DSIHOST_LCCR_CMDSIZE_SIZE     = 16u;

  RegisterUtility<uint32_t>::setBitsInRegister(
    &(m_DSIHostPeripheralPtr->LCCR),
    DSIHOST_LCCR_CMDSIZE_POSITION,
    DSIHOST_LCCR_CMDSIZE_SIZE,
    maximumLTDCWriteMemoryCommandSize);
}

void DSIHost::setDSIModeToAdaptedCommandMode(void)
{
  constexpr uint32_t DSIHOST_WCFGR_DSIM_POSITION = 0u;
  RegisterUtility<uint32_t>::setBitInRegister(&(m_DSIHostPeripheralPtr->WCFGR), DSIHOST_WCFGR_DSIM_POSITION);
}

void DSIHost::setCommandModeToAdaptedCommandMode(void)
{
  constexpr uint32_t DSIHOST_MCR_CMDM_POSITION = 0u;
  RegisterUtility<uint32_t>::setBitInRegister(&(m_DSIHostPeripheralPtr->MCR), DSIHOST_MCR_CMDM_POSITION);
}

void DSIHost::enableDSIHost(void)
{
  constexpr uint32_t DSIHOST_CR_EN_POSITION = 0u;
  RegisterUtility<uint32_t>::setBitInRegister(&(m_DSIHostPeripheralPtr->CR), DSIHOST_CR_EN_POSITION);
}

void DSIHost::enableDSIWrapper(void)
{
  constexpr uint32_t DSIHOST_WCR_DSIEN_POSITION = 3u;
  RegisterUtility<uint32_t>::setBitInRegister(&(m_DSIHostPeripheralPtr->WCR), DSIHOST_WCR_DSIEN_POSITION);
}

void DSIHost::disableAllDSIHostInterrupts(void)
{
  constexpr uint32_t ALL_INTERRUPTS_DISABLED = 0x00000000u;
  MemoryAccess::setRegisterValue(&(m_DSIHostPeripheralPtr->IER[0]), ALL_INTERRUPTS_DISABLED);
  MemoryAccess::setRegisterValue(&(m_DSIHostPeripheralPtr->IER[1]), ALL_INTERRUPTS_DISABLED);
}

void DSIHost::setPacketDataType(uint32_t &registerValueGHCR, uint8_t packetDataType)
{
  constexpr uint32_t DSIHOST_GHCR_DT_POSITION    = 0u;
  constexpr uint32_t DSIHOST_GHCR_DT_NUM_OF_BITS = 6u;

  registerValueGHCR = MemoryUtility<uint32_t>::setBits(
    registerValueGHCR,
    DSIHOST_GHCR_DT_POSITION,
    DSIHOST_GHCR_DT_NUM_OF_BITS,
    packetDataType);
}

void DSIHost::setVirtualChannelID(uint32_t &registerValueGHCR, VirtualChannelID virtualChannelId)
{
  constexpr uint32_t DSIHOST_GHCR_VCID_POSITION    = 6u;
  constexpr uint32_t DSIHOST_GHCR_VCID_NUM_OF_BITS = 2u;

  registerValueGHCR = MemoryUtility<uint32_t>::setBits(
    registerValueGHCR,
    DSIHOST_GHCR_VCID_POSITION,
    DSIHOST_GHCR_VCID_NUM_OF_BITS,
    static_cast<uint8_t>(virtualChannelId));
}

void DSIHost::setShortPacketData0(uint32_t &registerValueGHCR, uint8_t data)
{
  constexpr uint32_t DSIHOST_GHCR_WCLSB_POSITION    = 8u;
  constexpr uint32_t DSIHOST_GHCR_WCLSB_NUM_OF_BITS = 8u;

  registerValueGHCR = MemoryUtility<uint32_t>::setBits(
    registerValueGHCR,
    DSIHOST_GHCR_WCLSB_POSITION,
    DSIHOST_GHCR_WCLSB_NUM_OF_BITS,
    data);
}

void DSIHost::setShortPacketData1(uint32_t &registerValueGHCR, uint8_t data)
{
  constexpr uint32_t DSIHOST_GHCR_WCMSB_POSITION    = 16u;
  constexpr uint32_t DSIHOST_GHCR_WCMSB_NUM_OF_BITS = 8u;

  registerValueGHCR = MemoryUtility<uint32_t>::setBits(
    registerValueGHCR,
    DSIHOST_GHCR_WCMSB_POSITION,
    DSIHOST_GHCR_WCMSB_NUM_OF_BITS,
    data);
}

void DSIHost::setLongPacketDataSize(uint32_t &registerValueGHCR, uint16_t dataSize)
{
  constexpr uint32_t DSIHOST_GHCR_WCXSB_POSITION    = 8u;
  constexpr uint32_t DSIHOST_GHCR_WCXSB_NUM_OF_BITS = 16u;

  registerValueGHCR = MemoryUtility<uint32_t>::setBits(
    registerValueGHCR,
    DSIHOST_GHCR_WCXSB_POSITION,
    DSIHOST_GHCR_WCXSB_NUM_OF_BITS,
    dataSize);
}

bool DSIHost::isCommandFIFOEmpty(void) const
{
  constexpr uint32_t DSIHOST_GPSR_CMDFE_POSITION = 0u;
  return RegisterUtility<uint32_t>::isBitSetInRegister(&(m_DSIHostPeripheralPtr->GPSR), DSIHOST_GPSR_CMDFE_POSITION);
}

void DSIHost::writeDataToTransmitInFIFO(uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4)
{
  uint32_t registerValueGPDR = 0u;

  registerValueGPDR = MemoryUtility<uint32_t>::setBits(registerValueGPDR, (0u * BITS_IN_BYTE), BITS_IN_BYTE, data1);
  registerValueGPDR = MemoryUtility<uint32_t>::setBits(registerValueGPDR, (1u * BITS_IN_BYTE), BITS_IN_BYTE, data2);
  registerValueGPDR = MemoryUtility<uint32_t>::setBits(registerValueGPDR, (2u * BITS_IN_BYTE), BITS_IN_BYTE, data3);
  registerValueGPDR = MemoryUtility<uint32_t>::setBits(registerValueGPDR, (3u * BITS_IN_BYTE), BITS_IN_BYTE, data4);

  MemoryAccess::setRegisterValue(&(m_DSIHostPeripheralPtr->GPDR), registerValueGPDR);
}

void DSIHost::writeDataToTransmitInFIFO(const uint8_t *dataPtr, uint16_t dataSize)
{
  const uint16_t alignedBytesCount    = (dataSize / sizeof(uint32_t)) * sizeof(uint32_t);
  const uint16_t notAlignedBytesCount = dataSize % sizeof(uint32_t);

  for (uint16_t i = 0u; i < alignedBytesCount; i += 4u)
  {
    writeDataToTransmitInFIFO(dataPtr[i], dataPtr[i + 1], dataPtr[i + 2], dataPtr[i + 3]);
  }

  if (0u != notAlignedBytesCount)
  {
    uint8_t remainOfData[sizeof(uint32_t)] = {0u, 0u, 0u, 0u};

    for (uint16_t i = 0u; i < notAlignedBytesCount; ++i)
    {
      remainOfData[i] = dataPtr[alignedBytesCount + i];
    }

    writeDataToTransmitInFIFO(remainOfData[0], remainOfData[1], remainOfData[2], remainOfData[3]);
  }
}

void DSIHost::writeDCSCommandAndDataToTransmitInFIFO(uint8_t dcsCommand, const uint8_t *dataPtr, uint16_t dataSize)
{
  const uint16_t alignedBytesCount    = ((dataSize + sizeof(uint8_t)) / sizeof(uint32_t)) * sizeof(uint32_t);
  const uint16_t notAlignedBytesCount = (dataSize + sizeof(uint8_t)) % sizeof(uint32_t);

  if (0u != alignedBytesCount)
  {
    writeDataToTransmitInFIFO(dcsCommand, dataPtr[0], dataPtr[1], dataPtr[2]);
  }

  for (uint16_t i = sizeof(uint32_t); i < alignedBytesCount; i += 4u)
  {
    writeDataToTransmitInFIFO(dataPtr[i - 1], dataPtr[i], dataPtr[i + 1], dataPtr[i + 2]);
  }

  if (0u != notAlignedBytesCount)
  {
    uint8_t remainOfData[sizeof(uint32_t)] = {0u, 0u, 0u, 0u};

    if (0u == alignedBytesCount)
    {
      remainOfData[0] = dcsCommand;
    }

    for (uint16_t i = (0u == alignedBytesCount) ? 1u : 0u; i < notAlignedBytesCount; ++i)
    {
      remainOfData[i] = dataPtr[alignedBytesCount + i - 1];
    }

    writeDataToTransmitInFIFO(remainOfData[0], remainOfData[1], remainOfData[2], remainOfData[3]);
  }
}