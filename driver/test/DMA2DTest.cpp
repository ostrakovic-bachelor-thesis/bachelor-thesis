#include "DMA2D.h"
#include "MemoryUtility.h"
#include "MemoryAccess.h"
#include "DriverTest.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"


using namespace ::testing;


class ADMA2D : public DriverTest
{
public:

  //! Based on real reset values for DMA2D register (source STM32L4R9 reference manual)
  static constexpr uint32_t DMA2D_CR_RESET_VALUE      = 0x00000000;
  static constexpr uint32_t DMA2D_ISR_RESET_VALUE     = 0x00000000;
  static constexpr uint32_t DMA2D_IFCR_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DMA2D_FGMAR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t DMA2D_FGOR_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DMA2D_BGMAR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t DMA2D_BGOR_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DMA2D_FGPFCCR_RESET_VALUE = 0x00000000;
  static constexpr uint32_t DMA2D_FGCOLR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t DMA2D_BGPFCCR_RESET_VALUE = 0x00000000;
  static constexpr uint32_t DMA2D_BGCOLR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t DMA2D_FGCMAR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t DMA2D_BGCMAR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t DMA2D_OPFCCR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t DMA2D_OCOLR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t DMA2D_OMAR_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DMA2D_OOR_RESET_VALUE     = 0x00000000;
  static constexpr uint32_t DMA2D_NLR_RESET_VALUE     = 0x00000000;
  static constexpr uint32_t DMA2D_LWR_RESET_VALUE     = 0x00000000;
  static constexpr uint32_t DMA2D_AMTCR_RESET_VALUE   = 0x00000000;

  static constexpr uint32_t DMA2D_OCOLR_ARGB4444_BLUE_POSITION  = 0u;
  static constexpr uint32_t DMA2D_OCOLR_ARGB4444_GREEN_POSITION = 4u;
  static constexpr uint32_t DMA2D_OCOLR_ARGB4444_RED_POSITION   = 8u;
  static constexpr uint32_t DMA2D_OCOLR_ARGB4444_ALPHA_POSITION = 12u;
  static constexpr uint32_t DMA2D_OCOLR_ARGB4444_BLUE_SIZE  = 4u;
  static constexpr uint32_t DMA2D_OCOLR_ARGB4444_GREEN_SIZE = 4u;
  static constexpr uint32_t DMA2D_OCOLR_ARGB4444_RED_SIZE   = 4u;
  static constexpr uint32_t DMA2D_OCOLR_ARGB4444_ALPHA_SIZE = 4u;

  static constexpr uint32_t PIXEL_SIZE_RGB565 = 2u;
  static constexpr uint32_t PIXEL_SIZE_BGR565 = 2u;
  static constexpr uint32_t PIXEL_SIZE_RGB888 = 3u;
  static constexpr uint32_t PIXEL_SIZE_ARGB8888 = 4u;

  static constexpr uint16_t DEFAULT_CONFIG_VALUE_OUTPUT_X_POS = 50u;
  static constexpr uint16_t DEFAULT_CONFIG_VALUE_OUTPUT_Y_POS = 50u;
  static constexpr uint16_t DEFAULT_CONFIG_VALUE_OUTPUT_BUFFER_WIDTH = 100u;
  static constexpr uint16_t DEFAULT_CONFIG_VALUE_OUTPUT_BUFFER_HEIGHT = 100u;
  static constexpr uintptr_t DEFAULT_CONFIG_VALUES_OUTPUT_BUFFER_ADDRESS = 0xBEEF0000;
  static constexpr uintptr_t DEFAULT_CONFIG_VALUES_DMA2D_OMAR_OFFSET_VALUE =
    PIXEL_SIZE_ARGB8888 * (DEFAULT_CONFIG_VALUE_OUTPUT_X_POS + DEFAULT_CONFIG_VALUE_OUTPUT_Y_POS * DEFAULT_CONFIG_VALUE_OUTPUT_BUFFER_WIDTH);

  static constexpr uint16_t DEFAULT_CONFIG_VALUE_INPUT_X_POS = 100u;
  static constexpr uint16_t DEFAULT_CONFIG_VALUE_INPUT_Y_POS = 150u;
  static constexpr uint16_t DEFAULT_CONFIG_VALUE_INPUT_BUFFER_WIDTH = 300u;
  static constexpr uint16_t DEFAULT_CONFIG_VALUE_INPUT_BUFFER_HEIGHT = 300u;
  static constexpr uintptr_t DEFAULT_CONFIG_VALUES_INPUT_BUFFER_ADDRESS = 0xAEE00000;
  static constexpr uintptr_t DEFAULT_CONFIG_VALUES_DMA2D_FGMAR_OFFSET_VALUE =
    PIXEL_SIZE_BGR565 * (DEFAULT_CONFIG_VALUE_INPUT_X_POS + DEFAULT_CONFIG_VALUE_INPUT_Y_POS * DEFAULT_CONFIG_VALUE_INPUT_BUFFER_WIDTH);
  static constexpr uintptr_t DEFAULT_CONFIG_VALUES_DMA2D_BGMAR_OFFSET_VALUE =
    PIXEL_SIZE_BGR565 * (DEFAULT_CONFIG_VALUE_INPUT_X_POS + DEFAULT_CONFIG_VALUE_INPUT_Y_POS * DEFAULT_CONFIG_VALUE_INPUT_BUFFER_WIDTH);

  DMA2D::FillRectangleConfig fillRectangleConfig;
  DMA2D::CopyBitmapConfig copyBitmapConfig;
  DMA2D::BlendBitmapConfig blendBitmapConfig;

  DMA2D_TypeDef virtualDMA2DPeripheral;
  DMA2D virtualDMA2D = DMA2D(&virtualDMA2DPeripheral);

  void SetUp() override;
  void TearDown() override;
};

