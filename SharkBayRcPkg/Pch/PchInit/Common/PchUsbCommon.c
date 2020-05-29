/** @file
  Initializes PCH USB Controllers.

@copyright
  Copyright (c) 2009 - 2013 Intel Corporation. All rights reserved
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
#include "PchUsbCommon.h"

const USB_CONTROLLER EhciControllersMap[PchEhciControllerMax] = {
  {
    PCI_DEVICE_NUMBER_PCH_USB,
    PCI_FUNCTION_NUMBER_PCH_EHCI
  },
  {
    PCI_DEVICE_NUMBER_PCH_USB_EXT,
    PCI_FUNCTION_NUMBER_PCH_EHCI2
  }
};

UINTN PCH_H_PORTSCxUSB2[] = {
  R_PCH_XHCI_PORTSC01USB2,
  R_PCH_XHCI_PORTSC02USB2,
  R_PCH_XHCI_PORTSC03USB2,
  R_PCH_XHCI_PORTSC04USB2,
  R_PCH_XHCI_PORTSC05USB2,
  R_PCH_XHCI_PORTSC06USB2,
  R_PCH_XHCI_PORTSC07USB2,
  R_PCH_XHCI_PORTSC08USB2,
  R_PCH_XHCI_PORTSC09USB2,
  R_PCH_H_XHCI_PORTSC10USB2,
  R_PCH_H_XHCI_PORTSC11USB2,
  R_PCH_H_XHCI_PORTSC12USB2,
  R_PCH_H_XHCI_PORTSC13USB2,
  R_PCH_H_XHCI_PORTSC14USB2,
  R_PCH_H_XHCI_PORTSC15USB2
};

UINTN PCH_LP_PORTSCxUSB2[] = {
  R_PCH_XHCI_PORTSC01USB2,
  R_PCH_XHCI_PORTSC02USB2,
  R_PCH_XHCI_PORTSC03USB2,
  R_PCH_XHCI_PORTSC04USB2,
  R_PCH_XHCI_PORTSC05USB2,
  R_PCH_XHCI_PORTSC06USB2,
  R_PCH_XHCI_PORTSC07USB2,
  R_PCH_XHCI_PORTSC08USB2,
  R_PCH_XHCI_PORTSC09USB2
};

UINTN PCH_H_PORTSCxUSB3[] = {
  R_PCH_H_XHCI_PORTSC1USB3,
  R_PCH_H_XHCI_PORTSC2USB3,
  R_PCH_H_XHCI_PORTSC3USB3,
  R_PCH_H_XHCI_PORTSC4USB3,
  R_PCH_H_XHCI_PORTSC5USB3,
  R_PCH_H_XHCI_PORTSC6USB3
};

UINTN PCH_LP_PORTSCxUSB3[] = {
  R_PCH_LP_XHCI_PORTSC1USB3,
  R_PCH_LP_XHCI_PORTSC2USB3,
  R_PCH_LP_XHCI_PORTSC3USB3,
  R_PCH_LP_XHCI_PORTSC4USB3
};

///
///  Table: USB2 Pins Mapping between XHCI/EHCI Port
///   -------------------------------------------
///  | USB2 Pin     | EHCI Port      | XHCI Port |
///  |--------------+----------------+-----------|
///  | USB[P,N][0]  | EHCI 1 Port 0  | Port 0    |
///  | USB[P,N][1]  | EHCI 1 Port 1  | Port 1    |
///  | USB[P,N][2]  | EHCI 1 Port 2  | Port 2    |
///  | USB[P,N][3]  | EHCI 1 Port 3  | Port 3    |
///  | USB[P,N][4]  | EHCI 1 Port 4  | Port 8    |
///  | USB[P,N][5]  | EHCI 1 Port 5  | Port 9    |
///  | USB[P,N][6]  | EHCI 1 Port 6  | Port 12   |
///  | USB[P,N][7]  | EHCI 1 Port 7  | Port 13   |
///  | USB[P,N][8]  | EHCI 2 Port 8  | Port 4    |
///  | USB[P,N][9]  | EHCI 2 Port 9  | Port 5    |
///  | USB[P,N][10] | EHCI 2 Port 10 | Port 6    |
///  | USB[P,N][11] | EHCI 2 Port 11 | Port 7    |
///  | USB[P,N][12] | EHCI 2 Port 12 | Port 10   |
///  | USB[P,N][13] | EHCI 2 Port 13 | Port 11   |
///   -------------------------------------------
///
const UINT32 XhciUsb2InternalPortNumberLookUpTable[] = {
  0,1,2,3,8,9,12,13,4,5,6,7,10,11,12,13
};

/**
  Configures PCH USB controller

  @param[in] UsbConfig            The PCH Platform Policy for USB configuration
  @param[in] EhciMmioBase         Memory base address of EHCI Controller
  @param[in] XhciMmioBase         Memory base address of XHCI Controller
  @param[in] BusNumber            PCI Bus Number of the PCH device
  @param[in] RootComplexBar       RootComplexBar value of this PCH device
  @param[out] FuncDisableReg      Function Disable Register
  @param[in] Revision             The policy revision used for backward compatible check

  @retval EFI_INVALID_PARAMETER   The parameter of PchPlatformPolicy is invalid
  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
EFIAPI
CommonUsbInit (
  IN  PCH_USB_CONFIG              *UsbConfig,
  IN  UINT32                      EhciMmioBase,
  IN  UINT32                      XhciMmioBase,
  IN  UINT8                       BusNumber,
  IN  UINT32                      RootComplexBar,
  OUT UINT32                      *FuncDisableReg,
  IN  UINT8                       Revision
  )
{
  UINTN           PciD31F0RegBase;
  UINTN           XhciPciMmBase;
  UINTN           Ehci1PciMmBase;
  UINTN           Ehci2PciMmBase;
  UINT16          LpcDeviceId;
  UINT16          PmBase;
  UINT16          RegData16;
  PCH_SERIES      PchSeries;

  DEBUG ((EFI_D_INFO, "CommonUsbInit() - Start\n"));

  PchSeries       = GetPchSeries();
  PciD31F0RegBase = MmPciAddress (
                      0,
                      BusNumber,
                      PCI_DEVICE_NUMBER_PCH_LPC,
                      PCI_FUNCTION_NUMBER_PCH_LPC,
                      0
                      );
  XhciPciMmBase   = MmPciAddress (
                      0,
                      BusNumber,
                      PCI_DEVICE_NUMBER_PCH_XHCI,
                      PCI_FUNCTION_NUMBER_PCH_XHCI,
                      0
                      );
  Ehci1PciMmBase  = MmPciAddress (
                      0,
                      BusNumber,
                      EhciControllersMap[PchEhci1].Device,
                      EhciControllersMap[PchEhci1].Function,
                      0
                      );
  Ehci2PciMmBase = 0;
  if (PchSeries == PchH) {
    Ehci2PciMmBase  = MmPciAddress (
                        0,
                        BusNumber,
                        EhciControllersMap[PchEhci2].Device,
                        EhciControllersMap[PchEhci2].Function,
                        0
                        );
  }

  PmBase = MmioRead16 (PciD31F0RegBase + R_PCH_LPC_ACPI_BASE) & B_PCH_LPC_ACPI_BASE_BAR;
  LpcDeviceId = MmioRead16 (PciD31F0RegBase + R_PCH_LPC_DEVICE_ID);
  ///
  /// Check to disable USB Controllers
  ///
  if (UsbConfig->Usb20Settings[PchEhci1].Enable == PCH_DEVICE_DISABLE) {
    *FuncDisableReg |= B_PCH_RCRB_FUNC_DIS_EHCI1;
  }

  if (PchSeries == PchH) {
    if (UsbConfig->Usb20Settings[PchEhci2].Enable == PCH_DEVICE_DISABLE) {
      *FuncDisableReg |= B_PCH_RCRB_FUNC_DIS_EHCI2;
    }
  }

  if (UsbConfig->Usb30Settings.Mode == PCH_XHCI_MODE_OFF) {
    ///
    /// PCH BIOS Spec Rev 0.5.0, section 13.2.3 Hiding/Disabling xHCI Controller
    /// In some instances, the System BIOS may choose to "hide" the xHCI controller. When
    /// the xHCI device is hidden, all high speed shareable ports should be routed to the EHCI
    /// controller to avoid the situation where USB ports are not functioning. To hide a host
    /// controller, the BIOS must program the Function Disable register (RCBA + 3418h). See
    /// the PCH EDS for a description of the register.
    ///
    *FuncDisableReg |= B_PCH_RCRB_FUNC_DIS_XHCI;
  }

  ///
  /// Init USB Host Controllers
  ///
  CommonEhciHcsInit (
    UsbConfig,
    EhciMmioBase,
    BusNumber,
    Revision,
    LpcDeviceId,
    RootComplexBar
    );

  ///
  /// Assign memory resources
  ///
  XhciMemorySpaceOpen (
    UsbConfig,
    XhciMmioBase,
    XhciPciMmBase
    );

  CommonXhciHcInit (
    UsbConfig,
    XhciMmioBase,
    Revision,
    LpcDeviceId,
    XhciPciMmBase
    );

  ///
  /// Init Port Switching Flow
  ///
  PerformXhciEhciPortSwitchingFlow (
    UsbConfig,
    XhciMmioBase,
    Revision,
    LpcDeviceId,
    XhciPciMmBase,
    PciD31F0RegBase
    );
  ///
  /// Setup USB Over-Current Mapping.
  ///
  EhciOverCurrentMapping (
    UsbConfig,
    Ehci1PciMmBase,
    Ehci2PciMmBase
    );

  XhciOverCurrentMapping (
    UsbConfig,
    XhciPciMmBase
    );

  //
  // Tune the USB 2.0 high-speed signals quality.
  //
  Usb2PortLengthProgramming (
    UsbConfig,
    LpcDeviceId,
    RootComplexBar
    );

  ///
  /// Support USB Per-Port Disable Control Override feature
  ///
  if (UsbConfig->UsbPerPortCtl == PCH_DEVICE_ENABLE) {
    ///
    /// PCH BIOS Spec Rev 0.5.0 Section 12.2 Disabling USB Ports
    /// The PCH USB Port Disable Override Register (D26/29:F0 + 64h) can be locked by setting
    /// the Write Enable bit of the PCH USB Per-Port Register Write Control Register,
    /// PMBASE + 3Ch[1].
    ///
    /// Open the Per-Port Disable Control Override
    ///
    RegData16 = IoRead16 ((UINTN) ((UINT64) (PmBase + R_PCH_UPRWC)));
    RegData16 |= B_PCH_UPRWC_WR_EN;
    IoWrite16 ((UINTN) ((UINT64) (PmBase + R_PCH_UPRWC)), RegData16);
#ifdef SUS_WELL_RESTORE
    ///
    /// To support RapidStart resume from G3 state, all resume well registers need to be saved
    /// into S3 Script table.
    ///
    PCH_INIT_COMMON_SCRIPT_IO_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint16,
      (UINTN) (PmBase + R_PCH_UPRWC),
      1,
      &RegData16
      );
#endif

    EhciPortDisableOverride (
      UsbConfig,
      Ehci1PciMmBase,
      Ehci2PciMmBase
      );

    XhciPortDisableOverride (
      UsbConfig,
      XhciPciMmBase,
      Revision
      );

    ///
    /// Close the Per-Port Disable Control Override
    ///
    RegData16 &= (~B_PCH_UPRWC_WR_EN);
    IoWrite16 ((UINTN) ((UINT64) (PmBase + R_PCH_UPRWC)), RegData16);
#ifdef SUS_WELL_RESTORE
    ///
    /// To support RapidStart resume from G3 state, all resume well registers need to be saved
    /// into S3 Script table.
    ///
    PCH_INIT_COMMON_SCRIPT_IO_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint16,
      (UINTN) (PmBase + R_PCH_UPRWC),
      1,
      &RegData16
      );
#endif
  }
  ///
  /// Support USBR feature
  ///
  if (UsbConfig->Ehci1Usbr == PCH_DEVICE_ENABLE &&
      UsbConfig->Usb20Settings[PchEhci1].Enable == PCH_DEVICE_ENABLE) {
    EhciUsbrEnable (Ehci1PciMmBase);
  }
  if (PchSeries == PchH) {
    if (UsbConfig->Ehci2Usbr == PCH_DEVICE_ENABLE && UsbConfig->Usb20Settings[PchEhci2].Enable == PCH_DEVICE_ENABLE) {
      EhciUsbrEnable (Ehci2PciMmBase);
    }
  }
  ///
  /// Clear memory resources
  ///
  XhciMemorySpaceClose (
    UsbConfig,
    XhciMmioBase,
    XhciPciMmBase
    );

  DEBUG ((EFI_D_INFO, "CommonUsbInit() - End\n"));

  return EFI_SUCCESS;
}

/**
  Performs basic configuration of PCH EHCI controller.

  @param[in] UsbConfig            The PCH Platform Policy for USB configuration
  @param[in] EhciMmioBase         Memory base address of EHCI Controller
  @param[in] BusNumber            PCI Bus Number of the PCH device
  @param[in] Revision             The policy revision used for backward compatible check
  @param[in] LpcDeviceId          The device ID of LPC
  @param[in] RootComplexBar       RootComplexBar value of this PCH device

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
EFIAPI
CommonEhciHcsInit (
  IN  PCH_USB_CONFIG              *UsbConfig,
  IN  UINT32                      EhciMmioBase,
  IN  UINT8                       BusNumber,
  IN  UINT8                       Revision,
  IN  UINT16                      LpcDeviceId,
  IN  UINT32                      RootComplexBar
  )
{
  UINTN           EhciPciMmBase;
  UINT8           Index;
  UINT16          PciCmd;
  BOOLEAN         SkipRst;
  UINT32          DwordReg;
  UINT8           NumberOfPorts;
  EFI_STATUS      Status;
  UINT32          Data32And;
  UINT32          Data32Or;
  PCH_SERIES      PchSeries;

  PchSeries = GetPchSeries();

  for (Index = 0; Index < GetPchEhciMaxControllerNum (); Index++) {
    EhciPciMmBase = MmPciAddress (
                      0,
                      BusNumber,
                      EhciControllersMap[Index].Device,
                      EhciControllersMap[Index].Function,
                      0
                      );
    ///
    /// Set EHCI structural parameter
    ///
    if (UsbConfig->Usb20Settings[Index].Enable == PCH_DEVICE_DISABLE) {
      MmioWrite32 (EhciPciMmBase + R_PCH_EHCI_MEM_BASE, 0);
      MmioWrite16 (EhciPciMmBase + R_PCH_EHCI_COMMAND_REGISTER, 0);
    } else {
      PciCmd = 0;
      ///
      /// Shared EHCI/XHCI ports w/a.
      /// This step is required when some of the ports are routed to EHCI 
      /// and other ports are routed XHCI at the same time.
      ///
      /// Clear D26/D29:F0 + 78h [1:0]
      ///
      if (UsbConfig->Usb30Settings.ManualMode == PCH_DEVICE_ENABLE) {
        MmioAnd16 (
          (EhciPciMmBase + 0x78),
          (UINT16) ~(BIT1 | BIT0));
#ifdef SUS_WELL_RESTORE
        ///
        /// To support DeepSx and RapidStart resume from G3 state, all resume well registers
        /// need to be saved into S3 Script table.
        ///
        PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
          EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
          EfiBootScriptWidthUint16,
          (UINTN) (EhciPciMmBase + 0x78),
          1,
          (VOID *) (UINTN) (EhciPciMmBase + 0x78)
          );
#endif
      }
      ///
      /// For some cases, like usb debug mode, the Ehci memory resource will have been assigned and
      /// enabled here. If so, then set SkipRst flag to skip the steps that are for Ehci memory
      /// resource clear and host controller reset
      ///
      if ((MmioRead32 (EhciPciMmBase + R_PCH_EHCI_MEM_BASE) == 0) &&
          !(MmioRead16 (EhciPciMmBase + R_PCH_EHCI_COMMAND_REGISTER) & B_PCH_EHCI_COMMAND_MSE)) {
        MmioWrite32 ((EhciPciMmBase + R_PCH_EHCI_MEM_BASE), EhciMmioBase);
#ifdef SUS_WELL_RESTORE
        ///
        /// To support RapidStart resume from G3 state, all resume well registers need to be saved
        /// into S3 Script table.
        ///
        PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
          EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
          EfiBootScriptWidthUint32,
          (UINTN) (EhciPciMmBase + R_PCH_EHCI_MEM_BASE),
          1,
          (VOID *) (UINTN) (EhciPciMmBase + R_PCH_EHCI_MEM_BASE)
          );
#endif
        ///
        /// Clear SkipRst flag
        ///
        SkipRst = FALSE;
      } else {
        ///
        /// Use the memory address of Ehci controller that has been assigned before initialization
        /// to do the programming.
        ///
        EhciMmioBase = MmioRead32 (EhciPciMmBase + R_PCH_EHCI_MEM_BASE);
        ///
        /// Save Pci command register
        ///
        PciCmd = MmioRead16 (EhciPciMmBase + R_PCH_EHCI_COMMAND_REGISTER);
        ///
        /// Set SkipRst flag
        ///
        SkipRst = TRUE;
      }

      MmioOr16 (
        (EhciPciMmBase + R_PCH_EHCI_COMMAND_REGISTER),
        (UINT16) (B_PCH_EHCI_COMMAND_MSE | B_PCH_EHCI_COMMAND_BME)
        );
#ifdef SUS_WELL_RESTORE
      ///
      /// To support RapidStart resume from G3 state, all resume well registers need to be saved
      /// into S3 Script table.
      ///
      PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint16,
        (UINTN) (EhciPciMmBase + R_PCH_EHCI_COMMAND_REGISTER),
        1,
        (VOID *) (UINTN) (EhciPciMmBase + R_PCH_EHCI_COMMAND_REGISTER)
        );
#endif
      ///
      /// PCH BIOS Spec Rev 0.5.0 Section 12.10
      /// Additional Programming Requirements during USB Initialization
      ///
      /// Step 1
      /// Program D29/D26:MEM_BASE + 20h [1] = 1b,
      /// This should be done before FS/LS initialitiation and also after S4/S5
      ///
      /// For some cases, like usb debug mode, we will skip this step, in case something will be destroyed
      /// after doing host controller reset
      ///
      if (!SkipRst) {
#ifdef USB_PRECONDITION_ENABLE_FLAG
        ///
        /// Reset the EHCI when running in PEI phase where USB precondition feature is enabled
        /// or in DXE phase where USB precondition feature is disabled
        /// If the precondition is enabled and running in DXE phase, EHCI has reset done already
        ///
        if (USB_RUN_IN_PEI || (!USB_PRECONDITION_POLICY_SUPPORT (UsbConfig))) {
#endif  // USB_PRECONDITION_ENABLE_FLAG
          MmioOr16 ((EhciMmioBase + R_PCH_EHCI_USB2CMD), B_PCH_EHCI_USB2CMD_HCRESET);
#ifdef USB_PRECONDITION_ENABLE_FLAG
        }
#endif  // USB_PRECONDITION_ENABLE_FLAG
      }
      ///
      /// Step 2
      /// Configure number of controller and port:
      ///
      /// Step 2.a
      /// Set D26/D29:F0:80h [0] = 1b
      ///
      MmioOr16 ((EhciPciMmBase + R_PCH_EHCI_ACCESS_CNTL), (UINT16) V_PCH_EHCI_ACCESS_CNTL_ENABLE);
#ifdef SUS_WELL_RESTORE
      ///
      /// To support RapidStart resume from G3 state, all resume well registers need to be saved
      /// into S3 Script table.
      ///
      PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint16,
        (UINTN) (EhciPciMmBase + R_PCH_EHCI_ACCESS_CNTL),
        1,
        (VOID *) (UINTN) (EhciPciMmBase + R_PCH_EHCI_ACCESS_CNTL)
        );
#endif
      ///
      /// Step 2.b
      /// Set both EHCI's N_CC bit, D26 & D29 MEM_BASE + offset 04h [15:12], to 0000b
      ///
      MmioBitFieldWrite32 (
        (EhciMmioBase + R_PCH_EHCI_HCSPARAMS),
        N_PCH_EHCI_HCSPARAMS_N_CC,
        (N_PCH_EHCI_HCSPARAMS_N_CC + 3),
        0
        );
#ifdef SUS_WELL_RESTORE
      ///
      /// To support RapidStart resume from G3 state, all resume well registers need to be saved
      /// into S3 Script table.
      ///
      PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (EhciMmioBase + R_PCH_EHCI_HCSPARAMS),
        1,
        (VOID *) (UINTN) (EhciMmioBase + R_PCH_EHCI_HCSPARAMS)
        );
#endif
      ///
      /// Step 2.c
      /// Set both EHCI's N_PORTS bit, D26 & D29 MEM_BASE + offset 04h [3:0], to 2h
      ///
      NumberOfPorts = 2;
      if (Index == PchEhci1) {
        if (UsbConfig->Ehci1Usbr == PCH_DEVICE_ENABLE) {
          NumberOfPorts = NumberOfPorts + 1;
        }
      } else {
        if (PchSeries == PchH) {
          if (Index == PchEhci2) {
            if (UsbConfig->Ehci2Usbr == PCH_DEVICE_ENABLE) {
              NumberOfPorts = NumberOfPorts + 1;
            }
          }
        }
      }
      MmioBitFieldWrite32 (
        (EhciMmioBase + R_PCH_EHCI_HCSPARAMS),
        N_PCH_EHCI_HCSPARAMS_N_PORTS,
        (N_PCH_EHCI_HCSPARAMS_N_PORTS + 3),
        NumberOfPorts
        );
#ifdef SUS_WELL_RESTORE
      ///
      /// To support RapidStart resume from G3 state, all resume well registers need to be saved
      /// into S3 Script table.
      ///
      PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (EhciMmioBase + R_PCH_EHCI_HCSPARAMS),
        1,
        (VOID *) (UINTN) (EhciMmioBase + R_PCH_EHCI_HCSPARAMS)
        );
#endif
      ///
      /// Step 2.d
      /// Clear D26/D29:F0:80h [0] to 0b
      ///
      MmioAnd16 ((EhciPciMmBase + R_PCH_EHCI_ACCESS_CNTL), (UINT16) (~V_PCH_EHCI_ACCESS_CNTL_ENABLE));
#ifdef SUS_WELL_RESTORE
      ///
      /// To support RapidStart resume from G3 state, all resume well registers need to be saved
      /// into S3 Script table.
      ///
      PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint16,
        (UINTN) (EhciPciMmBase + R_PCH_EHCI_ACCESS_CNTL),
        1,
        (VOID *) (UINTN) (EhciPciMmBase + R_PCH_EHCI_ACCESS_CNTL)
        );
#endif
      ///
      /// Step 3
      /// Program D29/D26:F0 + 78h[2] = 1b.
      ///
      DwordReg = MmioRead32 (EhciPciMmBase + 0x78);
      DwordReg |= (UINT32) (BIT2);
      MmioWrite32 (
        (UINTN) (EhciPciMmBase + 0x78),
        DwordReg
        );
#ifdef SUS_WELL_RESTORE
      ///
      /// To support RapidStart resume from G3 state, all resume well registers need to be saved
      /// into S3 Script table.
      ///
      PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (EhciPciMmBase + 0x78),
        1,
        (VOID *) (UINTN) (EhciPciMmBase + 0x78)
        );
#endif
      ///
      /// Step 4
      /// Program D29/D26:F0 + 7Ch[14,7] = 1b
      ///
      MmioOr32 (EhciPciMmBase + 0x7C, (UINT32) BIT14 | BIT7);
#ifdef SUS_WELL_RESTORE
      ///
      /// To support RapidStart resume from G3 state, all resume well registers need to be saved
      /// into S3 Script table.
      ///
      PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (EhciPciMmBase + 0x7C),
        1,
        (VOID *) (UINTN) (EhciPciMmBase + 0x7C)
        );
#endif
      ///
      /// Step 5
      /// Program D29/D26:F0 + 8Ch[11:8] = 0100b
      /// Step 6
      /// Program D29/D26:F0 + 8Ch[26,17] = 0b, 1b
      ///
      DwordReg = MmioRead32 (EhciPciMmBase + 0x8C);
      DwordReg |= (UINT32) (BIT17 | BIT10);
      DwordReg &= (UINT32)~(BIT26 | BIT11 | BIT9 | BIT8);
      MmioWrite32 (
        (UINTN) (EhciPciMmBase + 0x8C),
        DwordReg
        );
      PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (EhciPciMmBase + 0x8C),
        1,
        (VOID *) (UINTN) (EhciPciMmBase + 0x8C)
        );

      if (SkipRst) {
        ///
        /// Restore Pci command register
        ///
        MmioWrite16 ((EhciPciMmBase + R_PCH_EHCI_COMMAND_REGISTER), PciCmd);
#ifdef SUS_WELL_RESTORE
        ///
        /// To support RapidStart resume from G3 state, all resume well registers need to be saved
        /// into S3 Script table.
        ///
        PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
          EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
          EfiBootScriptWidthUint16,
          (UINTN) (EhciPciMmBase + R_PCH_EHCI_COMMAND_REGISTER),
          1,
          (VOID *) (UINTN) (EhciPciMmBase + R_PCH_EHCI_COMMAND_REGISTER)
          );
#endif
      } else {
#ifdef USB_PRECONDITION_ENABLE_FLAG
        ///
        /// If precondition is enabled, execute USB precondition function by each phase call
        ///
        if (USB_PRECONDITION_POLICY_SUPPORT (UsbConfig)) {
           EHCI_PRECONDITION (EhciControllersMap[Index].Device, EhciMmioBase);
        }
#endif  // USB_PRECONDITION_ENABLE_FLAG
        ///
        /// Clear memory resource and command register after initialization
        ///
        MmioAnd16 (
          (EhciPciMmBase + R_PCH_EHCI_COMMAND_REGISTER),
          (UINT16)~(B_PCH_EHCI_COMMAND_MSE | B_PCH_EHCI_COMMAND_BME)
          );
        MmioWrite32 ((EhciPciMmBase + R_PCH_EHCI_MEM_BASE), 0);
#ifdef SUS_WELL_RESTORE
        ///
        /// To support RapidStart resume from G3 state, all resume well registers need to be saved
        /// into S3 Script table.
        ///
        PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
          EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
          EfiBootScriptWidthUint16,
          (UINTN) (EhciPciMmBase + R_PCH_EHCI_COMMAND_REGISTER),
          1,
          (VOID *) (UINTN) (EhciPciMmBase + R_PCH_EHCI_COMMAND_REGISTER)
          );
        PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
          EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
          EfiBootScriptWidthUint32,
          (UINTN) (EhciPciMmBase + R_PCH_EHCI_MEM_BASE),
          1,
          (VOID *) (UINTN) (EhciPciMmBase + R_PCH_EHCI_MEM_BASE)
          );
#endif
      }
    }
  }
#ifdef USB_PRECONDITION_ENABLE_FLAG
  ///
  /// Execute the code if running in PEI phase when USB precondition feature is enabled
  ///  or in DXE phase when USB precondition feature disabled
  /// If the precondition is enabled and running in DXE phase, 
  /// the code has already run once in PEI but the save S3 script need to run again in DXE phase
  /// but only run if and only if both EHCI is not disabled
  ///
  if ((USB_RUN_IN_PEI || (!USB_PRECONDITION_POLICY_SUPPORT (UsbConfig))) ||
      ((USB_RUN_IN_DXE) && ((UsbConfig->Usb20Settings[PchEhci1].Enable == PCH_DEVICE_ENABLE) ||
       ((UsbConfig->Usb20Settings[PchEhci2].Enable == PCH_DEVICE_ENABLE) && (PchSeries == PchH)))))
  {
#endif  // USB_PRECONDITION_ENABLE_FLAG
    ///
    /// PCH BIOS Spec Rev 0.5.5 Section 12.10
    /// Additional Programming Requirements during USB Initialization
    /// Step 7
    /// IOBP Programming:
    /// a) Set IOBP register 0xE5007F04 to 00004481h.
    /// b) Set IOBP register 0xE500400F[0] + (PortNumber * 0x100) = 0b.
    /// c) Set IOBP register 0xE5007F14[20:19] to 11b.
    /// d) Set IOBP register 0xE5007F02[23:22] to 00b for LPT-LP
    ///
    /// Set IOBP register 0xE5007F04[14:13] to 10b.
    ///
    Data32And = 0;
    Data32Or  = 0x00004481;
    Status = ProgramIobp (
              RootComplexBar,
              0xE5007F04,
              Data32And,
              Data32Or
              );
    ASSERT_EFI_ERROR (Status);
    Status = PCH_INIT_COMMON_SCRIPT_SAVE_IOBP_S3_ITEM (
              RootComplexBar,
              0xE5007F04,
              Data32And,
              Data32Or
              );
    ASSERT_EFI_ERROR (Status);
    for (Index = 0; Index < GetPchUsbMaxPhysicalPortNum (); Index++) {
      ///
      /// Set IOBP register 0xE500400F[0] + (PortNumber * 0x100) = 0b. 
      ///    
      Data32And = (UINT32)~(BIT0);
      Data32Or  = (UINT32) (0);
      Status = ProgramIobp (
                RootComplexBar,
                0xE500400F + ((Index + 1) * 0x100),
                Data32And,
                Data32Or
                );
      ASSERT_EFI_ERROR (Status);
      Status = PCH_INIT_COMMON_SCRIPT_SAVE_IOBP_S3_ITEM (
                RootComplexBar,
                0xE500400F + ((Index + 1) * 0x100),
                Data32And,
                Data32Or
                );
      ASSERT_EFI_ERROR (Status);
    }
    ///
    /// Set IOBP register 0xE5007F14[20:19] to 11b.
    ///
    Data32And = (UINT32)~(0);
    Data32Or  = (UINT32) (BIT20 | BIT19);
    Status = ProgramIobp (
              RootComplexBar,
              0xE5007F14,
              Data32And,
              Data32Or
              );
    ASSERT_EFI_ERROR (Status);
    Status = PCH_INIT_COMMON_SCRIPT_SAVE_IOBP_S3_ITEM (
              RootComplexBar,
              0xE5007F14,
              Data32And,
              Data32Or
              );
    ASSERT_EFI_ERROR (Status);
    ///
    /// Set IOBP register 0xE5007F02[23:22] to 00b for LPT-LP
    ///
    if (PchSeries == PchLp) {
      Data32And = (UINT32)~(BIT23 | BIT22);
      Data32Or  = (UINT32) (0);
      Status = ProgramIobp (
                RootComplexBar,
                0xE5007F02,
                Data32And,
                Data32Or
                );
      ASSERT_EFI_ERROR (Status);
      Status = PCH_INIT_COMMON_SCRIPT_SAVE_IOBP_S3_ITEM (
                RootComplexBar,
                0xE5007F02,
                Data32And,
                Data32Or
                );
      ASSERT_EFI_ERROR (Status);
    }

#ifdef USB_PRECONDITION_ENABLE_FLAG
  }
#endif  // USB_PRECONDITION_ENABLE_FLAG
  return EFI_SUCCESS;
}

/**
  Performs basic configuration of PCH USB3 (xHCI) controller.

  @param[in] UsbConfig            The PCH Platform Policy for USB configuration
  @param[in] XhciMmioBase         Memory base address of xHCI Controller
  @param[in] Revision             The policy revision used for backward compatible check
  @param[in] LpcDeviceId          The device ID of LPC
  @param[in] XhciPciMmBase        XHCI PCI Base Address

  @retval None
**/
VOID
CommonXhciHcInit (
  IN  PCH_USB_CONFIG              *UsbConfig,
  IN  UINT32                      XhciMmioBase,
  IN  UINT8                       Revision,
  IN  UINT16                      LpcDeviceId,
  IN  UINTN                       XhciPciMmBase
  )
{
  UINT32      Data32Or;
  UINT32      Data32And;
  UINT8       PchSteppingValue;
  PCH_SERIES  PchSeries;

  Data32Or = 0;
  Data32And = 0;
  PchSeries = GetPchSeries();
  PchSteppingValue = PchStepping();
  ///
  /// Check if XHCI disabled, Exit function.
  ///
  if (UsbConfig->Usb30Settings.Mode == PCH_XHCI_MODE_OFF) {
    return;
  }
  ///
  /// PCH BIOS Spec Rev 0.5.5, Section 13.2.1 xHCI controller setup
  ///
  if (PchSeries == PchH) {
    ///
    /// For LPT-H Only:
    /// USB3 ports always start at offset 16 (accounting for 15 USB2 ports) regardless of the number of USB2 ports
    /// the XHCI spec allows there to be a gap between the highest numbered USB2 port and the lowest numbered USB3 port).
    /// So the Maxports value is dependent entirely on the number of USB3 ports, and not upon the number of USB2 ports.
    /// So the appropriate BIOS workaround is to look at the number of USB3 ports in the FUS register  - config offset E0h.
    /// (since there are SKUs with fewer than 6 port).
    ///   * If number of SS ports = 6, maxports = 21 (15h)
    ///   * If number of SS ports = 4, maxports = 19 (13h)
    ///   * If number of SS ports = 2, maxports = 17 (11h)
    ///   * If number of SS ports = 0, maxports = 15 (0Fh)
    ///
    switch (MmioRead32 (XhciPciMmBase + R_PCH_XHCI_FUS) & B_PCH_XHCI_FUS_SSPRTCNT) {
      case V_PCH_XHCI_FUS_SSPRTCNT_11B:
        // Number of SS ports is 0, Set xHCIBAR + 04h[31:24] = 0Fh
        Data32Or = 0x0F000000;
        break;

      case V_PCH_XHCI_FUS_SSPRTCNT_10B:
        // Number of SS ports is 2, Set xHCIBAR + 04h[31:24] = 11h
        Data32Or = 0x11000000;
        break;

      case V_PCH_XHCI_FUS_SSPRTCNT_01B:
        // Number of SS ports is 4, Set xHCIBAR + 04h[31:24] = 13h
        Data32Or = 0x13000000;
        break;

      case V_PCH_XHCI_FUS_SSPRTCNT_00B:
      default:
        // Number of SS ports is 6, Set xHCIBAR + 04h[31:24] = 15h
        Data32Or = 0x15000000;
        break;
    }
    MmioAndThenOr32 (
      (XhciMmioBase + R_PCH_XHCI_HCSPARAMS1),
      (UINT32) 0x00FFFFFF,
      (UINT32) Data32Or
      );
    PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (XhciMmioBase + R_PCH_XHCI_HCSPARAMS1),
      1,
      (VOID *) (UINTN) (XhciMmioBase + R_PCH_XHCI_HCSPARAMS1)
      );
  }
  ///
  /// Set xHCIBAR + 0Ch[7:0] = 0Ah and [31:16] = 200h
  ///
  MmioAndThenOr32 (
    (XhciMmioBase + R_PCH_XHCI_HCSPARAMS3),
    (UINT32) 0x0000FF00,
    (UINT32) 0x0200000A
    );
  PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (XhciMmioBase + R_PCH_XHCI_HCSPARAMS3),
    1,
    (VOID *) (UINTN) (XhciMmioBase + R_PCH_XHCI_HCSPARAMS3)
    );
  ///
  /// Set xHCIBAR + 10h[10,9,5] to 1b, 1b, 0b
  ///
  MmioAndThenOr32 (
    (XhciMmioBase + R_PCH_XHCI_HCCPARAMS),
    (UINT32)~(BIT5),
    (UINT32) (BIT10 | BIT9)
    );
  PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (XhciMmioBase + R_PCH_XHCI_HCCPARAMS),
    1,
    (VOID *) (UINTN) (XhciMmioBase + R_PCH_XHCI_HCCPARAMS)
    );
  if (PchSeries == PchH) {
    ///
    /// For LPT-H, Set xHCIBAR + 8008h[19] to 0b
    ///
    MmioAnd32 (
      (XhciMmioBase + 0x8008),
      (UINT32)~(BIT19)
      );
    PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (XhciMmioBase + 0x8008),
      1,
      (VOID *) (UINTN) (XhciMmioBase + 0x8008)
      );
  }
  
  if (PchSeries == PchLp) {
    ///
    /// Set xHCIBAR + 8058h[16,8] to 1b, 0b
    ///
    MmioAndThenOr32 (
      (XhciMmioBase + 0x8058),
      (UINT32)~(BIT8),
      (UINT32) (BIT16)
      );
  } else if (PchSeries == PchH) {
    ///
    /// Set xHCIBAR + 8058h[20,16,8] to 1b, 1b, 0b
    ///
    MmioAndThenOr32 (
      (XhciMmioBase + 0x8058),
      (UINT32)~(BIT8),
      (UINT32) (BIT20 | BIT16)
      );
  }
  PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (XhciMmioBase + 0x8058),
    1,
    (VOID *) (UINTN) (XhciMmioBase + 0x8058)
    );
  ///
  /// Set xHCIBAR + 8060h[25, 18] to 1b, 1b  
  ///
  MmioOr32 (
    (XhciMmioBase + 0x8060),
    (UINT32) (BIT25 | BIT18)
    );

  PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (XhciMmioBase + 0x8060),
    1,
    (VOID *) (UINTN) (XhciMmioBase + 0x8060)
    );
  ///
  /// Set xHCIBAR + 8090h[14,8] to 1b, 1b
  ///
  MmioOr32 (XhciMmioBase + 0x8090, (UINT32) (BIT14 | BIT8));
  PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (XhciMmioBase + 0x8090),
    1,
    (VOID *) (UINTN) (XhciMmioBase + 0x8090)
    );
  ///
  /// Set xHCIBAR + 8094h[23, 21, 14] to 1b, 1b, 1b
  ///
  MmioOr32 (XhciMmioBase + 0x8094, (UINT32) (BIT23 | BIT21 | BIT14));
  PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (XhciMmioBase + 0x8094),
    1,
    (VOID *) (UINTN) (XhciMmioBase + 0x8094)
    );
  ///
  /// Set xHCIBAR + 80E0h[16, 6] to 0b, 1b
  ///
  MmioAndThenOr32 (
    (XhciMmioBase + 0x80E0),
    (UINT32)~(BIT16),
    (UINT32) (BIT6)
    );
