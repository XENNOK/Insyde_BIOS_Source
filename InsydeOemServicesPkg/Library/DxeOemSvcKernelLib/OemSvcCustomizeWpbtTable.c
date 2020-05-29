/** @file
  Allow OEM to customize the content of Wpbt ACPI table.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
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
  OEM may customize the file GUID of the executable and the input argument to the executable
  which are specified in Wpbt ACPI table. 

  @param[in, out]  *FileGuid                 The file GUID of the executable.
  @param[in, out]  *InputArg                 The input argument to the executable.

  @retval      EFI_UNSUPPORTED               Returns unsupported by default.
  @retval      EFI_SUCCESS                   The file GUID and the input argument are changed, 
                                             the specified executable is loaded to memory,
                                             and the Wpbt table is installed successfully.
  @retval      EFI_MEDIA_CHANGED             The file GUID and the input argument are changed. 
**/
EFI_STATUS
OemSvcCustomizeWpbtTable (
  IN OUT EFI_GUID                            *FileGuid,
  IN OUT CHAR16                              *InputArg
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}