void ADMA2D::SetUp()
{
  DriverTest::SetUp();

  // set values of virtual DMA2D peripheral to reset values
  virtualDMA2DPeripheral.CR      = DMA2D_CR_RESET_VALUE;
  virtualDMA2DPeripheral.ISR     = DMA2D_ISR_RESET_VALUE;
  virtualDMA2DPeripheral.IFCR    = DMA2D_IFCR_RESET_VALUE;
  virtualDMA2DPeripheral.FGMAR   = DMA2D_FGMAR_RESET_VALUE;
  virtualDMA2DPeripheral.FGOR    = DMA2D_FGOR_RESET_VALUE;
  virtualDMA2DPeripheral.BGMAR   = DMA2D_BGMAR_RESET_VALUE;
  virtualDMA2DPeripheral.BGOR    = DMA2D_BGOR_RESET_VALUE;
  virtualDMA2DPeripheral.FGPFCCR = DMA2D_FGPFCCR_RESET_VALUE;
  virtualDMA2DPeripheral.FGCOLR  = DMA2D_FGCOLR_RESET_VALUE;
  virtualDMA2DPeripheral.BGPFCCR = DMA2D_BGPFCCR_RESET_VALUE;
  virtualDMA2DPeripheral.BGCOLR  = DMA2D_BGCOLR_RESET_VALUE;
  virtualDMA2DPeripheral.FGCMAR  = DMA2D_FGCMAR_RESET_VALUE;
  virtualDMA2DPeripheral.BGCMAR  = DMA2D_BGCMAR_RESET_VALUE;
  virtualDMA2DPeripheral.OPFCCR  = DMA2D_OPFCCR_RESET_VALUE;
  virtualDMA2DPeripheral.OCOLR   = DMA2D_OCOLR_RESET_VALUE;
  virtualDMA2DPeripheral.OMAR    = DMA2D_OMAR_RESET_VALUE;
  virtualDMA2DPeripheral.OOR     = DMA2D_OOR_RESET_VALUE;
  virtualDMA2DPeripheral.NLR     = DMA2D_NLR_RESET_VALUE;
  virtualDMA2DPeripheral.LWR     = DMA2D_LWR_RESET_VALUE;
  virtualDMA2DPeripheral.AMTCR   = DMA2D_AMTCR_RESET_VALUE;

  DMA2D::OutputBufferConfiguration defaultOutputBufferConfig =
  {
    .colorFormat = DMA2D::OutputColorFormat::ARGB8888,

    .position =
    {
      .x = DEFAULT_CONFIG_VALUE_OUTPUT_X_POS,
      .y = DEFAULT_CONFIG_VALUE_OUTPUT_Y_POS
    },

    .bufferDimension =
    {
      .width  = DEFAULT_CONFIG_VALUE_OUTPUT_BUFFER_WIDTH,
      .height = DEFAULT_CONFIG_VALUE_OUTPUT_BUFFER_HEIGHT
    },

    .bufferPtr = reinterpret_cast<void*>(DEFAULT_CONFIG_VALUES_OUTPUT_BUFFER_ADDRESS)
  };

  DMA2D::InputBufferConfiguration defaultInputBufferConfig =
  {
    .colorFormat = DMA2D::InputColorFormat::BGR565,

    .position =
    {
      .x = DEFAULT_CONFIG_VALUE_INPUT_X_POS,
      .y = DEFAULT_CONFIG_VALUE_INPUT_Y_POS
    },

    .bufferDimension =
    {
      .width  = DEFAULT_CONFIG_VALUE_INPUT_BUFFER_WIDTH,
      .height = DEFAULT_CONFIG_VALUE_INPUT_BUFFER_HEIGHT
    },

    .bufferPtr = reinterpret_cast<void*>(DEFAULT_CONFIG_VALUES_INPUT_BUFFER_ADDRESS)
  };

  // fill rectange default configuration value
  fillRectangleConfig =
  {
    .color =
    {
      .alpha = 0,
      .red   = 15u,
      .green = 15u,
      .blue  = 15u
    },

    .rectangleDimension =
    {
      .width  = 0u,
      .height = 0u
    },

    .outputBufferConfig = defaultOutputBufferConfig
  };

  // copy bitmap default configuration value
  copyBitmapConfig =
  {
    .copyRectangleDimension =
    {
      .width  = 0u,
      .height = 0u
    },

    .inputBufferConfig = defaultInputBufferConfig,

    .outputBufferConfig = defaultOutputBufferConfig
  };

  // blend bitmap default configuration value
  blendBitmapConfig =
  {
    .blendRectangleDimension =
    {
      .width  = 0u,
      .height = 0u
    },

    .foregroundBufferConfig = defaultInputBufferConfig,

    .backgroundBufferConfig = defaultInputBufferConfig,

    .outputBufferConfig = defaultOutputBufferConfig
  };
}

void ADMA2D::TearDown()
{
  DriverTest::TearDown();
}


TEST_F(ADMA2D, InitSetsThatLineOffsetsAreExpressedInBytes)
{
  constexpr uint32_t DMA2D_CR_LOM_POSITION = 6u;
  constexpr uint32_t EXPECTED_DMA2D_CR_LOM_VALUE = 1u;
  auto bitValueMatcher =
    BitHasValue(DMA2D_CR_LOM_POSITION, EXPECTED_DMA2D_CR_LOM_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.CR), bitValueMatcher);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.init();

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.CR, bitValueMatcher);
}

TEST_F(ADMA2D, InitFailsIfAnDMA2DTransferIsOngoing)
{
  constexpr uint32_t DMA2D_CR_START_POSITION = 0u;
  constexpr uint32_t DMA2D_CR_START_FORCED_VALUE = 1u;
  virtualDMA2DPeripheral.CR =
    expectedRegVal(DMA2D_CR_RESET_VALUE, DMA2D_CR_START_POSITION, 1u, DMA2D_CR_START_FORCED_VALUE);
  expectNoRegisterToChange();

  const DMA2D::ErrorCode errorCode = virtualDMA2D.init();

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::BUSY));
}

TEST_F(ADMA2D, FillRectangleSetsModeToRegisterToMemory)
{
  constexpr uint32_t DMA2D_CR_MODE_POSITION = 16u;
  constexpr uint32_t DMA2D_CR_MODE_SIZE = 3u;
  constexpr uint32_t EXPECTED_DMA2D_CR_MODE_VALUE = 0b011;
  auto bitsValueMatcher =
    BitsHaveValue(DMA2D_CR_MODE_POSITION, DMA2D_CR_MODE_SIZE, EXPECTED_DMA2D_CR_MODE_VALUE);
  expectSpecificRegisterSetToBeCalledFirst(&(virtualDMA2DPeripheral.CR), bitsValueMatcher);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.fillRectangle(fillRectangleConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.CR, bitsValueMatcher);
}

TEST_F(ADMA2D, FillRectangleSetsWantedOutputBufferColorFormatInOPFCCRRegister)
{
  constexpr uint32_t DMA2D_OPFCCR_CM_POSITION = 0u;
  constexpr uint32_t DMA2D_OPFCCR_CM_SIZE = 3u;
  constexpr uint32_t EXPECTED_DMA2D_OPFCCR_CM_VALUE = 0b010;
  fillRectangleConfig.outputBufferConfig.colorFormat = DMA2D::OutputColorFormat::RGB565;
  auto bitsValueMatcher =
    BitsHaveValue(DMA2D_OPFCCR_CM_POSITION, DMA2D_OPFCCR_CM_SIZE, EXPECTED_DMA2D_OPFCCR_CM_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.OPFCCR), bitsValueMatcher);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.fillRectangle(fillRectangleConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.OPFCCR, bitsValueMatcher);
}

TEST_F(ADMA2D, FillRectangleSetsRBSBitInOPFCCRRegisterIfWantedOutputBufferColorFormatHasInversedOrderOfRedAndBlue)
{
  constexpr uint32_t DMA2D_OPFCCR_RBS_POSITION = 21u;
  constexpr uint32_t EXPECTED_DMA2D_OPFCCR_RBS_VALUE = 0x1;
  fillRectangleConfig.outputBufferConfig.colorFormat = DMA2D::OutputColorFormat::ABGR4444;
  auto bitValueMatcher =
    BitHasValue(DMA2D_OPFCCR_RBS_POSITION, EXPECTED_DMA2D_OPFCCR_RBS_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.OPFCCR), bitValueMatcher);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.fillRectangle(fillRectangleConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.OPFCCR, bitValueMatcher);
}

