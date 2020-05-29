/** @file
  This is part of the implementation of an Intel Graphics drivers OpRegion /
  Software SCI interface between system BIOS, ASL code, and Graphics drivers.
  The code in this file will load the driver and initialize the interface

  Supporting Specifiction: OpRegion / Software SCI SPEC 0.70

  Acronyms:
    IGD:        Internal Graphics Device
    NVS:        ACPI Non Volatile Storage
    OpRegion:   ACPI Operational Region
    VBT:        Video BIOS Table (OEM customizable data)

;******************************************************************************
;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

**/

///
/// Include files
///
#include "IgdOpRegion.h"
//#include "CpuIA32.h"
#include <CpuRegs.h>
#include <Library/CpuPlatformLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/SaPlatformPolicy.h>

///
/// Global variables
///
IGD_OPREGION_PROTOCOL                  mIgdOpRegion;
EFI_GUID                               mMiscSubClass = EFI_MISC_SUBCLASS_GUID;
EFI_EVENT                              mExitPmAuthEvent;
BOOLEAN                                mRunExitPmAuthRoutine;
SYSTEM_AGENT_GLOBAL_NVS_AREA_PROTOCOL  *mSaGlobalNvsArea;
DXE_PLATFORM_SA_POLICY_PROTOCOL        *mDxePlatformSaPolicy;

///
/// Function implementations.
///
EFI_STATUS
GetIntegratedIntelVbtPtr (
  OUT VBIOS_VBT_STRUCTURE **VbtFileBuffer
  )
/**
  Get VBT data using SaPlaformPolicy

  @param[in] VbtFileBuffer    Pointer to VBT data buffer.

  @retval EFI_SUCCESS      VBT data was returned.
  @retval EFI_NOT_FOUND    VBT data not found.
  @exception EFI_UNSUPPORTED  Invalid signature in VBT data.
**/
{
  EFI_STATUS                    Status;
  EFI_PHYSICAL_ADDRESS          VbtAddress;
  UINT32                        Size;

  ///
  /// Get the platform SA policy.
  ///
  Status = gBS->LocateProtocol (
                  &gDxePlatformSaPolicyGuid,
                  NULL,
                  (VOID **) &mDxePlatformSaPolicy
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  VbtAddress = mDxePlatformSaPolicy->IgdConfig->VbtAddress;
  Size       = mDxePlatformSaPolicy->IgdConfig->Size ;

  if (VbtAddress == 0x00000000) {
    return EFI_NOT_FOUND;
  } else {
    ///
    /// Check VBT signature
    ///
    *VbtFileBuffer  = NULL;
    *VbtFileBuffer = (VBIOS_VBT_STRUCTURE *) (UINTN) VbtAddress;
    if ((*((UINT32 *) ((*VbtFileBuffer)->HeaderSignature))) != VBT_SIGNATURE) {
      FreePool (*VbtFileBuffer);
      *VbtFileBuffer = NULL;
      return EFI_UNSUPPORTED;
    }
  }
  if (Size == 0) {
    return EFI_NOT_FOUND;
  } else {
    ///
    /// Check VBT size
    ///
    if ((*VbtFileBuffer)->HeaderVbtSize > Size) {
      (*VbtFileBuffer)->HeaderVbtSize = (UINT16) Size;
    }
  }
  return EFI_SUCCESS;
}

EFI_STATUS
GetIntegratedIntelVBiosPtr (
  OUT INTEL_VBIOS_OPTION_ROM_HEADER **VBiosImage
  )
/**
  Get a pointer to an uncompressed image of the Intel video BIOS.

  Note: This function would only be called if the video BIOS at 0xC000 is
        missing or not an Intel video BIOS.  It may not be an Intel video BIOS
        if the Intel graphic contoller is considered a secondary adapter.

  @param[in] VBiosImage   - Pointer to an uncompressed Intel video BIOS.  This pointer must
                            be set to NULL if an uncompressed image of the Intel Video BIOS
                            is not obtainable.

  @retval EFI_SUCCESS        - VBiosPtr is updated.
  @exception EFI_UNSUPPORTED - No Intel video BIOS found.
**/

{
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         HandleCount;
  UINTN                         Index;
  INTEL_VBIOS_PCIR_STRUCTURE    *PcirBlockPtr;
  EFI_STATUS                    Status;
  EFI_PCI_IO_PROTOCOL           *PciIo;
  INTEL_VBIOS_OPTION_ROM_HEADER *VBiosRomImage;
  ///
  /// Set as if an umcompressed Intel video BIOS image was not obtainable.
  ///
  VBiosRomImage = NULL;

  ///
  /// Get all PCI IO protocols
  ///
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Find the video BIOS by checking each PCI IO handle for an Intel video
  /// BIOS OPROM.
  ///
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiPciIoProtocolGuid,
                    (VOID **) &PciIo
                    );
    ASSERT_EFI_ERROR (Status);

    VBiosRomImage = PciIo->RomImage;

    ///
    /// If this PCI device doesn't have a ROM image, skip to the next device.
    ///
    if (!VBiosRomImage) {
      continue;
    }
    ///
    /// Get pointer to PCIR structure
    ///
    PcirBlockPtr = (INTEL_VBIOS_PCIR_STRUCTURE *) ((UINT8 *) VBiosRomImage + VBiosRomImage->PcirOffset);

    ///
    /// Check if we have an Intel video BIOS OPROM.
    ///
    if ((VBiosRomImage->Signature == OPTION_ROM_SIGNATURE) &&
        (PcirBlockPtr->VendorId == V_SA_MC_VID) &&
        (PcirBlockPtr->ClassCode[0] == 0x00) &&
        (PcirBlockPtr->ClassCode[1] == 0x00) &&
        (PcirBlockPtr->ClassCode[2] == 0x03)
        ) {
      ///
      /// Found Intel video BIOS.
      ///
      *VBiosImage = VBiosRomImage;
      return EFI_SUCCESS;
    }
  }
  ///
  /// No Intel video BIOS found.
  ///
  ///
  /// Free any allocated buffers
  ///
  FreePool (HandleBuffer);
  return EFI_UNSUPPORTED;
}