#ifdef SUS_WELL_RESTORE
  ///
  /// To support RapidStart resume from G3 state, all resume well registers need to be saved
  /// into S3 Script table.
  ///
  PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (XhciMmioBase + 0x80E0),
    1,
    (VOID *) (UINTN) (XhciMmioBase + 0x80E0)
    );
#endif // SUS_WELL_RESTORE
  ///
  /// Set xHCIBAR + 80ECh[14:12] to 00h
  /// Set xHCIBAR + 80ECh[11:9] to 06h
  ///
  MmioAndThenOr32 (
    (XhciMmioBase + 0x80EC),
    (UINT32)~(BIT14 | BIT13 | BIT12 | BIT9),
    (UINT32) (BIT11 | BIT10)
    );
  PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (XhciMmioBase + 0x80EC),
    1,
    (VOID *) (UINTN) (XhciMmioBase + 0x80EC)
    );
  ///
  /// Set xHCIBAR + 80F0h[20] to 0b
  ///
  MmioAnd32 (
    (XhciMmioBase + 0x80F0),
    (UINT32)~(BIT20)
    );
#ifdef SUS_WELL_RESTORE
  PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (XhciMmioBase + 0x80F0),
    1,
    (VOID *) (UINTN) (XhciMmioBase + 0x80F0)
    );
