/** @file
  Try to issue the LegacyBios->GetBbsInfo() first and then modify the BbsTable

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

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>

#include <Protocol/LegacyBios.h>

VOID                            *mLegacyBiosSearchKey;
EFI_LEGACY_BIOS_GET_BBS_INFO    mLegacyBiosGetBbsInfo;


VOID
EFIAPI
HookForGetBbsInfo (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  );

EFI_STATUS
EFIAPI
BbsTableHook (
  IN EFI_LEGACY_BIOS_PROTOCOL         * This,
  OUT UINT16                          *HddCount,
  OUT HDD_INFO                        **HddInfo,
  OUT UINT16                          *BbsCount,
  IN OUT BBS_TABLE                    **BbsTable
  );

/**

  Install callback when EFI_LEGACY_BIOS_PROTOCOL installed.

  @param  ImageHandle           EFI_HANDLE
  @param  SystemTable           EFI_SYSTEM_TABLE pointer

  @retval EFI_SUCCESS           Driver Dispatch success

**/
EFI_STATUS
EFIAPI
BbsTableHookEntry (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
  )
{
  EFI_STATUS                Status;
  EFI_EVENT                 OemModifyBbsEvent;

  Status = gBS->CreateEvent (
                   EVT_NOTIFY_SIGNAL,
                   TPL_CALLBACK,
                   HookForGetBbsInfo,
                   NULL,  // Notification Context
                   &OemModifyBbsEvent
                   );
  if (EFI_ERROR(Status)) {
    return EFI_ABORTED;
  }    
  
  Status = gBS->RegisterProtocolNotify (
                  &gEfiLegacyBiosProtocolGuid,
                  OemModifyBbsEvent,
                  &mLegacyBiosSearchKey
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    return EFI_ABORTED;
  }
  return EFI_SUCCESS;
}

VOID
EFIAPI
HookForGetBbsInfo (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS                Status;
  EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios;

  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **)&LegacyBios);
  mLegacyBiosGetBbsInfo = LegacyBios->GetBbsInfo;
  LegacyBios->GetBbsInfo = BbsTableHook;

  ASSERT_EFI_ERROR (Status);
}



typedef
EFI_STATUS
(EFIAPI *EFI_LEGACY_BIOS_GET_BBS_INFO) (
  IN EFI_LEGACY_BIOS_PROTOCOL         * This,
  OUT UINT16                          *HddCount,
  OUT HDD_INFO                        **HddInfo,
  OUT UINT16                          *BbsCount,
  IN OUT BBS_TABLE                    **BbsTable
  );


EFI_STATUS
EFIAPI
BbsTableHook (
  IN EFI_LEGACY_BIOS_PROTOCOL         * This,
  OUT UINT16                          *HddCount,
  OUT HDD_INFO                        **HddInfo,
  OUT UINT16                          *BbsCount,
  IN OUT BBS_TABLE                    **BbsTable
  )
{
  EFI_STATUS                Status;
  UINT16                    LocalHddCount;
  UINT16                    LocalBbsCount;
  HDD_INFO                  *LocalHddInfo;
  BBS_TABLE                 *LocalBbsTable;

  UINTN                     HddIndex;
  UINTN                     Index;
  UINTN                     BbsIndex;

  Status = mLegacyBiosGetBbsInfo (
                This,
                &LocalHddCount,
                &LocalHddInfo,
                &LocalBbsCount,
                &LocalBbsTable
                );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  for (HddIndex = 0; HddIndex < MAX_IDE_CONTROLLER; HddIndex++) {
    BbsIndex = HddIndex * 2 + 1;
    for (Index = 0; Index < 2; ++Index) {
      if (LocalHddInfo[HddIndex].CommandBaseAddress == 0) {
        LocalBbsTable[BbsIndex + Index].BootPriority = BBS_IGNORE_ENTRY;
      } else {
        if (Index == 0) {
          if (LocalHddInfo[HddIndex].Status & HDD_MASTER_ATAPI_ZIPDISK) {
            LocalBbsTable[BbsIndex + Index].DeviceType = BBS_CDROM;
          }
        }
      }
    }
  }

//  *BbsTable = LocalBbsTable;
//  *BbsCount = LocalBbsCount;
//  *HddInfo  = LocalHddInfo;
//  *HddCount = LocalHddCount;
  if (BbsTable != NULL) *BbsTable = LocalBbsTable;
  if (BbsCount != NULL) *BbsCount = LocalBbsCount;
  if (HddInfo  != NULL) *HddInfo  = LocalHddInfo;
  if (HddCount != NULL) *HddCount = LocalHddCount;

  return EFI_SUCCESS;
}



