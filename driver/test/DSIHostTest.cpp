#include "stm32l4r9xx.h"
#include "Peripheral.h"

class DSIHost
{
public:

  DSIHost(DSI_TypeDef *DSIHostPeripheralPtr);

private:

  //! Pointer to DSI Host peripheral
  DSI_TypeDef *m_DSIHostPeripheralPtr;
};

DSIHost::DSIHost(DSI_TypeDef *DSIHostPeripheralPtr):
  m_DSIHostPeripheralPtr(DSIHostPeripheralPtr)
{}

#include "DriverTest.h"
#include "ResetControlMock.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"


using namespace ::testing;


class ADSIHost : public DriverTest
{
public:

  //! Based on real reset values for DSI Host registers (source STM32L4R9 reference manual)
  static constexpr uint32_t DSIHOST_VR_RESET_VALUE      = 0x3133302A;
  static constexpr uint32_t DSIHOST_CR_RESET_VALUE      = 0x00000000;
  static constexpr uint32_t DSIHOST_CCR_RESET_VALUE     = 0x00000000;
  static constexpr uint32_t DSIHOST_LVCIDR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t DSIHOST_LCOLCR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t DSIHOST_LPCR_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DSIHOST_LPMCR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t DSIHOST_PCR_RESET_VALUE     = 0x00000000;
  static constexpr uint32_t DSIHOST_GVCIDR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t DSIHOST_MCR_RESET_VALUE     = 0x00000001;
  static constexpr uint32_t DSIHOST_VMCR_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DSIHOST_VPCR_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DSIHOST_VCCR_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DSIHOST_VNPCR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t DSIHOST_VHSACR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t DSIHOST_VHBPCR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t DSIHOST_VLCR_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DSIHOST_VVSACR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t DSIHOST_VVBPCR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t DSIHOST_VVFPCR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t DSIHOST_VVACR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t DSIHOST_LCCR_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DSIHOST_CMCR_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DSIHOST_GHCR_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DSIHOST_GPDR_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DSIHOST_GPSR_RESET_VALUE    = 0x00000015;
  static constexpr uint32_t DSIHOST_TCCRX_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t DSIHOST_CLCR_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DSIHOST_CLTCR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t DSIHOST_DLTCR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t DSIHOST_PCTLR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t DSIHOST_PCONFR_RESET_VALUE  = 0x00000001;
  static constexpr uint32_t DSIHOST_PUCR_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DSIHOST_PTTCR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t DSIHOST_PSR_RESET_VALUE     = 0x00001528;
  static constexpr uint32_t DSIHOST_ISRX_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DSIHOST_IERX_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DSIHOST_FIRX_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DSIHOST_VSCR_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DSIHOST_LCVCIDR_RESET_VALUE = 0x00000000;
  static constexpr uint32_t DSIHOST_LCCCR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t DSIHOST_LPMCCR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t DSIHOST_VMCCR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t DSIHOST_VPCCR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t DSIHOST_VCCCR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t DSIHOST_VNPCCR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t DSIHOST_VHSACCR_RESET_VALUE = 0x00000000;
  static constexpr uint32_t DSIHOST_VHBPCCR_RESET_VALUE = 0x00000000;
  static constexpr uint32_t DSIHOST_VLCCR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t DSIHOST_VVSACCR_RESET_VALUE = 0x00000000;
  static constexpr uint32_t DSIHOST_VVBPCCR_RESET_VALUE = 0x00000000;
  static constexpr uint32_t DSIHOST_VVFPCCR_RESET_VALUE = 0x00000000;
  static constexpr uint32_t DSIHOST_VVACCR_RESET_VALUE  = 0x00000000;
  static constexpr uint32_t DSIHOST_WCFGR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t DSIHOST_WCR_RESET_VALUE     = 0x00000000;
  static constexpr uint32_t DSIHOST_WIER_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DSIHOST_WISR_RESET_VALUE    = 0x00000000;
  static constexpr uint32_t DSIHOST_WIFCR_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t DSIHOST_WPCRX_RESET_VALUE   = 0x00000000;
  static constexpr uint32_t DSIHOST_WRPCR_RESET_VALUE   = 0x00000000;