#endif // SUS_WELL_RESTORE
  ///
  /// For LPT-LP,  Set xHCIBAR + 80FCh[25] to 1b  (Note: In document it is written as bit 121 of 80F0h.)
  ///
  if (PchSeries == PchLp) {
    MmioOr32 (XhciMmioBase + 0x80FC, (UINT32) (BIT25));
    PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (XhciMmioBase + 0x80FC),
      1,
      (VOID *) (UINTN) (XhciMmioBase + 0x80FC)
      );
  }
  if (PchSeries == PchLp) {
    ///
    /// Set xHCIBAR + 8110h[20,11, 8, 2] to 1b, 1b, 0b, 0b
    ///
    MmioAndThenOr32 (
      (XhciMmioBase + 0x8110),
      (UINT32)~(BIT2 | BIT8),
      (UINT32) (BIT20 | BIT11)
      );
    PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (XhciMmioBase + 0x8110),
      1,
      (VOID *) (UINTN) (XhciMmioBase + 0x8110)
      );
  } else  if (PchSeries == PchH) {
      ///
      /// Set xHCIBAR + 8110h[20,11,2] to 1b, 1b, 0b
      ///
      MmioAndThenOr32 (
        (XhciMmioBase + 0x8110),
        (UINT32)~(BIT2),
        (UINT32) (BIT20 | BIT11)
        );
      PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (XhciMmioBase + 0x8110),
        1,
        (VOID *) (UINTN) (XhciMmioBase + 0x8110)
        );
  }
  ///
  /// For LPT-H , Set xHCIBAR + 8140h[31:0] to FF03C132h
  /// For LPT-LP, Set xHCIBAR + 8140h[31:0] to FF00F03Ch
  ///
  if (PchSeries == PchH) {
    Data32And = (UINT32)~(0xFFFFFFFF);
    Data32Or  = (UINT32) (0xFF03C132);
    MmioAndThenOr32 (
     (XhciMmioBase + 0x8140),
      Data32And,
      Data32Or
      );
    PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (XhciMmioBase + 0x8140),
      1,
      (VOID *) (UINTN) (XhciMmioBase + 0x8140)
      );
  }
  if (PchSeries == PchLp) {
    Data32And = (UINT32)~(0xFFFFFFFF);
    Data32Or  = (UINT32) (0xFF00F03C);
    MmioAndThenOr32 (
     (XhciMmioBase + 0x8140),
      Data32And,
      Data32Or
      );
    PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (XhciMmioBase + 0x8140),
      1,
      (VOID *) (UINTN) (XhciMmioBase + 0x8140)
      );
  }
 
  ///
  /// For LPT-LP Set xHCIBAR + 8154h[21, 13] to 0b, 1b
  /// For LPT-H Set xHCIBAR + 8154h[21, 13] to 0b, 0b
  ///
  if (PchSeries == PchH) {
    Data32And = BIT21 | BIT13;
    Data32Or = 0;
  } else if (PchSeries == PchLp) {
    Data32And = BIT21;
    Data32Or = BIT13;
  }
  MmioAndThenOr32 (
    (XhciMmioBase + 0x8154),
    (UINT32)~(Data32And),
    (UINT32) (Data32Or)
    );
  ///
  /// Clear xHCIBAR + 8154h[3] to 0b
  ///
  MmioAnd32 (
    (XhciMmioBase + 0x8154),
    (UINT32)~(BIT3)
    );
  PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (XhciMmioBase + 0x8154),
    1,
    (VOID *) (UINTN) (XhciMmioBase + 0x8154)
    );
  
  ///
  /// For LPT-LP, Set xHCIBAR + 8164h[0,1] to 1b
  ///
  if (PchSeries == PchLp) {
    MmioOr32 (XhciMmioBase + 0x8164, (UINT32) (BIT1 | BIT0));
    PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (XhciMmioBase + 0x8164),
      1,
      (VOID *) (UINTN) (XhciMmioBase + 0x8164)
      );
  }

  ///
  /// For LPT-LP, Set xHCIBAR + 8174h = 0x01400C0A
  ///
  if (PchSeries == PchLp) {
    MmioWrite32 (XhciMmioBase + 0x8174, 0x01400c0a);
    PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (XhciMmioBase + 0x8174),
      1,
      (VOID *) (UINTN) (XhciMmioBase + 0x8174)
      );
  }
  ///
  /// For LPT-LP, Set xHCIBAR + 817Ch[31:0] to 033200A3h
  ///
  if (PchSeries == PchLp) {
    Data32And = (UINT32)~(0xFFFFFFFF);
    Data32Or  = (UINT32) (0x033200A3);
    MmioAndThenOr32 (
     (XhciMmioBase + 0x817C),
      Data32And,
      Data32Or
      );
    PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (XhciMmioBase + 0x817C),
      1,
      (VOID *) (UINTN) (XhciMmioBase + 0x817C)
      );
  }
  ///
  /// For LPT-LP, Set xHCIBAR + 8180h[31:0] to 00CB0028h
  ///
  if (PchSeries == PchLp) {
    Data32And = (UINT32)~(0xFFFFFFFF);
    Data32Or  = (UINT32) (0x00CB0028);
    MmioAndThenOr32 (
     (XhciMmioBase + 0x8180),
      Data32And,
      Data32Or
      );
    PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (XhciMmioBase + 0x8180),
      1,
      (VOID *) (UINTN) (XhciMmioBase + 0x8180)
      );
  }
  ///
  /// For LPT-LP, Set xHCIBAR + 8184h[31:0] to 0064001Eh
  ///
  if (PchSeries == PchLp) {
    Data32And = (UINT32)~(0xFFFFFFFF);
    Data32Or  = (UINT32) (0x0064001E);
    MmioAndThenOr32 (
     (XhciMmioBase + 0x8184),
      Data32And,
      Data32Or
      );
    PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (XhciMmioBase + 0x8184),
      1,
      (VOID *) (UINTN) (XhciMmioBase + 0x8184)
      );
  }

  ///
  /// Set D20:F0:44h[15,14,10,0] to 1b
  /// Note: Only update D20:F0:44h by word since D20:F0:44h[31] is write-once bit
  ///
  MmioOr16 (XhciPciMmBase + 0x44, (UINT16) (BIT15 | BIT14 | BIT10 | BIT0));
  PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint16,
    (UINTN) (XhciPciMmBase + 0x44),
    1,
    (VOID *) (UINTN) (XhciPciMmBase + 0x44)
    );

  ///
  /// Set D20:F0:44h[19:16] to 1111b
  /// Note: Update D20:F0:44h by byte to 46h since D20:F0:44h[31] is write-once bit
  ///
  MmioOr8 (XhciPciMmBase + 0x46, (UINT8) (BIT3 | BIT2 | BIT1 | BIT0));
  PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint8,
    (UINTN) (XhciPciMmBase + 0x46),
    1,
    (VOID *) (UINTN) (XhciPciMmBase + 0x46)
    );
    
  ///
  /// LPT-LP >= B0: BIOS must set XhciMmioBase + 0x8188[26, 24] to 1b, 1b
  /// LPT-H  >= C0: BIOS must set XhciMmioBase + 0x8188[    24] to     1b
  ///
  if(((PchSeries == PchLp) && (PchSteppingValue >= LptLpB0))) {
      MmioOr32 (XhciMmioBase + 0x8188, (UINT32) (BIT26 | BIT24));
      PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (XhciMmioBase + 0x8188),
        1,
        (VOID *) (UINTN) (XhciMmioBase + 0x8188)
        );
  } else if(((PchSeries == PchH) && (PchSteppingValue >= LptHC0))){     
      MmioOr32 (XhciMmioBase + 0x8188, (UINT32) (BIT24));
      PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (XhciMmioBase + 0x8188),
        1,
        (VOID *) (UINTN) (XhciMmioBase + 0x8188)
        );
  }
}