TEST_F(ADMA2D, FillRectangleSetsWantedColorInOCOLRRegisterInAccordanceToChoosenOutputBufferColorFormat)
{
  constexpr uint8_t EXPECTED_DMA2D_OCOLR_BLUE_VALUE = 4u;
  constexpr uint8_t EXPECTED_DMA2D_OCOLR_GREEN_VALUE = 10u;
  constexpr uint8_t EXPECTED_DMA2D_OCOLR_RED_VALUE = 15u;
  constexpr uint8_t EXPECTED_DMA2D_OCOLR_ALPHA_VALUE = 3u;
  fillRectangleConfig.outputBufferConfig.colorFormat = DMA2D::OutputColorFormat::ARGB4444;
  fillRectangleConfig.color =
  {
    .alpha = EXPECTED_DMA2D_OCOLR_ALPHA_VALUE,
    .red   = EXPECTED_DMA2D_OCOLR_RED_VALUE,
    .green = EXPECTED_DMA2D_OCOLR_GREEN_VALUE,
    .blue  = EXPECTED_DMA2D_OCOLR_BLUE_VALUE
  };
  auto bitsValueMatcher =
    AllOf(BitsHaveValue(DMA2D_OCOLR_ARGB4444_ALPHA_POSITION, DMA2D_OCOLR_ARGB4444_ALPHA_SIZE, EXPECTED_DMA2D_OCOLR_ALPHA_VALUE),
          BitsHaveValue(DMA2D_OCOLR_ARGB4444_RED_POSITION, DMA2D_OCOLR_ARGB4444_RED_SIZE, EXPECTED_DMA2D_OCOLR_RED_VALUE),
          BitsHaveValue(DMA2D_OCOLR_ARGB4444_GREEN_POSITION, DMA2D_OCOLR_ARGB4444_GREEN_SIZE, EXPECTED_DMA2D_OCOLR_GREEN_VALUE),
          BitsHaveValue(DMA2D_OCOLR_ARGB4444_BLUE_POSITION, DMA2D_OCOLR_ARGB4444_BLUE_SIZE, EXPECTED_DMA2D_OCOLR_BLUE_VALUE));
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.OCOLR), bitsValueMatcher);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.fillRectangle(fillRectangleConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.OCOLR, bitsValueMatcher);
}

TEST_F(ADMA2D, FillRectangleSetsOMARRegisterValueRelativeToOutputBufferAddress)
{
  constexpr uintptr_t OUTPUT_BUFFER_ADDRESS = 0xAB000000;
  constexpr uint32_t EXPECTED_DMA2D_OMAR_VALUE =
    OUTPUT_BUFFER_ADDRESS + DEFAULT_CONFIG_VALUES_DMA2D_OMAR_OFFSET_VALUE;
  fillRectangleConfig.outputBufferConfig.bufferPtr = reinterpret_cast<void*>(OUTPUT_BUFFER_ADDRESS);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.OMAR), EXPECTED_DMA2D_OMAR_VALUE);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.fillRectangle(fillRectangleConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.OMAR, EXPECTED_DMA2D_OMAR_VALUE);
}

TEST_F(ADMA2D, FillRectangleAddsOffsetToOutputBufferAddressAccordingToPositionAndOutputColorFormatAndOutputBufferWidth)
{
  constexpr uint16_t RECTANGLE_X_POS = 100u;
  constexpr uint16_t RECTANGLE_Y_POS = 50u;
  constexpr uint16_t OUTPUT_BUFFER_WIDTH = 200u;
  constexpr uintptr_t OUTPUT_BUFFER_OFFSET =
    PIXEL_SIZE_RGB888 * (static_cast<uintptr_t>(RECTANGLE_X_POS) +
    static_cast<uintptr_t>(RECTANGLE_Y_POS) * static_cast<uintptr_t>(OUTPUT_BUFFER_WIDTH));
  constexpr uint32_t EXPECTED_DMA2D_OMAR_VALUE =
    DEFAULT_CONFIG_VALUES_OUTPUT_BUFFER_ADDRESS + OUTPUT_BUFFER_OFFSET;
  fillRectangleConfig.outputBufferConfig.colorFormat = DMA2D::OutputColorFormat::RGB888;
  fillRectangleConfig.outputBufferConfig.position =
  {
    .x = RECTANGLE_X_POS,
    .y = RECTANGLE_Y_POS
  };
  fillRectangleConfig.outputBufferConfig.bufferDimension.width = OUTPUT_BUFFER_WIDTH;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.OMAR), EXPECTED_DMA2D_OMAR_VALUE);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.fillRectangle(fillRectangleConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.OMAR, EXPECTED_DMA2D_OMAR_VALUE);
}

TEST_F(ADMA2D, FillRectangleSetsOORRegisterValueAccordingToOutputBufferWidthAndRectangleWidthAndOutputColorFormat)
{
  constexpr uint16_t RECTANGLE_WIDTH = 100u;
  constexpr uint16_t OUTPUT_BUFFER_WIDTH = 200u;
  constexpr uint32_t EXPECTED_DMA2D_OOR_VALUE =
    PIXEL_SIZE_RGB888 * (OUTPUT_BUFFER_WIDTH - RECTANGLE_WIDTH);
  fillRectangleConfig.rectangleDimension.width = RECTANGLE_WIDTH,
  fillRectangleConfig.outputBufferConfig.colorFormat = DMA2D::OutputColorFormat::RGB888;
  fillRectangleConfig.outputBufferConfig.bufferDimension.width = OUTPUT_BUFFER_WIDTH;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.OOR), EXPECTED_DMA2D_OOR_VALUE);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.fillRectangle(fillRectangleConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.OOR, EXPECTED_DMA2D_OOR_VALUE);
}

TEST_F(ADMA2D, FillRectangleSetsNLRRegisterValueAccordingToRectangleDimension)
{
  constexpr uint16_t DMA2D_NLR_PL_POSITION = 16;
  constexpr uint16_t RECTANGLE_WIDTH = 100u;
  constexpr uint16_t RECTANGLE_HEIGHT = 100u;
  constexpr uint32_t EXPECTED_DMA2D_NLR_VALUE =
    (static_cast<uint32_t>(RECTANGLE_WIDTH) << DMA2D_NLR_PL_POSITION) | static_cast<uint32_t>(RECTANGLE_HEIGHT);
  fillRectangleConfig.rectangleDimension =
  {
    .width  = RECTANGLE_WIDTH,
    .height = RECTANGLE_HEIGHT
  };
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.NLR), EXPECTED_DMA2D_NLR_VALUE);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.fillRectangle(fillRectangleConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.NLR, EXPECTED_DMA2D_NLR_VALUE);
}

TEST_F(ADMA2D, FillRectangleEnablesDMA2DTransferCompleteInterrupt)
{
  constexpr uint32_t DMA2D_CR_TCIE_POSITION = 9u;
  constexpr uint32_t EXPECTED_DMA2D_CR_TCIE_VALUE = 1u;
  auto bitValueMatcher =
    BitHasValue(DMA2D_CR_TCIE_POSITION, EXPECTED_DMA2D_CR_TCIE_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.CR), bitValueMatcher);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.fillRectangle(fillRectangleConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.CR, bitValueMatcher);
}

TEST_F(ADMA2D, FillRectangleStartsDMA2DAtTheEndOfFillRectangleFunctionCall)
{
  constexpr uint32_t DMA2D_CR_START_POSITION = 0u;
  constexpr uint32_t EXPECTED_DMA2D_CR_START_VALUE = 1u;
  auto bitValueMatcher =
    BitHasValue(DMA2D_CR_START_POSITION, EXPECTED_DMA2D_CR_START_VALUE);
  expectSpecificRegisterSetToBeCalledLast(&(virtualDMA2DPeripheral.CR), bitValueMatcher);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.fillRectangle(fillRectangleConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.CR, bitValueMatcher);
}

TEST_F(ADMA2D, FillRectangleFailsIfAnotherDMA2DTransferIsOngoing)
{
  ASSERT_THAT(virtualDMA2D.fillRectangle(fillRectangleConfig), Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2D.fillRectangle(fillRectangleConfig), Eq(DMA2D::ErrorCode::BUSY));
}

TEST_F(ADMA2D, FillRectangleFailsIfForGivenOutputColorFormatAnyComponentOfOutputColorIsOutOfRange)
{
  fillRectangleConfig.outputBufferConfig.colorFormat = DMA2D::OutputColorFormat::RGB565;
  fillRectangleConfig.color =
  {
    .alpha = 0,
    .red   = 40,
    .green = 0,
    .blue  = 10
  };
  ASSERT_THAT(virtualDMA2D.fillRectangle(fillRectangleConfig), Eq(DMA2D::ErrorCode::COLOR_VALUE_OUT_OF_RANGE));
}

