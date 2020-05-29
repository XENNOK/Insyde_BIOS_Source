/** @file
  Provides an opportunity for OEM to decide what SSID/SVID to use.

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/DxeOemSvcKernelLib.h>

#include <Library/PciExpressLib.h>
#include <Library/S3BootScriptLib.h>

EFI_STATUS
ProgramSsidSvid0xFFFF1002 (
  IN     UINT8    Bus,
  IN     UINT8    Dev,
  IN     UINT8    Func,
  IN OUT UINT32  *SsidSvid
  )
{
  UINT64 BootScriptPciAddress;
  //
  // Program SSID / SSVID
  //
  PciExpressWrite32 (PCI_EXPRESS_LIB_ADDRESS(Bus, Dev, Func, 0x4C), *SsidSvid);

  BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Dev, Func, 0x4C);
  S3BootScriptSavePciCfgWrite (
      S3BootScriptWidthUint32, 
      BootScriptPciAddress,
      1, 
      SsidSvid);
  
  return EFI_SUCCESS;
}

EFI_STATUS
ProgramSsidSvid0xFFFF10DE (
  IN     UINT8    Bus,
  IN     UINT8    Dev,
  IN     UINT8    Func,
  IN OUT UINT32  *SsidSvid
  )
{
  UINT64     BootScriptPciAddress;
  //
  // Program SSID / SSVID
  //
  PciExpressWrite32 (PCI_EXPRESS_LIB_ADDRESS(Bus, Dev, Func, 0x40), *SsidSvid);

  BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Dev, Func, 0x40);
  S3BootScriptSavePciCfgWrite (
      S3BootScriptWidthUint32, 
      BootScriptPciAddress,
      1, 
      SsidSvid);

  return EFI_SUCCESS;
}

STATIC OEM_SSID_SVID_TABLE SsidTable[] = {
  0x10DE, DEVICE_ID_DONT_CARE, ProgramSsidSvid0xFFFF10DE,
  0x1002, DEVICE_ID_DONT_CARE, ProgramSsidSvid0xFFFF1002,
  DEVICE_ID_DONT_CARE, DEVICE_ID_DONT_CARE, NULL
};

/**
  The OemSvc is used to update SSID/SVID value by OEM.

  @param[in]      Bus                   Bus number.
  @param[in]      Dev                   Device number.
  @param[in]      Func             	    Function number. 
  @param[in]      VendorID              Vendor ID.
  @param[in]      DeviceID              Device ID.
  @param[in]      ClassCode             Class Code.
  @param[in out]  SsidSvid              Pointer to SSID/SVID.

  @retval     EFI_UNSUPPORTED       Returns unsupported by default.
  @retval     EFI_SUCCESS           OEM handled SSID/SVID programming on this PCI device. Skip default kernel
                                    programming mechanism. 
  @retval     EFI_MEDIA_CHANGED     Updated SsidSvid value and returned this value for default programming mechanism.
**/
EFI_STATUS
OemSvcUpdateSsidSvidInfo (
  IN     UINT8     Bus,
  IN     UINT8     Dev,
  IN     UINT8     Func,
  IN     UINT16    VendorId,
  IN     UINT16    DeviceId,
  IN     UINT16    ClassCode,
  IN OUT UINT32   *SsidSvid
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/
  UINT8       Index;
  EFI_STATUS  Status;

  Status = EFI_UNSUPPORTED;
  for (Index = 0; SsidTable[Index].SpecialSsidSvidFunction != NULL; Index++ ) {
    if (SsidTable[Index].VendorId == VendorId) {
      if ((SsidTable[Index].DeviceId == DEVICE_ID_DONT_CARE)
        || (SsidTable[Index].DeviceId == DeviceId)){
        Status = SsidTable[Index].SpecialSsidSvidFunction (Bus, Dev, Func, SsidSvid);
        break;
      }
    }
  }

  return Status;
}
