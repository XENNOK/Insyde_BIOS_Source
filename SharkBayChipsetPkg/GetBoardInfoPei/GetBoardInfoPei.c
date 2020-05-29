/** @file
  Get board id & other information and update it to PCD for future use.

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


#include <Uefi.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiOemSvcChipsetLib.h>
#include <Library/PcdLib.h>


EFI_STATUS
GetBoardInfoEntryPoint (
  IN       EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES           **PeiServices
  )
{

  EFI_STATUS    Status;
  UINT8         BoardId;
  UINT8         FabId;



//[-start-130613-IB05400415-modify]//
  //
  // Assign default value
  //
  BoardId = PcdGet8 (PcdDefaultBoardId);
  FabId = 0;

  //
  // Call to OEM
  //
//[-start-130524-IB05160451-modify]//
  Status = OemSvcGetBoardInfo (&BoardId, &FabId);
//[-end-130524-IB05160451-modify]//
  if (Status != EFI_SUCCESS) {
    PcdSet8 (PcdCurrentBoardId, BoardId);
    PcdSet8 (PcdCurrentFabId, FabId);
  }
//[-end-130613-IB05400415-modify]//


  return EFI_SUCCESS;
}

