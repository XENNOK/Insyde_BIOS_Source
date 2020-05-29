/** @file
  This is a simple variable default update driver. This driver checks policy to update
  default setting to NV_VARIABLE_STORE and provide interface for update default setting to
  NV_VARIABLE_STORE.

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


#include "VariableDefaultUpdate.h"

extern EFI_GUID        gEfiAlternateFvBlockGuid;
/**
  This function uses to update system setting to factory default.

  @param  This                    Pointer to EFI_VARIABLE_DEFAULT_UPDATE_PROTOCOL instance.
  @param  RestoreType             Restore type to update for the variable store.

  @retval EFI_INVALID_PARAMETER   Input parameter is invalid.
  @retval EFI_SUCCESS             Update system setting to factory default successful.
  @return Other                   Other error cause update system to factory default failed.

**/
EFI_STATUS
EFIAPI
UpdateFactorySetting (
  IN      EFI_VARIABLE_DEFAULT_UPDATE_PROTOCOL     *This,
  IN      UINT32                                   RestoreType
  )
{
  VARIABLE_DEFAULT_UPDATE_DEVICE        *VariableDefaultDevice;
  UINT8                                 *SpareBuffer;
  EFI_STATUS                            Status;
  UINT8                                 *VariableBuffer;
  UINTN                                 VariableStoreLength;


  if (This == NULL || (RestoreType != RESTORE_WITH_CLEARING_ALL_SETTINGS && RestoreType != RESTORE_WITH_RESERVING_OTHER_SETTINGS)) {
    return EFI_INVALID_PARAMETER;
  }

  VariableDefaultDevice = VARIABLE_DEFAULT_INSTANCE_FROM_THIS (This);
  if (!IsFactoryCopyValid (VariableDefaultDevice)) {
    return EFI_NOT_FOUND;
  }
  //
  // Check system is whether in factory resotre process. System cannot execure
  // restore factory copy process.
  //
  if (InRestoreFactoryDefaultProcess (VariableDefaultDevice)) {
    return EFI_ABORTED;;
  }

  SpareBuffer    = NULL;
  VariableBuffer = NULL;
  SpareBuffer = AllocateZeroPool (VariableDefaultDevice->StoreDev[SPARE_STORE].StoreLength);
  if (SpareBuffer == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }

  //
  // Five steps:
  // 1.Backup spare data.
  // 2.Write collected factory data to spare store.
  // 3.Update state to FACTORY_DEFAULT_STATE.
  // 4.Write collected factory data to variable store.
  // 5.Restore original data to spare store.
  //

  //
  // 1.Backup spare sub-region data
  //
  Status = DefaultUpdateReadBlock (
             VariableDefaultDevice,
             VariableDefaultDevice->StoreDev[SPARE_STORE].StoreLength,
             SPARE_STORE,
             0,
             SpareBuffer
             );
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  //
  // 2.Write collected factory data to spare store.
  //
  VariableStoreLength = VariableDefaultDevice->StoreDev[VARIABLE_STORE].StoreLength;
  VariableBuffer = AllocateZeroPool (VariableStoreLength + sizeof (FACTORY_DEFAULT_HEADER));
  SetMem (VariableBuffer, VariableStoreLength, 0xFF);
  Status = CollectFactoryDefaultSetting (
             VariableDefaultDevice,
             RestoreType,
             &VariableStoreLength,
             VariableBuffer + sizeof (FACTORY_DEFAULT_HEADER)
             );
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  Status = DefaultUpdateEraseBlock (
             VariableDefaultDevice,
             SPARE_STORE,
             0,
             VariableDefaultDevice->StoreDev[SPARE_STORE].NumberOfBlock
             );
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  Status = DefaultUpdateWriteBlock (
             VariableDefaultDevice,
             VariableStoreLength + sizeof (FACTORY_DEFAULT_HEADER),
             SPARE_STORE,
             0,
             VariableBuffer
             );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  //
  // 3.Update state to FACTORY_DEFAULT_STATE.
  //
  Status = SetDefaultUpdateState (VariableDefaultDevice, FACTORY_DEFAULT_STATE);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  //
  // 4.Write collected factory data to variable store.
  //
  Status = RestoreFactoryDefaultToVariableStore (
             VariableDefaultDevice,
             VariableStoreLength,
             VariableBuffer + sizeof (FACTORY_DEFAULT_HEADER)
             );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  //
  // 5.Restore original data to spare store
  //
  Status = DefaultUpdateEraseBlock (
             VariableDefaultDevice,
             SPARE_STORE,
             0,
             VariableDefaultDevice->StoreDev[SPARE_STORE].NumberOfBlock
             );
  Status = DefaultUpdateWriteBlock (
             VariableDefaultDevice,
             VariableDefaultDevice->StoreDev[SPARE_STORE].StoreLength,
             SPARE_STORE,
             0,
             SpareBuffer
             );

Done:
  if (SpareBuffer != NULL) {
    gBS->FreePool (SpareBuffer);
  }
  if (VariableBuffer != NULL) {
    gBS->FreePool (VariableBuffer);
  }

  return Status;
}


