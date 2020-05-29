/** @file
  PCH USB precondition feature support in DXE phase

@copyright
  Copyright (c) 2012 - 2013 Intel Corporation. All rights reserved
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
#include "PchUsbPrecondition.h"
#include "PchUsbCommon.h"

#ifdef USB_PRECONDITION_ENABLE_FLAG
//
// Data referred by EHCI
//
extern USB_CONTROLLER             EhciControllersMap[];

//
// Data referred by XHCI
//
UINTN                             *PORTSCxUSB2Ptr;
UINTN                             *PORTSCxUSB3Ptr;

//
// Data referred by USB Precondition feature
//
EFI_USB_HC_PORT_PRECONDITION      *mPrivatePreConditionList = NULL;

//
// This flag set when 50ms root port reset duration is satisified (Tdrstr). It is countered from
// last root port reset.
//
BOOLEAN                           PchUsbRPortsRstDoneFlag = FALSE;

//
// All root ports reset continuously, so the reset starting time between first root port and last
// root port should not exceed PCH ACPI timer High-to-Low transition frequency - 2.3435 seconds.
//
UINTN                             LastRPortResetTicks = 0;

//
// Tdrstr for all root portis satisfied as the following scenarios:
//
//  |
//  |-> Reset all root ports of 1st HC, save tick_1 to LastRPortResetTicks
//  |
//  |-> Reset all root ports of 2nd HC, save tick_2 to LastRPortResetTicks
//  |
//  |-> IsRootPortReset () for is invoked by first call, wait until if delay for tick_2 is enough
//  |   Set PchUsbRPortsRstDoneFlag = TRUE, return TRUE if the port is in the list
//  |
//  |-> IsRootPortReset () is invoked for the other HC, and PchUsbRPortsRstDoneFlag is set
//  |   Return TRUE if the port is in the list
//

/**
  Return current PCH PM1 timer value

  @param[in]                      None

  @retval                         PM1 timer value in 32 bit
**/
UINTN
PchGetPchTimerTick (
  VOID
  )
{
  UINT16                          AcpiBaseAddr;

  AcpiBaseAddr = PciRead16 (
                  PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH,
                  PCI_DEVICE_NUMBER_PCH_LPC,
                  0,
                  R_PCH_LPC_ACPI_BASE)
                  ) & B_PCH_LPC_ACPI_BASE_BAR;

  return IoRead32 ((UINTN) (AcpiBaseAddr + R_PCH_ACPI_PM1_TMR)) & B_PCH_ACPI_PM1_TMR_VAL;

}

/**
  Check if the required delay condition is satisified
  Note: The delay can't be larger than PCH ACPI timer High-to-Low
  transition frequency - 2.3435 seconds.

  @param[in]  InitialTicks        Initial PM1 tick value
  @param[in]  RequiredStallInUs   Required delay in us

  @retval     TRUE                The required delay is satisified
  @retval     FALSE               The required delay is not satisified
**/
BOOLEAN
UsbTimeout (
  IN UINTN                        InitialTicks,
  IN UINTN                        RequiredStallInUs
  )
{
  UINTN                           CurrentTick;
  UINTN                           ExpiredTick;

  //
  // The timer frequency is 3.579545 MHz, so 1 us corresponds 3.58 clocks
  //
  ExpiredTick = RequiredStallInUs * 358 / 100 + InitialTicks + 1;
  CurrentTick = PchGetPchTimerTick ();

  //
  // The High-to-Low transition will occur every 2.3435 seconds.
  //
  if (CurrentTick < InitialTicks) {
    CurrentTick += V_PCH_ACPI_PM1_TMR_MAX_VAL;
  }

  if (CurrentTick > ExpiredTick){
    return TRUE;
  }
  return FALSE;
}

/**
  Initialize usb global data and flag for reference

  @param[in]  None

  @retval     None
**/
VOID
UsbInitGlobalData (
  VOID
  )
{

  ///
  /// Set the flag to false and start to count time.
  ///
  PchUsbRPortsRstDoneFlag = FALSE;

  ///
  /// This is the latest root port reset, record it to ensure the Tdrstr is satisified.
  ///
  LastRPortResetTicks = PchGetPchTimerTick();
  return;
}

