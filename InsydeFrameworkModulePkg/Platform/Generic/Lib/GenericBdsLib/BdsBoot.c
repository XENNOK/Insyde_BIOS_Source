//;******************************************************************************
//;* Copyright (c) 1983-2014, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Copyright (c) 2004 - 2006, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  BdsBoot.c

Abstract:

  BDS Lib functions which relate with create or process the boot
  option.

--*/

#include "GenericBdsLib.h"
#include "OemGraphicsLib.h"
#include "SetupConfig.h"
#include "PostCode.h"
#include "CmosLib.h"
#include "EfiImage.h"
#include "GenericBdsLibStrDefs.h"
#include "MemoryCheck.h"
#include EFI_PROTOCOL_CONSUMER (SkipScanRemovableDev)
#include EFI_PROTOCOL_DEFINITION (EdidDiscovered)
#include EFI_PROTOCOL_DEFINITION (BootLogo)
#include "VgaDriverPolicy.h"
#include "OemGraphicsLib.h"
#include EFI_PROTOCOL_DEFINITION (OemServices)
#include "OemLogoResolution.h"
#include EFI_PROTOCOL_DEFINITION (TreeMeasureOsLoaderAuthority)
#include EFI_GUID_DEFINITION     (Gpt)
#include EFI_GUID_DEFINITION     (ReturnFromImage)
#include EFI_PROTOCOL_DEFINITION (SimpleTextInputEx)

BOOLEAN                 mEnumBootDevice = FALSE;
EFI_LIST_ENTRY          mWindowsToGoDeviceList;
//
//BDS General Uefi Boot OS table,
//that only support non-removable Media for UEFI OS
//  Windows(Win7,Win2008),
//  Redhat Linux,
//  SuSE Linux,
//  Ubuntu,
//  Android,
// Others UEFI OS could add in this table to be supported.
//
#define SECURE_BOOT_ENABLED       0x00
#define SECURE_BOOT_DISABLED      0x01
#define MAX_BIOS_ENVIRONMENT      0x02
typedef BDS_GENERAL_UEFI_BOOT_OS  BDS_GENERAL_UEFI_BOOT_ARRAY_OS[MAX_BIOS_ENVIRONMENT];
//
// {{SECURE_BOOT_ENABLED BDS_GENERAL_UEFI_BOOT_OS}, {SECURE_BOOT_DISABLED BDS_GENERAL_UEFI_BOOT_OS}}
// ...
//
BDS_GENERAL_UEFI_BOOT_ARRAY_OS mBdsGeneralUefiBootOs[] = {
  {{L"\\EFI\\Microsoft\\Boot\\bootmgfw.efi",          L"Windows Boot Manager"}, {L"\\EFI\\Microsoft\\Boot\\bootmgfw.efi", L"Windows Boot Manager"}},
  {{L"\\EFI\\redhat\\grub.efi",                       L"Red Hat Linux"},        {L"\\EFI\\redhat\\grub.efi",              L"Red Hat Linux"}},
  {{L"\\EFI\\SuSE\\elilo.efi",                        L"SuSE Linux"},           {L"\\EFI\\SuSE\\elilo.efi",               L"SuSE Linux"}},
  {{UBUNTU_BOOT_SECURE_BOOT_WITH_PROCESSOR_FILE_NAME, L"ubuntu"},               {UBUNTU_BOOT_WITH_PROCESSOR_FILE_NAME,    L"ubuntu"}},
  {{L"\\EFI\\ubuntu\\shim.efi",                       L"ubuntu"},               {L"\\EFI\\ubuntu\\grub.efi",              L"ubuntu"}},
  {{L"\\EFI\\fedora\\shim.efi",                       L"Fedora"},               {L"\\EFI\\fedora\\shim.efi",              L"Fedora"}},
  {{L"\\EFI\\android\\bootx64.efi",                   L"Android"},              {L"\\EFI\\android\\bootx64.efi",          L"Android"}},
  {{L"\\EFI\\Boot\\grubx64.efi",                      L"Linpus lite"},         {L"\\EFI\\Boot\\grubx64.efi",             L"Linpus lite"}},
  };

DUMMY_BOOT_OPTION_INFO  mDummyBootOptionTable[] = {
  {DummyUsbBootOptionNum,     L"EFI USB Device"},
  {DummyCDBootOptionNum,      L"EFI DVD/CDROM"},
  {DummyNetwokrBootOptionNum, L"EFI Network"}
  };


#define SHELL_ENVIRONMENT_INTERFACE_PROTOCOL \
  { 0x47c7b221, 0xc42a, 0x11d2, 0x8e, 0x57, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b }

EFI_STATUS
CreateDummyBootOptions (
  VOID
  )
/*++

Routine Description:

  Create dummy boot options which use to selected by UEFI OS.

Arguments:

  None.

Returns:

  EFI_SUCCESS  - Create dummy boot options successful.
  Other        - Any error occured while creating dummy boot options.

--*/
;

EFI_STATUS
SyncBootOrder (
  VOID
  )
/*++

Routine Description:

  Synchronize the contents of "BootOrder" to prevent from the contents is
  updated by SCU or UEFI OS.

Arguments:

  None.

Returns:

  EFI_SUCCESS  - Synchronize data successful.
  Other        - Set "BootOrder" varible failed.

--*/
;

EFI_DEVICE_PATH_PROTOCOL *
ExpandPartitionPartialDevicePathToFull (
  IN  HARDDRIVE_DEVICE_PATH      *HardDriveDevicePath
  );

VOID
SetNativeResolution (
  IN EFI_HANDLE                  *PrimaryVgaHandle
);

EFI_STATUS
CheckModeSupported (
  EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput,
  IN  UINT32                    HorizontalResolution,
  IN  UINT32                    VerticalResolution,
  OUT UINT32                    *SupportedModeNumber
  );

EFI_STATUS
BgrtUpdateImageInfo (
  IN  EFI_HANDLE                        *PrimaryVgaHandle,
  OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL     **BltBuffer,
  OUT UINTN                             *DestinationX,
  OUT UINTN                             *DestinationY,
  OUT UINTN                             *Width,
  OUT UINTN                             *Height
  );

EFI_STATUS
BgrtGetImageByBadging (
  IN OUT  UINT8                          **ImageData,
  IN OUT  UINTN                          *ImageSize,
  IN OUT  EFI_BADGING_SUPPORT_FORMAT     *ImageFormat
  );

VOID
BgrtGetLogoCoordinate (
  IN   UINTN      ResolutionX,
  IN   UINTN      ResolutionY,
  IN   UINTN      ImageWidth,
  IN   UINTN      ImageHeight,
  OUT  UINTN      *CoordinateX,
  OUT  UINTN      *CoordinateY
  );

BOOLEAN
IsSinglePhysicalGop (
  EFI_HANDLE  *SinglePhyGop
  );

VOID
EnableOptimalTextMode (
  VOID
  );

BOOLEAN
FoundTextBasedConsole (
  VOID
  );

VOID *
GetConOutVar (
  IN CHAR16 * Name
  );

VOID
GetComboVideoOptimalResolution (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput,
  OUT UINT32                        *XResolution,
  OUT UINT32                        *YResoulution
  );

VOID
SignalImageReturns (
  VOID
  )
/*++

Routine Description:

  Signal BIOS after the image returns

Arguments:

  None.

Returns:

  None.

--*/

{
  EFI_HANDLE                        SignalHandle;
  EFI_HANDLE                        *HandleBuffer;
  UINTN                             NumberOfHandles;
  EFI_STATUS                        Status;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiReturnFromImageGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    SignalHandle = NULL;
    Status = gBS->InstallProtocolInterface (
                    &SignalHandle,
                    &gEfiReturnFromImageGuid,
                    EFI_NATIVE_INTERFACE,
                    NULL
                    );
  } else {
    Status = gBS->ReinstallProtocolInterface (
                    HandleBuffer[0],
                    &gEfiReturnFromImageGuid,
                    NULL,
                    NULL
                    );
    gBS->FreePool (HandleBuffer);
  }
}

EFI_STATUS
SearchAcpiTable (
  EFI_ACPI_SUPPORT_PROTOCOL                 *AcpiSupport,
  UINT32                                    Signature,
  OUT EFI_ACPI_DESCRIPTION_HEADER           **Table,
  OUT EFI_ACPI_TABLE_VERSION                *Version,
  OUT UINTN                                 *TableHandle
  )
{
  UINTN                                     Index;
  UINTN                                     Handle;
  EFI_ACPI_DESCRIPTION_HEADER               *TempTable;
  EFI_ACPI_TABLE_VERSION                    TempVersion;
  EFI_STATUS                                Status;
  BOOLEAN                                   FindTable;

  Index  = 0;
  Handle = 0;
  FindTable = FALSE;

  do {
    TempTable  = NULL;
    Status = AcpiSupport->GetAcpiTable (
                            AcpiSupport,
                            Index,
                            &TempTable,
                            &TempVersion,
                            &Handle
                            );
    if (EFI_ERROR (Status)) {
      break;
    }

    if (TempTable->Signature == Signature) {
      FindTable = TRUE;
      break;
    }
    gBS->FreePool (TempTable);
    Index++;
  } while (TRUE);

  if (FindTable) {
    *Table = TempTable;
    *Version = TempVersion;
    *TableHandle = Handle;
    return EFI_SUCCESS;
  } else {
    *Table = NULL;
    *Version = 0;
    *TableHandle = 0;
    return EFI_NOT_FOUND;
  }
}

EFI_STATUS
GetFirmwareBasicBootRecord (
  OUT FPDT_FIRMWARE_BASIC_BOOT_PERFORMANCE_DATA_RECORD **Record
  )
{
  EFI_STATUS                                           Status;
  EFI_ACPI_SUPPORT_PROTOCOL                            *AcpiSupport;
  EFI_ACPI_DESCRIPTION_HEADER                          *Table;
  EFI_ACPI_TABLE_VERSION                               AcpiVersion;
  UINTN                                                TableHandle;
  EFI_ACPI_FIRMWARE_PERFORMANCE_DATA_TABLE             *FPDTTable;
  FPDT_FBPT_PERFORMANCE_TABLE_HEADER                   *FBPTRegion;
  UINT8                                                *PtrToRecord;
  FPDT_PERFORMANCE_RECORD_FORMAT                       *TempRecord;
  UINTN                                                TableLength;

  Status = gBS->LocateProtocol (&gEfiAcpiSupportGuid, NULL, &AcpiSupport);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Find FPDT
  //
  Status = SearchAcpiTable (AcpiSupport, EFI_ACPI_FPDT_TABLE_SIGNATURE, &Table, &AcpiVersion, &TableHandle);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  FPDTTable = (EFI_ACPI_FIRMWARE_PERFORMANCE_DATA_TABLE*)Table;

  FBPTRegion = (FPDT_FBPT_PERFORMANCE_TABLE_HEADER*)(UINTN)FPDTTable->RequiredPerfRecord.FBDTPointerRecord.FBPTPointer;

  gBS->FreePool (FPDTTable);
  //
  // Check signature "FBPT"
  //
  if (FBPTRegion->Signature != EFI_ACPI_FBPT_TABLE_SIGNATURE || FBPTRegion->Length == 0) {
    return EFI_NOT_FOUND;
  }

  PtrToRecord = (UINT8*) FBPTRegion;
  for (TableLength = sizeof(FPDT_FBPT_PERFORMANCE_TABLE_HEADER); TableLength < FBPTRegion->Length;) {
    TempRecord = (FPDT_PERFORMANCE_RECORD_FORMAT*) (PtrToRecord + TableLength);
    if (TempRecord->PerfRecordType == FIRMWARE_BASIC_BOOT_PERFORMANCE_DATA_RECORD_TYPE) {
      *Record = (FPDT_FIRMWARE_BASIC_BOOT_PERFORMANCE_DATA_RECORD*)TempRecord;
      return EFI_SUCCESS;
    }
    TableLength = TableLength + TempRecord->RecordLength;
  }

  *Record = NULL;
  return EFI_NOT_FOUND;
}

UINT64
GetNanoTime (
  VOID
  )
{
  EFI_CPU_ARCH_PROTOCOL   *CpuArch;
  UINT64                  Tick;
  UINT64                  TimePeriod;

  gBS->LocateProtocol (
         &gEfiCpuArchProtocolGuid,
         NULL,
         &CpuArch
         );

  CpuArch->GetTimerValue (CpuArch, 0, &Tick, &TimePeriod);

  return DivU64x32(MultU64x32(Tick , (UINTN)TimePeriod), 1000000, NULL);
}

VOID
UpdateFirmwareBasicLoadImageStart (
  VOID
  )
{
  EFI_STATUS                                         Status;
  UINT64                                             Time;
  FPDT_FIRMWARE_BASIC_BOOT_PERFORMANCE_DATA_RECORD   *FirmwareBasicBootRecord = NULL;

  Time = GetNanoTime ();

  Status = GetFirmwareBasicBootRecord (&FirmwareBasicBootRecord);
  if (EFI_ERROR(Status)) {
    return;
  }

  FirmwareBasicBootRecord->OSLoaderLoadImageStart = Time;
}

VOID
UpdateFirmwareBasicStartImageStart (
  VOID
  )
{
  EFI_STATUS                                         Status;
  UINT64                                             Time;
  FPDT_FIRMWARE_BASIC_BOOT_PERFORMANCE_DATA_RECORD   *FirmwareBasicBootRecord = NULL;

  Time = GetNanoTime ();

  Status = GetFirmwareBasicBootRecord (&FirmwareBasicBootRecord);
  if (EFI_ERROR(Status)) {
    return;
  }

  FirmwareBasicBootRecord->OSLoaderStartImageStart = Time;
}

BOOLEAN
IsWinBootManager (
  IN  EFI_DEVICE_PATH_PROTOCOL *DevicePath
  )
/*++

Routine Description:

  Check the Device path is Win Boot Manager.

Arguments:

  EFI_DEVICE_PATH_PROTOCOL  *DevicePath.

Returns:

  The function returns TRUE if the device path is Win Boot Manager.
  Otherwise, FALSE is returned.

--*/
{
  EFI_DEVICE_PATH_PROTOCOL *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL *FilePath;
  UINTN                    DevPathSize1;
  UINTN                    DevPathSize2;
  EFI_HANDLE               Handle;
  EFI_STATUS               Status;
  INTN                     Result;
  CHAR16                   *BootLoaderPathString = L"\\EFI\\Microsoft\\Boot\\bootmgfw.efi";

  TempDevicePath = DevicePath;

  Status = gBS->LocateDevicePath (
                  &gEfiSimpleFileSystemProtocolGuid,
                  &TempDevicePath,
                  &Handle
                  );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  FilePath = EfiFileDevicePath (Handle, BootLoaderPathString);

  ASSERT (FilePath);
  ASSERT (DevicePath);

  DevPathSize1  = EfiDevicePathSize (FilePath);
  DevPathSize2  = EfiDevicePathSize (DevicePath);

  if (DevPathSize1 != DevPathSize2) {
    return FALSE;
  } else {
    Result = EfiCompareMem (FilePath, DevicePath, DevPathSize1);
    if (Result == 0) {
      return TRUE;
    } else {
      return FALSE;
    }
  }
}

EFI_STATUS
BdsLibDoLegacyBoot (
  IN  BDS_COMMON_OPTION           *Option
  )
/*++

Routine Description:

  Boot the legacy system with the boot option

Arguments:

  Option           - The legacy boot option which have BBS device path

Returns:

  EFI_UNSUPPORTED  - There is no legacybios protocol, do not support legacy boot.
  EFI_STATUS       - Return the status of LegacyBios->LegacyBoot ().

--*/
{
  EFI_STATUS                Status;
  EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios;

  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, &LegacyBios);
  if (EFI_ERROR (Status)) {
    //
    // If no LegacyBios protocol we do not support legacy boot
    //
    return EFI_UNSUPPORTED;
  }
  //
  // Notes: if we seperate the int 19, then we don't need to refresh BBS
  //
  //
  // to set BBS Table priority
  //
  SetBbsPriority (LegacyBios, Option);
  //
  // Write boot to OS performance data to a file
  //
  WRITE_BOOT_TO_OS_PERFORMANCE_DATA;
  DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Legacy Boot: %S\n", Option->Description));

  return LegacyBios->LegacyBoot (
                      LegacyBios,
                      (BBS_BBS_DEVICE_PATH *) Option->DevicePath,
                      Option->LoadOptionsSize,
                      Option->LoadOptions
                      );
}

UINT8
BdsLibGetMessagingDevicePathSubType (
  IN  BDS_COMMON_OPTION          * Option,
  IN  HARDDRIVE_DEVICE_PATH      * HardDriveDevicePath
  )
/*++

Routine Description:

Arguments:

  Option              - The current processing boot option.
  HardDriveDevicePath - EFI Device Path to boot, if it starts with a hard drive device path.

Returns:

--*/
{
  EFI_STATUS                Status;
  UINTN                     BlockIoHandleCount;
  EFI_HANDLE                *BlockIoBuffer;
  EFI_DEVICE_PATH_PROTOCOL  *BlockIoDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *NewDevicePath;
  UINTN                     Index;

  if (!((DevicePathType (&HardDriveDevicePath->Header) == MEDIA_DEVICE_PATH) &&
        (DevicePathSubType (&HardDriveDevicePath->Header) == MEDIA_HARDDRIVE_DP))) {
    //
    // If the HardDriveDevicePath does not start with a Hard Drive Device Path
    // exit.
    //
    return 0;
  }
  //
  // The boot device have already been connected
  //
  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiBlockIoProtocolGuid, NULL, &BlockIoHandleCount, &BlockIoBuffer);
  if (EFI_ERROR (Status) || BlockIoHandleCount == 0) {
    //
    // If there was an error or there are no device handles that support
    // the BLOCK_IO Protocol, then return.
    //
    return 0;
  }
  //
  // Loop through all the device handles that support the BLOCK_IO Protocol
  //
  for (Index = 0; Index < BlockIoHandleCount; Index++) {

    Status = gBS->HandleProtocol (BlockIoBuffer[Index], &gEfiDevicePathProtocolGuid, (VOID *) &BlockIoDevicePath);
    if (EFI_ERROR (Status) || BlockIoDevicePath == NULL) {
      continue;
    }
    //
    // Only do the boot, when devicepath match
    //
    if (MatchPartitionDevicePathNode (BlockIoDevicePath, HardDriveDevicePath)) {
      //
      // Combine the Block IO and Hard Drive Device path together and try
      // to boot from it.
      //
      DevicePath    = NextDevicePathNode ((EFI_DEVICE_PATH_PROTOCOL *) HardDriveDevicePath);
      NewDevicePath = EfiAppendDevicePath (BlockIoDevicePath, DevicePath);

      while (!IsDevicePathEnd (NewDevicePath)) {
        if (DevicePathType (NewDevicePath) == MESSAGING_DEVICE_PATH) {
          return (UINT8) (DevicePathSubType (NewDevicePath));
        }

        NewDevicePath = NextDevicePathNode (NewDevicePath);
      }
    }
  }

  gBS->FreePool (BlockIoBuffer);
  return 0;
}

