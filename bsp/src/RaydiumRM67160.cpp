#include "RaydiumRM67160.h"

const uint8_t RaydiumRM67160::s_dcsCommandsManufCmdSetPage0[][2] =
{ { 0x06, 0x62 }, { 0x0E, 0x80 }, { 0x0F, 0x80 }, { 0x10, 0x71 }, { 0x13, 0x81 }, { 0x14, 0x81 },
	{ 0x15, 0x82 }, { 0x16, 0x82 }, { 0x18, 0x88 }, { 0x19, 0x55 }, { 0x1A, 0x10 }, { 0x1C, 0x99 },
	{ 0x1D, 0x03 }, { 0x1E, 0x03 }, { 0x1F, 0x03 }, { 0x20, 0x03 }, { 0x25, 0x03 }, { 0x26, 0x8D },
	{ 0x2A, 0x03 }, { 0x2B, 0x8D }, { 0x36, 0x00 }, { 0x37, 0x10 }, { 0x3A, 0x00 }, { 0x3B, 0x00 },
  { 0x3D, 0x20 }, { 0x3F, 0x3A }, { 0x40, 0x30 }, { 0x41, 0x1A }, { 0x42, 0x33 }, { 0x43, 0x22 },
	{ 0x44, 0x11 }, { 0x45, 0x66 }, { 0x46, 0x55 }, { 0x47, 0x44 }, { 0x4C, 0x33 }, { 0x4D, 0x22 },
	{ 0x4E, 0x11 }, { 0x4F, 0x66 }, { 0x50, 0x55 }, { 0x51, 0x44 }, { 0x57, 0x33 }, { 0x6B, 0x1B },
  { 0x70, 0x55 }, { 0x74, 0x0C }
};

const uint8_t RaydiumRM67160::s_dcsCommandsManufCmdSetPage1[][2] =
{ { 0x9B, 0x40 }, { 0x9C, 0x00 }, { 0x9D, 0x20 } };

const uint8_t RaydiumRM67160::s_dcsCommandsManufCmdSetPage2[][2] =
{ { 0x9B, 0x40 }, { 0x9C, 0x00 }, { 0x9D, 0x20 } };

const uint8_t RaydiumRM67160::s_dcsCommandsManufCmdSetPage3[][2] =
{ { 0x5D, 0x10 }, { 0x00, 0x8D }, { 0x01, 0x00 }, { 0x02, 0x01 }, { 0x03, 0x01 }, { 0x04, 0x10 },
	{ 0x05, 0x01 }, { 0x06, 0xA7 }, { 0x07, 0x20 }, { 0x08, 0x00 }, { 0x09, 0xC2 }, { 0x0A, 0x00 },
	{ 0x0B, 0x02 }, { 0x0C, 0x01 }, { 0x0D, 0x40 }, { 0x0E, 0x06 }, { 0x0F, 0x01 }, { 0x10, 0xA7 },
	{ 0x11, 0x00 }, { 0x12, 0xC2 }, { 0x13, 0x00 }, { 0x14, 0x02 }, { 0x15, 0x01 }, { 0x16, 0x40 },
	{ 0x17, 0x07 }, { 0x18, 0x01 }, { 0x19, 0xA7 }, { 0x1A, 0x00 }, { 0x1B, 0x82 }, { 0x1C, 0x00 },
	{ 0x1D, 0xFF }, { 0x1E, 0x05 }, { 0x1F, 0x60 }, { 0x20, 0x02 }, { 0x21, 0x01 }, { 0x22, 0x7C },
	{ 0x23, 0x00 }, { 0x24, 0xC2 }, { 0x25, 0x00 }, { 0x26, 0x04 }, { 0x27, 0x02 }, { 0x28, 0x70 },
	{ 0x29, 0x05 }, { 0x2A, 0x74 }, { 0x2B, 0x8D }, { 0x2D, 0x00 }, { 0x2F, 0xC2 }, { 0x30, 0x00 },
	{ 0x31, 0x04 }, { 0x32, 0x02 }, { 0x33, 0x70 }, { 0x34, 0x07 }, { 0x35, 0x74 }, { 0x36, 0x8D },
	{ 0x37, 0x00 }, { 0x5E, 0x20 }, { 0x5F, 0x31 }, { 0x60, 0x54 }, { 0x61, 0x76 }, { 0x62, 0x98 }
};

const uint8_t RaydiumRM67160::s_dcsCommandsManufCmdSetPage4[][2] =
{ { 0x05, 0x17 }, { 0x2A, 0x04 }, { 0x91, 0x00 } };


RaydiumRM67160::RaydiumRM67160(DSIHost *dsiHostPtr, SysTick *sysTickPtr):
  m_dsiHostPtr(dsiHostPtr),
  m_sysTickPtr(sysTickPtr)
{}

