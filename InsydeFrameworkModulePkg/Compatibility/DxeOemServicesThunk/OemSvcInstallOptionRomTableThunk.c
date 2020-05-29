/** @file
  Provide OEM to install the PCI Option ROM table and Non-PCI Option ROM table.

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
  Provide OEM to install the PCI Option ROM table and Non-PCI Option ROM table. 
  The detail refers to the document "OptionRomTable Restructure User Guide".

  @param  *RomType              The type of option rom. This parameter decides which kind of option ROM table will be access.
  @param  **RomTable            A pointer to the option ROM table.
  
  @retval EFI_SUCCESS           Get Option ROM Table info success.
  @retval EFI_NOT_FOUND         Get Option ROM Table info failed.
**/
EFI_STATUS 
OemSvcInstallOptionRomTableThunk (
  IN OEM_SERVICES_PROTOCOL              *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  )
{
  VA_LIST                               Marker;
  UINT8                                 RomType;
  VOID                                  **mOptionRomTable;
  EFI_STATUS                            Status;

  VA_START (Marker, NumOfArgs);
  RomType                  = VA_ARG (Marker, UINT8);

  //
  // The second argument **RomTable bases on the RomType. Sample code as following:
  // switch (Type) {
  //
  // case PCI_OPROM:
  //   mOptionRomTable     = VA_ARG (Marker, PCI_OPTION_ROM_TABLE **);      
  //
  // case SYSTEM_ROM:
  //   mOptionRomTable     = VA_ARG (Marker, SYSTEM_ROM_TABLE **);
  //
  mOptionRomTable     = VA_ARG (Marker, VOID **);      
  VA_END (Marker);
  
  //
  // make a call to DxeOemSvcKernelLib internally
  //
  Status = OemSvcInstallOptionRomTable (
             RomType,
             mOptionRomTable
             );

  return Status;
}