BOOLEAN
IsUefiOsFilePath (
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
/*++

Routine Description:

  Check the Device path is a file path pointer to UEFI OS boot option.

Arguments:

  EFI_DEVICE_PATH_PROTOCOL  *DevicePath.

Returns:
  TRUE  - The device path is pointer to  UEFI OS boot opiton.
  FALSE - The device path isn't pointer to  UEFI OS boot opiton.

--*/
{
  EFI_DEVICE_PATH_PROTOCOL  *WorkingDevicePath;
  EFI_HANDLE                Handle;
  EFI_STATUS                Status;
  BOOLEAN                   IsUefiOsFilePath;
  FILEPATH_DEVICE_PATH      *FilePath;

  IsUefiOsFilePath  = FALSE;
  WorkingDevicePath = DevicePath;

  Status = gBS->LocateDevicePath (
                  &gEfiSimpleFileSystemProtocolGuid,
                  &WorkingDevicePath,
                  &Handle
                  );
  if (!EFI_ERROR (Status) && DevicePathType (WorkingDevicePath) == MEDIA_DEVICE_PATH &&
      DevicePathSubType (WorkingDevicePath) == MEDIA_FILEPATH_DP) {
    //
    //  If file name isn't default removable file name, we consider this file path pointer to UEFI OS
    //
    FilePath = (FILEPATH_DEVICE_PATH *) WorkingDevicePath;
    if ((EfiStrLen (FilePath->PathName) != EfiStrLen (DEFAULT_REMOVABLE_FILE_NAME)) ||
        (EfiStrCmp (FilePath->PathName, DEFAULT_REMOVABLE_FILE_NAME) != 0)) {
      IsUefiOsFilePath = TRUE;
    }
  }

  return IsUefiOsFilePath;
}

BOOLEAN
IsPxeBoot (
  EFI_DEVICE_PATH_PROTOCOL     *DevicePath
  )
{
  BOOLEAN                      IsPxeImage;
  EFI_DEVICE_PATH_PROTOCOL     *NetworkDevicePath;

  IsPxeImage = FALSE;
  NetworkDevicePath = DevicePath;
  while (!EfiIsDevicePathEnd (NetworkDevicePath)) {
    if (NetworkDevicePath->Type == MESSAGING_DEVICE_PATH) {
      if (NetworkDevicePath->SubType == MSG_MAC_ADDR_DP) {
        IsPxeImage = TRUE;
        break;
      }
    }
    NetworkDevicePath = EfiNextDevicePathNode (NetworkDevicePath);
  }
  return IsPxeImage;
}

BOOLEAN
IsNoLegacyDependency (
  IN  EFI_LOADED_IMAGE_PROTOCOL *ImageInfo
  )
{
  EFI_IMAGE_DOS_HEADER                  *DosHdr;
  EFI_IMAGE_NT_HEADERS32                *Pe32 = NULL;
  EFI_IMAGE_NT_HEADERS64                *Pe64 = NULL;

  if (ImageInfo == NULL) {
    return FALSE;
  }

  DosHdr = (EFI_IMAGE_DOS_HEADER *) ImageInfo->ImageBase;
  if (DosHdr->e_magic == EFI_IMAGE_DOS_SIGNATURE) {
    //
    // Valid DOS header so get address of PE header
    //
    Pe32 = (EFI_IMAGE_NT_HEADERS32 *)(((CHAR8 *)DosHdr) + DosHdr->e_lfanew);
  } else {
    //
    // No Dos header so assume image starts with PE header.
    //
    Pe32 = (EFI_IMAGE_NT_HEADERS32 *)DosHdr;
  }

  if ((Pe32->FileHeader.Machine == EFI_IMAGE_MACHINE_IA64) || (Pe32->FileHeader.Machine == EFI_IMAGE_MACHINE_X64)) {
    Pe64 = (EFI_IMAGE_NT_HEADERS64 *)Pe32;
  }

  //
  // Verify the "NO_LEGACY_BIOS_DEPENDENCIES" flag in the PE head
  //
  if ((((Pe64 != NULL) && (Pe64->OptionalHeader.DllCharacteristics & 0x2000)) ||
       ((Pe64 == NULL) && (Pe32->OptionalHeader.DllCharacteristics & 0x2000)))) {
    return TRUE;
  }

  return FALSE;
}

EFI_STATUS
BootFromImage (
  IN  EFI_HANDLE                        ImageHandle,
  IN  BDS_COMMON_OPTION             *Option,
  IN  EFI_DEVICE_PATH_PROTOCOL      *DevicePath,
  OUT UINTN                         *ExitDataSize,
  OUT CHAR16                        **ExitData OPTIONAL
  )
/*++

Routine Description:

  According to device path  and image handle to boot.

Arguments:

  ImageHandle  -
  Option       - The boot option need to be processed
  DevicePath   - The device path which describe where to load
                 the boot image or the legcy BBS device path
                 to boot the legacy OS
  ExitDataSize - Returned directly from gBS->StartImage ()
  ExitData     - Returned directly from gBS->StartImage ()

Returns:

  EFI_SUCCESS   - Boot from recovery boot option successful
  Other         - Some errors occured during boot process

--*/
{
  EFI_STATUS                        Status;
  EFI_LOADED_IMAGE_PROTOCOL         *ImageInfo;
  EFI_HANDLE                        *SelectedVgaHandle;

  SelectedVgaHandle = NULL;

  Status = gBS->HandleProtocol (ImageHandle, &gEfiLoadedImageProtocolGuid, &ImageInfo);
  ASSERT_EFI_ERROR (Status);

  if (Option->LoadOptionsSize != 0) {
    ImageInfo->LoadOptionsSize  = Option->LoadOptionsSize;
    ImageInfo->LoadOptions      = Option->LoadOptions;
  }

  //
  // Before calling the image, enable the Watchdog Timer for
  // the 5 Minute period
  //
  gBS->SetWatchdogTimer (5 * 60, 0x0000, 0x00, NULL);

  POSTCODE(POST_BDS_START_IMAGE);    //PostCode = 0xFB, UEFI Boot Start Image

  PERF_END (0, L"PostBDS", NULL, 0);
  WRITE_BOOT_TO_OS_PERFORMANCE_DATA;
#ifdef AUTO_CREATE_DUMMY_BOOT_OPTION
  BdsLibChangeToVirtualBootOrder ();
#endif
  DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Image Start...\n"));
  UpdateFirmwareBasicStartImageStart ();

  Status = gBS->StartImage (ImageHandle, ExitDataSize, ExitData);
  DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Image Return Status = %r\n", Status));

  //
  // Clear the Watchdog Timer after the image returns
  //
  gBS->SetWatchdogTimer (0x0000, 0x0000, 0x0000, NULL);
#ifdef AUTO_CREATE_DUMMY_BOOT_OPTION
  SyncBootOrder ();
  BdsLibRestoreBootOrderFromPhysicalBootOrder ();
#endif
  return Status;
}

EFI_STATUS
BootRecoveryOption (
  IN  BDS_COMMON_OPTION             *Option,
  IN  EFI_DEVICE_PATH_PROTOCOL      *DevicePath,
  OUT UINTN                         *ExitDataSize,
  OUT CHAR16                        **ExitData OPTIONAL
  )
/*++

Routine Description:

  According to device path  to boot from reocvery boot option.

Arguments:

  Option       - The boot option need to be processed
  DevicePath   - The device path which describe where to load
                 the boot image or the legcy BBS device path
                 to boot the legacy OS
  ExitDataSize - Returned directly from gBS->StartImage ()
  ExitData     - Returned directly from gBS->StartImage ()

Returns:

  EFI_SUCCESS   - Boot from recovery boot option successful
  Other         - Some errors occured during boot process

--*/
{
  EFI_DEVICE_PATH_PROTOCOL  *WorkingDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *FilePath;
  EFI_HANDLE                Handle;
  EFI_STATUS                Status;
  EFI_HANDLE                ImageHandle;

  if (!IsUefiOsFilePath (DevicePath)) {
    return EFI_UNSUPPORTED;
  }
  WorkingDevicePath = DevicePath;
  Status = gBS->LocateDevicePath (
                  &gEfiSimpleFileSystemProtocolGuid,
                  &WorkingDevicePath,
                  &Handle
                  );
  if (!EFI_ERROR (Status)) {
    FilePath = EfiFileDevicePath (Handle, DEFAULT_REMOVABLE_FILE_NAME);
    if (FilePath != NULL) {
      Status = gBS->LoadImage (
                      TRUE,
                      mBdsImageHandle,
                      FilePath,
                      NULL,
                      0,
                      &ImageHandle
                      );

      if (!EFI_ERROR (Status)) {
        Status = BootFromImage (ImageHandle, Option, FilePath, ExitDataSize, ExitData);
      }
      gBS->FreePool (FilePath);
    }
  }
  return Status;
}


EFI_STATUS
BdsLibBootViaBootOption (
  IN  BDS_COMMON_OPTION             * Option,
  IN  EFI_DEVICE_PATH_PROTOCOL      * DevicePath,
  OUT UINTN                         *ExitDataSize,
  OUT CHAR16                        **ExitData OPTIONAL
  )
/*++

Routine Description:

  Process the boot option follow the EFI 1.1 specification and
  special treat the legacy boot option with BBS_DEVICE_PATH.

Arguments:

  Option       - The boot option need to be processed
  DevicePath   - The device path which describe where to load
                 the boot image or the legcy BBS device path
                 to boot the legacy OS
  ExitDataSize - Returned directly from gBS->StartImage ()
  ExitData     - Returned directly from gBS->StartImage ()

Returns:

  EFI_SUCCESS   - Status from gBS->StartImage (), or BdsBootByDiskSignatureAndPartition ()
  EFI_NOT_FOUND - If the Device Path is not found in the system

--*/
{
  EFI_STATUS                Status;
  EFI_HANDLE                Handle;
  EFI_HANDLE                ImageHandle;
  EFI_DEVICE_PATH_PROTOCOL  *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *FilePath;
  EFI_EVENT                 ReadyToBootEvent;
  EFI_ACPI_S3_SAVE_PROTOCOL *AcpiS3Save;
  EFI_BLOCK_IO_PROTOCOL     *BlkIo;
  VOID                      *Buffer;
  BDS_COMMON_OPTION         *OptionTemp;
  EFI_LIST_ENTRY            *LinkTemp;
  EFI_DEVICE_PATH_PROTOCOL  *WorkingDevicePath;
  EFI_STATUS                LocateDevicePathStatus;
  EFI_STATUS                RecoverBootStatus;

  EFI_TREE_MEASURE_OS_LOADER_AUTHORITY_PROTOCOL   *MeasureOsLoaderAuthorityProtocol;
  UINT16                            BootNext;
  UINTN                             DefaultConsoleAttribute;

  POSTCODE (BDS_EFI64_SHADOW_ALL_LEGACY_ROM); //PostCode = 0x2C, Shadow Misc Option ROM
  //
  // All the driver options should have been processed since
  // now boot will be performed.
  //
  PERF_END (0, BDS_TOK, NULL, 0);

  PERF_START (0, L"PostBDS", NULL, 0);
  *ExitDataSize = 0;
  *ExitData     = NULL;

  //
  // Before boot to device, always clean BootNext variable.
  //
  gRT->SetVariable (
         L"BootNext",
         &gEfiGlobalVariableGuid,
         EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
         0,
         &BootNext
         );

  //
  // Notes: put EFI64 ROM Shadow Solution
  //
  EFI64_SHADOW_ALL_LEGACY_ROM ();

  //
  // If it's Device Path that starts with a hard drive path, append it with the front part to compose a
  // full device path
  //
  WorkingDevicePath = NULL;
  if ((DevicePathType (DevicePath) == MEDIA_DEVICE_PATH) &&
      (DevicePathSubType (DevicePath) == MEDIA_HARDDRIVE_DP)) {
    WorkingDevicePath = ExpandPartitionPartialDevicePathToFull (
                          (HARDDRIVE_DEVICE_PATH *) DevicePath
                          );
    if (WorkingDevicePath != NULL) {
      DevicePath = WorkingDevicePath;
    }
  }


  //
  // Signal the EFI_EVENT_SIGNAL_READY_TO_BOOT event
  //
  POSTCODE (BDS_READY_TO_BOOT_EVENT); //PostCode = 0x2E, Last Chipset initial before boot to OS

  Status = EfiCreateEventReadyToBoot (
             EFI_TPL_CALLBACK,
             NULL,
             NULL,
             &ReadyToBootEvent
             );
  if (!EFI_ERROR (Status)) {
    gBS->SignalEvent (ReadyToBootEvent);
    gBS->CloseEvent (ReadyToBootEvent);
  }
  //
  // S3Save should be executed after EventReadyToBoot
  //
  Status = gBS->LocateProtocol (&gEfiAcpiS3SaveGuid, NULL, &AcpiS3Save);
  if (!EFI_ERROR (Status)) {
    AcpiS3Save->S3Save (AcpiS3Save, NULL);
  }

  //
  // Set Boot Current
  //
  gRT->SetVariable (
        L"BootCurrent",
        &gEfiGlobalVariableGuid,
        EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
        sizeof (UINT16),
        &Option->BootCurrent
        );
  if ((DevicePathType (Option->DevicePath) == BBS_DEVICE_PATH) &&
      (DevicePathSubType (Option->DevicePath) == BBS_BBS_DP)) {
#ifdef AUTO_CREATE_DUMMY_BOOT_OPTION
    if (BdsLibIsBootOrderHookEnabled ()) {
      BdsLibRestoreBootOrderFromPhysicalBootOrder ();
    }
#endif
    //
    // Check to see if we should legacy BOOT. If yes then do the legacy boot
    //
    POSTCODE (BDS_GO_LEGACY_BOOT);  //PostCode = 0x2F, Start to boot Legacy OS
    return BdsLibDoLegacyBoot (Option);
  }

  //
  // Drop the TPL level from EFI_TPL_DRIVER to EFI_TPL_APPLICATION
  //
  POSTCODE (BDS_GO_UEFI_BOOT);  //PostCode = 0x30, Start to boot UEFI OS

  gBS->RestoreTPL (EFI_TPL_APPLICATION);

#ifdef EFI_DEBUG
  DumpMemoryMap();
#endif

#ifdef MEMORY_MAP_CONSISTENCY_CHECK
  CheckRtAndBsMemUsage ();
#endif

  UpdateFirmwareBasicLoadImageStart ();
  DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Booting EFI 1.1 way %S\n", Option->Description));

  EnableOptimalTextMode ();

  DefaultConsoleAttribute = gST->ConOut->Mode->Attribute;
  gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR(DEFAULT_CONSOLE_TEXT_COLOR, DEFAULT_COLSOLE_BACKGROUND_COLOR));

  Status = gBS->LocateProtocol (
                &gEfiTreeMeasureOsLoaderAuthorityProtocolGuid,
                NULL,
                &MeasureOsLoaderAuthorityProtocol
                );
  if (!EFI_ERROR(Status)) {
    MeasureOsLoaderAuthorityProtocol->OsLoader = TRUE;
  }

  Status = gBS->LoadImage (
                  TRUE,
                  mBdsImageHandle,
                  DevicePath,
                  NULL,
                  0,
                  &ImageHandle
                  );

  //
  // If we didn't find an image, we may need to load the default
  // boot behavior for the device.
  //
  if (EFI_ERROR (Status)) {
    //
    // Find a Simple File System protocol on the device path. If the remaining
    // device path is set to end then no Files are being specified, so try
    // the removable media file name.
    //
    TempDevicePath = DevicePath;
    LocateDevicePathStatus = gBS->LocateDevicePath (
                                    &gEfiSimpleFileSystemProtocolGuid,
                                    &TempDevicePath,
                                    &Handle
                                    );
    if (!EFI_ERROR (LocateDevicePathStatus) && IsDevicePathEnd (TempDevicePath)) {
      FilePath = EfiFileDevicePath (Handle, DEFAULT_REMOVABLE_FILE_NAME);
      if (FilePath) {
        //
        // Issue a dummy read to the device to check for media change.
        // When the removable media is changed, any Block IO read/write will
        // cause the BlockIo protocol be reinstalled and EFI_MEDIA_CHANGED is
        // returned. After the Block IO protocol is reinstalled, subsequent
        // Block IO read/write will success.
        //
        Status = gBS->HandleProtocol (
                        Handle,
                        &gEfiBlockIoProtocolGuid,
                        (VOID **) &BlkIo
                        );
        if (!EFI_ERROR (Status)) {
          Buffer = EfiLibAllocatePool (BlkIo->Media->BlockSize);
          if (Buffer != NULL) {
            BlkIo->ReadBlocks (
                     BlkIo,
                     BlkIo->Media->MediaId,
                     0,
                     BlkIo->Media->BlockSize,
                     Buffer
                     );
            gBS->FreePool (Buffer);
          }
        }

        Status = gBS->LoadImage (
                        TRUE,
                        mBdsImageHandle,
                        FilePath,
                        NULL,
                        0,
                        &ImageHandle
                        );
        gBS->FreePool (FilePath);

        if (EFI_ERROR (Status)) {
          //
          // The DevicePath failed, and it's not a valid
          // removable media device.
          //
          goto Done;
        }
      }
    }
  }

  if (EFI_ERROR (Status)) {
    //
    // If the DevicePath is USB class, we will boot the next USB EFI Boot Option
    //
    if ((DevicePathType (Option->DevicePath) == MESSAGING_DEVICE_PATH) &&
        (DevicePathSubType (Option->DevicePath) == MSG_USB_CLASS_DP)) {
      //
      // Try Windows To Go devices in the list until one device return success
      //
      Status = EFI_NOT_FOUND;
      LinkTemp = mWindowsToGoDeviceList.ForwardLink;
      while (LinkTemp != &mWindowsToGoDeviceList) {
        OptionTemp = CR (LinkTemp, BDS_COMMON_OPTION, Link, BDS_LOAD_OPTION_SIGNATURE);
        Status = BdsLibBootViaBootOption (OptionTemp, OptionTemp->DevicePath, ExitDataSize, &(*ExitData));
        if (!EFI_ERROR (Status)) {
          break;
        }

        LinkTemp = LinkTemp->ForwardLink;
      }
    }
    //
    // It there is any error from the Boot attempt exit now.
    //
    goto Done;
  }
  //
  // Provide the image with it's load options
  //
  Status = BootFromImage (ImageHandle, Option, DevicePath, ExitDataSize, ExitData);

  //
  // Restore Console attribure
  //
  gST->ConOut->SetAttribute (gST->ConOut, DefaultConsoleAttribute);

Done:
  if (EFI_ERROR (Status) && IsUefiOsFilePath (DevicePath)) {
    //
    // Try to boot from recovery boot option and only update status to EFI_SUCCESS, if boot from
    // this recovery file path is successful.
    //
    RecoverBootStatus = BootRecoveryOption (Option, DevicePath, ExitDataSize, ExitData);
    if (!EFI_ERROR (RecoverBootStatus)) {
      Status = EFI_SUCCESS;
    }
  }
  if (WorkingDevicePath != NULL) {
    gBS->FreePool (WorkingDevicePath);
  }
  //
  // Clear Boot Current
  //
  gRT->SetVariable (
         L"BootCurrent",
         &gEfiGlobalVariableGuid,
         EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
         0,
         &Option->BootCurrent
         );
  //
  // Clear Boot Option
  //
  mEnumBootDevice = FALSE;

  //
  // Signal BIOS after the image returns
  //
  SignalImageReturns ();

  //
  // Raise the TPL level back to EFI_TPL_DRIVER
  //
  gBS->RaiseTPL (EFI_TPL_DRIVER);

  return Status;
}

VOID
BdsLibDeleteRedundantOption (
  )
/*++
Routine Description:
  Delete the boot option that content was duplicated.

--*/
{
  UINT16                    *BootOrder;
  UINT16                    *TmpBootOrder;
  UINTN                     BootOrderSize;
  UINTN                     SearchIndex;
  UINTN                     Index;
  UINTN                     BootOptionCnt;
  UINT16                    BootIndex[BOOT_OPTION_MAX_CHAR];
  UINT8                     **BootOptionPool;
  UINTN                     *BootOptionSizePool;
  INTN                      CompareResult;
  BOOLEAN                   BootOrderChanged;

  BootOrder          = NULL;
  TmpBootOrder       = NULL;
  BootOptionPool     = NULL;
  BootOptionSizePool = NULL;
  BootOrderChanged   = FALSE;
  BootOptionCnt      = 0;
  BootOrderSize      = 0;
  //
  // Get all boot option
  //
  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &BootOrderSize
                );
  TmpBootOrder = EfiLibAllocateZeroPool (BootOrderSize);
  if (NULL == BootOrder || TmpBootOrder == NULL) {
    goto EXIT;
  }
  EfiCopyMem (TmpBootOrder, BootOrder, BootOrderSize);
  BootOptionCnt      = BootOrderSize / sizeof (UINT16);
  if (BootOptionCnt == 1) {
    goto EXIT;
  }

  BootOptionSizePool = EfiLibAllocateZeroPool (BootOptionCnt * sizeof (UINTN));
  BootOptionPool     = EfiLibAllocateZeroPool (BootOptionCnt * sizeof (UINT8 *));
  if (BootOptionSizePool == NULL || BootOptionPool == NULL) {
    goto EXIT;
  }
  Index = 0;
  while (Index < BootOptionCnt) {
    SPrint (BootIndex, sizeof (BootIndex), L"Boot%04x", BootOrder[Index]);
    BootOptionPool[Index] = BdsLibGetVariableAndSize (
                              BootIndex,
                              &gEfiGlobalVariableGuid,
                              (BootOptionSizePool + Index)
                              );
    if (NULL == BootOptionPool[Index]) {
      goto EXIT;
    }
    Index++;
  }

  //
  // Scan and delete redundant option
  //
  for (Index = 0; Index < BootOptionCnt; Index++) {
    if (BootOptionSizePool[Index] == 0) {
      continue;
    }
    for (SearchIndex = Index + 1; SearchIndex < BootOptionCnt; SearchIndex++) {
      if ((BootOptionSizePool[Index] != 0) && (BootOptionSizePool[Index] == BootOptionSizePool[SearchIndex])) {
        CompareResult = EfiCompareMem (
                          BootOptionPool[Index],
                          BootOptionPool[SearchIndex],
                          BootOptionSizePool[Index]
                          );
        if (CompareResult == 0) {
          BootOrderChanged = TRUE;
          BdsDeleteBootOption (TmpBootOrder[SearchIndex], BootOrder, &BootOrderSize);
          BootOptionSizePool[SearchIndex] = 0;
        }
      }
    }
  }

  //
  // Restore BootOrder if need.
  //
  if (BootOrderChanged) {
    gRT->SetVariable (
           L"BootOrder",
           &gEfiGlobalVariableGuid,
           EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
           BootOrderSize,
           BootOrder
           );
  }

EXIT:
  if (TmpBootOrder != NULL) {
    gBS->FreePool (TmpBootOrder);
  }
  if (BootOptionSizePool != NULL) {
    gBS->FreePool (BootOptionSizePool);
  }
  if (BootOptionPool != NULL) {
    for (Index = 0; Index < BootOptionCnt; Index++) {
      if (BootOptionPool[Index] != NULL) {
        gBS->FreePool (BootOptionPool[Index]);
      }
    }
    gBS->FreePool (BootOptionPool);
  }
  if (BootOrder != NULL) {
    gBS->FreePool (BootOrder);
  }
}

EFI_STATUS
BdsBootByDiskSignatureAndPartition (
  IN  BDS_COMMON_OPTION          * Option,
  IN  HARDDRIVE_DEVICE_PATH      * HardDriveDevicePath,
  IN  UINT32                     LoadOptionsSize,
  IN  VOID                       *LoadOptions,
  OUT UINTN                      *ExitDataSize,
  OUT CHAR16                     **ExitData OPTIONAL
  )
/*++

Routine Description:

  Check to see if a hard ware device path was passed in. If it was then search
  all the block IO devices for the passed in hard drive device path.

Arguments:

  Option              - The current processing boot option.
  HardDriveDevicePath - EFI Device Path to boot, if it starts with a hard drive device path.
  LoadOptionsSize     - Passed into gBS->StartImage () via the loaded image protocol.
  LoadOptions         - Passed into gBS->StartImage () via the loaded image protocol.
  ExitDataSize        - returned directly from gBS->StartImage ()
  ExitData            - returned directly from gBS->StartImage ()

Returns:

  EFI_SUCCESS   - Status from gBS->StartImage (), or BootByDiskSignatureAndPartition ()
  EFI_NOT_FOUND - If the Device Path is not found in the system

--*/
{
  EFI_STATUS                Status;
  UINTN                     BlockIoHandleCount;
  EFI_HANDLE                *BlockIoBuffer;
  EFI_DEVICE_PATH_PROTOCOL  *BlockIoDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *NewDevicePath;
  UINTN                     Index;
  EFI_STATUS                HandleProtocolStatus;

  *ExitDataSize = 0;
  *ExitData     = NULL;

  if (!((DevicePathType (&HardDriveDevicePath->Header) == MEDIA_DEVICE_PATH) &&
        (DevicePathSubType (&HardDriveDevicePath->Header) == MEDIA_HARDDRIVE_DP))) {
    //
    // If the HardDriveDevicePath does not start with a Hard Drive Device Path exit.
    //
    return EFI_NOT_FOUND;
  }
  //
  // The boot device have already been connected
  //
  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiBlockIoProtocolGuid, NULL, &BlockIoHandleCount, &BlockIoBuffer);
  if (EFI_ERROR (Status) || BlockIoHandleCount == 0) {
    //
    // If there was an error or there are no device handles that support
    // the BLOCK_IO Protocol, then return.
    //
    return EFI_NOT_FOUND;
  }
  //
  // Loop through all the device handles that support the BLOCK_IO Protocol
  //
  Status = EFI_NOT_FOUND;
  for (Index = 0; Index < BlockIoHandleCount; Index++) {
    HandleProtocolStatus = gBS->HandleProtocol (
                                  BlockIoBuffer[Index],
                                  &gEfiDevicePathProtocolGuid,
                                  (VOID *) &BlockIoDevicePath
                                  );
    if (EFI_ERROR (HandleProtocolStatus) || BlockIoDevicePath == NULL) {
      continue;
    }
    //
    // Only do the boot, when devicepath match
    //
    if (MatchPartitionDevicePathNode (BlockIoDevicePath, HardDriveDevicePath)) {
      //
      // Combine the Block IO and Hard Drive Device path together and try
      // to boot from it.
      //
      DevicePath    = NextDevicePathNode ((EFI_DEVICE_PATH_PROTOCOL *) HardDriveDevicePath);
      NewDevicePath = EfiAppendDevicePath (BlockIoDevicePath, DevicePath);

      //
      // Recursive boot with new device path
      //
      Status = BdsLibBootViaBootOption (Option, NewDevicePath, ExitDataSize, ExitData);
      //
      // No matter MBR or GPT, the partition sigunature is unique, so we should break after booting from
      // boot option.
      //
      break;
    }
  }

  gBS->FreePool (BlockIoBuffer);
  return Status;
}

EFI_STATUS
BdsLibDeleteOptionFromHandle (
  IN  EFI_HANDLE                 Handle
  )
