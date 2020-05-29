/** @file
  Driver to trigger CapsuleUpdate Flash Loader and Firmware Update

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/SeamlessRecoveryLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/DevicePath.h>
#include <Library/DevicePathLib.h>
#include <Library/VariableLib.h>
#include <Guid/GlobalVariable.h>
#include <Guid/EfiSystemResourceTable.h>
#include <SecureFlash.h>
#include "CapsuleLoaderTriggerDxe.h"
#include <Guid/DebugMask.h>

UINT8                     mBootOptionData[MAX_BOOT_OPTION_DATA_SIZE];

/**
  Convert EFI Status to ESRT status

  @param[in] Status     The value of EFI status

  @return    The converted ESRT status
             Listing of ESRT status
               ESRT_SUCCESS                      0x00000000
               ESRT_ERROR_UNSUCCESSFUL           0x00000001
               ESRT_ERROR_INSUFFICIENT_RESOURCES 0x00000002
               ESRT_ERROR_INCORRECT_VERSION      0x00000003
               ESRT_ERROR_INVALID_IMAGE_FORMAT   0x00000004
               ESRT_ERROR_AUTHENTICATION         0x00000005
               ESRT_ERROR_AC_NOT_CONNECTED       0x00000006
               ESRT_ERROR_INSUFFICIENT_BATTERY   0x00000007
**/
ESRT_STATUS
GetEsrtStatus (
  EFI_STATUS Status
  )
{
  ESRT_STATUS EsrtStatus;
  switch (Status) {
    case EFI_SUCCESS:
      EsrtStatus = ESRT_SUCCESS;
      break;
    case EFI_INCOMPATIBLE_VERSION:
      EsrtStatus = ESRT_ERROR_INCORRECT_VERSION;
      break;
    case EFI_OUT_OF_RESOURCES:
    case EFI_VOLUME_FULL:
      EsrtStatus = ESRT_ERROR_INSUFFICIENT_RESOURCES;
      break;
    case EFI_LOAD_ERROR:
      EsrtStatus = ESRT_ERROR_INVALID_IMAGE_FORMAT;
      break;
    case EFI_SECURITY_VIOLATION:
      EsrtStatus = ESRT_ERROR_AUTHENTICATION;
      break;
    default:
      EsrtStatus = ESRT_ERROR_UNSUCCESSFUL;
  }
  return EsrtStatus;
}

/**
  Retrieve the device path of a boot option

  @param[in] BootIndex     The index of the boot option

  @return    The device path of the specified boot option
  @retval    NULL          Failed to get the device path of the boot option
**/
EFI_DEVICE_PATH_PROTOCOL *
GetBootOptionDevicePath (
  UINT16     BootIndex
  )
{
  EFI_STATUS                Status;
  CHAR16                    BootOption[] = L"Boot0000";
  CHAR16                    *BootOptionDesc;
  UINTN                     Size;
  EFI_DEVICE_PATH_PROTOCOL  *BootDevicePath;

  BootDevicePath = NULL;

  UnicodeSPrint (BootOption, sizeof(BootOption), L"Boot%04x", BootIndex);
  Size = MAX_BOOT_OPTION_DATA_SIZE;
  Status = CommonGetVariable (
             BootOption,
             &gEfiGlobalVariableGuid,
             &Size,
             mBootOptionData
             );
  if (Status != EFI_SUCCESS) {
    return NULL;
  }

  //
  // Get the boot loader file path from the current Boot Option data
  //
  BootOptionDesc = (CHAR16 *)(mBootOptionData + sizeof(UINT32) + sizeof(UINT16));
  BootDevicePath = (EFI_DEVICE_PATH_PROTOCOL *)(mBootOptionData + sizeof(UINT32) + sizeof(UINT16) + StrSize(BootOptionDesc));

  return BootDevicePath;
}

