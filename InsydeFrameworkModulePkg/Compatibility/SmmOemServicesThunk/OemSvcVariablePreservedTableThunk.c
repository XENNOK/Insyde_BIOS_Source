/** @file
  To return the preserved list.

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

#include "SmmOemServicesThunk.h"
#include <Library/SmmOemSvcKernelLib.h>

/**
  To return the preserved list.

  @param[in, out]  VariablePreservedTable    Pointer to the table of preserved variables.
  @param[in, out]  IsKeepVariableInList      True: The variables in the table should be preserved.
                                             FALSE: The variables in the table should be deleted.

  @retval          EFI_UNSUPPORTED           Returns unsupported by default.
  @retval          EFI_SUCCESS               The work to delete the variables is completed.
  @retval          EFI_MEDIA_CHANGED         The table of preserved variables is updated.
**/
EFI_STATUS
OemSvcVariablePreservedTableThunk (
  IN  SMM_OEM_SERVICES_PROTOCOL         *This,
  IN  UINTN                             NumOfArgs,
  ...
  )
{
  VA_LIST                               Marker;
  EFI_STATUS                            Status;
  PRESERVED_VARIABLE_TABLE              **PreservedVariableTable;
  BOOLEAN                               *IsKeepVariableInList;

  VA_START (Marker, NumOfArgs);

  PreservedVariableTable = VA_ARG (Marker, PRESERVED_VARIABLE_TABLE **);
  IsKeepVariableInList   = VA_ARG (Marker, BOOLEAN *);

  VA_END (Marker);

  //
  // make a call to SmmOemSvcKernelLib internally
  //
  Status = OemSvcVariablePreservedTable (
             PreservedVariableTable,
             IsKeepVariableInList
             );

  return Status;
}
