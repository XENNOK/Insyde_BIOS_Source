/** @file
 DXE IPMI Package library implement code - Enter Setup.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/


#include <Library/DxeIpmiPackageLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DevicePathLib.h>
#include <Protocol/FirmwareVolume2.h>

/**
 This package function can enter Setup Utility as IPMI boot option request.
 Platform implement this function since entering Setup Utility is a platform dependent function.

 @retval EFI_SUCCESS                    Entering Setup Utility success.
 @return EFI_ERROR (Status)             Locate gEfiSetupUtilityProtocolGuid Protocol error.
*/
EFI_STATUS
IpmiEnterSetup (
  VOID
  )
{
  EFI_STATUS                          Status;
  UINTN                               HandleCount;
  EFI_HANDLE                          *HandleBuffer;
  EFI_GUID                            FileNameGuid = {0x3935B0A1, 0xA182, 0x4887, {0xbc, 0x56, 0x67, 0x55, 0x28, 0xe7, 0x88, 0x77}};
  MEDIA_FW_VOL_FILEPATH_DEVICE_PATH   FvFilePath;
  UINTN                               Index;
  EFI_FIRMWARE_VOLUME2_PROTOCOL       *Fv;
  UINTN                               Size;
  EFI_FV_FILETYPE                     Type;
  EFI_FV_FILE_ATTRIBUTES              Attributes;
  UINT32                              AuthenticationStatus;
  EFI_DEVICE_PATH_PROTOCOL            *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL            *SetupAppDevicePath;
  EFI_HANDLE                          ImageHandle;
  UINTN                               ExitDataSize;
  CHAR16                              *ExitData;

  HandleCount  = 0;
  HandleBuffer = NULL;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (Status != EFI_SUCCESS || HandleBuffer == NULL) {
    return EFI_NOT_FOUND;
  }

  EfiInitializeFwVolDevicepathNode (&FvFilePath, &FileNameGuid);

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiFirmwareVolume2ProtocolGuid,
                    (VOID **) &Fv
                    );
    if (!EFI_ERROR (Status)) {
      Status = Fv->ReadFile (
                     Fv,
                     &FileNameGuid,
                     NULL,
                     &Size,
                     &Type,
                     &Attributes,
                     &AuthenticationStatus
                     );
    }
    if (EFI_ERROR (Status)) {
      continue;
    }

    //
    // Create device path of setup utility application
    //
    DevicePath = DevicePathFromHandle (HandleBuffer[Index]);
    if (DevicePath == NULL) {
      continue;
    }
    SetupAppDevicePath = AppendDevicePathNode (DevicePath, (EFI_DEVICE_PATH_PROTOCOL *) &FvFilePath);
    if (SetupAppDevicePath == NULL) {
      continue;
    }

    Status = gBS->LoadImage (
                    TRUE,
                    gImageHandle,
                    SetupAppDevicePath,
                    NULL,
                    0,
                    &ImageHandle
                    );
    gBS->FreePool (SetupAppDevicePath);
    if (EFI_ERROR (Status)) {
      continue;
    }

    gBS->StartImage (ImageHandle, &ExitDataSize, &ExitData);
    break;
  }

  gBS->FreePool (HandleBuffer);

  return Status;}

