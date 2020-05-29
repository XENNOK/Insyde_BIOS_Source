/** @file
  Initializes PCH USB Controllers.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**

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
#include "PchInitPeim.h"

/**
  The function performing USB init in PEI phase. This could be used by USB recovery
  or debug features that need USB initialization during PEI phase.
  Note: Before executing this function, please be sure that PCH_INIT_PPI.Initialize
  has been done and PchUsbPolicyPpi has been installed.

  @param[in] PeiServices    General purpose services available to every PEIM

  @retval EFI_SUCCESS       The function completed successfully
  @retval Others            All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
EFIAPI
PchUsbInit (
  IN CONST EFI_PEI_SERVICES             **PeiServices
  )
{
  EFI_STATUS          Status;
  PCH_USB_POLICY_PPI  *PchUsbPolicyPpi;

  DEBUG ((EFI_D_INFO, "PchUsbInit() - Start\n"));

  ///
  /// Get PchUsbPolicy PPI for PCH_USB_CONFIG
  ///
  Status = (**PeiServices).LocatePpi (
                            PeiServices,
                            &gPchUsbPolicyPpiGuid,
                            0,
                            NULL,
                            (VOID **)&PchUsbPolicyPpi
                            );

  if (Status == EFI_SUCCESS) {
#ifdef EFI_DEBUG
    DEBUG ((EFI_D_INFO, "\n------------------------ PchUsbPolicyPpi Dump Begin -----------------\n"));
    DEBUG ((EFI_D_INFO, "Revision : 0x%x\n", PchUsbPolicyPpi->Revision));
    DEBUG ((EFI_D_INFO, "Mode : 0x%x\n", PchUsbPolicyPpi->Mode));
    DEBUG ((EFI_D_INFO, "EhciMemBaseAddr : 0x%x\n", PchUsbPolicyPpi->EhciMemBaseAddr));
    DEBUG ((EFI_D_INFO, "EhciMemLength : 0x%x\n", PchUsbPolicyPpi->EhciMemLength));
    DEBUG ((EFI_D_INFO, "XhciMemBaseAddr : 0x%x\n", PchUsbPolicyPpi->XhciMemBaseAddr));
#endif
    Status = PchStartUsbInit (
              PchUsbPolicyPpi->UsbConfig,
              (UINT32) PchUsbPolicyPpi->EhciMemBaseAddr,
              (UINT32) PchUsbPolicyPpi->XhciMemBaseAddr,
              PchUsbPolicyPpi->Revision
              );
#ifdef EFI_DEBUG
    DEBUG ((EFI_D_INFO, "\n"));
    DEBUG ((EFI_D_INFO, "\n------------------------ PchUsbPolicyPpi Dump End -----------------\n"));
#endif
  }
  DEBUG ((EFI_D_INFO, "PchUsbInit() - End\n"));
  return Status;
}

/**
  The function performing USB init in PEI phase. This could be
  used by USB recovery ,debug features or usb precondition
  enabled case that need USB initialization during PEI phase.
  Please be sure the function should not be executed in if the
  boot mode is S3 resume.

  @param[in] UsbConfig            Pointer to a PCH_USB_CONFIG that provides the platform setting
  @param[in] EhciMemBaseAddr      Predefined Ehci memory base address for Ehci hc configuration
  @param[in] XhciMemBaseAddr      Predefined Xhci memory base address for Xhci hc configuration
  @param[in] Revision             Revision of PCH_USB_CONFIG

  @retval EFI_SUCCESS             The function completed successfully
  @retval Others                  All other error conditions encountered result in an ASSERT
**/
EFI_STATUS
PchStartUsbInit (
  IN PCH_USB_CONFIG               *UsbConfig,
  IN UINT32                       EhciMemBaseAddr,
  IN UINT32                       XhciMemBaseAddr,
  IN UINT8                        Revision
  )
{
  EFI_STATUS          Status;
  UINT32              RootComplexBar;
  UINT32              FuncDisableReg;
#ifdef EFI_DEBUG
  UINT8               i;
#endif

  DEBUG ((EFI_D_INFO, "PchStartUsbInit() - Start\n"));
  Status = EFI_INVALID_PARAMETER;
  if (UsbConfig != NULL) {
#ifdef EFI_DEBUG
    DEBUG ((EFI_D_INFO, "Revision : 0x%x\n", Revision));
    DEBUG ((EFI_D_INFO, "EhciMemBaseAddr : 0x%x\n", EhciMemBaseAddr));
    DEBUG ((EFI_D_INFO, "XhciMemBaseAddr : 0x%x\n", XhciMemBaseAddr));

    DEBUG ((EFI_D_INFO, "\n"));
    DEBUG ((EFI_D_INFO, "------------------------ PCH_USB_CONFIG Dump Start -----------------\n"));
    DEBUG ((EFI_D_INFO, " PCH_USB_CONFIG UsbPerPortCtl= %x\n", UsbConfig->UsbPerPortCtl));
    DEBUG ((EFI_D_INFO, " PCH_USB_CONFIG Ehci1Usbr= %x\n", UsbConfig->Ehci1Usbr));
    DEBUG ((EFI_D_INFO, " PCH_USB_CONFIG Ehci2Usbr= %x\n", UsbConfig->Ehci2Usbr));
    for (i = 0; i < GetPchUsbMaxPhysicalPortNum (); i++) {
      DEBUG ((EFI_D_INFO, " PCH_USB_CONFIG PortSettings[%d] Enabled= %x\n", i, UsbConfig->PortSettings[i].Enable));
      DEBUG ((EFI_D_INFO, " PCH_USB_CONFIG PortSettings[%d] Location = %x\n", i, UsbConfig->PortSettings[i].Location));
    }

    for (i = 0; i < GetPchXhciMaxUsb3PortNum (); i++) {
      DEBUG ((EFI_D_INFO, " PCH_USB_CONFIG Port30Settings[%d] Enabled= %x\n", i, UsbConfig->Port30Settings[i].Enable));
    }

    for (i = 0; i < GetPchEhciMaxControllerNum (); i++) {
      DEBUG ((EFI_D_INFO, " PCH_USB_CONFIG Usb20Settings[%d] Enabled= %x\n", i, UsbConfig->Usb20Settings[i].Enable));
    }

    DEBUG ((EFI_D_INFO, " PCH_USB_CONFIG Usb30Settings.Mode= %x\n", UsbConfig->Usb30Settings.Mode));
    DEBUG ((EFI_D_INFO, " PCH_USB_CONFIG Usb30Settings.PreBootSupport= %x\n", UsbConfig->Usb30Settings.PreBootSupport));
    DEBUG ((EFI_D_INFO, " XhciStreams is obsoleted, it doesn't effect any setting change since Revision 2.\n"));
    DEBUG ((EFI_D_INFO, " PCH_USB_CONFIG Usb30Settings.ManualMode= %x\n", UsbConfig->Usb30Settings.ManualMode));
    DEBUG ((EFI_D_INFO, " PCH_USB_CONFIG Usb30Settings.XhciIdleL1= %x\n", UsbConfig->Usb30Settings.XhciIdleL1));

    for (i = 0; i < GetPchUsbMaxPhysicalPortNum (); i++) {
      if (UsbConfig->Usb30Settings.ManualModeUsb20PerPinRoute[i] == 0) {
        DEBUG ((EFI_D_INFO, " PCH_USB_CONFIG Usb30Settings.ManualModeUsb20PerPinRoute[%d]= EHCI\n", i));
      } else {
        DEBUG ((EFI_D_INFO, " PCH_USB_CONFIG Usb30Settings.ManualModeUsb20PerPinRoute[%d]= XHCI\n", i));
      }
    }

    for (i = 0; i < GetPchXhciMaxUsb3PortNum (); i++) {
      DEBUG ((EFI_D_INFO,
              "PCH_USB_CONFIG Usb30Settings.ManualModeUsb30PerPinEnable[%d]= %x\n",
              i,
              UsbConfig->Usb30Settings.ManualModeUsb30PerPinEnable[i]));
    }

    for (i = 0; i < GetPchUsbMaxPhysicalPortNum (); i++) {
      DEBUG ((EFI_D_INFO, " PCH_USB_CONFIG Usb20OverCurrentPins[%d]= OC%x\n", i, UsbConfig->Usb20OverCurrentPins[i]));
    }

    for (i = 0; i < GetPchXhciMaxUsb3PortNum (); i++) {
      DEBUG ((EFI_D_INFO, " PCH_USB_CONFIG Usb30OverCurrentPins[%d]= OC%x\n", i, UsbConfig->Usb30OverCurrentPins[i]));
    }

    for (i = 0; i < GetPchEhciMaxUsbPortNum (); i++) {
      DEBUG ((EFI_D_INFO, " PCH_USB_CONFIG Usb20PortLength[%d]= %x.%0x\n", i, UsbConfig->PortSettings[i].Usb20PortLength >> 4, UsbConfig->PortSettings[i].Usb20PortLength & 0xF));
    }
    DEBUG ((EFI_D_INFO, "\n"));
    DEBUG ((EFI_D_INFO, "\n------------------------ PCH_USB_CONFIG Dump End -----------------\n"));
#endif
    RootComplexBar  = PCH_RCRB_BASE;
    FuncDisableReg  = MmioRead32 (RootComplexBar + R_PCH_RCRB_FUNC_DIS);

    Status = CommonUsbInit (
              UsbConfig,
              (UINT32) EhciMemBaseAddr,
              (UINT32) XhciMemBaseAddr,
              0,
              RootComplexBar,
              &FuncDisableReg,
              Revision
              );

    ASSERT_EFI_ERROR (Status);

    MmioWrite32 ((UINTN) (RootComplexBar + R_PCH_RCRB_FUNC_DIS), (UINT32) (FuncDisableReg));
    //
    // Reads back for posted write to take effect
    //
    MmioRead32 ((UINTN) (RootComplexBar + R_PCH_RCRB_FUNC_DIS));
  }

  DEBUG ((EFI_D_INFO, "PchStartUsbInit() - End\n"));

  return Status;

}
