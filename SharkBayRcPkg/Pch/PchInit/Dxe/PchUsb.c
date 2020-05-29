/** @file
  Initializes PCH USB Controllers.

@copyright
  Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
#include "PchInit.h"

/**
  Lock USB registers before boot

  @param[in] PchPlatformPolicy    The PCH Platform Policy

  @retval None
**/
VOID
UsbInitBeforeBoot(
  IN  DXE_PCH_PLATFORM_POLICY_PROTOCOL  *PchPlatformPolicy
)
{
  EFI_STATUS                            Status;
  UINT32                                XhccCfg;
  UINTN                                 XhciPciMmBase;
  UINT32                                XhciMmioBase;
  UINT32                                Data32And;
  UINT32                                Data32Or;
  UINT32                                PchSeries;
  UINT16                                OrgCommandWord;
  BOOLEAN                               NeedGcdMemSpace;

  Data32And       = 0xFFFFFFFF;
  Data32Or        = 0x0;
  NeedGcdMemSpace = FALSE;

  if (PchPlatformPolicy->UsbConfig->Usb30Settings.Mode == PCH_XHCI_MODE_OFF) {
    return;
  }

  XhciPciMmBase   = MmPciAddress (
                      0,
                      PchPlatformPolicy->BusNumber,
                      PCI_DEVICE_NUMBER_PCH_XHCI,
                      PCI_FUNCTION_NUMBER_PCH_XHCI,
                      0
                      );
  XhciMmioBase = MmioRead32(XhciPciMmBase + R_PCH_XHCI_MEM_BASE) & ~(0x0F);
  if(XhciMmioBase == 0){
    ///
    /// Allocate GCD mem space
    ///
    XhciMmioBase = 0xFFFFFFFF;
    Status = gDS->AllocateMemorySpace (
                EfiGcdAllocateMaxAddressSearchBottomUp,
                    EfiGcdMemoryTypeMemoryMappedIo,
                    N_PCH_XHCI_MEM_ALIGN,
                    V_PCH_XHCI_MEM_LENGTH,
                    (EFI_PHYSICAL_ADDRESS *)&XhciMmioBase,
                    mImageHandle,
                    NULL
                    );
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      return;
    }
    NeedGcdMemSpace = TRUE;
    MmioWrite32 (XhciPciMmBase + R_PCH_XHCI_MEM_BASE, XhciMmioBase);
  }
  PchSeries = GetPchSeries();

  ///
  ///Restore xHCI MMIO Enable
  ///
  OrgCommandWord = MmioRead16 (XhciPciMmBase + R_PCH_XHCI_COMMAND_REGISTER);
  MmioOr16 (
    XhciPciMmBase + R_PCH_XHCI_COMMAND_REGISTER,
    (UINT16) (B_PCH_XHCI_COMMAND_MSE | B_PCH_XHCI_COMMAND_BME)
    );
  PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint16,
    (UINTN) (XhciPciMmBase + R_PCH_XHCI_COMMAND_REGISTER),
    1,
    (VOID *) (UINTN) (XhciPciMmBase + R_PCH_XHCI_COMMAND_REGISTER)
    );

  ///
  ///Restore xHCI BAR
  ///
  PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (XhciPciMmBase + R_PCH_XHCI_MEM_BASE),
    1,
    (VOID *) (UINTN) (XhciPciMmBase + R_PCH_XHCI_MEM_BASE)
    );

  if (PchSeries == PchH) {
    ///
    /// For LPT-H, Set xHCIBAR + 8144h[8, 7, 6] to 1b, 0b, 0b
    ///
    MmioAndThenOr32 (XhciMmioBase + 0x8144, (UINT32) ~(BIT7 | BIT6), (UINT32) (BIT8));
    PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (XhciMmioBase + 0x8144),
      1,
      (VOID *) (UINTN) (XhciMmioBase + 0x8144)
      );
  } else if (PchSeries == PchLp) {
    ///
    /// For LPT-LP, Set xHCIBAR + 8144h[8, 7, 6] to 1b, 1b, 1b
    ///
    MmioOr32 (XhciMmioBase + 0x8144, (UINT32) (BIT8 | BIT7 | BIT6));
    PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (XhciMmioBase + 0x8144),
      1,
      (VOID *) (UINTN) (XhciMmioBase + 0x8144)
      );
    ///
    /// For LPT-LP, Set xHCIBAR + 816Ch[19:0] to 000E0038h
    ///
    Data32And = (UINT32) ~(0x000FFFFF);
    Data32Or  = (UINT32) (0x000E0038);
    MmioAndThenOr32 (
     (XhciMmioBase + 0x816C),
      Data32And,
      Data32Or
      );
    PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (XhciMmioBase + 0x816C),
      1,
      (VOID *) (UINTN) (XhciMmioBase + 0x816C)
      );
    ///
    /// For LPT-LP, Set D20:F0:B0h[17,14,13] to 1b, 0b, 0b
    ///
    MmioAndThenOr32 (XhciPciMmBase + 0xB0, (UINT32) ~(BIT14 | BIT13),  (UINT32) (BIT17));
    PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (XhciPciMmBase + 0xB0),
      1,
      (VOID *) (UINTN) (XhciPciMmBase + 0xB0)
      );
  }

  ///
  /// Set D20:F0:50h[28:0] to 0FCE2E5F for LPT-LP
  /// Set D20:F0:50h[26:0] to 07886E9Fh for LPT-H B0 onward
  ///
  if (PchSeries == PchH) {
    Data32And = (UINT32)~(0x07FFFFFF);
    Data32Or  = (UINT32) (0x07886E9F);
  } else if (PchSeries == PchLp) {
    Data32And = (UINT32) ~(0x1FFFFFFF);
    Data32Or  = (UINT32) (0x0FCE2E5F);
  }
  MmioAndThenOr32 (
   (XhciPciMmBase + 0x50),
    Data32And,
    Data32Or
    );
  PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (XhciPciMmBase + 0x50),
    1,
    (VOID *) (UINTN) (XhciPciMmBase + 0x50)
    );

  if ((GetBootModeHob () == BOOT_ON_S4_RESUME) &&
      (PchPlatformPolicy->UsbConfig->UsbPrecondition == PCH_DEVICE_ENABLE)) {
    ///
    /// For LPT-LP, Set xHCIBAR + 80E0[24] to 1h
    ///
    MmioOr32 (XhciMmioBase + 0x80E0, (UINT32) (BIT24));

    ///
    /// For LPT-LP, Set xHCIBAR + 80E0[24] to 0h
    ///
    MmioAnd32 (XhciMmioBase + 0x80E0, (UINT32) ~(BIT24));
  }

  ///
  /// PCH BIOS Spec xHCI controller setup
  /// Note:
  /// D20:F0:40h is write once register.
  /// Unsupported Request Detected bit is write clear
  ///
  XhccCfg = MmioRead32 (XhciPciMmBase + R_PCH_XHCI_XHCC1);
  XhccCfg &= (UINT32) ~(B_PCH_XHCI_XHCC1_URD);
  ///
  /// PCH BIOS Spec Rev 0.5.5, Section 13.2.4 Locking xHCI Register Settings
  /// PCH BIOS Spec Locking xHCI Register settings
  /// After xHCI is initialized, BIOS should lock the xHCI configuration registers to RO.
  /// This prevent any unintended changes. There is also a lockdown feature for OverCurrent
  /// registers. BIOS should set these bits to lock down the settings prior to end of POST.
  /// 1. Set Access Control bit at D20:F0:40h[31] to 1b to lock xHCI register settings.
  /// 2. Set OC Configuration Done bit at D20:F0:44h[31] to lock overcurrent mappings from
  ///    further changes.
  ///
  MmioOr32 (XhciPciMmBase + 0x44, (UINT32) (BIT31));
  PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (XhciPciMmBase + 0x44),
    1,
    (VOID *) (UINTN) (XhciPciMmBase + 0x44)
    );
  XhccCfg |= (UINT32) (B_PCH_XHCI_XHCC1_ACCTRL);
  MmioWrite32 (XhciPciMmBase + R_PCH_XHCI_XHCC1, XhccCfg);
  PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (XhciPciMmBase + R_PCH_XHCI_XHCC1),
    1,
    &XhccCfg
    );

  ///
  ///restore xHCI original command byte
  ///
  MmioWrite16 ((XhciPciMmBase + R_PCH_XHCI_COMMAND_REGISTER), OrgCommandWord);
  PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint16,
    (UINTN) (XhciPciMmBase + R_PCH_XHCI_COMMAND_REGISTER),
    1,
    (VOID *) (UINTN) (XhciPciMmBase + R_PCH_XHCI_COMMAND_REGISTER)
    );

  if (NeedGcdMemSpace) {
    MmioWrite32 (XhciPciMmBase + R_PCH_XHCI_MEM_BASE, 0);
    ///
    ///clear xHCI BAR
    ///
    PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (XhciPciMmBase + R_PCH_XHCI_MEM_BASE),
      1,
      (VOID *) (UINTN) (XhciPciMmBase + R_PCH_XHCI_MEM_BASE)
      );
    ///
    /// release GCD Mem space
    ///
    gDS->FreeMemorySpace (
           XhciMmioBase,
           V_PCH_XHCI_MEM_LENGTH
           );
  }

}