/**
  Expand a file device path to full device path

  @param[in] FileDevicePath     The device path to be expanded

  @return    The expanded full device path
  @retval    NULL               Failed to expand the device path
**/
EFI_DEVICE_PATH_PROTOCOL *
ExpandToFullDevicePath (
  EFI_DEVICE_PATH_PROTOCOL *FileDevicePath
  )
{
  EFI_STATUS Status;
  UINTN Index;
  UINTN NumberOfHandles;
  EFI_HANDLE *HandleBuffer;
  EFI_DEVICE_PATH *FullDevicePath;
  EFI_DEVICE_PATH *ParentDevicePath;
  EFI_DEVICE_PATH *Node;
  UINTN ParentDevicePathSize;
  UINTN FileDevicePathSize;

  Status = gBS->LocateHandleBuffer(
                  ByProtocol,
                  &gEfiSimpleFileSystemProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR(Status)) {
    return NULL;
  }
  FullDevicePath = NULL;
  ParentDevicePathSize = 0;
  ParentDevicePath = NULL;
  for (Index = 0; Index < NumberOfHandles; Index++) {
    Status = gBS->HandleProtocol(
                    HandleBuffer[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID **)&ParentDevicePath
                    );
    if (!EFI_ERROR(Status)) {
      Node = ParentDevicePath;
      Status = EFI_NOT_FOUND;
      while (!IsDevicePathEnd(Node)) {
        if ((DevicePathType (Node) == DevicePathType (FileDevicePath)) &&
            (DevicePathSubType (Node) == DevicePathSubType(FileDevicePath)) &&
            (DevicePathNodeLength(Node) == DevicePathNodeLength(FileDevicePath)) &&
            (CompareMem(Node, FileDevicePath, DevicePathNodeLength(Node)) == 0)
           ) {
          ParentDevicePathSize = (UINT8 *)Node - (UINT8 *)ParentDevicePath;
          Status = EFI_SUCCESS;
          break;
        } else {
          Status = EFI_NOT_FOUND;
        }
        Node = NextDevicePathNode (Node);
      }
      if (Status == EFI_SUCCESS) {
        break;
      }
    }
  }
  if (!EFI_ERROR(Status)) {
    FileDevicePathSize = GetDevicePathSize(FileDevicePath);
    FullDevicePath = (EFI_DEVICE_PATH_PROTOCOL *)AllocatePool(ParentDevicePathSize + FileDevicePathSize);
    CopyMem(FullDevicePath, ParentDevicePath, ParentDevicePathSize);
    CopyMem((UINT8 *)FullDevicePath + ParentDevicePathSize, FileDevicePath, FileDevicePathSize);
  }
  FreePool(HandleBuffer);
  return FullDevicePath;

}

VOID
DisableBiosRegionLock(
  VOID
  )
{
  //
  // Delele SecureFlashInfo variable to disable BIOS region lock
  //
  CommonSetVariable (
    SECURE_FLASH_INFORMATION_NAME,
    &gSecureFlashInfoGuid,
    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
    0,
    NULL
    );
}

/**
  ReadyToBoot notification event handler.

  @param[in] Event              Event whose notification function is being invoked.
  @param[in] Context            Pointer to the notification function's context.

  @retval    None
**/
VOID
EFIAPI
TriggerCapsuleUpdateLoader (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS       Status;
  EFI_DEVICE_PATH  *CapsuleDevicePath;
  EFI_FILE_HANDLE  CapsuleHandle;
  UINT16           BootCurrent;

  gBS->CloseEvent(Event);

  CapsuleDevicePath = GetBootOptionDevicePath (INSYDE_MAGIC_BOOT_INDEX);
  if (CapsuleDevicePath == NULL) {
    ASSERT(CapsuleDevicePath != NULL);
    return;
  }
  CapsuleDevicePath = ExpandToFullDevicePath(CapsuleDevicePath);
  if (CapsuleDevicePath != NULL) {
    EnableCapsuleSecurityCheck();
    DisableBiosRegionLock();
    Status = gBS->LoadImage (FALSE, gImageHandle, CapsuleDevicePath, NULL, 0, (VOID **)&CapsuleHandle);
    FreePool(CapsuleDevicePath);
    if (!EFI_ERROR(Status)) {
      BootCurrent = INSYDE_MAGIC_BOOT_INDEX;
      Status =CommonSetVariable (
                L"BootCurrent",
                &gEfiGlobalVariableGuid,
                EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                sizeof(UINT16),
                &BootCurrent
                );
      Status = gBS->StartImage(CapsuleHandle, NULL, NULL);
    }
    DisableCapsuleSecurityCheck();
  }
}

