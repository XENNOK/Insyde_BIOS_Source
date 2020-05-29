/** @file
  Provide OEM to update the MSDM Data.

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
  This OemService provides OEM to update the MSDM Data. 
  The update info will return to ACPI MSDM table. The service is following the OA specification 3.0. 

  @param  *MsdmData             The MSDM Data will be copied to this address if OEM updates MSDM Data here.
  @param  *UpdatedMsdmData      Return TRUE if OEM updates the MSDM Data.

  @retval EFI_SUCCESS           Get OA3.0 information success.
  @retval Others                Base on OEM design.
**/
EFI_STATUS
OemSvcGetOa30MsdmDataThunk (
  IN  OEM_SERVICES_PROTOCOL                *OemServices,
  IN  UINTN                                NumOfArgs,
  ...
  )
{
  VA_LIST                               Marker;
  EFI_ACPI_MSDM_DATA_STRUCTURE          *MsdmData         = NULL;
  BOOLEAN                               *UpdatedMsdmData  = NULL;
  EFI_STATUS                            Status;


  VA_START (Marker, NumOfArgs);

  MsdmData         = VA_ARG (Marker, EFI_ACPI_MSDM_DATA_STRUCTURE*);
  UpdatedMsdmData  = VA_ARG (Marker, BOOLEAN*);
  VA_END (Marker);

  //
  // make a call to DxeOemSvcKernelLib internally
  //
  Status = OemSvcGetOa30MsdmData (
             MsdmData
             );

  return EFI_UNSUPPORTED;
}
