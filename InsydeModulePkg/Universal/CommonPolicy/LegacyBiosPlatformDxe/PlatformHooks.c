/** @file
  The EFI Legacy BIOS Patform Protocol member function

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

#include "LegacyBiosPlatformDxe.h"

#include <Protocol/LegacyRegion.h>
#include <Library/DxeServicesLib.h>
#include <Library/DxeChipsetSvcLib.h>
#include <Protocol/GraphicsOutput.h>

#define NULL_ROM_FILE_GUID \
  { \
    0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 \
  }

EFI_GUID NullRomFileGuid = NULL_ROM_FILE_GUID;
BOOLEAN  mGetVideoOriginalModeOK = FALSE;
UINT32   mVideoOriginalMode = 0;

typedef struct {
  VOID  *BisSourceImage;
  VOID  *BisTargetImage;
  VOID  *Bis16Entry;
  VOID  *Bis32Entry;
} EFI_BIS16_PARAMETER;

static
EFI_STATUS
CallBisImage (
  IN  EFI_LEGACY_BIOS_PROTOCOL      *LegacyBios,
  IN  VOID                          *BisImage,
  IN  UINT32                        BisImageSize,
  IN  VOID                          *BisLoaderImage,
  IN  UINTN                         BisLoaderSize
  );

/**
  Allows platform to perform any required action after a LegacyBios operation.
  Invokes the specific sub function specified by Mode.

  @param  This                  The protocol instance pointer.
  @param  Mode                  Specifies what handle to return. See EFI_GET_PLATFORM_HOOK_MODE enum.
  @param  Type                  Mode specific.  See EFI_GET_PLATFORM_HOOK_MODE enum.
  @param  DeviceHandle          Mode specific.  See EFI_GET_PLATFORM_HOOK_MODE enum.
  @param  ShadowAddress         Mode specific.  See EFI_GET_PLATFORM_HOOK_MODE enum.
  @param  Compatibility16Table  Mode specific.  See EFI_GET_PLATFORM_HOOK_MODE enum.
  @param  AdditionalData        Mode specific.  See EFI_GET_PLATFORM_HOOK_MODE enum.

  @retval EFI_SUCCESS           The operation performed successfully. Mode specific.
  @retval EFI_UNSUPPORTED       Mode is not supported on the platform.

**/
EFI_STATUS
EFIAPI
PlatformHooks (
  IN      EFI_LEGACY_BIOS_PLATFORM_PROTOCOL     *This,
  IN      EFI_GET_PLATFORM_HOOK_MODE            Mode,
  IN      UINT16                                Type,
  OUT     EFI_HANDLE                            DeviceHandle, OPTIONAL
  IN OUT  UINTN                                 *ShadowAddress, OPTIONAL
  IN      EFI_COMPATIBILITY16_TABLE             *Compatibility16Table, OPTIONAL
  OUT     VOID                                  **AdditionalData OPTIONAL
  )
{
  EFI_LEGACY_BIOS_PROTOCOL         *LegacyBios;
  EFI_STATUS                       Status;
  VOID                             *BisLoaderImage;
  UINTN                            BisLoaderSize;
  UINTN                            LocalRomSize;
  UINTN                            Flags;
  VOID                             *LocalRomImage;
  UINTN                            FilenameIndex;
  SERVICE_ROM_TABLE                *ServiceRomTable;
  BOOLEAN                          ForceSwitchTextMode;
  EFI_IA32_REGISTER_SET            RegisterSet;
  EFI_GRAPHICS_OUTPUT_PROTOCOL     *GraphicsOutput;

  Status = gBS->LocateProtocol (
                  &gEfiLegacyBiosProtocolGuid,
                  NULL,
                  (VOID **)&LegacyBios
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  switch (Mode) {

  case EfiPlatformHookPrepareToScanRom:

    ForceSwitchTextMode = FALSE;    
    //
    // Check platform & oem policy for 'ForceSwitchTextMode'.
    //
    DxeCsSvcInstallPciRomSwitchTextMode (DeviceHandle, &ForceSwitchTextMode);
    
    //
    // OemServices
    //
    Status = OemSvcInstallPciRomSwitchTextMode (
               DeviceHandle,
               &ForceSwitchTextMode
               );
    
    //
    // If we need to switch to text mode, save original video mode and flag.
    //
    if (ForceSwitchTextMode) {
      //
      // Backup current video mode
      //
      Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, (VOID **)&GraphicsOutput);
      if (!EFI_ERROR (Status)) { 
        mVideoOriginalMode = GraphicsOutput->Mode->Mode;
        mGetVideoOriginalModeOK = TRUE;
      }
      //
      // Set the 80x25 Text VGA Mode before dispatch option ROM
      //
      RegisterSet.H.AH = 0x00;
      RegisterSet.H.AL = 0x03;
      Status    = LegacyBios->Int86 (LegacyBios, 0x10, &RegisterSet);
    }
    return EFI_SUCCESS;

  case EfiPlatformHookShadowServiceRoms:
    //
    // Get Service Rom table from OptionRom Table of OemServices
    //
    ServiceRomTable = NULL;
    Status = OemSvcInstallOptionRomTable (
               SERVICE_ROM,
               &ServiceRomTable
               );
    if (!EFI_ERROR (Status) || ServiceRomTable == NULL) {
      return EFI_NOT_FOUND;
    }
    //
    // Get Bis Loader image
    //
    BisLoaderImage  = NULL;
    BisLoaderSize   = 0;

    for (FilenameIndex = 0; !CompareGuid(&(ServiceRomTable[FilenameIndex].FileName), &NullRomFileGuid); FilenameIndex++) {
      if (ServiceRomTable[FilenameIndex].Valid) {
        continue;
      }
      if (ServiceRomTable[FilenameIndex].OpRomType == BIS_LOADER) {
        Status = GetSectionFromFv (
                   &ServiceRomTable[FilenameIndex].FileName, 
                   EFI_SECTION_RAW, 
                   0,
                   &BisLoaderImage, 
                   &BisLoaderSize
                   );
        
        ASSERT_EFI_ERROR (Status);
        if (EFI_ERROR (Status)) {
          continue;
        }
      }
    }

    for (FilenameIndex = 0; !CompareGuid (&(ServiceRomTable[FilenameIndex].FileName), &NullRomFileGuid); FilenameIndex++) {
      if (!ServiceRomTable[FilenameIndex].Valid) {
        continue;
      }
      if (ServiceRomTable[FilenameIndex].OpRomType != SERVICE_ROM) {
        continue;
      }
      LocalRomImage = NULL;
      LocalRomSize  = 0;

      Status = GetSectionFromFv (
                 &ServiceRomTable[FilenameIndex].FileName, 
                 EFI_SECTION_RAW, 
                 0, 
                 &LocalRomImage, 
                 &LocalRomSize
                 );

      if (!EFI_ERROR (Status)) {
        if (ServiceRomTable[FilenameIndex].OptionRomSpace) {
          LegacyBios->InstallPciRom (
                        LegacyBios,
                        NULL,
                        &LocalRomImage,
                        &Flags,
                        NULL,
                        NULL,
                        NULL,
                        NULL
                        );
        } else {
          //
          // Do non-option rom here like BIS
          //
          if (BisLoaderImage != NULL || BisLoaderSize != 0) {
            CallBisImage (
              LegacyBios,
              LocalRomImage,
              (UINT32)LocalRomSize,
              BisLoaderImage,
              BisLoaderSize
              );
          }
        }
        gBS->FreePool (LocalRomImage);
      }
    }

    if (BisLoaderImage != NULL) {
      gBS->FreePool (BisLoaderImage);
    }
    gBS->FreePool (ServiceRomTable);
    return EFI_SUCCESS;

  case EfiPlatformHookAfterRomInit:
    //
    // If we are in text mode now, switch back to original video mode.
    //
    if (mGetVideoOriginalModeOK) {
      Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, (VOID **)&GraphicsOutput);
      if (!EFI_ERROR (Status)) {
        GraphicsOutput->SetMode (GraphicsOutput, mVideoOriginalMode);
        mGetVideoOriginalModeOK = FALSE;
      }
    }   
    return EFI_SUCCESS;

  default:
    return EFI_UNSUPPORTED;
  }
}