TEST_F(ADMA2D, isTransferOngoingReturnsTrueIfThereIsAnOngoingDMATransfer)
{
  virtualDMA2D.fillRectangle(fillRectangleConfig);

  ASSERT_THAT(virtualDMA2D.isTransferOngoing(), true);
}

TEST_F(ADMA2D, isTransferOngoingReturnsFalseIfAnotherTransferIsNotStarted)
{
  ASSERT_THAT(virtualDMA2D.isTransferOngoing(), false);
}

TEST_F(ADMA2D, isTransferOngoingReturnsTrueIfStartBitInCRRegisterIsSet)
{
  constexpr uint32_t DMA2D_CR_START_POSITION = 0u;
  constexpr uint32_t DMA2D_CR_START_FORCED_VALUE = 1u;
  virtualDMA2DPeripheral.CR =
    expectedRegVal(DMA2D_CR_RESET_VALUE, DMA2D_CR_START_POSITION, 1u, DMA2D_CR_START_FORCED_VALUE);

  ASSERT_THAT(virtualDMA2D.isTransferOngoing(), true);
}

TEST_F(ADMA2D, IRQHandlerClearsTCIFFlagInISRRegisterOnlyIfItIsSetAndTransferCompleteInterruptIsEnabled)
{
  constexpr uint32_t DMA2D_ISR_TCIF_POSITION = 1u;
  constexpr uint32_t EXPECTED_DMA2D_ISR_TCIF_VALUE = 0u;
  virtualDMA2DPeripheral.ISR =
    expectedRegVal(DMA2D_ISR_RESET_VALUE, DMA2D_ISR_TCIF_POSITION, 1u, 1u);
  virtualDMA2D.fillRectangle(fillRectangleConfig);
  auto bitValueMatcher =
    BitHasValue(DMA2D_ISR_TCIF_POSITION, EXPECTED_DMA2D_ISR_TCIF_VALUE);
  expectRegisterSet(&(virtualDMA2DPeripheral.ISR), bitValueMatcher);

  virtualDMA2D.IRQHandler();
}

TEST_F(ADMA2D, IRQHandlerDisablesTransferCompleteInterruptBeforeCleaningTCIFFlagInISRRegister)
{
  constexpr uint32_t DMA2D_ISR_TCIF_POSITION = 1u;
  constexpr uint32_t DMA2D_CR_TCIE_POSITION = 9u;
  constexpr uint32_t EXPECTED_DMA2D_CR_TCIE_VALUE = 0u;
  virtualDMA2DPeripheral.ISR =
    expectedRegVal(DMA2D_ISR_RESET_VALUE, DMA2D_ISR_TCIF_POSITION, 1u, 1u);
  virtualDMA2D.fillRectangle(fillRectangleConfig);
  auto bitValueMatcher =
    BitHasValue(DMA2D_CR_TCIE_POSITION, EXPECTED_DMA2D_CR_TCIE_VALUE);
  expectSpecificRegisterSetToBeCalledFirst(&(virtualDMA2DPeripheral.CR), bitValueMatcher);

  virtualDMA2D.IRQHandler();
}

 TEST_F(ADMA2D, IRQHandlerDoesNotClearsTCIFFlagIfTransferCompleteInterruptIsNotEnabled)
 {
  constexpr uint32_t DMA2D_ISR_TCIF_POSITION = 1u;
  constexpr uint32_t DMA2D_CR_TCIE_POSITION = 9u;
  constexpr uint32_t NOT_EXPECTED_DMA2D_CR_TCIE_VALUE = 0u;
  virtualDMA2DPeripheral.ISR =
    expectedRegVal(DMA2D_ISR_RESET_VALUE, DMA2D_ISR_TCIF_POSITION, 1u, 1u);
  virtualDMA2DPeripheral.CR =
    expectedRegVal(DMA2D_CR_RESET_VALUE, DMA2D_CR_TCIE_POSITION, 1u, 0u);
  auto bitValueMatcher =
    BitHasValue(DMA2D_CR_TCIE_POSITION, NOT_EXPECTED_DMA2D_CR_TCIE_VALUE);
  doesNotExpectRegisterSet(&(virtualDMA2DPeripheral.ISR), bitValueMatcher);

  virtualDMA2D.IRQHandler();
}

TEST_F(ADMA2D, IRQHandlerWhenClearsTCIFFlagSetTransferStatusToCompleted)
{
  constexpr uint32_t DMA2D_ISR_TCIF_POSITION = 1u;
  constexpr uint32_t DMA2D_CR_START_POSITION = 0u;
  virtualDMA2DPeripheral.ISR =
    expectedRegVal(DMA2D_ISR_RESET_VALUE, DMA2D_ISR_TCIF_POSITION, 1u, 1u);
  virtualDMA2D.fillRectangle(fillRectangleConfig);

  // simulate that START bit is cleared because the ongoing transfer end
  virtualDMA2DPeripheral.CR =
    expectedRegVal(virtualDMA2DPeripheral.CR, DMA2D_CR_START_POSITION, 1u, 0u);
  virtualDMA2D.IRQHandler();

  ASSERT_THAT(virtualDMA2D.isTransferOngoing(), false);
}

TEST_F(ADMA2D, CopyBitmapSetsModeToMemoryToMemoryWithPixelFormatConversion)
{
  constexpr uint32_t DMA2D_CR_MODE_POSITION = 16u;
  constexpr uint32_t DMA2D_CR_MODE_SIZE = 3u;
  constexpr uint32_t EXPECTED_DMA2D_CR_MODE_VALUE = 0b001;
  auto bitsValueMatcher =
    BitsHaveValue(DMA2D_CR_MODE_POSITION, DMA2D_CR_MODE_SIZE, EXPECTED_DMA2D_CR_MODE_VALUE);
  expectSpecificRegisterSetToBeCalledFirst(&(virtualDMA2DPeripheral.CR), bitsValueMatcher);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.copyBitmap(copyBitmapConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.CR, bitsValueMatcher);
}

TEST_F(ADMA2D, CopyBitmapSetsWantedInputBufferColorFormatInFGPFCCRRegister)
{
  constexpr uint32_t DMA2D_FGPFCCR_CM_POSITION = 0u;
  constexpr uint32_t DMA2D_FGPFCCR_CM_SIZE = 4u;
  constexpr uint32_t EXPECTED_DMA2D_FGPFCCR_CM_VALUE = 0b0100;
  copyBitmapConfig.inputBufferConfig.colorFormat = DMA2D::InputColorFormat::ABGR4444;
  auto bitsValueMatcher =
    BitsHaveValue(DMA2D_FGPFCCR_CM_POSITION, DMA2D_FGPFCCR_CM_SIZE, EXPECTED_DMA2D_FGPFCCR_CM_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.FGPFCCR), bitsValueMatcher);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.copyBitmap(copyBitmapConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.FGPFCCR, bitsValueMatcher);
}

TEST_F(ADMA2D, CopyBitmapSetsRBSBitInFGPFCCRRegisterIfWantedInputBufferColorFormatHasInversedOrderOfRedAndBlue)
{
  constexpr uint32_t DMA2D_FGPFCCR_RBS_POSITION = 21u;
  constexpr uint32_t EXPECTED_DMA2D_FGPFCCR_RBS_VALUE = 0x1;
  copyBitmapConfig.inputBufferConfig.colorFormat = DMA2D::InputColorFormat::ABGR4444;
  auto bitValueMatcher =
    BitHasValue(DMA2D_FGPFCCR_RBS_POSITION, EXPECTED_DMA2D_FGPFCCR_RBS_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.FGPFCCR), bitValueMatcher);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.copyBitmap(copyBitmapConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.FGPFCCR, bitValueMatcher);
}