RaydiumRM67160::ErrorCode RaydiumRM67160::init(const RaydiumRM67160Config &raydiumRM67160Config)
{
  constexpr uint64_t WAIT_AFTER_EXITING_SLEEP_MODE_PERIOD_MS = 120u;

  powerOnDisplay(raydiumRM67160Config);

  setCommandSet(DSIHost::VirtualChannelID::CHANNEL_0, CommandSet::MANUFACTURE_CMD_SET_PAGE_0);
  sendDCSShortWriteCommands(DSIHost::VirtualChannelID::CHANNEL_0, s_dcsCommandsManufCmdSetPage0);

  setCommandSet(DSIHost::VirtualChannelID::CHANNEL_0, CommandSet::MANUFACTURE_CMD_SET_PAGE_1);
  sendDCSShortWriteCommands(DSIHost::VirtualChannelID::CHANNEL_0, s_dcsCommandsManufCmdSetPage1);

  setCommandSet(DSIHost::VirtualChannelID::CHANNEL_0, CommandSet::MANUFACTURE_CMD_SET_PAGE_2);
  sendDCSShortWriteCommands(DSIHost::VirtualChannelID::CHANNEL_0, s_dcsCommandsManufCmdSetPage2);

  setCommandSet(DSIHost::VirtualChannelID::CHANNEL_0, CommandSet::MANUFACTURE_CMD_SET_PAGE_3);
  sendDCSShortWriteCommands(DSIHost::VirtualChannelID::CHANNEL_0, s_dcsCommandsManufCmdSetPage3);

  setCommandSet(DSIHost::VirtualChannelID::CHANNEL_0, CommandSet::MANUFACTURE_CMD_SET_PAGE_4);
  sendDCSShortWriteCommands(DSIHost::VirtualChannelID::CHANNEL_0, s_dcsCommandsManufCmdSetPage4);

  setCommandSet(DSIHost::VirtualChannelID::CHANNEL_0, CommandSet::USER_CMD_SET);
  setDsiInterfaceColorFormat(DSIHost::VirtualChannelID::CHANNEL_0, DSIInterfaceColorFormat::RGB888);
  disableTearingEffectLine(DSIHost::VirtualChannelID::CHANNEL_0);
  setDSIMode(DSIHost::VirtualChannelID::CHANNEL_0, DSIMode::INTERNAL_TIMING);
  setStartAndEndColumnAddress(
    DSIHost::VirtualChannelID::CHANNEL_0,
    raydiumRM67160Config.startColumnAddress,
    raydiumRM67160Config.endColumnAddress);
  setStartAndEndRowAddress(
    DSIHost::VirtualChannelID::CHANNEL_0,
    raydiumRM67160Config.startRowAddress,
    raydiumRM67160Config.endRowAddress);
  exitSleepMode(DSIHost::VirtualChannelID::CHANNEL_0);

  waitMs(WAIT_AFTER_EXITING_SLEEP_MODE_PERIOD_MS);

  setBrightness(DSIHost::VirtualChannelID::CHANNEL_0, raydiumRM67160Config.defaultBrightness);
  turnOnDisplaying(DSIHost::VirtualChannelID::CHANNEL_0);

  return ErrorCode::OK;
}

RaydiumRM67160::ErrorCode RaydiumRM67160::setDisplayBrightness(uint8_t brightness)
{
  setCommandSet(DSIHost::VirtualChannelID::CHANNEL_0, CommandSet::USER_CMD_SET);
  setBrightness(DSIHost::VirtualChannelID::CHANNEL_0, brightness);

  return ErrorCode::OK;
}

void RaydiumRM67160::powerOnDisplay(const RaydiumRM67160Config &raydiumRM67160Config)
{
  constexpr uint64_t WAIT_BEFORE_ENABLE_DSI_3V3_PERIOD_MS        = 5u;
  constexpr uint64_t WAIT_BEFORE_ENABLE_DSI_1V8_PERIOD_MS        = 1u;
  constexpr uint64_t WAIT_BEFORE_SET_DSI_RESET_TO_HIGH_PERIOD_MS = 15u;
  constexpr uint64_t WAIT_FOR_RESET_TO_COMPLETE_PERIOD_MS        = 120u;

  raydiumRM67160Config.setDSIResetLineToLowCallback();

  waitMs(WAIT_BEFORE_ENABLE_DSI_3V3_PERIOD_MS);

  raydiumRM67160Config.enableDSI3V3Callback();

  waitMs(WAIT_BEFORE_ENABLE_DSI_1V8_PERIOD_MS);

  raydiumRM67160Config.enableDSI1V8Callback();

  waitMs(WAIT_BEFORE_SET_DSI_RESET_TO_HIGH_PERIOD_MS);

  raydiumRM67160Config.setDSIResetLineToHighCallback();

  waitMs(WAIT_FOR_RESET_TO_COMPLETE_PERIOD_MS);
}

void RaydiumRM67160::setCommandSet(DSIHost::VirtualChannelID virtualChannelId, CommandSet commandSet)
{
  constexpr uint8_t COMMAND_SET_COMMAND_SET = 0xFE;
  m_dsiHostPtr->dcsShortWrite(virtualChannelId, COMMAND_SET_COMMAND_SET, static_cast<uint8_t>(commandSet));
}