/**
  Initialization XHCI Clock Gating registers

  @param[in] PchPlatformPolicy    The PCH Platform Policy
  @param[in] RootComplexBar       RootComplexBar value of this PCH device

  @retval None
**/
VOID
ConfigureXhciClockGating (
  IN     DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy,
  IN     UINT32                               RootComplexBar
  )
{
  UINT32      XhccCfg;
  UINTN       XhciPciMmBase;
  UINT8       Data8;
  UINT16      Data16;
  UINT32      Data32And;
  UINT32      Data32Or;
  EFI_STATUS  Status;
  PCH_SERIES      PchSeries;
  PchSeries     = GetPchSeries();
  XhciPciMmBase = MmPciAddress (
                    0,
                    PchPlatformPolicy->BusNumber,
                    PCI_DEVICE_NUMBER_PCH_XHCI,
                    PCI_FUNCTION_NUMBER_PCH_XHCI,
                    0
                    );
  ///
  /// Set IOBP register 0xE5004001[7:6] to 11b
  ///
  Data32And = (UINT32)~(0);
  Data32Or  = (UINT32) (BIT7 | BIT6);
  Status = ProgramIobp (
            RootComplexBar,
            0xE5004001,
            Data32And,
            Data32Or
            );
  ASSERT_EFI_ERROR (Status);
  Status = PCH_INIT_COMMON_SCRIPT_SAVE_IOBP_S3_ITEM (
            RootComplexBar,
            0xE5004001,
            Data32And,
            Data32Or
            );
  ASSERT_EFI_ERROR (Status);
  ///
  /// For LPT-H : Set D20:F0:40h[21,20,18,17,8] to 1b
  /// For LPT-LP: 
  /// Set D20:F0:40h[18,17,8] to 1b. 
  /// Set D20:F0:40h[21,20,19] to 000b to disable XHCI Idle L1. 
  /// Set D20:F0:40h[21,20,19] to 110b to enable XHCI Idle L1. 
  /// Note for LPT-LP Ax stepping, 
  ///   USB3 hot plug will fail after 1 hot plug removal. 
  ///   BIOS implement a Setup Option to disable XHCI Idle L1 as workaround.
  ///   Option should default to enable XHCI Idle L1 to allow PCH PM testing.
  ///   User need to put the system to G3 when changing from Enable to Disable state.
  ///
  XhccCfg = MmioRead32 (XhciPciMmBase + R_PCH_XHCI_XHCC1);
  XhccCfg &= (UINT32) ~(B_PCH_XHCI_XHCC1_URD);
  if (PchSeries == PchH) {
    XhccCfg |= (UINT32) (BIT21 | BIT20 | BIT18 | BIT17 | BIT8);
  } else if (PchSeries == PchLp) {
    XhccCfg |= (UINT32) (BIT18 | BIT17 | BIT8);
    if (PchPlatformPolicy->UsbConfig->Usb30Settings.XhciIdleL1 == PCH_DEVICE_DISABLE) {
      XhccCfg &= (UINT32)~(BIT21 | BIT20 | BIT19);
    } else {
      XhccCfg |= (UINT32) (BIT21 | BIT20);
      XhccCfg &= (UINT32)~(BIT19);
    }
  }
  Data16 = (UINT16)XhccCfg;
  Data8 = (UINT8)(XhccCfg >> 16);
  MmioWrite16 (XhciPciMmBase + R_PCH_XHCI_XHCC1, Data16);
  MmioWrite8  (XhciPciMmBase + R_PCH_XHCI_XHCC1 + 2, Data8);
  ///
  /// Set D20:F0:44h[9, 7, 3] to 1b
  ///
  MmioOr16 (XhciPciMmBase + 0x44, (UINT16) (BIT9 | BIT7 | BIT3));
  PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint16,
    (UINTN) (XhciPciMmBase + 0x44),
    1,
    (VOID *) (UINTN) (XhciPciMmBase + 0x44)
    );
 
  ///
  /// For LPT-LP, Set D20:F0:A0h[18] to 1b
  /// For LPT-H, Set D20:F0:A0h[6] to 1b
  ///
  if (PchSeries == PchH) {
    Data32Or = BIT6;
  } else if (PchSeries == PchLp) {
    Data32Or = BIT18;
  }
  MmioOr32 (XhciPciMmBase + 0xA0, (UINT32) Data32Or);
  PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (XhciPciMmBase + 0xA0),
    1,
    (VOID *) (UINTN) (XhciPciMmBase + 0xA0)
    );
  ///
  /// Set D20:F0:A4h[13] to 0b
  ///
  MmioAnd32 (XhciPciMmBase + 0xA4, (UINT32)~(BIT13));
  PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (XhciPciMmBase + 0xA4),
    1,
    (VOID *) (UINTN) (XhciPciMmBase + 0xA4)
    );
}

