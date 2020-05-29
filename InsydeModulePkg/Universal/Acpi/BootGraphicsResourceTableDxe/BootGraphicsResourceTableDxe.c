/** @file

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

/** 
  This module install ACPI Boot Graphics Resource Table (BGRT).

  Copyright (c) 2014, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#include <BootGraphicsResourceTableDxe.h>
#include <Protocol/AcpiSupport.h>

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
EFI_EVENT  mBootGraphicsReadyToBootEvent;
EFI_EVENT  mBootGraphicsLegacyBootEvent;
UINTN      mBootGraphicsResourceTableKey = 0;

EFI_HANDLE                     mBootLogoHandle = NULL;
BOOLEAN                        mIsLogoValid = FALSE;
BOOLEAN                        mIsMonitor = FALSE;
EFI_GRAPHICS_OUTPUT_BLT_PIXEL  *mLogoBltBuffer = NULL;
UINTN                          mLogoDestX = 0;
UINTN                          mLogoDestY = 0;
UINTN                          mLogoWidth = 0;
UINTN                          mLogoHeight = 0;

BMP_IMAGE_HEADER  mBmpImageHeaderTemplate = {
  'B',    // CharB
  'M',    // CharM
  0,      // Size will be updated at runtime
  {0, 0}, // Reserved
  sizeof (BMP_IMAGE_HEADER), // ImageOffset
  sizeof (BMP_IMAGE_HEADER) - OFFSET_OF (BMP_IMAGE_HEADER, HeaderSize), // HeaderSize
  0,      // PixelWidth will be updated at runtime
  0,      // PixelHeight will be updated at runtime
  1,      // Planes
  24,     // BitPerPixel
  0,      // CompressionType
  0,      // ImageSize will be updated at runtime
  0,      // XPixelsPerMeter
  0,      // YPixelsPerMeter
  0,      // NumberOfColors
  0       // ImportantColors
};

BOOLEAN  mAcpiBgrtInstalled = FALSE;

EFI_ACPI_5_0_BOOT_GRAPHICS_RESOURCE_TABLE mBootGraphicsResourceTableTemplate = {
  {
    EFI_ACPI_5_0_BOOT_GRAPHICS_RESOURCE_TABLE_SIGNATURE,
    sizeof (EFI_ACPI_5_0_BOOT_GRAPHICS_RESOURCE_TABLE),
    EFI_ACPI_5_0_BOOT_GRAPHICS_RESOURCE_TABLE_REVISION,     // Revision
    0x00,  // Checksum will be updated at runtime
    //
    // It is expected that these values will be updated at runtime.
    //
    EFI_ACPI_OEM_ID,            // OEMID is a 6 bytes long field
    EFI_ACPI_OEM_TABLE_ID,      // OEM table identification(8 bytes long)
    EFI_ACPI_OEM_REVISION,      // OEM revision number
    EFI_ACPI_CREATOR_ID,        // ASL compiler vendor ID
    EFI_ACPI_CREATOR_REVISION,  // ASL compiler revision number
  },
  EFI_ACPI_5_0_BGRT_VERSION,         // Version
  EFI_ACPI_5_0_BGRT_STATUS_VALID,    // Status
  EFI_ACPI_5_0_BGRT_IMAGE_TYPE_BMP,  // Image Type
  0,                                 // Image Address
  0,                                 // Image Offset X
  0                                  // Image Offset Y
};

HOOKED_GOP_BLT_SET_MODE                 *mGopHookedList = NULL;
UINTN                                   mNumberofGops = 0;
UINT32                                  mLogoResolutionX = 0;
UINT32                                  mLogoResolutionY = 0;
BOOLEAN                                 mIsUpdateCoordinate = FALSE;
UINTN                                   mCurNoGopDrvBinding = 0;
HOOKED_GOP_DRV_BINDING                  *mGopBindingHookedList = NULL;

/**
  Update information of logo image drawn on screen.

  @param  This           The pointer to the Boot Logo protocol instance.
  @param  BltBuffer      The BLT buffer for logo drawn on screen. If BltBuffer
                         is set to NULL, it indicates that logo image is no
                         longer on the screen.
  @param  DestinationX   X coordinate of destination for the BltBuffer.
  @param  DestinationY   Y coordinate of destination for the BltBuffer.
  @param  Width          Width of rectangle in BltBuffer in pixels.
  @param  Height         Hight of rectangle in BltBuffer in pixels.

  @retval EFI_SUCCESS             The boot logo information was updated.
  @retval EFI_INVALID_PARAMETER   One of the parameters has an invalid value.
  @retval EFI_OUT_OF_RESOURCES    The logo information was not updated due to
                                  insufficient memory resources.

**/
EFI_STATUS
EFIAPI
SetBootLogo (
  IN EFI_BOOT_LOGO_PROTOCOL            *This,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL     *BltBuffer       OPTIONAL,
  IN UINTN                             DestinationX,
  IN UINTN                             DestinationY,
  IN UINTN                             Width,
  IN UINTN                             Height
  );

