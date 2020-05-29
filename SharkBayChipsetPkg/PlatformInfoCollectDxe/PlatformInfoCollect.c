/** @file
  This driver will initial and update PrePostHotkey Variable.

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Protocol/PlatformInfo.h>
#include <Guid/PlatformInfoHob.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>

EFI_STATUS
EFIAPI
PlatformInfoCollect (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                                Status;
  VOID                                      *HobList;
  PLATFORM_INFO_HOB                         *GetHob;
  EFI_HANDLE                                HobHandle;
  PLATFORM_INFO_PROTOCOL                    *PlatformInfoProtocol = NULL;

  HobList = GetHobList ();
  GetHob = GetNextGuidHob (&gBoardIDGuid, HobList);
  if (GetHob == NULL) {
    return EFI_NOT_FOUND;
  }

  HobHandle = NULL;
  PlatformInfoProtocol = AllocatePool (sizeof (PLATFORM_INFO_PROTOCOL));
//[-start-130207-IB10870073-add]//
  ASSERT (PlatformInfoProtocol != NULL);
  if (PlatformInfoProtocol == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130207-IB10870073-add]//
//[-start-130111-IB05280033-modify]//
  PlatformInfoProtocol->PlatInfo.BoardId = *((UINT8 *)(GetHob) + sizeof (EFI_HOB_GUID_TYPE));
  PlatformInfoProtocol->PlatInfo.FabId = *((UINT8 *)(GetHob) + sizeof (EFI_HOB_GUID_TYPE) + 1);
  PlatformInfoProtocol->PlatInfo.UltPlatform = *((BOOLEAN *)(GetHob) + sizeof (EFI_HOB_GUID_TYPE) + 2);
//[-end-130111-IB05280033-modify]//

  //
  // Install on a new handle
  //
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "Install Platform Info Protocol\n"));

  Status = gBS->InstallProtocolInterface (
                  &HobHandle,
                  &gEfiPlatformInfoProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  PlatformInfoProtocol
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