/*++

Routine Description:

  Delete the boot option associated with the handle passed in

Arguments:

  Handle - The handle which present the device path to create boot option

Returns:

  EFI_SUCCESS           - Delete the boot option success
  EFI_NOT_FOUND         - If the Device Path is not found in the system
  EFI_OUT_OF_RESOURCES  - Lack of memory resource
  Other                 - Error return value from SetVariable()

--*/
{
  UINT16                    *BootOrder;
  UINT8                     *BootOptionVar;
  UINTN                     BootOrderSize;
  UINTN                     BootOptionSize;
  EFI_STATUS                Status;
  UINTN                     Index;
  UINT16                    BootOption[BOOT_OPTION_MAX_CHAR];
  UINTN                     DevicePathSize;
  UINTN                     OptionDevicePathSize;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *OptionDevicePath;
  UINT8                     *TempPtr;
  CHAR16                    *Description;

  Status        = EFI_SUCCESS;
  BootOrder     = NULL;
  BootOrderSize = 0;

  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &BootOrderSize
                );
  if (NULL == BootOrder) {
    return EFI_NOT_FOUND;
  }

  DevicePath = EfiDevicePathFromHandle (Handle);
  if (DevicePath == NULL) {
    return EFI_NOT_FOUND;
  }
  DevicePathSize = EfiDevicePathSize (DevicePath);

  Index = 0;
  while (Index < BootOrderSize / sizeof (UINT16)) {
    SPrint (BootOption, sizeof (BootOption), L"Boot%04x", BootOrder[Index]);
    BootOptionVar = BdsLibGetVariableAndSize (
                      BootOption,
                      &gEfiGlobalVariableGuid,
                      &BootOptionSize
                      );
    if (NULL == BootOptionVar) {
      gBS->FreePool (BootOrder);
      return EFI_OUT_OF_RESOURCES;
    }

    TempPtr = BootOptionVar;
    TempPtr += sizeof (UINT32) + sizeof (UINT16);
    Description = (CHAR16 *) TempPtr;
    TempPtr += EfiStrSize ((CHAR16 *) TempPtr);
    OptionDevicePath = (EFI_DEVICE_PATH_PROTOCOL *) TempPtr;
    OptionDevicePathSize = EfiDevicePathSize (OptionDevicePath);

    //
    // Check whether the device path match
    //
    if ((OptionDevicePathSize == DevicePathSize) &&
        (EfiCompareMem (DevicePath, OptionDevicePath, DevicePathSize) == 0)) {
      BdsDeleteBootOption (BootOrder[Index], BootOrder, &BootOrderSize);
      gBS->FreePool (BootOptionVar);
      break;
    }

    gBS->FreePool (BootOptionVar);
    Index++;
  }

  Status = gRT->SetVariable (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  BootOrderSize,
                  BootOrder
                  );

  gBS->FreePool (BootOrder);

  return Status;
}

BOOLEAN
MatchPartitionDevicePathNode (
  IN  EFI_DEVICE_PATH_PROTOCOL   *BlockIoDevicePath,
  IN  HARDDRIVE_DEVICE_PATH      *HardDriveDevicePath
  )
/*++

Routine Description:

  Check whether there is a instance in BlockIoDevicePath, which contain multi device path
  instances, has the same partition node with HardDriveDevicePath device path

Arguments:

  BlockIoDevicePath     - Multi device path instances which need to check
  HardDriveDevicePath   - A device path which starts with a hard drive media device path.

Returns:

  TRUE  - There is a matched device path instance
  FALSE - There is no matched device path instance

--*/
{
  HARDDRIVE_DEVICE_PATH     *TmpHdPath;
  HARDDRIVE_DEVICE_PATH     *TempPath;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  BOOLEAN                   Match;
  EFI_DEVICE_PATH_PROTOCOL  *BlockIoHdDevicePathNode;

  if ((BlockIoDevicePath == NULL) || (HardDriveDevicePath == NULL)) {
    return FALSE;
  }
  //
  // Make PreviousDevicePath == the device path node before the end node
  //
  DevicePath          = BlockIoDevicePath;
  BlockIoHdDevicePathNode = NULL;

  //
  // find the partition device path node
  //
  while (!IsDevicePathEnd (DevicePath)) {
    if ((DevicePathType (DevicePath) == MEDIA_DEVICE_PATH) &&
        (DevicePathSubType (DevicePath) == MEDIA_HARDDRIVE_DP)
        ) {
      BlockIoHdDevicePathNode = DevicePath;
      break;
    }

    DevicePath = NextDevicePathNode (DevicePath);
  }

  if (BlockIoHdDevicePathNode == NULL) {
    return FALSE;
  }
  //
  // See if the harddrive device path in blockio matches the orig Hard Drive Node
  //
  TmpHdPath = (HARDDRIVE_DEVICE_PATH *) BlockIoHdDevicePathNode;
  TempPath  = (HARDDRIVE_DEVICE_PATH *) BdsLibUnpackDevicePath ((EFI_DEVICE_PATH_PROTOCOL *) HardDriveDevicePath);
  Match = FALSE;
  //
  // Check for the match
  //
  if ((TempPath != NULL) &&
      (TmpHdPath->MBRType == TempPath->MBRType) &&
      (TmpHdPath->SignatureType == TempPath->SignatureType)) {
    switch (TmpHdPath->SignatureType) {
    case SIGNATURE_TYPE_GUID:
      Match = EfiCompareGuid ((EFI_GUID *)TmpHdPath->Signature, (EFI_GUID *)TempPath->Signature);
      break;
    case SIGNATURE_TYPE_MBR:
      Match = (BOOLEAN)(*((UINT32 *)(&(TmpHdPath->Signature[0]))) == *(UINT32 *)(&(TempPath->Signature[0])));
      break;
    default:
      Match = FALSE;
      break;
    }
  }

  if (TempPath != NULL) {
    gBS->FreePool (TempPath);
  }

  return Match;
}

EFI_DEVICE_PATH_PROTOCOL *
ExpandPartitionPartialDevicePathToFull (
  IN  HARDDRIVE_DEVICE_PATH      *HardDriveDevicePath
  )
/*++

Routine Description:

  Expand a device path that starts with a hard drive media device path node to be a
  full device path that includes the full hardware path to the device.
  It maybe been combined with BdsBootByDiskSignatureAndPartition.

Arguments:

  HardDriveDevicePath - EFI Device Path to boot, if it starts with a hard
                        drive media device path.
Returns:

  A Pointer to the full device path.
  NULL - Cannot find a valid Hard Drive devic path

--*/
{
  EFI_STATUS                Status;
  UINTN                     BlockIndex;
  UINTN                     BlockIoHandleCount;
  EFI_HANDLE                *BlockIoBuffer;
  EFI_DEVICE_PATH_PROTOCOL  *BlockIoDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *NewDevicePath;

  if (!((DevicePathType (&HardDriveDevicePath->Header) == MEDIA_DEVICE_PATH) &&
        (DevicePathSubType (&HardDriveDevicePath->Header) == MEDIA_HARDDRIVE_DP))) {
    return NULL;
  }

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiBlockIoProtocolGuid,
                  NULL,
                  &BlockIoHandleCount,
                  &BlockIoBuffer
                  );

  NewDevicePath = NULL;
  if (!EFI_ERROR (Status)) {
   for (BlockIndex = 0; BlockIndex < BlockIoHandleCount; BlockIndex++) {

      Status = gBS->HandleProtocol (
                      BlockIoBuffer[BlockIndex],
                      &gEfiDevicePathProtocolGuid,
                      &BlockIoDevicePath
                      );
      if (EFI_ERROR (Status) || BlockIoDevicePath == NULL) {
        continue;
      }

      if (MatchPartitionDevicePathNode (BlockIoDevicePath, HardDriveDevicePath)) {
        //
        // Append the file path infomration
        //
        DevicePath    = NextDevicePathNode ((EFI_DEVICE_PATH_PROTOCOL *) HardDriveDevicePath);
        NewDevicePath = EfiAppendDevicePath (BlockIoDevicePath, DevicePath);
        break;
      }
    }
  }

  if (BlockIoBuffer != NULL) {
    gBS->FreePool (BlockIoBuffer);
  }

  return NewDevicePath;
}

BOOLEAN
IsAlreadyInBootOrder (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
/*++

Routine Description:

  Compare device path to check if this boot device is already exist in BootOrder.

Arguments:

  DevicePath - Device path of the query boot device

Returns:

  TRUE  - The device is already in BootOrder variable.
  FALSE - The device is not in BootOrder variable.

--*/
{
  UINTN                     DevicePathSize;
  UINT16                    *BootOrder;
  UINTN                     BootOrderSize;
  UINTN                     BootDevNum;
  UINTN                     Index;
  UINT16                    BootOptionName[20];
  UINTN                     VariableSize;
  UINT8                     *BootOption;
  UINT8                     *WorkingPtr;
  EFI_DEVICE_PATH_PROTOCOL  *BootOptionDevicePath;
  UINTN                     BootOptionDevicePathSize;

  DevicePathSize = EfiDevicePathSize (DevicePath);

  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &BootOrderSize
                );
  if (BootOrder == NULL) {
    return FALSE;
  }

  BootDevNum = BootOrderSize / sizeof (UINT16);
  for (Index = 0; Index < BootDevNum; Index++) {
    SPrint (BootOptionName, sizeof (BootOptionName), L"Boot%04x", BootOrder[Index]);
    BootOption = BdsLibGetVariableAndSize (
                   BootOptionName,
                   &gEfiGlobalVariableGuid,
                   &VariableSize
                   );
    if (BootOption == NULL) {
      continue;
    }

    //
    // Find device path in Bootxxxx variable
    //
    WorkingPtr = BootOption;
    WorkingPtr += sizeof (UINT32);
    WorkingPtr += sizeof (UINT16);
    WorkingPtr += (UINTN) EfiStrSize ((UINT16 *) WorkingPtr);
    BootOptionDevicePath     = (EFI_DEVICE_PATH_PROTOCOL *) WorkingPtr;
    BootOptionDevicePathSize = EfiDevicePathSize (BootOptionDevicePath);

    if (DevicePathSize == BootOptionDevicePathSize &&
        EfiCompareMem (DevicePath, BootOptionDevicePath, BootOptionDevicePathSize) == 0) {
      gBS->FreePool (BootOption);
      gBS->FreePool (BootOrder);
      return TRUE;
    }

    gBS->FreePool (BootOption);
  }

  gBS->FreePool (BootOrder);

  return FALSE;
}

EFI_STATUS
GetShellFileHashValue (
  IN  EFI_HANDLE                  Handle,
  OUT UINT32                      *HashValue
  )
/*++

Routine Description:

  Get the hash value of shell file

Arguments:

  Handle      - FV handle value
  HashValue   - Output hash value of file

Returns:

  EFI_SUCCESS - Get hash value of file successfully.
  EFI_ABORTED - Read file fail.
  otherwise   - HandleProtocol or AllocatePage or CalculateCrc32 fail.

--*/
{
  EFI_STATUS                      Status;
  EFI_FIRMWARE_VOLUME_PROTOCOL    *Fv;
  UINT8                           *File;
  UINTN                           Size;
  EFI_FV_FILETYPE                 Type;
  EFI_FV_FILE_ATTRIBUTES          Attributes;
  UINT32                          AuthenticationStatus;
  UINTN                           PageNum;

  PageNum = 0;

  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiFirmwareVolumeProtocolGuid,
                  (VOID **) &Fv
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  File = NULL;
  Status = Fv->ReadFile (
                 Fv,
                 &gEfiShellFileGuid,
                 &File,
                 &Size,
                 &Type,
                 &Attributes,
                 &AuthenticationStatus
                 );
  if (Status == EFI_WARN_BUFFER_TOO_SMALL || Status == EFI_BUFFER_TOO_SMALL) {
    PageNum = EFI_SIZE_TO_PAGES(Size);
    Status = gBS->AllocatePages (
                    AllocateMaxAddress,
                    EfiBootServicesData,
                    PageNum,
                    (EFI_PHYSICAL_ADDRESS *) &File
                    );
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Size = EFI_PAGES_TO_SIZE(PageNum);
    Status = Fv->ReadFile (
                   Fv,
                   &gEfiShellFileGuid,
                   &File,
                   &Size,
                   &Type,
                   &Attributes,
                   &AuthenticationStatus
                   );
    if (Status != EFI_SUCCESS) {
      gBS->FreePages ((EFI_PHYSICAL_ADDRESS) File, PageNum);
      return EFI_ABORTED;
    }
  }

  if (Status != EFI_SUCCESS) {
    return EFI_ABORTED;
  }

  Status = gBS->CalculateCrc32 (File, Size, HashValue);

  if (PageNum != 0) {
    gBS->FreePages ((EFI_PHYSICAL_ADDRESS) File, PageNum);
  } else {
    gBS->FreePool (File);
  }

  return Status;
}

EFI_STATUS
UpdateShellDevicePath (
  IN CHAR16                      *OptionName,
  IN CHAR16                      *Description,
  IN UINT32                      BootOptionHashValue
  )
/*++

Routine Description:

  Update device path of shell in BootXXXX variable.

Arguments:

  OptionName          - Boot option string
  Description         - Description of boot option
  BootOptionHashValue - Hase value of shell boot option

Returns:

  EFI_SUCCESS   - Success to update shell device path in BootXXXX variable.
  EFI_NOT_FOUND - Can not find the corresponding device path by hash value.
  otherwise     - Update variable fail.

--*/
{
  EFI_STATUS                               Status;
  UINTN                                    Index;
  UINTN                                    FvHandleCount;
  EFI_HANDLE                               *FvHandleBuffer;
  EFI_DEVICE_PATH_PROTOCOL                 *DevicePath;
  MEDIA_FW_VOL_FILEPATH_DEVICE_PATH        ShellNode;
  UINT8                                    OptionalData[SHELL_OPTIONAL_DATA_SIZE];
  UINT32                                   HashValue;

  Status = EFI_NOT_FOUND;
  FvHandleCount = 0;
  FvHandleBuffer = NULL;

  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiFirmwareVolumeProtocolGuid,
         NULL,
         &FvHandleCount,
         &FvHandleBuffer
         );
  for (Index = 0; Index < FvHandleCount; Index++) {
    Status = GetShellFileHashValue (FvHandleBuffer[Index], &HashValue);
    if (EFI_ERROR (Status) || (BootOptionHashValue != HashValue)) {
      continue;
    }

    DevicePath = EfiDevicePathFromHandle (FvHandleBuffer[Index]);
    EfiInitializeFwVolDevicepathNode (&ShellNode, &gEfiShellFileGuid);
    DevicePath = EfiAppendDevicePathNode (DevicePath, (EFI_DEVICE_PATH_PROTOCOL *) &ShellNode);

    if (IsAlreadyInBootOrder (DevicePath)) {
      gBS->FreePool (DevicePath);
      continue;
    }

    gBS->CopyMem (OptionalData    , "RC"      , 2);
    gBS->CopyMem (&OptionalData[2], &HashValue, 4);

    Status = BdsLibUpdateOptionVar (
               OptionName,
               DevicePath,
               Description,
               OptionalData,
               SHELL_OPTIONAL_DATA_SIZE
               );
    gBS->FreePool (DevicePath);
    if (!EFI_ERROR (Status)) {
      break;
    }
  }

  if (FvHandleCount != 0) {
    gBS->FreePool (FvHandleBuffer);
  }

  if (Index == FvHandleCount) {
    return EFI_NOT_FOUND;
  }

  return Status;
}

STATIC
BOOLEAN
IsEfiDevice (
  IN     UINT16    NumberOfDevice
  )
/*++

Routine Description:

  Check the Bootxxxx number of device is EFI device or legacy device.

Arguments:

  NumberOfDevice - The device number in BootOrder variable (ex: Boot0001,
                   then NumberOfDevice is 1)

Returns:

  TRUE  - The device is EFI device.
  FALSE - The device is legacy device.

--*/
{
  UINT16                    BootOptionName[20];
  UINTN                     BootOptionSize;
  UINT8                     *BootOption;
  UINT8                     *WorkingPtr;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  BOOLEAN                   IsEfiDevice;

  SPrint (
    BootOptionName,
    sizeof (BootOptionName),
    L"Boot%04x",
    NumberOfDevice
    );
  BootOption = BdsLibGetVariableAndSize (
                 BootOptionName,
                 &gEfiGlobalVariableGuid,
                 &BootOptionSize
                 );

  ASSERT (BootOption != NULL);
  if (BootOption == NULL) {
    return FALSE;
  }

  //
  // Find device path in Bootxxxx variable
  //
  WorkingPtr = BootOption;
  WorkingPtr += sizeof (UINT32);
  WorkingPtr += sizeof (UINT16);
  WorkingPtr += (UINTN) EfiStrSize ((UINT16 *) WorkingPtr);
  DevicePath = (EFI_DEVICE_PATH_PROTOCOL *) WorkingPtr;

  IsEfiDevice = TRUE;
  if ((BBS_DEVICE_PATH == DevicePath->Type) && (BBS_BBS_DP == DevicePath->SubType)) {
    IsEfiDevice = FALSE;
  }

  return IsEfiDevice;
}

BOOLEAN
ISUsbDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
/*++

Routine Description:

  Check the iput device path is wheter a USB device path or not.

Arguments:

  DevicePath -  Pointer to device path instance

Returns:

  TRUE       - It is a USB device path.
  FALSE      - It isn't a USB device path.

--*/
{
  BOOLEAN                      IsEfiUsb;
  EFI_DEVICE_PATH_PROTOCOL     *FullDevicePath;
  EFI_DEVICE_PATH_PROTOCOL     *WorkingDevicePath;


  if (DevicePath == NULL) {
    return FALSE;
  }

  IsEfiUsb = FALSE;
  FullDevicePath = ExpandPartitionPartialDevicePathToFull ((HARDDRIVE_DEVICE_PATH *) DevicePath);
  WorkingDevicePath = (FullDevicePath == NULL) ? DevicePath : FullDevicePath;
  while (!EfiIsDevicePathEnd (WorkingDevicePath)) {
    if (DevicePathType (WorkingDevicePath) == MESSAGING_DEVICE_PATH && DevicePathSubType (WorkingDevicePath) == MSG_USB_DP) {
      IsEfiUsb = TRUE;
      break;
    }
    WorkingDevicePath = EfiNextDevicePathNode (WorkingDevicePath);
  }
  if (FullDevicePath != NULL) {
    gBS->FreePool (FullDevicePath);
  }
  return IsEfiUsb;

}

EFI_STATUS
AdjustBootOrder (
  IN      BOOLEAN      EfiDeviceFirst,
  IN      UINTN        DeviceCount,
  IN OUT  UINT16       *BootOrder
  )
