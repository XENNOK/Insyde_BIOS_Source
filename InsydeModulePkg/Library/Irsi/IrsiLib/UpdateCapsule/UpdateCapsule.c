/** @file
  Library Instance implementation for IRSI Update Capsule Function

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
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/IrsiLib.h>
#include <Library/PcdLib.h>
#include <Library/PrintLib.h>
#include <Protocol/DevicePath.h>
#include <Library/DevicePathLib.h>
#include <Guid/GlobalVariable.h>
#include <Library/DebugLib.h>

#define INSYDE_CAPSULE_UPDATE_LOADER_DESC     L"Insyde Capsule Update Loader"
#define DEFAULT_CAPSULE_UPDATE_LOADER_PATH    L"EFI\\Insyde\\IsCapLoader.efi"
#define EFI_PATH_SIGNATURE                    L"EFI\\"
#define INSYDE_MAGIC_BOOT_INDEX               0x1688
#define MAX_BOOT_OPTION_DATA_SIZE             2048

/**
  Get the offset of the boot loader file path from system partition for the boot device path
  of the current boot option

  @param[in] BootDevicePath      Unicode string of the loader path from the system partition
  @param[in] BootDevicePathSize  Size of the BootPath string
  @param[out] BootPathOffset

  @retval EFI_SUCCESS            Next boot option successfully created
  @return others                 Unable to set next boot option

**/
EFI_STATUS
EFIAPI
GetFileDevicePathOffset (
  IN EFI_DEVICE_PATH_PROTOCOL     *BootDevicePath,
  OUT UINTN                       *FileDevicePathOffset
  )
{
  EFI_DEVICE_PATH_PROTOCOL   *DevicePath;
  
  if (BootDevicePath == NULL || FileDevicePathOffset == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  DevicePath = (EFI_DEVICE_PATH_PROTOCOL *)BootDevicePath;
  while (!IsDevicePathEnd(DevicePath)) {
    if ((DevicePathType (DevicePath) == MEDIA_DEVICE_PATH) &&
      (DevicePathSubType (DevicePath) == MEDIA_FILEPATH_DP)) {
      *FileDevicePathOffset = (UINTN)((UINT8 *)DevicePath - (UINT8 *)BootDevicePath);
      break;
    }
    DevicePath = NextDevicePathNode (DevicePath);
    if (DevicePath == NULL) {
      return EFI_NOT_FOUND;
    }
  }
  *FileDevicePathOffset = (UINTN)((UINT8 *)DevicePath - (UINT8 *)BootDevicePath);

  return EFI_SUCCESS;
}



/**
  Set capsule update loader as the next boot option

  @param BootPath               Unicode string of the loader path from the system partition
  @param BootPathSize           Size of the BootPath string

  @retval EFI_SUCCESS           Next boot option successfully created
  @return others                Unable to set next boot option
  
**/
EFI_STATUS
EFIAPI
SetNextBootOption (
  CHAR16 *InputBootPath,
  UINT32 BootPathSize
  )
{
  EFI_STATUS       Status;
  UINT16           BootIndex;
  CHAR16           BootOption[] = L"Boot0000";
  UINT8            BootCurrentData[MAX_BOOT_OPTION_DATA_SIZE];
  UINT8            BootNextData[MAX_BOOT_OPTION_DATA_SIZE];
  UINT8            *BootDevicePathBuf;
  UINT16           BootDevicePathSize;
  CHAR16           *BootOptionDesc;
  UINTN            FileDevicePathOffset;
  UINT8            *BufPtr;
  UINTN            Size;
  CHAR16           BootPath[256];
  
  if (BootPathSize == 0 || InputBootPath == NULL) {
    //
    // Default Capsule Update boot loader is used
    //
    BootPathSize = sizeof (DEFAULT_CAPSULE_UPDATE_LOADER_PATH);
    CopyMem (BootPath, DEFAULT_CAPSULE_UPDATE_LOADER_PATH, BootPathSize);
  } else {
    BootPathSize = (UINT16)StrSize (InputBootPath);
    CopyMem (BootPath, InputBootPath, BootPathSize);
  }

  Size = sizeof(UINT16);
  Status = EfiGetVariable (
             L"BootCurrent",
             &gEfiGlobalVariableGuid,
             NULL,
             &Size,
             &BootIndex
             );

  if (EFI_ERROR(Status)) {
    return Status;
  }
  UnicodeSPrint (BootOption, sizeof(BootOption), L"Boot%04x", (UINTN)BootIndex);
  
  Size = MAX_BOOT_OPTION_DATA_SIZE;
  Status = EfiGetVariable (
             BootOption,
             &gEfiGlobalVariableGuid,
             NULL,
             &Size,
             BootCurrentData
             );
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  BootOptionDesc = (CHAR16 *)(BootCurrentData + sizeof(UINT32) + sizeof(UINT16));
  BootDevicePathBuf = BootCurrentData + sizeof(UINT32) + sizeof(UINT16) + StrSize(BootOptionDesc);
  BootDevicePathSize = *(UINT16 *)(BootCurrentData + sizeof(UINT32));
  
  Status = GetFileDevicePathOffset (
             (EFI_DEVICE_PATH_PROTOCOL *)BootDevicePathBuf,
             &FileDevicePathOffset);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  if (Status == EFI_SUCCESS) {
    //
    // Boot Option Attributes
    //
    BufPtr = BootNextData;
    *(UINT32 *)BufPtr = LOAD_OPTION_ACTIVE;
    BufPtr += sizeof(UINT32);
    
    //
    // Boot DevicePath size
    //
    BootDevicePathSize = (UINT16)(FileDevicePathOffset + sizeof(EFI_DEVICE_PATH_PROTOCOL) + BootPathSize + END_DEVICE_PATH_LENGTH);
    *(UINT16 *)BufPtr = BootDevicePathSize;
    BufPtr += sizeof(UINT16);
    
    //
    // Boot Option Description
    //
    CopyMem (BufPtr, INSYDE_CAPSULE_UPDATE_LOADER_DESC, sizeof(INSYDE_CAPSULE_UPDATE_LOADER_DESC));
    BufPtr += sizeof(INSYDE_CAPSULE_UPDATE_LOADER_DESC);

    //
    // Boot Device Path
    //
    CopyMem (BufPtr, BootDevicePathBuf, FileDevicePathOffset);
    BufPtr += FileDevicePathOffset;
    //
    // File Device Path Protocol
    //
    *BufPtr++ = MEDIA_DEVICE_PATH;
    *BufPtr++ = MEDIA_FILEPATH_DP;
    //
    //  File Device Path Size
    //
    *(UINT16 *)BufPtr = (UINT16)(sizeof(EFI_DEVICE_PATH_PROTOCOL) + BootPathSize);
    BufPtr += 2;
    //
    // File Path String
    //
    CopyMem (BufPtr, BootPath, BootPathSize);
    BufPtr += BootPathSize;
    //
    // End of Device Path
    //
    *BufPtr++ = 0x7F;
    *BufPtr++ = 0xFF;
    *BufPtr++ = 0x04;
    *BufPtr   = 0x00;

    Size = sizeof(UINT32) +
           sizeof(UINT16) +
           sizeof (INSYDE_CAPSULE_UPDATE_LOADER_DESC) +
           BootDevicePathSize;
           
    UnicodeSPrint (BootOption, sizeof(BootOption), L"Boot%04x", INSYDE_MAGIC_BOOT_INDEX);
    Status = EfiSetVariable (
               BootOption,
               &gEfiGlobalVariableGuid,
               EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
               Size,
               BootNextData
               );
    ASSERT_EFI_ERROR(Status);
    if (Status == EFI_SUCCESS) {
      BootIndex = INSYDE_MAGIC_BOOT_INDEX;
      Status = EfiSetVariable (
                 L"BootNext",
                 &gEfiGlobalVariableGuid,
                 EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                 sizeof(UINT16),
                 &BootIndex
                 );
      ASSERT_EFI_ERROR(Status);
    }
  }
  return Status;

}

/**
  Irsi Update Capsule function


  @param UpdateCapsule          pointer to IRSI_UPDATE_CAPSULE structure

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
IrsiUpdateCapsule (
  VOID     *UpdateCapsuleBuf
  )
{
  EFI_STATUS            Status;
  IRSI_UPDATE_CAPSULE   *UpdateCapsule;

  UpdateCapsule = (IRSI_UPDATE_CAPSULE *)UpdateCapsuleBuf;
  Status = SetNextBootOption((CHAR16 *)(UINTN)UpdateCapsule->BootPath, UpdateCapsule->BootPathSize);

  return Status;
}

/**
  Irsi BootNext Update Initialization

  @param ImageHandle     A handle for the image that is initializing this driver
  @param SystemTable     A pointer to the EFI system table

  @retval EFI_SUCCESS:   Module initialized successfully
  @retval Others     :   Module initialization failed

**/
EFI_STATUS
EFIAPI
IrsiUpdateCapsuleInit (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                     Status;

  Status = IrsiRegisterFunction (
               &gIrsiServicesGuid,
               IRSI_UPDATE_CAPSULE_COMMAND,
               IrsiUpdateCapsule
               );
               
  return Status;
}