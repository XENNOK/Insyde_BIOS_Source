/** @file
 DXE Chipset Services Library.
 	
 This file contains only one function that is DxeCsSvcProgramChipsetSsid().
 The function DxeCsSvcProgramChipsetSsid() use chipset services to program subsystem vendor identification.
	
***************************************************************************
* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <Library/S3BootScriptLib.h>
#include <Library/IoLib.h>
#include <Protocol/PciRootBridgeIo.h>
#include <IndustryStandard/Pci30.h>
#include <PchAccess.h>
#include <SaAccess.h>

#define DEVICE_ID_DONT_CARE           0xFFFF

typedef
VOID
(*SPECIAL_PROGRAM_MECHANISM) (
  IN     UINT8    Bus,
  IN     UINT8    Dev,
  IN     UINT8    Func,
  IN     UINT32   SsidSvid
  );

typedef struct {
  UINT16                       VendorId;
  UINT16                       DeviceId;
  SPECIAL_PROGRAM_MECHANISM    SpecialSsidSvidFunction;
} SPECIFIC_SSID_SVID_TABLE;

/**
 EHCI SSID/SVID Special Program Function

 @param[in]         Bus                 PCI Bus number
 @param[in]         Dev                 PCI Device number
 @param[in]         Func                PCI Function number
 @param[in]         SsidVid             SsidVid

 @retval            EFI_SUCCESS         Function returns successfully
*/
EFI_STATUS
EhciSsidSvidSpecialProgramFunction (
  IN UINT8    Bus,
  IN UINT8    Dev,
  IN UINT8    Func,
  IN UINT32   SsidSvid
  )
{

  UINT8     EhciAccessCntl;
  UINT8     WrtRdonly;

  EhciAccessCntl = 0;
  WrtRdonly      = 0;

  //
  // Set EHCI devices WRT_RDONLY bit ( D29:F0 0x80[0], D26:F0 0x80[0] ) to 1, to make SVID and SSID registers are writable
  //
  EhciAccessCntl = MmPci8 (0, Bus, Dev, Func, R_PCH_EHCI_ACCESS_CNTL);
  WrtRdonly = EhciAccessCntl & B_PCH_EHCI_ACCESS_CNTL_ENABLE;
  EhciAccessCntl = EhciAccessCntl | V_PCH_EHCI_ACCESS_CNTL_ENABLE;
  MmPci8 (0, Bus, Dev, Func, R_PCH_EHCI_ACCESS_CNTL ) = EhciAccessCntl;
  S3BootScriptSavePciCfgWrite (
    S3BootScriptWidthUint8,
    (UINTN) (EFI_PCI_ADDRESS (Bus, Dev, Func, R_PCH_EHCI_ACCESS_CNTL)),
    1,
    &EhciAccessCntl
    );

  //
  // Program SSID & SVID as DID & VID.
  //
  MmPci32 (0, Bus, Dev, Func, PCI_SUBSYSTEM_VENDOR_ID_OFFSET ) = SsidSvid;
  S3BootScriptSavePciCfgWrite (
    S3BootScriptWidthUint32, 
    (UINTN) (EFI_PCI_ADDRESS (Bus, Dev, Func, PCI_SUBSYSTEM_VENDOR_ID_OFFSET)),
    1,
    &SsidSvid
    );

  //
  // Restore the EHCI devices WRT_RDONLY bit ( D29:F0 0x80[0], D26:F0 0x80[0] ) value
  //
  EhciAccessCntl = MmPci8 (0, Bus, Dev, Func, R_PCH_EHCI_ACCESS_CNTL );
  EhciAccessCntl = EhciAccessCntl & ( ~B_PCH_EHCI_ACCESS_CNTL_ENABLE );
  EhciAccessCntl = EhciAccessCntl | WrtRdonly;
  MmPci8 (0, Bus, Dev, Func, R_PCH_EHCI_ACCESS_CNTL ) = EhciAccessCntl;
  S3BootScriptSavePciCfgWrite (
    S3BootScriptWidthUint8,
    (UINTN) (EFI_PCI_ADDRESS (Bus, Dev, Func, R_PCH_EHCI_ACCESS_CNTL)),
    1,
    &EhciAccessCntl
    );

  return EFI_SUCCESS;
}