/*++

Routine Description:

  According to input parameter to adjust boot order to EFI device first or
  legacy device first.

Arguments:

  EfiDeviceFirst -  TRUE : Indicate EFI device first.
                    FALSE: Indicate legacy device first.
  DeviceCount    -  Total device count
  BootOrder      -  Pointer to BootOrder.

Returns:

  EFI_SUCCESS           - Adjust boot order successful.
  EFI_INVALID_PARAMETER - Input parameter is invalid.

--*/
{
  UINTN               BootOrderIndex;
  UINT16              *EfiDevicOrder;
  UINTN               EfiDeviceIndex;
  UINT16              *LegacyDeviceOrder;
  UINTN               LegacyDeviceIndex;

  if (DeviceCount == 0 || BootOrder == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  EfiDevicOrder = EfiLibAllocateZeroPool (DeviceCount * sizeof (CHAR16));
  LegacyDeviceOrder = EfiLibAllocateZeroPool (DeviceCount * sizeof (CHAR16));
  EfiDeviceIndex = 0;
  LegacyDeviceIndex = 0;

  //
  // According boot type (EFI or legacy) to put boot order respective buffer
  //
  for (BootOrderIndex = 0; BootOrderIndex < DeviceCount; BootOrderIndex++) {
    if (IsEfiDevice (BootOrder[BootOrderIndex])) {
      EfiDevicOrder[EfiDeviceIndex++] = BootOrder[BootOrderIndex];
    } else {
      LegacyDeviceOrder[LegacyDeviceIndex++] = BootOrder[BootOrderIndex];
    }
  }

  //
  // Adjust boot order depend on EFI device first or legacy device first
  //
  if (EfiDeviceFirst) {
    EfiCopyMem (BootOrder, EfiDevicOrder, EfiDeviceIndex * sizeof (CHAR16));
    EfiCopyMem (&BootOrder[EfiDeviceIndex], LegacyDeviceOrder, LegacyDeviceIndex * sizeof (CHAR16));
  } else {
    EfiCopyMem (BootOrder, LegacyDeviceOrder, LegacyDeviceIndex * sizeof (CHAR16));
    EfiCopyMem (&BootOrder[LegacyDeviceIndex], EfiDevicOrder, EfiDeviceIndex * sizeof (CHAR16));
  }

  gBS->FreePool (EfiDevicOrder);
  gBS->FreePool (LegacyDeviceOrder);
  return EFI_SUCCESS;
}

PCI_DEVICE_PATH *
GetPciDevicePath (
  EFI_DEVICE_PATH_PROTOCOL *DevicePath
  )
/*++

Routine Description:

  Get PCI device path from the list of device path

Arguments:

  DevicePath - The list of Device patch

Returns:

  DevicePath - PCI device path
  NULL       - No PCI device path

--*/
{
  if (DevicePath == NULL) {
    return NULL;
  }

  while (!IsDevicePathEnd (DevicePath)) {
    if (DevicePath->Type == HARDWARE_DEVICE_PATH && DevicePath->SubType == HW_PCI_DP) {
      return (PCI_DEVICE_PATH *) DevicePath;
    } else {
      DevicePath = NextDevicePathNode (DevicePath);
    }
  }

  return NULL;
}

BOOLEAN
IsOpromStorageDev (
  IN EFI_DEVICE_PATH_PROTOCOL      *DevicePath,
  IN OPROM_STORAGE_DEVICE_INFO     *OpromStorageDev,
  IN UINTN                         OpromStorageDevCount
  )
/*++

Routine Description:

  Based on the input oprom storage device table, check if the device path belongs to oprom storage device.

Arguments:

  DevicePath            - The list of device patch
  OpromStorageDev       - Pointer of array which contains oprom storage device information
  OpromStorageDevCount  - Number of oprom storage device

Returns:

  TRUE  - Device path belongs to oprom storage device.
  FALSE - otherwise.

--*/
{
  EFI_STATUS                         Status;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL    *PciRootBridgeIo;
  PCI_DEVICE_PATH                    *PciDevicePath;
  PCI_DEVICE_PATH                    *ParentPciDevicePath;
  PCI_DEVICE_PATH                    *NextPciDevicePath;
  EFI_DEVICE_PATH_PROTOCOL           *NextDevicePath;
  UINT8                              DevicePathBusNumber;
  UINT64                             PciAddress;
  UINTN                              Index;

  if (DevicePath == NULL || OpromStorageDev == NULL || OpromStorageDevCount == 0) {
    return FALSE;
  }

  Status = gBS->LocateProtocol (
                  &gEfiPciRootBridgeIoProtocolGuid,
                  NULL,
                  &PciRootBridgeIo
                  );
  if (EFI_ERROR(Status)) {
    return FALSE;
  }

  PciDevicePath = GetPciDevicePath (DevicePath);
  if (PciDevicePath == NULL) {
    return FALSE;
  }

  //
  // Get bus, device and function number form device path.
  //
  ParentPciDevicePath = NULL;
  DevicePathBusNumber = 0;
  while (TRUE) {
    NextDevicePath    = (EFI_DEVICE_PATH_PROTOCOL *) PciDevicePath;
    NextDevicePath    = NextDevicePathNode (NextDevicePath);
    NextPciDevicePath = GetPciDevicePath (NextDevicePath);
    if (NextPciDevicePath == NULL) {
      break;
    }
    //
    // If found next PCI Device Path, current Device Path is a P2P bridge
    //
    ParentPciDevicePath = PciDevicePath;
    PciDevicePath       = NextPciDevicePath;

    PciAddress = EFI_PCI_ADDRESS (
                   DevicePathBusNumber,
                   ParentPciDevicePath->Device,
                   ParentPciDevicePath->Function,
                   PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET
                   );

    Status = PciRootBridgeIo->Pci.Read (
                                    PciRootBridgeIo,
                                    EfiPciWidthUint8,
                                    PciAddress,
                                    1,
                                    &DevicePathBusNumber
                                    );
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }

  for (Index = 0; Index < OpromStorageDevCount; Index++) {
    if (DevicePathBusNumber     == OpromStorageDev[Index].Bus &&
        PciDevicePath->Device   == OpromStorageDev[Index].Device &&
        PciDevicePath->Function == OpromStorageDev[Index].Function) {
      return TRUE;
    }
  }

  return FALSE;
}

EFI_STATUS
BdsLibGetOpromStorageDevInfo (
  OUT OPROM_STORAGE_DEVICE_INFO **OpromStorageDev,
  OUT UINTN                     *OpromStorageDevCount
  )
/*++

Routine Description:

  Collect option ROM storage device information.

Arguments:

  OpromStorageDev       - Pointer of array which contains oprom storage device information
  OpromStorageDevCount  - Number of oprom storage device

Returns:

  EFI_SUCCESS           - Get Oprom storage device information successfully.
  EFI_INVALID_PARAMETER - Input parameter is NULL.
  otherwise             - LocateHandleBuffer or AllocatePool fail.

--*/
{
  EFI_STATUS                       Status;
  UINTN                            NumberPciIoHandles;
  EFI_HANDLE                       *PciIoHandles;
  EFI_PCI_IO_PROTOCOL              *PciIo;
  UINTN                            Segment;
  UINT8                            Class;
  UINTN                            Index;
  EFI_PCI_IO_PROTOCOL              *OpromStorageDevTable[MAX_OPTION_ROM_STORAGE_DEVICE];

  if (OpromStorageDev == NULL || OpromStorageDevCount == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *OpromStorageDev = NULL;
  *OpromStorageDevCount = 0;
  NumberPciIoHandles = 0;
  PciIoHandles = NULL;

  //
  // Find all storage devices which are controlled by option ROM.
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &NumberPciIoHandles,
                  &PciIoHandles
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  for (Index = 0; Index < NumberPciIoHandles; Index++) {
    Status = gBS->HandleProtocol (PciIoHandles[Index], &gEfiPciIoProtocolGuid, &PciIo);
     if (EFI_ERROR (Status) || PciIo->RomSize == 0) {
      continue;
    }

    Status = PciIo->Pci.Read (
                          PciIo,
                          EfiPciWidthUint8,
                          PCI_SUBCLASSCODE_OFFSET + 1,
                          1,
                          &Class
                          );
    if (EFI_ERROR (Status) || Class != PCI_CLASS_MASS_STORAGE) {
      continue;
    }

    if (*OpromStorageDevCount == MAX_OPTION_ROM_STORAGE_DEVICE) {
      DEBUG((EFI_D_ERROR, "WARNING: Unable to get all option ROM storage device information!\n"));
      break;
    }

    OpromStorageDevTable[*OpromStorageDevCount] = PciIo;
    (*OpromStorageDevCount)++;
  }

  if (NumberPciIoHandles) {
    gBS->FreePool (PciIoHandles);
  }

  //
  // From the table of oprom storage device, get info one by one.
  //
  if (*OpromStorageDevCount) {
    Status = gBS->AllocatePool (
                     EfiBootServicesData,
                    *OpromStorageDevCount * sizeof (OPROM_STORAGE_DEVICE_INFO),
                         OpromStorageDev
                         );
    if (EFI_ERROR (Status)) {
      *OpromStorageDevCount = 0;
      return Status;
    }

    for (Index = 0; Index < *OpromStorageDevCount; Index++) {
      PciIo = OpromStorageDevTable[Index];
      PciIo->GetLocation (
               PciIo,
               &Segment,
               &((*OpromStorageDev)[Index].Bus),
               &((*OpromStorageDev)[Index].Device),
               &((*OpromStorageDev)[Index].Function)
               );
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
DeleteBiosCreateOption (
  IN      UINTN       BootOrderIndex,
  IN      UINT16      PositionPolicy,
  IN OUT  UINT16      *BootOrder,
  IN OUT  UINTN       *BootOrderSize,
  OUT     UINT16       *BiosCreatedBootNum
  )
/*++

Routine Description:

  According input boot order index to delete BIOS created boot option which partition device path is same
  as input boot option. If input boot option is created by BIOS, this function will delete input boot option
  directly.

Arguments:

  BootOrderIndex        - Input boot order index.
  PositionPolicy        - The new created boot order policy.
  BootOrder             - Boot order list.
  BootOrderSize         - Boot order size by byte.
  BiosCreatedBootNum    - The boot option number which is created by BIOS.

Returns:

  EFI_SUCCESS           - BIOS created boot option is deleted and the delete boot order index is returned.
  EFI_INVALID_PARAMETER - BootOrder is NULL or BiosCreatedIndex is NULL or corresponding Boot#### is incorrect.
  EFI_NOT_FOUND         - Cannot find BIOS created boot option.
  EFI_OUT_OF_RESOURCES  - Lack of memory resource.

--*/
{
  UINT16                    BootOption[BOOT_OPTION_MAX_CHAR];
  UINT8                     *BootOptionVar;
  UINTN                     VariableSize;
  UINT16                    BootOrderNum;
  UINT16                    Index;
  EFI_DEVICE_PATH_PROTOCOL  *OptionDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *WorkingDevicePath;
  BOOLEAN                   BiosCreatedFound;

  if (BootOrder == NULL || BootOrderSize == NULL || BiosCreatedBootNum == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  SPrint (BootOption, sizeof (BootOption), L"Boot%04x", BootOrder[BootOrderIndex]);
  BootOptionVar = BdsLibGetVariableAndSize (
                    BootOption,
                    &gEfiGlobalVariableGuid,
                    &VariableSize
                    );
  if (BootOptionVar == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  BiosCreatedFound = FALSE;
  OptionDevicePath = NULL;
  if (BdsLibIsBiosCreatedOption (BootOptionVar, VariableSize)) {
    gBS->FreePool (BootOptionVar);
    *BiosCreatedBootNum = BootOrder[BootOrderIndex];
    BdsDeleteBootOption (BootOrder[BootOrderIndex], BootOrder, BootOrderSize);
    BiosCreatedFound = TRUE;
  } else {
    gBS->FreePool (BootOptionVar);
    OptionDevicePath = BdsLibGetDevicePathFromBootOption (BootOrder[BootOrderIndex]);
    if (OptionDevicePath == NULL) {
      return EFI_INVALID_PARAMETER;
    }
    BootOrderNum = (UINT16) (*BootOrderSize / sizeof (UINT16));
    for (Index = 0; Index < BootOrderNum; Index++) {
      if (Index == BootOrderIndex) {
        continue;
      }
      SPrint (BootOption, sizeof (BootOption), L"Boot%04x", BootOrder[Index]);
      BootOptionVar = BdsLibGetVariableAndSize (
                        BootOption,
                        &gEfiGlobalVariableGuid,
                        &VariableSize
                        );
      if (BootOptionVar == NULL || !BdsLibIsBiosCreatedOption (BootOptionVar, VariableSize)) {
        if (BootOptionVar != NULL) {
          gBS->FreePool (BootOptionVar);
        }
        continue;
      }

      WorkingDevicePath = BdsLibGetDevicePathFromBootOption (BootOrder[Index]);
      if (WorkingDevicePath == NULL) {
         gBS->FreePool (BootOptionVar);
         continue;
      }
      if (MatchPartitionDevicePathNode (OptionDevicePath, (HARDDRIVE_DEVICE_PATH *) WorkingDevicePath) &&
          BdsLibMatchFilePathDevicePathNode (OptionDevicePath, WorkingDevicePath)) {
        BiosCreatedFound = TRUE;
        *BiosCreatedBootNum = BootOrder[Index];
      }

      gBS->FreePool (BootOptionVar);
      gBS->FreePool (WorkingDevicePath);
      if (BiosCreatedFound) {
        SPrint (BootOption, sizeof (BootOption), L"Boot%04x", BootOrder[Index]);
        gRT->SetVariable (
               BootOption,
               &gEfiGlobalVariableGuid,
               EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
               0,
               NULL
               );
        if (PositionPolicy == IN_AUTO) {
          //
          // Keep OS created EFI Boot option prioiry, if NewPositionPolicy is auto
          //
          EfiCopyMem (
            &BootOrder[Index],
            BootOrder + Index + 1,
            *BootOrderSize - (Index + 1) * sizeof (UINT16)
            );
        } else {
          //
          // Keep recovery EFI Boot option prioiry, if NewPositionPolicy isn't auto
          //
          BootOrder[Index] = BootOrder[BootOrderIndex];
          EfiCopyMem (
            &BootOrder[BootOrderIndex],
            BootOrder + BootOrderIndex + 1,
            *BootOrderSize - (BootOrderIndex + 1) * sizeof (UINT16)
            );
        }
         *BootOrderSize -= sizeof (UINT16);
        break;
      }
    }
  }

  if (OptionDevicePath != NULL) {
    gBS->FreePool (OptionDevicePath);
  }

  return BiosCreatedFound ? EFI_SUCCESS : EFI_NOT_FOUND;
}


EFI_STATUS
BdsDeleteAllInvalidEfiBootOption (
  OUT BOOLEAN               *HaveUefiOs,
  OUT BOOLEAN               *WindowsToGoBootVarExist
  )
/*++

Routine Description:

  Delete all invalid EFI boot options. The probable invalid boot option could
  be Removable media or Network boot device.

Arguments:

Returns:

  EFI_SUCCESS           - Delete all invalid boot option success
  EFI_NOT_FOUND         - Variable "BootOrder" is not found
  EFI_OUT_OF_RESOURCES  - Lack of memory resource
  Other                 - Error return value from SetVariable()

--*/
{
  UINT16                        *BootOrder;
  UINT8                         *BootOptionVar;
  UINTN                         BootOrderSize;
  UINTN                         BootOptionSize;
  EFI_STATUS                    Status;
  UINTN                         Index;
  UINT16                        BootOption[BOOT_OPTION_MAX_CHAR];
  EFI_DEVICE_PATH_PROTOCOL      *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL      *LastDeviceNode;
  EFI_DEVICE_PATH_PROTOCOL      *OptionDevicePath;
  UINT8                         *TempPtr;
  CHAR16                        *Description;
  EFI_HANDLE                    Handle;
  BOOLEAN                       NeedDelete;
  EFI_DEVICE_PATH_PROTOCOL      *NewDevicePath;
  UINT16                        BiosCreatedBootNum;
  EFI_SETUP_UTILITY_PROTOCOL    *SetupUtility;
  SYSTEM_CONFIGURATION          *SystemConfiguration;
  EFI_FILE_HANDLE               FileHandle;
  UINTN                         Size;
  EFI_FIRMWARE_VOLUME_PROTOCOL  *Fv;
  EFI_FV_FILETYPE               Type;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINT32                        AuthenticationStatus;
  OPROM_STORAGE_DEVICE_INFO     *OpromStorageDev;
  UINTN                         OpromStorageDevCount;
  UINT8                         *DisableOpromStorageDevBoot;
  BOOLEAN                       UefiOsFound;
  UINTN                         VariableSize;
  UINTN                         OptionDevicePathSize;
  UINT8                         *OptionalData;
  UINT32                        HashValue;
#ifdef UEFI_NETWORK_SUPPORT
  BOOLEAN                       IsIPv4BootOption;
  BOOLEAN                       IsIPv6BootOption;
#endif //UEFI_NETWORK_SUPPORT

  Status        = EFI_SUCCESS;
  BootOrder     = NULL;
  BootOrderSize = 0;
  SystemConfiguration = NULL;
  UefiOsFound         = FALSE;
  *WindowsToGoBootVarExist = FALSE;

  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &BootOrderSize
                );
  if (NULL == BootOrder) {
    *HaveUefiOs = UefiOsFound;
    return EFI_NOT_FOUND;
  }

  Status = gBS->LocateProtocol (
                  &gEfiSetupUtilityProtocolGuid,
                  NULL,
                  &SetupUtility
                  );
  if (!EFI_ERROR (Status)) {
    SystemConfiguration = (SYSTEM_CONFIGURATION *)SetupUtility->SetupNvData;
  }

  OpromStorageDev = NULL;
  OpromStorageDevCount = 0;
  DisableOpromStorageDevBoot = BdsLibGetVariableAndSize (
                                 L"DisableOpromStorageDevBoot",
                                 &gEfiGenericVariableGuid,
                                 &Size
                                 );
  if (DisableOpromStorageDevBoot != NULL) {
    BdsLibGetOpromStorageDevInfo (&OpromStorageDev, &OpromStorageDevCount);
  }

  Index = 0;
  while (Index < BootOrderSize / sizeof (UINT16)) {
#ifdef AUTO_CREATE_DUMMY_BOOT_OPTION
    if (BdsLibIsDummyBootOption (BootOrder[Index])) {
      Index++;
      continue;
    }
#endif
    SPrint (BootOption, sizeof (BootOption), L"Boot%04x", BootOrder[Index]);
    BootOptionVar = BdsLibGetVariableAndSize (
                      BootOption,
                      &gEfiGlobalVariableGuid,
                      &VariableSize
                      );
    if (NULL == BootOptionVar) {
      //
      //If the Boot Device is not exit, we should dynamically adjust the BootOrder
      //
      BdsLibUpdateInvalidBootOrder (&BootOrder, Index, &BootOrderSize);
      continue;
    }

    TempPtr = BootOptionVar;
    TempPtr += sizeof (UINT32) + sizeof (UINT16);
    Description = (CHAR16 *) TempPtr;
    TempPtr += EfiStrSize ((CHAR16 *) TempPtr);
    OptionDevicePath = (EFI_DEVICE_PATH_PROTOCOL *) TempPtr;
    OptionDevicePathSize = EfiDevicePathSize (OptionDevicePath);
    BootOptionSize = (UINTN) (TempPtr - BootOptionVar) + OptionDevicePathSize;
    OptionalData = BootOptionVar + BootOptionSize;
    //
    // Skip legacy boot option (BBS boot device)
    //
    if ((DevicePathType (OptionDevicePath) == BBS_DEVICE_PATH) &&
        (DevicePathSubType (OptionDevicePath) == BBS_BBS_DP)) {
      gBS->FreePool (BootOptionVar);
      Index++;
      continue;
    }

    //
    // SCU disable EFI Boot
    //
    if (BdsLibGetBootType () == LEGACY_BOOT_TYPE) {
      BdsDeleteBootOption (BootOrder[Index], BootOrder, &BootOrderSize);
      gBS->FreePool (BootOptionVar);
      continue;
    }

    //
    // Skip USB class boot option
    //
    if ((DevicePathType (OptionDevicePath) == MESSAGING_DEVICE_PATH) &&
        (DevicePathSubType (OptionDevicePath) == MSG_USB_CLASS_DP)) {
      *WindowsToGoBootVarExist = TRUE;
      gBS->FreePool (BootOptionVar);
      Index++;
      continue;
    }
    //
    // If system doesn't support USB boot, delete all of USB EFI boot options.
    //
    if (SystemConfiguration != NULL && SystemConfiguration->UsbBoot != 0 && ISUsbDevicePath (OptionDevicePath)) {
      BdsDeleteBootOption (BootOrder[Index], BootOrder, &BootOrderSize);
      gBS->FreePool (BootOptionVar);
      continue;
    }

    TempDevicePath = OptionDevicePath;
    LastDeviceNode = OptionDevicePath;
    while (!EfiIsDevicePathEnd (TempDevicePath)) {
      LastDeviceNode = TempDevicePath;
      TempDevicePath = EfiNextDevicePathNode (TempDevicePath);
    }

    //
    // Skip boot option for internal Shell if read file successfully.
    //
    if (EfiGetNameGuidFromFwVolDevicePathNode ((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *) LastDeviceNode) != NULL) {
      TempDevicePath = OptionDevicePath;
      Status = gBS->LocateDevicePath (
                      &gEfiFirmwareVolumeProtocolGuid,
                      &TempDevicePath,
                      &Handle
                      );
      if (!EFI_ERROR (Status)) {
        gBS->HandleProtocol (
               Handle,
               &gEfiFirmwareVolumeProtocolGuid,
               (VOID **) &Fv
               );
        Status = Fv->ReadFile (
                       Fv,
                       &gEfiShellFileGuid,
                       NULL,
                       &Size,
                       &Type,
                       &Attributes,
                       &AuthenticationStatus
                       );
        if (!EFI_ERROR (Status)) {
          gBS->FreePool (BootOptionVar);
          Index++;
          continue;
        }
      }

      if (VariableSize - BootOptionSize == SHELL_OPTIONAL_DATA_SIZE) {
        gBS->CopyMem (&HashValue, &OptionalData[2], 4);
        Status = UpdateShellDevicePath ((CHAR16 *) BootOption, Description, HashValue);
        if (!EFI_ERROR (Status)) {
          gBS->FreePool (BootOptionVar);
          Index++;
          continue;
        }
      }

      BdsDeleteBootOption (BootOrder[Index], BootOrder, &BootOrderSize);
      gBS->FreePool (BootOptionVar);
      continue;
    }

    NeedDelete = TRUE;
    //
    // Check if it's a valid UEFI OS boot option
    //
    if ((DevicePathType (LastDeviceNode) == MEDIA_DEVICE_PATH) &&
        (DevicePathSubType (LastDeviceNode) == MEDIA_FILEPATH_DP)) {
      NewDevicePath = ExpandPartitionPartialDevicePathToFull (
                        (HARDDRIVE_DEVICE_PATH *)OptionDevicePath
                        );

      if (NewDevicePath == NULL) {
        NewDevicePath = OptionDevicePath;
      }

      if (OpromStorageDevCount != 0 &&
          IsOpromStorageDev (NewDevicePath, OpromStorageDev, OpromStorageDevCount)) {
        BdsDeleteBootOption (BootOrder[Index], BootOrder, &BootOrderSize);
        gBS->FreePool (BootOptionVar);
        if (NewDevicePath != OptionDevicePath) {
          gBS->FreePool (NewDevicePath);
        }
        continue;
      }

      Status = BdsLibOpenFileFromDevicePath (
                 NewDevicePath,
                 EFI_FILE_MODE_READ,
                 0,
                 &FileHandle
                 );
      if (NewDevicePath != OptionDevicePath) {
        gBS->FreePool (NewDevicePath);
      }

      if (!EFI_ERROR(Status)) {
        NeedDelete = FALSE;
        UefiOsFound = TRUE;
        FileHandle->Close (FileHandle);
      }
      gBS->FreePool (BootOptionVar);

      if (NeedDelete) {
        //
        // Only delete BIOS created boot option, doesn't delete OS created boot option, just don't display and boot these
        // boot options.
        //
        BdsDeleteBootOption (BootOrder[Index], BootOrder, &BootOrderSize);
        continue;
      }
      if (!NeedDelete && !BdsLibIsBiosCreatedOption (BootOptionVar, VariableSize)) {
        Status = DeleteBiosCreateOption (Index, SystemConfiguration->NewPositionPolicy, BootOrder, &BootOrderSize, &BiosCreatedBootNum);
        if (!EFI_ERROR (Status)) {
          if (EfiReadCmos8 (LastBootDevice) == (UINT8) BiosCreatedBootNum) {
            EfiWriteCmos8 (LastBootDevice, (UINT8) BootOrder[Index]);
          }
          continue;
        }
      }
      Index++;
      continue;
   }
    //
    // Check if it's a valid boot option for removable media
    //
    TempDevicePath = OptionDevicePath;
    Status = gBS->LocateDevicePath (
                    &gEfiSimpleFileSystemProtocolGuid,
                    &TempDevicePath,
                    &Handle
                    );
    if (!EFI_ERROR (Status) &&
        (OpromStorageDevCount == 0 ||
         !IsOpromStorageDev (OptionDevicePath, OpromStorageDev, OpromStorageDevCount))) {
      NeedDelete = FALSE;
    }

    //
    // Check if it's a valid boot option for network boot device
    //
    //
    // if PxeBootToLan is disable, delete all network boot option
    //
    if (SystemConfiguration != NULL && SystemConfiguration->PxeBootToLan == 1) {
      TempDevicePath = OptionDevicePath;
      Status = gBS->LocateDevicePath (
                      &gEfiLoadFileProtocolGuid,
                      &TempDevicePath,
                      &Handle
                      );
      //
      // Focus on removable media and network boot device
      //
      if (!EFI_ERROR (Status)) {
#ifndef UEFI_NETWORK_SUPPORT
        NeedDelete = FALSE;
#else //UEFI_NETWORK_SUPPORT

        IsIPv4BootOption = FALSE;
        IsIPv6BootOption = FALSE;

        //
        // Scan Build Option device path, if is IPv4 or IPv6 will trigger boolean
        //
        TempDevicePath = OptionDevicePath;
        while (!EfiIsDevicePathEnd (TempDevicePath)) {
          if (TempDevicePath->Type == MESSAGING_DEVICE_PATH) {
            if (TempDevicePath->SubType == MSG_IPv4_DP) {
              IsIPv4BootOption = TRUE;
            }
            else if (TempDevicePath->SubType == MSG_IPv6_DP) {
              IsIPv6BootOption = TRUE;
            }
          }
          TempDevicePath = EfiNextDevicePathNode (TempDevicePath);
        }

        //
        // Detect user set Network is IPv4/IPv6/Both
        // Will delete delete not selected item
        //
        switch (SystemConfiguration->NetworkProtocol) {
        case UEFI_NETWORK_BOOT_OPTION_IPV4:
          if (IsIPv4BootOption) {
            NeedDelete = FALSE;
          }
          break;

        case UEFI_NETWORK_BOOT_OPTION_IPV6:
          if (IsIPv6BootOption) {
            NeedDelete = FALSE;
          }
          break;

        case UEFI_NETWORK_BOOT_OPTION_BOTH:
          NeedDelete = FALSE;
        }

        //
        // If Boot option is vaild, will not delete
        // Include Unknown LoadFile build option
        //
        if (!IsIPv4BootOption & !IsIPv6BootOption) {
          NeedDelete = FALSE;
        }
#endif //UEFI_NETWORK_SUPPORT
      }
    }

    if (NeedDelete) {
      BdsDeleteBootOption (BootOrder[Index], BootOrder, &BootOrderSize);
      gBS->FreePool (BootOptionVar);
      continue;
    }

    gBS->FreePool (BootOptionVar);
    Index++;
  }

  if (SystemConfiguration != NULL) {
    AdjustBootOrder (
      !((BOOLEAN) SystemConfiguration->BootNormalPriority),
      BootOrderSize / sizeof (UINT16),
      BootOrder
      );
  }

  Status = gRT->SetVariable (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  BootOrderSize,
                  BootOrder
                  );

  gBS->FreePool (BootOrder);
  *HaveUefiOs = UefiOsFound;

  if (DisableOpromStorageDevBoot != NULL) {
    gBS->FreePool (DisableOpromStorageDevBoot);

    if (OpromStorageDevCount) {
      gBS->FreePool (OpromStorageDev);
    }
  }

  return Status;
}

EFI_STATUS
RegisterPortalbeBootOption (
  IN EFI_DEVICE_PATH_PROTOCOL    *DevicePath,
  IN OUT EFI_LIST_ENTRY          *BdsBootOptionList
 )
/*++

Routine Description:

  Add a BDS_COMMON_OPTION to portabl boot option list

Arguments:

  DevicePath        - A poitner to device path instance.
  BdsBootOptionList - The header of the link list which indexed all current boot options

Returns:

  EFI_SUCCESS - Finished all the boot device enumerate and create
                the boot option base on that boot device

--*/
{
  BDS_COMMON_OPTION         *Option;

  Option = EfiLibAllocateZeroPool (sizeof (BDS_COMMON_OPTION));
  if (Option == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Option->Signature   = BDS_LOAD_OPTION_SIGNATURE;
  Option->DevicePath  = EfiLibAllocateZeroPool (EfiDevicePathSize (DevicePath));
  EfiCopyMem (Option->DevicePath, DevicePath, EfiDevicePathSize (DevicePath));
  Option->Attribute   = 0;
  Option->Description = NULL;
  Option->LoadOptions = NULL;
  Option->LoadOptionsSize = 0;
  InsertTailList (BdsBootOptionList, &Option->Link);

  return EFI_SUCCESS;
}

BOOLEAN
BdsLibCheckDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath,
  IN UINT8                     QueryType,
  IN UINT8                     QuerySubType
  )
/*++

Routine Description:

  Check if the input device path contains the query type and subtype or not.

Arguments:

  DevicePath    -  Pointer to device path instance
  QueryType     -  Query type of device path
  QuerySubType  -  Query subtype of device path

Returns:

  TRUE       - DevicePath contains the query type and subtype.
  FALSE      - DevicePath doesn't contain the query type and subtype.

--*/
{
  BOOLEAN                      Match;
  EFI_DEVICE_PATH_PROTOCOL     *FullDevicePath;
  EFI_DEVICE_PATH_PROTOCOL     *WorkingDevicePath;

  Match = FALSE;

  if (DevicePath == NULL) {
    return FALSE;
  }

  FullDevicePath = ExpandPartitionPartialDevicePathToFull ((HARDDRIVE_DEVICE_PATH *) DevicePath);
  WorkingDevicePath = (FullDevicePath == NULL) ? DevicePath : FullDevicePath;

  while (!EfiIsDevicePathEnd (WorkingDevicePath)) {
    if (DevicePathType (WorkingDevicePath) == QueryType &&
        DevicePathSubType (WorkingDevicePath) == QuerySubType) {
      Match = TRUE;
      break;
    }

    WorkingDevicePath = EfiNextDevicePathNode (WorkingDevicePath);
  }

  if (FullDevicePath != NULL) {
    gBS->FreePool (FullDevicePath);
  }

  return Match;
}

BOOLEAN
HaveDefaultRemovableFile (
  IN   EFI_HANDLE         Handle
  )
/*++

Routine Description:

  Check if it has the default removable file or not.

Arguments:

  Handle                 - A pointer to a device handle.

Returns:

  TRUE  - It has the default removable file.
  FALSE - It does not has the default removable file.

--*/
{
  EFI_DEVICE_PATH_PROTOCOL      *FilePath;
  EFI_FILE_HANDLE               FileHandle;
  EFI_STATUS                    Status;

  FilePath = EfiFileDevicePath (Handle, DEFAULT_REMOVABLE_FILE_NAME);
  Status = BdsLibOpenFileFromDevicePath (
             FilePath,
             EFI_FILE_MODE_READ,
             0,
             &FileHandle
             );
  gBS->FreePool (FilePath);
  if (!EFI_ERROR(Status)) {
    FileHandle->Close (FileHandle);
    return TRUE;
  }

  return FALSE;
}

BOOLEAN
IsGenericUefiBootOs (
  IN  EFI_HANDLE                    Handle
  )
/*++

Routine Description:

  Check if it is a generic UEFI bootable OS or not.

Arguments:

  Handle                 - A pointer to a device handle.

Returns:

  TRUE  - This is a generic UEFI Bootable OS.
  FALSE - This is not a generic UEFI Bootable OS.

--*/
{
  UINTN                         MaxUefiOs;
  UINTN                         Index;
  UINTN                         EnvType;
  EFI_DEVICE_PATH_PROTOCOL      *FilePath;
  EFI_FILE_HANDLE               FileHandle;
  EFI_STATUS                    Status;

  MaxUefiOs = sizeof (mBdsGeneralUefiBootOs) / sizeof (BDS_GENERAL_UEFI_BOOT_ARRAY_OS);

  for(Index = 0; Index < MaxUefiOs; Index++) {
    for (EnvType = 0; EnvType < MAX_BIOS_ENVIRONMENT; EnvType++) {
      if (mBdsGeneralUefiBootOs[Index][EnvType].FilePathString != NULL) {
        FilePath = EfiFileDevicePath (Handle, mBdsGeneralUefiBootOs[Index][EnvType].FilePathString);
        Status = BdsLibOpenFileFromDevicePath (
                   FilePath,
                   EFI_FILE_MODE_READ,
                   0,
                   &FileHandle
                   );
        gBS->FreePool (FilePath);

        if (!EFI_ERROR(Status)) {
          FileHandle->Close (FileHandle);
          return TRUE;
        }
      }
    }
  }

  return FALSE;
}

EFI_STATUS
CreateGenericUefiBootOs (
  IN  EFI_HANDLE                    Handle,
  IN  EFI_LIST_ENTRY                *BdsBootOptionList
  )
/*++

Routine Description:

  Create generic UEFI bootable OS.

Arguments:

  Handle                 - A pointer to a device handle.
  DoRegisterNewOption    - Flag to decide to register a new boot option or not
  BdsBootOptionList      - The header of the link list which indexed all current boot options

Returns:

  EFI_SUCCESS            - Create generic UEFI OS success
  Other                  - Register new boot option fail

--*/
{
  UINTN                         MaxUefiOs;
  UINTN                         Index;
  UINTN                         EnvType;
  EFI_DEVICE_PATH_PROTOCOL      *FilePath;
  EFI_FILE_HANDLE               FileHandle;
  EFI_STATUS                    Status;
  EFI_STATUS                    RegisterStatus;
  UINT8                         *TempPtr;
  EFI_DEVICE_PATH_PROTOCOL      *TempFilePath;

  RegisterStatus = EFI_SUCCESS;
  MaxUefiOs = sizeof (mBdsGeneralUefiBootOs) / sizeof (BDS_GENERAL_UEFI_BOOT_ARRAY_OS);

  for(Index = 0; Index < MaxUefiOs; Index++) {
    for (EnvType = 0; EnvType < MAX_BIOS_ENVIRONMENT; EnvType++) {
      if (mBdsGeneralUefiBootOs[Index][EnvType].FilePathString == NULL) {
        continue;
      }
      FilePath = EfiFileDevicePath (Handle, mBdsGeneralUefiBootOs[Index][EnvType].FilePathString);
      Status = BdsLibOpenFileFromDevicePath (
                 FilePath,
                 EFI_FILE_MODE_READ,
                 0,
                 &FileHandle
                 );
      if (!EFI_ERROR(Status)) {
        //
        // If open generic OS loader file success, register a new boot option.
        //
        FileHandle->Close (FileHandle);

        TempPtr      = (UINT8 *) FilePath;
        TempFilePath = FilePath;

        while (!EfiIsDevicePathEnd (TempFilePath) &&
               (TempFilePath->Type != MEDIA_DEVICE_PATH || TempFilePath->SubType != MEDIA_HARDDRIVE_DP)) {
          TempPtr += EfiDevicePathNodeLength (TempFilePath);
          TempFilePath = (EFI_DEVICE_PATH_PROTOCOL *) TempPtr;
        }
        if (EfiIsDevicePathEnd (TempFilePath)) {
          gBS->FreePool (FilePath);
          continue;
        }

        Status = BdsLibRegisterNewOption (
                   BdsBootOptionList,
                   TempFilePath,
                   mBdsGeneralUefiBootOs[Index][EnvType].Description,
                   L"BootOrder",
                   "RC",
                   2
                   );
        if (EFI_ERROR(Status)) {
          ASSERT_EFI_ERROR (Status);
          RegisterStatus = Status;
        }
        gBS->FreePool (FilePath);
        break;
      }

      gBS->FreePool (FilePath);
    }
  }

  return RegisterStatus;
}

BOOLEAN
IsSignedImage (
  IN EFI_HANDLE                         Handle,
  IN CHAR16                             *FileName
  )
/*++

Routine Description:

  Check if the target file is signed image or not

Arguments:

  Handle     - A pointer to a device handle.
  FileName   - Pointer to file name

Returns:

  TRUE  - This is a signed image
  FALSE - The file isn't exist, the image is not supported or this isn't a signed image

--*/
{
  EFI_STATUS                            Status;
  EFI_IMAGE_DOS_HEADER                  DosHeader;
  EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION   Hdr;
  EFI_IMAGE_OPTIONAL_HEADER_UNION       HdrData;
  UINT16                                Magic;
  EFI_IMAGE_DATA_DIRECTORY              *SecDataDir;


  Hdr.Union = &HdrData;
  Status = BdsLibGetImageHeader (
             Handle,
             FileName,
             &DosHeader,
             Hdr
             );
  if (EFI_ERROR (Status) || !EFI_IMAGE_MACHINE_TYPE_SUPPORTED (Hdr.Pe32->FileHeader.Machine)) {
    return FALSE;
  }

  if (Hdr.Pe32->FileHeader.Machine == EFI_IMAGE_MACHINE_IA64 && Hdr.Pe32->OptionalHeader.Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
    Magic = EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC;
  } else {
    Magic = Hdr.Pe32->OptionalHeader.Magic;
  }

  if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
    //
    // Use PE32 offset.
    //
    SecDataDir = (EFI_IMAGE_DATA_DIRECTORY *) &Hdr.Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY];
  } else {
    //
    // Use PE32+ offset.
    //
    SecDataDir = (EFI_IMAGE_DATA_DIRECTORY *) &Hdr.Pe32Plus->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY];
  }

  if (SecDataDir->Size != 0) {
    return TRUE;
  }

  return FALSE;
}

BOOLEAN
IsWindowsToGo (
  IN   EFI_HANDLE         Handle
  )
/*++

Routine Description:

  This function uses device handle to check the EFI boot option is Windows To Go device or not

Arguments:

  Handle     - A pointer to a device handle.

Returns:

  TRUE  - This is a Windows To Go device
  FALSE - This isn't a Windows To Go device

--*/
{
  EFI_DEVICE_PATH_PROTOCOL    *DevicePath;
  UINTN                       DevicePathType;

  DevicePath     = EfiDevicePathFromHandle (Handle);
  DevicePathType = BdsLibGetBootTypeFromDevicePath (DevicePath);

  if (DevicePathType == BDS_EFI_MESSAGE_USB_DEVICE_BOOT &&
      IsSignedImage (Handle, DEFAULT_REMOVABLE_FILE_NAME)) {
    return TRUE;
  }

  return FALSE;
}

EFI_STATUS
DeleteRecoveryOption (
  IN EFI_DEVICE_PATH_PROTOCOL      *DevicePath
  )
/*++

Routine Description:

  Delete boot option which device path and input device path is matched

Arguments:

  EFI_DEVICE_PATH_PROTOCOL    - Pointer to device path instance


Returns:

  EFI_SUCCESS - Finished all the boot device enumerate and create
                the boot option base on that boot device

--*/
{
  UINT16                    *BootOrder;
  UINT8                     *BootOptionVar;
  UINTN                     BootOrderSize;
  UINTN                     BootOptionSize;
  EFI_STATUS                Status;
  UINTN                     Index;
  UINT16                    BootOption[BOOT_OPTION_MAX_CHAR];
  EFI_DEVICE_PATH_PROTOCOL  *OptionDevicePath;
  UINT8                     *TempPtr;
  CHAR16                    *Description;
  UINTN                     DevicePathLen;

  Status        = EFI_SUCCESS;
  BootOrder     = NULL;
  BootOrderSize = 0;

  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &BootOrderSize
                );
  if (NULL == BootOrder) {
    return EFI_NOT_FOUND;
  }

  Index = 0;
  while (Index < BootOrderSize / sizeof (UINT16)) {
    SPrint (BootOption, sizeof (BootOption), L"Boot%04x", BootOrder[Index]);
    BootOptionVar = BdsLibGetVariableAndSize (
                      BootOption,
                      &gEfiGlobalVariableGuid,
                      &BootOptionSize
                      );
    if (NULL == BootOptionVar) {
      //
      //If the Boot Device is not exit, we should dynamically adjust the BootOrder
      //
      BdsLibUpdateInvalidBootOrder (&BootOrder, Index, &BootOrderSize);
      continue;
    }

    TempPtr = BootOptionVar;
    TempPtr += sizeof (UINT32);
    DevicePathLen = (UINTN) (*((UINT16 *) TempPtr));
    TempPtr += sizeof (UINT16);
    Description = (CHAR16 *) TempPtr;
    TempPtr += EfiStrSize ((CHAR16 *) TempPtr);
    OptionDevicePath = (EFI_DEVICE_PATH_PROTOCOL *) TempPtr;

    if (DevicePathLen == EfiDevicePathSize (DevicePath) && EfiCompareMem (OptionDevicePath, DevicePath, DevicePathLen) == 0) {
      BdsDeleteBootOption (BootOrder[Index], BootOrder, &BootOrderSize);
      gBS->FreePool (BootOptionVar);
      continue;
    }
    gBS->FreePool (BootOptionVar);
    Index++;
  }

  Status = gRT->SetVariable (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  BootOrderSize,
                  BootOrder
                  );

  gBS->FreePool (BootOrder);
  return Status;
}

EFI_STATUS
EnumerateAllWindowsToGoDevice (
  OUT UINT16                    *DeviceNum
  )
/*++

Routine Description:

  Enumernate all Windows To Go devices and return device number.

Arguments:

  DeviceNum      - Pointer to device number

Returns:

  EFI_SUCCESS    - Enumernate all Windows To Go devices success
  EFI_NOT_FOUND  - Windows To Go device is not found

--*/
{
  UINTN                         Index;
  UINTN                         NumberFileSystemHandles;
  EFI_HANDLE                    *FileSystemHandles;
  EFI_DEVICE_PATH_PROTOCOL      *FilePath;
  UINT16                        Count;

  Count                   = 0;
  NumberFileSystemHandles = 0;

  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiSimpleFileSystemProtocolGuid,
         NULL,
         &NumberFileSystemHandles,
         &FileSystemHandles
         );
  for (Index = 0; Index < NumberFileSystemHandles; Index++) {
    if (IsWindowsToGo (FileSystemHandles[Index])) {
      //
      // Add to Windows To Go list if this is USB Entry for Windows To Go
      //
      FilePath = EfiFileDevicePath (FileSystemHandles[Index], DEFAULT_REMOVABLE_FILE_NAME);
      RegisterPortalbeBootOption (FilePath, &mWindowsToGoDeviceList);
      gBS->FreePool (FilePath);
      Count++;
    }
  }

  if (NumberFileSystemHandles != 0) {
    gBS->FreePool (FileSystemHandles);
  }

  if (Count == 0) {
    return EFI_NOT_FOUND;
  }

  *DeviceNum = Count;

  return EFI_SUCCESS;
}

EFI_STATUS
SetWindowsToGoVariable (
  IN UINT16                     DeviceNum
  )
/*++

Routine Description:

  Set Windows To Go variable

Arguments:

  DeviceNum             - Windows To Go device number

Returns:

  EFI_SUCCESS           - Set Windows To Go device variable success
  EFI_NOT_FOUND         - Windows To Go device is not found
  EFI_OUT_OF_RESOURCES  - Allocate memory fail

--*/
{
  EFI_STATUS                    Status;
  BDS_COMMON_OPTION             *Option;
  UINT8                         *Ptr;
  UINT8                         *VarData;
  UINTN                         Size;
  UINTN                         DevPathSize;

  if (IsListEmpty (&mWindowsToGoDeviceList)) {
    return EFI_NOT_FOUND;
  }

  Option      = CR (mWindowsToGoDeviceList.ForwardLink, BDS_COMMON_OPTION, Link, BDS_LOAD_OPTION_SIGNATURE);
  DevPathSize = EfiDevicePathSize (Option->DevicePath);
  Size        = DevPathSize + sizeof(UINT16);
  VarData     = EfiLibAllocatePool (Size);
  if (VarData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Ptr = VarData;
  EfiCopyMem (Ptr, Option->DevicePath, DevPathSize);
  Ptr += DevPathSize;
  EfiCopyMem (Ptr, &DeviceNum, sizeof(UINT16));
  Status = gRT->SetVariable (
                  L"WindowsToGo",
                  &gEfiGenericVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS,
                  Size,
                  VarData
                  );
  gBS->FreePool (VarData);

  return Status;
}

VOID
UpdateWindowsToGoList (
  VOID
  )
/*++

Routine Description:

  Enumernate all Windows To Go devices and update them to list.

Arguments:

  None.

Returns:

  None.

--*/
{
  EFI_STATUS                    Status;
  UINT16                        DeviceNum;

  Status = EnumerateAllWindowsToGoDevice (&DeviceNum);
  if (!EFI_ERROR (Status)) {
    SetWindowsToGoVariable (DeviceNum);
  }
}

STATIC
BOOLEAN
IsEfiSystemPartition (
  EFI_HANDLE       Handle
  )
/*++

Routine Description:

  According to handle to check this handle is whether contains EFI system partition.

Arguments:

  Handle    -  The handle for the protocol interface that is being opened.

Returns:

  TRUE      -  This handle contains EFI system partition.
  FALSE     -  This handle doesn't contain EFI system partition.

--*/
{
  EFI_STATUS      Status;

  Status = gBS->OpenProtocol (
                  Handle,
                  &gEfiPartTypeSystemPartGuid,
                  NULL,
                  NULL,
                  NULL,
                  EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                  );
  return Status == EFI_SUCCESS ? TRUE : FALSE;
}


EFI_STATUS
BdsLibEnumerateAllBootOption (
  IN     BOOLEAN           ForceEnumerateAll,
  IN OUT EFI_LIST_ENTRY      *BdsBootOptionList
  )
/*++

Routine Description:

  This function will enumerate all possible boot device in the system,
  it will only excute once of every boot.

Arguments:

  FullEnumerate     - A bool value to indicate want to full enumarate all boot options
                      or partial enumarate all boot options.
  BdsBootOptionList - The header of the link list which indexed all current boot options


Returns:

  EFI_SUCCESS - Finished all the boot device enumerate and create
                the boot option base on that boot device

--*/
{
  EFI_STATUS                    Status;
  UINTN                         NumberFileSystemHandles;
  EFI_HANDLE                    *FileSystemHandles;
  EFI_BLOCK_IO_PROTOCOL         *BlkIo;
  UINTN                         Index;
#ifndef UEFI_NETWORK_SUPPORT
  UINTN                         NumberLoadFileHandles;
  EFI_HANDLE                    *LoadFileHandles;
  VOID                          *ProtocolInstance;
#endif
  EFI_FIRMWARE_VOLUME_PROTOCOL  *Fv;
  UINTN                         FvHandleCount;
  EFI_HANDLE                    *FvHandleBuffer;
  EFI_FV_FILETYPE               Type;
  UINTN                         Size;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINT32                        AuthenticationStatus;
  UINT16                        FloppyNumber;
  UINT16                        CdromNumber;
  UINT16                        UsbNumber;
  UINT16                        ScsiNumber;
  UINT16                        IScsiNumber;
  UINT16                        MiscNumber;
  UINT16                        HddNumber;
  UINT16                        InternalShellNumber;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;
  UINTN                         DevicePathType;
  CHAR16                        Buffer[128];
  EFI_SETUP_UTILITY_PROTOCOL    *SetupUtility;
  SYSTEM_CONFIGURATION          *SystemConfiguration;
  OPROM_STORAGE_DEVICE_INFO     *OpromStorageDev;
  UINTN                         OpromStorageDevCount;
  UINT8                         *DisableOpromStorageDevBoot;
  BOOLEAN                       UefiOsFound;
  BOOLEAN                       WindowsToGoBootVarExist;
  STATIC BOOLEAN                Win8FastBootEnum = TRUE;
  STATIC BOOLEAN                DeletedInvalidBootOption = FALSE;
  HARDWARE_BOOT_DEVICE_INFO     HwBootDeviceInfo[MAX_BOOT_DEVICES_NUMBER];
  UINTN                         HwBootDeviceCount;
  UINTN                         HwNum;
  BOOLEAN                       IsCreated;
  STATIC BOOLEAN                EnableBootOrderHook = FALSE;

  POSTCODE (BDS_ENUMERATE_ALL_BOOT_OPTION); //PostCode = 27, Get boot device information

  FloppyNumber = 0;
  CdromNumber = 0;
  UsbNumber = 0;
  ScsiNumber = 0;
  IScsiNumber = 0;
  MiscNumber = 0;
  HddNumber = 0;
  InternalShellNumber = 0;

  if (!DeletedInvalidBootOption) {
    BdsLibDeleteInvalidBootOptions ();
    DeletedInvalidBootOption = TRUE;
  }

  //
  // If the boot device enumerate happened, just get the boot
  // device from the boot order variable
  //
  if (mEnumBootDevice && !ForceEnumerateAll) {
    BdsLibBuildOptionFromVar (BdsBootOptionList, L"BootOrder");
    return EFI_SUCCESS;
  }
  //
  // Notes: this dirty code is to get the legacy boot option from the
  // BBS table and create to variable as the EFI boot option, it should
  // be removed after the CSM can provide legacy boot option directly
  //
  if (BdsLibGetBootType () != EFI_BOOT_TYPE) {
    REFRESH_LEGACY_BOOT_OPTIONS;
    if (BdsLibGetBootType () == LEGACY_BOOT_TYPE) {
      BdsLibRemovedBootOption (FALSE);
    }
  } else {
    BdsLibRemovedBootOption (TRUE);
  }
#ifdef AUTO_CREATE_DUMMY_BOOT_OPTION
  if (BdsLibGetBootType () != LEGACY_BOOT_TYPE) {
    CreateDummyBootOptions ();
    SyncBootOrder ();
  }
#endif

  BdsLibDeleteRedundantOption ();

  if (BdsLibIsWin8FastBootActive () && Win8FastBootEnum) {
    //
    // First enumernate is for Win 8 Fast Boot, just skip it for first time.
    //
    Win8FastBootEnum = FALSE;
    return EFI_SUCCESS;
  }


  if (BdsLibGetBootType () != LEGACY_BOOT_TYPE && !EnableBootOrderHook) {
    BdsLibEnableBootOrderHook ();
    EnableBootOrderHook = TRUE;
  }

  //
  // Delete invalid boot option
  //
  BdsDeleteAllInvalidEfiBootOption (&UefiOsFound, &WindowsToGoBootVarExist);

  //
  // SCU disable EFI Boot
  //
  if (BdsLibGetBootType () == LEGACY_BOOT_TYPE) {
    BdsLibBuildOptionFromVar (BdsBootOptionList, L"BootOrder");
    mEnumBootDevice = TRUE;
    return EFI_SUCCESS;
  }

  InitializeListHead (&mWindowsToGoDeviceList);

  if (WindowsToGoBootVarExist) {
    UpdateWindowsToGoList ();
  }

  SystemConfiguration = NULL;
  Status = gBS->LocateProtocol (
                  &gEfiSetupUtilityProtocolGuid,
                  NULL,
                  &SetupUtility
                  );
  if (!EFI_ERROR (Status)) {
    SystemConfiguration = (SYSTEM_CONFIGURATION *) SetupUtility->SetupNvData;
  }

  OpromStorageDev = NULL;
  OpromStorageDevCount = 0;
  DisableOpromStorageDevBoot = BdsLibGetVariableAndSize (
                                 L"DisableOpromStorageDevBoot",
                                 &gEfiGenericVariableGuid,
                                 &Size
                                 );
  if (DisableOpromStorageDevBoot != NULL) {
    BdsLibGetOpromStorageDevInfo (&OpromStorageDev, &OpromStorageDevCount);
  }

  HwBootDeviceCount = 0;
  BdsLibGetAllHwBootDeviceInfo (&HwBootDeviceCount, HwBootDeviceInfo);

  //
  // Parse removable media
  //
  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiSimpleFileSystemProtocolGuid,
         NULL,
         &NumberFileSystemHandles,
         &FileSystemHandles
         );
  for (Index = 0; Index < NumberFileSystemHandles; Index++) {
    //
    // If need, skip the option ROM storage device.
    //
    if (OpromStorageDevCount != 0) {
      Status = gBS->HandleProtocol (
                      FileSystemHandles[Index],
                      &gEfiDevicePathProtocolGuid,
                      (VOID *) &DevicePath
                      );
      if (!EFI_ERROR (Status) &&
          IsOpromStorageDev (DevicePath, OpromStorageDev, OpromStorageDevCount)) {
        continue;
      }
    }
    //
    // If system doesn't support USB boot, needn't add USB EFI boot option
    //
    if (SystemConfiguration != NULL && SystemConfiguration->UsbBoot != 0 &&
        ISUsbDevicePath (EfiDevicePathFromHandle (FileSystemHandles[Index]))) {
      continue;
    }

    Status = gBS->HandleProtocol (
                    FileSystemHandles[Index],
                    &gEfiBlockIoProtocolGuid,
                    (VOID **) &BlkIo
                    );
    if (!EFI_ERROR (Status)) {
      DevicePath  = EfiDevicePathFromHandle (FileSystemHandles[Index]);
      DevicePathType = BdsLibGetBootTypeFromDevicePath (DevicePath);
      //
      // If the file system handle supports a BlkIo protocol, skip the removable media devices
      //
      if (!BlkIo->Media->RemovableMedia &&
          DevicePathType != BDS_EFI_MESSAGE_USB_DEVICE_BOOT &&
          BdsLibCheckDevicePath (DevicePath, MEDIA_DEVICE_PATH, MEDIA_HARDDRIVE_DP)) {
        if (!IsEfiSystemPartition (FileSystemHandles[Index])) {
          continue;
        }

        //
        // If find UEFI OS option in unremovable device, delete exist recovery boot option.
        // If cannot find UEFI OS option, try to find default EFI boot option as recovery boot option.
        //
        if (IsGenericUefiBootOs (FileSystemHandles[Index])) {
          CreateGenericUefiBootOs (FileSystemHandles[Index], BdsBootOptionList);
          DeleteRecoveryOption (DevicePath);
          continue;
        }
      }
    }

    if (WindowsToGoBootVarExist && IsWindowsToGo (FileSystemHandles[Index])) {
      BdsLibDeleteOptionFromHandle (FileSystemHandles[Index]);
      continue;
    }

    //
    // Do the removable Media thing. \EFI\BOOT\boot{machinename}.EFI
    // machinename is ia32, ia64, x64, ...
    //
    if (!HaveDefaultRemovableFile (FileSystemHandles[Index])) {
      //
      // No such file or the file is not a EFI application, delete this boot option
      //
      BdsLibDeleteOptionFromHandle (FileSystemHandles[Index]);
    } else {
      DevicePath  = EfiDevicePathFromHandle (FileSystemHandles[Index]);
      DevicePathType = BdsLibGetBootTypeFromDevicePath (DevicePath);

      IsCreated = FALSE;
      //
      // Get hardware device name for current boot option.
      //
      for (HwNum = 0; HwNum < HwBootDeviceCount; HwNum++) {
        if (BdsLibCompareBlockIoDevicePath (HwBootDeviceInfo[HwNum].BlockIoDevicePath, DevicePath)) {
          break;
        }
      }

      switch (DevicePathType) {

      case BDS_EFI_ACPI_FLOPPY_BOOT:
        if (FloppyNumber == 0) {
          SPrint (Buffer, sizeof (Buffer), L"%s", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_FLOPPY)));
        } else {
          SPrint (Buffer, sizeof (Buffer), L"%s %d", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_FLOPPY)), FloppyNumber);
        }
        FloppyNumber++;
        break;

      //
      // Assume a removable SATA device should be the DVD/CD device
      //
      case BDS_EFI_MESSAGE_SATA_BOOT:
      case BDS_EFI_MESSAGE_ATAPI_BOOT:
      case BDS_EFI_MEDIA_CDROM_BOOT:
        if (CdromNumber == 0) {
          SPrint (Buffer, sizeof (Buffer), L"%s", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_CD_DVD)));
        } else {
          SPrint (Buffer, sizeof (Buffer), L"%s %d", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_CD_DVD)), CdromNumber);
        }
        CdromNumber++;
        break;

      case BDS_EFI_MESSAGE_USB_DEVICE_BOOT:
        if (UsbNumber == 0) {
          SPrint (Buffer, sizeof (Buffer), L"%s", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_USB)));
        } else {
          SPrint (Buffer, sizeof (Buffer), L"%s %d", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_USB)), UsbNumber);
        }
        UsbNumber++;
        break;

      case BDS_EFI_MESSAGE_ISCSI_BOOT:
        if (IScsiNumber == 0) {
          SPrint (Buffer, sizeof (Buffer), L"%s", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_ISCSI)));
        } else {
          SPrint (Buffer, sizeof (Buffer), L"%s %d", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_ISCSI)), IScsiNumber);
        }
        IScsiNumber++;
        break;

      case BDS_EFI_MESSAGE_SCSI_BOOT:
        if (ScsiNumber == 0) {
          SPrint (Buffer, sizeof (Buffer), L"%s", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_SCSI)));
        } else {
          SPrint (Buffer, sizeof (Buffer), L"%s %d", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_SCSI)), ScsiNumber);
        }
        ScsiNumber++;
        break;

      case BDS_EFI_MESSAGE_MISC_BOOT:
        if (MiscNumber == 0) {
          SPrint (Buffer, sizeof (Buffer), L"%s", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_MISC)));
        } else {
          SPrint (Buffer, sizeof (Buffer), L"%s %d", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_MISC)), MiscNumber);
        }
        MiscNumber++;
        break;

      case BDS_EFI_MEDIA_HD_BOOT:
        if (HddNumber == 0) {
          SPrint (Buffer, sizeof (Buffer), L"%s", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_HARDDRIVE)));
        } else {
          SPrint (Buffer, sizeof (Buffer), L"%s %d", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_HARDDRIVE)), HddNumber);
        }
        HddNumber++;
        break;

      default:
        BdsLibBuildOptionFromHandle (FileSystemHandles[Index], BdsBootOptionList, NULL);
        IsCreated = TRUE;
        break;
      }

      if (!IsCreated) {
        if (HwNum == HwBootDeviceCount) {
          BdsLibBuildOptionFromHandle (FileSystemHandles[Index], BdsBootOptionList, Buffer);
        } else {
          SPrint (Buffer, sizeof(Buffer), L"%s (%s)", Buffer, HwBootDeviceInfo[HwNum].HwDeviceName);
          BdsLibBuildOptionFromHandle (FileSystemHandles[Index], BdsBootOptionList, Buffer);
        }
      }
    }
  }

  if (HwBootDeviceCount != 0) {
    for (Index = 0; Index < HwBootDeviceCount; Index++) {
      gBS->FreePool (HwBootDeviceInfo[Index].HwDeviceName);
    }
  }

  if (NumberFileSystemHandles) {
    gBS->FreePool (FileSystemHandles);
  }
  if (DisableOpromStorageDevBoot != NULL) {
    gBS->FreePool (DisableOpromStorageDevBoot);

    if (OpromStorageDevCount) {
      gBS->FreePool (OpromStorageDev);
    }
  }

  //
  // Parse Network Boot Device
  //
  if(SystemConfiguration != NULL && SystemConfiguration->PxeBootToLan != 0) {
#ifdef UEFI_NETWORK_SUPPORT
    BuildNetworkBootOption (BdsBootOptionList);
#else
    gBS->LocateHandleBuffer (
          ByProtocol,
          &gEfiSimpleNetworkProtocolGuid,
          NULL,
          &NumberLoadFileHandles,
          &LoadFileHandles
          );
    for (Index = 0; Index < NumberLoadFileHandles; Index++) {
      Status = gBS->HandleProtocol (
                      LoadFileHandles[Index],
                      &gEfiLoadFileProtocolGuid,
                      (VOID **) &ProtocolInstance
                      );
      if (EFI_ERROR (Status)) {
        continue;
      }

      if (Index == 0) {
        SPrint (Buffer, sizeof (Buffer), L"%s", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_NETWORK)));
      } else {
        SPrint (Buffer, sizeof (Buffer), L"%s %d", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_NETWORK)), Index);
      }
      BdsLibBuildOptionFromHandle (LoadFileHandles[Index], BdsBootOptionList, Buffer);
    }

    if (NumberLoadFileHandles) {
      gBS->FreePool (LoadFileHandles);
    }
