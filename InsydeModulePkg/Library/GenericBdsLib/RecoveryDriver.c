/** @file
  The main funcitons of BDS platform recovery

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

#include "RecoveryDriver.h"

static EFI_GUID  mApplicationsFvGuid = { 0x607D62F9, 0x2615, 0x40ac, 0x98, 0x58, 0x68, 0xD3, 0xF7, 0x3A, 0x62, 0x67 };
static BOOLEAN   mAppFvLoaded  = FALSE;

static
EFI_STATUS
FindShell (
  IN     EFI_HANDLE       ParentImageHandle,
  IN     CHAR16           *Command
  );

static
EFI_STATUS
RunShell (
 IN     EFI_HANDLE                   ParentImageHandle,
 IN     EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv,
 IN     EFI_HANDLE                   FvHandle,
 IN     CHAR16                       *Command
 );

static
EFI_DEVICE_PATH_PROTOCOL *
FwVolFileDevicePath (
  IN     EFI_HANDLE       DeviceHandle  OPTIONAL,
  IN     EFI_GUID         *FileNameGuid
  );

static
EFI_STATUS
SetArgs (
  IN     EFI_HANDLE    ImageHandle,
  IN     CHAR16        *Command
  );


/**
  Locate Firmware volume in which shell resides

  @param  ParentImageHandle    The image handle.

  @retval EFI_SUCCESS            The command completed successfully
  @retval EFI_INVALID_PARAMETER  Command usage error
  @retval EFI_UNSUPPORTED        Protocols unsupported
  @retval EFI_OUT_OF_RESOURCES   Out of memory
  @retval Other                  Unknown error

**/
static
EFI_STATUS
FindShell (
  IN     EFI_HANDLE       ParentImageHandle,
  IN     CHAR16           *Command
  )
{
 EFI_STATUS              Status;
 UINTN                   HandleCount;
 EFI_HANDLE              *HandleBuffer;
 EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv;
 EFI_FV_FILETYPE           Type;
 UINTN                   Size;
 EFI_FV_FILE_ATTRIBUTES     Attributes;
 UINT32                 AuthenticationStatus;
 UINTN                   Index;

 Status = gBS->LocateHandleBuffer (
                 ByProtocol,
                 &gEfiFirmwareVolume2ProtocolGuid,
                 NULL,
                 &HandleCount,
                 &HandleBuffer
                 );
 if (EFI_ERROR (Status) || HandleCount == 0) {
  return EFI_NOT_FOUND;
 }

 //
 // Loop through all the Firmware Volumes looking for the
 // Guid Filename
 //
 for (Index = 0; Index < HandleCount; Index++) {
  Status = gBS->HandleProtocol (
                  HandleBuffer[Index],
                    &gEfiFirmwareVolume2ProtocolGuid,
                    (VOID **)&Fv
                    );
  if (EFI_ERROR (Status)) {
   gBS->FreePool (HandleBuffer);

   return Status;
  }

  Size = 0;
  Status = Fv->ReadFile (
                 Fv,
                 PcdGetPtr(PcdShellFile),
                 NULL,
                 &Size,
                 &Type,
                 &Attributes,
                 &AuthenticationStatus
                 );

  if (EFI_ERROR(Status)) {
   //
   // Skip if no specifie file in the FV
   //
   continue;
  }

  Status = RunShell (ParentImageHandle, Fv, HandleBuffer[Index], Command);

  break;
 }

 gBS->FreePool (HandleBuffer);

 return Status;
}


/**
  Load & Execute the shell

  @param  ParentImageHandle    The image handle.
  @param  Fv                   Firmware Volume in which shell resides
  @param  Size                 Size of the shell file

  @retval EFI_SUCCESS             The command completed successfully
  @retval EFI_INVALID_PARAMETER   Command usage error
  @retval EFI_UNSUPPORTED         Protocols unsupported
  @retval EFI_OUT_OF_RESOURCES    Out of memory
  @retval Other                   Unknown error

**/
static
EFI_STATUS
RunShell (
 IN     EFI_HANDLE                    ParentImageHandle,
 IN     EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv,
 IN     EFI_HANDLE                    FvHandle,
 IN     CHAR16                        *Command
 )
{
  EFI_STATUS            Status;
  EFI_HANDLE            ImageHandle;
  EFI_DEVICE_PATH_PROTOCOL  *FileDevicePath;

  FileDevicePath = FwVolFileDevicePath (FvHandle, PcdGetPtr(PcdShellFile));

  if (FileDevicePath == NULL) {
    return EFI_NOT_FOUND;
  }

  Status = gBS->LoadImage (
                  TRUE,
                  ParentImageHandle,
                  FileDevicePath,
                  NULL,
                  0,
                  &ImageHandle
                  );
  gBS->FreePool (FileDevicePath);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Drop the TPL level from EFI_TPL_DRIVER to EFI_TPL_APPLICATION
  //
  gBS->RestoreTPL (TPL_APPLICATION);

  Status = SetArgs (ImageHandle, Command);

  Status = gBS->StartImage (ImageHandle, NULL, NULL);

  gBS->RaiseTPL (TPL_DRIVER);

  return Status;
}