/**
  Configures ports of the PCH USB3 (xHCI) controller
  just before OS boot.

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance

  @retval EFI_INVALID_PARAMETER   The parameter of PchPlatformPolicy is invalid
  @retval EFI_SUCCESS             The function completed successfully
**/
VOID
ConfigureXhciAtBoot (
  IN DXE_PCH_PLATFORM_POLICY_PROTOCOL *PchPlatformPolicy
  )
{
  UINTN   PciD20F0RegBase;
  UINT32  PortMask;

  DEBUG ((EFI_D_INFO, "ConfigureXhciAtBoot() Start\n"));

  PciD20F0RegBase = MmPciAddress (
                      0,
                      PchPlatformPolicy->BusNumber,
                      PCI_DEVICE_NUMBER_PCH_XHCI,
                      PCI_FUNCTION_NUMBER_PCH_XHCI,
                      0
                      );

  ///
  /// PCH BIOS Spec Rev 0.5.0
  /// When the BIOS does not have xHCI pre-boot software available:
  /// Section 13.1.1.2 xHCI Enabled mode
  /// BIOS should route the Ports to the EHCI controller and prior to OS boot
  /// it should route the ports to the xHCI controller.
  ///
  if ((PchPlatformPolicy->UsbConfig->Usb30Settings.Mode == PCH_XHCI_MODE_ON) &&
      (PchPlatformPolicy->UsbConfig->Usb30Settings.PreBootSupport == PCH_DEVICE_DISABLE)) {
    ///
    /// PCH BIOS Spec Rev 0.5.0 Section 13.2.6 Routing of switchable USB Ports to
    /// xHCI Controller
    /// Step 1
    /// Done in GetXhciPortsNumber()
    /// Step 2
    /// Program D20:F0:D8h[5:0] to the value of xHCI D20:F0:DCh[5:0]
    ///
    PortMask = MmioRead32 (PciD20F0RegBase + R_PCH_XHCI_USB3PRM);

    MmioAndThenOr32 (
      PciD20F0RegBase + R_PCH_XHCI_USB3PR,
      (UINT32)~B_PCH_XHCI_USB3PR_USB3SSEN,
      PortMask
      );
    ///
    /// Step 3
    /// Program D20:F0:D0h[14:0] to the value of xHCI D20:F0:D4h[15:0]
    ///
    PortMask = MmioRead32 (PciD20F0RegBase + R_PCH_XHCI_USB2PRM);

    MmioAndThenOr32 (
      PciD20F0RegBase + R_PCH_XHCI_USB2PR,
      (UINT32)~B_PCH_XHCI_USB2PR_USB2HCSEL,
      PortMask
      );
    ///
    /// Note: Registers USB3PR[5:0] and USB2PR[14:0] are located in SUS well so BIOS doesn't
    ///       need to restore them during S3 resume, but needs to restore corresponding mask
    ///       registers. For RapidStart resume from G3 state support, HC Switch driver will call
    ///       _OSC method to restore USB2PR and USB3PR.
  }
  
  DEBUG ((EFI_D_INFO, "ConfigureXhciAtBoot() End\n"));
}