/**
 XHCI SSID/SVID Special Program Function

 @param[in]         Bus                 PCI Bus number
 @param[in]         Dev                 PCI Device number
 @param[in]         Func                PCI Function number
 @param[in]         SsidVid             SsidVid

 @retval            EFI_SUCCESS         Function returns successfully
*/
EFI_STATUS
XhciSsidSvidSpecialProgramFunction (
  IN UINT8    Bus,
  IN UINT8    Dev,
  IN UINT8    Func,
  IN UINT32   SsidSvid
  )
{
  //
  // Program SSID & SVID as DID & VID.
  //
  MmPci32 (0, Bus, Dev, Func, PCI_SUBSYSTEM_VENDOR_ID_OFFSET) = SsidSvid;
  S3BootScriptSavePciCfgWrite (
    S3BootScriptWidthUint32,
    (UINTN) (EFI_PCI_ADDRESS (Bus, Dev, Func, PCI_SUBSYSTEM_VENDOR_ID_OFFSET)),
    1,
    &SsidSvid
    );

  return EFI_SUCCESS;
}

/**
 IGD SSID/SVID Special Program Function

 @param[in]         Bus                 PCI Bus number
 @param[in]         Dev                 PCI Device number
 @param[in]         Func                PCI Function number
 @param[in]         SsidVid             SsidVid

 @retval            EFI_SUCCESS         Function returns successfully
*/
EFI_STATUS
IgdSsidSvidSpecialProgramFunction (
  IN UINT8    Bus,
  IN UINT8    Dev,
  IN UINT8    Func,
  IN UINT32   SsidSvid
  )
{

  //
  // Program SSID / SSVID
  //
  MmPci32 (0, Bus, Dev, Func, PCI_SUBSYSTEM_VENDOR_ID_OFFSET ) = SsidSvid;
  S3BootScriptSavePciCfgWrite (
    S3BootScriptWidthUint32,
    (UINTN) (EFI_PCI_ADDRESS (Bus, Dev, Func, PCI_SUBSYSTEM_VENDOR_ID_OFFSET)),
    1,
    &SsidSvid
    );

  return EFI_SUCCESS;
}
SPECIFIC_SSID_SVID_TABLE mSpecificSsidSvidTable[] ={
  {V_SA_IGD_VID         , V_SA_IGD_DID                    , IgdSsidSvidSpecialProgramFunction},
  {V_PCH_INTEL_VENDOR_ID, V_PCH_LPTH_USB_DEVICE_ID_EHCI_1 , EhciSsidSvidSpecialProgramFunction},
  {V_PCH_INTEL_VENDOR_ID, V_PCH_LPTH_USB_DEVICE_ID_EHCI_2 , EhciSsidSvidSpecialProgramFunction},
  {V_PCH_INTEL_VENDOR_ID, V_PCH_LPTLP_USB_DEVICE_ID_EHCI_1, EhciSsidSvidSpecialProgramFunction},
  {V_PCH_INTEL_VENDOR_ID, V_PCH_LPTH_USB_DEVICE_ID_XHCI_1 , XhciSsidSvidSpecialProgramFunction},
  {V_PCH_INTEL_VENDOR_ID, V_PCH_LPTLP_USB_DEVICE_ID_XHCI_1, XhciSsidSvidSpecialProgramFunction},
  {DEVICE_ID_DONT_CARE  , DEVICE_ID_DONT_CARE   , NULL}
};

/**
 Program Chipset SSID

 @param[in]         Bus                 PCI Bus number
 @param[in]         Dev                 PCI Device number
 @param[in]         Func                PCI Function number
 @param[in]         VendorId            Vendor ID
 @param[in]         DeviceId            Device ID
 @param[in]         ClassCode           PCI Class Code
 @param[in]         SsidVid             SsidVid

 @retval            EFI_SUCCESS         Function returns successfully
 @retval            EFI_UNSUPPORTED     1. The specific ID is not find.
                                        2. The specific device can not be set SSID.
*/
EFI_STATUS
ProgramChipsetSsid (
  IN UINT8                               Bus,
  IN UINT8                               Dev,
  IN UINT8                               Func,
  IN UINT16                              VendorId,
  IN UINT16                              DeviceId,
  IN UINT32                              SsidSvid
  )
{
  UINT8                               Index;

  for (Index = 0; mSpecificSsidSvidTable[Index].SpecialSsidSvidFunction != NULL; Index++ ) {
    if (mSpecificSsidSvidTable[Index].VendorId == VendorId) {
      if ((mSpecificSsidSvidTable[Index].DeviceId == DEVICE_ID_DONT_CARE)
        || (mSpecificSsidSvidTable[Index].DeviceId == DeviceId)) {
        mSpecificSsidSvidTable[Index].SpecialSsidSvidFunction (Bus, Dev, Func, SsidSvid);
        return EFI_SUCCESS;
      }
    }
  }
  return EFI_UNSUPPORTED;
}  
