//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
--*/
/*++

Copyright (c)  1999 - 2006 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  FlashMap.c

Abstract:

  EFI 2.0 PEIM to build GUIDed HOBs for platform specific flash map

--*/

#include "Tiano.h"
#include "Pei.h"
#include "PeiLib.h"

#include "FlashLayout.h"

#include EFI_GUID_DEFINITION (FlashMapHob)
#include EFI_PPI_DEFINITION (FlashMap)

#include EFI_GUID_DEFINITION (SystemNvDataGuid)
#include EFI_PROTOCOL_DEFINITION (FirmwareVolumeBlock)
#include EFI_GUID_DEFINITION (FirmwareFileSystem)

EFI_GUID                            mFvBlockGuid    = EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL_GUID;
EFI_GUID                            mFfsGuid        = EFI_FIRMWARE_FILE_SYSTEM_GUID;
EFI_GUID                            mSystemDataGuid = EFI_SYSTEM_NV_DATA_HOB_GUID;

EFI_STATUS
GetAreaInfo (
  IN  EFI_PEI_SERVICES            **PeiServices,
  IN PEI_FLASH_MAP_PPI            *This,
  IN  EFI_FLASH_AREA_TYPE         AreaType,
  IN  EFI_GUID                    *AreaTypeGuid,
  OUT UINT32                      *NumEntries,
  OUT EFI_FLASH_SUBAREA_ENTRY     **Entries
  );

//
// Module globals
//
extern EFI_FLASH_AREA_DATA          mFlashAreaData[];
extern EFI_HOB_FLASH_MAP_ENTRY_TYPE mFlashMapHobData[];

extern UINTN                        mNumOfFlashAreaData;
extern UINTN                        mNumOfHobFlashMapData;

static PEI_FLASH_MAP_PPI            mFlashMapPpi = { GetAreaInfo };

static EFI_PEI_PPI_DESCRIPTOR       mPpiListFlashMap = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiFlashMapPpiGuid,
  &mFlashMapPpi
};

EFI_STATUS
EFIAPI
PeimInitializeFlashMap (
  IN EFI_FFS_FILE_HEADER       *FfsHeader,
  IN EFI_PEI_SERVICES          **PeiServices
  );

EFI_PEIM_ENTRY_POINT (PeimInitializeFlashMap)

EFI_STATUS
EFIAPI
PeimInitializeFlashMap (
  IN EFI_FFS_FILE_HEADER       *FfsHeader,
  IN EFI_PEI_SERVICES          **PeiServices
  )
