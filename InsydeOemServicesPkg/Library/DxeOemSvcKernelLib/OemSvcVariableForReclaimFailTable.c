/** @file
  Return a table of preserved variables for recalim failed.

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

//
// This is a sample.
//
/*
#define SYSTEM_CONFIGURATION_GUID {0xA04A27f4, 0xDF00, 0x4D42, 0xB5, 0x52, 0x39, 0x51, 0x13, 0x02, 0x11, 0x3D}

PRESERVED_VARIABLE_TABLE mReclaimFailPreservedVariableTable[] = {
  { SYSTEM_CONFIGURATION_GUID, L"Setup" },
  { {0},   NULL }
};
*/

/**
  Return a table of preserved variables for recalim failed.

  @param[in, out]  TablePtr                  Pointer to the table of preserved variables.
  @param[in, out]  TableSize                 The size of this table.

  @retval          EFI_UNSUPPORTED           Returns unsupported by default.
  @retval          EFI_SUCCESS               The work to delete the variables is completed.
  @retval          EFI_MEDIA_CHANGED         The table of preserved variables is updated.
**/
EFI_STATUS
OemSvcVariableForReclaimFailTable (
  IN OUT PRESERVED_VARIABLE_TABLE              **TablePtr,
  IN OUT UINT32                                *TableSize
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  //
  // This is a sample.
  //
  /*
  UINTN                         Index;

  *TablePtr = mReclaimFailPreservedVariableTable;

  //
  // Calculate the table size.
  //
  *TableSize = sizeof (PRESERVED_VARIABLE_TABLE);
  for (Index = 0; mReclaimFailPreservedVariableTable[Index].VariableName != NULL; Index++) {
    *TableSize += sizeof (PRESERVED_VARIABLE_TABLE);
  }

  return EFI_MEDIA_CHANGED;
  */

  return EFI_UNSUPPORTED;
}

