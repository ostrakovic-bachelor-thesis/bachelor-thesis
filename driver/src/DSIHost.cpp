#include "DSIHost.h"
#include "MemoryAccess.h"
#include "MemoryUtility.h"
#include "RegisterUtility.h"


DSIHost::DSIHost(DSI_TypeDef *DSIHostPeripheralPtr, ResetControl *resetControlPtr):
  m_DSIHostPeripheralPtr(DSIHostPeripheralPtr),
  m_resetControlPtr(resetControlPtr)
{}

DSIHost::ErrorCode DSIHost::init(const DSIHostConfig &dsiHostConfig)
{
  ErrorCode errorCode = enablePeripheralClock();
  if (ErrorCode::OK != errorCode)
  {
    return errorCode;
  }

  enableDPHYRegulator();

  uint32_t registerValueWRPCR = MemoryAccess::getRegisterValue(&(m_DSIHostPeripheralPtr->WRPCR));

  setPLLInputClockDivider(registerValueWRPCR, dsiHostConfig.pllConfig.inputClockDivider);
  setPLLInputClockMultiplier(registerValueWRPCR, dsiHostConfig.pllConfig.inputClockMultiplier);
  setPLLOutputClockDivider(registerValueWRPCR, dsiHostConfig.pllConfig.outputClockDivider);

  MemoryAccess::setRegisterValue(&(m_DSIHostPeripheralPtr->WRPCR), registerValueWRPCR);

  enableDPHYPLL();

  setNumberOfDataLanes(dsiHostConfig.numberOfDataLanes);

  enableDPHYClockLaneModuleAndDigitalSection();

  disableAutomaticClockLaneControl();

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

void DSIHost::disableAutomaticClockLaneControl(void)
{
  constexpr uint32_t DSIHOST_CLCR_ACR_POSITION = 1u;
  RegisterUtility<uint32_t>::resetBitInRegister(&(m_DSIHostPeripheralPtr->CLCR), DSIHOST_CLCR_ACR_POSITION);
}