#endif
  }

  //
  // Check if we have on flash shell
  //
  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiFirmwareVolumeProtocolGuid,
         NULL,
         &FvHandleCount,
         &FvHandleBuffer
         );
  for (Index = 0; Index < FvHandleCount; Index++) {
    gBS->HandleProtocol (
          FvHandleBuffer[Index],
          &gEfiFirmwareVolumeProtocolGuid,
          (VOID **) &Fv
          );

    Status = Fv->ReadFile (
                  Fv,
                  &gEfiShellFileGuid,
                  NULL,
                  &Size,
                  &Type,
                  &Attributes,
                  &AuthenticationStatus
                  );
    if (EFI_ERROR (Status)) {
      //
      // Skip if no shell file in the FV
      //
      continue;
    }
    //
    // Build the shell boot option
    //
    if (InternalShellNumber == 0) {
      SPrint (Buffer, sizeof (Buffer), L"%s", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_SHELL)));
    } else {
      SPrint (Buffer, sizeof (Buffer), L"%s %d", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_SHELL)), InternalShellNumber);
    }
    InternalShellNumber++;
    BdsLibBuildOptionFromShell (FvHandleBuffer[Index], BdsBootOptionList, Buffer);
  }

  if (FvHandleCount) {
    gBS->FreePool (FvHandleBuffer);
  }

  //
  // Make sure every boot only have one time boot device enumerate
  //
  BdsLibBuildOptionFromVar (BdsBootOptionList, L"BootOrder");

  if (SystemConfiguration->Win8FastBoot == 0 &&
      BdsLibGetBootType () == EFI_BOOT_TYPE) {
    UpdateTargetHddVariable ();
  }