/**
  Configures PCH USB controller

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] RootComplexBar       RootComplexBar address of this PCH device
  @param[in, out] FuncDisableReg  Function Disable Register

  @retval EFI_INVALID_PARAMETER   The parameter of PchPlatformPolicy is invalid
  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
ConfigureUsb (
  IN      DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy,
  IN      UINT32                              RootComplexBar,
  IN OUT  UINT32                              *FuncDisableReg
  )
{
  EFI_STATUS            Status;
  UINT8                 BusNumber;
  PCH_USB_CONFIG        *UsbConfig;
  UINT32                UsbFuncDisable;
  EFI_PHYSICAL_ADDRESS  EhciMemBaseAddress;
  EFI_PHYSICAL_ADDRESS  XhciMemBaseAddress;

  DEBUG ((EFI_D_INFO, "ConfigureUsb() Start\n"));

  BusNumber          = PchPlatformPolicy->BusNumber;
  UsbConfig          = PchPlatformPolicy->UsbConfig;
  EhciMemBaseAddress = 0x0ffffffff;

  Status = gDS->AllocateMemorySpace (
                  EfiGcdAllocateMaxAddressSearchBottomUp,
                  EfiGcdMemoryTypeMemoryMappedIo,
                  N_PCH_EHCI_MEM_ALIGN,
                  V_PCH_EHCI_MEM_LENGTH,
                  &EhciMemBaseAddress,
                  mImageHandle,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  XhciMemBaseAddress = 0x0ffffffff;

  Status = gDS->AllocateMemorySpace (
                  EfiGcdAllocateMaxAddressSearchBottomUp,
                  EfiGcdMemoryTypeMemoryMappedIo,
                  N_PCH_XHCI_MEM_ALIGN,
                  V_PCH_XHCI_MEM_LENGTH,
                  &XhciMemBaseAddress,
                  mImageHandle,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {

    gDS->FreeMemorySpace (
          EhciMemBaseAddress,
          V_PCH_EHCI_MEM_LENGTH
          );

    return Status;
  }

  UsbFuncDisable = *FuncDisableReg;

  Status = CommonUsbInit (
            UsbConfig,
            (UINT32) EhciMemBaseAddress,
            (UINT32) XhciMemBaseAddress,
            BusNumber,
            RootComplexBar,
            &UsbFuncDisable,
            PchPlatformPolicy->Revision
            );
  *FuncDisableReg = UsbFuncDisable;

  //
  // Free allocated resources
  //
  gDS->FreeMemorySpace (
        EhciMemBaseAddress,
        V_PCH_EHCI_MEM_LENGTH
        );

  gDS->FreeMemorySpace (
        XhciMemBaseAddress,
        V_PCH_XHCI_MEM_LENGTH
        );
  DEBUG ((EFI_D_INFO, "ConfigureUsb() End\n"));

  return EFI_SUCCESS;
}