TEST_F(ADMA2D, CopyBitmapSetsFGMARRegisterValueRelativeToInputBufferAddress)
{
  constexpr uintptr_t INPUT_BUFFER_ADDRESS = 0xAB000000;
  constexpr uint32_t EXPECTED_DMA2D_FGMAR_VALUE =
    INPUT_BUFFER_ADDRESS + DEFAULT_CONFIG_VALUES_DMA2D_FGMAR_OFFSET_VALUE;
  copyBitmapConfig.inputBufferConfig.bufferPtr = reinterpret_cast<void*>(INPUT_BUFFER_ADDRESS);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.FGMAR), EXPECTED_DMA2D_FGMAR_VALUE);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.copyBitmap(copyBitmapConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.FGMAR, EXPECTED_DMA2D_FGMAR_VALUE);
}

TEST_F(ADMA2D, CopyBitmapAddsOffsetToInputBufferAddressAccordingToPositionAndInputBufferColorFormatAndBufferWidth)
{
  constexpr uint16_t RECTANGLE_X_POS = 100u;
  constexpr uint16_t RECTANGLE_Y_POS = 50u;
  constexpr uint16_t INPUT_BUFFER_WIDTH = 200u;
  constexpr uintptr_t INPUT_BUFFER_OFFSET =
    PIXEL_SIZE_RGB565 * (static_cast<uintptr_t>(RECTANGLE_X_POS) +
    static_cast<uintptr_t>(RECTANGLE_Y_POS) * static_cast<uintptr_t>(INPUT_BUFFER_WIDTH));
  constexpr uint32_t EXPECTED_DMA2D_FGMAR_VALUE =
    DEFAULT_CONFIG_VALUES_INPUT_BUFFER_ADDRESS + INPUT_BUFFER_OFFSET;
  copyBitmapConfig.inputBufferConfig.colorFormat = DMA2D::InputColorFormat::RGB565;
  copyBitmapConfig.inputBufferConfig.position =
  {
    .x = RECTANGLE_X_POS,
    .y = RECTANGLE_Y_POS
  };
  copyBitmapConfig.inputBufferConfig.bufferDimension.width = INPUT_BUFFER_WIDTH;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.FGMAR), EXPECTED_DMA2D_FGMAR_VALUE);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.copyBitmap(copyBitmapConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.FGMAR, EXPECTED_DMA2D_FGMAR_VALUE);
}

TEST_F(ADMA2D, CopyBitmapSetsFGORRegisterValueAccordingToInputBufferWidthAndColorFormatAndCopyRectangleWidth)
{
  constexpr uint16_t COPY_RECTANGLE_WIDTH = 100u;
  constexpr uint16_t INPUT_BUFFER_WIDTH = 200u;
  constexpr uint32_t EXPECTED_DMA2D_FGOR_VALUE =
    PIXEL_SIZE_RGB565 * (INPUT_BUFFER_WIDTH - COPY_RECTANGLE_WIDTH);
  copyBitmapConfig.copyRectangleDimension.width = COPY_RECTANGLE_WIDTH,
  copyBitmapConfig.inputBufferConfig.colorFormat = DMA2D::InputColorFormat::RGB565;
  copyBitmapConfig.inputBufferConfig.bufferDimension.width = INPUT_BUFFER_WIDTH;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.FGOR), EXPECTED_DMA2D_FGOR_VALUE);

 const DMA2D::ErrorCode errorCode = virtualDMA2D.copyBitmap(copyBitmapConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.FGOR, EXPECTED_DMA2D_FGOR_VALUE);
}

TEST_F(ADMA2D, CopyBitmapSetsWantedOutputBufferColorFormatInOPFCCRRegister)
{
  constexpr uint32_t DMA2D_OPFCCR_CM_POSITION = 0u;
  constexpr uint32_t DMA2D_OPFCCR_CM_SIZE = 3u;
  constexpr uint32_t EXPECTED_DMA2D_OPFCCR_CM_VALUE = 0b0011;
  copyBitmapConfig.outputBufferConfig.colorFormat = DMA2D::OutputColorFormat::ARGB1555;
  auto bitsValueMatcher =
    BitsHaveValue(DMA2D_OPFCCR_CM_POSITION, DMA2D_OPFCCR_CM_SIZE, EXPECTED_DMA2D_OPFCCR_CM_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.OPFCCR), bitsValueMatcher);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.copyBitmap(copyBitmapConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.OPFCCR, bitsValueMatcher);
}

TEST_F(ADMA2D, CopyBitmapSetsRBSBitInOPFCCRRegisterIfWantedOutputBufferColorFormatHasInversedOrderOfRedAndBlue)
{
  constexpr uint32_t DMA2D_OPFCCR_RBS_POSITION = 21u;
  constexpr uint32_t EXPECTED_DMA2D_OPFCCR_RBS_VALUE = 0x1;
  copyBitmapConfig.outputBufferConfig.colorFormat = DMA2D::OutputColorFormat::BGR888;
  auto bitValueMatcher =
    BitHasValue(DMA2D_OPFCCR_RBS_POSITION, EXPECTED_DMA2D_OPFCCR_RBS_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.OPFCCR), bitValueMatcher);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.copyBitmap(copyBitmapConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.OPFCCR, bitValueMatcher);
}

TEST_F(ADMA2D, CopyBitmapSetsOMARRegisterValueRelativeToOutputBufferAddress)
{
  constexpr uintptr_t OUTPUT_BUFFER_ADDRESS = 0xAB000000;
  constexpr uint32_t EXPECTED_DMA2D_OMAR_VALUE =
    OUTPUT_BUFFER_ADDRESS + DEFAULT_CONFIG_VALUES_DMA2D_OMAR_OFFSET_VALUE;
  copyBitmapConfig.outputBufferConfig.bufferPtr = reinterpret_cast<void*>(OUTPUT_BUFFER_ADDRESS);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.OMAR), EXPECTED_DMA2D_OMAR_VALUE);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.copyBitmap(copyBitmapConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.OMAR, EXPECTED_DMA2D_OMAR_VALUE);
}

TEST_F(ADMA2D, CopyBitmapAddsOffsetToOutputBufferAddressAccordingToOutputPositionAndOutputColorFormatAndOutputBufferWidth)
{
  constexpr uint16_t OUTPUT_X_POS = 100u;
  constexpr uint16_t OUTPUT_Y_POS = 50u;
  constexpr uint16_t OUTPUT_BUFFER_WIDTH = 200u;
  constexpr uintptr_t OUTPUT_BUFFER_OFFSET =
    PIXEL_SIZE_RGB888 * (static_cast<uintptr_t>(OUTPUT_X_POS) +
    static_cast<uintptr_t>(OUTPUT_Y_POS) * static_cast<uintptr_t>(OUTPUT_BUFFER_WIDTH));
  constexpr uint32_t EXPECTED_DMA2D_OMAR_VALUE =
    DEFAULT_CONFIG_VALUES_OUTPUT_BUFFER_ADDRESS + OUTPUT_BUFFER_OFFSET;
  copyBitmapConfig.outputBufferConfig.colorFormat = DMA2D::OutputColorFormat::RGB888;
  copyBitmapConfig.outputBufferConfig.position =
  {
    .x = OUTPUT_X_POS,
    .y = OUTPUT_Y_POS
  };
  copyBitmapConfig.outputBufferConfig.bufferDimension.width = OUTPUT_BUFFER_WIDTH;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.OMAR), EXPECTED_DMA2D_OMAR_VALUE);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.copyBitmap(copyBitmapConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.OMAR, EXPECTED_DMA2D_OMAR_VALUE);
}

