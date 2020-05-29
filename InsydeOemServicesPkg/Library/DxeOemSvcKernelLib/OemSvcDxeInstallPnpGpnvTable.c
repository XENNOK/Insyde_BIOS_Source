/** @file
  GPNV is a persistent general-purpose storage area managed by SMBIOS. 
  The attribute of GPNV is OEM - specific that the format and location of GPNV are not defined in SMBIOS specification.
  This OemServiece provides OEM to define the handle parameter which is passed into GPNV function (55H, 56H, and 57H) to access specific GPNV area. 
  OEM can define accessible GPNV area by modifying the handle value of the UpdateableGpnvs Table.  

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/DxeOemSvcKernelLib.h>

/**
  GPNV is a persistent general-purpose storage area managed by SMBIOS. 
  The attribute of GPNV is OEM - specific that the format and location of GPNV are not defined in SMBIOS specification.
  This OemServiece provides OEM to define the handle parameter which is passed into GPNV function (55H, 56H, and 57H) to access specific GPNV area. 
  OEM can define accessible GPNV area by modifying the handle value of the UpdateableGpnvs Table.  

  @param[out]  *UpdateableGpnvCount  The number of updatable GPNV (General - Purpose -Nonvolatile) handles. 
  @param[out]  **mUpdatableGpnvs     Pointer to UpdateableGpnvs Table.

  @retval      EFI_UNSUPPORTED       Returns unsupported by default.
  @retval      EFI_SUCCESS           Get Updatable string success.
  @retval      EFI_MEDIA_CHANGED     The value of IN OUT parameter is changed. 
  @retval      Others                Base on OEM design.
**/
EFI_STATUS
OemSvcDxeInstallPnpGpnvTable (
  OUT UINTN                          *UpdateableGpnvCount,
  OUT OEM_GPNV_MAP                  **GetOemGPNVMap
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}
