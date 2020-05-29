/** @file
  To return the preserved list.

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

#include <Library/SmmOemSvcKernelLib.h>

//
// The followings are the sample codes. Please customize here.
//
//#define SampleGuid { 0x11111111, 0x1111, 0x1111, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11 }
//PRESERVED_VARIABLE_TABLE mPreservedVariableTable[] = {
//  { SampleGuid, L"SampleVariable" },
//  { {0},   NULL}
//};


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
OemSvcVariablePreservedTable (
  IN OUT PRESERVED_VARIABLE_TABLE               **VariablePreservedTable,
  IN OUT BOOLEAN                               *IsKeepVariableInList
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  //
  // The followings are the sample codes. Please customize here.
  //
  //*IsKeepVariableInList = TRUE;
  //*VariablePreservedTable = mPreservedVariableTable;
  //return EFI_MEDIA_CHANGED;

  return EFI_UNSUPPORTED;
}

