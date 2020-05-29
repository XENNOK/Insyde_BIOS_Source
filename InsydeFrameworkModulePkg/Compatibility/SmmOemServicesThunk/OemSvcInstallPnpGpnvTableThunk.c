/** @file
  GPNV is a persistent general-purpose storage area managed by SMBIOS. 
  The attribute of GPNV is OEM - specific that the format and location of GPNV are not defined in SMBIOS specification.
  This OemServiece provides OEM to define the handle parameter which is passed into GPNV function (55H, 56H, and 57H) to access specific GPNV area. 
  OEM can define accessible GPNV area by modifying the handle value of the UpdateableGpnvs Table.  

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "SmmOemServicesThunk.h"
#include <Library/SmmOemSvcKernelLib.h>

/**
  GPNV is a persistent general-purpose storage area managed by SMBIOS. 
  The attribute of GPNV is OEM - specific that the format and location of GPNV are not defined in SMBIOS specification.
  This OemServiece provides OEM to define the handle parameter which is passed into GPNV function (55H, 56H, and 57H) to access specific GPNV area. 
  OEM can define accessible GPNV area by modifying the handle value of the UpdateableGpnvs Table.  

  @param  *UpdateableGpnvCount  The number of updatable GPNV (General - Purpose -Nonvolatile) handles. 
  @param  **mUpdatableGpnvs     Pointer to UpdateableGpnvs Table.

  @retval EFI_SUCCESS           Get Updatable string success.
  @retval Others                Base on OEM design.
**/
EFI_STATUS
OemSvcInstallPnpGpnvTableThunk (
  IN SMM_OEM_SERVICES_PROTOCOL          *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  )
{
  VA_LIST                                Marker;
  UINTN                                 *UpdateableGpnvCount;
  OEM_GPNV_MAP                         **mUpdatableGpnv;
  EFI_STATUS                             Status;

  VA_START (Marker, NumOfArgs);
  UpdateableGpnvCount = VA_ARG (Marker, UINTN *);
  mUpdatableGpnv      = VA_ARG (Marker, OEM_GPNV_MAP **);
  VA_END (Marker);

  //
  // make a call to SmmOemSvcKernelLib internally
  //
  Status = OemSvcSmmInstallPnpGpnvTable (
             UpdateableGpnvCount,
             mUpdatableGpnv
             );

  return Status;
}