TEST_F(ADMA2D, CopyBitmapSetsOORRegisterValueAccordingToOutputBufferWidthAndCopyRectangleWidthAndOutputBufferColorFormat)
{
  constexpr uint16_t RECTANGLE_WIDTH = 100u;
  constexpr uint16_t OUTPUT_BUFFER_WIDTH = 200u;
  constexpr uint32_t EXPECTED_DMA2D_OOR_VALUE =
    PIXEL_SIZE_RGB888 * (OUTPUT_BUFFER_WIDTH - RECTANGLE_WIDTH);
  copyBitmapConfig.copyRectangleDimension.width = RECTANGLE_WIDTH,
  copyBitmapConfig.outputBufferConfig.bufferDimension.width = OUTPUT_BUFFER_WIDTH;
  copyBitmapConfig.outputBufferConfig.colorFormat = DMA2D::OutputColorFormat::RGB888;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.OOR), EXPECTED_DMA2D_OOR_VALUE);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.copyBitmap(copyBitmapConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.OOR, EXPECTED_DMA2D_OOR_VALUE);
}

TEST_F(ADMA2D, CopyBitmapSetsNLRRegisterValueAccordingToCopyRectangleDimension)
{
  constexpr uint16_t DMA2D_NLR_PL_POSITION = 16;
  constexpr uint16_t RECTANGLE_WIDTH = 100u;
  constexpr uint16_t RECTANGLE_HEIGHT = 100u;
  constexpr uint32_t EXPECTED_DMA2D_NLR_VALUE =
    (static_cast<uint32_t>(RECTANGLE_WIDTH) << DMA2D_NLR_PL_POSITION) | static_cast<uint32_t>(RECTANGLE_HEIGHT);
  copyBitmapConfig.copyRectangleDimension =
  {
    .width  = RECTANGLE_WIDTH,
    .height = RECTANGLE_HEIGHT
  };
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.NLR), EXPECTED_DMA2D_NLR_VALUE);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.copyBitmap(copyBitmapConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.NLR, EXPECTED_DMA2D_NLR_VALUE);
}

TEST_F(ADMA2D, CopyBitmapEnablesDMA2DTransferCompleteInterrupt)
{
  constexpr uint32_t DMA2D_CR_TCIE_POSITION = 9u;
  constexpr uint32_t EXPECTED_DMA2D_CR_TCIE_VALUE = 1u;
  auto bitValueMatcher =
    BitHasValue(DMA2D_CR_TCIE_POSITION, EXPECTED_DMA2D_CR_TCIE_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.CR), bitValueMatcher);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.copyBitmap(copyBitmapConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.CR, bitValueMatcher);
}

TEST_F(ADMA2D, CopyBitmapStartsDMA2DAtTheEndOfCopyBitmapFunctionCall)
{
  constexpr uint32_t DMA2D_CR_START_POSITION = 0u;
  constexpr uint32_t EXPECTED_DMA2D_CR_START_VALUE = 1u;
  auto bitValueMatcher =
    BitHasValue(DMA2D_CR_START_POSITION, EXPECTED_DMA2D_CR_START_VALUE);
  expectSpecificRegisterSetToBeCalledLast(&(virtualDMA2DPeripheral.CR), bitValueMatcher);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.copyBitmap(copyBitmapConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.CR, bitValueMatcher);
}

TEST_F(ADMA2D, CopyBitmapFailsIfAnotherDMA2DTransferIsOngoing)
{
  ASSERT_THAT(virtualDMA2D.copyBitmap(copyBitmapConfig), Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2D.copyBitmap(copyBitmapConfig), Eq(DMA2D::ErrorCode::BUSY));
}

TEST_F(ADMA2D, BlendBitmapSetsModeToMemoryToMemoryWithBlending)
{
  constexpr uint32_t DMA2D_CR_MODE_POSITION = 16u;
  constexpr uint32_t DMA2D_CR_MODE_SIZE = 3u;
  constexpr uint32_t EXPECTED_DMA2D_CR_MODE_VALUE = 0b010;
  auto bitsValueMatcher =
    BitsHaveValue(DMA2D_CR_MODE_POSITION, DMA2D_CR_MODE_SIZE, EXPECTED_DMA2D_CR_MODE_VALUE);
  expectSpecificRegisterSetToBeCalledFirst(&(virtualDMA2DPeripheral.CR), bitsValueMatcher);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.blendBitmap(blendBitmapConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.CR, bitsValueMatcher);
}

TEST_F(ADMA2D, BlendBitmapSetsWantedForegroundBufferColorFormatInFGPFCCRRegister)
{
  constexpr uint32_t DMA2D_FGPFCCR_CM_POSITION = 0u;
  constexpr uint32_t DMA2D_FGPFCCR_CM_SIZE = 4u;
  constexpr uint32_t EXPECTED_DMA2D_FGPFCCR_CM_VALUE = 0b0011;
  blendBitmapConfig.foregroundBufferConfig.colorFormat = DMA2D::InputColorFormat::ABGR1555;
  auto bitsValueMatcher =
    BitsHaveValue(DMA2D_FGPFCCR_CM_POSITION, DMA2D_FGPFCCR_CM_SIZE, EXPECTED_DMA2D_FGPFCCR_CM_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.FGPFCCR), bitsValueMatcher);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.blendBitmap(blendBitmapConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.FGPFCCR, bitsValueMatcher);
}

TEST_F(ADMA2D, BlendBitmapSetsRBSBitInFGPFCCRRegisterIfWantedForegroundBufferColorFormatHasInversedOrderOfRedAndBlue)
{
  constexpr uint32_t DMA2D_FGPFCCR_RBS_POSITION = 21u;
  constexpr uint32_t EXPECTED_DMA2D_FGPFCCR_RBS_VALUE = 0x1;
  blendBitmapConfig.foregroundBufferConfig.colorFormat = DMA2D::InputColorFormat::ABGR1555;
  auto bitValueMatcher =
    BitHasValue(DMA2D_FGPFCCR_RBS_POSITION, EXPECTED_DMA2D_FGPFCCR_RBS_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.FGPFCCR), bitValueMatcher);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.blendBitmap(blendBitmapConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.FGPFCCR, bitValueMatcher);
}

TEST_F(ADMA2D, BlendBitmapSetsFGMARRegisterValueRelativeToForegroundBufferAddress)
{
  constexpr uintptr_t FOREGROUND_BUFFER_ADDRESS = 0xEB000000;
  constexpr uint32_t EXPECTED_DMA2D_FGMAR_VALUE =
    FOREGROUND_BUFFER_ADDRESS + DEFAULT_CONFIG_VALUES_DMA2D_FGMAR_OFFSET_VALUE;
  blendBitmapConfig.foregroundBufferConfig.bufferPtr = reinterpret_cast<void*>(FOREGROUND_BUFFER_ADDRESS);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.FGMAR), EXPECTED_DMA2D_FGMAR_VALUE);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.blendBitmap(blendBitmapConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.FGMAR, EXPECTED_DMA2D_FGMAR_VALUE);
}

