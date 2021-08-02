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

  static constexpr uint32_t PIXEL_SIZE_RGB888 = 3u;
  static constexpr uint32_t PIXEL_SIZE_ARGB8888 = 4u;

  static constexpr uint16_t DEFAULT_CONFIG_VALUE_X_POS = 50u;
  static constexpr uint16_t DEFAULT_CONFIG_VALUE_Y_POS = 50u;
  static constexpr uint16_t DEFAULT_CONFIG_VALUE_FB_WIDTH = 100u;
  static constexpr uint16_t DEFAULT_CONFIG_VALUE_FB_HEIGHT = 100u;
  static constexpr uintptr_t DEFAULT_CONFIG_VALUES_FRAME_BUFFER_ADDRESS = 0xBEEF0000;
  static constexpr uintptr_t DEFAULT_CONFIG_VALUES_DMA2D_OMAR_OFFSET_VALUE =
    PIXEL_SIZE_ARGB8888 * (DEFAULT_CONFIG_VALUE_X_POS + DEFAULT_CONFIG_VALUE_Y_POS * DEFAULT_CONFIG_VALUE_FB_WIDTH);

  DMA2D::FillRectangleConfig fillRectangleConfig;

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

  fillRectangleConfig.outputColorFormat = DMA2D::OutputColorFormat::ARGB8888;
  fillRectangleConfig.position =
  {
    .x = DEFAULT_CONFIG_VALUE_X_POS,
    .y = DEFAULT_CONFIG_VALUE_Y_POS
  };
  fillRectangleConfig.frameBufferDimension =
  {
    .width  = DEFAULT_CONFIG_VALUE_FB_WIDTH,
    .height = DEFAULT_CONFIG_VALUE_FB_HEIGHT
  };
  fillRectangleConfig.frameBufferPtr = reinterpret_cast<void*>(DEFAULT_CONFIG_VALUES_FRAME_BUFFER_ADDRESS);
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

TEST_F(ADMA2D, FillRectangleSetsWantedOutputColorFormatInOPFCCRRegister)
{
  constexpr uint32_t DMA2D_OPFCCR_CM_POSITION = 0u;
  constexpr uint32_t DMA2D_OPFCCR_CM_SIZE = 3u;
  constexpr uint32_t EXPECTED_DMA2D_OPFCCR_CM_VALUE = 0b010;
  fillRectangleConfig.outputColorFormat = DMA2D::OutputColorFormat::RGB565;
  auto bitsValueMatcher =
    BitsHaveValue(DMA2D_OPFCCR_CM_POSITION, DMA2D_OPFCCR_CM_SIZE, EXPECTED_DMA2D_OPFCCR_CM_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.OPFCCR), bitsValueMatcher);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.fillRectangle(fillRectangleConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.OPFCCR, bitsValueMatcher);
}

TEST_F(ADMA2D, FillRectangleSetsRBSBitInOPFCCRRegisterIfWantedOutputColorFormatHasInversedOrderOfRedAndBlue)
{
  constexpr uint32_t DMA2D_OPFCCR_RBS_POSITION = 21u;
  constexpr uint32_t EXPECTED_DMA2D_OPFCCR_RBS_VALUE = 0x1;
  fillRectangleConfig.outputColorFormat = DMA2D::OutputColorFormat::ABGR4444;
  auto bitValueMatcher =
    BitHasValue(DMA2D_OPFCCR_RBS_POSITION, EXPECTED_DMA2D_OPFCCR_RBS_VALUE);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.OPFCCR), bitValueMatcher);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.fillRectangle(fillRectangleConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.OPFCCR, bitValueMatcher);
}