/**
  Performs basic configuration of PCH USB3 (xHCI) controller.

  @param[in] UsbConfig            The PCH Platform Policy for USB configuration
  @param[in] XhciMmioBase         Memory base address of XHCI Controller
  @param[in] Revision             The policy revision used for backward compatible check
  @param[in] LpcDeviceId          The device ID of LPC
  @param[in] XhciPciMmBase        XHCI PCI Base Address
  @param[in] PciD31F0RegBase      LPC PCI Base Address

  @retval None
**/
VOID
PerformXhciEhciPortSwitchingFlow (
  IN  PCH_USB_CONFIG              *UsbConfig,
  IN  UINT32                      XhciMmioBase,
  IN  UINT8                       Revision,
  IN  UINT16                      LpcDeviceId,
  IN  UINTN                       XhciPciMmBase,
  IN  UINTN                       PciD31F0RegBase
  )
{
  UINT32      UsbPort;
  UINTN       PortResetTimeout;
  UINTN       HsPortCount;
  UINTN       HsUsbrPortCount;
  UINTN       SsPortCount;
  UINT32      PortMask;
  UINT8       UsbPortRouting;
  PCH_SERIES  PchSeries;
  UINTN       *PORTSCxUSB2Ptr;
  UINTN       *PORTSCxUSB3Ptr;
  UINT32      Data32;

  PchSeries = GetPchSeries();
  PORTSCxUSB2Ptr = NULL;
  PORTSCxUSB3Ptr = NULL;
  switch (PchSeries) {
    case PchLp:
      PORTSCxUSB2Ptr = &PCH_LP_PORTSCxUSB2[0];
      PORTSCxUSB3Ptr = &PCH_LP_PORTSCxUSB3[0];
      break;

    case PchH:
      PORTSCxUSB2Ptr = &PCH_H_PORTSCxUSB2[0];
      PORTSCxUSB3Ptr = &PCH_H_PORTSCxUSB3[0];
      break;

    default:
      break;
  }
  ///
  /// Check if XHCI disabled, Exit function.
  ///
  if (UsbConfig->Usb30Settings.Mode == PCH_XHCI_MODE_OFF) {
    return;
  }
  ///
  /// Retrieves information about number of implemented xHCI ports and sets
  /// appropriate port mask registers
  /// Get the xHCI port number and program xHCI Port Routing Mask register
  ///
  GetXhciPortCountAndSetPortRoutingMask (
    XhciPciMmBase,
    &HsPortCount,
    &HsUsbrPortCount,
    &SsPortCount
    );
  ///
  /// Workaround for USB2PR / USB3PR value not surviving warm reset.
  ///
  /// Check if Warm Reset
  ///
#ifdef USB_PRECONDITION_ENABLE_FLAG
  if (USB_RUN_IN_PEI || (!USB_PRECONDITION_POLICY_SUPPORT (UsbConfig))) {
#endif  // USB_PRECONDITION_ENABLE_FLAG
//[-start-130221-IB02950497-modify]//
    if (MmioRead16 (PciD31F0RegBase + R_PCH_LPC_GEN_PMCON_2) & B_PCH_LPC_GEN_PMCON_MEM_SR) {
      ///
      /// Restore USB Port Routing registers if OS HC Switch driver has been executed
      ///
      if (MmioRead32 (PciD31F0RegBase + 0xAC) & BIT16) {
        if (UsbConfig->Usb30Settings.ManualMode == PCH_DEVICE_ENABLE) {
          ///
          /// Program D20:F0:D8h[5:0] manually.
          ///
          PortMask = 0;
          for (UsbPort = 0; UsbPort < SsPortCount; UsbPort++) {
            if (UsbConfig->Usb30Settings.ManualModeUsb30PerPinEnable[UsbPort]==1) {  // 0: Disable; 1:Enable;
              PortMask |= 1 << UsbPort;
            }
          }
          
          MmioAndThenOr32 (
            XhciPciMmBase + R_PCH_XHCI_USB3PR,
            (UINT32)~B_PCH_XHCI_USB3PR_USB3SSEN,
            PortMask
            );

          ///
          /// Program D20:F0:D0h[14:0] manually
          ///
          PortMask = 0;
          for (UsbPort = 0; UsbPort < HsPortCount; UsbPort++) {
            if (UsbConfig->Usb30Settings.ManualModeUsb20PerPinRoute[UsbPort]==1) {  // 0: EHCI; 1 :XHCI;
              PortMask |= 1 << UsbPort;
            }
          }
          
          MmioAndThenOr32 (
            XhciPciMmBase + R_PCH_XHCI_USB2PR,
            (UINT32)~B_PCH_XHCI_USB2PR_USB2HCSEL,
            PortMask
            );      	
        } else {
          ///
          /// Program D20:F0:D8h[5:0] to the value of xHCI D20:F0:DCh[5:0]
          ///
          PortMask = MmioRead32 (XhciPciMmBase + R_PCH_XHCI_USB3PRM) & (UINT32) B_PCH_XHCI_USB3PR_USB3SSENM;
          
          MmioAndThenOr32 (
            XhciPciMmBase + R_PCH_XHCI_USB3PR,
            (UINT32)~B_PCH_XHCI_USB3PR_USB3SSEN,
            PortMask
            );
          ///
          /// Step 3
          /// Program D20:F0:D0h[14:0] to the value of xHCI D20:F0:D4h[14:0]
          ///
          PortMask = MmioRead32 (XhciPciMmBase + R_PCH_XHCI_USB2PRM) & (UINT32) B_PCH_XHCI_USB2PR_USB2HCSELM;
          
          MmioAndThenOr32 (
            XhciPciMmBase + R_PCH_XHCI_USB2PR,
            (UINT32)~B_PCH_XHCI_USB2PR_USB2HCSEL,
            PortMask
            );
        }
      }
    }
//[-end-130221-IB02950497-modify]//
#ifdef USB_PRECONDITION_ENABLE_FLAG
  }
#endif  // USB_PRECONDITION_ENABLE_FLAG
#ifdef USB_PRECONDITION_ENABLE_FLAG
  ///
  /// Only clear this bit after DXE phase has finished using it.
  /// because if we clear it too early in PEI phase
  /// then we cannot determine if OS HC Switch driver has been executed in DXE phase.
  ///
  if (USB_RUN_IN_DXE || (!USB_PRECONDITION_POLICY_SUPPORT (UsbConfig))) {
#endif  // USB_PRECONDITION_ENABLE_FLAG
    ///
    /// Clear B0:D31:F0 ACh[16] to indicate finish using this bit and begin of BIOS phase of USB port routing
    ///
    MmioAnd32 (PciD31F0RegBase + 0xAC, (UINT32) ~BIT16);
#ifdef USB_PRECONDITION_ENABLE_FLAG
  }
#endif  // USB_PRECONDITION_ENABLE_FLAG
  ///
  /// Do nothing for this case
  ///
  UsbPortRouting = USB_PR_CASE_0;

  if ((UsbConfig->Usb30Settings.PreBootSupport == PCH_DEVICE_DISABLE) ||
      ((UsbConfig->Usb30Settings.PreBootSupport == PCH_DEVICE_ENABLE) &&
       (UsbConfig->Usb30Settings.Mode == PCH_XHCI_MODE_AUTO))) {
    ///
    /// PCH BIOS Spec Rev 0.5.0
    /// When the BIOS does not have xHCI pre-boot software available:
    /// Section 13.1.1.2 xHCI Enabled mode
    /// BIOS should route the Ports to the EHCI controller and prior to OS boot
    /// it should route the ports to the xHCI controller.
    /// Section 13.1.1.3 xHCI Auto mode
    /// BIOS should route the Ports to the EHCI controller
    ///
    /// When the BIOS has xHCI pre-boot software available:
    /// Section 13.1.2.3 xHCI Auto mode
    /// BIOS should route the Ports to the EHCI controller
    ///
    /// For above cases, BIOS should follow Section 13.2.5 to route the
    /// USB Ports to EHCI Controller.
    ///
    UsbPortRouting = USB_PR_CASE_1;
  }

  if ((UsbConfig->Usb30Settings.PreBootSupport == PCH_DEVICE_ENABLE) &&
      (UsbConfig->Usb30Settings.Mode == PCH_XHCI_MODE_ON)) {
    ///
    /// PCH BIOS Spec Rev 0.5.0
    /// When the BIOS has xHCI pre-boot software available:
    /// Section 13.1.2.2 xHCI Enabled mode
    /// BIOS should route the Ports to the xHCI controller
    ///
    /// For the above case, BIOS should follow Section 13.2.6 to route the
    /// USB Ports to xHCI Controller.
    ///
    UsbPortRouting = USB_PR_CASE_2;
#ifdef USB_PRECONDITION_ENABLE_FLAG
    ///
    /// Execute if runnin in PEI phase or USB precondition feature is not enabled
    /// If the precondition is enabled and running in DXE phase, the workaround is done already
    ///
    if (USB_RUN_IN_PEI || (!USB_PRECONDITION_POLICY_SUPPORT (UsbConfig))) {
#endif  // USB_PRECONDITION_ENABLE_FLAG
      if (MmioRead16(PciD31F0RegBase + R_PCH_LPC_GEN_PMCON_2) & B_PCH_LPC_GEN_PMCON_MEM_SR) {
        ///
        /// Step 3
        /// Initiate warm reset to all USB3 ports
        ///
        for (UsbPort = 0; UsbPort < SsPortCount; UsbPort++) {
          MmioAndThenOr32 (
          XhciMmioBase + PORTSCxUSB3Ptr[UsbPort],
            (UINT32)~ (B_PCH_XHCI_PORTSCXUSB3_PED),
            B_PCH_XHCI_PORTSCXUSB3_WPR
            );
        }
        ///
        /// 3.c. Poll for warm reset bit at steps #a to be cleared or timeout at 100ms
        ///
        PortResetTimeout = 0;
        do {
          Data32 = 0;
          for (UsbPort = 0; UsbPort < SsPortCount; UsbPort++) {
            Data32 |= MmioRead32 (XhciMmioBase + PORTSCxUSB3Ptr[UsbPort]);
          }
          PchPmTimerStall (TEN_MS_TIMEOUT);
          PortResetTimeout++;
        } while ((Data32 & B_PCH_XHCI_PORTSCXUSB3_PR) &&
                 (PortResetTimeout < PORT_RESET_TIMEOUT));
      }
#ifdef USB_PRECONDITION_ENABLE_FLAG
    }
#endif  // USB_PRECONDITION_ENABLE_FLAG
  }
#ifdef USB_PRECONDITION_ENABLE_FLAG
  ///
  /// Execute if runnin in PEI phase or USB precondition feature is not enabled
  /// If the precondition is enabled and running in DXE phase, the workaround is done already
  ///
  if (USB_RUN_IN_PEI || (!USB_PRECONDITION_POLICY_SUPPORT (UsbConfig))) {
#endif  // USB_PRECONDITION_ENABLE_FLAG
    if ((SsPortCount != 0) &&
        (UsbPortRouting == USB_PR_CASE_1) &&
        ((MmioRead32 (XhciPciMmBase + R_PCH_XHCI_USB2PR) != 0) ||
         (MmioRead32 (XhciPciMmBase + R_PCH_XHCI_USB3PR) != 0))) {
      ///
      /// PCH BIOS Spec Rev 0.5.0 Section 13.2.5 Routing of switchable USB Ports to
      /// EHCI Controller
      /// Step 1
      /// Retrieve information about the number of implemented xHCI ports and set appropriate
      /// port mask registers
      /// Done in GetXhciPortCountAndSetPortRoutingMask()
      /// Step 2
      /// Based on available number of ports (from step 1) initiate port reset to enabled ports
      /// where USB 2.0 device is connected
      ///
      /// 2.a. For Port #1, if xHCIBAR + 480h [0] is sets then
      /// 2.b. Issue port reset by sets xHCIBAR + 480h [4] to 1b
      /// 2.f. Repeat steps #a to #e for all the USB2.0 ports.
      ///
      for (UsbPort = 0; UsbPort < HsPortCount; UsbPort++) {
        if (MmioRead32 (XhciMmioBase + PORTSCxUSB2Ptr[UsbPort]) & B_PCH_XHCI_PORTSCXUSB2_CCS) {
          MmioAndThenOr32 (
            XhciMmioBase + PORTSCxUSB2Ptr[UsbPort],
            (UINT32)~(B_PCH_XHCI_PORTSCXUSB2_PED),
            B_PCH_XHCI_PORTSCXUSB2_PR
            );
        }
      }
      ///
      /// 2.c. Poll for port reset bit at steps #b to be cleared or timeout at 100ms
      ///
      PortResetTimeout = 0;
      do {
        Data32 = 0;
        for (UsbPort = 0; UsbPort < HsPortCount; UsbPort++) {
          Data32 |= MmioRead32 (XhciMmioBase + PORTSCxUSB2Ptr[UsbPort]);
        }
        PchPmTimerStall (TEN_MS_TIMEOUT);
        PortResetTimeout++;
      } while ((Data32 & B_PCH_XHCI_PORTSCXUSB2_PR) &&
              (PortResetTimeout < PORT_RESET_TIMEOUT));
      ///
      /// 2.d. Program D20:F0:D0h[14:0] to 0
      ///
      MmioAnd32 (
        XhciPciMmBase + R_PCH_XHCI_USB2PR,
        (UINT32)~B_PCH_XHCI_USB2PR_USB2HCSEL
        );
      ///
      /// 2.e. Clear all the port's status by program xHCIBAR + 480h [23:17] to 1111111b
      ///
      for (UsbPort = 0; UsbPort < HsPortCount; UsbPort++) {
        MmioAndThenOr32 (
          XhciMmioBase + PORTSCxUSB2Ptr[UsbPort],
          (UINT32)~ (B_PCH_XHCI_PORTSCXUSB2_PED),
          B_PCH_XHCI_PORTSCXUSB2_CEC |
          B_PCH_XHCI_PORTSCXUSB2_PLC |
          B_PCH_XHCI_PORTSCXUSB2_PRC |
          B_PCH_XHCI_PORTSCXUSB2_OCC |
          B_PCH_XHCI_PORTSCXUSB2_WRC |
          B_PCH_XHCI_PORTSCXUSB2_PEC |
          B_PCH_XHCI_PORTSCXUSB2_CSC
          );
      }
      if (HsUsbrPortCount > 0) {
        MmioAndThenOr32 (
          XhciMmioBase + PORTSCxUSB2Ptr[HsPortCount],
          (UINT32)~ (B_PCH_XHCI_PORTSCXUSB2_PED),
          B_PCH_XHCI_PORTSCXUSB2_CEC |
          B_PCH_XHCI_PORTSCXUSB2_PLC |
          B_PCH_XHCI_PORTSCXUSB2_PRC |
          B_PCH_XHCI_PORTSCXUSB2_OCC |
          B_PCH_XHCI_PORTSCXUSB2_WRC |
          B_PCH_XHCI_PORTSCXUSB2_PEC |
          B_PCH_XHCI_PORTSCXUSB2_CSC
          );
      }
      ///
      /// Step 3
      /// Initiate warm reset to all USB 3.0 ports
      ///
      /// 3.a. For Port #1,  sets xHCIBAR + 570h [31]
      /// 3.e. Repeat steps #a to #e for all the USB3.0 ports.
      ///
      for (UsbPort = 0; UsbPort < SsPortCount; UsbPort++) {
        MmioAndThenOr32 (
          XhciMmioBase + PORTSCxUSB3Ptr[UsbPort],
          (UINT32)~ (B_PCH_XHCI_PORTSCXUSB3_PED),
          B_PCH_XHCI_PORTSCXUSB3_WPR
          );
      }
      ///
      /// 3.b. Program D20:F0:D8h[5:0] to 0h.
      ///
      MmioAnd32 (
        XhciPciMmBase + R_PCH_XHCI_USB3PR,
        (UINT32)~B_PCH_XHCI_USB3PR_USB3SSEN
        );
      ///
      /// 3.c. Poll for warm reset bit at steps #a to be cleared or timeout at 100ms
      ///
      PortResetTimeout = 0;
      do {
        Data32 = 0;
        for (UsbPort = 0; UsbPort < SsPortCount; UsbPort++) {
          Data32 |= MmioRead32 (XhciMmioBase + PORTSCxUSB3Ptr[UsbPort]);
        }
        PchPmTimerStall (TEN_MS_TIMEOUT);
        PortResetTimeout++;
      } while ((Data32 & B_PCH_XHCI_PORTSCXUSB3_PR) &&
              (PortResetTimeout < PORT_RESET_TIMEOUT));
      ///
      /// 3.d. Clear all the port's status by program xHCIBAR + 570h [23:17] to 1111111b.
      ///
      for (UsbPort = 0; UsbPort < SsPortCount; UsbPort++) {
        MmioAndThenOr32 (
          XhciMmioBase + PORTSCxUSB3Ptr[UsbPort],
          (UINT32)~ (B_PCH_XHCI_PORTSCXUSB3_PED),
          B_PCH_XHCI_PORTSCXUSB3_CEC |
          B_PCH_XHCI_PORTSCXUSB3_PLC |
          B_PCH_XHCI_PORTSCXUSB3_PRC |
          B_PCH_XHCI_PORTSCXUSB3_OCC |
          B_PCH_XHCI_PORTSCXUSB3_WRC |
          B_PCH_XHCI_PORTSCXUSB3_PEC |
          B_PCH_XHCI_PORTSCXUSB3_CSC
          );
      }
      ///
      /// Step 4
      /// Set the Run bit of xHCI controller, xHCIBAR +80h[0] to 1b
      ///
      MmioOr32 (
        XhciMmioBase + R_PCH_XHCI_USBCMD,
        B_PCH_XHCI_USBCMD_RS
        );
      ///
      /// Step 5
      /// Then clear the Run bit of xHCI controller, xHCIBAR +80h[0] to 0b
      ///
      MmioAnd32 (
        XhciMmioBase + R_PCH_XHCI_USBCMD,
        (UINT32)~B_PCH_XHCI_USBCMD_RS
        );
    } else if ((SsPortCount != 0) && (UsbPortRouting == USB_PR_CASE_2)) {
      ///
      /// PCH BIOS Spec Rev 0.5.0 Section 13.2.6 Routing of switchable USB Ports to
      /// xHCI Controller
      /// Step 1
      /// Retrieve information about the number of implemented xHCI ports and set appropriate
      /// port mask registers
      /// Done in GetXhciPortCountAndSetPortRoutingMask()
      /// Step 2
      /// Program D20:F0:D8h[5:0] to the value of xHCI D20:F0:DCh[5:0]
      ///
      PortMask = MmioRead32 (XhciPciMmBase + R_PCH_XHCI_USB3PRM) & (UINT32) B_PCH_XHCI_USB3PR_USB3SSENM;

      MmioAndThenOr32 (
        XhciPciMmBase + R_PCH_XHCI_USB3PR,
        (UINT32)~B_PCH_XHCI_USB3PR_USB3SSEN,
        PortMask
        );
      ///
      /// Step 3
      /// Program D20:F0:D0h[14:0] to the value of xHCI D20:F0:D4h[14:0]
      ///
      PortMask = MmioRead32 (XhciPciMmBase + R_PCH_XHCI_USB2PRM) & (UINT32) B_PCH_XHCI_USB2PR_USB2HCSELM;

      MmioAndThenOr32 (
        XhciPciMmBase + R_PCH_XHCI_USB2PR,
        (UINT32)~B_PCH_XHCI_USB2PR_USB2HCSEL,
        PortMask
        );
    } else if (SsPortCount != 0) {
      //
      // Check if Warm Reset
      //
      if (MmioRead16(PciD31F0RegBase + R_PCH_LPC_GEN_PMCON_2) & B_PCH_LPC_GEN_PMCON_MEM_SR) {
        ///
        /// Step 3
        /// Initiate warm reset to all USB3 ports
        ///
        for (UsbPort = 0; UsbPort < SsPortCount; UsbPort++) {
          MmioAndThenOr32 (
          XhciMmioBase + PORTSCxUSB3Ptr[UsbPort],
            (UINT32)~ (B_PCH_XHCI_PORTSCXUSB3_PED),
            B_PCH_XHCI_PORTSCXUSB3_WPR
            );
        }
        ///
        /// 3.c. Poll for warm reset bit at steps #a to be cleared or timeout at 100ms
        ///
        PortResetTimeout = 0;
        do {
          Data32 = 0;
          for (UsbPort = 0; UsbPort < SsPortCount; UsbPort++) {
            Data32 |= MmioRead32 (XhciMmioBase + PORTSCxUSB3Ptr[UsbPort]);
          }
          PchPmTimerStall (TEN_MS_TIMEOUT);
          PortResetTimeout++;
        } while ((Data32 & B_PCH_XHCI_PORTSCXUSB3_PR) &&
                 (PortResetTimeout < PORT_RESET_TIMEOUT));
      }
    }
    if (UsbConfig->Usb30Settings.ManualMode == PCH_DEVICE_ENABLE) {
      ///
      /// Using the similar method as
      /// PCH BIOS Spec Rev 0.5.0 Section 13.2.5 Routing of switchable USB Ports to
      /// EHCI Controller
      /// Step 1
      /// Retrieve information about the number of implemented xHCI ports and set appropriate
      /// port mask registers
      /// Done in GetXhciPortCountAndSetPortRoutingMask()
      /// Step 2
      /// Based on available number of ports (from step 1) initiate port reset to enabled ports
      /// where USB 2.0 device is connected
      ///
      /// 2.a. For Port #1, if xHCIBAR + 480h [0] is sets then
      /// 2.b. Issue port reset by sets xHCIBAR + 480h [4] to 1b
      /// 2.f. Repeat steps #a to #e for all the USB2.0 ports.
      ///
      for (UsbPort = 0; UsbPort < HsPortCount; UsbPort++) {
        if (MmioRead32 (XhciMmioBase + PORTSCxUSB2Ptr[UsbPort]) & B_PCH_XHCI_PORTSCXUSB2_CCS) {
          MmioAndThenOr32 (
            XhciMmioBase + PORTSCxUSB2Ptr[UsbPort],
            (UINT32)~(B_PCH_XHCI_PORTSCXUSB2_PED),
            B_PCH_XHCI_PORTSCXUSB2_PR
            );
        }
      }
      ///
      /// 2.c. Poll for port reset bit at steps #b to be cleared or timeout at 100ms
      ///
      PortResetTimeout = 0;
      do {
        Data32 = 0;
        for (UsbPort = 0; UsbPort < HsPortCount; UsbPort++) {
          Data32 |= MmioRead32 (XhciMmioBase + PORTSCxUSB2Ptr[UsbPort]);
        }
        PchPmTimerStall (TEN_MS_TIMEOUT);
        PortResetTimeout++;
      } while ((Data32 & B_PCH_XHCI_PORTSCXUSB2_PR) &&
              (PortResetTimeout < PORT_RESET_TIMEOUT));
      ///
      /// 2.d. Program D20:F0:D0h[14:0] manually
      ///
      PortMask = 0;
      for (UsbPort = 0; UsbPort < HsPortCount; UsbPort++) {
        if (UsbConfig->Usb30Settings.ManualModeUsb20PerPinRoute[UsbPort]==1) {  // 0: EHCI; 1 :XHCI;
          PortMask |= 1 << UsbPort;
        }
      }
//[-start-130205-IB02950497-modify]//      
      if (UsbConfig->Usb30Settings.Mode == PCH_XHCI_MODE_ON) {
        MmioAndThenOr32 (
          XhciPciMmBase + R_PCH_XHCI_USB2PR,
          (UINT32)~B_PCH_XHCI_USB2PR_USB2HCSEL,
          PortMask
          );      	
      }
//[-end-130205-IB02950497-modify]//      
      ///
      /// 2.e. Clear all the port's status by program xHCIBAR + 480h [23:17] to 1111111b
      ///
      for (UsbPort = 0; UsbPort < HsPortCount; UsbPort++) {
        MmioAndThenOr32 (
          XhciMmioBase + PORTSCxUSB2Ptr[UsbPort],
          (UINT32)~ (B_PCH_XHCI_PORTSCXUSB2_PED),
          B_PCH_XHCI_PORTSCXUSB2_CEC |
          B_PCH_XHCI_PORTSCXUSB2_PLC |
          B_PCH_XHCI_PORTSCXUSB2_PRC |
          B_PCH_XHCI_PORTSCXUSB2_OCC |
          B_PCH_XHCI_PORTSCXUSB2_WRC |
          B_PCH_XHCI_PORTSCXUSB2_PEC |
          B_PCH_XHCI_PORTSCXUSB2_CSC
          );
      }
      if (HsUsbrPortCount > 0) {
        MmioAndThenOr32 (
          XhciMmioBase + PORTSCxUSB2Ptr[HsPortCount],
          (UINT32)~ (B_PCH_XHCI_PORTSCXUSB2_PED),
          B_PCH_XHCI_PORTSCXUSB2_CEC |
          B_PCH_XHCI_PORTSCXUSB2_PLC |
          B_PCH_XHCI_PORTSCXUSB2_PRC |
          B_PCH_XHCI_PORTSCXUSB2_OCC |
          B_PCH_XHCI_PORTSCXUSB2_WRC |
          B_PCH_XHCI_PORTSCXUSB2_PEC |
          B_PCH_XHCI_PORTSCXUSB2_CSC
          );
      }
      ///
      /// Step 3
      /// Initiate warm reset to all USB 3.0 ports
      ///
      /// 3.a. For Port #1,  sets xHCIBAR + 570h [31]
      /// 3.e. Repeat steps #a to #e for all the USB3.0 ports.
      ///
      for (UsbPort = 0; UsbPort < SsPortCount; UsbPort++) {
        MmioAndThenOr32 (
          XhciMmioBase + PORTSCxUSB3Ptr[UsbPort],
          (UINT32)~ (B_PCH_XHCI_PORTSCXUSB3_PED),
          B_PCH_XHCI_PORTSCXUSB3_WPR
          );
      }
      ///
      /// 3.b. Program D20:F0:D8h[5:0] manually.
      ///
      PortMask = 0;
      for (UsbPort = 0; UsbPort < SsPortCount; UsbPort++) {
        if (UsbConfig->Usb30Settings.ManualModeUsb30PerPinEnable[UsbPort]==1) {  // 0: Disable; 1:Enable;
          PortMask |= 1 << UsbPort;
        }
      }

//[-start-130205-IB02950497-modify]//
      if (UsbConfig->Usb30Settings.Mode == PCH_XHCI_MODE_ON) {   
        MmioAndThenOr32 (
          XhciPciMmBase + R_PCH_XHCI_USB3PR,
          (UINT32)~B_PCH_XHCI_USB3PR_USB3SSEN,
          PortMask
          );
      }
//[-end-130205-IB02950497-modify]// 

      ///
      /// 3.c. Poll for warm reset bit at steps #a to be cleared or timeout at 100ms
      ///
      PortResetTimeout = 0;
      do {
        Data32 = 0;
        for (UsbPort = 0; UsbPort < SsPortCount; UsbPort++) {
          Data32 |= MmioRead32 (XhciMmioBase + PORTSCxUSB3Ptr[UsbPort]);
        }
        PchPmTimerStall (TEN_MS_TIMEOUT);
        PortResetTimeout++;
      } while ((Data32 & B_PCH_XHCI_PORTSCXUSB3_PR) &&
              (PortResetTimeout < PORT_RESET_TIMEOUT));
      ///
      /// 3.d. Clear all the port's status by program xHCIBAR + 570h [23:17] to 1111111b.
      ///
      for (UsbPort = 0; UsbPort < SsPortCount; UsbPort++) {
        MmioAndThenOr32 (
          XhciMmioBase + PORTSCxUSB3Ptr[UsbPort],
          (UINT32)~ (B_PCH_XHCI_PORTSCXUSB3_PED),
          B_PCH_XHCI_PORTSCXUSB3_CEC |
          B_PCH_XHCI_PORTSCXUSB3_PLC |
          B_PCH_XHCI_PORTSCXUSB3_PRC |
          B_PCH_XHCI_PORTSCXUSB3_OCC |
          B_PCH_XHCI_PORTSCXUSB3_WRC |
          B_PCH_XHCI_PORTSCXUSB3_PEC |
          B_PCH_XHCI_PORTSCXUSB3_CSC
          );
      }
      ///
      /// Step 4
      /// Set the Run bit of xHCI controller, xHCIBAR + 80h[0] to 1b
      ///
      MmioOr32 (
        XhciMmioBase + R_PCH_XHCI_USBCMD,
        (UINT32) B_PCH_XHCI_USBCMD_RS
        );
      ///
      /// Step 5
      /// Then clear the Run bit of xHCI controller, xHCIBAR + 80h[0] to 0b
      ///
      MmioAnd32 (
        XhciMmioBase + R_PCH_XHCI_USBCMD,
        (UINT32) ~B_PCH_XHCI_USBCMD_RS
        );
    }
#ifdef USB_PRECONDITION_ENABLE_FLAG
  }
#endif  // USB_PRECONDITION_ENABLE_FLAG

#ifdef SUS_WELL_RESTORE
  ///
  /// To support RapidStart resume from G3 state, XHCI USB3 PR resume well registers
  /// get save into S3 Script table only when not running in Auto/Smart Auto mode.
  ///
  if (((UsbConfig->Usb30Settings.ManualMode == PCH_DEVICE_ENABLE)) ||
      ((UsbConfig->Usb30Settings.PreBootSupport == PCH_DEVICE_ENABLE) &&
       (UsbConfig->Usb30Settings.Mode == PCH_XHCI_MODE_ON))) {
    PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (XhciPciMmBase + R_PCH_XHCI_USB3PR),
      1,
      (VOID *) (UINTN) (XhciPciMmBase + R_PCH_XHCI_USB3PR)
      );
    PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (XhciPciMmBase + R_PCH_XHCI_USB2PR),
      1,
      (VOID *) (UINTN) (XhciPciMmBase + R_PCH_XHCI_USB2PR)
      );
  }
