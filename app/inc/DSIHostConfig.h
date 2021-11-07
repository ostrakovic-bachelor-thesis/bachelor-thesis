#ifndef DSI_HOST_CONFIG_H
#define DSI_HOST_CONFIG_H

#include "DSIHost.h"


DSIHost::DSIHostConfig g_dsiHostConfig =
{
  .pllConfig =
  {
    .inputClockDivider    = 4u,
    .inputClockMultiplier = 125u,
    .outputClockDivider   = 1u,
  },
  .numberOfDataLanes = 1u,
  .dsiPhyTiming =
  {
    .clockLaneHighSpeedToLowPowerTime = 530u, // ns
    .clockLaneLowPowerToHighSpeedTime = 530u, // ns
    .dataLaneHighSpeedToLowPowerTime  = 180u, // ns
    .dataLaneLowPowerToHighSpeedTime  = 340u, // ns
    .stopWaitTime                     = 115u, // ns
  },
  .flowControlConfig =
  {
    .enableCRCReception     = false,
    .enableECCReception     = false,
    .enableBusTurnAround    = true,
    .enableEoTpReception    = false,
    .enableEoTpTransmission = false,
  },
  .dbiInterfaceConfig =
  {
    .transmissionTypeMaximumReadPacketSize      = DSIHost::TransmissionType::HIGH_SPEED,
    .transmissionTypeDCSLongWrite               = DSIHost::TransmissionType::HIGH_SPEED,
    .transmissionTypeDCSShortReadZeroParam      = DSIHost::TransmissionType::LOW_POWER,
    .transmissionTypeDCSShortReadOneParam       = DSIHost::TransmissionType::LOW_POWER,
    .transmissionTypeDCSShortWriteZeroParam     = DSIHost::TransmissionType::LOW_POWER,
    .transmissionTypeGenericLongWrite           = DSIHost::TransmissionType::HIGH_SPEED,
    .transmissionTypeGenericShortReadTwoParam   = DSIHost::TransmissionType::LOW_POWER,
    .transmissionTypeGenericShortReadOneParam   = DSIHost::TransmissionType::LOW_POWER,
    .transmissionTypeGenericShortReadZeroParam  = DSIHost::TransmissionType::LOW_POWER,
    .transmissionTypeGenericShortWriteTwoParam  = DSIHost::TransmissionType::LOW_POWER,
    .transmissionTypeGenericShortWriteOneParam  = DSIHost::TransmissionType::LOW_POWER,
    .transmissionTypeGenericShortWriteZeroParam = DSIHost::TransmissionType::LOW_POWER,
    .enableAcknowledgeRequest                   = false,
    .enableTearingEffectAcknowledgeRequest      = true,
  },

  .virtualChannelId      = 0u,
  .ltdcColorCoding       = DSIHost::LTDCColorCoding::RGB888,
  .vsyncLtdcHaltPolarity = DSIHost::VSyncLTDCHaltPolarity::FALLING_EDGE,
  .hsyncPolarity         = DSIHost::SignalPolarity::ACTIVE_LOW,
  .vsyncPolarity         = DSIHost::SignalPolarity::ACTIVE_LOW,
  .dataEnablePolarity    = DSIHost::SignalPolarity::ACTIVE_HIGH,

  .maxLTDCWriteMemoryCmdSize = 390u,
  .tearingEffectSource       = DSIHost::TearingEffectSource::DSI_LINK,
  .tearingEffectPolarity     = DSIHost::TearingEffectPolarity::FALLING_EDGE,
  .enableAutoRefreshMode     = true,
};

#endif // #ifndef DSI_HOST_CONFIG_H