/**
  Check if the delay is enough since last root port reset

  @param[in]  None

  @retval     None
**/
VOID
UsbTdrstrDelayCheck (
  VOID
  )
{
  UINTN                           i;

  ///
  /// If the latest root port reset done, and then for all root ports reset by
  /// this protocol is ready. If we are in scenario#3, wait until delay time is enough. The flag
  /// is set either by timer event or the waitting loop.
  ///
  for (i = 0; (PchUsbRPortsRstDoneFlag != TRUE) && (i < USB_ROOT_PORT_RESET_STALL_US/ USB_TDRSTR_CHECK_INTERVAL_US); i++) {
    if (UsbTimeout (LastRPortResetTicks, USB_ROOT_PORT_RESET_STALL_US)) {
      PchUsbRPortsRstDoneFlag = TRUE;
      LastRPortResetTicks = 0;
      break;
    }
    PchPmTimerStall (USB_TDRSTR_CHECK_INTERVAL_US);
  }

  return ;
}

/**
  Check if the queried port is reset by USB precondition feature or not

  @param[in]  This                EFI_USB_HC_PORT_PRECONDITION instance
  @param[in]  PortNumber          The root port number (started by zero) to be queried

  @retval     TRUE                The root port is reset done
  @retval     FALSE               The root port is not reset
**/
BOOLEAN
EFIAPI
IsEhcRootPortReset (
  IN EFI_USB_HC_PORT_PRECONDITION *This,
  IN UINT8                        PortNumber
  )
{
  USB_EHCI_PRECONDITION_DEV       *EhcPreCondition;

  EhcPreCondition = EHC_PRECONDITION_FROM_THIS (This);

  ///
  /// For the EHCI on PCH, the root port 0 is always RMH and existing.
  /// PCH USB precondition feature resets the root port 0 on PCH EHCI only
  /// If the signature, PortNumber, or PortResetBitMap is invalid, return
  /// FALSE directly. Otherwise, return TRUE when required reset signal delay
  /// is satisified.
  ///
  if ((EhcPreCondition->Signature != EHCI_PRECONDITION_DEV_SIGN) ||
      (PortNumber != 0) ||
      (EhcPreCondition->PortResetBitMap == 0)) {
    return FALSE;
  }

  //
  // Drive the reset signal on root port for at least 50ms(Tdrstr). Check USB 2.0 Spec
  // section 7.1.7.5 for timing requirements.
  //
  if (!PchUsbRPortsRstDoneFlag) {
    UsbTdrstrDelayCheck ();
  }
  return TRUE;
}