template<uint32_t N>
void RaydiumRM67160::sendDCSShortWriteCommands(
  DSIHost::VirtualChannelID virtualChannelId,
  const uint8_t (&dcsCommands)[N][2])
{
  for (uint32_t i = 0u; i < N; ++i)
  {
     m_dsiHostPtr->dcsShortWrite(virtualChannelId, dcsCommands[i][0], dcsCommands[i][1]);
  }
}

void RaydiumRM67160::waitMs(uint64_t periodToWaitInMs)
{
  const uint32_t timestamp = m_sysTickPtr->getTicks();

  while (m_sysTickPtr->getElapsedTimeInMs(timestamp) < periodToWaitInMs);
}

void RaydiumRM67160::setDsiInterfaceColorFormat(
  DSIHost::VirtualChannelID virtualChannelId,
  DSIInterfaceColorFormat dsiInterfaceColorFormat)
{
  m_dsiHostPtr->dcsShortWrite(
    virtualChannelId,
    static_cast<uint8_t>(UserCmdSetCommands::SET_INTERFACE_COLOR_FORMAT),
    static_cast<uint8_t>(dsiInterfaceColorFormat));
}

void RaydiumRM67160::disableTearingEffectLine(DSIHost::VirtualChannelID virtualChannelId)
{
  m_dsiHostPtr->dcsShortWrite(virtualChannelId, static_cast<uint8_t>(UserCmdSetCommands::DISABLE_TEARING_EFFECT_LINE));
}

void RaydiumRM67160::setDSIMode(DSIHost::VirtualChannelID virtualChannelId, DSIMode dsiMode)
{
  m_dsiHostPtr->dcsShortWrite(
    virtualChannelId,
    static_cast<uint8_t>(UserCmdSetCommands::SET_DSI_MODE),
    static_cast<uint8_t>(dsiMode));
}

void RaydiumRM67160::setStartAndEndColumnAddress(
  DSIHost::VirtualChannelID virtualChannelId,
  uint16_t startColumnAddress,
  uint16_t endColumnAddress)
{
  uint8_t commandData[4];

  commandData[0] = static_cast<uint8_t>(MemoryUtility<uint16_t>::getBits(startColumnAddress, 8u, BITS_IN_BYTE));
  commandData[1] = static_cast<uint8_t>(MemoryUtility<uint16_t>::getBits(startColumnAddress, 0u, BITS_IN_BYTE));
  commandData[2] = static_cast<uint8_t>(MemoryUtility<uint16_t>::getBits(endColumnAddress, 8u, BITS_IN_BYTE));
  commandData[3] = static_cast<uint8_t>(MemoryUtility<uint16_t>::getBits(endColumnAddress, 0u, BITS_IN_BYTE));

  m_dsiHostPtr->dcsLongWrite(
    virtualChannelId,
    static_cast<uint8_t>(UserCmdSetCommands::SET_COLUMN_START_END_ADDRESS),
    commandData,
    sizeof(commandData));
}

void RaydiumRM67160::setStartAndEndRowAddress(
  DSIHost::VirtualChannelID virtualChannelId,
  uint16_t startRowAddress,
  uint16_t endRowAddress)
{
  uint8_t commandData[4];

  commandData[0] = static_cast<uint8_t>(MemoryUtility<uint16_t>::getBits(startRowAddress, 8u, BITS_IN_BYTE));
  commandData[1] = static_cast<uint8_t>(MemoryUtility<uint16_t>::getBits(startRowAddress, 0u, BITS_IN_BYTE));
  commandData[2] = static_cast<uint8_t>(MemoryUtility<uint16_t>::getBits(endRowAddress, 8u, BITS_IN_BYTE));
  commandData[3] = static_cast<uint8_t>(MemoryUtility<uint16_t>::getBits(endRowAddress, 0u, BITS_IN_BYTE));

  m_dsiHostPtr->dcsLongWrite(
    virtualChannelId,
    static_cast<uint8_t>(UserCmdSetCommands::SET_ROW_START_END_ADDRESS),
    commandData,
    sizeof(commandData));
}

void RaydiumRM67160::exitSleepMode(DSIHost::VirtualChannelID virtualChannelId)
{
  m_dsiHostPtr->dcsShortWrite(virtualChannelId, static_cast<uint8_t>(UserCmdSetCommands::EXIT_SLEEP_MODE));
}

void RaydiumRM67160::setBrightness(DSIHost::VirtualChannelID virtualChannelId, uint8_t brightness)
{
  m_dsiHostPtr->dcsShortWrite(virtualChannelId,
    static_cast<uint8_t>(UserCmdSetCommands::SET_DISPLAY_BRIGHTNESS),
    brightness);
}

void RaydiumRM67160::turnOnDisplaying(DSIHost::VirtualChannelID virtualChannelId)
{
  m_dsiHostPtr->dcsShortWrite(virtualChannelId, static_cast<uint8_t>(UserCmdSetCommands::START_DISPLAYING));
}