TEST_F(ADMA2D, FillRectangleSetsWantedColorInOCOLRRegisterInAccordanceToChoosenOutputColorFormat)
{
  constexpr uint8_t EXPECTED_DMA2D_OCOLR_BLUE_VALUE = 4u;
  constexpr uint8_t EXPECTED_DMA2D_OCOLR_GREEN_VALUE = 10u;
  constexpr uint8_t EXPECTED_DMA2D_OCOLR_RED_VALUE = 15u;
  constexpr uint8_t EXPECTED_DMA2D_OCOLR_ALPHA_VALUE = 3u;
  fillRectangleConfig.outputColorFormat = DMA2D::OutputColorFormat::ARGB4444;
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

TEST_F(ADMA2D, FillRectangleSetsOMARRegisterValueRelativeToFrameBufferAddress)
{
  constexpr uintptr_t FRAME_BUFFER_ADDRESS = 0xAB000000;
  constexpr uint32_t EXPECTED_DMA2D_OMAR_VALUE =
    FRAME_BUFFER_ADDRESS + DEFAULT_CONFIG_VALUES_DMA2D_OMAR_OFFSET_VALUE;
  fillRectangleConfig.frameBufferPtr = reinterpret_cast<void*>(FRAME_BUFFER_ADDRESS);
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.OMAR), EXPECTED_DMA2D_OMAR_VALUE);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.fillRectangle(fillRectangleConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.OMAR, EXPECTED_DMA2D_OMAR_VALUE);
}

TEST_F(ADMA2D, FillRectangleAddsOffsetToFrameBufferAddressAccordingToPositionAndColorFormatAndFrameBufferWidth)
{
  constexpr uint16_t RECTANGLE_X_POS = 100u;
  constexpr uint16_t RECTANGLE_Y_POS = 50u;
  constexpr uint16_t FRAME_BUFFER_WIDTH = 200u;
  constexpr uintptr_t FRAME_BUFFER_OFFSET =
    PIXEL_SIZE_RGB888 * (static_cast<uintptr_t>(RECTANGLE_X_POS) +
    static_cast<uintptr_t>(RECTANGLE_Y_POS) * static_cast<uintptr_t>(FRAME_BUFFER_WIDTH));
  constexpr uint32_t EXPECTED_DMA2D_OMAR_VALUE =
    DEFAULT_CONFIG_VALUES_FRAME_BUFFER_ADDRESS + FRAME_BUFFER_OFFSET;
  fillRectangleConfig.outputColorFormat = DMA2D::OutputColorFormat::RGB888;
  fillRectangleConfig.position =
  {
    .x = RECTANGLE_X_POS,
    .y = RECTANGLE_Y_POS
  };
  fillRectangleConfig.frameBufferDimension.width = FRAME_BUFFER_WIDTH;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.OMAR), EXPECTED_DMA2D_OMAR_VALUE);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.fillRectangle(fillRectangleConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.OMAR, EXPECTED_DMA2D_OMAR_VALUE);
}

TEST_F(ADMA2D, FillRectangleSetsOORRegisterValueAccordingToFrameBufferWidthAndRectangleWidthAndOutputColorFormat)
{
  constexpr uint16_t RECTANGLE_WIDTH = 100u;
  constexpr uint16_t FRAME_BUFFER_WIDTH = 200u;
  constexpr uint32_t EXPECTED_DMA2D_OOR_VALUE =
    PIXEL_SIZE_RGB888 * (FRAME_BUFFER_WIDTH - RECTANGLE_WIDTH);
  fillRectangleConfig.rectangleDimension.width = RECTANGLE_WIDTH,
  fillRectangleConfig.frameBufferDimension.width = FRAME_BUFFER_WIDTH;
  fillRectangleConfig.outputColorFormat = DMA2D::OutputColorFormat::RGB888;
  expectSpecificRegisterSetWithNoChangesAfter(&(virtualDMA2DPeripheral.OOR), EXPECTED_DMA2D_OOR_VALUE);

  const DMA2D::ErrorCode errorCode = virtualDMA2D.fillRectangle(fillRectangleConfig);

  ASSERT_THAT(errorCode, Eq(DMA2D::ErrorCode::OK));
  ASSERT_THAT(virtualDMA2DPeripheral.OOR, EXPECTED_DMA2D_OOR_VALUE);
}

TEST_F(ADMA2D, FillRectangleSetsNLRRegisterValueAccordingRectangleDimension)
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