TEST_F(ADMA2D, BlendBitmapAddsOffsetToForegroundBufferAddressAccordingToPositionAndForegroundBufferColorFormatAndBufferWidth)
{
  constexpr uint16_t RECTANGLE_X_POS = 100u;
  constexpr uint16_t RECTANGLE_Y_POS = 50u;
  constexpr uint16_t FOREGROUND_BUFFER_WIDTH = 200u;
  constexpr uintptr_t FOREGROUND_BUFFER_OFFSET =
    PIXEL_SIZE_RGB565 * (static_cast<uintptr_t>(RECTANGLE_X_POS) +
    static_cast<uintptr_t>(RECTANGLE_Y_POS) * static_cast<uintptr_t>(FOREGROUND_BUFFER_WIDTH));
  constexpr uint32_t EXPECTED_DMA2D_FGMAR_VALUE =
    DEFAULT_CONFIG_VALUES_INPUT_BUFFER_ADDRESS + FOREGROUND_BUFFER_OFFSET;
  blendBitmapConfig.foregroundBufferConfig.colorFormat = DMA2D::InputColorFormat::RGB565;
  blendBitmapConfig.foregroundBufferConfig.position =
  {
    .x = RECTANGLE_X_POS,
    .y = RECTANGLE_Y_POS
  };
  blendBitmapConfig.foregroundBufferConfig.bufferDimension.width = FOREGROUND_BUFFER_WIDTH;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.FGMAR), EXPECTED_DMA2D_FGMAR_VALUE);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.blendBitmap(blendBitmapConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.FGMAR, EXPECTED_DMA2D_FGMAR_VALUE);
}

TEST_F(ADMA2D, BlendBitmapSetsFGORRegisterValueAccordingToForegroundBufferWidthAndColorFormatAndBlendRectangleWidth)
{
  constexpr uint16_t BLEND_RECTANGLE_WIDTH = 100u;
  constexpr uint16_t FOREGROUND_BUFFER_WIDTH = 200u;
  constexpr uint32_t EXPECTED_DMA2D_FGOR_VALUE =
    PIXEL_SIZE_RGB565 * (FOREGROUND_BUFFER_WIDTH - BLEND_RECTANGLE_WIDTH);
  blendBitmapConfig.blendRectangleDimension.width = BLEND_RECTANGLE_WIDTH,
  blendBitmapConfig.foregroundBufferConfig.colorFormat = DMA2D::InputColorFormat::RGB565;
  blendBitmapConfig.foregroundBufferConfig.bufferDimension.width = FOREGROUND_BUFFER_WIDTH;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.FGOR), EXPECTED_DMA2D_FGOR_VALUE);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.blendBitmap(blendBitmapConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.FGOR, EXPECTED_DMA2D_FGOR_VALUE);
}

TEST_F(ADMA2D, BlendBitmapSetsWantedBackgroundBufferColorFormatInBGPFCCRRegister)
{
  constexpr uint32_t DMA2D_BGPFCCR_CM_POSITION = 0u;
  constexpr uint32_t DMA2D_BGPFCCR_CM_SIZE = 4u;
  constexpr uint32_t EXPECTED_DMA2D_BGPFCCR_CM_VALUE = 0b0011;
  blendBitmapConfig.backgroundBufferConfig.colorFormat = DMA2D::InputColorFormat::ABGR1555;
  auto bitsValueMatcher =
    BitsHaveValue(DMA2D_BGPFCCR_CM_POSITION, DMA2D_BGPFCCR_CM_SIZE, EXPECTED_DMA2D_BGPFCCR_CM_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.BGPFCCR), bitsValueMatcher);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.blendBitmap(blendBitmapConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.BGPFCCR, bitsValueMatcher);
}

TEST_F(ADMA2D, BlendBitmapSetsRBSBitInBGPFCCRRegisterIfWantedBackgroundBufferColorFormatHasInversedOrderOfRedAndBlue)
{
  constexpr uint32_t DMA2D_BGPFCCR_RBS_POSITION = 21u;
  constexpr uint32_t EXPECTED_DMA2D_BGPFCCR_RBS_VALUE = 0x1;
  blendBitmapConfig.backgroundBufferConfig.colorFormat = DMA2D::InputColorFormat::BGR888;
  auto bitValueMatcher =
    BitHasValue(DMA2D_BGPFCCR_RBS_POSITION, EXPECTED_DMA2D_BGPFCCR_RBS_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.BGPFCCR), bitValueMatcher);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.blendBitmap(blendBitmapConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.BGPFCCR, bitValueMatcher);
}

TEST_F(ADMA2D, BlendBitmapSetsBGMARRegisterValueRelativeToBackgroundBufferAddress)
{
  constexpr uintptr_t BACKGROUND_BUFFER_ADDRESS = 0xEB000000;
  constexpr uint32_t EXPECTED_DMA2D_BGMAR_VALUE =
    BACKGROUND_BUFFER_ADDRESS + DEFAULT_CONFIG_VALUES_DMA2D_BGMAR_OFFSET_VALUE;
  blendBitmapConfig.backgroundBufferConfig.bufferPtr = reinterpret_cast<void*>(BACKGROUND_BUFFER_ADDRESS);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.BGMAR), EXPECTED_DMA2D_BGMAR_VALUE);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.blendBitmap(blendBitmapConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.BGMAR, EXPECTED_DMA2D_BGMAR_VALUE);
}

TEST_F(ADMA2D, BlendBitmapAddsOffsetToBackgroundBufferAddressAccordingToPositionAndBackgroundBufferColorFormatAndBufferWidth)
{
  constexpr uint16_t RECTANGLE_X_POS = 100u;
  constexpr uint16_t RECTANGLE_Y_POS = 50u;
  constexpr uint16_t BACKGROUND_BUFFER_WIDTH = 200u;
  constexpr uintptr_t BACKGROUND_BUFFER_OFFSET =
    PIXEL_SIZE_RGB565 * (static_cast<uintptr_t>(RECTANGLE_X_POS) +
    static_cast<uintptr_t>(RECTANGLE_Y_POS) * static_cast<uintptr_t>(BACKGROUND_BUFFER_WIDTH));
  constexpr uint32_t EXPECTED_DMA2D_BGMAR_VALUE =
    DEFAULT_CONFIG_VALUES_INPUT_BUFFER_ADDRESS + BACKGROUND_BUFFER_OFFSET;
  blendBitmapConfig.backgroundBufferConfig.colorFormat = DMA2D::InputColorFormat::RGB565;
  blendBitmapConfig.backgroundBufferConfig.position =
  {
    .x = RECTANGLE_X_POS,
    .y = RECTANGLE_Y_POS
  };
  blendBitmapConfig.backgroundBufferConfig.bufferDimension.width = BACKGROUND_BUFFER_WIDTH;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.BGMAR), EXPECTED_DMA2D_BGMAR_VALUE);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.blendBitmap(blendBitmapConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.BGMAR, EXPECTED_DMA2D_BGMAR_VALUE);
}

TEST_F(ADMA2D, BlendBitmapSetsBGORRegisterValueAccordingToBackgroundBufferWidthAndColorFormatAndBlendRectangleWidth)
{
  constexpr uint16_t BLEND_RECTANGLE_WIDTH = 100u;
  constexpr uint16_t BACKGROUND_BUFFER_WIDTH = 200u;
  constexpr uint32_t EXPECTED_DMA2D_BGOR_VALUE =
    PIXEL_SIZE_RGB565 * (BACKGROUND_BUFFER_WIDTH - BLEND_RECTANGLE_WIDTH);
  blendBitmapConfig.blendRectangleDimension.width = BLEND_RECTANGLE_WIDTH,
  blendBitmapConfig.backgroundBufferConfig.colorFormat = DMA2D::InputColorFormat::RGB565;
  blendBitmapConfig.backgroundBufferConfig.bufferDimension.width = BACKGROUND_BUFFER_WIDTH;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.BGOR), EXPECTED_DMA2D_BGOR_VALUE);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.blendBitmap(blendBitmapConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.BGOR, EXPECTED_DMA2D_BGOR_VALUE);
}

