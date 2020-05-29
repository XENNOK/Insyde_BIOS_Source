/** @file
  PnpSmm driver initialization

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "PnpSmm.h"
#include <Library/IoLib.h>
#include <Guid/FlashMapHob.h>
#include <EfiFlashMap.h>             // for GPNV

#define FixedPcdGetPtrSize(TokenName)      (UINTN)_PCD_PATCHABLE_##TokenName##_SIZE

EFI_SMM_SYSTEM_TABLE2                   *mSmst;
EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL       *mSmmFwBlockService;

PLATFORM_GPNV_MAP_BUFFER                GPNVMapBuffer;
PLATFORM_GPNV_HEADER                    *GPNVHeader = NULL;
UINTN                                   UpdateableStringCount;
DMI_UPDATABLE_STRING                    *mUpdatableStrings;
UINTN                                   OemGPNVHandleCount = 0;
BOOLEAN                                 mIsOemGPNVMap = FALSE;


/**
  This callback fucntion triggers SMI to SmmPnp callback, in order to get the SMBIOS table entry point.

  @param[In]  Event       Event type
  @param[In]  Context     Context for the event

**/
VOID
EFIAPI
GetSmbiosEntryCallback (
  IN EFI_EVENT                              Event,
  IN VOID                                   *Cotext
  )
{
  gBS->CloseEvent (Event);
  IoWrite8 (SW_SMI_PORT, SMM_PnP_BIOS_CALL);
}

EFI_STATUS
FillPlatformGPNVMapBuffer (
  IN OEM_GPNV_MAP       *GPNVMap
  )
{
  UINTN               Index;
  
  for (Index = 0; Index < OemGPNVHandleCount; Index++) {
    GPNVMapBuffer.PlatFormGPNVMap[Index].Handle      = GPNVMap->Handle;
    GPNVMapBuffer.PlatFormGPNVMap[Index].MinGPNVSize = GPNVMap->GPNVSize;
    GPNVMapBuffer.PlatFormGPNVMap[Index].GPNVSize    = GPNVMap->GPNVSize;
    GPNVMapBuffer.PlatFormGPNVMap[Index].BaseAddress = GPNVMap->GPNVBaseAddress;
    ++GPNVMap;
  }

  return EFI_SUCCESS;
}


/**
  Entry point for SmmPnp drivers.

  @param[In]  ImageHandle           EFI_HANDLE
  @param[In]  SystemTable           EFI_SYSTEM_TABLE

  @retval EFI_SUCCESS           Success
  @retval EFI_DEVICE_ERROR      Fail

**/
EFI_STATUS
EFIAPI
SmmPnpInit (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            SwHandle;
  EFI_SMM_SW_DISPATCH2_PROTOCOL        *SwDispatch;
  EFI_SMM_SW_REGISTER_CONTEXT           SwContext;
  EFI_EVENT                             SmbiosGetTableEvent;
  DMI_UPDATABLE_STRING                 *TempStringTable;
  OEM_GPNV_MAP                         *OemGPNVMap;
  EFI_SMM_BASE2_PROTOCOL                *SmmBase;
  BOOLEAN                               InSmm;
  BOOLEAN                               NeedFreePnpTable;

  SmmBase = NULL;
  Status = gBS->LocateProtocol (&gEfiSmmBase2ProtocolGuid, NULL, (VOID **)&SmmBase);
  InSmm = FALSE;
  if (!EFI_ERROR (Status)) {
    SmmBase->InSmm (SmmBase, &InSmm);
  }

  if (InSmm) {
    Status = SmmBase->GetSmstLocation (SmmBase, &mSmst);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    Status = mSmst->SmmLocateProtocol (&gEfiSmmFwBlockServiceProtocolGuid, NULL, (VOID **)&mSmmFwBlockService);

    UpdateableStringCount = 0;
    mUpdatableStrings     = NULL;
	
    //
    // OemServices (Smm)
    //
    NeedFreePnpTable = TRUE;
    Status = OemSvcSmmInstallPnpStringTable (
               &UpdateableStringCount,
               &TempStringTable
               );

    if (Status == EFI_UNSUPPORTED) {
      NeedFreePnpTable = FALSE;
      TempStringTable = PcdGetPtr(PcdSmbiosUpdatableStringTable);
      UpdateableStringCount = FixedPcdGetPtrSize(PcdSmbiosUpdatableStringTable) / sizeof(DMI_UPDATABLE_STRING);
    }

    if ((TempStringTable != NULL) && (UpdateableStringCount != 0)) {
      Status = mSmst->SmmAllocatePool (
                        EfiRuntimeServicesData,
                        UpdateableStringCount * sizeof (DMI_UPDATABLE_STRING),
                        (VOID **)&mUpdatableStrings
                        );
      if (!EFI_ERROR (Status)) {
        CopyMem (mUpdatableStrings, TempStringTable, UpdateableStringCount * sizeof (DMI_UPDATABLE_STRING));
      }
      if (NeedFreePnpTable) {
        Status = gBS->FreePool(TempStringTable);
      }
    }

    //
    // OemServices (Smm)
    //
	OemGPNVHandleCount = 0;
	OemGPNVMap = NULL;
    Status = OemSvcSmmInstallPnpGpnvTable (&OemGPNVHandleCount, &OemGPNVMap);
    if (Status == EFI_MEDIA_CHANGED && OemGPNVMap != NULL) {
      FillPlatformGPNVMapBuffer (OemGPNVMap);
      mIsOemGPNVMap = TRUE;
    } else {
      mIsOemGPNVMap = FALSE;
      }

    //
    // Get the Sw dispatch protocol
    //
    Status = mSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, (VOID **)&SwDispatch);
    ASSERT_EFI_ERROR (Status);

    //
    // Register SMBIOS call SMI function
    //
    SwContext.SwSmiInputValue = SMM_PnP_BIOS_CALL;
    SwHandle = NULL;
    Status = SwDispatch->Register (SwDispatch, PnPBiosCallback, &SwContext, &SwHandle);
    ASSERT_EFI_ERROR (Status);
  } else {
    //
    // This event will be triggered after SMBIOS table entry point is ready.
    // So, we can get the address of entry point and save it.
    //
    Status = EfiCreateEventReadyToBootEx (
               TPL_CALLBACK - 1,
               GetSmbiosEntryCallback,
               NULL,
               &SmbiosGetTableEvent
               );
  }

 return EFI_SUCCESS;
}