#ifdef AUTO_CREATE_DUMMY_BOOT_OPTION
  BdsLibSyncPhysicalBootOrder ();
#endif
  mEnumBootDevice = TRUE;

  return EFI_SUCCESS;
}

VOID
BdsLibBuildOptionFromHandle (
  IN  EFI_HANDLE                 Handle,
  IN  EFI_LIST_ENTRY             *BdsBootOptionList,
  IN  CHAR16                     *String
  )
/*++

Routine Description:

  Build the boot option with the handle parsed in

Arguments:

  Handle            - The handle which present the device path to create boot option
  BdsBootOptionList - The header of the link list which indexed all current boot options

Returns:

--*/
{
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  CHAR16                    *TempString;

  DevicePath  = EfiDevicePathFromHandle (Handle);
  if (String == NULL) {
    TempString  = DevicePathToStr (DevicePath);
  } else {
    TempString = String;
  }

  //
  // Create and register new boot option
  //
  BdsLibRegisterNewOption (BdsBootOptionList, DevicePath, TempString, L"BootOrder", "RC", 2);
}

VOID
BdsLibBuildOptionFromShell (
  IN EFI_HANDLE                  Handle,
  IN OUT EFI_LIST_ENTRY          *BdsBootOptionList,
  IN CHAR16                      *Description
  )
/*++

Routine Description:

  Build the on flash shell boot option with the handle parsed in

Arguments:

  Handle            - The handle which present the device path to create on flash shell boot option
  BdsBootOptionList - The header of the link list which indexed all current boot options
  Description       - Description of EFI shell boot option

Returns:

  None.

--*/
{
  EFI_DEVICE_PATH_PROTOCOL          *DevicePath;
  MEDIA_FW_VOL_FILEPATH_DEVICE_PATH ShellNode;
  EFI_STATUS                        Status;
  UINT32                            HashValue;
  UINT8                             OptionalData[SHELL_OPTIONAL_DATA_SIZE];

  DevicePath = EfiDevicePathFromHandle (Handle);

  //
  // Build the shell device path
  //
  EfiInitializeFwVolDevicepathNode (&ShellNode, &gEfiShellFileGuid);
  DevicePath = EfiAppendDevicePathNode (DevicePath, (EFI_DEVICE_PATH_PROTOCOL *) &ShellNode);

  if (IsAlreadyInBootOrder (DevicePath)) {
    gBS->FreePool (DevicePath);
    return;
  }

  Status = GetShellFileHashValue (Handle, &HashValue);
  if (EFI_ERROR(Status)) {
    BdsLibRegisterNewOption (BdsBootOptionList, DevicePath, Description, L"BootOrder", "RC", 2);
    gBS->FreePool (DevicePath);
    return;
  }

  gBS->CopyMem (OptionalData    , "RC"                , 2);
  gBS->CopyMem (&OptionalData[2], (UINT8 *) &HashValue, 4);

  //
  // Create and register the shell boot option
  //
  BdsLibRegisterNewOption (BdsBootOptionList, DevicePath, Description, L"BootOrder", OptionalData, SHELL_OPTIONAL_DATA_SIZE);

  gBS->FreePool (DevicePath);
}

EFI_STATUS
BdsLibBootNext (
  VOID
  )
/*++

Routine Description:

  Boot from the EFI1.1 spec defined "BootNext" variable

Arguments:

  None.

Returns:

  None.

--*/
{
  UINT16            *BootNext;
  UINTN             BootNextSize;
  CHAR16            Buffer[20];
  BDS_COMMON_OPTION *BootOption;
  EFI_LIST_ENTRY    TempList;
  UINTN             ExitDataSize;
  CHAR16            *ExitData;
  EFI_STATUS        Status;

  //
  // Init the boot option name buffer and temp link list
  //
  InitializeListHead (&TempList);
  EfiZeroMem (Buffer, sizeof (Buffer));

  BootNext = BdsLibGetVariableAndSize (
               L"BootNext",
               &gEfiGlobalVariableGuid,
               &BootNextSize
               );

  //
  // Clear the boot next variable first
  //
  Status = EFI_NOT_FOUND;
  if (BootNext != NULL) {
    gRT->SetVariable (
          L"BootNext",
          &gEfiGlobalVariableGuid,
          EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
          0,
          BootNext
          );

    //
    // Start to build the boot option and try to boot
    //
    SPrint (Buffer, sizeof (Buffer), L"Boot%04x", *BootNext);
    BootOption = BdsLibVariableToOption (&TempList, Buffer);
    BdsLibConnectDevicePath (BootOption->DevicePath);
    Status = BdsLibBootViaBootOption (BootOption, BootOption->DevicePath, &ExitDataSize, &ExitData);
  }

  return Status;
}

