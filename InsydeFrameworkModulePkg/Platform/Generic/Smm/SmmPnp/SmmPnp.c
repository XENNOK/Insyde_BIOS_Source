//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include "SmmPnp.h"
#include EFI_PROTOCOL_DEFINITION (PciRootBridgeIo)

EFI_EVENT_LOG_PROTOCOL                  *SmmEventLog;

EFI_SMM_SYSTEM_TABLE                    *mSmst;
EFI_SMM_RUNTIME_PROTOCOL                *mSmmRT;

PLATFORM_GPNV_MAP_BUFFER                GPNVMapBuffer;
PLATFORM_GPNV_HEADER                    *GPNVHeader;
EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL       *SmmFwBlockService;

UINTN                                   UpdateableStringCount;
DMI_UPDATABLE_STRING                    *mUpdatableStrings;

UINTN                                   OemGPNVHandleCount;
OEM_DEFINE_GPNV_HANDLE                  *mOemGPNVHandleTab;

VOID
EFIAPI
SmbiosGetTableEntryPointCallback (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  EFI_STATUS                                Status;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL           *IoDev;
  UINT8                                     Value;

  Status = gBS->CloseEvent (Event);
  Status = gBS->LocateProtocol (&gEfiPciRootBridgeIoProtocolGuid, NULL, &IoDev);
  if (Status == EFI_SUCCESS) {
    Value = SMM_PnP_BIOS_CALL;
    Status = IoDev->Io.Write (
                         IoDev,
                         EfiPciWidthUint8,
                         SW_SMI_PORT,
                         1,
                         &Value
                         );
  }
}

EFI_DRIVER_ENTRY_POINT(SmmPnpInit)

EFI_STATUS
SmmPnpInit (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
  EFI_STATUS                            Status;
  BOOLEAN                               InSmm;
  EFI_HANDLE                            SwHandle;
  EFI_SMM_RUNTIME_PROTOCOL              *SmmRT;
  EFI_SMM_SW_DISPATCH_PROTOCOL          *SwDispatch;
  EFI_SMM_SW_DISPATCH_CONTEXT           SwContext;

  SMM_OEM_SERVICES_PROTOCOL             *OemServices;
  DMI_UPDATABLE_STRING                  *TempStringTable;

  OEM_DEFINE_GPNV_HANDLE                *TempOemGPNVHandleTab;
  EFI_EVENT                             SmbiosGetTableEvent;

  SwHandle = NULL;
  
  //
  // Initialize the EFI Runtime Library
  //
  InsydeEfiInitializeSmmDriverLib (ImageHandle, SystemTable, &InSmm);
  
  Status = gBS->LocateProtocol (&gEfiSmmRuntimeProtocolGuid, NULL, &SmmRT);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (InSmm) {
    //
    // Great!  We're now in SMM!
    //
    //
    // Initialize global variables
    //
    Status = gSMM->GetSmstLocation (gSMM, &mSmst);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = SmmRT->LocateProtocol (
                      &gEfiSmmRuntimeProtocolGuid,
                      NULL,
                      &mSmmRT
                      );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = mSmmRT->LocateProtocol (
                       &gEfiSmmFwBlockServiceProtocolGuid,
                       NULL,
                       &SmmFwBlockService
                       );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    UpdateableStringCount = 0;
    mUpdatableStrings = NULL;
    OemGPNVHandleCount = 0;
    mOemGPNVHandleTab = NULL;

    Status = gBS->LocateProtocol (
                    &gSmmOemServicesProtocolGuid,
                    NULL,
                    &OemServices
                    );
    if (!EFI_ERROR (Status)) {
      Status = OemServices->Funcs[COMMON_PNP_UPDATE_STRING_TABLE] (
                              OemServices,
                              COMMON_PNP_UPDATE_STRING_TABLE_ARG_COUNT,
                              &UpdateableStringCount,
                              &TempStringTable
                              );
      if (!EFI_ERROR (Status) && (UpdateableStringCount != 0)) {
        Status = mSmst->SmmAllocatePool (EfiRuntimeServicesData,
                                         UpdateableStringCount * sizeof (DMI_UPDATABLE_STRING),
                                         &mUpdatableStrings
                                         );
        if (!EFI_ERROR (Status)) {
          EfiCopyMem (mUpdatableStrings,
                      TempStringTable,
                      UpdateableStringCount * sizeof (DMI_UPDATABLE_STRING)
                      );
        }
        gBS->FreePool (TempStringTable);
      }
      Status = OemServices->Funcs[COMMON_PNP_GPNV_HANDLE_TABLE] (
                              OemServices,
                              COMMON_PNP_GPNV_HANDLE_TABLE_ARG_COUNT,
                              &OemGPNVHandleCount,
                              &TempOemGPNVHandleTab
                              );
      if (!EFI_ERROR (Status) && (OemGPNVHandleCount != 0)) {
        Status = mSmst->SmmAllocatePool (EfiRuntimeServicesData,
                                         OemGPNVHandleCount * sizeof (OEM_DEFINE_GPNV_HANDLE),
                                         &mOemGPNVHandleTab
                                         );
        if (!EFI_ERROR (Status)) {
          EfiCopyMem (mOemGPNVHandleTab,
                      TempOemGPNVHandleTab,
                      OemGPNVHandleCount * sizeof (OEM_DEFINE_GPNV_HANDLE)
                      );
        }
        gBS->FreePool (TempOemGPNVHandleTab);
      }
    }

    //
    //  Get the Sw dispatch protocol
    //
    Status = gBS->LocateProtocol (
                    &gEfiSmmSwDispatchProtocolGuid,
                    NULL,
                    &SwDispatch
                    );
    ASSERT_EFI_ERROR (Status);

    //
    // Register SMBIOS call SMI function
    //
    SwContext.SwSmiInputValue = SMM_PnP_BIOS_CALL;
    Status = SwDispatch->Register (
                           SwDispatch,
                           PnPBiosCallback,
                           &SwContext,
                           &SwHandle
                           );
    ASSERT_EFI_ERROR (Status);

    GetGPNVMap ();

    SmmEventLog = NULL;
    mSmmRT->LocateProtocol (
              &gEfiEventLogProtocolGuid,
              NULL,
              &SmmEventLog
              );
  } else {
    //
    // This event will be triggered after SMBIOS table entry point is ready.
    // So, we can get the address of entry point and save it.
    //
    Status = EfiCreateEventReadyToBoot (
               EFI_TPL_CALLBACK - 1,
               SmbiosGetTableEntryPointCallback,
               NULL,
               &SmbiosGetTableEvent
               );
  }
    
  return Status;
}