/**
  This function uses to update system setting to backup default.

  @param  This                    Pointer to EFI_VARIABLE_DEFAULT_UPDATE_PROTOCOL instance.
  @param  DevicePath              Device path pointer to the source of backup settings.
  @param  RestoreType             Restore type to update for the variable store.

  @retval EFI_INVALID_PARAMETER   Input parameter is invalid.
  @retval EFI_SUCCESS             Update system setting to backup default successful.
  @return Other                   Other errors cause update system to backup default failed.

**/
EFI_STATUS
EFIAPI
UpdateBackupSetting (
  IN      EFI_VARIABLE_DEFAULT_UPDATE_PROTOCOL     *This,
  IN      EFI_DEVICE_PATH_PROTOCOL                 *DevicePath,
  IN      UINT32                                   RestoreType
  )
{
  if (This == NULL || DevicePath == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_UNSUPPORTED;
}

/**
  Check this input handle is whether a NV store FVB handle.

  @param[in] Handle    Input EFI_HANDLE instance

  @retval TRUE         This is NV storage FVB handle.
  @retval FALSE        This isn't NV storage FVB handle.
**/
STATIC
BOOLEAN
IsNvStorageHandle (
  EFI_HANDLE      Handle
  )
{
  EFI_STATUS   Status;

  Status = gBS->OpenProtocol (
                  Handle,
                  &gEfiFirmwareVolumeBlockProtocolGuid,
                  NULL,
                  NULL,
                  NULL,
                  EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }
  Status = gBS->OpenProtocol (
                  Handle,
                  &gEfiAlternateFvBlockGuid,
                  NULL,
                  NULL,
                  NULL,
                  EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                  );

  return (BOOLEAN) (Status == EFI_SUCCESS);
}

/**
  This function is the entry point of the variable default update driver.

  @param  ImageHandle       EFI_HANDLE: A handle for the image that is initializing
                            this driver.
  @param  SystemTable       EFI_SYSTEM_TABLE: A pointer to the EFI system table.

  @retval EFI_SUCCESS       Variable default device has finished the initialization.
  @retval EFI_ABORTED       Error occurs during Variable default device initialization.

**/
EFI_STATUS
EFIAPI
InitializeVariableDefaultUpdate (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL        *Fvb;
  UINTN                                     Index;
  UINTN                                     LbaIndex;
  UINTN                                     StoreIndex;
  EFI_HANDLE                                *HandleBuffer;
  UINTN                                     HandleCount;
  EFI_FIRMWARE_VOLUME_HEADER                *FwVolHeader;
  EFI_STATUS                                Status;
  EFI_FV_BLOCK_MAP_ENTRY                    *FvbMapEntry;
  EFI_PHYSICAL_ADDRESS                      BaseAddress;
  VARIABLE_DEFAULT_UPDATE_DEVICE            *VariableDefaultDevice;
  FACTORY_DEFAULT_HEADER                    *FactoryDefaultHeader;

  VariableDefaultDevice = AllocateZeroPool (sizeof (VARIABLE_DEFAULT_UPDATE_DEVICE));
  if (VariableDefaultDevice == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  VariableDefaultDevice->Signature = VARIABLE_DEFAULT_UPDATE_SIGNATURE;

  //
  // Get the variable store  Flash Map SUB area
  //
  VariableDefaultDevice->StoreDev[VARIABLE_STORE].StoreAddress = (EFI_PHYSICAL_ADDRESS) (UINTN) (PcdGet64 (PcdFlashNvStorageVariableBase64) != 0 ?
                                                                                                 PcdGet64 (PcdFlashNvStorageVariableBase64) :
                                                                                                 PcdGet32 (PcdFlashNvStorageVariableBase)
                                                                                                 );
  VariableDefaultDevice->StoreDev[VARIABLE_STORE].StoreLength  = (UINTN) PcdGet32 (PcdFlashNvStorageVariableSize);

  //
  // Get the spare store SUB area
  //
  VariableDefaultDevice->StoreDev[SPARE_STORE].StoreAddress = (EFI_PHYSICAL_ADDRESS) (UINTN) PcdGet32 (PcdFlashNvStorageFtwSpareBase);
  VariableDefaultDevice->StoreDev[SPARE_STORE].StoreLength  = (UINTN) PcdGet32 (PcdFlashNvStorageFtwSpareSize);

  //
  // Get the factory default SUB area
  //
  VariableDefaultDevice->StoreDev[FACTORY_COPY_STORE].StoreAddress = (EFI_PHYSICAL_ADDRESS) (UINTN) PcdGet32 (PcdFlashNvStorageFactoryCopyBase);
  VariableDefaultDevice->StoreDev[FACTORY_COPY_STORE].StoreLength  = (UINTN) PcdGet32 (PcdFlashNvStorageFactoryCopySize);


  ASSERT (VariableDefaultDevice->StoreDev[FACTORY_COPY_STORE].StoreLength <= VariableDefaultDevice->StoreDev[VARIABLE_STORE].StoreLength);
  //
  // Locate FVB protocol
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolumeBlockProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  ASSERT_EFI_ERROR (Status);

  ASSERT (HandleCount > 0);

  for (Index = 0; Index < HandleCount; Index += 1) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiFirmwareVolumeBlockProtocolGuid,
                    (VOID **) &Fvb
                    );
    ASSERT_EFI_ERROR (Status);

    Status = Fvb->GetPhysicalAddress (Fvb, &BaseAddress);
    if (EFI_ERROR (Status)) {
      continue;
    }

    FwVolHeader = (EFI_FIRMWARE_VOLUME_HEADER *) ((UINTN) BaseAddress);

    for (StoreIndex = 0; StoreIndex < MAX_NUM_STORE; StoreIndex++) {
      if ((VariableDefaultDevice->StoreDev[StoreIndex].StoreAddress >= BaseAddress) &&
          (VariableDefaultDevice->StoreDev[StoreIndex].StoreAddress < (BaseAddress + FwVolHeader->FvLength) &&
          IsNvStorageHandle (HandleBuffer[Index]))
          ) {
        //
        // FV may have multiple types of BlockLength
        //
        FvbMapEntry = &FwVolHeader->BlockMap[0];
        while (!((FvbMapEntry->NumBlocks == 0) && (FvbMapEntry->Length == 0))) {
          for (LbaIndex = 1; LbaIndex <= FvbMapEntry->NumBlocks; LbaIndex += 1) {
            if (VariableDefaultDevice->StoreDev[StoreIndex].StoreAddress < (BaseAddress + FvbMapEntry->Length * LbaIndex)) {
              VariableDefaultDevice->StoreDev[StoreIndex].Fvb = Fvb;
              VariableDefaultDevice->StoreDev[StoreIndex].StoreLba = LbaIndex - 1;
              //
              // Get the Work space size and Base(Offset)
              //
              VariableDefaultDevice->StoreDev[StoreIndex].SizeofBlock = FvbMapEntry->Length;
              VariableDefaultDevice->StoreDev[StoreIndex].NumberOfBlock = VariableDefaultDevice->StoreDev[StoreIndex].StoreLength / FvbMapEntry->Length;
              break;
            }
          }
          FvbMapEntry++;
        }
      }
    }
  }
  //
  // Process restore facotry default action if system powers off during restore facotry
  // default process in previous POST.
  //
  if (InRestoreFactoryDefaultProcess (VariableDefaultDevice)) {
    Status = UpdateFactorySettingFromBackup (&VariableDefaultDevice->DefaultUpdateInstance);
    if (!EFI_ERROR (Status)) {
      gST->RuntimeServices->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
    }
  }
  //
  // Update factory default sub-region length if the length isn't initialized.
  //
  FactoryDefaultHeader =  (FACTORY_DEFAULT_HEADER *) (UINTN) VariableDefaultDevice->StoreDev[FACTORY_COPY_STORE].StoreAddress;
  if (~FactoryDefaultHeader->Length == 0) {
    Status = DefaultUpdateWriteBlock (
               VariableDefaultDevice,
               sizeof (UINT32),
              FACTORY_COPY_STORE,
              sizeof (UINT32),
              (UINT8 *) &VariableDefaultDevice->StoreDev[FACTORY_COPY_STORE].StoreLength
              );
    ASSERT_EFI_ERROR (Status);
  }


  if (!IsFacotryCopyHeaderValid (VariableDefaultDevice)) {
    DEBUG ((EFI_D_ERROR, "Try to recovery Factory Copy region!!!\n"));
    Status = RecoveryFactoryDefault (VariableDefaultDevice);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Recovery Factory Copy regions failed!!!\n"));
      ASSERT (FALSE);
    }
  }

  VariableDefaultDevice->DefaultUpdateInstance.UpdateFactorySetting = UpdateFactorySetting;
  VariableDefaultDevice->DefaultUpdateInstance.UpdateBackupSetting = UpdateBackupSetting;
  //
  // Install protocol interface
  //
  Status = gBS->InstallProtocolInterface (
                  &VariableDefaultDevice->Handle,
                  &gEfiVariableDefaultUpdateProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &VariableDefaultDevice->DefaultUpdateInstance
                  );

  return Status;
}