EFI_STATUS
GetVBiosVbtExitPmAuth (
  VOID
  )
/**
  Get Intel video BIOS VBT information (i.e. Pointer to VBT and VBT size).
  The VBT (Video BIOS Table) is a block of customizable data that is built
  within the video BIOS and edited by customers.

  @retval EFI_SUCCESS            - Video BIOS VBT information returned.
  @exception EFI_UNSUPPORTED     - Could not find VBT information (*VBiosVbtPtr = NULL).
**/
{
  INTEL_VBIOS_PCIR_STRUCTURE    *PcirBlockPtr;
  UINT16                        PciVenderId;
  INTEL_VBIOS_OPTION_ROM_HEADER *VBiosPtr;
  VBIOS_VBT_STRUCTURE           *VBiosVbtPtr;
  EFI_LEGACY_BIOS_PROTOCOL      *LegacyBios;
  EFI_STATUS                    Status;
  VBIOS_VBT_STRUCTURE           *VbtFileBuffer;
  UINTN                         Index;

//[-start-120925-IB06150247-add]//
  VBiosVbtPtr   = NULL;
//[-end-120925-IB06150247-add]//
  if (!mRunExitPmAuthRoutine) {
    return EFI_SUCCESS;
  }

  ///
  /// Get the platform SA policy.
  ///
  Status = gBS->LocateProtocol (
                  &gDxePlatformSaPolicyGuid,
                  NULL,
                  (VOID **) &mDxePlatformSaPolicy
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  LegacyBios = NULL;
  VBiosPtr = NULL;
  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **) &LegacyBios);

  if (LegacyBios) {
    VBiosPtr      = (INTEL_VBIOS_OPTION_ROM_HEADER *) (UINTN) (VBIOS_LOCATION_PRIMARY);
    PcirBlockPtr  = (INTEL_VBIOS_PCIR_STRUCTURE *) ((UINT8 *) VBiosPtr + VBiosPtr->PcirOffset);
    PciVenderId   = PcirBlockPtr->VendorId;
    ///
    /// If the video BIOS is not at 0xC0000 or it is not an Intel video BIOS get
    /// the integrated Intel video BIOS (must be uncompressed).
    ///
    if ((VBiosPtr->Signature != OPTION_ROM_SIGNATURE) || (PciVenderId != V_SA_MC_VID)) {
      GetIntegratedIntelVBiosPtr (&VBiosPtr);
      if (VBiosPtr != NULL) {
        ///
        /// Video BIOS found.
        ///
        PcirBlockPtr  = (INTEL_VBIOS_PCIR_STRUCTURE *) ((UINT8 *) VBiosPtr + VBiosPtr->PcirOffset);
        PciVenderId   = PcirBlockPtr->VendorId;

        if ((VBiosPtr->Signature != OPTION_ROM_SIGNATURE) || (PciVenderId != V_SA_MC_VID)) {
          ///
          /// Intel video BIOS not found.
          ///
          VBiosVbtPtr = NULL;
//[-start-120925-IB06150247-modify]//
          Status = EFI_UNSUPPORTED;
//[-end-120925-IB06150247-modify]//
        }
      }
    }
  } else {
    ///
    /// No Video BIOS found, try to get VBT from FV.
    ///
    GetIntegratedIntelVbtPtr (&VbtFileBuffer);
    if (VbtFileBuffer != NULL) {
      ///
      /// Video BIOS not found, use VBT from SaPlatformPolicy
      ///
      DEBUG ((EFI_D_INFO, "VBT data found\n"));
      for (Index = 0; (mDxePlatformSaPolicy->IgdConfig->GopVersion[Index] != '\0'); Index++) {
      }
      Index = (Index+1)*2;
      CopyMem (mIgdOpRegion.OpRegion->Header.DVER, mDxePlatformSaPolicy->IgdConfig->GopVersion, Index);
      (gBS->CopyMem) (mIgdOpRegion.OpRegion->VBT.GVD1, VbtFileBuffer, VbtFileBuffer->HeaderVbtSize);
//[-start-120925-IB06150247-modify]//
      VBiosVbtPtr = VbtFileBuffer;
//[-start-130402-IB05400394-remove]//
//      FreePool (VbtFileBuffer);
//[-end-130402-IB05400394-remove]//
      Status =  EFI_SUCCESS;
//[-end-120925-IB06150247-modify]//
    }
  }