#endif

#ifdef USB_PRECONDITION_ENABLE_FLAG
  ///
  /// Check if XHCI disabled or auto even no preboot support, exit function directly
  ///
  if ((UsbConfig->Usb30Settings.Mode == PCH_XHCI_MODE_OFF) ||
      (UsbConfig->Usb30Settings.Mode == PCH_XHCI_MODE_AUTO) ||
      (UsbConfig->Usb30Settings.PreBootSupport == PCH_DEVICE_DISABLE)
      ) {
    return;
  }
  ///
  /// If precondition is enabled, execute USB precondition function by each phase call
  ///
  if (USB_PRECONDITION_POLICY_SUPPORT (UsbConfig)) {
     XHCI_PRECONDITION (
       (UINT8) ((XhciPciMmBase >> 20) &0xFF),
       PCI_DEVICE_NUMBER_PCH_XHCI,
       PCI_FUNCTION_NUMBER_PCH_XHCI,
       XhciMmioBase,
       PORTSCxUSB2Ptr,
       HsPortCount,
       PORTSCxUSB3Ptr,
       SsPortCount
       );
  }
#endif  // USB_PRECONDITION_ENABLE_FLAG
}

/**
  Retrieves information about number of implemented xHCI ports
  and sets appropriate port mask registers.

  @param[in]  XhciPciMmBase       XHCI PCI Base Address
  @param[out] HsPortCount         Count of High Speed Ports
  @param[out] HsUsbrPortCount     Count of USBr Port
  @param[out] SsPortCount         Count of Super Speed Ports

  @retval None
**/
VOID
GetXhciPortCountAndSetPortRoutingMask (
  IN  UINTN                       XhciPciMmBase,
  OUT UINTN                       *HsPortCount,
  OUT UINTN                       *HsUsbrPortCount,
  OUT UINTN                       *SsPortCount
  )
{
  UINT32      HsPortEnableMask;
  UINT32      SsPortEnableMask;
  PCH_SERIES  PchSeries;

  HsPortEnableMask = 0;
  SsPortEnableMask = 0;
  PchSeries        = GetPchSeries();
  ///
  /// PCH BIOS Spec Rev 0.5.0
  /// Section 13.2.5 Routing of switchable USB Ports to EHCI Controller
  /// Section 13.2.6 Routing of switchable USB Ports to xHCI Controller
  ///
  if (PchSeries == PchH) {
    ///
    /// Step 1.a Check xHCI D20:F0:E0h[2:1] to get HS Port Count.
    ///
    switch (MmioRead32 (XhciPciMmBase + R_PCH_XHCI_FUS) & B_PCH_XHCI_FUS_HSPRTCNT) {
      case V_PCH_XHCI_FUS_HSPRTCNT_11B:
        ///
        /// If the value is 11b: Set xHCI D20:F0:D4h[13:0] to 00FFh. Number of HS ports is 8.
        ///
        *HsPortCount      = V_PCH_H_XHCI_FUS_HSPRTCNT_11B_CNT;
        HsPortEnableMask  = V_PCH_H_XHCI_FUS_HSPRTCNT_11B_MASK;
        break;

      case V_PCH_XHCI_FUS_HSPRTCNT_10B:
        ///
        /// If the value is 10b: Set xHCI D20:F0:D4h[13:0] to 0FFFh. Number of HS ports is 10.
        /// It is work around. Bit 6 and 7 have to be set to 1 to enable USB2 ports.
        ///
        *HsPortCount      = V_PCH_H_XHCI_FUS_HSPRTCNT_10B_CNT;
        HsPortEnableMask  = V_PCH_H_XHCI_FUS_HSPRTCNT_10B_MASK;
        break;

      case V_PCH_XHCI_FUS_HSPRTCNT_01B:
        ///
        /// If the value is 01b: Set xHCI D20:F0:D4h[13:0] to 3FFFh. Number of HS ports is 12.
        /// It is work around. Bit 6 and 7 have to be set to 1 to enable USB2 ports.
        ///
        *HsPortCount      = V_PCH_H_XHCI_FUS_HSPRTCNT_01B_CNT;
        HsPortEnableMask  = V_PCH_H_XHCI_FUS_HSPRTCNT_01B_MASK;
        break;

      case V_PCH_XHCI_FUS_HSPRTCNT_00B:
        ///
        /// If the value is 00b: Set xHCI D20:F0:D4h[13:0] to 3FFFh. Number of HS ports is 14
        ///
      default:
        *HsPortCount      = V_PCH_H_XHCI_FUS_HSPRTCNT_00B_CNT;
        HsPortEnableMask  = V_PCH_H_XHCI_FUS_HSPRTCNT_00B_MASK;
        break;
    }
    ///
    /// Step 1.b Check xHCI D20:F0:E0h[4:3] to get SS Port Count.
    ///
    switch (MmioRead32 (XhciPciMmBase + R_PCH_XHCI_FUS) & B_PCH_XHCI_FUS_SSPRTCNT) {
      case V_PCH_XHCI_FUS_SSPRTCNT_11B:
        ///
        /// If the value is 11b: Set xHCI D20:F0:DCh[5:0] to 000000b. Number of SS ports is 0.
        ///
        *SsPortCount      = V_PCH_H_XHCI_FUS_SSPRTCNT_11B_CNT;
        SsPortEnableMask  = V_PCH_H_XHCI_FUS_SSPRTCNT_11B_MASK;
        break;

      case V_PCH_XHCI_FUS_SSPRTCNT_10B:
        ///
        /// If the value is 10b: Set xHCI D20:F0:DCh[5:0] to 000011b. Number of SS ports is 2.
        ///
        *SsPortCount      = V_PCH_H_XHCI_FUS_SSPRTCNT_10B_CNT;
        SsPortEnableMask  = V_PCH_H_XHCI_FUS_SSPRTCNT_10B_MASK;
        break;

      case V_PCH_XHCI_FUS_SSPRTCNT_01B:
        ///
        /// If the value is 01b: Set xHCI D20:F0:DCh[5:0] to 001111b. Number of SS ports is 4.
        ///
        *SsPortCount      = V_PCH_H_XHCI_FUS_SSPRTCNT_01B_CNT;
        SsPortEnableMask  = V_PCH_H_XHCI_FUS_SSPRTCNT_01B_MASK;
        break;

      case V_PCH_XHCI_FUS_SSPRTCNT_00B:
        ///
        /// If the value is 00b: Set xHCI D20:F0:DCh[5:0] to 111111b. Number of SS ports is 6.
        ///
      default:
        *SsPortCount      = V_PCH_H_XHCI_FUS_SSPRTCNT_00B_CNT;
        SsPortEnableMask  = V_PCH_H_XHCI_FUS_SSPRTCNT_00B_MASK;
        break;
    }
    ///
    /// Step 1.c Check xHCI D20:F0:E0h[5] to know if USBr is enabled.
    /// @todo Need more comments to understand this
    ///
    switch (MmioRead32 (XhciPciMmBase + R_PCH_XHCI_FUS) & B_PCH_XHCI_FUS_USBR) {
      case V_PCH_XHCI_FUS_USBR_EN:
        ///
        /// If 0b: Set xHCI D20:F0:D4[14] to 1b. USBr port is enabled.
        ///
        *HsUsbrPortCount = 1;
        HsPortEnableMask |= BIT14;
        break;

      case V_PCH_XHCI_FUS_USBR_DIS:
        ///
        /// If 1b: Set xHCI D20:F0:D4[14] to 0b. USBr port is disabled.
        ///
        *HsUsbrPortCount = 0;
        HsPortEnableMask &= (~BIT14);
        break;
    }
  } else if (PchSeries == PchLp) {
    ///
    /// Step 1.a LPT-LP has a fixed number of 8 HS ports.  Set xHCI D20:F0:D4h[13:0] to 00FFh. 
    ///
     *HsPortCount      = V_PCH_LP_XHCI_FIXED_HSPRTCNT;
     HsPortEnableMask  = V_PCH_LP_XHCI_FIXED_HSPRTCNT_MASK;
    ///
    /// Step 1.b LPT-LP has a fixed number of 4 SS ports.  Set xHCI D20:F0:DCh[3:0] to 0Fh.
    ///
    *SsPortCount      = V_PCH_LP_XHCI_FIXED_SSPRTCNT;
    SsPortEnableMask  = V_PCH_LP_XHCI_FIXED_SSPRTCNT_MASK;
    ///
    /// Step 1.c Check xHCI D20:F0:E0h[5] to know if USBr is enabled.
    /// @todo Need more comments to understand this
    ///
    switch (MmioRead32 (XhciPciMmBase + R_PCH_XHCI_FUS) & B_PCH_XHCI_FUS_USBR) {
      case V_PCH_XHCI_FUS_USBR_EN:
        ///
        /// If 0b: Set xHCI D20:F0:D4[8] to 1b. USBr port is enabled.
        ///
        *HsUsbrPortCount = 1;
        HsPortEnableMask |= BIT8;
        break;

      case V_PCH_XHCI_FUS_USBR_DIS:
        ///
        /// If 1b: Set xHCI D20:F0:D4[8] to 0b. USBr port is disabled.
        ///
        *HsUsbrPortCount = 0;
        HsPortEnableMask &= (~BIT8);
        break;
    }
  }

 //Routing the USB ports 12, 8, and 5 to EHCI to avoid  OC detection when wakening up from S3

  #ifdef UPSERVER_SUPPORT 
    HsPortEnableMask &=~(BIT12|BIT8|BIT5); 
  #endif
  
  ///
  /// Set xHCI USB2 Port Routing Mask register (D20:F0:D4h[14:0])
  /// per HS Port Enable Mask value
  ///
  MmioAndThenOr32 (
    XhciPciMmBase + R_PCH_XHCI_USB2PRM,
    ~ (UINT32) B_PCH_XHCI_USB2PR_USB2HCSELM,
    HsPortEnableMask
    );
  PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (XhciPciMmBase + R_PCH_XHCI_USB2PRM),
    1,
    (VOID *) (UINTN) (XhciPciMmBase + R_PCH_XHCI_USB2PRM)
    );
  ///
  /// Set xHCI USB3 Port Routing Mask register (D20:F0:DCh[5:0])
  /// per SS Port Enable Mask value
  ///
  MmioAndThenOr32 (
    XhciPciMmBase + R_PCH_XHCI_USB3PRM,
    ~ (UINT32) B_PCH_XHCI_USB3PR_USB3SSENM,
    SsPortEnableMask
    );
  PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (XhciPciMmBase + R_PCH_XHCI_USB3PRM),
    1,
    (VOID *) (UINTN) (XhciPciMmBase + R_PCH_XHCI_USB3PRM)
    );
}