VOID
GetGPNVMap (
  VOID
  )
{
  EFI_FLASH_MAP_ENTRY_DATA      *FlashMapEntryData;
  VOID                          *HobList;
  VOID                          *Buffer;
  EFI_STATUS                    Status;
  UINT32                         i;
  UINT16                         GPNVCount;
  UINT32                        MinGPNVSize;


  Status = EfiLibGetSystemConfigurationTable (&gEfiHobListGuid, &HobList);
  ASSERT_EFI_ERROR (Status);

  i = 0;
  GPNVCount = 0;
  MinGPNVSize = 0xFFFF;

  EfiSetMem (&GPNVMapBuffer, sizeof (PLATFORM_GPNV_MAP_BUFFER), 0xff);

  for (;;) {
    Status = GetNextGuidHob (&HobList, &gEfiFlashMapHobGuid, &Buffer, NULL);
    if (EFI_ERROR (Status)) {
      Status = FillPlatformGPNVMapBuffer (
                 (UINT16)0xFFFF,
                 0,
                 0,
                 0
                 );
      break;
    }

    FlashMapEntryData = (EFI_FLASH_MAP_ENTRY_DATA *) Buffer;

    //
    // Get the variable store area
    //

    switch (FlashMapEntryData->AreaType) {

    case EFI_FLASH_AREA_SMBIOS_LOG:  //GPNV header
      if ((UINT32)GPNVHeader->Signature != GPNV_SIGNATURE) {
        GPNVHeader = (PLATFORM_GPNV_HEADER *)(UINTN)((UINT32)FlashMapEntryData->Entries[0].Base);
        if ((UINT32)GPNVHeader->Signature != GPNV_SIGNATURE) {
            return;
        }
       }
      break;

    case EFI_FLASH_AREA_GPNV:
      Status = FillPlatformGPNVMapBuffer (
                 (UINT16)GPNVCount,
                 (UINTN)(FlashMapEntryData->Entries[0].Length),
                 (UINTN)(FlashMapEntryData->Entries[0].Length),
                 (UINTN)(FlashMapEntryData->Entries[0].Base)
                 );
      GPNVCount++;
      break;

    default:
      break;
    }
    if (EFI_ERROR (Status)) {
      break;
    }
  }

  return ;
}

EFI_STATUS
FillPlatformGPNVMapBuffer (
  IN UINT16   Handle,
  IN UINTN    MinGPNVSize,
  IN UINTN    GPNVSize,
  IN UINTN    BaseAddress
)
{
  if (Handle > (sizeof (PLATFORM_GPNV_MAP_BUFFER) / sizeof (PLATFORM_GPNV_MAP)) ) {
   return EFI_UNSUPPORTED;
  }

  if (mOemGPNVHandleTab == NULL) {
    GPNVMapBuffer.PlatFormGPNVMap[Handle].Handle = Handle;
  } else {
    if (OemGPNVHandleCount >= (UINTN)(Handle + 1)) {
      GPNVMapBuffer.PlatFormGPNVMap[Handle].Handle = mOemGPNVHandleTab[Handle].OemHandle;
    } else {
      return EFI_UNSUPPORTED;
    }
  }
  GPNVMapBuffer.PlatFormGPNVMap[Handle].MinGPNVSize = MinGPNVSize;
  GPNVMapBuffer.PlatFormGPNVMap[Handle].GPNVSize    = GPNVSize;
  GPNVMapBuffer.PlatFormGPNVMap[Handle].BaseAddress = BaseAddress;

  return EFI_SUCCESS;
}