/**
  Create a device path that appends a MEDIA_FW_VOL_FILEPATH_DEVICE_PATH with
  FileNameGuid to the device path of DeviceHandle.

  @param  DeviceHandle         Optional Device Handle to use as Root of the Device Path
  @param  FileNameGuid         Guid used to create MEDIA_FW_VOL_FILEPATH_DEVICE_PATH

  @return EFI_DEVICE_PATH_PROTOCOL that was allocated from dynamic memory
  or NULL pointer.

**/
static
EFI_DEVICE_PATH_PROTOCOL *
FwVolFileDevicePath (
  IN     EFI_HANDLE       DeviceHandle  OPTIONAL,
  IN     EFI_GUID         *FileNameGuid
  )
{
  EFI_STATUS                          Status;
  EFI_DEVICE_PATH_PROTOCOL            *RootDevicePath;
  EFI_DEVICE_PATH_PROTOCOL            *DevicePath;
  MEDIA_FW_VOL_FILEPATH_DEVICE_PATH   Node;

  RootDevicePath = NULL;
  if (NULL != DeviceHandle) {
    Status = gBS->HandleProtocol (
                    DeviceHandle,
                    &gEfiDevicePathProtocolGuid,
                    (VOID **)&RootDevicePath
                    );
  }

#if 1
//#if (EFI_SPECIFICATION_VERSION < 0x00020000)
  //
  // Create a device path that appends a MEDIA_FW_VOL_FILEPATH_DEVICE_PATH with
  // FileNameGuid to the device path of DeviceHandle
  //
  Node.Header.Type = MEDIA_DEVICE_PATH;
  Node.Header.SubType = MEDIA_PIWG_FW_FILE_DP;

  SetDevicePathNodeLength (&Node.Header, sizeof (MEDIA_FW_VOL_FILEPATH_DEVICE_PATH));

#else
  //
  // Create a device path that appends a MEDIA_FW_VOL_FILEPATH_DEVICE_PATH with
  // FileNameGuid to the device path of DeviceHandle
  //
  Node.Piwg.Header.Type = MEDIA_DEVICE_PATH;
  Node.Piwg.Header.SubType = MEDIA_VENDOR_DP;

  SetDevicePathNodeLength (&Node.Piwg.Header, sizeof (MEDIA_FW_VOL_FILEPATH_DEVICE_PATH));

  //
  // Add the GUID for generic PIWG device paths
  //
  EfiCopyMem (&Node.Piwg.PiwgSpecificDevicePath, &gEfiFrameworkDevicePathGuid, sizeof(EFI_GUID));

  //
  // Add in the FW Vol File Path PIWG defined inforation
  //
  Node.Piwg.Type = PIWG_MEDIA_FW_VOL_FILEPATH_DEVICE_PATH_TYPE;

#endif

  CopyMem (&Node.FvFileName, FileNameGuid, sizeof(EFI_GUID));

  DevicePath = AppendDevicePathNode (RootDevicePath, (EFI_DEVICE_PATH_PROTOCOL *) &Node);
  return DevicePath;
}


/**
  pass a "canned" argument to the shell script, "DoRecovery.nsh"

  @param  ImageHandle   

  @retval EFI_SUCCESS   
  @retval Other         

**/
static
EFI_STATUS
SetArgs (
  IN     EFI_HANDLE    ImageHandle,
  IN     CHAR16        *Command
  )
{
  EFI_STATUS             Status;
  EFI_LOADED_IMAGE_PROTOCOL *Image;
  UINTN                  ArgsLen;
  CHAR16                *ArgsCopy;

  Status = gBS->HandleProtocol (
                  ImageHandle,
                  &gEfiLoadedImageProtocolGuid,
                  (VOID*) &Image
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ArgsLen = (StrLen (Command) + 1) * 2;

  if (ArgsLen >= 256) {
    ArgsCopy = AllocatePool (ArgsLen);
  } else {
    ArgsCopy = AllocatePool (256);
  }

  StrCpy (ArgsCopy, Command);

  if (Image->LoadOptions != NULL) {
    gBS->FreePool (Image->LoadOptions);
  }

  Image->LoadOptions = ArgsCopy;
  Image->LoadOptionsSize = (UINT32)ArgsLen;

  return EFI_SUCCESS;
}

