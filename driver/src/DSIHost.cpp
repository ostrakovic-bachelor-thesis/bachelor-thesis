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
    uint32_t registerValuePCR = MemoryAccess::getRegisterValue(&(m_DSIHostPeripheralPtr->PCR));

    if (dsiHostConfig.enableCRCReception)
    {
      enableCRCReception(registerValuePCR);
    }
    else
    {
      disableCRCReception(registerValuePCR);
    }

    if (dsiHostConfig.enableECCReception)
    {
      enableECCReception(registerValuePCR);
    }
    else
    {
      disableECCReception(registerValuePCR);
    }

    if (dsiHostConfig.enableBusTurnAround)
    {
      enableBusTurnAround(registerValuePCR);
    }
    else
    {
      disableBusTurnAround(registerValuePCR);
    }

    if (dsiHostConfig.enableEoTpReception)
    {
      enableEoTpReception(registerValuePCR);
    }
    else
    {
      disableEoTpReception(registerValuePCR);
    }

    if (dsiHostConfig.enableEoTpTransmission)
    {
      enableEoTpTransmission(registerValuePCR);
    }
    else
    {
      disableEoTpTransmission(registerValuePCR);
    }

    MemoryAccess::setRegisterValue(&(m_DSIHostPeripheralPtr->PCR), registerValuePCR);
  }

  if (ErrorCode::OK == errorCode)
  {
    uint32_t registerValueCMCR = MemoryAccess::getRegisterValue(&(m_DSIHostPeripheralPtr->CMCR));

    setMaximumReadPacketSizeTransmissionType(registerValueCMCR, dsiHostConfig.transmissionTypeMaximumReadPacketSize);
    setDCSLongWriteTransmissionType(registerValueCMCR, dsiHostConfig.transmissionTypeDCSLongWrite);
    setDCSShortReadZeroParamTransmissionType(registerValueCMCR, dsiHostConfig.transmissionTypeDCSShortReadZeroParam);
    setDCSShortReadOneParamTransmissionType(registerValueCMCR, dsiHostConfig.transmissionTypeDCSShortReadOneParam);

    MemoryAccess::setRegisterValue(&(m_DSIHostPeripheralPtr->CMCR), registerValueCMCR);
  }

  return errorCode;
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