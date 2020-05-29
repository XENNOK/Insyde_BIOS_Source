/** @file
  This module install ACPI Windows Platform Binary Table (WPBT).

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <PiDxe.h>

#include "WpbtDxe.h"

#include <Protocol/AcpiTable.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/AcpiWpbt.h>

#include <Guid/EventGroup.h>
#include <Guid/EventLegacyBios.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>

//
// ACPI table information used to initialize tables.
//
#define EFI_ACPI_OEM_ID            'I','N','S','Y','D','E'
#define EFI_ACPI_OEM_TABLE_ID      SIGNATURE_64('H','2','O',' ','B','I','O','S')
#define EFI_ACPI_OEM_REVISION      0x00000001
#define EFI_ACPI_CREATOR_ID        SIGNATURE_32('A','C','P','I')
#define EFI_ACPI_CREATOR_REVISION  0x00040000

//
// Module globals.
//
UINTN         mPlatformBinaryResourceTableKey = 0;
//
// WPBT conent
//
EFI_GUID      mFileGuid;
UINT16        mArgLength;

BOOLEAN  mAcpiWpbtInstalled = FALSE;

EFI_ACPI_5_0_PLATFORM_BINARY_TABLE mPlatformBinaryTableTemplate = {
  {
    EFI_ACPI_5_0_PLATFORM_BINARY_TABLE_SIGNATURE,
    sizeof (EFI_ACPI_5_0_PLATFORM_BINARY_TABLE),
    EFI_ACPI_5_0_PLATFORM_BINARY_TABLE_REVISION,     // Revision
    0x00,                                            // Checksum will be updated at runtime
    //
    // It is expected that these values will be updated at runtime.
    //
    EFI_ACPI_OEM_ID,                                 // OEMID is a 6 bytes long field
    EFI_ACPI_OEM_TABLE_ID,                           // OEM table identification(8 bytes long)
    EFI_ACPI_OEM_REVISION,                           // OEM revision number
    EFI_ACPI_CREATOR_ID,                             // ASL compiler vendor ID
    EFI_ACPI_CREATOR_REVISION,                       // ASL compiler revision number
  },
  0,                                                 // BinarySize
  0,                                                 // BinaryLocation
  EFI_ACPI_5_0_WPBT_CONTENT_LAYOUT,                  // Content Layout
  EFI_ACPI_5_0_WPBT_CONTENT_TYPE,                    // Content Type
  0                                                  // ArgLength
};

EFI_ACPI_5_0_PLATFORM_BINARY_TABLE *mPlatformBinaryTable;    // dynamically allocated for variable-length arg


EFI_STATUS
EFIAPI
LocatePlatformBinary (
  IN EFI_ACPI_WPBT_PROTOCOL            *This,
  IN EFI_GUID                          *FileGuid,
  IN UINT16                            ArgLength,
  IN CHAR16                            *Arg
  );

EFI_ACPI_WPBT_PROTOCOL  mAcpiWpbtProtocolTemplate = { LocatePlatformBinary };

/**
 Function of EFI_ACPI_WPBT_PROTOCOL,
 for sample driver to pass the file guid of binary.

 @param [in]   This
 @param [in]   FileGuid
 @param [in]   ArgLength        The length (in bytes) of the input string for the binary execution
 @param [in]   Arg

 @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
LocatePlatformBinary (
  IN EFI_ACPI_WPBT_PROTOCOL            *This,
  IN EFI_GUID                          *FileGuid,
  IN UINT16                            ArgLength,
  IN CHAR16                            *Arg
  )
{
  CHAR16    *PtrArg;
  CopyGuid (&mFileGuid, FileGuid);

  //
  // ArgLength should be multiples of sizeof (CHAR16)
  //
  if ((ArgLength % (sizeof (CHAR16))) != 0) {
    DEBUG ((EFI_D_ERROR, "Invalid ArgLength: (%d) \n", ArgLength));
    return EFI_INVALID_PARAMETER;
  }

  mArgLength = ArgLength;

  //
  // allocate real PlatformBinaryTable
  //
  gBS->AllocatePool (
         EfiRuntimeServicesData,
         (sizeof (EFI_ACPI_5_0_PLATFORM_BINARY_TABLE) + mArgLength),
         (VOID **)&mPlatformBinaryTable
         );

  CopyMem (mPlatformBinaryTable, &mPlatformBinaryTableTemplate, sizeof (EFI_ACPI_5_0_PLATFORM_BINARY_TABLE));
  //
  // update Header->Length
  //
  mPlatformBinaryTable->Header.Length = (UINT32)(sizeof (EFI_ACPI_5_0_PLATFORM_BINARY_TABLE) + mArgLength);

  if (mArgLength > 0) {
    PtrArg = (CHAR16 *)((UINT8 *)(mPlatformBinaryTable) + sizeof (EFI_ACPI_5_0_PLATFORM_BINARY_TABLE));
    CopyMem (PtrArg, Arg, mArgLength);
  }  //end if (>0)

  return EFI_SUCCESS;
}

/**
 This function calculates and updates an UINT8 checksum.

 @param [in]   Buffer
 @param [in]   Size             Number of bytes to checksum.

 @retval None

**/
VOID
WpbtAcpiTableChecksum (
  IN UINT8      *Buffer,
  IN UINTN      Size
  )
{
  UINTN ChecksumOffset;

  ChecksumOffset = OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER, Checksum);

  //
  // Set checksum to 0 first.
  //
  Buffer[ChecksumOffset] = 0;

  //
  // Update checksum value.
  //
  Buffer[ChecksumOffset] = CalculateCheckSum8 (Buffer, Size);
}