//[-start-120925-IB06150247-modify]//
  if (VBiosPtr == NULL) {
    Status = EFI_UNSUPPORTED;
  } else {
  DEBUG ((EFI_D_INFO, "VBIOS found at 0x%X\n", VBiosPtr));
    VBiosVbtPtr = (VBIOS_VBT_STRUCTURE *) ((UINT8 *) VBiosPtr + VBiosPtr->VbtOffset);
  }
//[-end-120925-IB06150247-modify]//

  if ((*((UINT32 *) (VBiosVbtPtr->HeaderSignature))) != VBT_SIGNATURE) {
    return EFI_UNSUPPORTED;
  }
  ///
  /// No PlatformGopPolicy.h in EDK II code
  ///
#if 0
  GetSVER (mIgdOpRegion.OpRegion->Header.SVER);
#endif
  DEBUG ((EFI_D_INFO, "System BIOS ID is %a\n", mIgdOpRegion.OpRegion->Header.SVER));

  ///
  /// Initialize Video BIOS version with its build number.
  ///
  mIgdOpRegion.OpRegion->Header.VVER[0] = VBiosVbtPtr->CoreBlockBiosBuild[0];
  mIgdOpRegion.OpRegion->Header.VVER[1] = VBiosVbtPtr->CoreBlockBiosBuild[1];
  mIgdOpRegion.OpRegion->Header.VVER[2] = VBiosVbtPtr->CoreBlockBiosBuild[2];
  mIgdOpRegion.OpRegion->Header.VVER[3] = VBiosVbtPtr->CoreBlockBiosBuild[3];
  (gBS->CopyMem) (mIgdOpRegion.OpRegion->VBT.GVD1, VBiosVbtPtr, VBiosVbtPtr->HeaderVbtSize);

  ///
  /// Return final status
  ///
  return EFI_SUCCESS;
}
///
/// No PlatformGopPolicy.h in EDK II code
///
#if 0
EFI_STATUS
GetSVER (
  OUT UINT8 *SVER
  )