/**
  Perform USB precondition on EHCI, it is the root port reset on
  installed USB device in DXE phase

  @param[in]  Device              The device number of the EHCI
  @param[in]  EhciMmioBase        Memory base address of EHCI Controller

  @retval     None
**/
VOID
EhciPrecondition (
  IN  UINT8                       Device,
  IN  UINT32                      EhciMmioBase
  )
{
  UINTN                           i;
  UINT32                          Data32;
  USB_EHCI_PRECONDITION_DEV       *EhcPreCondition;
  EFI_USB_HC_LOCATION             EhcLocation = {0, 0, 0, 0};

  //
  // Check if all ports routed to this EHCI successfully, if not, exit directly
  //
  if ((MmioRead32 (EhciMmioBase + R_PCH_EHCI_CONFIGFLAG) & BIT0) == 0) {
    return;
  }

  EhcPreCondition = AllocateZeroPool (sizeof (USB_EHCI_PRECONDITION_DEV));
  if (EhcPreCondition == NULL) {
    return;
  }

  ///
  /// This is Intel RMH behind EHCI, and it is on root hub port 0. Reset the root hub port0.
  ///
  for (i = 0; i < (USB_HC_RESET_STALL_US/ 10); i++) {
    if ((MmioRead32 (EhciMmioBase + R_PCH_EHCI_PORTSC0) & BIT0) == 0) {
      //
      // Root port 0 on EHCI is RMH, check the CCS bit before reset port.
      // If the CCS bit is not true, wait and poll until timeout
      //
      PchPmTimerStall (10);
    } else {
      break;
    }
  }

  Data32 = MmioRead32 (EhciMmioBase + R_PCH_EHCI_PORTSC0);

  //
  // Mask of the port change bits, they are WC (write clean).
  // Set one to PortReset bit and must also set zero to PortEnable bit
  //
  Data32 &= ~B_PCH_EHCI_PORTSC0_CHANGE_ENABLE_MASK;
  Data32 |= B_PCH_EHCI_PORTSC0_RESET;
  MmioWrite32 ((EhciMmioBase + R_PCH_EHCI_PORTSC0), Data32);

  UsbInitGlobalData ();
  EhcPreCondition->Signature = EHCI_PRECONDITION_DEV_SIGN;

  //
  // RMH is at root hub port 0
  //
  EhcPreCondition->PortResetBitMap = BIT0;

  //
  // Suggest required delay time defined by specification per RMH implementation. Reserved so far
  //
  ZeroMem (&(EhcPreCondition->Protocol.Timing), sizeof (EFI_USB_PORT_ENUM_TIMING_TABLE));

  EhcLocation.DeviceNumber = (UINTN) Device;
  CopyMem (&(EhcPreCondition->Protocol.Location), &EhcLocation, sizeof (EFI_USB_HC_LOCATION));

  EhcPreCondition->Protocol.IsRootPortReset = IsEhcRootPortReset;
  EhcPreCondition->Protocol.Next = mPrivatePreConditionList;
  mPrivatePreConditionList = &(EhcPreCondition->Protocol);
}