EFI_STATUS
EFIAPI
DummyBlt (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL            * This,
  IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL           * BltBuffer, OPTIONAL
  IN  EFI_GRAPHICS_OUTPUT_BLT_OPERATION       BltOperation,
  IN  UINTN                                   SourceX,
  IN  UINTN                                   SourceY,
  IN  UINTN                                   DestinationX,
  IN  UINTN                                   DestinationY,
  IN  UINTN                                   Width,
  IN  UINTN                                   Height,
  IN  UINTN                                   Delta         OPTIONAL
  );

EFI_STATUS
UninstallBootGraphicsResourceTable (
  VOID
  );

VOID
EFIAPI
GopInstallCallback (
  IN EFI_EVENT                                Event,
  IN VOID                                     *Context
  );

EFI_STATUS
EFIAPI
DummySetMode (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL * This,
  IN  UINT32                       ModeNumber
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

VOID
UpdateLogoPosition (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL         *GraphicsOutput
  );

VOID
HookGopStartFun (
  VOID
  );

EFI_STATUS
EFIAPI
DummyGopBindingStart (
  IN  EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  EFI_HANDLE                   ControllerHandle,
  IN  EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath  OPTIONAL
  );

VOID
CheckGopHook (
  VOID
  );

EFI_BOOT_LOGO_PROTOCOL  mBootLogoProtocolTemplate = { SetBootLogo };

/**
  Update information of logo image drawn on screen.

  @param  This           The pointer to the Boot Logo protocol instance.
  @param  BltBuffer      The BLT buffer for logo drawn on screen. If BltBuffer
                         is set to NULL, it indicates that logo image is no
                         longer on the screen.
  @param  DestinationX   X coordinate of destination for the BltBuffer.
  @param  DestinationY   Y coordinate of destination for the BltBuffer.
  @param  Width          Width of rectangle in BltBuffer in pixels.
  @param  Height         Hight of rectangle in BltBuffer in pixels.

  @retval EFI_SUCCESS             The boot logo information was updated.
  @retval EFI_INVALID_PARAMETER   One of the parameters has an invalid value.
  @retval EFI_OUT_OF_RESOURCES    The logo information was not updated due to
                                  insufficient memory resources.

**/
EFI_STATUS
EFIAPI
SetBootLogo (
  IN EFI_BOOT_LOGO_PROTOCOL            *This,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL     *BltBuffer       OPTIONAL,
  IN UINTN                             DestinationX,
  IN UINTN                             DestinationY,
  IN UINTN                             Width,
  IN UINTN                             Height
  )
{
  UINT64                                BufferSize;
  EFI_STATUS                            Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL          *GraphicsOutput;

  if (BltBuffer == NULL) {
    mIsLogoValid = FALSE;
    return EFI_SUCCESS;
  }

  if (Width == 0 || Height == 0) {
    return EFI_INVALID_PARAMETER;
  }

  if (mLogoBltBuffer != NULL) {
    FreePool (mLogoBltBuffer);
    mLogoBltBuffer = NULL;
  }
  
  //
  // Ensure the Height * Width doesn't overflow
  //
  if (Height > DivU64x64Remainder ((UINTN) ~0, Width, NULL)) {
    return EFI_UNSUPPORTED;
  }
  BufferSize = MultU64x64 (Width, Height);
  
  //
  // Ensure the BufferSize * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL) doesn't overflow
  //
  if (BufferSize > DivU64x32 ((UINTN) ~0, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL))) {
    return EFI_UNSUPPORTED;
  }

  mLogoBltBuffer = AllocateCopyPool (
                     (UINTN)BufferSize * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL),
                     BltBuffer
                     );
  if (mLogoBltBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  mLogoDestX = DestinationX;
  mLogoDestY = DestinationY;
  mLogoWidth = Width;
  mLogoHeight = Height;

  Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, (VOID **)&GraphicsOutput);
  if (!EFI_ERROR (Status)) {
    mLogoResolutionX = GraphicsOutput->Mode->Info->HorizontalResolution;
    mLogoResolutionY = GraphicsOutput->Mode->Info->VerticalResolution;
  }
  if (mNumberofGops == 0) {
    mIsLogoValid = FALSE;
  } else {
    mIsLogoValid = TRUE;
  }
  mIsMonitor = TRUE;
  return EFI_SUCCESS;
}

