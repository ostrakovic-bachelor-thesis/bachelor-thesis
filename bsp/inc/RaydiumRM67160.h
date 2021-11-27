#ifndef RAYDIUM_RM67160_H
#define RAYDIUM_RM67160_H

#include "DSIHost.h"
#include "SysTick.h"


class RaydiumRM67160
{
public:

  typedef void (*CallbackFunc)(void);

  RaydiumRM67160(DSIHost *dsiHostPtr, SysTick *sysTickPtr);

  //! This enum class represents errors which can happen during method calls
  enum class ErrorCode : uint8_t
  {
    OK = 0u,
  };

  struct RaydiumRM67160Config
  {
    CallbackFunc enableDSI3V3Callback;
    CallbackFunc enableDSI1V8Callback;
    CallbackFunc setDSIResetLineToLowCallback;
    CallbackFunc setDSIResetLineToHighCallback;
    uint16_t startColumnAddress;
    uint16_t endColumnAddress;
    uint16_t startRowAddress;
    uint16_t endRowAddress;
    uint8_t defaultBrightness;
  };

  ErrorCode init(const RaydiumRM67160Config &raydiumRM67160Config);

  ErrorCode setDisplayBrightness(uint8_t brightness);

private:

  static constexpr uint32_t BITS_IN_BYTE = 8u;

  enum class CommandSet : uint8_t
  {
    USER_CMD_SET               = 0b000,
    MANUFACTURE_CMD_SET_PAGE_0 = 0b001,
    MANUFACTURE_CMD_SET_PAGE_1 = 0b010,
    MANUFACTURE_CMD_SET_PAGE_2 = 0b011,
    MANUFACTURE_CMD_SET_PAGE_3 = 0b100,
    MANUFACTURE_CMD_SET_PAGE_4 = 0b101
  };

  enum class UserCmdSetCommands : uint8_t
  {
    EXIT_SLEEP_MODE              = 0x11,
    START_DISPLAYING             = 0x29,
    SET_COLUMN_START_END_ADDRESS = 0x2A,
    SET_ROW_START_END_ADDRESS    = 0x2B,
    DISABLE_TEARING_EFFECT_LINE  = 0x34,
    SET_INTERFACE_COLOR_FORMAT   = 0x3A,
    SET_DISPLAY_BRIGHTNESS       = 0x51,
    SET_DSI_MODE                 = 0xC2,
  };

  enum class DSIInterfaceColorFormat : uint8_t
  {
    RGB565 = 0b101,
    RGB666 = 0b110,
    RGB888 = 0b111
  };

  enum class DSIMode : uint8_t
  {
    INTERNAL_TIMING        = 0b00,
    VSYNC_ALIGN_MODE       = 0b10,
    VSYNC_HSYNC_ALIGN_MODE = 0b11
  };

  static const uint8_t s_dcsCommandsManufCmdSetPage0[][2];
  static const uint8_t s_dcsCommandsManufCmdSetPage1[][2];
  static const uint8_t s_dcsCommandsManufCmdSetPage2[][2];
  static const uint8_t s_dcsCommandsManufCmdSetPage3[][2];
  static const uint8_t s_dcsCommandsManufCmdSetPage4[][2];

  void powerOnDisplay(const RaydiumRM67160Config &raydiumRM67160Config);

  void setCommandSet(DSIHost::VirtualChannelID virtualChannelId, CommandSet commandSet);

  template<uint32_t N>
  void sendDCSShortWriteCommands(DSIHost::VirtualChannelID virtualChannelId, const uint8_t (&dcsCommands)[N][2]);

  void waitMs(uint64_t periodToWaitInMs);

  void setDsiInterfaceColorFormat(
    DSIHost::VirtualChannelID virtualChannelId,
    DSIInterfaceColorFormat dsiInterfaceColorFormat);
  void disableTearingEffectLine(DSIHost::VirtualChannelID virtualChannelId);
  void setDSIMode(DSIHost::VirtualChannelID virtualChannelId, DSIMode dsiMode);
  void setStartAndEndColumnAddress(
    DSIHost::VirtualChannelID virtualChannelId,
    uint16_t startColumnAddress,
    uint16_t endColumnAddress);
  void setStartAndEndRowAddress(
    DSIHost::VirtualChannelID virtualChannelId,
    uint16_t startRowAddress,
    uint16_t endRowAddress);
  void exitSleepMode(DSIHost::VirtualChannelID virtualChannelId);
  void setBrightness(DSIHost::VirtualChannelID virtualChannelId, uint8_t brightness);
  void turnOnDisplaying(DSIHost::VirtualChannelID virtualChannelId);

  //! Pointer to DSIHost
  DSIHost *m_dsiHostPtr;

  //! Pointer to SysTick
  SysTick *m_sysTickPtr;
};

#endif // #ifndef RAYDIUM_RM67160_H