/*++

Routine Description:

  Build GUIDed HOBs for platform specific flash map

Arguments:

  FfsHeader   - A pointer to the EFI_FFS_FILE_HEADER structure.
  PeiServices - General purpose services available to every PEIM.

Returns:

  EFI_STATUS

--*/
// GC_TODO:    EFI_SUCCESS - add return value to function comment
{
  EFI_STATUS                    Status;
  UINTN                         Index;
  EFI_FLASH_AREA_HOB_DATA       FlashHobData;
  EFI_HOB_FLASH_MAP_ENTRY_TYPE  *Hob;

  //
  // Install FlashMap PPI
  //
  Status = (**PeiServices).InstallPpi (PeiServices, &mPpiListFlashMap);
  ASSERT_PEI_ERROR (PeiServices, Status);

  PEI_DEBUG ((PeiServices, EFI_D_ERROR, "Flash Map PEIM Loaded\n"));

  //
  // Build GUIDed Hobs
  //
  //
  // Build flash area entries as GUIDed HOBs.
  //
  for (Index = 0; Index < mNumOfFlashAreaData; Index++) {
    (*PeiServices)->SetMem (&FlashHobData, sizeof (EFI_FLASH_AREA_HOB_DATA), 0);

    FlashHobData.AreaType               = mFlashAreaData[Index].AreaType;
    FlashHobData.NumberOfEntries        = 1;
    FlashHobData.SubAreaData.Attributes = mFlashAreaData[Index].Attributes;
    FlashHobData.SubAreaData.Base       = (EFI_PHYSICAL_ADDRESS) (UINTN) mFlashAreaData[Index].Base;
    FlashHobData.SubAreaData.Length     = (EFI_PHYSICAL_ADDRESS) (UINTN) mFlashAreaData[Index].Length;

    switch (FlashHobData.AreaType) {
    case EFI_FLASH_AREA_RECOVERY_BIOS:
    case EFI_FLASH_AREA_MAIN_BIOS:
      (*PeiServices)->CopyMem (
                        &FlashHobData.AreaTypeGuid,
                        &mFfsGuid,
                        sizeof (EFI_GUID)
                        );
      (*PeiServices)->CopyMem (
                        &FlashHobData.SubAreaData.FileSystem,
                        &mFvBlockGuid,
                        sizeof (EFI_GUID)
                        );
      break;

    case EFI_FLASH_AREA_GUID_DEFINED:
      (*PeiServices)->CopyMem (
                        &FlashHobData.AreaTypeGuid,
                        &mFlashAreaData[Index].AreaTypeGuid,
                        sizeof (EFI_GUID)
                        );
      (*PeiServices)->CopyMem (
                        &FlashHobData.SubAreaData.FileSystem,
                        &mFvBlockGuid,
                        sizeof (EFI_GUID)
                        );
      break;

    default:
      break;
    }

    Status = PeiBuildHobGuidData (
              PeiServices,
              &gEfiFlashMapHobGuid,
              &FlashHobData,
              sizeof (EFI_FLASH_AREA_HOB_DATA)
              );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  for (Index = 0; Index < mNumOfHobFlashMapData; Index++) {
    Status = (*PeiServices)->CreateHob (
                              PeiServices,
                              EFI_HOB_TYPE_GUID_EXTENSION,
                              (UINT16) (sizeof (EFI_HOB_FLASH_MAP_ENTRY_TYPE)),
                              &Hob
                              );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    (*PeiServices)->CopyMem (
                      Hob,
                      &mFlashMapHobData[Index],
                      sizeof (EFI_HOB_FLASH_MAP_ENTRY_TYPE)
                      );
    if (mFlashMapHobData[Index].AreaType == EFI_FLASH_AREA_EFI_VARIABLES) {
      Hob->Entries[0].Base    = Hob->Entries[0].Base + EFI_RUNTIME_UPDATABLE_FV_HEADER_LENGTH;
      Hob->Entries[0].Length  = Hob->Entries[0].Length - EFI_RUNTIME_UPDATABLE_FV_HEADER_LENGTH;
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
GetAreaInfo (
  IN  EFI_PEI_SERVICES            **PeiServices,
  IN PEI_FLASH_MAP_PPI            *This,
  IN  EFI_FLASH_AREA_TYPE         AreaType,
  IN  EFI_GUID                    *AreaTypeGuid,
  OUT UINT32                      *NumEntries,
  OUT EFI_FLASH_SUBAREA_ENTRY     **Entries
  )
/*++

  Routine Description:

    Implementation of Flash Map PPI

--*/
// GC_TODO: function comment is missing 'Arguments:'
// GC_TODO: function comment is missing 'Returns:'
// GC_TODO:    PeiServices - add argument and description to function comment
// GC_TODO:    This - add argument and description to function comment
// GC_TODO:    AreaType - add argument and description to function comment
// GC_TODO:    AreaTypeGuid - add argument and description to function comment
// GC_TODO:    NumEntries - add argument and description to function comment
// GC_TODO:    Entries - add argument and description to function comment
// GC_TODO:    EFI_SUCCESS - add return value to function comment
// GC_TODO:    EFI_NOT_FOUND - add return value to function comment
{
  EFI_STATUS                    Status;
  EFI_PEI_HOB_POINTERS          Hob;
  EFI_HOB_FLASH_MAP_ENTRY_TYPE  *FlashMapEntry;

  Status = (*PeiServices)->GetHobList (PeiServices, &Hob.Raw);
  while (!END_OF_HOB_LIST (Hob)) {
    if (Hob.Header->HobType == EFI_HOB_TYPE_GUID_EXTENSION && CompareGuid (&Hob.Guid->Name, &gEfiFlashMapHobGuid)) {
      FlashMapEntry = (EFI_HOB_FLASH_MAP_ENTRY_TYPE *) Hob.Raw;
      if (AreaType == FlashMapEntry->AreaType) {
        if (AreaType == EFI_FLASH_AREA_GUID_DEFINED) {
          if (!CompareGuid (AreaTypeGuid, &FlashMapEntry->AreaTypeGuid)) {
            goto NextHob;
          }
        }

        *NumEntries = FlashMapEntry->NumEntries;
        *Entries    = FlashMapEntry->Entries;
        return EFI_SUCCESS;
      }
    }
  NextHob:
    Hob.Raw = GET_NEXT_HOB (Hob);
  }

  return EFI_NOT_FOUND;
}
