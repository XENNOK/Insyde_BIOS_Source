/** @file
  Provide OEM to define the SMBIOS policy.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "DxeOemServicesThunk.h"
#include <Library/DxeOemSvcKernelLib.h>

/**
  Provide OEM to define the SMBIOS policy. The MiscSubClass table in SMBIOS is constructed by the instance of SMBIOS Type.
  This policy decides which instance of Type (RecordNumber) will be added into the table.

  @param  *DataHubRecordPolicy  Point to EFI_DATAHUB_RECORD_POLICY

  @retval EFI_SUCCESS           Get SMBIOS policy success.
  @retval Others                The error status bases on OEM design.
**/
EFI_STATUS
OemSvcInstallDmiSwitchTableThunk (
  IN OEM_SERVICES_PROTOCOL              *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  )
{
  VA_LIST                               Marker;
  EFI_DATAHUB_RECORD_POLICY             **mDataHubRecordPolicy;
  UINTN                                 *MiscSubclassDefaultSize;
  EFI_STATUS                            Status;

  VA_START (Marker, NumOfArgs);
  MiscSubclassDefaultSize = VA_ARG (Marker, UINTN *);
  mDataHubRecordPolicy    = VA_ARG (Marker, EFI_DATAHUB_RECORD_POLICY **);
  VA_END (Marker);

  //
  // make a call to DxeOemSvcKernelLib internally
  //
  Status = OemSvcInstallDmiSwitchTable (
             MiscSubclassDefaultSize,
             mDataHubRecordPolicy
             );

  return Status;
}