  DSI_TypeDef virtualDSIHostPeripheral;
  DSIHost virtualDSIHost = DSIHost(&virtualDSIHostPeripheral);

  void SetUp() override;
  void TearDown() override;
};

void ADSIHost::SetUp()
{
  DriverTest::SetUp();

  // based on real reset values of DSI Host registers (source STM32L4R9 reference manual)
  virtualDSIHostPeripheral.VR      = DSIHOST_VR_RESET_VALUE;
  virtualDSIHostPeripheral.CR      = DSIHOST_CR_RESET_VALUE;
  virtualDSIHostPeripheral.CCR     = DSIHOST_CCR_RESET_VALUE;
  virtualDSIHostPeripheral.LVCIDR  = DSIHOST_LVCIDR_RESET_VALUE;
  virtualDSIHostPeripheral.LCOLCR  = DSIHOST_LCOLCR_RESET_VALUE;
  virtualDSIHostPeripheral.LPCR    = DSIHOST_LPCR_RESET_VALUE;
  virtualDSIHostPeripheral.LPMCR   = DSIHOST_LPMCR_RESET_VALUE;
  virtualDSIHostPeripheral.PCR     = DSIHOST_PCR_RESET_VALUE;
  virtualDSIHostPeripheral.GVCIDR  = DSIHOST_GVCIDR_RESET_VALUE;
  virtualDSIHostPeripheral.MCR     = DSIHOST_MCR_RESET_VALUE;
  virtualDSIHostPeripheral.VMCR    = DSIHOST_VMCR_RESET_VALUE;
  virtualDSIHostPeripheral.VPCR    = DSIHOST_VPCR_RESET_VALUE;
  virtualDSIHostPeripheral.VCCR    = DSIHOST_VCCR_RESET_VALUE;
  virtualDSIHostPeripheral.VNPCR   = DSIHOST_VNPCR_RESET_VALUE;
  virtualDSIHostPeripheral.VHSACR  = DSIHOST_VHSACR_RESET_VALUE;
  virtualDSIHostPeripheral.VHBPCR  = DSIHOST_VHBPCR_RESET_VALUE;
  virtualDSIHostPeripheral.VLCR    = DSIHOST_VLCR_RESET_VALUE;
  virtualDSIHostPeripheral.VVSACR  = DSIHOST_VVSACR_RESET_VALUE;
  virtualDSIHostPeripheral.VVBPCR  = DSIHOST_VVBPCR_RESET_VALUE;
  virtualDSIHostPeripheral.VVFPCR  = DSIHOST_VVFPCR_RESET_VALUE;
  virtualDSIHostPeripheral.VVACR   = DSIHOST_VVACR_RESET_VALUE;
  virtualDSIHostPeripheral.LCCR    = DSIHOST_LCCR_RESET_VALUE;
  virtualDSIHostPeripheral.CMCR    = DSIHOST_CMCR_RESET_VALUE;
  virtualDSIHostPeripheral.GHCR    = DSIHOST_GHCR_RESET_VALUE;
  virtualDSIHostPeripheral.GPDR    = DSIHOST_GPDR_RESET_VALUE;
  virtualDSIHostPeripheral.GPSR    = DSIHOST_GPSR_RESET_VALUE;
  virtualDSIHostPeripheral.TCCR[0] = DSIHOST_TCCRX_RESET_VALUE;
  virtualDSIHostPeripheral.TCCR[1] = DSIHOST_TCCRX_RESET_VALUE;
  virtualDSIHostPeripheral.TCCR[2] = DSIHOST_TCCRX_RESET_VALUE;
  virtualDSIHostPeripheral.TCCR[3] = DSIHOST_TCCRX_RESET_VALUE;
  virtualDSIHostPeripheral.TCCR[4] = DSIHOST_TCCRX_RESET_VALUE;
  virtualDSIHostPeripheral.TCCR[5] = DSIHOST_TCCRX_RESET_VALUE;
  virtualDSIHostPeripheral.CLCR    = DSIHOST_CLCR_RESET_VALUE;
  virtualDSIHostPeripheral.CLTCR   = DSIHOST_CLTCR_RESET_VALUE;
  virtualDSIHostPeripheral.DLTCR   = DSIHOST_DLTCR_RESET_VALUE;
  virtualDSIHostPeripheral.PCTLR   = DSIHOST_PCTLR_RESET_VALUE;
  virtualDSIHostPeripheral.PCONFR  = DSIHOST_PCONFR_RESET_VALUE;
  virtualDSIHostPeripheral.PUCR    = DSIHOST_PUCR_RESET_VALUE;
  virtualDSIHostPeripheral.PTTCR   = DSIHOST_PTTCR_RESET_VALUE;
  virtualDSIHostPeripheral.PSR     = DSIHOST_PSR_RESET_VALUE;
  virtualDSIHostPeripheral.ISR[0]  = DSIHOST_ISRX_RESET_VALUE;
  virtualDSIHostPeripheral.ISR[1]  = DSIHOST_ISRX_RESET_VALUE;
  virtualDSIHostPeripheral.IER[0]  = DSIHOST_IERX_RESET_VALUE;
  virtualDSIHostPeripheral.IER[1]  = DSIHOST_IERX_RESET_VALUE;
  virtualDSIHostPeripheral.FIR[0]  = DSIHOST_FIRX_RESET_VALUE;
  virtualDSIHostPeripheral.FIR[1]  = DSIHOST_FIRX_RESET_VALUE;
  virtualDSIHostPeripheral.VSCR    = DSIHOST_VSCR_RESET_VALUE;
  virtualDSIHostPeripheral.LCVCIDR = DSIHOST_LCVCIDR_RESET_VALUE;
  virtualDSIHostPeripheral.LCCCR   = DSIHOST_LCCCR_RESET_VALUE;
  virtualDSIHostPeripheral.LPMCCR  = DSIHOST_LPMCCR_RESET_VALUE;
  virtualDSIHostPeripheral.VMCCR   = DSIHOST_VMCCR_RESET_VALUE;
  virtualDSIHostPeripheral.VPCCR   = DSIHOST_VPCCR_RESET_VALUE;
  virtualDSIHostPeripheral.VCCCR   = DSIHOST_VCCCR_RESET_VALUE;
  virtualDSIHostPeripheral.VNPCCR  = DSIHOST_VNPCCR_RESET_VALUE;
  virtualDSIHostPeripheral.VHSACCR = DSIHOST_VHSACCR_RESET_VALUE;
  virtualDSIHostPeripheral.VHBPCCR = DSIHOST_VHBPCCR_RESET_VALUE;
  virtualDSIHostPeripheral.VLCCR   = DSIHOST_VLCCR_RESET_VALUE;
  virtualDSIHostPeripheral.VVSACCR = DSIHOST_VVSACCR_RESET_VALUE;
  virtualDSIHostPeripheral.VVBPCCR = DSIHOST_VVBPCCR_RESET_VALUE;
  virtualDSIHostPeripheral.VVFPCCR = DSIHOST_VVFPCCR_RESET_VALUE;
  virtualDSIHostPeripheral.VVACCR  = DSIHOST_VVACCR_RESET_VALUE;
  virtualDSIHostPeripheral.WCFGR   = DSIHOST_WCFGR_RESET_VALUE;
  virtualDSIHostPeripheral.WCR     = DSIHOST_WCR_RESET_VALUE;
  virtualDSIHostPeripheral.WIER    = DSIHOST_WIER_RESET_VALUE;
  virtualDSIHostPeripheral.WISR    = DSIHOST_WISR_RESET_VALUE;
  virtualDSIHostPeripheral.WIFCR   = DSIHOST_WIFCR_RESET_VALUE;
  virtualDSIHostPeripheral.WPCR[0] = DSIHOST_WPCRX_RESET_VALUE;
  virtualDSIHostPeripheral.WPCR[1] = DSIHOST_WPCRX_RESET_VALUE;
  virtualDSIHostPeripheral.WPCR[2] = DSIHOST_WPCRX_RESET_VALUE;
  virtualDSIHostPeripheral.WPCR[3] = DSIHOST_WPCRX_RESET_VALUE;
  virtualDSIHostPeripheral.WPCR[4] = DSIHOST_WPCRX_RESET_VALUE;
  virtualDSIHostPeripheral.WRPCR   = DSIHOST_WRPCR_RESET_VALUE;
}

void ADSIHost::TearDown()
{
  DriverTest::TearDown();
}