/**
  Setup XHCI Over-Current Mapping

  @param[in] UsbConfig            The PCH Platform Policy for USB configuration
  @param[in] XhciPciMmBase        XHCI PCI Base Address

  @retval None
**/
VOID
XhciOverCurrentMapping (
  IN  PCH_USB_CONFIG              *UsbConfig,
  IN  UINTN                       XhciPciMmBase
  )
{
  ///
  ///  BIOS responsibility on Overcurrent protection.
  ///  ----------------------------------------------
  ///  There are 8 total overcurrent pins
  ///  which can be map to 14 USB2 ports and 6 USB3 ports.
  ///  On a given physical connector,
  ///  one OC pin is shared between the USB2 (HS) pins and USB3 (SS) pins.
  ///  USB2 (HS) pins are programmable to be owned by either XHCI or EHCI.
  ///  OC pins are associated to the current owner.
  ///  USB2 (HS) ports 1-8 use OC pins 1-4, ports 9-14 use OC pins 4-8
  ///  USB3 (SS) ports has the flexibility in pairing with any of the OC pins.
  ///  It is ok to map multiple ports to a single pin.
  ///  It is not ok to map a single ports to a multiple pins.
  ///  All USB ports routed out of the package must have Overcurrent protection.
  ///  USB Ports not routed out from the package should not be assigned OC pins.
  ///
  UINT32                   Index;
  UINT32                   CurrentIndex;
  UINT32                   XhciHsOcm1;
  UINT32                   XhciHsOcm2;
  UINT32                   XhciSsOcm1;
  UINT32                   XhciSsOcm2;
  PCH_SERIES               PchSeries;

  PchSeries = GetPchSeries();
  ///
  /// Check if XHCI disabled, Exit function.
  ///
  if (UsbConfig->Usb30Settings.Mode == PCH_XHCI_MODE_OFF) {
    return;
  }
  ///
  /// Find the corresponding register and set the bits
  ///
  XhciHsOcm1  = 0;
  XhciHsOcm2  = 0;
  XhciSsOcm1  = 0;
  XhciSsOcm2  = 0;

  for (Index = 0; Index < GetPchUsbMaxPhysicalPortNum (); Index++) {
    if (UsbConfig->Usb20OverCurrentPins[Index] == PchUsbOverCurrentPinSkip) {
      ///
      /// No OC pin assigned, skip this port
      ///
    } else {
      if (Index < 8) {
        ///
        /// Port 0-7: OC0 - OC3
        ///
        if (UsbConfig->Usb20OverCurrentPins[Index] > PchUsbOverCurrentPin3) {
          ASSERT (FALSE);
          continue;
        }

        CurrentIndex = UsbConfig->Usb20OverCurrentPins[Index] * 8 + Index;
        XhciHsOcm1 |= (UINT32) (BIT0 << CurrentIndex);
      } else {
        ///
        /// Port 8-13: OC4 - OC7
        ///
        if ((UsbConfig->Usb20OverCurrentPins[Index] < PchUsbOverCurrentPin4) ||
            (UsbConfig->Usb20OverCurrentPins[Index] > PchUsbOverCurrentPin7)) {
          ASSERT (FALSE);
          continue;
        }

        CurrentIndex = (UsbConfig->Usb20OverCurrentPins[Index] - 4) * 8 + (Index - 8);
        XhciHsOcm2 |= (UINT32) (BIT0 << CurrentIndex);
      }
    }
  }

  for (Index = 0; Index < GetPchXhciMaxUsb3PortNum (); Index++) {
    if (UsbConfig->Usb30OverCurrentPins[Index] == PchUsbOverCurrentPinSkip) {
      ///
      /// No OC pin assigned, skip this port
      ///
    } else {
      ///
      /// Port 0-5: OC0 - OC7
      ///
      if (UsbConfig->Usb30OverCurrentPins[Index] < PchUsbOverCurrentPin4) {
        CurrentIndex = UsbConfig->Usb30OverCurrentPins[Index] * 8 + Index;
        XhciSsOcm1 |= (UINT32) (BIT0 << CurrentIndex);
      } else {
        CurrentIndex = (UsbConfig->Usb30OverCurrentPins[Index] - 4) * 8 + Index;
        XhciSsOcm2 |= (UINT32) (BIT0 << CurrentIndex);
      }
    }
  }
  ///
  /// OCM registers are in the suspend well.
  ///
  MmioWrite32 (XhciPciMmBase + R_PCH_XHCI_U2OCM1, XhciHsOcm1);
  MmioWrite32 (XhciPciMmBase + R_PCH_XHCI_U3OCM1, XhciSsOcm1);
  if (PchSeries == PchH) {
    MmioWrite32 (XhciPciMmBase + R_PCH_XHCI_U2OCM2, XhciHsOcm2);
    MmioWrite32 (XhciPciMmBase + R_PCH_XHCI_U3OCM2, XhciSsOcm2);
  }

#ifdef SUS_WELL_RESTORE
  ///
  /// To support RapidStart resume from G3 state, all resume well registers need to be saved
  /// into S3 Script table.
  ///
  PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (XhciPciMmBase + R_PCH_XHCI_U2OCM1),
    1,
    (VOID *) (UINTN) (XhciPciMmBase + R_PCH_XHCI_U2OCM1)
    );
  PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (XhciPciMmBase + R_PCH_XHCI_U3OCM1),
    1,
    (VOID *) (UINTN) (XhciPciMmBase + R_PCH_XHCI_U3OCM1)
    );
  if (PchSeries == PchH) {
    PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (XhciPciMmBase + R_PCH_XHCI_U2OCM2),
      1,
      (VOID *) (UINTN) (XhciPciMmBase + R_PCH_XHCI_U2OCM2)
      );
    PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (XhciPciMmBase + R_PCH_XHCI_U3OCM2),
      1,
      (VOID *) (UINTN) (XhciPciMmBase + R_PCH_XHCI_U3OCM2)
      );
  }
#endif // SUS_WELL_RESTORE
}

/**
  Setup EHCI Over-Current Mapping

  @param[in] UsbConfig            The PCH Platform Policy for USB configuration
  @param[in] Ehci1PciMmBase       EHCI 1 PCI Base Address
  @param[in] Ehci2PciMmBase       EHCI 2 PCI Base Address

  @retval None
**/
VOID
EhciOverCurrentMapping (
  IN  PCH_USB_CONFIG              *UsbConfig,
  IN  UINTN                       Ehci1PciMmBase,
  IN  UINTN                       Ehci2PciMmBase
  )
{
  UINT32          Index;
  UINT32          CurrentIndex;
  UINT32          Ehci1Ocm;
  UINT32          Ehci2Ocm;
  PCH_SERIES      PchSeries;

  Ehci1Ocm  = 0;
  Ehci2Ocm  = 0;
  PchSeries = GetPchSeries();

  for (Index = 0; Index < GetPchUsbMaxPhysicalPortNum (); Index++) {
    if (UsbConfig->Usb20OverCurrentPins[Index] == PchUsbOverCurrentPinSkip) {
      ///
      /// No OC pin assigned, skip this port
      ///
    } else {
      if (Index < 8) {
        ///
        /// Port 0~7  -> OC0~3
        ///
        if (UsbConfig->Usb20OverCurrentPins[Index] > PchUsbOverCurrentPin3) {
          ASSERT (FALSE);
          continue;
        }

        CurrentIndex = UsbConfig->Usb20OverCurrentPins[Index] * 8 + Index;
        Ehci1Ocm |= (UINT32) (BIT0 << CurrentIndex);
      } else {
        if (PchSeries == PchH) {
          ///
          /// Port 8~13 -> OC4~7
          ///
          if ((UsbConfig->Usb20OverCurrentPins[Index] < PchUsbOverCurrentPin4) ||
              (UsbConfig->Usb20OverCurrentPins[Index] > PchUsbOverCurrentPin7)) {
            ASSERT (FALSE);
            continue;
          }
          CurrentIndex = (UsbConfig->Usb20OverCurrentPins[Index] - 4) * 8 + (Index - 8);
          Ehci2Ocm |= (UINT32) (BIT0 << CurrentIndex);
        }
      }
    }
  }
  ///
  /// EHCI1OCM and EHCI2OCM are in the suspend well.
  ///
  if (UsbConfig->Usb20Settings[PchEhci1].Enable == PCH_DEVICE_ENABLE) {
    MmioWrite32 (Ehci1PciMmBase + R_PCH_EHCI_OCMAP, Ehci1Ocm);
  }

  if (PchSeries == PchH) {
    if (UsbConfig->Usb20Settings[PchEhci2].Enable == PCH_DEVICE_ENABLE) {
      MmioWrite32 (Ehci2PciMmBase + R_PCH_EHCI_OCMAP, Ehci2Ocm);
    }
  }

#ifdef SUS_WELL_RESTORE
  ///
  /// To support RapidStart resume from G3 state, all resume well registers need to be saved
  /// into S3 Script table.
  ///
  if (UsbConfig->Usb20Settings[PchEhci1].Enable == PCH_DEVICE_ENABLE) {
    PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (Ehci1PciMmBase + R_PCH_EHCI_OCMAP),
      1,
      (VOID *) (UINTN) (Ehci1PciMmBase + R_PCH_EHCI_OCMAP)
      );
  }

  if (PchSeries == PchH) {
    if (UsbConfig->Usb20Settings[PchEhci2].Enable == PCH_DEVICE_ENABLE) {
      PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (Ehci2PciMmBase + R_PCH_EHCI_OCMAP),
        1,
        (VOID *) (UINTN) (Ehci2PciMmBase + R_PCH_EHCI_OCMAP)
        );
    }
  }
#endif
}

/**
  Program Ehci Port Disable Override

  @param[in] UsbConfig            The PCH Platform Policy for USB configuration
  @param[in] Ehci1PciMmBase       EHCI 1 PCI Base Address
  @param[in] Ehci2PciMmBase       EHCI 2 PCI Base Address

  @retval None
**/
VOID
EhciPortDisableOverride (
  IN  PCH_USB_CONFIG              *UsbConfig,
  IN  UINTN                       Ehci1PciMmBase,
  IN  UINTN                       Ehci2PciMmBase
  )
{
  UINT32          Index;
  PCH_SERIES      PchSeries;

  PchSeries = GetPchSeries();
  ///
  /// PCH BIOS Spec Rev 0.5.0 Section 12.2 Disabling USB Ports
  /// System BIOS may choose to disable individual USB ports to save power or for security
  /// purposes. Each of the USB ports has a corresponding bit within the PCH USB Port
  /// Disable Override Register (D26/29:F0 + 64h). The PCH USB Port Disable Override
  /// Register can be locked by setting the Write Enable bit of the PCH USB Per-Port Register
  /// Write Control Register, PMBASE + 3Ch[1]. Refer to the PCH EDS for more details on
  /// these registers.
  ///
  for (Index = 0; Index < GetPchUsbMaxPhysicalPortNum (); Index++) {
    if ((Index < 8) && (UsbConfig->Usb20Settings[PchEhci1].Enable == PCH_DEVICE_ENABLE)) {
      ///
      /// EHCI1 PDO for Port 0 to 7
      ///
      if (UsbConfig->PortSettings[Index].Enable == PCH_DEVICE_DISABLE) {
        MmioOr8 (Ehci1PciMmBase + R_PCH_EHCI_PDO, (UINT8) (B_PCH_EHCI_PDO_DIS_PORT0 << Index));
      } else {
        MmioAnd8 (Ehci1PciMmBase + R_PCH_EHCI_PDO, (UINT8) ~(B_PCH_EHCI_PDO_DIS_PORT0 << Index));
      }
    }
    if (PchSeries == PchH) {
      if ((Index < 14) && (UsbConfig->Usb20Settings[PchEhci2].Enable == PCH_DEVICE_ENABLE)) {
        ///
        /// EHCI2 PDO for Port 8 to 13
        ///
        if (UsbConfig->PortSettings[Index].Enable == PCH_DEVICE_DISABLE) {
          MmioOr8 (Ehci2PciMmBase + R_PCH_EHCI_PDO, (UINT8) (B_PCH_EHCI_PDO_DIS_PORT0 << (Index - 8)));
        } else {
          MmioAnd8 (Ehci2PciMmBase + R_PCH_EHCI_PDO, (UINT8) ~(B_PCH_EHCI_PDO_DIS_PORT0 << (Index - 8)));
        }
      }
    }
  }

#ifdef SUS_WELL_RESTORE
  ///
  /// To support RapidStart resume from G3 state, all resume well registers need to be saved
  /// into S3 Script table.
  ///
  if (UsbConfig->Usb20Settings[PchEhci1].Enable == PCH_DEVICE_ENABLE) {
    PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint16,
      (UINTN) (Ehci1PciMmBase + R_PCH_EHCI_PDO),
      1,
      (VOID *) (UINTN) (Ehci1PciMmBase + R_PCH_EHCI_PDO)
      );
  }

  if (PchSeries == PchH) {
    if (UsbConfig->Usb20Settings[PchEhci2].Enable == PCH_DEVICE_ENABLE) {
      PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint16,
        (UINTN) (Ehci2PciMmBase + R_PCH_EHCI_PDO),
        1,
        (VOID *) (UINTN) (Ehci2PciMmBase + R_PCH_EHCI_PDO)
        );
    }
  }
