/** @file
  Update Bbs table.

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
  Update Bbs table.

  @param  *EfiToLegacy16BootTable   Pointer to Legacy16BootTable
  @param  *BbsTable                 Pointer to BBS table

  @retval Base on OEM design.
**/
EFI_STATUS
OemSvcUpdateBbsTableThunk (
  IN OEM_SERVICES_PROTOCOL                 *OemServices,
  IN  UINTN                                NumOfArgs,
  ...
  )
{
  VA_LIST                           Marker;
  EFI_STATUS                        Status = EFI_UNSUPPORTED;
  EFI_TO_COMPATIBILITY16_BOOT_TABLE *EfiToLegacy16BootTable;
  BBS_TABLE                         *BbsTable;

  VA_START (Marker, NumOfArgs);
  
  EfiToLegacy16BootTable = VA_ARG (Marker, EFI_TO_COMPATIBILITY16_BOOT_TABLE *);
  BbsTable               = VA_ARG (Marker, BBS_TABLE *);
  VA_END (Marker);

  //
  // make a call to DxeOemSvcKernelLib internally
  //
  Status = OemSvcUpdateBbsTable (
             EfiToLegacy16BootTable,
             BbsTable
             );

  return Status;
}