TEST_F(ADMA2D, BlendBitmapSetsWantedOutputBufferColorFormatInOPFCCRRegister)
{
  constexpr uint32_t DMA2D_OPFCCR_CM_POSITION = 0u;
  constexpr uint32_t DMA2D_OPFCCR_CM_SIZE = 3u;
  constexpr uint32_t EXPECTED_DMA2D_OPFCCR_CM_VALUE = 0b0011;
  blendBitmapConfig.outputBufferConfig.colorFormat = DMA2D::OutputColorFormat::ARGB1555;
  auto bitsValueMatcher =
    BitsHaveValue(DMA2D_OPFCCR_CM_POSITION, DMA2D_OPFCCR_CM_SIZE, EXPECTED_DMA2D_OPFCCR_CM_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.OPFCCR), bitsValueMatcher);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.blendBitmap(blendBitmapConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.OPFCCR, bitsValueMatcher);
}

TEST_F(ADMA2D, BlendBitmapSetsRBSBitInOPFCCRRegisterIfWantedOutputBufferColorFormatHasInversedOrderOfRedAndBlue)
{
  constexpr uint32_t DMA2D_OPFCCR_RBS_POSITION = 21u;
  constexpr uint32_t EXPECTED_DMA2D_OPFCCR_RBS_VALUE = 0x1;
  blendBitmapConfig.outputBufferConfig.colorFormat = DMA2D::OutputColorFormat::BGR888;
  auto bitValueMatcher =
    BitHasValue(DMA2D_OPFCCR_RBS_POSITION, EXPECTED_DMA2D_OPFCCR_RBS_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.OPFCCR), bitValueMatcher);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.blendBitmap(blendBitmapConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.OPFCCR, bitValueMatcher);
}

TEST_F(ADMA2D, BlendBitmapSetsOMARRegisterValueRelativeToOutputBufferAddress)
{
  constexpr uintptr_t OUTPUT_BUFFER_ADDRESS = 0xAB000000;
  constexpr uint32_t EXPECTED_DMA2D_OMAR_VALUE =
    OUTPUT_BUFFER_ADDRESS + DEFAULT_CONFIG_VALUES_DMA2D_OMAR_OFFSET_VALUE;
  blendBitmapConfig.outputBufferConfig.bufferPtr = reinterpret_cast<void*>(OUTPUT_BUFFER_ADDRESS);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.OMAR), EXPECTED_DMA2D_OMAR_VALUE);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.blendBitmap(blendBitmapConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.OMAR, EXPECTED_DMA2D_OMAR_VALUE);
}

TEST_F(ADMA2D, BlendBitmapAddsOffsetToOutputBufferAddressAccordingToOutputPositionAndOutputColorFormatAndOutputBufferWidth)
{
  constexpr uint16_t OUTPUT_X_POS = 100u;
  constexpr uint16_t OUTPUT_Y_POS = 50u;
  constexpr uint16_t OUTPUT_BUFFER_WIDTH = 200u;
  constexpr uintptr_t OUTPUT_BUFFER_OFFSET =
    PIXEL_SIZE_RGB888 * (static_cast<uintptr_t>(OUTPUT_X_POS) +
    static_cast<uintptr_t>(OUTPUT_Y_POS) * static_cast<uintptr_t>(OUTPUT_BUFFER_WIDTH));
  constexpr uint32_t EXPECTED_DMA2D_OMAR_VALUE =
    DEFAULT_CONFIG_VALUES_OUTPUT_BUFFER_ADDRESS + OUTPUT_BUFFER_OFFSET;
  blendBitmapConfig.outputBufferConfig.colorFormat = DMA2D::OutputColorFormat::RGB888;
  blendBitmapConfig.outputBufferConfig.position =
  {
    .x = OUTPUT_X_POS,
    .y = OUTPUT_Y_POS
  };
  blendBitmapConfig.outputBufferConfig.bufferDimension.width = OUTPUT_BUFFER_WIDTH;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.OMAR), EXPECTED_DMA2D_OMAR_VALUE);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.blendBitmap(blendBitmapConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.OMAR, EXPECTED_DMA2D_OMAR_VALUE);
}

TEST_F(ADMA2D, BlendBitmapSetsOORRegisterValueAccordingToOutputBufferWidthAndCopyRectangleWidthAndOutputBufferColorFormat)
{
  constexpr uint16_t RECTANGLE_WIDTH = 100u;
  constexpr uint16_t OUTPUT_BUFFER_WIDTH = 200u;
  constexpr uint32_t EXPECTED_DMA2D_OOR_VALUE =
    PIXEL_SIZE_RGB888 * (OUTPUT_BUFFER_WIDTH - RECTANGLE_WIDTH);
  blendBitmapConfig.blendRectangleDimension.width = RECTANGLE_WIDTH,
  blendBitmapConfig.outputBufferConfig.bufferDimension.width = OUTPUT_BUFFER_WIDTH;
  blendBitmapConfig.outputBufferConfig.colorFormat = DMA2D::OutputColorFormat::RGB888;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.OOR), EXPECTED_DMA2D_OOR_VALUE);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.blendBitmap(blendBitmapConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.OOR, EXPECTED_DMA2D_OOR_VALUE);
}

TEST_F(ADMA2D, BlendBitmapSetsNLRRegisterValueAccordingToCopyRectangleDimension)
{
  constexpr uint16_t DMA2D_NLR_PL_POSITION = 16;
  constexpr uint16_t RECTANGLE_WIDTH = 100u;
  constexpr uint16_t RECTANGLE_HEIGHT = 100u;
  constexpr uint32_t EXPECTED_DMA2D_NLR_VALUE =
    (static_cast<uint32_t>(RECTANGLE_WIDTH) << DMA2D_NLR_PL_POSITION) | static_cast<uint32_t>(RECTANGLE_HEIGHT);
  blendBitmapConfig.blendRectangleDimension =
  {
    .width  = RECTANGLE_WIDTH,
    .height = RECTANGLE_HEIGHT
  };
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.NLR), EXPECTED_DMA2D_NLR_VALUE);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.blendBitmap(blendBitmapConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.NLR, EXPECTED_DMA2D_NLR_VALUE);
}

TEST_F(ADMA2D, BlendBitmapEnablesDMA2DTransferCompleteInterrupt)
{
  constexpr uint32_t DMA2D_CR_TCIE_POSITION = 9u;
  constexpr uint32_t EXPECTED_DMA2D_CR_TCIE_VALUE = 1u;
  auto bitValueMatcher =
    BitHasValue(DMA2D_CR_TCIE_POSITION, EXPECTED_DMA2D_CR_TCIE_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.CR), bitValueMatcher);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.blendBitmap(blendBitmapConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.CR, bitValueMatcher);
}

TEST_F(ADMA2D, BlendBitmapStartsDMA2DAtTheEndOfCopyBitmapFunctionCall)
{
  constexpr uint32_t DMA2D_CR_START_POSITION = 0u;
  constexpr uint32_t EXPECTED_DMA2D_CR_START_VALUE = 1u;
  auto bitValueMatcher =
    BitHasValue(DMA2D_CR_START_POSITION, EXPECTED_DMA2D_CR_START_VALUE);
  expectSpecificRegisterSetToBeCalledLast(&(virtualDMA2DPeripheral.CR), bitValueMatcher);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.blendBitmap(blendBitmapConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.CR, bitValueMatcher);
}

TEST_F(ADMA2D, BlendBitmapFailsIfAnotherDMA2DTransferIsOngoing)
{
  ASSERT_THAT(virtualDMA2D.blendBitmap(blendBitmapConfig), Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2D.blendBitmap(blendBitmapConfig), Eq(DMA2D::ErrorCode::BUSY));
}