EFI_STATUS
SetBbsPriority (
  IN  EFI_LEGACY_BIOS_PROTOCOL          *LegacyBios,
  IN  BDS_COMMON_OPTION                 *Option
  )
{
  BBS_TABLE                     *LocalBbsTable;
  UINT16                        BbsIndex;
  UINT16                        BootOption[10];
  UINTN                         BootOptionSize;
  UINT8                         *Ptr, *BootOptionVar;
  UINT16                        *OptionOrder;
  UINTN                         OptionOrderSize;
  UINT16                        PriorityIndex;
  BOOLEAN                       Flag;  // TRUE for Option->BootCurrent is the highest priority
  UINT16                        DevPathSize;
  CHAR16                        *BootDesc;

  //
  // Read the BootOrder variable.
  //
  OptionOrder = BdsLibGetVariableAndSize (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  &OptionOrderSize
                  );
  if (OptionOrder == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Flag = FALSE;

  //
  // Set BBS priority according OptionOrder variable
  //
  for (PriorityIndex = 0; PriorityIndex < OptionOrderSize / sizeof (UINT16); PriorityIndex++) {
    SPrint (BootOption, sizeof (BootOption), L"Boot%04x", OptionOrder[PriorityIndex]);

    BootOptionVar = BdsLibGetVariableAndSize (
                         BootOption,
                         &gEfiGlobalVariableGuid,
                         &BootOptionSize

                         );
    //
    // Skip the native boot options(EFI shell...)
    //
    Ptr = BootOptionVar + sizeof(UINT32) + sizeof(UINT16) + EfiStrSize ((CHAR16 *)(BootOptionVar + 6));
    if (*Ptr != BBS_DEVICE_PATH) {
      continue;
    }

    Ptr = BootOptionVar;

    Ptr += sizeof (UINT32);

    DevPathSize = *((UINT16 *) Ptr);
    Ptr += sizeof (UINT16);

    BootDesc = (CHAR16*) Ptr;
    Ptr += EfiStrSize (BootDesc);

    Ptr += DevPathSize;
    Ptr += sizeof (BBS_TABLE);

    BbsIndex = *(UINT16 *)Ptr;

    LegacyBios->GetBbsInfo (LegacyBios, NULL, NULL, NULL, &LocalBbsTable);

    LocalBbsTable[BbsIndex].BootPriority = (UINT16)PriorityIndex;

    //
    // Pull Option->BootCurrent up to the highest priority
    //
    if (!Flag) {
      if (Option->BootCurrent == OptionOrder[PriorityIndex]) {
        LocalBbsTable[BbsIndex].BootPriority = 0;
        Flag = TRUE;
      } else {
        LocalBbsTable[BbsIndex].BootPriority ++;
      }
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
BdsLibRemovedBootOption (
  IN  BOOLEAN                           RemovedLegacy
  )
/*++

Routine Description:

  According Input value to determine remove legacy boot device or EFI
  boot device.

Arguments:

  RemovedLegacy          - TRUE: Remove all of legacy boot devices
                         - FALSE: Remove all of EFI boot devices

Returns:
  EFI_NOT_FOUND          - Cannot find any boot device.
  EFI_OUT_OF_RESOURCES   - Boot Order and Boot option does't sychronization
  EFI_SUCCESS            - Remove boot devices successful.


--*/
{
  UINT16                    *BootOrder;
  UINT8                     *BootOptionVar;
  UINTN                     BootOrderSize;
  UINTN                     BootOptionSize;
  EFI_STATUS                Status;
  UINTN                     Index;
  UINTN                     Index2;
  UINT16                    BootOption[BOOT_OPTION_MAX_CHAR];

  BOOLEAN                   IsLegacyBootOption;
  BBS_TABLE                 *BbsEntry;
  UINT16                    BbsIndex;

  Status        = EFI_SUCCESS;
  BootOrder     = NULL;
  BootOrderSize = 0;

  //
  // delete physical and virtual boot order variable if system is in legacy mode.
  //
#ifdef AUTO_CREATE_DUMMY_BOOT_OPTION
  if (!RemovedLegacy) {
    Status = gRT->SetVariable (
                    PHYSICAL_BOOT_ORDER_NAME,
                    &gEfiGenericVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    0,
                    NULL
                    );
  }
#endif

  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &BootOrderSize
                );
  if (NULL == BootOrder) {
    return EFI_NOT_FOUND;
  }

  Index = 0;
  while (Index < BootOrderSize / sizeof (UINT16)) {
    SPrint (BootOption, sizeof (BootOption), L"Boot%04x", BootOrder[Index]);
    BootOptionVar = BdsLibGetVariableAndSize (
                      BootOption,
                      &gEfiGlobalVariableGuid,
                      &BootOptionSize
                      );
    if (NULL == BootOptionVar) {
      gBS->FreePool (BootOrder);
      return EFI_OUT_OF_RESOURCES;
    }

    IsLegacyBootOption = BdsLibIsLegacyBootOption (BootOptionVar, &BbsEntry, &BbsIndex);

    if (RemovedLegacy && !IsLegacyBootOption) {
      gBS->FreePool (BootOptionVar);
      Index++;
      continue;
    }

    if (!RemovedLegacy && IsLegacyBootOption) {
      gBS->FreePool (BootOptionVar);
      Index++;
      continue;
    }
    //
    // Delete this invalid boot option "Boot####"
    //
    Status = gRT->SetVariable (
                    BootOption,
                    &gEfiGlobalVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    0,
                    NULL
                    );
    //
    // Mark this boot option in boot order as deleted
    //
    BootOrder[Index] = 0xffff;

    gBS->FreePool (BootOptionVar);
    Index++;
  }

  //
  // Adjust boot order array
  //
  Index2 = 0;
  for (Index = 0; Index < BootOrderSize / sizeof (UINT16); Index++) {
    if (BootOrder[Index] != 0xffff) {
      BootOrder[Index2] = BootOrder[Index];
      Index2 ++;
    }
  }
  Status = gRT->SetVariable (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  Index2 * sizeof (UINT16),
                  BootOrder
                  );

  gBS->FreePool (BootOrder);

  return Status;

}

UINT32
BdsLibGetBootTypeFromDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL     *DevicePath
  )
/*++

Routine Description:

  For a bootable Device path, return its boot type

Arguments:

  DevicePath - The bootable device Path to check

Returns:

  UINT32 Boot type :
  //
  // If the device path contains any media deviec path node, it is media boot type
  // For the floppy node, handle it as media node
  //
  BDS_EFI_MEDIA_HD_BOOT
  BDS_EFI_MEDIA_CDROM_BOOT
  BDS_EFI_ACPI_FLOPPY_BOOT
  //
  // If the device path not contains any media deviec path node,  and
  // its last device path node point to a message device path node, it is
  // a message boot type
  //
  BDS_EFI_MESSAGE_ATAPI_BOOT
  BDS_EFI_MESSAGE_SCSI_BOOT
  BDS_EFI_MESSAGE_USB_DEVICE_BOOT
  BDS_EFI_MESSAGE_MISC_BOOT
  //
  // Legacy boot type
  //
  BDS_LEGACY_BBS_BOOT
  //
  // If a EFI Removable BlockIO device path not point to a media and message devie,
  // it is unsupported
  //
  BDS_EFI_UNSUPPORT

--*/
{
  ACPI_HID_DEVICE_PATH          *Acpi;
  EFI_DEVICE_PATH_PROTOCOL      *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL      *LastDeviceNode;
  UINT32                        BootType;

  if (NULL == DevicePath) {
    return BDS_EFI_UNSUPPORT;
  }

  TempDevicePath = DevicePath;

  while (!IsDevicePathEndType (TempDevicePath)) {
    switch (DevicePathType (TempDevicePath)) {

    case BBS_DEVICE_PATH:
       return BDS_LEGACY_BBS_BOOT;

    case MEDIA_DEVICE_PATH:
      if (DevicePathSubType (TempDevicePath) == MEDIA_HARDDRIVE_DP) {
        return BDS_EFI_MEDIA_HD_BOOT;
      } else if (DevicePathSubType (TempDevicePath) == MEDIA_CDROM_DP) {
        return BDS_EFI_MEDIA_CDROM_BOOT;
      } else if (DevicePathSubType (TempDevicePath) == MEDIA_FV_FILEPATH_DP) {
        return BDS_EFI_MEDIA_FV_FILEPATH_BOOT;
      }
      break;

    case ACPI_DEVICE_PATH:
      Acpi = (ACPI_HID_DEVICE_PATH *) TempDevicePath;
      if (EISA_ID_TO_NUM (Acpi->HID) == 0x0604) {
        return BDS_EFI_ACPI_FLOPPY_BOOT;
      }
      break;

    case MESSAGING_DEVICE_PATH:

      //
      // check message device path is USB device first.
      //
      if (DevicePathSubType(TempDevicePath) == MSG_USB_DP) {
        return BDS_EFI_MESSAGE_USB_DEVICE_BOOT;
      }

      //
      // check message device path is ISCSI device.
      //
      if (DevicePathSubType (TempDevicePath) == MSG_ISCSI_DP) {
        return BDS_EFI_MESSAGE_ISCSI_BOOT;
      }

      //
      // Get the last device path node
      //
      LastDeviceNode = NextDevicePathNode (TempDevicePath);

#if (EFI_SPECIFICATION_VERSION >= 0x00020000)
      if (DevicePathSubType(LastDeviceNode) == MSG_DEVICE_LOGICAL_UNIT_DP) {
        //
        // if the next node type is Device Logical Unit, which specify the Logical Unit Number (LUN),
        // skit it
        //
          LastDeviceNode = NextDevicePathNode (LastDeviceNode);
      }
#endif
      //
      // if the device path not only point to driver device, it is not a messaging device path,
      //
      if (!IsDevicePathEndType (LastDeviceNode)) {
        break;
      }

      switch (DevicePathSubType (TempDevicePath)) {

      case MSG_ATAPI_DP:
        BootType = BDS_EFI_MESSAGE_ATAPI_BOOT;
        break;

      case MSG_USB_DP:
        BootType = BDS_EFI_MESSAGE_USB_DEVICE_BOOT;
        break;

      case MSG_SCSI_DP:
        BootType = BDS_EFI_MESSAGE_SCSI_BOOT;
        break;

      case MSG_SATA_DP:
        BootType = BDS_EFI_MESSAGE_SATA_BOOT;
        break;

      case MSG_MAC_ADDR_DP:
      case MSG_VLAN_DP:
      case MSG_IPv4_DP:
      case MSG_IPv6_DP:
        BootType = BDS_EFI_MESSAGE_MAC_BOOT;
        break;

      default:
        BootType = BDS_EFI_MESSAGE_MISC_BOOT;
        break;
      }
      return BootType;

    default:
      break;
    }
    TempDevicePath = NextDevicePathNode (TempDevicePath);
  }

  return BDS_EFI_UNSUPPORT;
}

#ifdef UEFI_NETWORK_SUPPORT
BOOLEAN
MacEmpty (
  IN     UINT8                  *MacAddr
  )
/*
Routine Description:

  Check the MAC address is empty

Arguments:

  MacAddr

Returns:

  BOOLEAN : Yes or not

*/
{
  UINTN       Index;
  BOOLEAN     Result;
  UINT8       TempValue;

  Result    = FALSE;
  TempValue = 0;

  for (Index = 0; Index < UEFI_NETWORK_MAC_ADDRESS_LENGTH; Index++ ) {
    TempValue = TempValue + (UINT8)(MacAddr[Index]);
  }
  if (TempValue == 0) {
    Result = TRUE;
  }
  return Result;
}

BOOLEAN
MacCompare (
  IN     UINT8                  *MacAddr1,
  IN     UINT8                  *MacAddr2
  )
/*
Routine Description:

  Check two MAC address is equal

Arguments:

  MacAddr1
  MacAddr2

Returns:

  BOOLEAN : Yes or not

*/
{
  UINTN       Index;
  BOOLEAN     Result;

  Result = TRUE;

  for (Index = 0; Index < UEFI_NETWORK_MAC_ADDRESS_LENGTH; Index++ ) {
    if ((UINT8)(MacAddr1[Index]) != (UINT8)(MacAddr2[Index])) {
      Result = FALSE;
      break;
    }
  }
  return Result;
}

EFI_STATUS
MacToIndex (
  IN     EFI_MAC_ADDRESS        *DeviceMacAddr,
  IN OUT EFI_MAC_ADDRESS        *MacAddressList,
  OUT    UINTN                  *DeviceId
  )
/*
Routine Description:

  Transfer Mac address to device index
  It will compare device and mac address list
  If not found and not over UEFI_NETWORK_BOOT_OPTION_MAX value
  It will insert into mac address

Arguments:

  DeviceMacAddr   - Your Devices MAC Address
  MacAddressList  - Mac Address list buffer
  DeviceId        - Result

Returns:

  EFI_SUCCESS     - Found
  EFI_NOT_FOUND   - Not found or buffer full, check with UEFI_NETWORK_BOOT_OPTION_MAX
                    Id = UEFI_NETWORK_BOOT_OPTION_MAX

*/
{
  BOOLEAN      Found;
  UINTN        Index;

  Found = FALSE;
  for (Index = 0; Index < UEFI_NETWORK_BOOT_OPTION_MAX; Index++) {
    if (MacEmpty ( (VOID *) &(MacAddressList[Index]))) {
      EfiCopyMem ( &(MacAddressList[Index]), DeviceMacAddr, sizeof (EFI_MAC_ADDRESS));
    }
    if ( MacCompare ( (VOID *) &(MacAddressList[Index]),  (VOID *)(DeviceMacAddr))) {
      *DeviceId = Index;
      return EFI_SUCCESS;
    }
  }
  *DeviceId = Index;
  return EFI_NOT_FOUND;
}

EFI_STATUS
BuildNetworkBootOption (
  IN     EFI_LIST_ENTRY  *BdsBootOptionList
  )
{
  EFI_STATUS                    Status;
  UINTN                         Index;
  EFI_SETUP_UTILITY_PROTOCOL    *SetupUtility;
  SYSTEM_CONFIGURATION          *SystemConfiguration;
  EFI_DEVICE_PATH_PROTOCOL      *NetworkDevicePath;
  EFI_DEVICE_PATH_PROTOCOL      *TempDevicePath;
  UINTN                         NumberLoadFileHandles;
  EFI_HANDLE                    *LoadFileHandles;
  CHAR16                        Buffer[UEFI_NETWORK_BOOT_OPTION_MAX_CHAR];
  MAC_ADDR_DEVICE_PATH          *MAC;
  UINTN                         DeviceId;
  EFI_MAC_ADDRESS               *MacAddressList = NULL;
  BOOLEAN                       NeedBuild;
  UINT8                         IpType;
  CHAR16                        VlanString[UEFI_NETWORK_VLAN_STRING_LENGTH];
  UINT16                        VlanID;
  BOOLEAN                       IsIPv4BootOption = FALSE;
  BOOLEAN                       IsIPv6BootOption = FALSE;

  SystemConfiguration = NULL;
  NetworkDevicePath   = NULL;
  TempDevicePath      = NULL;
  MacAddressList      = NULL;
  LoadFileHandles     = NULL;

  Status = gBS->LocateProtocol (
                  &gEfiSetupUtilityProtocolGuid,
                  NULL,
                  &SetupUtility
                  );
  if (!EFI_ERROR (Status)) {
    SystemConfiguration = (SYSTEM_CONFIGURATION *)SetupUtility->SetupNvData;
  } else {
    //
    // make sure we have valid pointer before continuing
    //
    return Status;
  }

  //
  // If Pxe disabled then do not build Network boot variables
  //
  if (SystemConfiguration->PxeBootToLan == 0) {
    return EFI_SUCCESS;
  }

  EfiZeroMem (Buffer, sizeof (UEFI_NETWORK_BOOT_OPTION_MAX_CHAR) * sizeof (CHAR16));

  MacAddressList = EfiLibAllocateZeroPool (sizeof (EFI_MAC_ADDRESS) * UEFI_NETWORK_BOOT_OPTION_MAX);
  if (MacAddressList == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ERROR_HANDLE;
  }

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiLoadFileProtocolGuid,
                  NULL,
                  &NumberLoadFileHandles,
                  &LoadFileHandles
                  );

  if (EFI_ERROR (Status)) {
    goto ERROR_HANDLE;
  }

  for (Index = 0; Index < NumberLoadFileHandles; Index++) {

    NeedBuild = FALSE;
    //
    // VlanID is Range 0 ~ 4094, If VlanID == 0xFFFF means not Vlan device
    //
    VlanID              = 0xFFFF;
    MAC                 = NULL;
    IsIPv4BootOption    = FALSE;
    IsIPv6BootOption    = FALSE;
    IpType              = 0;

    Status = gBS->HandleProtocol (
                    LoadFileHandles[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID **) &TempDevicePath
                    );

    if (EFI_ERROR (Status)) {
      continue;
    }

    //
    // Scan DevicePath include MAC and IPv4/IPv6
    //
    NetworkDevicePath = TempDevicePath;
    while (!EfiIsDevicePathEnd (TempDevicePath)) {
      if (TempDevicePath->Type == MESSAGING_DEVICE_PATH) {
        if (TempDevicePath->SubType == MSG_MAC_ADDR_DP) {
          MAC = (MAC_ADDR_DEVICE_PATH *)TempDevicePath;
        } else if (TempDevicePath->SubType == MSG_VLAN_DP) {
          VlanID = ((VLAN_DEVICE_PATH *)TempDevicePath)->VlanId;
        } else if (TempDevicePath->SubType == MSG_IPv4_DP) {
          IsIPv4BootOption = TRUE;
        } else if (TempDevicePath->SubType == MSG_IPv6_DP) {
          IsIPv6BootOption = TRUE;
        }
      }
      TempDevicePath = EfiNextDevicePathNode (TempDevicePath);
    }

    //
    // IPv4 and IPv6 in one DevicePath is InValid
    //
    if (IsIPv4BootOption && IsIPv6BootOption) {
      continue;
    }

    //
    // Build Policy:
    //   Device Path is:
    //     MAC\IPv4    : "EFI Network X for IPv4 (MAC Address)"
    //     MAC\IPv6    : "EFI Network X for IPv6 (MAC Address)"
    //     MAC         : "EFI Network X for IPv4 (MAC Address)"
    //     Other       : "Others: (Device Path)"
    //
    if (MAC != NULL && (!IsIPv4BootOption && !IsIPv6BootOption)) {
      IsIPv4BootOption = TRUE;
    }

    switch (SystemConfiguration->NetworkProtocol) {
    case UEFI_NETWORK_BOOT_OPTION_IPV4:
#ifdef DUAL_NETWORK_ENABLE
    case UEFI_NETWORK_BOOT_OPTION_BOTH:
      //
      // Only IPv4 Device
      //
      if (IsIPv4BootOption && !IsIPv6BootOption) {
#endif
        if (MAC != NULL) {
          IpType    = 4;
          NeedBuild = TRUE;
        }
#ifndef DUAL_NETWORK_ENABLE
        break;
#else
      }
      //
      // Break when option is Only IPv4
      //
      if (SystemConfiguration->NetworkProtocol == UEFI_NETWORK_BOOT_OPTION_IPV4) {
        break;
      }
    case UEFI_NETWORK_BOOT_OPTION_IPV6:
      //
      // Only IPv6 Device
      //
      if (!IsIPv4BootOption && IsIPv6BootOption) {
        if (MAC != NULL) {
          IpType    = 6;
          NeedBuild = TRUE;
        }
      }
      break;
#endif
    case UEFI_NETWORK_BOOT_OPTION_NONE:
    default:
      break;
    }

    //
    // Build when DevicePath check is vaild
    // But if DevicePath include IPv4 and IPv6 is Invaild, will not build it
    //
    if (!IsIPv4BootOption && !IsIPv6BootOption) {
      NeedBuild = TRUE;
    }

    if (NeedBuild) {
      if (IsIPv4BootOption || IsIPv6BootOption) {
        MacToIndex (&(MAC->MacAddress), MacAddressList, &DeviceId);
        SPrint (
          Buffer,
          sizeof (Buffer),
          L"EFI Network %d for IPv%d (%02x-%02x-%02x-%02x-%02x-%02x) ",  //        NetworkOptionString,
          DeviceId,
          IpType,
          MAC->MacAddress.Addr[0],
          MAC->MacAddress.Addr[1],
          MAC->MacAddress.Addr[2],
          MAC->MacAddress.Addr[3],
          MAC->MacAddress.Addr[4],
          MAC->MacAddress.Addr[5]
          );
        if (VlanID != 0xFFFF) {
          SPrint (
            VlanString,
            sizeof (VlanString),
            L"VLAN(%d)",
            VlanID
            );
          EfiStrCat (Buffer, VlanString);
        }
        BdsLibBuildOptionFromHandle (LoadFileHandles[Index], BdsBootOptionList, Buffer);
      } else {
        //
        // Build unknown load file option
        //
        EfiStrCat (Buffer, L"Others: ");
        EfiStrCat (Buffer, DevicePathToStr (NetworkDevicePath));
        BdsLibBuildOptionFromHandle (LoadFileHandles[Index], BdsBootOptionList, Buffer);
      }
    }
  }

ERROR_HANDLE:
  if (LoadFileHandles) {
    gBS->FreePool (LoadFileHandles);
  }
  if (MacAddressList) {
    gBS->FreePool (MacAddressList);
  }

  return Status;
}
#endif

EFI_STATUS
BdsLibOpenFileFromDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL   *DevicePath,
  IN  UINT64                     OpenMode,
  IN  UINT64                     Attributes,
  OUT EFI_FILE_HANDLE            *NewHandle
  )
/*++

Routine Description:

  According to device path to open file .

Arguments:

  DevicePath - Pointer to EFI_DEVICE_PATH_PROTOCOL
  OpenMode   - The mode to open the file. The only valid combinations that the
               file may be opened with are: Read, Read/Write, or Create/Read/
               Write. See ¡§Related Definitions¡¨ below.
  Attributes - Only valid for EFI_FILE_MODE_CREATE, in which case these
               are the attribute bits for the newly created file. See ¡§Related
               Definitions¡¨ below.
  NewHandle  - A pointer to the location to return the opened handle for the new
               file. See the type EFI_FILE_PROTOCOL description.

Returns:

  EFI_SUCCESS          - The file was opened.
  EFI_NOT_FOUND        - The specified file could not be found on the device.
  EFI_NO_MEDIA         - The device has no medium.
  EFI_MEDIA_CHANGED    - The device has a different medium in it or the medium is no longer supported.
  EFI_DEVICE_ERROR     - The device reported an error.
  EFI_VOLUME_CORRUPTED - The file system structures are corrupted.
  EFI_WRITE_PROTECTED  - An attempt was made to create a file, or open a file for write
                         when the media is write-protected.
  EFI_ACCESS_DENIED    - The service denied access to the file.
  EFI_OUT_OF_RESOURCES - Not enough resources were available to open the file.
  EFI_VOLUME_FULL      - The volume is full.

--*/
{
  EFI_STATUS                        Status;
  EFI_HANDLE                        DeviceHandle;
  FILEPATH_DEVICE_PATH              *FilePathNode;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL   *Volume;
  EFI_FILE_HANDLE                   FileHandle;
  EFI_FILE_HANDLE                   LastHandle;
  FILEPATH_DEVICE_PATH              *OriginalFilePathNode;

  FilePathNode = (FILEPATH_DEVICE_PATH *) DevicePath;
  Volume       = NULL;
  *NewHandle   = NULL;
  Status = gBS->LocateDevicePath (
                  &gEfiSimpleFileSystemProtocolGuid,
                  (EFI_DEVICE_PATH_PROTOCOL **) &FilePathNode,
                  &DeviceHandle
                  );
  if (!EFI_ERROR (Status)) {
    Status = gBS->HandleProtocol (DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, &Volume);
  }

  if (EFI_ERROR (Status) || Volume == NULL) {
    return Status;
  }

  Status = Volume->OpenVolume (Volume, &FileHandle);
  if (!EFI_ERROR (Status)) {
    //
    // Duplicate the device path to avoid the access to unaligned device path node.
    // Because the device path consists of one or more FILE PATH MEDIA DEVICE PATH
    // nodes, It assures the fields in device path nodes are 2 byte aligned.
    //
    FilePathNode = (FILEPATH_DEVICE_PATH *) EfiDuplicateDevicePath (
                                              (EFI_DEVICE_PATH_PROTOCOL *)(UINTN)FilePathNode
                                              );
    if (FilePathNode == NULL) {
      FileHandle->Close (FileHandle);
      Status = EFI_OUT_OF_RESOURCES;
    } else {
      OriginalFilePathNode = FilePathNode;
      //
      // Parse each MEDIA_FILEPATH_DP node. There may be more than one, since the
      //  directory information and filename can be seperate. The goal is to inch
      //  our way down each device path node and close the previous node
      //
      while (!IsDevicePathEnd (&FilePathNode->Header)) {
        if (DevicePathType (&FilePathNode->Header) != MEDIA_DEVICE_PATH ||
            DevicePathSubType (&FilePathNode->Header) != MEDIA_FILEPATH_DP) {
          Status = EFI_UNSUPPORTED;
        }

        if (EFI_ERROR (Status)) {
          //
          // Exit loop on Error
          //
          break;
        }

        LastHandle = FileHandle;
        FileHandle = NULL;
        Status = LastHandle->Open (
                               LastHandle,
                               &FileHandle,
                               FilePathNode->PathName,
                               OpenMode,
                               Attributes
                               );

        //
        // Close the previous node
        //
        LastHandle->Close (LastHandle);
        FilePathNode = (FILEPATH_DEVICE_PATH *) NextDevicePathNode (&FilePathNode->Header);
      }
      //
      // Free the allocated memory pool
      //
      gBS->FreePool (OriginalFilePathNode);
    }
  }
  if (!EFI_ERROR (Status)) {
    *NewHandle = FileHandle;
  }

  return Status;
}

EFI_STATUS
CheckModeSupported (
  EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput,
  IN  UINT32                    HorizontalResolution,
  IN  UINT32                    VerticalResolution,
  OUT UINT32                    *SupportedModeNumber
  )
/*++

Routine Description:

  Check if the current specific mode supported the user defined resolution
  for the Graphics Console device based on Graphics Output Protocol.

Arguments:

  GraphicsOutput       - Graphics Output Protocol instance pointer.
  HorizontalResolution - User defined horizontal resolution
  VerticalResolution   - User defined vertical resolution.
  CurrentModeNumber    - Current specific mode to be check.

Returns:

  EFI_SUCCESS          - The mode is supported.
  EFI_UNSUPPORTED      - The specific mode is out of range of graphics device supported.
  @retval other        - The specific mode does not support user defined
                         resolution or failed to set the current mode to the
                         specific mode on graphics device.

--*/
{
  UINT32                               ModeNumber;
  EFI_STATUS                           Status;
  UINTN                                SizeOfInfo;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;
  UINT32                               MaxMode;

  Status  = EFI_SUCCESS;
  MaxMode = GraphicsOutput->Mode->MaxMode;

  for (ModeNumber = 0; ModeNumber < MaxMode; ModeNumber++) {
    Status = GraphicsOutput->QueryMode (
                               GraphicsOutput,
                               ModeNumber,
                               &SizeOfInfo,
                               &Info
                               );
    if (!EFI_ERROR (Status)) {
      if ((Info->HorizontalResolution == HorizontalResolution) &&
          (Info->VerticalResolution == VerticalResolution)) {
          gBS->FreePool (Info);
          break;
      }
      gBS->FreePool (Info);
    }
  }

  if (ModeNumber == GraphicsOutput->Mode->MaxMode) {
    Status = EFI_UNSUPPORTED;
  }

  *SupportedModeNumber = ModeNumber;
  return Status;
}

EFI_STATUS
BgrtUpdateImageInfo (
  IN  EFI_HANDLE                        *PrimaryVgaHandle,
  OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL     **BltBuffer,
  OUT UINTN                             *DestinationX,
  OUT UINTN                             *DestinationY,
  OUT UINTN                             *Width,
  OUT UINTN                             *Height
  )