#endif // SUS_WELL_RESTORE
}

/**
  Program Xhci Port Disable Override

  @param[in] UsbConfig            The PCH Platform Policy for USB configuration
  @param[in] XhciPciMmBase        XHCI PCI Base Address
  @param[in] Revision             Platform policy revision

  @retval None
**/
VOID
XhciPortDisableOverride (
  IN  PCH_USB_CONFIG              *UsbConfig,
  IN  UINTN                       XhciPciMmBase,
  IN  UINT8                       Revision
  )
{
  UINT32          Index;
  UINT32          XhciUsb2Pdo;
  UINT32          XhciUsb3Pdo;
  UINT32          XhciIndex;
  PCH_SERIES      PchSeries;

  PchSeries = GetPchSeries();
  ///
  /// Check if XHCI disabled, Exit function.
  ///
  if (UsbConfig->Usb30Settings.Mode == PCH_XHCI_MODE_OFF) {
    return;
  }
  ///
  /// PCH BIOS Spec Rev 0.5.0 Section 13.2.2 Port Disable Override
  /// In LynxPoint PCH, Port disable override on xHCI ports is implemented by mapping the
  /// appropriate ports to the EHCI controller and then setting the port disable override in
  /// the EHCI function. Please refer to section 12.2.
  /// Please note that there is a corresponding disable bit on D20:F0:E4h[14:0] for USB 2.0
  /// ports and D20:F0:E8h[5:0] for USB 3.0 ports. BIOS needs to program them accordingly.
  ///
  /// XHCI PDO for HS
  ///
  XhciUsb2Pdo = MmioRead32 (XhciPciMmBase + R_PCH_XHCI_USB2PDO) & B_PCH_XHCI_USB2PDO_MASK;
  for (Index = 0; Index < GetPchUsbMaxPhysicalPortNum (); Index++) {
    XhciIndex = Index;
    if (PchSeries == PchH) {
      ///
      /// Translate physical pins to internal ports numbering
      ///
      XhciIndex = XhciUsb2InternalPortNumberLookUpTable[Index];
    }
    if (UsbConfig->PortSettings[Index].Enable == PCH_DEVICE_DISABLE) {
      XhciUsb2Pdo |= (UINT32) (B_PCH_XHCI_USB2PDO_DIS_PORT0 << XhciIndex);
    } else {
      XhciUsb2Pdo &= (UINT32)~(B_PCH_XHCI_USB2PDO_DIS_PORT0 << XhciIndex);
    }
  }
  ///
  /// XHCI PDO for SS
  ///
  XhciUsb3Pdo = MmioRead32 (XhciPciMmBase + R_PCH_XHCI_USB3PDO) & B_PCH_XHCI_USB3PDO_MASK;
  for (Index = 0; Index < GetPchXhciMaxUsb3PortNum (); Index++) {

    if (USB3PORT_SETTING_POLICY_SUPPORT(Revision)){
      if (UsbConfig->Port30Settings[Index].Enable == PCH_DEVICE_DISABLE) {
        XhciUsb3Pdo |= (UINT32) (B_PCH_XHCI_USB3PDO_DIS_PORT0 << Index);
      } else {
        XhciUsb3Pdo &= (UINT32)~(B_PCH_XHCI_USB3PDO_DIS_PORT0 << Index);
      }
    } else {
      if (UsbConfig->PortSettings[Index].Enable == PCH_DEVICE_DISABLE) {
        XhciUsb3Pdo |= (UINT32) (B_PCH_XHCI_USB3PDO_DIS_PORT0 << Index);
      } else {
        XhciUsb3Pdo &= (UINT32)~(B_PCH_XHCI_USB3PDO_DIS_PORT0 << Index);
      }
    }
  }
  ///
  /// USB2PDO and USB3PDO are Write-Once registers and bits in them are in the SUS Well.
  ///
  MmioWrite32 (XhciPciMmBase + R_PCH_XHCI_USB2PDO, XhciUsb2Pdo);
  MmioWrite32 (XhciPciMmBase + R_PCH_XHCI_USB3PDO, XhciUsb3Pdo);
#ifdef SUS_WELL_RESTORE
  ///
  /// To support RapidStart resume from G3 state, all resume well registers need to be saved
  /// into S3 Script table.
  ///
  PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (XhciPciMmBase + R_PCH_XHCI_USB2PDO),
    1,
    (VOID *) (UINTN) (XhciPciMmBase + R_PCH_XHCI_USB2PDO)
    );
  PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (XhciPciMmBase + R_PCH_XHCI_USB3PDO),
    1,
    (VOID *) (UINTN) (XhciPciMmBase + R_PCH_XHCI_USB3PDO)
    );
#endif
}

/**
  Enable EHCI USBR device

  @param[in] EhciPciMmBase        Ehci PCI Base Address

  @retval None
**/
VOID
EhciUsbrEnable (
  IN  UINTN                       EhciPciMmBase
  )
{
  ///
  /// NOTE: EHCI USBR Enable
  /// EHCI1_USBr_en and EHCI2_USBr_en are mutually exclusive. Both cannot be set to 1 at any one time.
  /// SW must ensure at any one time, only 1 EHCI should have the bit set.
  ///
  MmioOr16 (EhciPciMmBase + 0x7A, (UINT16) BIT8);
#ifdef SUS_WELL_RESTORE
  ///
  /// To support RapidStart resume from G3 state, all resume well registers need to be saved
  /// into S3 Script table.
   ///
  PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint16,
    (UINTN) (EhciPciMmBase + 0x7A),
    1,
    (VOID *) (UINTN) (EhciPciMmBase + 0x7A)
    );
#endif
}

/**
  Program and enable XHCI Memory Space

  @param[in] UsbConfig            The PCH Platform Policy for USB configuration
  @param[in] XhciMmioBase         Memory base address of XHCI Controller
  @param[in] XhciPciMmBase        XHCI PCI Base Address

  @retval None
**/
VOID
XhciMemorySpaceOpen (
  IN  PCH_USB_CONFIG              *UsbConfig,
  IN  UINT32                      XhciMmioBase,
  IN  UINTN                       XhciPciMmBase
  )
{
  ///
  /// Check if XHCI disabled, Exit function.
  ///
  if (UsbConfig->Usb30Settings.Mode == PCH_XHCI_MODE_OFF) {
    return;
  }
  ///
  /// Assign memory resources
  ///
  MmioWrite32 (XhciPciMmBase + R_PCH_XHCI_MEM_BASE, XhciMmioBase);
  PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (XhciPciMmBase + R_PCH_XHCI_MEM_BASE),
    1,
    (VOID *) (UINTN) (XhciPciMmBase + R_PCH_XHCI_MEM_BASE)
    );

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
}

/**
  Clear and disable XHCI Memory Space

  @param[in] UsbConfig            The PCH Platform Policy for USB configuration
  @param[in] XhciMmioBase         Memory base address of XHCI Controller
  @param[in] XhciPciMmBase        XHCI PCI Base Address

  @retval None
**/
VOID
XhciMemorySpaceClose (
  IN  PCH_USB_CONFIG              *UsbConfig,
  IN  UINT32                      XhciMmioBase,
  IN  UINTN                       XhciPciMmBase
  )
{
  ///
  /// Check if XHCI disabled, Exit function.
  ///
  if (UsbConfig->Usb30Settings.Mode == PCH_XHCI_MODE_OFF) {
    return;
  }
  ///
  /// Clear memory resources
  ///
  MmioAnd16 (
    XhciPciMmBase + R_PCH_XHCI_COMMAND_REGISTER,
    (UINT16)~(B_PCH_XHCI_COMMAND_MSE | B_PCH_XHCI_COMMAND_BME)
    );
  PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint16,
    (UINTN) (XhciPciMmBase + R_PCH_XHCI_COMMAND_REGISTER),
    1,
    (VOID *) (UINTN) (XhciPciMmBase + R_PCH_XHCI_COMMAND_REGISTER)
    );

  MmioWrite32 ((XhciPciMmBase + R_PCH_XHCI_MEM_BASE), 0);
  PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (XhciPciMmBase + R_PCH_XHCI_MEM_BASE),
    1,
    (VOID *) (UINTN) (XhciPciMmBase + R_PCH_XHCI_MEM_BASE)
    );
}

/**
  Tune the USB 2.0 high-speed signals quality.

  @param[in] UsbConfig            The PCH Platform Policy for USB configuration
  @param[in] LpcDeviceId          The device ID of LPC
  @param[in] RootComplexBar       RootComplexBar value of this PCH device

  @retval EFI_SUCCESS             Successfully completed
  @retval EFI_DEVICE_ERROR        Programming is failed
**/
VOID
Usb2PortLengthProgramming (
  IN  PCH_USB_CONFIG              *UsbConfig,
  IN  UINT16                      LpcDeviceId,
  IN  UINT32                      RootComplexBar
  )
{
  EFI_STATUS      Status;
  UINT32          Index;
  UINT32          Data32And;
  UINT32          Data32Or;
#ifdef USB_PRECONDITION_ENABLE_FLAG
  PCH_SERIES      PchSeries;
  PchSeries       = GetPchSeries();
  ///
  /// Execute the code if running in PEI phase when USB precondition feature is enabled
  ///  or in DXE phase when USB precondition feature disabled
  /// If the precondition is enabled and running in DXE phase, 
  /// the code has already run once in PEI but the save S3 script need to run again in DXE phase
  /// but only run if and only if both EHCI is not disabled
  ///
  if ((USB_RUN_IN_PEI || (!USB_PRECONDITION_POLICY_SUPPORT (UsbConfig))) ||
      ((USB_RUN_IN_DXE) && ((UsbConfig->Usb20Settings[PchEhci1].Enable == PCH_DEVICE_ENABLE) ||
       ((UsbConfig->Usb20Settings[PchEhci2].Enable == PCH_DEVICE_ENABLE) && (PchSeries == PchH)))))
  {
#endif  // USB_PRECONDITION_ENABLE_FLAG

    //
    // Set EHCI AFE USB2 PER PORT register
    // IOBP registers 0xE5004000 + ((PortNumber+1) * 0x100)
    //
    for (Index = 0; Index < GetPchUsbMaxPhysicalPortNum (); Index++) {
      Data32And = (UINT32)~(0x00007F00);
      Data32Or  = (UINT32) (0x00000000);                                                     // BIT[14]    (PERPORTTXPEHALF) = 0
      ASSERT (UsbConfig->PortSettings[Index].Usb20EyeDiagramTuningParam2 < 8);
      Data32Or |= (UsbConfig->PortSettings[Index].Usb20EyeDiagramTuningParam2 & 0x07) << 11; // BIT[13:11] (PERPORTPETXISET) = Usb20EyeDiagramTuningParam2
      ASSERT (UsbConfig->PortSettings[Index].Usb20EyeDiagramTuningParam1 < 8);
      Data32Or |= (UsbConfig->PortSettings[Index].Usb20EyeDiagramTuningParam1 & 0x07) << 8;  // BIT[10:08] (PERPORTTXISET)   = Usb20EyeDiagramTuningParam1
      Status = ProgramIobp (
                RootComplexBar,
                0xE5004000 + ((Index + 1) * 0x100),
                Data32And,
                Data32Or
                );
      ASSERT_EFI_ERROR (Status);
      Status = PCH_INIT_COMMON_SCRIPT_SAVE_IOBP_S3_ITEM (
                RootComplexBar,
                0xE5004000 + ((Index + 1) * 0x100),
                Data32And,
                Data32Or
                );
      ASSERT_EFI_ERROR (Status);
    }
#ifdef USB_PRECONDITION_ENABLE_FLAG
  }
#endif  // USB_PRECONDITION_ENABLE_FLAG
}

VOID
ConfigureUsbClockGating (
  IN     DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy,
  IN     UINT32                               RootComplexBar
)
{
  ConfigureEhciClockGating(PchPlatformPolicy,RootComplexBar);
  ConfigureXhciClockGating(PchPlatformPolicy,RootComplexBar);
}

VOID
ConfigureEhciClockGating (
  IN     DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy,
  IN     UINT32                               RootComplexBar
)
{
  UINTN           EhciPciMmBase;
  UINT8           Index;
  UINT32          Data32And;
  UINT32          Data32Or;
  EFI_STATUS      Status;
  PCH_SERIES      PchSeries;
  PchSeries = GetPchSeries();
  //
  // Enable EHCI Clock Gating
  //

  ///
  /// If LPT-LP when EHCI disabled, Set RCBA + Offset 3A84[2,0] = 1b, 1b
  ///
  if (PchSeries == PchLp) {
    if (PchPlatformPolicy->UsbConfig->Usb20Settings[PchEhci1].Enable == PCH_DEVICE_DISABLE) {
      MmioOr32 (
        (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3A84),
        (UINT32) (BIT2 | BIT0)
        );
      PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3A84),
        1,
        (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3A84)
        );
    }
  }

  ///
  /// Set IOBP register 0xE5004001[7:6] to 11b.
  ///
  Data32And = (UINT32)~(0);
  Data32Or  = (UINT32) (BIT7 | BIT6);
  Status = ProgramIobp (
            RootComplexBar,
            0xE5004001,
            Data32And,
            Data32Or
            );
  ASSERT_EFI_ERROR (Status);
  Status = PCH_INIT_COMMON_SCRIPT_SAVE_IOBP_S3_ITEM (
            RootComplexBar,
            0xE5004001,
            Data32And,
            Data32Or
            );
  ///
  /// For each EHCI's PCI Config space registers
  ///
  for (Index = 0; Index < GetPchEhciMaxControllerNum (); Index++) {
    EhciPciMmBase = MmPciAddress (
                      0,
                      PchPlatformPolicy->BusNumber,
                      EhciControllersMap[Index].Device,
                      EhciControllersMap[Index].Function,
                      0
                      );
    ///
    /// Set D29/D26:F0 + DCh[5,2,1] to 1b
    /// Set D29/D26:F0 + DCh[0] to 1b when EHCI controller is disable
    ///
    Data32Or = (UINT32) (BIT5 | BIT2 | BIT1);
    if (PchPlatformPolicy->UsbConfig->Usb20Settings[Index].Enable == PCH_DEVICE_DISABLE) {
      Data32Or |= (UINT32) (BIT0);
    }
    MmioOr32 (EhciPciMmBase + 0xDC, Data32Or);
    PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (EhciPciMmBase + 0xDC),
      1,
      (VOID *) (UINTN) (EhciPciMmBase + 0xDC)
      );
    ///
    /// Set D29/D26:F0 + 78h[1:0] to 11b
    ///
    Data32Or = (UINT32) (BIT1 | BIT0);
    MmioOr32 (EhciPciMmBase + 0x78, Data32Or);
    PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (EhciPciMmBase + 0x78),
      1,
      (VOID *) (UINTN) (EhciPciMmBase + 0x78)
      );
    ASSERT_EFI_ERROR (Status);
  }
}

