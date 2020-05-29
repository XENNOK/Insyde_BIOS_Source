//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name:
//;
//;   RecoveryDriver.c
//;
//; Abstract:
//;
//;  The main funcitons of BDS platform recovery
//;

#include "RecoveryDriver.h"
//
// Consumes
//
#include EFI_PROTOCOL_CONSUMER (ConsoleControl)
#include EFI_PROTOCOL_CONSUMER (FirmwareVolume)
#include EFI_PROTOCOL_CONSUMER (LoadedImage)

#include EFI_GUID_DEFINITION (EfiShell)

static EFI_GUID mApplicationsFvGuid = { 0x607D62F9, 0x2615, 0x40ac, 0x98, 0x58, 0x68, 0xD3, 0xF7, 0x3A, 0x62, 0x67 };

static	BOOLEAN				       mAppFvLoaded		= FALSE;
extern	EFI_HANDLE           mBdsImageHandle;


static
EFI_STATUS
FindShell (
  IN     EFI_HANDLE       ParentImageHandle,
  IN     CHAR16           *Command
  );

static
EFI_STATUS
RunShell (
  IN 	   EFI_HANDLE                   ParentImageHandle,
	IN     EFI_FIRMWARE_VOLUME_PROTOCOL *Fv,
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

static
EFI_STATUS
FindShell (
  IN     EFI_HANDLE       ParentImageHandle,
  IN     CHAR16           *Command
  )
/*++

Routine Description:
  Locate Firmware volume in which shell resides

Arguments:
  ParentImageHandle     The image handle.

Returns:
  EFI_SUCCESS             - The command completed successfully
  EFI_INVALID_PARAMETER   - Command usage error
  EFI_UNSUPPORTED         - Protocols unsupported
  EFI_OUT_OF_RESOURCES    - Out of memory
  Other value             - Unknown error

--*/
{
	EFI_STATUS						        Status;
	UINTN							            HandleCount;
	EFI_HANDLE						        *HandleBuffer;
	EFI_FIRMWARE_VOLUME_PROTOCOL	*Fv;
	EFI_FV_FILETYPE					      Type;
	UINTN							            Size;
	EFI_FV_FILE_ATTRIBUTES			  Attributes;
	UINT32							          AuthenticationStatus;
	UINTN							            Index;

	Status = gBS->LocateHandleBuffer (
	                ByProtocol,
                  &gEfiFirmwareVolumeProtocolGuid,
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
                    &gEfiFirmwareVolumeProtocolGuid,
                    (VOID **)&Fv
                    );

		if (EFI_ERROR (Status)) {
			gBS->FreePool (HandleBuffer);

			return Status;
		}

		Size = 0;
		Status = Fv->ReadFile (
		               Fv,
                   &gEfiShellFileGuid,
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

static
EFI_STATUS
RunShell (
  IN 	   EFI_HANDLE                   ParentImageHandle,
	IN     EFI_FIRMWARE_VOLUME_PROTOCOL *Fv,
	IN     EFI_HANDLE                   FvHandle,
	IN     CHAR16                       *Command
	)
/*++

Routine Description:
  Load & Execute the shell

Arguments:
  ParentImageHandle - The image handle.
  Fv					      - Firmware Volume in which shell resides
  Size					    - Size of the shell file

Returns:
  EFI_SUCCESS             - The command completed successfully
  EFI_INVALID_PARAMETER   - Command usage error
  EFI_UNSUPPORTED         - Protocols unsupported
  EFI_OUT_OF_RESOURCES    - Out of memory
  Other value             - Unknown error

--*/
{
	EFI_STATUS						      Status;
	EFI_HANDLE						      ImageHandle;
	EFI_DEVICE_PATH_PROTOCOL		*FileDevicePath;

	FileDevicePath = FwVolFileDevicePath (FvHandle, &gEfiShellFileGuid);

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
	gBS->RestoreTPL (EFI_TPL_APPLICATION);

	Status = SetArgs (ImageHandle, Command);

	Status = gBS->StartImage (ImageHandle, NULL, NULL);

	gBS->RaiseTPL (EFI_TPL_DRIVER);

	return Status;
}


static
EFI_DEVICE_PATH_PROTOCOL *
FwVolFileDevicePath (
  IN     EFI_HANDLE       DeviceHandle  OPTIONAL,
  IN     EFI_GUID         *FileNameGuid
  )
/*++

Routine Description:
  Create a device path that appends a MEDIA_FW_VOL_FILEPATH_DEVICE_PATH with
  FileNameGuid to the device path of DeviceHandle.

Arguments:
  DeviceHandle - Optional Device Handle to use as Root of the Device Path

  FileNameGuid - Guid used to create MEDIA_FW_VOL_FILEPATH_DEVICE_PATH

Returns:
  EFI_DEVICE_PATH_PROTOCOL that was allocated from dynamic memory
  or NULL pointer.

--*/
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
  Node.Header.SubType = MEDIA_FV_FILEPATH_DP;

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

  EfiCopyMem (&Node.NameGuid, FileNameGuid, sizeof(EFI_GUID));

  DevicePath = EfiAppendDevicePathNode (RootDevicePath, (EFI_DEVICE_PATH_PROTOCOL *) &Node);
  return DevicePath;
}

static
EFI_STATUS
SetArgs (
  IN     EFI_HANDLE    ImageHandle,
  IN     CHAR16        *Command
  )
/*++

Routine Description:
  pass a "canned" argument to the shell script, "DoRecovery.nsh"

Arguments:
  ImageHandle

Returns:
  Status

--*/
{
	EFI_STATUS			          Status;
	EFI_LOADED_IMAGE_PROTOCOL	*Image;
	UINTN				              ArgsLen;
	CHAR16				            *ArgsCopy;

	Status = gBS->HandleProtocol (
	                ImageHandle,
                  &gEfiLoadedImageProtocolGuid,
                  (VOID*) &Image
                  );

	if (EFI_ERROR (Status)) {
		return Status;
  }

	ArgsLen = (EfiStrLen (Command) + 1) * 2;

	if (ArgsLen >= 256) {
		ArgsCopy = EfiLibAllocatePool (ArgsLen);
	} else {
		ArgsCopy = EfiLibAllocatePool (256);
  }

	EfiStrCpy (ArgsCopy, Command);

	if (Image->LoadOptions != NULL) {
		gBS->FreePool (Image->LoadOptions);
	}

	Image->LoadOptions = ArgsCopy;
	Image->LoadOptionsSize = (UINT32)ArgsLen;

	return EFI_SUCCESS;
}