/**
  Set the SVER (system BIOS ID) string with the system BIOS build number.

  @param[in] SVER  String to populate with system BIOS build number.

  @retval EFI_SUCCESS     The SVER string is populated.
  @exception EFI_UNSUPPORTED The SVER string is not populated.
**/
{
  EFI_SUBCLASS_TYPE1_HEADER *DataHeader;
  EFI_DATA_HUB_PROTOCOL     *DataHub;
  EFI_MISC_BIOS_VENDOR      *BiosVendor;
  UINTN                     Length;
  UINT64                    MonotonicCount;
  CHAR16                    *NewString;
  EFI_DATA_RECORD_HEADER    *Record;
  EFI_STATUS                Status;

  ///
  ///  Locate the data hub protocol.
  ///
  Status = gBS->LocateProtocol (
                  &gEfiDataHubProtocolGuid,
                  NULL,
                  (VOID **)&DataHub
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  ///  Get the BIOS ID from the data hub.
  ///
  MonotonicCount  = 0;
  Record          = NULL;

  do {
    ///
    /// Check each data class record
    ///
    Status = DataHub->GetNextRecord (DataHub, &MonotonicCount, NULL, &Record);
    if (Record->DataRecordClass == EFI_DATA_RECORD_CLASS_DATA) {
      ///
      /// Check for BIOS vendor information
      ///
      DataHeader = (EFI_SUBCLASS_TYPE1_HEADER *) (Record + 1);
      if (CompareGuid (&Record->DataRecordGuid, &mMiscSubClass) &&
          (DataHeader->RecordType == EFI_MISC_BIOS_VENDOR_RECORD_NUMBER)
          ) {
        BiosVendor = (EFI_MISC_BIOS_VENDOR *) (DataHeader + 1);

        ///
        /// Get the BIOS ID string from the HII database
        ///
        Status = GetStringFromToken (&Record->ProducerName, BiosVendor->BiosVersion, &NewString);
        ASSERT_EFI_ERROR (Status);

        ///
        /// Convert from Unicode to ASCII
        ///
        if (NewString != NULL) {
          Length = StrLen (NewString);
          ASSERT (Length <= SVER_SIZE);
          ASPrint (SVER, Length, "%s", NewString);
          return EFI_SUCCESS;
        }
      }
    }
  } while ((!EFI_ERROR (Status)) && (MonotonicCount != 0));

  ///
  /// We assume BIOS ID is present and required.
  /// If this is not the case, BIOS ID will need to be determined in another way.
  ///
  ASSERT_EFI_ERROR (EFI_UNSUPPORTED);
  return EFI_UNSUPPORTED;
}

#if (EFI_SPECIFICATION_VERSION < 0x0002000A)
EFI_STATUS
GetStringFromToken (
  IN      EFI_GUID                  *ProducerGuid,
  IN      STRING_REF                Token,
  OUT     CHAR16                    **String
  )
/**
  Acquire the string associated with the ProducerGuid and return it.

  @param[in] ProducerGuid - The Guid to search the HII database for
  @param[in] Token - The token value of the string to extract
  @param[in] String - The string that is extracted

  @retval EFI_SUCCESS       The function completed successfully
  @retval EFI_NOT_FOUND     The requested string was not found
**/
{
  EFI_STATUS        Status;
  UINT16            HandleBufferLength;
  EFI_HII_HANDLE    *HiiHandleBuffer;
  UINTN             StringBufferLength;
  UINTN             NumberOfHiiHandles;
  UINTN             Index;
  UINT16            Length;
  EFI_GUID          HiiGuid;
  EFI_HII_PROTOCOL  *Hii;

  HandleBufferLength  = 0x1000;
  HiiHandleBuffer     = NULL;

  ///
  /// Locate HII protocol
  ///
  Status = gBS->LocateProtocol (&gEfiHiiProtocolGuid, NULL, (VOID **)&Hii);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Get all the Hii handles
  ///
  HiiHandleBuffer = AllocateZeroPool (HandleBufferLength);

  Status          = Hii->FindHandles (Hii, &HandleBufferLength, HiiHandleBuffer);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Get the Hii Handle that matches the StructureNode->ProducerName
  ///
  NumberOfHiiHandles = HandleBufferLength / sizeof (EFI_HII_HANDLE);
  for (Index = 0; Index < NumberOfHiiHandles; Index++) {
    Length = 0;
    Status = ExtractDataFromHiiHandle (
              HiiHandleBuffer[Index],
              &Length,
              NULL,
              &HiiGuid
              );
    if (CompareGuid (ProducerGuid, &HiiGuid)) {
      break;
    }
  }
  ///
  /// Find the string based on the current language
  ///
  StringBufferLength  = 0x100;
  *String             = AllocateZeroPool (0x100);
  Status = Hii->GetString (
                  Hii,
                  HiiHandleBuffer[Index],
                  Token,
                  FALSE,
                  NULL,
                  &StringBufferLength,
                  *String
                  );

  (gBS->FreePool) (HiiHandleBuffer);

  if (EFI_ERROR (Status)) {
    (gBS->FreePool) (*String);
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}
#endif
#endif

EFI_STATUS
IgdOpRegionInit (
  VOID
  )
/**
  Graphics OpRegion / Software SCI driver installation function.

  @param[in] void         - None
  @retval EFI_SUCCESS     - The driver installed without error.
  @retval EFI_ABORTED     - The driver encountered an error and could not complete
                            installation of the ACPI tables.
**/

{
  EFI_HANDLE                    Handle;
  EFI_STATUS                    Status;
  ///
  ///  VBIOS_VBT_STRUCTURE           *VBiosVbtPtr;
  ///
  UINT32                        DwordData;
  EFI_CPU_IO_PROTOCOL           *CpuIo;
  UINT16                        Data16;
  UINT32                        Data32;
  UINT16                        PchAcpiBaseAddress;
  UINT16                        DeviceId;
  PCH_SERIES                    PchSeries;
  CPU_FAMILY                    CpuFamilyId;
  EFI_GUID                      DxePlatformSaPolicyGuid = DXE_PLATFORM_SA_POLICY_GUID;
  DXE_PLATFORM_SA_POLICY_PROTOCOL  *DxePlatformSaPolicy;

  CpuFamilyId = GetCpuFamily();

  ///
  /// Get the platform SA policy.
  ///
  Status = gBS->LocateProtocol (
                  &gDxePlatformSaPolicyGuid,
                  NULL,
                  (VOID **) &mDxePlatformSaPolicy
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ///
  ///  Locate the SA Global NVS Protocol.
  ///
  Status = gBS->LocateProtocol (
                  &gSaGlobalNvsAreaProtocolGuid,
                  NULL,
                  (VOID **) &mSaGlobalNvsArea
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Allocate an ACPI NVS memory buffer as the IGD OpRegion, zero initialize
  /// the first 1K, and set the IGD OpRegion pointer in the Global NVS
  /// area structure.
  ///
  Status = (gBS->AllocatePool) (EfiACPIMemoryNVS, sizeof (IGD_OPREGION_STRUC), (VOID **) &mIgdOpRegion.OpRegion);
  ASSERT_EFI_ERROR (Status);

  (gBS->SetMem) (mIgdOpRegion.OpRegion, 0x2000, 0);
  mSaGlobalNvsArea->Area->IgdOpRegionAddress = (UINT32) (UINTN) (mIgdOpRegion.OpRegion);

  ///
  /// If IGD is disabled return
  ///
  mRunExitPmAuthRoutine = TRUE;
  if (IgdMmPci32 (0) == 0xFFFFFFFF) {
    mRunExitPmAuthRoutine = FALSE;
    return EFI_SUCCESS;
  }

  PchSeries = GetPchSeries();
  ///
  /// Initialize OpRegion Header
  ///
  (gBS->CopyMem) (mIgdOpRegion.OpRegion->Header.SIGN, HEADER_SIGNATURE, sizeof (HEADER_SIGNATURE));
  ///
  /// Set OpRegion Size in KBs
  ///
  mIgdOpRegion.OpRegion->Header.SIZE = HEADER_SIZE / 1024;
  mIgdOpRegion.OpRegion->Header.OVER = (UINT32) (LShiftU64 (HEADER_OPREGION_VER, 16) + LShiftU64 (HEADER_OPREGION_REV, 8));

  ///
  /// Get CPU Flavor by reading System Agent's Device ID (B0:D1F:F0:R02)
  ///
  DeviceId = McD0PciCfg16 (R_SA_MC_DEVICE_ID);
  if (IS_SA_DEVICE_ID_MOBILE (DeviceId)) {
    ///
    /// For Mobile, all Mailbox are supported.
    ///
    mIgdOpRegion.OpRegion->Header.MBOX = HEADER_MBOX_SUPPORT_MOBILE;
  } else if (IS_SA_DEVICE_ID_DESKTOP (DeviceId) || IS_SA_DEVICE_ID_SERVER (DeviceId)) {
    ///
    /// For Desktop and Server, Mailbox 1/3/5 are not supported.
    ///
    mIgdOpRegion.OpRegion->Header.MBOX = HEADER_MBOX_SUPPORT_DESKTOP;
  } else {
    DEBUG ((EFI_D_ERROR, "Hang if unknown System Agent\n"));
    ASSERT (FALSE);
    ///
    /// Hang if unknown System Agent
    ///
  }
  ///
  /// Initialize OpRegion Mailbox 1 (Public ACPI Methods).
  ///
  /// @todo:  The initial setting of mailbox 1 fields is implementation specific.
  /// Adjust them as needed many even coming from user setting in setup.
  ///
  ///
  /// Initialize OpRegion Mailbox 3 (ASLE Interrupt and Power Conservation).
  ///
  /// @todo:  The initial setting of mailbox 3 fields is implementation specific.
  /// Adjust them as needed many even coming from user setting in setup.
  ///
  ///
  /// Do not initialize TCHE. This field is written by the graphics driver only.
  ///
  ///
  /// The ALSI field is generally initialized by ASL code by reading the embedded controller.
  ///
  if (mDxePlatformSaPolicy->Revision >= 5) {
    mIgdOpRegion.OpRegion->Header.PCON = mDxePlatformSaPolicy->IgdConfig->PlatformConfig;
    if (CpuFamilyId == EnumCpuHswUlt) {
      mIgdOpRegion.OpRegion->Header.PCON = mIgdOpRegion.OpRegion->Header.PCON | 0x2;
    }
  }
  mIgdOpRegion.OpRegion->MBox3.BCLP = BACKLIGHT_BRIGHTNESS;

  mIgdOpRegion.OpRegion->MBox3.PFIT = (FIELD_VALID_BIT | PFIT_STRETCH);

  ///
  /// Reporting to driver for VR IMON Calibration. Bits [5-1] values supported 14A to 31A.
  ///
  mIgdOpRegion.OpRegion->MBox3.PCFT = (mSaGlobalNvsArea->Area->GfxTurboIMON << 1) & 0x003E;

  ///
  /// Set Initial current Brightness
  ///
  mIgdOpRegion.OpRegion->MBox3.CBLV = (INIT_BRIGHT_LEVEL | FIELD_VALID_BIT);

  ///
  /// <EXAMPLE> Create a static Backlight Brightness Level Duty cycle Mapping Table
  /// Possible 20 entries (example used 10), each 16 bits as follows:
  /// [15] = Field Valid bit, [14:08] = Level in Percentage (0-64h), [07:00] = Desired duty cycle (0 - FFh).
  ///
  mIgdOpRegion.OpRegion->MBox3.BCLM[0] = (0x0000 + WORD_FIELD_VALID_BIT);  ///< 0%
  ///
  mIgdOpRegion.OpRegion->MBox3.BCLM[1] = (0x0A19 + WORD_FIELD_VALID_BIT);  ///< 10%
  mIgdOpRegion.OpRegion->MBox3.BCLM[2] = (0x1433 + WORD_FIELD_VALID_BIT);  ///< 20%
  mIgdOpRegion.OpRegion->MBox3.BCLM[3] = (0x1E4C + WORD_FIELD_VALID_BIT);  ///< 30%
  mIgdOpRegion.OpRegion->MBox3.BCLM[4] = (0x2866 + WORD_FIELD_VALID_BIT);  ///< 40%
  mIgdOpRegion.OpRegion->MBox3.BCLM[5] = (0x327F + WORD_FIELD_VALID_BIT);  ///< 50%
  mIgdOpRegion.OpRegion->MBox3.BCLM[6] = (0x3C99 + WORD_FIELD_VALID_BIT);  ///< 60%
  mIgdOpRegion.OpRegion->MBox3.BCLM[7] = (0x46B2 + WORD_FIELD_VALID_BIT);  ///< 70%
  mIgdOpRegion.OpRegion->MBox3.BCLM[8] = (0x50CC + WORD_FIELD_VALID_BIT);  ///< 80%
  mIgdOpRegion.OpRegion->MBox3.BCLM[9] = (0x5AE5 + WORD_FIELD_VALID_BIT);  ///< 90%
  mIgdOpRegion.OpRegion->MBox3.BCLM[10] = (0x64FF + WORD_FIELD_VALID_BIT);  ///< 100%

  mIgdOpRegion.OpRegion->MBox3.IUER = 0x00;

  Status = gBS->LocateProtocol (&DxePlatformSaPolicyGuid, NULL, (VOID **)&DxePlatformSaPolicy);
 
  if (!EFI_ERROR(Status)) {
    mIgdOpRegion.OpRegion->MBox3.IUER =  DxePlatformSaPolicy->IgdConfig->IuerStatusVal;
  }

  ///
  /// Initialize hardware state:
  ///   Set ASLS Register to the OpRegion physical memory address.
  ///   Set SWSCI register bit 15 to a "1" to activate SCI interrupts.
  ///
  IgdMmPci32 (R_SA_IGD_ASLS_OFFSET) = (UINT32) (UINTN) (mIgdOpRegion.OpRegion);
  IgdMmPci16AndThenOr (R_SA_IGD_SWSCI_OFFSET, ~(BIT0), BIT15);

  DwordData = IgdMmPci32 (R_SA_IGD_ASLS_OFFSET);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (MmPciAddress (0x0,
            SA_IGD_BUS,
            SA_IGD_DEV,
            SA_IGD_FUN_0,
            R_SA_IGD_ASLS_OFFSET)),
    1,
    &DwordData
    );
  DwordData = IgdMmPci32 (R_SA_IGD_SWSCI_OFFSET);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (MmPciAddress (0x0,
            SA_IGD_BUS,
            SA_IGD_DEV,
            SA_IGD_FUN_0,
            R_SA_IGD_SWSCI_OFFSET)),
    1,
    &DwordData
    );
  PchAcpiBaseAddress = MmPci16 (
                        0,
                        DEFAULT_PCI_BUS_NUMBER_PCH,
                        PCI_DEVICE_NUMBER_PCH_LPC,
                        PCI_FUNCTION_NUMBER_PCH_LPC,
                        R_PCH_LPC_ACPI_BASE
                        ) &~BIT0;

  ///
  /// Find the CPU I/O Protocol.  ASSERT if not found.
  ///
  Status = gBS->LocateProtocol (
                  &gEfiCpuIoProtocolGuid,
                  NULL,
                  (VOID **) &CpuIo
                  );
  ASSERT_EFI_ERROR (Status);

  CpuIo->Io.Read (
              CpuIo,
              EfiCpuIoWidthUint16,
              PchAcpiBaseAddress + PCH_TCO_BASE + R_PCH_TCO1_STS,
              1,
              &Data16
              );
  ///
  /// Clear the B_DMISCI_STS bit in R_TCO1_STS by writing a '1'.
  ///
  Data16 &= B_PCH_TCO1_STS_DMISCI;

  CpuIo->Io.Write (
              CpuIo,
              EfiCpuIoWidthUint16,
              PchAcpiBaseAddress + PCH_TCO_BASE + R_PCH_TCO1_STS,
              1,
              &Data16
              );

  if (PchSeries == PchLp) {
    ///
    /// Clear the ACPI TCO status.
    ///
    Data32 = B_PCH_ACPI_GPE0_STS_127_96_TC0SCI;
    CpuIo->Io.Write (
                CpuIo,
                EfiCpuIoWidthUint32,
                (UINT64) (PchAcpiBaseAddress + R_PCH_ACPI_GPE0_STS_127_96),
                1,
                &Data32
                );

    ///
    /// Enable ACPI TCO SCI's.
    ///
    CpuIo->Io.Read (
                CpuIo,
                EfiCpuIoWidthUint16,
                (UINT64) (PchAcpiBaseAddress + R_PCH_ACPI_GPE0_EN_127_96),
                1,
                &Data16
                );
    Data16 |= B_PCH_ACPI_GPE0_EN_127_96_TC0SCI;
    CpuIo->Io.Write (
                CpuIo,
                EfiCpuIoWidthUint16,
                (UINT64) (PchAcpiBaseAddress + R_PCH_ACPI_GPE0_EN_127_96),
                1,
                &Data16
                );
  } else if (PchSeries == PchH) {
    ///
    /// Clear the ACPI TCO status.
    ///
    Data32 = B_PCH_ACPI_GPE0a_STS_TC0SCI;
    CpuIo->Io.Write (
                CpuIo,
                EfiCpuIoWidthUint32,
                (UINT64) (PchAcpiBaseAddress + R_PCH_ACPI_GPE0a_STS),
                1,
                &Data32
                );

    ///
    /// Enable ACPI TCO SCI's.
    ///
    CpuIo->Io.Read (
                CpuIo,
                EfiCpuIoWidthUint16,
                (UINT64) (PchAcpiBaseAddress + R_PCH_ACPI_GPE0a_EN),
                1,
                &Data16
                );
    Data16 |= B_PCH_ACPI_GPE0a_EN_TC0SCI;
    CpuIo->Io.Write (
                CpuIo,
                EfiCpuIoWidthUint16,
                (UINT64) (PchAcpiBaseAddress + R_PCH_ACPI_GPE0a_EN),
                1,
                &Data16
                );
  }

  ///
  /// Install OpRegion / Software SCI protocol
  ///
  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gIgdOpRegionProtocolGuid,
                  &mIgdOpRegion,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Return final status
  ///
  return EFI_SUCCESS;
}


EFI_STATUS
UpdateIgdOpRegionExitPmAuth (
  VOID
  )
/**
  Update Graphics OpRegion after PCI enumeration.

  @param[in] void         - None
  @retval EFI_SUCCESS     - The function completed successfully.
**/
{
  EFI_STATUS                    Status;
  UINTN                         HandleCount;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         Index;
  EFI_PCI_IO_PROTOCOL           *PciIo;
  PCI_TYPE00                    Pci;
  UINTN                         Segment;
  UINTN                         Bus;
  UINTN                         Device;
  UINTN                         Function;

  Bus      = 0;
  Device   = 0;
  Function = 0;

  DEBUG ((EFI_D_INFO, "UpdateIgdOpRegionExitPmAuth\n"));

  mIgdOpRegion.OpRegion->Header.PCON |= BIT8; //Set External Gfx Adapter field is valid
  mIgdOpRegion.OpRegion->Header.PCON &= (UINT32) (~BIT7); //Assume No External Gfx Adapter 

  ///
  /// Get all PCI IO protocols handles
  ///
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );

  if (!EFI_ERROR (Status)) {
    for (Index = 0; Index < HandleCount; Index++) {
      ///
      /// Get the PCI IO Protocol Interface corresponding to each handle
      ///
      Status = gBS->HandleProtocol (
                      HandleBuffer[Index],
                      &gEfiPciIoProtocolGuid,
                      (VOID **) &PciIo
                      );

      if (!EFI_ERROR (Status)) {
        ///
        /// Read the PCI configuration space
        ///
        Status = PciIo->Pci.Read (
                          PciIo,
                          EfiPciIoWidthUint32,
                          0,
                          sizeof (Pci) / sizeof (UINT32),
                          &Pci
                          );

        ///
        /// Find the display controllers devices
        ///
        if (!EFI_ERROR (Status) && IS_PCI_DISPLAY (&Pci)) {
          Status = PciIo->GetLocation (
                            PciIo,
                            &Segment,
                            &Bus,
                            &Device,
                            &Function
                            );

          //
          // Assumption: Onboard devices will be sits on Bus no 0, while external devices will be sits on Bus no > 0
          //
          if (!EFI_ERROR (Status) && (Bus > 0)) {
            //External Gfx Adapter Detected and Available
            DEBUG ((EFI_D_INFO, "PCON - External Gfx Adapter Detected and Available\n"));
            mIgdOpRegion.OpRegion->Header.PCON |= BIT7;
            break;
		  }
        }
      }
    }
  }

  ///
  /// Free any allocated buffers
  ///
  if (HandleBuffer != NULL) {
    FreePool (HandleBuffer);
  }

  ///
  /// Return final status
  ///
  return Status;
}