/**
  This function calculates and updates an UINT8 checksum.

  @param[in]  Buffer          Pointer to buffer to checksum.
  @param[in]  Size            Number of bytes to checksum.

**/
VOID
BgrtAcpiTableChecksum (
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
  Allocate EfiReservedMemoryType below 4G memory address.

  This function allocates EfiReservedMemoryType below 4G memory address.

  @param[in]  Size   Size of memory to allocate.

  @return Allocated address for output.

**/
VOID *
BgrtAllocateMemoryBelow4G (
  IN UINTN       Size
  )
{
  UINTN                 Pages;
  EFI_PHYSICAL_ADDRESS  Address;
  EFI_STATUS            Status;
  VOID                  *Buffer;

  Pages   = EFI_SIZE_TO_PAGES (Size);
  Address = 0xffffffff;
  //
  // According to the Acpi5.0, the memory type of Boot image must be the EfiBootServicesData!
  // and BGRT only is supported on Uefi system.
  // That's different to the EDKII implementation of BGRT.
  // The EDKII used the EfiReservedMemoryType memory type which can support the legacy and Uefi system, 
  // but it violate the Acpi5.0 spec.
  //
  Status = gBS->AllocatePages (
                  AllocateMaxAddress,
                  EfiBootServicesData,
                  Pages,
                  &Address
                  );
  ASSERT_EFI_ERROR (Status);

  Buffer = (VOID *) (UINTN) Address;
  ZeroMem (Buffer, Size);

  return Buffer;
}

/**
  Install Boot Graphics Resource Table to ACPI table.

  @return Status code.

**/
EFI_STATUS
InstallBootGraphicsResourceTable (
  VOID
  )
{
  EFI_STATUS                            Status;
  EFI_ACPI_TABLE_PROTOCOL               *AcpiTableProtocol;
  UINT8                                 *ImageBuffer;
  UINTN                                 PaddingSize;
  UINTN                                 BmpSize;
  UINTN                                 OrigBmpSize;
  UINT8                                 *Image;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL         *BltPixel;
  UINTN                                 Col;
  UINTN                                 Row;
  EFI_CONSOLE_CONTROL_PROTOCOL          *ConsoleControl;
  EFI_CONSOLE_CONTROL_SCREEN_MODE       CurrentMode;
  EFI_GRAPHICS_OUTPUT_PROTOCOL          *GraphicsOutput;
  
  //
  // Check whether Boot Graphics Resource Table is already installed.
  //
  if (mAcpiBgrtInstalled) {
    Status = UninstallBootGraphicsResourceTable ();
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  //
  // Get ACPI Table protocol.
  //
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &AcpiTableProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Check whether Logo exist.
  //
  if (mLogoBltBuffer == NULL) {
    Status = LogoLibSetBootLogo ();
    if (EFI_ERROR(Status)) {
      return Status;
    }
    mIsLogoValid = FALSE;
  }

  if (mBootGraphicsResourceTableTemplate.ImageAddress != 0) {
    //
    // reserve original BGRT buffer size
    //
    OrigBmpSize = mBmpImageHeaderTemplate.ImageSize + sizeof (BMP_IMAGE_HEADER);
    //
    // Free orignal BMP memory 
    // 
    if (mBootGraphicsResourceTableTemplate.ImageAddress) {
      gBS->FreePages(mBootGraphicsResourceTableTemplate.ImageAddress, EFI_SIZE_TO_PAGES (OrigBmpSize));
    }
    mBootGraphicsResourceTableTemplate.ImageAddress = 0;
  }

  //
  // Allocate memory for BMP file.
  //
  PaddingSize = mLogoWidth & 0x3;

  //
  // First check mLogoWidth * 3 + PaddingSize doesn't overflow
  //
  if (mLogoWidth > (((UINT32) ~0) - PaddingSize) / 3 ) {
    return EFI_UNSUPPORTED;
  }

  //
  // Second check (mLogoWidth * 3 + PaddingSize) * mLogoHeight + sizeof (BMP_IMAGE_HEADER) doesn't overflow
  //
  if (mLogoHeight > (((UINT32) ~0) - sizeof (BMP_IMAGE_HEADER)) / (mLogoWidth * 3 + PaddingSize)) {
    return EFI_UNSUPPORTED;
  }
    
  BmpSize = (mLogoWidth * 3 + PaddingSize) * mLogoHeight + sizeof (BMP_IMAGE_HEADER);
  ImageBuffer = BgrtAllocateMemoryBelow4G (BmpSize);
  if (ImageBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  mBmpImageHeaderTemplate.Size = (UINT32) BmpSize;
  mBmpImageHeaderTemplate.ImageSize = (UINT32) BmpSize - sizeof (BMP_IMAGE_HEADER);
  mBmpImageHeaderTemplate.PixelWidth = (UINT32) mLogoWidth;
  mBmpImageHeaderTemplate.PixelHeight = (UINT32) mLogoHeight;
  CopyMem (ImageBuffer, &mBmpImageHeaderTemplate, sizeof (BMP_IMAGE_HEADER));

  //
  // Convert BLT buffer to BMP file.
  //
  Image = ImageBuffer + sizeof (BMP_IMAGE_HEADER);
  for (Row = 0; Row < mLogoHeight; Row++) {
    BltPixel = &mLogoBltBuffer[(mLogoHeight - Row - 1) * mLogoWidth];

    if (BltPixel != NULL) { 
      for (Col = 0; Col < mLogoWidth; Col++) {
        *Image++ = BltPixel->Blue;
        *Image++ = BltPixel->Green;
        *Image++ = BltPixel->Red;
        BltPixel++;
      }
    }

    //
    // Padding for 4 byte alignment.
    //
    Image += PaddingSize;
  }
  //
  // Check Screen Mode
  //
  Status = gBS->LocateProtocol (&gEfiConsoleControlProtocolGuid, NULL, (VOID **)&ConsoleControl);
  if (!EFI_ERROR (Status)) {
    ConsoleControl->GetMode (ConsoleControl, &CurrentMode, NULL, NULL);
    if (CurrentMode != EfiConsoleControlScreenGraphics) {
      mIsLogoValid = FALSE;
    }
  }

  mBootGraphicsResourceTableTemplate.Status = (UINT8) (mIsLogoValid ? EFI_ACPI_5_0_BGRT_STATUS_VALID : EFI_ACPI_5_0_BGRT_STATUS_INVALID);
  mBootGraphicsResourceTableTemplate.ImageAddress = (UINT64) (UINTN) ImageBuffer;
  if (mIsUpdateCoordinate == FALSE) {
    Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, (VOID **)&GraphicsOutput);
    if (!EFI_ERROR (Status)) {
      UpdateLogoPosition (GraphicsOutput);
    }
    mIsUpdateCoordinate = TRUE;
  }
  mBootGraphicsResourceTableTemplate.ImageOffsetX = (UINT32) mLogoDestX;
  mBootGraphicsResourceTableTemplate.ImageOffsetY = (UINT32) mLogoDestY;

  //
  // Update Checksum.
  //
  BgrtAcpiTableChecksum ((UINT8 *) &mBootGraphicsResourceTableTemplate, sizeof (EFI_ACPI_5_0_BOOT_GRAPHICS_RESOURCE_TABLE));

  //
  // Publish Boot Graphics Resource Table.
  //
  Status = AcpiTableProtocol->InstallAcpiTable (
                                AcpiTableProtocol,
                                &mBootGraphicsResourceTableTemplate,
                                sizeof (EFI_ACPI_5_0_BOOT_GRAPHICS_RESOURCE_TABLE),
                                &mBootGraphicsResourceTableKey
                                );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  mAcpiBgrtInstalled = TRUE;
  return Status;
}

/**
  Uninstall Boot Graphics Resource Table to ACPI table.

  @return Status code.

**/
EFI_STATUS
UninstallBootGraphicsResourceTable (
  VOID
  )
{
  EFI_STATUS                    Status;
  EFI_ACPI_TABLE_PROTOCOL       *AcpiTableProtocol;
  EFI_ACPI_SUPPORT_PROTOCOL     *AcpiSupport;
  UINTN                         Index;
  EFI_ACPI_DESCRIPTION_HEADER   *Table;
  UINTN                         Handle;
  EFI_ACPI_TABLE_VERSION        Version;
  UINT8                         *BytePtr;
  UINT8                         *BytePtr2;
  BOOLEAN                       FoundBGRT;

  //
  //init local vars
  //
  AcpiSupport = NULL;
  Index = 0;
  Table = NULL;
  BytePtr = NULL;
  FoundBGRT = FALSE;
  
  //
  // found BGRT table
  //
  Status = gBS->LocateProtocol (
             &gEfiAcpiSupportProtocolGuid,
             NULL,
             (VOID **)&AcpiSupport
             );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  do {
    Status = AcpiSupport->GetAcpiTable (
                            AcpiSupport,
                            Index,
                            (VOID **)&Table,
                            &Version,
                            &Handle
                            );
    if (Table->Signature == EFI_ACPI_5_0_BOOT_GRAPHICS_RESOURCE_TABLE_SIGNATURE) {
      FoundBGRT = TRUE;
      break;
    }
    Index++;
  } while (Status != EFI_NOT_FOUND);

  if (FoundBGRT == FALSE) {
    return EFI_NOT_FOUND;
  }
  
  //
  //  keep OEM BGRT Information
  //
  BytePtr  = (UINT8*)&mBootGraphicsResourceTableTemplate;
  BytePtr2 = (UINT8*)Table;
  gBS->CopyMem (BytePtr + 10, BytePtr2 + 10, 28);

  //
  // Get ACPI Table protocol.
  //
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &AcpiTableProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (mAcpiBgrtInstalled == TRUE) {
    Status = AcpiTableProtocol->UninstallAcpiTable (
                                  AcpiTableProtocol, 
                                  mBootGraphicsResourceTableKey
                                  );
  }

  return Status;
}

/**
  Notify function for event group EFI_EVENT_GROUP_READY_TO_BOOT. This is used to
  install the Boot Graphics Resource Table.

  @param[in]  Event   The Event that is being processed.
  @param[in]  Context The Event Context.

**/
VOID
EFIAPI
BgrtReadyToBootEventNotify (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  InstallBootGraphicsResourceTable ();
}

/**
  Notify function for event EFI_EVENT_LEGACY_BOOT. This is used to
  uninstall the Boot Graphics Resource Table.

  @param[in]  Event   The Event that is being processed.
  @param[in]  Context The Event Context.

**/
VOID
EFIAPI
BgrtLegacyBootEventNotify (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  UninstallBootGraphicsResourceTable ();
}

/**
  The module Entry Point of the Boot Graphics Resource Table DXE driver.

  @param[in]  ImageHandle    The firmware allocated handle for the EFI image.
  @param[in]  SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS    The entry point is executed successfully.
  @retval Other          Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
BootGraphicsDxeEntryPoint (
  IN EFI_HANDLE          ImageHandle,
  IN EFI_SYSTEM_TABLE    *SystemTable
  )
{
  EFI_STATUS              Status;
  EFI_EVENT               GopInstallEvent;
  VOID                    *Registration;  

  //
  // Install Boot Logo protocol.
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mBootLogoHandle,
                  &gEfiBootLogoProtocolGuid,
                  &mBootLogoProtocolTemplate,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Register notify function to install BGRT on ReadyToBoot Event.
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  BgrtReadyToBootEventNotify,
                  NULL,
                  &gEfiEventReadyToBootGuid,
                  &mBootGraphicsReadyToBootEvent
                  );
  ASSERT_EFI_ERROR (Status);
  //
  // Create An Event to Hook Physical Gop->Blt
  //
  if (!EFI_ERROR(Status)) {
    Status = gBS->CreateEvent (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    GopInstallCallback,
                    NULL,
                    &GopInstallEvent
                    );
    if (!EFI_ERROR (Status)) {
      Status = gBS->RegisterProtocolNotify (
                      &gEfiGraphicsOutputProtocolGuid,
                      GopInstallEvent,
                      &Registration
                      );
    }
  }
  //
  // According to the description of BGRT in ACPI 5.0 spec 
  // that the BGRT only supported on Uefi system.
  // Removed the Table when boot to legacy system.
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  BgrtLegacyBootEventNotify,
                  NULL,
                  &gEfiEventLegacyBootGuid,
                  &mBootGraphicsLegacyBootEvent
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

EFI_STATUS
EFIAPI
DummyBlt (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL            *This,
  IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL           *BltBuffer, OPTIONAL
  IN  EFI_GRAPHICS_OUTPUT_BLT_OPERATION       BltOperation,
  IN  UINTN                                   SourceX,
  IN  UINTN                                   SourceY,
  IN  UINTN                                   DestinationX,
  IN  UINTN                                   DestinationY,
  IN  UINTN                                   Width,
  IN  UINTN                                   Height,
  IN  UINTN                                   Delta         OPTIONAL
  )
{
  EFI_STATUS                                  Status;
  UINTN                                       ListIndex;
  UINTN                                       SizeOfInfo;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION        *Info;

  Status = EFI_SUCCESS;
  
  if (mNumberofGops <= 0) {
    return EFI_NOT_STARTED;
  }

  for (ListIndex = 0; ListIndex < mNumberofGops; ListIndex++) {
    if (mGopHookedList[ListIndex].Gop == This) {
      if (BltOperation == EfiBltVideoFill) {
        if (mGopHookedList[ListIndex].BltWidth == 0 && mGopHookedList[ListIndex].BltHeight == 0) {
          // 
          // This indicates that screen has already been cleared when gop driver inits screen. 
          // So, Save initial clear screen's context.
          //
          This->QueryMode (This, This->Mode->Mode, &SizeOfInfo, &Info);
          mGopHookedList[ListIndex].BltWidth  = Info->HorizontalResolution;
          mGopHookedList[ListIndex].BltHeight = Info->VerticalResolution;
          mGopHookedList[ListIndex].BltColor = 0;
          mGopHookedList[ListIndex].BltX = 0;
          mGopHookedList[ListIndex].BltY = 0;
        }
        if (*(UINT32*)BltBuffer == mGopHookedList[ListIndex].BltColor &&
            DestinationX == mGopHookedList[ListIndex].BltX &&
            DestinationY == mGopHookedList[ListIndex].BltY &&
            Width == mGopHookedList[ListIndex].BltWidth &&
            Height == mGopHookedList[ListIndex].BltHeight) {
          //
          // Skip the operation if last operation same as this time
          //
          continue;
        } else {
          //
          // Record the last X, Y, W, H for next comparison
          //
          mGopHookedList[ListIndex].BltColor = *(UINT32*)BltBuffer;
          mGopHookedList[ListIndex].BltX = DestinationX;
          mGopHookedList[ListIndex].BltY = DestinationY;
          mGopHookedList[ListIndex].BltWidth = Width;
          mGopHookedList[ListIndex].BltHeight = Height;
        }
      } else {
        //
        // Destroy the record due to normal Blt be called
        //
        mGopHookedList[ListIndex].BltWidth = (UINTN)-1;
      }  
      Status = mGopHookedList[ListIndex].HookedBlt (
                                                This,
                                                BltBuffer,
                                                BltOperation,
                                                SourceX,
                                                SourceY,
                                                DestinationX,
                                                DestinationY,
                                                Width,
                                                Height,
                                                Delta
                                                );
    }
  }
  
  if (!EFI_ERROR (Status) && mIsMonitor == TRUE && mIsLogoValid == TRUE &&
      (DestinationY <= mLogoDestY + mLogoHeight)) {
    mIsLogoValid = FALSE;
    InstallBootGraphicsResourceTable ();
  }
  return Status;

}

VOID
EFIAPI
GopInstallCallback (
  IN EFI_EVENT                          Event,
  IN VOID                               *Context
  )
{
  CheckGopHook ();
  HookGopStartFun ();
}

EFI_STATUS
EFIAPI
DummySetMode (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL            *This,
  IN  UINT32                                  ModeNumber
  )
{
  EFI_STATUS                                  Status;
  UINTN                                       ListIndex;
  UINTN                                       SizeOfInfo;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION        *Info;

  Status = EFI_SUCCESS;
  
  if (mNumberofGops <= 0) {
    return EFI_NOT_STARTED;
  }

  for (ListIndex = 0; ListIndex < mNumberofGops; ListIndex++) {
    if (mGopHookedList[ListIndex].Gop == This) {
      Status = mGopHookedList[ListIndex].HookedSetMode (This,ModeNumber);
      //
      // Get current resolution used for reduce the redundancy clear screen
      //
      This->QueryMode (This, This->Mode->Mode, &SizeOfInfo, &Info);
      mGopHookedList[ListIndex].BltColor = 0;
      mGopHookedList[ListIndex].BltX = 0;
      mGopHookedList[ListIndex].BltY = 0;
      mGopHookedList[ListIndex].BltWidth  = Info->HorizontalResolution;
      mGopHookedList[ListIndex].BltHeight = Info->VerticalResolution;
    }
  }

  mIsLogoValid = FALSE;
  if ((!EFI_ERROR (Status)) && mIsUpdateCoordinate == TRUE) {
    UpdateLogoPosition (This);
    InstallBootGraphicsResourceTable ();
  }
  
  return Status;
}

VOID
UpdateLogoPosition (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL         *GraphicsOutput
  )
{

  if (mLogoResolutionX == GraphicsOutput->Mode->Info->HorizontalResolution &&
      mLogoResolutionY == GraphicsOutput->Mode->Info->VerticalResolution) {
    return;
  }

  BgrtGetLogoCoordinate (
    GraphicsOutput->Mode->Info->HorizontalResolution,
    GraphicsOutput->Mode->Info->VerticalResolution,
    mLogoWidth,
    mLogoHeight,
    &mLogoDestX,
    &mLogoDestY
    );

  mLogoResolutionX = GraphicsOutput->Mode->Info->HorizontalResolution;
  mLogoResolutionY = GraphicsOutput->Mode->Info->VerticalResolution;
  mIsLogoValid = FALSE;
}

VOID
HookGopStartFun (
  VOID
  )
{
  UINTN                                 NoHandles;
  EFI_HANDLE                            *HandleBuf;
  EFI_OPEN_PROTOCOL_INFORMATION_ENTRY   *OpenInfoBuffer;
  UINTN                                 EntryCount;
  UINTN                                 Index;
  UINTN                                 Index2;
  UINTN                                 Index3;
  EFI_HANDLE                            ControllerHandle;
  EFI_PCI_IO_PROTOCOL                   *PciIo;
  UINT8                                 BaseClassCode;
  EFI_HANDLE                            GopDrvImgHandle;
  EFI_STATUS                            Status;
  UINT32                                Attributes;
  EFI_DRIVER_BINDING_PROTOCOL           *TempGopDrvBinding;
  UINTN                                 CountDrvBinding;
  UINTN                                 AllocationSize;
  HOOKED_GOP_DRV_BINDING                *OldBindingHookedList;
  UINTN                                 NewCount;
  BOOLEAN                               FoundNewBinding;

  //
  //init locals
  //
  GopDrvImgHandle = NULL;
  TempGopDrvBinding = NULL;
  CountDrvBinding = 0;
  AllocationSize = 0;
  OldBindingHookedList = NULL;
  NewCount = 0;
  FoundNewBinding = FALSE;
  
  //
  // find gop driver's binding protocol
  //
  gBS->LocateHandleBuffer (
    AllHandles ,
    &gEfiPciIoProtocolGuid,
    NULL,
    &NoHandles,
    &HandleBuf
    );

  for (Index = 0; Index < NoHandles; Index++) {
    Status = gBS->OpenProtocolInformation (
                HandleBuf[Index],
                &gEfiPciIoProtocolGuid,
                &OpenInfoBuffer,
                &EntryCount
                );

    for (Index2 = 0; Index2 < EntryCount; Index2++) {
      ControllerHandle = OpenInfoBuffer[Index2].ControllerHandle;
      Attributes = OpenInfoBuffer[Index2].Attributes;
      
      if ((ControllerHandle != NULL) && (Attributes == EFI_OPEN_PROTOCOL_BY_DRIVER)) {
        gBS->HandleProtocol (
          ControllerHandle,
          &gEfiPciIoProtocolGuid,
          (VOID **)&PciIo
          );

        PciIo->Pci.Read (
          PciIo,
          EfiPciIoWidthUint8,
          0x0b,
          1,
          &BaseClassCode
          );

        if (BaseClassCode == 0x03) {
          GopDrvImgHandle = OpenInfoBuffer[Index2].AgentHandle;
          gBS->HandleProtocol (
            GopDrvImgHandle,
            &gEfiDriverBindingProtocolGuid,
            (VOID **)&TempGopDrvBinding
            );  

          if (mCurNoGopDrvBinding == 0 ) {
            NewCount = (mCurNoGopDrvBinding + 1);
            mGopBindingHookedList = AllocateZeroPool (sizeof (HOOKED_GOP_DRV_BINDING) * NewCount);
            ASSERT (mGopBindingHookedList != NULL);
            if (mGopBindingHookedList == NULL) {
              return ;
            }
            FoundNewBinding = TRUE;
          } else { 
            for (Index3 = 0; Index3 < mCurNoGopDrvBinding; Index3++) {
              if (mGopBindingHookedList[Index3].GopDriverBinding == TempGopDrvBinding) {
                break;
              }
            }
            if (Index3 == mCurNoGopDrvBinding) {
              if (mGopBindingHookedList[Index3].GopDriverBinding != TempGopDrvBinding) {
                NewCount = (mCurNoGopDrvBinding + 1);
                AllocationSize = sizeof (HOOKED_GOP_DRV_BINDING) * NewCount;
                OldBindingHookedList = mGopBindingHookedList;
                mGopBindingHookedList = (HOOKED_GOP_DRV_BINDING*)AllocateCopyPool (AllocationSize, mGopBindingHookedList);
                ASSERT (mGopBindingHookedList != NULL);
                if (mGopBindingHookedList == NULL) {
                  return ;
                }
                gBS->FreePool (OldBindingHookedList);
                FoundNewBinding = TRUE;
              }
            }
          }
          //
          // add new binding to tail
          //
          if (FoundNewBinding) { 
            mGopBindingHookedList[NewCount-1].GopDriverBinding = TempGopDrvBinding;
            mGopBindingHookedList[NewCount-1].HookedGopStartFun = TempGopDrvBinding->Start;
            TempGopDrvBinding->Start = DummyGopBindingStart;
            mCurNoGopDrvBinding = NewCount;
          }
        }
      }
    }
  } 

}

EFI_STATUS
EFIAPI
DummyGopBindingStart (
  IN  EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  EFI_HANDLE                   ControllerHandle,
  IN  EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath  OPTIONAL
  )
{
  EFI_STATUS                                  Status;
  UINTN                                       ListIndex;

  Status = EFI_SUCCESS;
  
  if (mNumberofGops <= 0) {
    return EFI_NOT_STARTED;
  }

  for (ListIndex = 0; ListIndex < mCurNoGopDrvBinding; ListIndex++) {
    if (mGopBindingHookedList[ListIndex].GopDriverBinding == This) {
        Status =  mGopBindingHookedList[ListIndex].HookedGopStartFun (
                    This,
                    ControllerHandle,
                    RemainingDevicePath
                    );
        
        CheckGopHook ();
    }
  }

  return Status;
}

VOID
CheckGopHook (
  VOID
  )
{
  EFI_HANDLE                                  *HandleBuffer;
  UINTN                                       NumberOfHandles;
  UINTN                                       Index;
  EFI_DEVICE_PATH_PROTOCOL                    *GopDevicePath;
  EFI_STATUS                                  Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL                *GraphicsOutput;
  HOOKED_GOP_BLT_SET_MODE                     *ThisGopHookedList;
  UINTN                                       GopCount;
  UINTN                                       ListIndex;
  BOOLEAN                                     NotInHookList;
  
  GopCount = 0;
 
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiGraphicsOutputProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return;
  }

  ThisGopHookedList = mGopHookedList;
  mGopHookedList = AllocateZeroPool (sizeof (HOOKED_GOP_BLT_SET_MODE) * NumberOfHandles);
  ASSERT (mGopHookedList != NULL);
  if (mGopHookedList == NULL) {
    return ;
  }
  
  for (Index = 0; Index < NumberOfHandles; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID*)&GopDevicePath
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }
    
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index], 
                    &gEfiGraphicsOutputProtocolGuid, 
                    (VOID **)&GraphicsOutput
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    NotInHookList = TRUE;
    
    for (ListIndex = 0; ListIndex < mNumberofGops; ListIndex++) {
      if (ThisGopHookedList[ListIndex].Gop == GraphicsOutput) {        
          NotInHookList = FALSE;
          mGopHookedList[GopCount].Gop = ThisGopHookedList[ListIndex].Gop;
          mGopHookedList[GopCount].HookedBlt = ThisGopHookedList[ListIndex].HookedBlt;
          mGopHookedList[GopCount].HookedSetMode = ThisGopHookedList[ListIndex].HookedSetMode;                 
          mGopHookedList[GopCount].BltColor = ThisGopHookedList[ListIndex].BltColor;
          mGopHookedList[GopCount].BltX = ThisGopHookedList[ListIndex].BltX;
          mGopHookedList[GopCount].BltY = ThisGopHookedList[ListIndex].BltY;
          mGopHookedList[GopCount].BltWidth = ThisGopHookedList[ListIndex].BltWidth;
          mGopHookedList[GopCount].BltHeight = ThisGopHookedList[ListIndex].BltHeight;        
     
        if(ThisGopHookedList[ListIndex].HookedBlt == GraphicsOutput->Blt &&
           ThisGopHookedList[ListIndex].HookedSetMode == GraphicsOutput->SetMode) {
          //
          // Need to rehook blt & setmode, since blt & setmode has been changed back by gop driver.
          //
          GraphicsOutput->Blt = DummyBlt;
          GraphicsOutput->SetMode = DummySetMode;        
        }
        GopCount++;
        break;
      }
    } 

    if (NotInHookList == TRUE) {
      mGopHookedList[GopCount].Gop = GraphicsOutput;
      mGopHookedList[GopCount].HookedBlt = GraphicsOutput->Blt;
      mGopHookedList[GopCount].HookedSetMode = GraphicsOutput->SetMode;      
      GraphicsOutput->Blt = DummyBlt;
      GraphicsOutput->SetMode = DummySetMode; 
      GopCount++;
    }
  }

  mNumberofGops = GopCount;
  if (ThisGopHookedList) {
    gBS->FreePool (ThisGopHookedList);
  }
  
  gBS->FreePool (HandleBuffer);

}