/*++

Routine Description:

  Update Bgrt information and retrive image information.

Arguments:

  PrimaryVgaHandle       - A pointer to primary vga handle
  **BltBuffer            - A pointer to Blt Buffer which contains image information
  *DestinationX          - A pointer to a calculated offset X which will be set into Bgrt
  *DestinationY          - A pointer to a calculated offset Y which will be set into Bgrt
  *Width                 - A pointer to a image size width which will be set into Bgrt
  *Height                - A pointer to a image size height which will be set into Bgrt

Returns:

  EFI_SUCCESS            - The function runs correctly.

--*/
{
  EFI_STATUS                                    Status;
  UINT8                                         *Blt;
  UINT8                                         *ImageData;
  UINTN                                         ImageSize;
  UINTN                                         ResolutionX;
  UINTN                                         ResolutionY;
  UINTN                                         ImageWidth;
  UINTN                                         ImageHeight;
  EFI_BADGING_SUPPORT_FORMAT                    BadgingImageFormat;
  EFI_GRAPHICS_OUTPUT_PROTOCOL                  *GraphicsOutput;
  UINT32                                        ModeNumber;
  OEM_SERVICES_PROTOCOL                         *OemServices;
  OEM_LOGO_RESOLUTION_DEFINITION                *OemLogoResolutionPtr;
  OEM_LOGO_RESOLUTION_DEFINITION                LogoResolution;
  EFI_EDID_DISCOVERED_PROTOCOL                  *EdidDiscovered;
  UINT32                                        NativeResolutionX;
  UINT32                                        NativeResolutionY;

  Blt                = NULL;
  ImageData          = NULL;
  ImageSize          = 0;
  ImageWidth         = 0;
  ImageHeight        = 0;
  ModeNumber         = 0;
  BadgingImageFormat = EfiBadgingSupportFormatUnknown;
  ResolutionX = 0;
  ResolutionY = 0;

  Status = BgrtGetImageByBadging (&ImageData, &ImageSize, &BadgingImageFormat);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = gBS->HandleProtocol (
                  PrimaryVgaHandle,
                  &gEfiGraphicsOutputProtocolGuid,
                  (VOID **) &GraphicsOutput
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = gBS->HandleProtocol (
                  PrimaryVgaHandle,
                  &gEfiEdidDiscoveredProtocolGuid,
                  (VOID **) &EdidDiscovered
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  OemLogoResolutionPtr = NULL;

  Status = GetResolutionByEdid (EdidDiscovered, &NativeResolutionX, &NativeResolutionY);
  if (EFI_ERROR (Status)) {
    return  Status;
  }

  LogoResolution.LogoResolutionX = NativeResolutionX;
  LogoResolution.LogoResolutionY = NativeResolutionY;
  Status = CheckModeSupported (
             GraphicsOutput,
             NativeResolutionX,
             NativeResolutionY,
             &ModeNumber
             );
  if (!EFI_ERROR(Status)) {
    OemLogoResolutionPtr = &LogoResolution;
  }

  Status = gBS->LocateProtocol(&gOemServicesProtocolGuid, NULL, &OemServices);
  if (EFI_ERROR(Status)) {
    OemLogoResolutionPtr = &LogoResolution;
  } else {
    OemServices->Funcs[COMMON_LOGO_RESOLUTION] (
                   OemServices,
                   COMMON_LOGO_RESOLUTION_ARG_COUNT,
                   &OemLogoResolutionPtr
                   );
  }

  Status = CheckModeSupported (
                     GraphicsOutput,
                     OemLogoResolutionPtr->LogoResolutionX,
                     OemLogoResolutionPtr->LogoResolutionY,
                     &ModeNumber
                     );
  if (EFI_ERROR (Status)) {
    Status = CheckModeSupported (
               GraphicsOutput,
               DEFAULT_HORIZONTAL_RESOLUTION,
               DEFAULT_VERTICAL_RESOLUTION,
               &ModeNumber
               );
    if (!EFI_ERROR (Status)) {
      ResolutionX = DEFAULT_HORIZONTAL_RESOLUTION;
      ResolutionY = DEFAULT_VERTICAL_RESOLUTION;
    }
  } else {
    ResolutionX = OemLogoResolutionPtr->LogoResolutionX;
    ResolutionY = OemLogoResolutionPtr->LogoResolutionY;
  }

  Status = BgrtDecodeImageToBlt (
             ImageData,
             ImageSize,
             BadgingImageFormat,
             &Blt,
             &ImageWidth,
             &ImageHeight
             );

  if (!EFI_ERROR(Status)) {
    *BltBuffer = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL*)Blt;
    *Width     = ImageWidth;
    *Height    = ImageHeight;
  }

  BgrtGetLogoCoordinate (
    ResolutionX,
    ResolutionY,
    ImageWidth,
    ImageHeight,
    DestinationX,
    DestinationY
    );

  if (ImageData != NULL) {
    gBS->FreePool (ImageData);
  }

  return Status;
}

EFI_STATUS
CreateDummyBootOptions (
  VOID
  )
/*++

Routine Description:

  Create dummy boot options which use to selected by UEFI OS.

Arguments:

  None.

Returns:

  EFI_SUCCESS  - Create dummy boot options successful.
  Other        - Any error occured while creating dummy boot options .

--*/
{
  UINT16                        BootOption[BOOT_OPTION_MAX_CHAR];
  UINTN                         VariableSize;
  UINT8                         *BootOptionVar;
  EFI_STATUS                    Status;
  EFI_DEVICE_PATH_PROTOCOL      DevicePath;
  UINTN                         TableCnt;
  UINTN                         Index;

  TableCnt = sizeof (mDummyBootOptionTable) / sizeof (DUMMY_BOOT_OPTION_INFO);
  Status = EFI_SUCCESS;
  for (Index = 0; Index < TableCnt; Index++) {
    ASSERT (BdsLibIsDummyBootOption (mDummyBootOptionTable[Index].BootOptionNum));
    SPrint (BootOption, sizeof (BootOption), L"Boot%04x", mDummyBootOptionTable[Index].BootOptionNum);
    BootOptionVar = BdsLibGetVariableAndSize (
                      BootOption,
                      &gEfiGlobalVariableGuid,
                      &VariableSize
                      );
    if (BootOptionVar != NULL) {
      gBS->FreePool (BootOptionVar);
    } else {
      SetDevicePathEndNode (&DevicePath);
      Status = BdsLibUpdateOptionVar (
                 BootOption,
                 &DevicePath,
                 mDummyBootOptionTable[Index].Description,
                 "RC",
                 2
                 );
      ASSERT_EFI_ERROR (Status);
    }
  }
  return Status;
}

UINT16 *
GetFirstAdjacentOptionsByDummyOptionNum (
  IN  DUMMY_BOOT_OPTION_NUM   DummyOptionNum,
  IN  UINT16                  *BootOrder,
  IN  UINTN                   BootOrderNum,
  OUT UINTN                   *OptionNum
  )
/*++

Routine Description:

  Parse the input boot order and return first adjacent boot options which belongs to specific dummy boot option.
  If the first option of boot order does not belong to specific dummy boot option, return not found.

Arguments:

  DummyOptionNum  - Dummy boot option number.
  BootOrder       - Pointer to the boot order.
  BootOrderNum    - The number of boot options in the boot order.
  OptionNum       - Pointer to the number of boot options returned in the output option list.

Returns:

  The list of boot options which are the first adjacent boot options in boot order and belongs to specific
  dummy boot option or NULL if not found.

--*/
{
  UINTN                      Index;

  for (Index = 0; Index < BootOrderNum; Index++) {
    if (BdsLibGetDummyBootOptionNum (BootOrder[Index]) != DummyOptionNum) {
      break;
    }
  }

  *OptionNum  = Index;
  return EfiLibAllocateCopyPool (Index * sizeof(UINT16), BootOrder);
}

UINT16 *
GetAllOptionsByDummyOptionNum (
  IN  DUMMY_BOOT_OPTION_NUM    DummyOptionNum,
  IN  UINT16                   *BootOrder,
  IN  UINTN                    BootOrderNum,
  OUT UINTN                    *OptionNum
  )
/*++

Routine Description:

  Parse the input boot order and return all boot options which belong to specific dummy boot option.

Arguments:

  DummyOptionNum  - Dummy boot option number.
  BootOrder       - Pointer to the boot order.
  BootOrderNum    - The number of boot options in the boot order.
  OptionNum       - Pointer to the number of boot options returned in the output option list.

Returns:

  The list of boot options which belong to specific dummy boot option or NULL if not found.

--*/
{
  UINTN                      Index;
  UINTN                      Count;
  UINT16                     *OptionList;

  Count = 0;
  OptionList = EfiLibAllocatePool (BootOrderNum * sizeof(UINT16));

  for (Index = 0; Index < BootOrderNum; Index++) {
    if (BdsLibGetDummyBootOptionNum (BootOrder[Index]) == DummyOptionNum) {
      OptionList[Count] = BootOrder[Index];
      Count++;
    }
  }

  *OptionNum  = Count;

  if (Count == 0) {
    if (OptionList != NULL) {
      gBS->FreePool (OptionList);
      OptionList = NULL;
    }
  }

  return OptionList;
}

EFI_STATUS
AddOptionsToBootOrder (
  IN     UINT16              *OptionList,
  IN     UINTN               OptionNum,
  IN     UINTN               BootOrderSize,
  IN OUT UINT16              *BootOrder,
  IN OUT UINTN               *BootOrderNum
  )
/*++

Routine Description:

  Add boot option(s) into the input boot order.

Arguments:

  OptionList      - The list of boot options.
  OptionNum       - The number of boot options in the option list.
  BootOrderSize   - The buffer size of boot order.
  BootOrder       - Pointer to the boot order.
  BootOrderNum    - Pointer to the number of boot options in the boot order.

Returns:

  EFI_SUCCESS           - Success to add boot options into input boot order.
  EFI_INVALID_PARAMETER - Input parameter is NULL.
  EFI_OUT_OF_RESOURCES  - The buffer size of boot order is not enough.

--*/
{
  if (OptionList == NULL || BootOrder == NULL || BootOrderNum == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (BootOrderSize < (*BootOrderNum + OptionNum) * sizeof(UINT16)) {
    return EFI_OUT_OF_RESOURCES;
  }

  EfiCopyMem (&BootOrder[*BootOrderNum], OptionList, OptionNum * sizeof(UINT16));
  (*BootOrderNum) += OptionNum;

  return EFI_SUCCESS;
}


EFI_STATUS
RemoveOptionsInBootOrder (
  IN     UINT16              *OptionList,
  IN     UINTN               OptionNum,
  IN OUT UINT16              *BootOrder,
  IN OUT UINTN               *BootOrderNum
  )
/*++

Routine Description:

  Remove boot option(s) in the input boot order.

Arguments:

  OptionList      - The list of boot options.
  OptionNum       - The number of boot options in the option list.
  BootOrder       - Pointer to the boot order.
  BootOrderNum    - Pointer to the number of boot options in the boot order.

Returns:

  EFI_SUCCESS           - Success to delete boot options in the input boot order.
  EFI_INVALID_PARAMETER - Input parameter is NULL.

--*/
{
  UINTN       OptionIndex;
  UINTN       Index;


  if (OptionList == NULL || BootOrder == NULL || BootOrderNum == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  for (OptionIndex = 0; OptionIndex < OptionNum; OptionIndex++) {
    for (Index = 0; Index < *BootOrderNum; Index++) {
      if (BootOrder[Index] == OptionList[OptionIndex]) {
        EfiCopyMem (&BootOrder[Index], &BootOrder[Index + 1], (*BootOrderNum - Index - 1) * sizeof (UINT16));
        (*BootOrderNum)--;
        break;
      }
    }
  }

  return EFI_SUCCESS;
}

#ifdef AUTO_CREATE_DUMMY_BOOT_OPTION
EFI_STATUS
SyncBootOrder (
  VOID
  )
/*++

Routine Description:

  Synchronize the contents of "BootOrder" to prevent from the contents is
  updated by SCU or UEFI OS.

Arguments:

  None.

Returns:

  EFI_SUCCESS  - Synchronize data successful.
  Other        - Set "BootOrder" varible failed.

--*/
{
  UINT16          *BootOrder;
  UINTN           BootOrderSize;
  UINT16          *PhysicalBootOrder;
  UINTN           PhysicalBootOrderSize;
  UINT16          *WorkingBootOrder;
  UINTN           WorkingBootOrderNum;
  EFI_STATUS      Status;
  BOOLEAN         IsPhysicalBootOrder;
  UINTN           Index;
  UINTN           BootOrderNum;
  UINTN           PhysicalBootOrderNum;
  UINTN           WorkingBootOrderSize;
  UINT16          *OptionList;
  UINTN           OptionNum;


  if (BdsLibIsBootOrderHookEnabled ()) {
    return EFI_SUCCESS;
  }
  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &BootOrderSize
                );
  if (BootOrder == NULL) {
    return EFI_SUCCESS;
  }
  BootOrderNum = BootOrderSize / sizeof (UINT16);

  Status = BdsLibGetBootOrderType (BootOrder, BootOrderNum, &IsPhysicalBootOrder);
  if (!EFI_ERROR (Status) && IsPhysicalBootOrder) {
    Status = gRT->SetVariable (
                    PHYSICAL_BOOT_ORDER_NAME,
                    &gEfiGenericVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    BootOrderSize,
                    BootOrder
                    );
  } else if (!EFI_ERROR (Status) && !IsPhysicalBootOrder) {
    PhysicalBootOrder = BdsLibGetVariableAndSize (
                          PHYSICAL_BOOT_ORDER_NAME,
                          &gEfiGenericVariableGuid,
                          &PhysicalBootOrderSize
                          );
    PhysicalBootOrderNum = PhysicalBootOrderSize / sizeof (UINT16);

    WorkingBootOrderSize = PhysicalBootOrderSize + BootOrderSize;
    WorkingBootOrder = EfiLibAllocateZeroPool (WorkingBootOrderSize);
    WorkingBootOrderNum = 0;

    //
    // Based on the priority of BootOrder variable with dummy boot options,
    // sync the priority of physical boot order into working boot order.
    //
    for (Index = 0; Index < BootOrderNum; Index++) {
      if (BdsLibIsDummyBootOption (BootOrder[Index])) {
        if (PhysicalBootOrderNum == 0) {
          continue;
        }

        if (BdsLibGetDummyBootOptionNum (PhysicalBootOrder[0]) == BootOrder[Index]) {
          OptionList = GetFirstAdjacentOptionsByDummyOptionNum (
                         BootOrder[Index],
                         PhysicalBootOrder,
                         PhysicalBootOrderNum,
                         &OptionNum
                         );
        } else {
          OptionList = GetAllOptionsByDummyOptionNum (
                         BootOrder[Index],
                         PhysicalBootOrder,
                         PhysicalBootOrderNum,
                         &OptionNum
                         );
        }

        if (OptionList != NULL) {
          AddOptionsToBootOrder (
            OptionList,
            OptionNum,
            WorkingBootOrderSize,
            WorkingBootOrder,
            &WorkingBootOrderNum
            );
          RemoveOptionsInBootOrder (
            OptionList,
            OptionNum,
            PhysicalBootOrder,
            &PhysicalBootOrderNum
            );
          gBS->FreePool (OptionList);
        }
      } else {
        AddOptionsToBootOrder (
          &BootOrder[Index],
          1,
          WorkingBootOrderSize,
          WorkingBootOrder,
          &WorkingBootOrderNum
          );
        RemoveOptionsInBootOrder (
          &BootOrder[Index],
          1,
          PhysicalBootOrder,
          &PhysicalBootOrderNum
          );
      }
    }

    EfiCopyMem (&WorkingBootOrder[WorkingBootOrderNum], PhysicalBootOrder, PhysicalBootOrderNum * sizeof(UINT16));
    WorkingBootOrderNum += PhysicalBootOrderNum;
    Status = gRT->SetVariable (
                    PHYSICAL_BOOT_ORDER_NAME,
                    &gEfiGenericVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    WorkingBootOrderNum * sizeof (UINT16),
                    WorkingBootOrder
                    );
    if (PhysicalBootOrder != NULL) {
      gBS->FreePool (PhysicalBootOrder);
    }
    if (WorkingBootOrder != NULL) {
      gBS->FreePool (WorkingBootOrder);
    }
  }

  if (BootOrder != NULL) {
    gBS->FreePool (BootOrder);
  }

  return Status;
}
#else
EFI_STATUS
SyncBootOrder (
  VOID
  )
/*++

Routine Description:

  Synchronize the contents of "BootOrder" to prevent from the contents is
  updated by SCU or UEFI OS.

Arguments:

  None.

Returns:

  EFI_SUCCESS  - Synchronize data successful.
  Other        - Set "BootOrder" varible failed.

--*/
{
  return EFI_SUCCESS;
}
#endif

VOID
EnableOptimalTextMode (
  VOID
  )
{
  EFI_STATUS                                Status;
  OEM_LOGO_RESOLUTION_DEFINITION            *OemLogoResolutionPtr;
  OEM_LOGO_RESOLUTION_DEFINITION            LogoResolution;
  EFI_GRAPHICS_OUTPUT_PROTOCOL              *GraphicsOutput;
  UINT32                                    SizeOfX;
  UINT32                                    SizeOfY;
  UINTN                                     RequestedRows;
  UINTN                                     RequestedColumns;
  UINTN                                     TextModeNum;
  UINT32                                    Index;
  EFI_HANDLE                                SinglePhyGopHandle;
  EFI_EDID_DISCOVERED_PROTOCOL              *EdidDiscovered;
  UINT32                                    NativeResolutionX;
  UINT32                                    NativeResolutionY;
  UINTN                                     BestResolution;
  UINTN                                     SizeOfInfo;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION      *Info;
  OEM_SERVICES_PROTOCOL                     *OemServices;
  UINT32                                    GopModeNum;
  UINTN                                     ModeNum;
  UINTN                                     Columns;
  UINTN                                     Rows;
  UINTN                                     OptimalTxtMode;
  EFI_DEVICE_PATH_PROTOCOL                  *ConOutDevicePath;

  //
  //init local vars
  //
  Index = 0;
  SinglePhyGopHandle = NULL;
  OemServices = NULL;
  OemLogoResolutionPtr = NULL;
  SizeOfInfo = 0;
  Info = NULL;
  BestResolution = 0;
  RequestedColumns  = 0;
  RequestedRows = 0;
  SizeOfX = 0;
  SizeOfY = 0;
  GraphicsOutput = NULL;
  OptimalTxtMode = 0;
  TextModeNum = gST->ConOut->Mode->MaxMode;
  ConOutDevicePath = NULL;

  ConOutDevicePath = GetConOutVar (L"ConOut");
  if (ConOutDevicePath == NULL) {
    //
    // no monitor is attached
    //
    return ;
  }

  if (FoundTextBasedConsole ()) {
    //
    //if any Console deveice is text-based display,
    //based on text mode intersection, find optimal text mode
    //
    for (ModeNum = 0; ModeNum < gST->ConOut->Mode->MaxMode; ModeNum++) {
      Status =  gST->ConOut->QueryMode (gST->ConOut, ModeNum, &Columns, &Rows);
      if (!EFI_ERROR (Status)) {
        if ((Columns * Rows) > OptimalTxtMode) {
          OptimalTxtMode = Columns * Rows;
          TextModeNum = (UINTN)ModeNum;
        }
      }
    }
    if (TextModeNum == gST->ConOut->Mode->MaxMode) {
      Status = EFI_UNSUPPORTED;
    } else {
      Status = EFI_SUCCESS;
    }
    goto Done;
  }


  if (IsSinglePhysicalGop (&SinglePhyGopHandle)) {
    gBS->HandleProtocol (
       SinglePhyGopHandle,
       &gEfiGraphicsOutputProtocolGuid,
       (VOID**) &GraphicsOutput
       );
    Status = gBS->HandleProtocol (
                    SinglePhyGopHandle,
                    &gEfiEdidDiscoveredProtocolGuid,
                    (VOID **) &EdidDiscovered
                    );
    if (!EFI_ERROR (Status)) {
    Status = GetResolutionByEdid (EdidDiscovered, &NativeResolutionX, &NativeResolutionY);
      if (!EFI_ERROR (Status)) {
        LogoResolution.LogoResolutionX = NativeResolutionX;
        LogoResolution.LogoResolutionY = NativeResolutionY;
        Status = CheckModeSupported (
                   GraphicsOutput,
                   NativeResolutionX,
                   NativeResolutionY,
                   &GopModeNum
                   );
        if (!EFI_ERROR (Status)) {
          OemLogoResolutionPtr = &LogoResolution;
        }
      }
    } else {
      //
      // find optimal resoltion from gop modes to set for combination of two video output devices
      // attached to a gop device
      //
      GetComboVideoOptimalResolution (GraphicsOutput, &SizeOfX, &SizeOfY);
      LogoResolution.LogoResolutionX = SizeOfX;
      LogoResolution.LogoResolutionY = SizeOfY;
      OemLogoResolutionPtr = &LogoResolution;
    }

    Status = gBS->LocateProtocol(&gOemServicesProtocolGuid, NULL, &OemServices);
    if (!EFI_ERROR (Status)) {
      OemServices->Funcs[COMMON_LOGO_RESOLUTION] (
                   OemServices,
                   COMMON_LOGO_RESOLUTION_ARG_COUNT,
                   &OemLogoResolutionPtr
                   );
      SizeOfX = OemLogoResolutionPtr->LogoResolutionX;
      SizeOfY = OemLogoResolutionPtr->LogoResolutionY;
	  }

  } else {
    Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, &GraphicsOutput);
    if (!EFI_ERROR (Status)) {
      for (Index = 0;Index < GraphicsOutput->Mode->MaxMode; Index++) {
        //
        //find best resolution from virtual gop
        //
        GraphicsOutput->QueryMode (GraphicsOutput, Index, &SizeOfInfo, &Info);
        if (((Info->HorizontalResolution) * (Info->VerticalResolution)) > BestResolution) {
          BestResolution =  (Info->HorizontalResolution) * (Info->VerticalResolution);
          LogoResolution.LogoResolutionX = Info->HorizontalResolution;
          LogoResolution.LogoResolutionY = Info->VerticalResolution;
          OemLogoResolutionPtr = &LogoResolution;
        }
        gBS->FreePool (Info);
      }

	    Status = gBS->LocateProtocol(&gOemServicesProtocolGuid, NULL, &OemServices);
	    if (!EFI_ERROR (Status)) {
	      OemServices->Funcs[COMMON_LOGO_RESOLUTION] (
	                   OemServices,
	                   COMMON_LOGO_RESOLUTION_ARG_COUNT,
	                   &OemLogoResolutionPtr
	                   );
	      SizeOfX = OemLogoResolutionPtr->LogoResolutionX;
	      SizeOfY = OemLogoResolutionPtr->LogoResolutionY;
		  }
    }
  }

  RequestedColumns = SizeOfX / GLYPH_WIDTH;
  RequestedRows = SizeOfY / GLYPH_HEIGHT;
  Status = ChkTextModeNum (RequestedColumns, RequestedRows, &TextModeNum);

Done:
  if (!EFI_ERROR (Status)) {
    if (gST->ConOut->Mode->Mode != TextModeNum) {
      Status = gST->ConOut->SetMode (gST->ConOut, TextModeNum);
    }
  } else {
    Status = gST->ConOut->SetMode (gST->ConOut, 0);
  }

}

VOID *
GetConOutVar (
  IN CHAR16 * Name
  )
{
  EFI_STATUS  Status;
  VOID        *Buffer;
  UINTN       BufferSize;

  BufferSize  = 0;
  Buffer      = NULL;

  //
  // Test to see if ConOut content exists
  //
  Status = gRT->GetVariable (
                  Name,
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &BufferSize,
                  Buffer
                  );

  if (Status == EFI_BUFFER_TOO_SMALL) {
    //
    // Allocate the buffer to return
    //
    Status = gBS->AllocatePool (EfiBootServicesData, BufferSize, &Buffer);
    if (EFI_ERROR (Status)) {
      return NULL;
    }
    //
    // Read variable into the allocated buffer.
    //
    Status = gRT->GetVariable (
                    Name,
                    &gEfiGlobalVariableGuid,
                    NULL,
                    &BufferSize,
                    Buffer
                    );
    if (EFI_ERROR (Status)) {
      gBS->FreePool (Buffer);
      Buffer = NULL;
    }
  }

  return Buffer;
}


BOOLEAN
FoundTextBasedConsole (
  VOID
  )
{
  EFI_DEVICE_PATH_PROTOCOL         *RemainingDevicePath;
  EFI_DEVICE_PATH_PROTOCOL         *NextDevPathInstance;
  EFI_STATUS                       Status;
  EFI_HANDLE                       Handle;
  EFI_GRAPHICS_OUTPUT_PROTOCOL     *Gop;
  BOOLEAN                          FoundTextBasedCon;
  UINTN                            Size;

  //
  //init local
  //
  RemainingDevicePath = NULL;
  Gop = NULL;
  FoundTextBasedCon = FALSE;
  NextDevPathInstance = NULL;

  RemainingDevicePath = GetConOutVar (L"ConOut");
  do {
    //
    //find all output console handles
    //
    NextDevPathInstance = EfiDevicePathInstance (&RemainingDevicePath, &Size);
    if (NextDevPathInstance != NULL) {
      gBS->LocateDevicePath (&gEfiDevicePathProtocolGuid, &NextDevPathInstance, &Handle);
      Status = gBS->HandleProtocol (
                      Handle,
                      &gEfiGraphicsOutputProtocolGuid,
                      (VOID*)&Gop
                      );
       if (EFI_ERROR (Status)) {
         //
         //found text-based console
         //
         FoundTextBasedCon = TRUE;
         break;
       }
    }
  } while (RemainingDevicePath != NULL);

  return FoundTextBasedCon;
}

BOOLEAN
IsSinglePhysicalGop (
  EFI_HANDLE  *SinglePhyGop
  )
{
  EFI_DEVICE_PATH_PROTOCOL              *GopDevicePath;
  EFI_STATUS                             Status;
  UINTN                                  HandleCount;
  EFI_HANDLE                             *HandleBuffer;
  UINTN                                  Index;
  UINTN                                  NumberOfPhysicalGop;

  NumberOfPhysicalGop = 0;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiGraphicsOutputProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID*)&GopDevicePath
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }
    *SinglePhyGop = HandleBuffer[Index];
    NumberOfPhysicalGop++;
  }
  gBS->FreePool (HandleBuffer);

  if (NumberOfPhysicalGop == 1) {
    return TRUE;
  } else {
    return FALSE;
  }
}

VOID
GetComboVideoOptimalResolution (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput,
  OUT UINT32                        *XResolution,
  OUT UINT32                        *YResoulution
  )
{
  UINT32                               ModeNumber;
  EFI_STATUS                           Status;
  UINTN                                SizeOfInfo;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;
  UINT32                               MaxMode;
  UINTN                                MaxResolution;
  UINTN                                TempResolution;

  Status  = EFI_SUCCESS;
  MaxMode = GraphicsOutput->Mode->MaxMode;
  MaxResolution = 0;
  TempResolution = 0;

  for (ModeNumber = 0; ModeNumber < MaxMode; ModeNumber++) {
    Status = GraphicsOutput->QueryMode (
                       GraphicsOutput,
                       ModeNumber,
                       &SizeOfInfo,
                       &Info
                       );
    if (!EFI_ERROR (Status)) {
      TempResolution = (Info->HorizontalResolution) * (Info->VerticalResolution);
      if (TempResolution > MaxResolution) {
        MaxResolution = TempResolution;
        *XResolution = Info->HorizontalResolution;
        *YResoulution = Info->VerticalResolution;
      }
      gBS->FreePool (Info);
    }
  }
}