/**
 Install Windows Platform Binary Table to ACPI table.

 @param None

 @return Status code

**/
EFI_STATUS
InstallWindowsPlatformBinaryTable (
  VOID
  )
{
  EFI_STATUS                    Status;
  EFI_ACPI_TABLE_PROTOCOL       *AcpiTableProtocol;

  UINTN                         FvHandleCount;
  EFI_HANDLE                    *FvHandleBuffer;
  UINTN                         Index;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv;

  UINT8                         *LoadBuffer;
  UINTN                         Size;
  UINT32                        AuthenticationStatus;
  UINT8                         Temp;
  EFI_GUID                      ZeroGuid;

  //
  // variables initialization
  //
  LoadBuffer = &Temp;
  Size = 1;
  Fv = NULL;
  ZeroMem (&ZeroGuid, sizeof (EFI_GUID));

  if (mAcpiWpbtInstalled) {
    return EFI_SUCCESS;
  }

  if (CompareGuid (&ZeroGuid, &mFileGuid)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &AcpiTableProtocol);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Locate AcpiTableProtocol failed: (%r) \n", Status));
    return Status;
  }

  //
  // Locate binary file location
  //
  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiFirmwareVolume2ProtocolGuid,
         NULL,
         &FvHandleCount,
         &FvHandleBuffer
         );

  for (Index = 0; Index < FvHandleCount; Index++) {
    gBS->HandleProtocol (
           FvHandleBuffer[Index],
           &gEfiFirmwareVolume2ProtocolGuid,
           (VOID **) &Fv
           );

    //
    // the binary file is encapsulated as EFI_SECTION_RAW
    //
    Status = Fv->ReadSection (
                  Fv,
                  &mFileGuid,
                  EFI_SECTION_RAW,
                  0,
                  (VOID **)&LoadBuffer,
                  &Size,
                  &AuthenticationStatus
                  );

    if (Status == EFI_WARN_BUFFER_TOO_SMALL || Status == EFI_BUFFER_TOO_SMALL) {

      LoadBuffer = NULL;
      Status = gBS->AllocatePool (
		      EfiACPIReclaimMemory,
		      Size,
		      (VOID **)&LoadBuffer
		      );

      Status = Fv->ReadSection (
                     Fv,
                     &mFileGuid,
                     EFI_SECTION_RAW,
                     0,
                     (VOID **)&LoadBuffer,
                     &Size,
                     &AuthenticationStatus
                     );
      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "ReadSection with LoadBuffer failed: (%r) \n", Status));
        return Status;
      }
    }  //end if (BUFFER_TOO_SMALL)

    if (!EFI_ERROR (Status)) {
      break;
    }
  }  //end for (Index)

  //
  // can't read the file
  //
  if (Index == FvHandleCount) {
    DEBUG ((EFI_D_ERROR, "Can not ReadFile: (%r) \n", Status));
    return Status;
  }

  //
  // initialize WPBT members
  //
  mPlatformBinaryTable->BinarySize = (UINT32)Size;
  mPlatformBinaryTable->BinaryLocation = (UINT64)(UINTN)LoadBuffer;
  mPlatformBinaryTable->Layout = EFI_ACPI_5_0_WPBT_CONTENT_LAYOUT;
  mPlatformBinaryTable->Type = EFI_ACPI_5_0_WPBT_CONTENT_TYPE;
  mPlatformBinaryTable->ArgLength = mArgLength;
  //
  // Update Checksum.
  //
  WpbtAcpiTableChecksum ((UINT8 *)mPlatformBinaryTable, (sizeof (EFI_ACPI_5_0_PLATFORM_BINARY_TABLE) + mPlatformBinaryTable->ArgLength));

  //
  // Publish Windows Platform Binary Table.
  //
  Status = AcpiTableProtocol->InstallAcpiTable (
                                AcpiTableProtocol,
                                mPlatformBinaryTable,
                                (sizeof (EFI_ACPI_5_0_PLATFORM_BINARY_TABLE) + mPlatformBinaryTable->ArgLength),
                                &mPlatformBinaryResourceTableKey
                                );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "InstallAcpiTable failed: (%r) \n", Status));
    return Status;
  }

  mAcpiWpbtInstalled = TRUE;

  gBS->FreePool (mPlatformBinaryTable);
  mPlatformBinaryTable = NULL;

  return Status;
}