/**
  Check if the queried port is reset by USB precondition feature or not. This service must be called when
  XHC is in Run(R/S = '1') mode per XHCI specification requirement.

  @param[in]  This                EFI_USB_HC_PORT_PRECONDITION instance
  @param[in]  PortNumber          The root port number (started by zero) to be queried

  @retval     TRUE                The root port is reset done
  @retval     FALSE               The root port is not reset
**/
BOOLEAN
EFIAPI
IsXhcRootPortReset (
  IN EFI_USB_HC_PORT_PRECONDITION *This,
  IN UINT8                        PortNumber
  )
{
  UINT32                          UsbPort;
  UINT32                          Data32;
  UINT32                          XhciMmioBase;
  UINT32                          XhciPciMmBase;
  USB_XHCI_PRECONDITION_DEV       *XhcPreCondition;
  BOOLEAN                         ResumeFlag;

  XhcPreCondition = XHC_PRECONDITION_FROM_THIS (This);

  ///
  /// If the signature, PortNumber, or PortResetBitMap is invalid, return
  /// FALSE directly. Otherwise, return TRUE when required reset signal delay
  /// is satisified.
  ///
  if ((XhcPreCondition->PortResetBitMap == 0) || (XhcPreCondition->Signature != XHCI_PRECONDITION_DEV_SIGN)) {
    return FALSE;
  }

  ///
  /// Resume all USB2 protocol ports by first call
  ///
  if (XhcPreCondition->PORTSCxResumeDoneFlag != TRUE) {
    //
    // Drive the reset signal on root port for at least 50ms(Tdrstr). Check USB 2.0 Spec
    // section 7.1.7.5 for timing requirements.
    //
    if (!PchUsbRPortsRstDoneFlag) {
      UsbTdrstrDelayCheck ();
    }

    XhciPciMmBase = (UINT32) MmPciAddress (
                      0,
                      (UINT8) XhcPreCondition->Protocol.Location.BusNumber,
                      (UINT8) XhcPreCondition->Protocol.Location.DeviceNumber,
                      (UINT8) XhcPreCondition->Protocol.Location.FunctionNumber,
                      0
                      );

    XhciMmioBase = MmioRead32 (XhciPciMmBase + R_PCH_XHCI_MEM_BASE) & (~0xF);
    ResumeFlag = FALSE;

    ///
    /// For USB2 protocol port on XHCI, the reset done port will enter U3 state once the HC is halted
    /// To recovery the USB2 protocol port from U3 to U0, SW should:
    /// 1. SW shall ensure that the XHC is in Run mode prior to transitioning a root hub port from Resume to
    ///    the U0 state.
    /// 2. Write a "15" (Resume) to the PLS, XHC shall transmit the resume signaling within 1ms (Tursm)
    /// 3. SW shall ensure that resume is signaled for at least 20 ms (Tdrsmdn) from the write of Resume
    /// 4. After Tdrsmdn is complete, SW shall write a "0"(U0) to the PLS field
    ///
    for (UsbPort = 0; UsbPort < XhcPreCondition->HsPortCount; UsbPort++) {
      if (((UINT32)(1 << UsbPort) & XhcPreCondition->PortResetBitMap) != 0) {
        Data32 = MmioRead32 (XhciMmioBase + PORTSCxUSB2Ptr[UsbPort]);
        if ((Data32 & B_PCH_XHCI_PORTSCXUSB2_CCS) != 0) {
          Data32 &= ~B_PCH_XHCI_PORT_CHANGE_ENABLE_MASK;
          Data32 |= (B_PCH_XHCI_USB2_U3_EXIT + B_PCH_XHCI_PORTSCXUSB2_PP + B_PCH_XHCI_PORTSCXUSB2_LWS);
          MmioWrite32 (
            XhciMmioBase + PORTSCxUSB2Ptr[UsbPort],
            Data32
            );
          ResumeFlag = TRUE;
        } else {
          //
          // The CCS bit of this port disappears, it may be caused by the following reasons:
          // 1. Link training is successfully now, the CCS shows on correct USB speed port, i.e. USB3
          //    speed if it is USB3 device.
          // 2. The device is removed.
          // Ignore this port due to there is no device on it now.
          //
          XhcPreCondition->PortResetBitMap &= ~(UINT32) (1 << UsbPort);
        }
      }
    }

    if (ResumeFlag) {
      //
      // There is one root port resuming from U3 at least.
      //
      PchPmTimerStall (20 * 1000);
      for (UsbPort = 0; UsbPort < XhcPreCondition->HsPortCount; UsbPort++) {
        if (((UINT32)(1 << UsbPort) & XhcPreCondition->PortResetBitMap) != 0 ) {
          Data32 = (B_PCH_XHCI_PORTSCXUSB2_PP + B_PCH_XHCI_PORTSCXUSB2_LWS + B_PCH_XHCI_PORTSCXUSB2_CCS);
          MmioWrite32 (
            XhciMmioBase + PORTSCxUSB2Ptr[UsbPort],
            Data32
            );
        }
      }
    }
    XhcPreCondition->PORTSCxResumeDoneFlag = TRUE;
  }

  if (XhcPreCondition->PORTSCxResumeDoneFlag == TRUE) {
    //
    // If the signature, PortNumber, or PortResetBitMap is invalid, return
    // FALSE directly. Otherwise, return TRUE when required reset signal delay
    // is satisified.
    //
    if (((UINT32)(1 << PortNumber) & XhcPreCondition->PortResetBitMap) != 0) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
  Perform USB precondition on XHCI, it is the root port reset on
  installed USB device in DXE phase

  @param[in]  BusNumber           The Bus number of the XHCI
  @param[in]  Device              The device number of the XHCI
  @param[in]  Function            The function number of the XHCI
  @param[in]  XhciMmioBase        Memory base address of XHCI Controller
  @param[in]  XhciUSB2Ptr         Pointer to USB2 protocol port register
  @param[in]  HsPortCount         The number of USB2 protocol port supported by this XHCI

  @retval     None
**/
VOID
XhciPrecondition (
  IN  UINT8                       BusNumber,
  IN  UINT8                       Device,
  IN  UINT8                       Function,
  IN  UINT32                      XhciMmioBase,
  IN  UINTN                       *XhciUSB2Ptr,
  IN  UINTN                       HsPortCount,
  IN  UINTN                       *XhciUSB3Ptr,
  IN  UINTN                       SsPortCount
  )
{
  UINT32                          UsbPort;
  UINT32                          Data32;
  USB_XHCI_PRECONDITION_DEV       *XhcPreCondition;
  EFI_USB_HC_LOCATION             XhcLocation = {0, 0, 0, 0};

  XhcPreCondition = AllocateZeroPool (sizeof (USB_XHCI_PRECONDITION_DEV));
  if (XhcPreCondition == NULL) {
    return;
  }

  PORTSCxUSB2Ptr = XhciUSB2Ptr;
  PORTSCxUSB3Ptr = XhciUSB3Ptr;
  XhcPreCondition->Signature = XHCI_PRECONDITION_DEV_SIGN;

  XhcPreCondition->HsPortCount = HsPortCount;

  for (UsbPort = 0; UsbPort < HsPortCount; UsbPort++) {
    Data32 = MmioRead32 (XhciMmioBase + PORTSCxUSB2Ptr[UsbPort]);
    if ((Data32 & B_PCH_XHCI_PORTSCXUSB2_CCS) != 0) {
      Data32 &= ~B_PCH_XHCI_PORTSCXUSB2_PED;
      Data32 |= B_PCH_XHCI_PORTSCXUSB2_PR | B_PCH_XHCI_PORTSCXUSB2_PP;
      MmioWrite32 (
        XhciMmioBase + PORTSCxUSB2Ptr[UsbPort],
        Data32
        );
      //
      // PortSC registers in PCH XHCI is counted from HS ports
      //
      XhcPreCondition->PortResetBitMap |= (UINT32) (1 << UsbPort);
    }
  }
#ifdef EFI_DEBUG
  DEBUG ((EFI_D_INFO, "XhciPreconditionDxe - USB3PORTSC Start\n"));
  for (UsbPort = 0; UsbPort < SsPortCount; UsbPort++) {
    DEBUG ((EFI_D_INFO, "USB3Port %x - %x\n", UsbPort, MmioRead32 (XhciMmioBase + XhciUSB3Ptr[UsbPort])));
  }
#endif

  //
  // Clear WRC bit for all USB3 PORTSC
  //
  for (UsbPort = 0; UsbPort < SsPortCount; UsbPort++) {
    MmioAndThenOr32 (
      XhciMmioBase + PORTSCxUSB3Ptr[UsbPort],
      (UINT32)~ (B_PCH_XHCI_PORTSCXUSB3_CHANGE_ENABLE_MASK),
      B_PCH_XHCI_PORTSCXUSB3_WRC
      );
  }
#ifdef EFI_DEBUG
  DEBUG ((EFI_D_INFO, "XhciPreconditionDxe - USB3PORTSC Done\n"));
  for (UsbPort = 0; UsbPort < SsPortCount; UsbPort++) {
    DEBUG ((EFI_D_INFO, "USB3Port %x - %x\n", UsbPort, MmioRead32 (XhciMmioBase + XhciUSB3Ptr[UsbPort])));
  }
#endif
  UsbInitGlobalData ();
  XhcLocation.DeviceNumber = (UINTN) Device;
  XhcLocation.FunctionNumber = (UINTN) Function;
  CopyMem (&(XhcPreCondition->Protocol.Location), &XhcLocation, sizeof (EFI_USB_HC_LOCATION));
  XhcPreCondition->Protocol.IsRootPortReset = IsXhcRootPortReset;
  XhcPreCondition->PORTSCxResumeDoneFlag = FALSE;
  XhcPreCondition->Protocol.Next = mPrivatePreConditionList;
  mPrivatePreConditionList = &(XhcPreCondition->Protocol);
}

#endif  // USB_PRECONDITION_ENABLE_FLAG
