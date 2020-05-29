/** @file
  Provides an opportunity for OEM to update the MSDM Data.

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
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
  This OemService provides an opportunity for OEM to update the MSDM Data.
  The returned data is used to build the ACPI MSDM table.
  The service follows the OA specification 3.0.

  @param[in, out]  MsdmData              The MSDM Data will be copied to this address if OEM updates MSDM Data here.

  @retval          EFI_UNSUPPORTED       Returns unsupported by default.
  @retval          EFI_SUCCESS           Get OA3.0 information failed.
  @retval          EFI_MEDIA_CHANGED     Get OA3.0 information success.
  @retval          Others                Base on OEM design.
**/
EFI_STATUS
OemSvcGetOa30MsdmData (
  IN OUT EFI_ACPI_MSDM_DATA_STRUCTURE          *MsdmData
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}