/**
  Call 16-bit code.

  @param  BisImage              Pointer to BIS Image
  @param  BisImageSize          Size of BIS Image
  @param  BisLoaderImage        Pointer to BIS Loader
  @param  BisLoaderSize         Size of BIS Loader

  @retval EFI_SUCCESS           BIS dispatched

**/
static
EFI_STATUS
CallBisImage (
  IN  EFI_LEGACY_BIOS_PROTOCOL      *LegacyBios,
  IN  VOID                          *BisImage,
  IN  UINT32                        BisImageSize,
  IN  VOID                          *BisLoaderImage,
  IN  UINTN                         BisLoaderSize
  )
{
  UINT32                        Pages;
  EFI_PHYSICAL_ADDRESS          RelocatedBisImage;
  EFI_STATUS                    Status;
  EFI_IA32_REGISTER_SET         Regs;
  VOID                          *BisLoaderPointer = NULL;
  EFI_BIS16_PARAMETER           BisEntryParameters;
  EFI_LEGACY_REGION_PROTOCOL    *LegacyRegion;

  ZeroMem (&Regs, sizeof (EFI_IA32_REGISTER_SET));

  Status = LegacyBios->GetLegacyRegion (
                        LegacyBios,
                        (BisLoaderSize + 0xF),
                        E0000Region,  // 0xE0000 block
                        0x01,
                        &BisLoaderPointer
                        );
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }  

  Pages = EFI_SIZE_TO_PAGES (BisImageSize) + 1;
  RelocatedBisImage = 0xFFFFFFFF;
  Status = gBS->AllocatePages (
                  AllocateMaxAddress,
                  EfiACPIMemoryNVS,
                  Pages,
                  &RelocatedBisImage
                  );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol (
                  &gEfiLegacyRegionProtocolGuid,
                  NULL,
                  (VOID **)&LegacyRegion
                  );
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  LegacyRegion->UnLock (LegacyRegion, 0xE0000, 0x20000, NULL);
  CopyMem (BisLoaderPointer, BisLoaderImage, BisLoaderSize);

  //
  // Define entry parameter here
  //
  BisEntryParameters.BisSourceImage = BisImage;
  BisEntryParameters.BisTargetImage = (VOID *) (UINTN) RelocatedBisImage;
  BisEntryParameters.Bis16Entry     = 0;
  BisEntryParameters.Bis32Entry     = 0;
  Regs.X.DX                         = (UINT16) (((UINT32)(UINTN)&BisEntryParameters) >> 16);
  Regs.X.AX                         = (UINT16) ((UINT32)(UINTN)&BisEntryParameters);
  Status = LegacyBios->FarCall86 (
                         LegacyBios,
                         (((UINT16)(UINTN)BisLoaderPointer) >> 4),
                         0,
                         &Regs,
                         NULL,
                         0
                         );
  if (BisEntryParameters.Bis16Entry) {
    //
    // Log to SMBIOS
    //
  }

  if (BisEntryParameters.Bis32Entry) {
    //
    // Log to SMBIOS
    //
  }

  LegacyRegion->Lock (LegacyRegion, 0xE0000, 0x20000, NULL);
  return Status;
}