/**
  ReadyToBoot notification event handler for capsule image update.

  @param[in] Event              Event whose notification function is being invoked.
  @param[in] Context            Pointer to the notification function's context.

  @retval    None
**/
VOID
EFIAPI
TriggerFirmwareUpdate (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS                  Status;
  EFI_DEVICE_PATH             *CapsuleDevicePath;
  EFI_FILE_HANDLE             CapsuleHandle;
  EFI_SYSTEM_RESOURCE_TABLE   *Esrt;
  UINTN                       Index;
  ESRT_STATUS                 EsrtStatus;
  BOOLEAN                     WindowsUxCapsuleFound;

  gBS->CloseEvent(Event);

  Status = EfiGetSystemConfigurationTable (&gEfiSystemResourceTableGuid, (VOID **)&Esrt);
  if (Status != EFI_SUCCESS) {
    return;
  }

  WindowsUxCapsuleFound = FALSE;

  for (Index = 0; Index < Esrt->FirmwareResourceCount; Index++) {
    CapsuleHandle = NULL;
    CapsuleDevicePath = CommonGetVariableData (
                          L"CapsuleDevicePath",
                          &Esrt->FirmwareResources[Index].FirmwareClass
                          );
    if (CapsuleDevicePath == NULL) {
      continue;
    }
    Status = CommonSetVariable (
               L"CapsuleDevicePath",
               &Esrt->FirmwareResources[Index].FirmwareClass,
               EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
               0,
               NULL
               );
    if (CapsuleDevicePath != NULL) {
      Status = EnableCapsuleSecurityCheck();
      DisableBiosRegionLock();
      WindowsUxCapsuleFound = DisplayWindowsUxCapsule();
      if (!EFI_ERROR(Status)) {
        Status = gBS->LoadImage (FALSE, gImageHandle, CapsuleDevicePath, NULL, 0, (VOID **)&CapsuleHandle);
      }
      FreePool(CapsuleDevicePath);
      if (Status == EFI_SUCCESS) {
        SetFirmwareUpdatingFlag(TRUE);
        Status = gBS->StartImage(CapsuleHandle, NULL, NULL);
        SetFirmwareUpdatingFlag(FALSE);
      } else {
        Status = EFI_SECURITY_VIOLATION;
      }
      DisableCapsuleSecurityCheck();
      EsrtStatus = GetEsrtStatus(Status);
      CommonSetVariable (
        L"EsrtLastAttemptStatus",
        &Esrt->FirmwareResources[Index].FirmwareClass,
        EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
        sizeof(UINT32),
        &EsrtStatus
        );
      gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
    }
  }
  //
  // Remove WindowsUxCapsuleDevicePath variable after all the firmware images have been updated
  //
  if (WindowsUxCapsuleFound) {
    Status = CommonSetVariable (
               L"WindowsUxCapsuleDevicePath",
               &gWindowsUxCapsuleGuid,
               EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
               0,
               NULL
               );
  }
}

/**
  CapsuleLoaderTrigger DXE driver initialization


  @param ImageHandle     A handle for the image that is initializing this driver
  @param SystemTable     A pointer to the EFI system table

  @retval EFI_SUCCESS:   Module initialized successfully
**/
EFI_STATUS
EFIAPI
CapsuleLoaderTriggerInit (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                     Status;
  EFI_EVENT                      ReadyToBootEvent;
  EFI_EVENT                      ReadyToBootEvent2;
  UINTN                          Size;
  UINT16                         BootNext;
  UINT8                          Data;

  if (IsFirmwareFailureRecovery ()) {
    Data = FIRMWARE_FAILURE_FLAG_DATA;
    Size = sizeof (UINT8);
    Status = CommonSetVariable (
                    FIRMWARE_FAILURE_FLAG_NAME,
                    &gEfiGenericVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    Size,
                    &Data
                    );
  }

  SetFirmwareUpdatingFlag(FALSE);
  Size = sizeof(UINT16);
  Status = CommonGetVariable (
          L"BootNext",
          &gEfiGlobalVariableGuid,
          &Size,
          &BootNext
          );
  if (!EFI_ERROR(Status) && BootNext == INSYDE_MAGIC_BOOT_INDEX) {
    Status = CommonSetVariable (
          L"BootNext",
          &gEfiGlobalVariableGuid,
          EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
          0,
          &BootNext
          );
    ASSERT_EFI_ERROR(Status);

    if (!EFI_ERROR(Status)) {
      Status = EfiCreateEventReadyToBootEx (
                    TPL_NOTIFY,
                    TriggerCapsuleUpdateLoader,
                    NULL,
                    &ReadyToBootEvent
                    );
    }
  } else {
     Status = EfiCreateEventReadyToBootEx (
                  TPL_NOTIFY,
                  TriggerFirmwareUpdate,
                  NULL,
                  &ReadyToBootEvent2
                  );
  }
  return Status;
}