/**
 Uninstall Windows Platform Binary Table to ACPI table.

 @param None

 @return Status code

**/
EFI_STATUS
UninstallWindowsPlatformBinaryTable (
  VOID
  )
{
  EFI_STATUS                    Status;
  EFI_ACPI_TABLE_PROTOCOL       *AcpiTableProtocol;
  //
  // Get ACPI Table protocol.
  //
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &AcpiTableProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (mAcpiWpbtInstalled == TRUE) {
    Status = AcpiTableProtocol->UninstallAcpiTable (
                                  AcpiTableProtocol,
                                  mPlatformBinaryResourceTableKey
                                  );
  }

  if (mPlatformBinaryTable) {
    gBS->FreePool (mPlatformBinaryTable);
  }

  return Status;
}


/**
 Notify function for event group EFI_EVENT_GROUP_READY_TO_BOOT.
 This is used to install the Windows Platform Binary Table.

 @param [in]   Event            The registered event
 @param [in]   Context

 @retval None

**/
VOID
EFIAPI
WpbtReadyToBootEventNotify (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  InstallWindowsPlatformBinaryTable ();

  gBS->CloseEvent (Event);
}


/**
 Notify function for event EFI_EVENT_LEGACY_BOOT.
 This is used to install the Windows Platform Binary Table.

 @param [in]   Event            The registered event
 @param [in]   Context

 @retval None

**/
VOID
EFIAPI
WpbtLegacyBootEventNotify (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  UninstallWindowsPlatformBinaryTable ();

  gBS->CloseEvent (Event);
}


/**
 The module Entry Point of the Windows Platform Binary Table DXE driver.

 @param [in]   ImageHandle      The firmware allocated handle for the EFI image.
 @param [in]   SystemTable      A pointer to the EFI System Table.

 @return Status code

**/
EFI_STATUS
EFIAPI
PlatformBinaryDxeEntryPoint (
  IN EFI_HANDLE          ImageHandle,
  IN EFI_SYSTEM_TABLE    *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_EVENT   PlatformBinaryReadyToBootEvent;
  EFI_EVENT   PlatformBinaryLegacyBootEvent;
  EFI_HANDLE  PlatformBinaryHandle = NULL;

  //
  // initialize global variables
  //
  ZeroMem (&mFileGuid, sizeof (EFI_GUID));
  mArgLength = 0;
  mPlatformBinaryTable = NULL;

  //
  // Install Platform Binary protocol.
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &PlatformBinaryHandle,
                  &gEfiAcpiWpbtProtocolGuid,
                  &mAcpiWpbtProtocolTemplate,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Register notify function to install WPBT on ReadyToBoot Event.
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  WpbtReadyToBootEventNotify,
                  NULL,
                  &gEfiEventReadyToBootGuid,
                  &PlatformBinaryReadyToBootEvent
                  );
  ASSERT_EFI_ERROR (Status);


  //
  // According to the description of WPBT
  // that the WPBT only supported on pure Uefi system.
  // Removed the Table when boot to legacy system.
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  WpbtLegacyBootEventNotify,
                  NULL,
                  &gEfiEventLegacyBootGuid,
                  &PlatformBinaryLegacyBootEvent
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;

}
