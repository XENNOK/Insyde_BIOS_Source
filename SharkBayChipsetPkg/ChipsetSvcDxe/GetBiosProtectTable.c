/** @file
 DXE Chipset Services Library.
	
 This file contains only one function that is DxeCsSvcGetBiosProtectTable().
 The function DxeCsSvcGetBiosProtectTable() use chipset services to return 
 a Bios Protect Region Table.
	
***************************************************************************
* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <Library/PcdLib.h>
#include <Library/DxeOemSvcChipsetLib.h>
#include <Library/MemoryAllocationLib.h>
//[-start-130927-IB12360020-add]//
#include <Library/DebugLib.h>
//[-end-130927-IB12360020-add]//

//[-start-130927-IB12360020-modify]//
/**
 Provide bios protect table for Kernel.

 @param[out]            *BiosRegionTable    Pointer to BiosRegion Table.
 @param[out]            ProtectRegionNum    The number of Bios protect region instances.

 @retval                EFI_SUCCESS         Provide table for kernel to set protect region and lock flash program registers.
                                            This table will be freed by kernel.
*/
//[-end-130927-IB12360020-modify]//
EFI_STATUS
GetBiosProtectTable (
//[-start-130927-IB12360020-modify]//
  OUT BIOS_PROTECT_REGION           **BiosRegionTable,
  OUT UINT8                         *ProtectRegionNum
//[-end-130927-IB12360020-modify]//
  )
{
  UINT8         BiosProtectRegionNumber;       
//[-start-130927-IB12360020-modify]//
  BiosProtectRegionNumber = 2;

  if (BiosRegionTable == NULL || ProtectRegionNum == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if (*BiosRegionTable != NULL) {
    //
    // We will allocate a new one, so free it.
    //
    FreePool (*BiosRegionTable);
    *BiosRegionTable = NULL;
  } 
  
  *BiosRegionTable = AllocateZeroPool (sizeof (BIOS_PROTECT_REGION) * (BiosProtectRegionNumber));

  if (*BiosRegionTable == NULL) {
    DEBUG ((EFI_D_INFO, "BiosRegionTable allocate memory resource failed.\n"));
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130927-IB12360020-modify]//
  (*BiosRegionTable)[0].Base = PcdGet32 (PcdFlashFvMainBase);
  (*BiosRegionTable)[0].Size = PcdGet32 (PcdFlashFvMainSize);
  (*BiosRegionTable)[1].Base = PcdGet32 (PcdFlashFvRecoveryBase);
  (*BiosRegionTable)[1].Size = PcdGet32 (PcdFlashFvRecoverySize);
  
  *ProtectRegionNum = BiosProtectRegionNumber;
  OemSvcUpdateBiosProtectTable (BiosRegionTable, ProtectRegionNum);
  
  return EFI_SUCCESS;
}

