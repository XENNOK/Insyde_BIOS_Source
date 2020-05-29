//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/**
  This is the driver that initializes the Intel System Agent.

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
#include "SaInit.h"
#include <Guid/SaDataHob.h>
#include <Guid/SaSsdtTableStorage.h>
#include <Protocol/PciEnumerationComplete.h>
#include <Protocol/SaGlobalNvsArea.h>
#include <Protocol/FirmwareVolume.h>

///
/// Global Variables
///
SYSTEM_AGENT_GLOBAL_NVS_AREA_PROTOCOL  mSaGlobalNvsAreaProtocol;
DXE_PLATFORM_SA_POLICY_PROTOCOL        *mDxePlatformSaPolicy;
UINT16                                 mSaIotrapSmiAddress;
BOOLEAN                                mInitPcieAspmAfterOprom;
EFI_GUID                               gEfiPciEnumerationCompleteProtocolGuid = EFI_PCI_ENUMERATION_COMPLETE_GUID;

EFI_STATUS
InitializeSaSsdtAcpiTables (
  VOID
  )
/**
@brief
  Initialize System Agent SSDT ACPI tables

  @retval EFI_SUCCESS    ACPI tables are initialized successfully
  @retval EFI_NOT_FOUND  ACPI tables not found
**/
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         NumberOfHandles;
  EFI_FV_FILETYPE               FileType;
  UINT32                        FvStatus;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINTN                         Size;
  UINTN                         i;
  EFI_FIRMWARE_VOLUME_PROTOCOL  *FwVol;
  INTN                          Instance;
  EFI_ACPI_COMMON_HEADER        *CurrentTable;
  UINTN                         AcpiTableKey;
  UINT8                         *CurrPtr;
  UINT8                         *EndPtr;
  UINT32                        *Signature;
  EFI_ACPI_DESCRIPTION_HEADER   *SaAcpiTable;
  EFI_ACPI_TABLE_PROTOCOL       *AcpiTable;

  FwVol               = NULL;
  SaAcpiTable = NULL;

  ///
  /// Locate ACPI Table protocol
  ///
  DEBUG ((EFI_D_INFO, "Init SA SSDT table\n"));
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **)&AcpiTable);
  if (Status != EFI_SUCCESS) {
    DEBUG ((EFI_D_ERROR, "Fail to locate EfiAcpiTableProtocol.\n"));
    return EFI_NOT_FOUND;
  }

  ///
  /// Locate protocol.
  /// There is little chance we can't find an FV protocol
  ///
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolumeProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  ASSERT_EFI_ERROR (Status);
  ///
  /// Looking for FV with ACPI storage file
  ///
  for (i = 0; i < NumberOfHandles; i++) {
    ///
    /// Get the protocol on this handle
    /// This should not fail because of LocateHandleBuffer
    ///
    Status = gBS->HandleProtocol (
                    HandleBuffer[i],
                    &gEfiFirmwareVolumeProtocolGuid,
                    (VOID **)&FwVol
                    );
    ASSERT_EFI_ERROR (Status);

    ///
    /// See if it has the ACPI storage file
    ///
    Size      = 0;
    FvStatus  = 0;
    Status = FwVol->ReadFile (
                      FwVol,
                      &gSaSsdtAcpiTableStorageGuid,
                      NULL,
                      &Size,
                      &FileType,
                      &Attributes,
                      &FvStatus
                      );

    ///
    /// If we found it, then we are done
    ///
    if (Status == EFI_SUCCESS) {
      break;
    }
  }
  ///
  /// Free any allocated buffers
  ///
  FreePool (HandleBuffer);

  ///
  /// Sanity check that we found our data file
  ///
  ASSERT (FwVol != NULL);
  if (FwVol == NULL) {
    DEBUG ((EFI_D_INFO, "SA Global NVS table not found\n"));
    return EFI_NOT_FOUND;
  }
  ///
  /// Our exit status is determined by the success of the previous operations
  /// If the protocol was found, Instance already points to it.
  /// Read tables from the storage file.
  ///
  Instance      = 0;
  CurrentTable  = NULL;
  while (Status == EFI_SUCCESS) {
    Status = FwVol->ReadSection (
                      FwVol,
                      &gSaSsdtAcpiTableStorageGuid,
                      EFI_SECTION_RAW,
                      Instance,
                      &CurrentTable,
                      &Size,
                      &FvStatus
                      );

    if (!EFI_ERROR (Status)) {
      ///
      /// Check the table ID to modify the table
      ///
      if (((EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable)->OemTableId == SIGNATURE_64 ('S', 'a', 'S', 's', 'd', 't', ' ', 0)) {
        SaAcpiTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
        ///
        /// Locate the SSDT package
        ///
        CurrPtr = (UINT8 *) SaAcpiTable;
        EndPtr  = CurrPtr + SaAcpiTable->Length;

        for (; CurrPtr <= EndPtr; CurrPtr++) {
          Signature = (UINT32 *) (CurrPtr + 3);
          if (*Signature == SIGNATURE_32 ('S', 'A', 'N', 'V')) {
            ASSERT_EFI_ERROR (*(UINT32 *) (CurrPtr + 3 + sizeof (*Signature) + 2) == 0xFFFF0000);
            ASSERT_EFI_ERROR (*(UINT16 *) (CurrPtr + 3 + sizeof (*Signature) + 2 + sizeof (UINT32) + 1) == 0xAA55);
            ///
            /// SA Global NVS Area address
            ///
            *(UINT32 *) (CurrPtr + 3 + sizeof (*Signature) + 2) = (UINT32) (UINTN) mSaGlobalNvsAreaProtocol.Area;
            ///
            /// SA Global NVS Area size
            ///
            *(UINT16 *) (CurrPtr + 3 + sizeof (*Signature) + 2 + sizeof (UINT32) + 1) =
            sizeof (SYSTEM_AGENT_GLOBAL_NVS_AREA);

            AcpiTableKey = 0;
            Status = AcpiTable->InstallAcpiTable (
                                  AcpiTable,
                                  SaAcpiTable,
                                  SaAcpiTable->Length,
                                  &AcpiTableKey
                                  );
            ASSERT_EFI_ERROR (Status);
            return EFI_SUCCESS;
          }
        }
      }
      ///
      /// Increment the instance
      ///
      Instance++;
      CurrentTable = NULL;
    }
  }

  return Status;

}

EFI_STATUS
EFIAPI
SaInitEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
/**
  SystemAgent DXE Initialization.

  @param[in] ImageHandle             Handle for the image of this driver
  @param[in] SystemTable             Pointer to the EFI System Table

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_OUT_OF_RESOURCES    No enough buffer to allocate
**/
{
  EFI_STATUS                Status;
  VOID                      *Registration;
  SA_INSTANCE_PRIVATE_DATA  *SaInstance;
  UINTN                     MCHBAR_BASE;
  SA_DATA_HOB               *SaDataHob;

  DEBUG ((EFI_D_INFO, "SaInitDxe Start\n"));

  SaInstance  = NULL;

  MCHBAR_BASE = McD0PciCfg64 (R_SA_MCHBAR) &~BIT0;

  ///
  /// Get the platform setup policy.
  ///
  Status = gBS->LocateProtocol (&gDxePlatformSaPolicyGuid, NULL, (VOID **) &mDxePlatformSaPolicy);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Dump SA Platform Policy
  ///
  SaDxePolicyDump (mDxePlatformSaPolicy);

  ///
  /// Get SaDataHob HOB and see if ASPM should be programmed before or after OpROM.
  ///
  mSaIotrapSmiAddress    = 0;
  mInitPcieAspmAfterOprom = 0;
  SaDataHob              = NULL;
  SaDataHob              = (SA_DATA_HOB *)GetFirstGuidHob (&gSaDataHobGuid);
  if (SaDataHob != NULL) {
    mSaIotrapSmiAddress    = SaDataHob->SaIotrapSmiAddress;
    mInitPcieAspmAfterOprom = SaDataHob->InitPcieAspmAfterOprom;
  }

  ///
  /// If there was no DXE ASPM code, always executes SMM code
  ///
#if SA_PCIE_ASPM_IN_DXE == 0
  mInitPcieAspmAfterOprom = 1;
#endif
  ///
  /// If there was no SMM mode supported, always enable DXE mode
  ///
#if SA_PCIE_ASPM_IN_SMM == 0
  mInitPcieAspmAfterOprom = 0;
#endif

  ///
  /// Install System Agent Global NVS protocol
  ///
  DEBUG ((EFI_D_INFO, "Install SA GNVS protocol\n"));
  Status = (gBS->AllocatePool) (EfiACPIMemoryNVS, sizeof (SYSTEM_AGENT_GLOBAL_NVS_AREA), &mSaGlobalNvsAreaProtocol.Area);
  ASSERT_EFI_ERROR (Status);
  ZeroMem ((VOID *) mSaGlobalNvsAreaProtocol.Area, sizeof (SYSTEM_AGENT_GLOBAL_NVS_AREA));
  mSaGlobalNvsAreaProtocol.Area->SaRcRevision = SA_RC_VERSION;
  mSaGlobalNvsAreaProtocol.Area->XPcieCfgBaseAddress  = (UINT32) (MmPciAddress (0, 0, 0, 0, 0x0));

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gSaGlobalNvsAreaProtocolGuid,
                  &mSaGlobalNvsAreaProtocol,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// PciExpress Dxe Initialization
  ///
  DEBUG ((EFI_D_INFO, "Initializing PciExpress (Dxe)\n"));
  PciExpressInit (mDxePlatformSaPolicy);

  ///
  /// Internal devices and Misc configurations.
  ///
  DEBUG ((EFI_D_INFO, "Internal Device and Misc Configurations\n"));
  DeviceConfigure (mDxePlatformSaPolicy);
  ProgramSvidSid (mDxePlatformSaPolicy);

  ///
  /// LegacyRegion Driver
  ///
  DEBUG ((EFI_D_INFO, "Initializing Legacy Region\n"));
  LegacyRegionInstall (ImageHandle);

  ///
  /// GtPostInit Initialization
  ///
  DEBUG ((EFI_D_INFO, "Initializing GT PowerManagement and other GT POST related\n"));
  GraphicsInit (ImageHandle, mDxePlatformSaPolicy);

  ///
  /// Audio (dHDA) Initialization
  ///
  DEBUG ((EFI_D_INFO, "Initializing CPU Audio (dHDA) device\n"));
  AudioInit (ImageHandle, mDxePlatformSaPolicy);

  //
  // @todo remove this VP SKIP and prepare for PO settings to skip this code.
  //
  ///
  /// Vtd Initialization
  ///
  DEBUG ((EFI_D_INFO, "Initializing VT-d\n"));
  VtdInit (mDxePlatformSaPolicy);

  ///
  /// IgdOpRegion Install Initialization
  ///
  DEBUG ((EFI_D_INFO, "Initializing IGD OpRegion\n"));
  IgdOpRegionInit ();


  ///
  /// Create PCI Enumeration Completed callback for SA
  ///
  EfiCreateProtocolNotifyEvent (
    &gEfiPciEnumerationCompleteProtocolGuid,
    TPL_CALLBACK,
    SaPciEnumCompleteCallback,
    NULL,
    &Registration
    );

  ///
  /// Create ExitPmAuth callback for SA
  ///
  EfiCreateProtocolNotifyEvent (
    &gExitPmAuthProtocolGuid,
    TPL_CALLBACK,
    SaExitPmAuthCallback,
    NULL,
    &Registration
    );

  ///
  /// Install SA_INFO_PROTOCOL
  ///
  SaInstance = AllocateZeroPool (sizeof (SA_INSTANCE_PRIVATE_DATA));
  if (SaInstance == NULL) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }

  SaInstance->SaInfo.Revision = SA_INFO_PROTOCOL_REVISION_1;
  ///
  /// RCVersion[31:0] is the release number.
  /// For example:
  /// SaFramework 0.6.0.01 should be 00 06 00 01 (0x00060001)
  /// SaFramework 0.6.2    should be 00 06 02 00 (0x00060200)
  ///
  SaInstance->SaInfo.RCVersion = SA_RC_VERSION;

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gEfiSaInfoProtocolGuid,
                  &(SaInstance->SaInfo),
                  NULL
                  );

  ///
  /// Install System Agent Global NVS ACPI table
  ///
  Status = InitializeSaSsdtAcpiTables ();

//[-start-140220-IB10920078-modify]//
//[-start-121023-IB03780467-remove]//
#ifdef SG_SUPPORT
  DEBUG ((EFI_D_INFO, "Initializing Switchable Graphics (Dxe)\n"));
  SwitchableGraphicsInit (ImageHandle, SystemTable, mDxePlatformSaPolicy);
#endif
//[-end-121023-IB03780467-remove]//
//[-end-140220-IB10920078-modify]//

  DEBUG ((EFI_D_INFO, "SaInitDxe End\n"));

  return EFI_SUCCESS;
}

VOID
DeviceConfigure (
  IN DXE_PLATFORM_SA_POLICY_PROTOCOL *DxePlatformSaPolicy
  )
/**
  Initialize GT Post Routines.

    @param[in] ImageHandle         -     Handle for the image of this driver
    @param[in] DxePlatformSaPolicy -     SA DxePlatformPolicy protocol
**/
{
  UINT64  MchBar;

  MchBar = McD0PciCfg64 (R_SA_MCHBAR) &~BIT0;

  ///
  /// Enable/Disable CHAP device (B0,D7,F0).
  ///
  if (DxePlatformSaPolicy->MiscConfig->ChapDeviceEnable) {
    McD0PciCfg32Or (R_SA_DEVEN, B_SA_DEVEN_D7EN_MASK);
  } else {
    McD0PciCfg32And (R_SA_DEVEN, ~(B_SA_DEVEN_D7EN_MASK));
  }
  ///
  /// Enable/Disable Thermal device (B0,D4,F0).
  ///
  if (DxePlatformSaPolicy->MiscConfig->Device4Enable) {
    McD0PciCfg32Or (R_SA_DEVEN, B_SA_DEVEN_D4EN_MASK);
  } else {
    McD0PciCfg32And (R_SA_DEVEN, ~(B_SA_DEVEN_D4EN_MASK));
  }
  ///
  /// Enable/Disable Audio device (B0,D3,F0).
  ///
  if ((DxePlatformSaPolicy->MiscConfig->AudioEnable) && (McD2PciCfg16 (R_SA_IGD_VID) != 0xFFFF)) {
    McD0PciCfg32Or (R_SA_DEVEN, B_SA_DEVEN_D3EN_MASK);
  } else {
    McD0PciCfg32And (R_SA_DEVEN, ~(B_SA_DEVEN_D3EN_MASK));
  }

  return ;
}

VOID
ProgramSvidSid (
  IN DXE_PLATFORM_SA_POLICY_PROTOCOL *DxePlatformSaPolicy
  )
/**
    Program SA devices Subsystem Vendor Identifier (SVID) and Subsystem Identifier (SID).

    @param[in] DxePlatformSaPolicy   The SAPlatform Policy protocol instance
**/
{
  UINT8                         Index;
  UINT8                         BusNumber;
  UINTN                         PciEAddressBase;
  UINT8                         DeviceNumber;
  UINT8                         FunctionNumber;
  UINT8                         SvidRegOffset;
  UINT16                        Data16;
  STATIC SA_SVID_SID_INIT_ENTRY SvidSidInitTable[] = {
    {
      0,
      0,
      PCI_SVID_OFFSET
    },
    {
      1,
      0,
      R_SA_PEG_SS_OFFSET
    },
    {
      1,
      1,
      R_SA_PEG_SS_OFFSET
    },
    {
      1,
      2,
      R_SA_PEG_SS_OFFSET
    },
    {
      2,
      0,
      PCI_SVID_OFFSET
    },
    {
      3,
      0,
      PCI_SVID_OFFSET
    },
    {
      4,
      0,
      PCI_SVID_OFFSET
    },
    {
      7,
      0,
      PCI_SVID_OFFSET
    }
  };

  if ((DxePlatformSaPolicy->MiscConfig->DefaultSvidSid->SubSystemVendorId != 0) ||
      (DxePlatformSaPolicy->MiscConfig->DefaultSvidSid->SubSystemId != 0)
      ) {
    for (Index = 0; Index < (sizeof (SvidSidInitTable) / sizeof (SA_SVID_SID_INIT_ENTRY)); Index++) {
      BusNumber       = 0;
      DeviceNumber    = SvidSidInitTable[Index].DeviceNumber;
      FunctionNumber  = SvidSidInitTable[Index].FunctionNumber;
      SvidRegOffset   = SvidSidInitTable[Index].SvidRegOffset;
      PciEAddressBase = (UINT32) MmPciAddress (0, BusNumber, DeviceNumber, FunctionNumber, 0);
      ///
      /// Skip if the device is disabled
      ///
      if (MmioRead16 (PciEAddressBase + PCI_VID) == 0xFFFF) {
        continue;
      }
      ///
      /// Program SA devices Subsystem Vendor Identifier (SVID)
      ///
      Data16 = DxePlatformSaPolicy->MiscConfig->DefaultSvidSid->SubSystemVendorId;
      MmioWrite16 (
        (UINTN) (PciEAddressBase + SvidRegOffset),
        Data16
        );
      SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint16,
        (UINTN) (PciEAddressBase + SvidRegOffset),
        1,
        (VOID *) (UINTN) (PciEAddressBase + SvidRegOffset)
        );

      ///
      /// Program SA devices Subsystem Identifier (SID)
      ///
      Data16 = DxePlatformSaPolicy->MiscConfig->DefaultSvidSid->SubSystemId;
      MmioWrite16 (
        (UINTN) (PciEAddressBase + SvidRegOffset + 2),
        Data16
        );
      SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint16,
        (UINTN) (PciEAddressBase + SvidRegOffset + 2),
        1,
        (VOID *) (UINTN) (PciEAddressBase + SvidRegOffset + 2)
        );
    }
  }

  return;
}

VOID
SaFviExitPmAuth(
  VOID
  )
/**
  This function populates the SA FVI version numbers
**/
{
  const UINT8               StrEnabled[sizeof (SA_CRID_ENABLED)]    = SA_CRID_ENABLED;
  const UINT8               StrDisabled[sizeof (SA_CRID_DISABLED)]  = SA_CRID_DISABLED;
  const CodeVersion         PcieRcVersion = {
#include "SaPcieVersion.h"
  };
  const CodeVersion         MemRcVersion = {
#include "MrcVersion.h"
  };
  EFI_STATUS                Status;
  EFI_IA32_REGISTER_SET     RegSet;
  UINT16                    VbiosBuildNum;
  EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios;

  if (mDxePlatformSaPolicy->MiscConfig->FviReport) {
    InitFviDataHubCbContext (
      mDxePlatformSaPolicy->MiscConfig->FviSmbiosType,
      (UINT8) mSaFviElements,
      &mSaFviVersionData
      );
    mSaFviElementsData[MEM_RC_VER].Element.Version.MajorVersion = (UINT8) MemRcVersion.Major;
    mSaFviElementsData[MEM_RC_VER].Element.Version.MinorVersion = (UINT8) MemRcVersion.Minor;
    mSaFviElementsData[MEM_RC_VER].Element.Version.Revision     = (UINT8) MemRcVersion.Rev;
    mSaFviElementsData[MEM_RC_VER].Element.Version.BuildNum     = (UINT16) MemRcVersion.Build;
    mSaFviElementsData[PCIE_VER].Element.Version.MajorVersion   = (UINT8) PcieRcVersion.Major;
    mSaFviElementsData[PCIE_VER].Element.Version.MinorVersion   = (UINT8) PcieRcVersion.Minor;
    mSaFviElementsData[PCIE_VER].Element.Version.Revision       = (UINT8) PcieRcVersion.Rev;
    mSaFviElementsData[PCIE_VER].Element.Version.BuildNum       = (UINT16) PcieRcVersion.Build;
    mSaFviElementsData[CRID_ORIGINAL].Element.Version.BuildNum  = (UINT16) McD0PciCfg8 (PCI_REVISION_ID_OFFSET);

    if (mDxePlatformSaPolicy->MiscConfig->CridEnable == TRUE) {
      CopyMem (mSaFviElementsData[CRID_STATUS].Element.VerString, StrEnabled, sizeof (StrEnabled));
    } else {
      CopyMem (mSaFviElementsData[CRID_STATUS].Element.VerString, StrDisabled, sizeof (StrDisabled));
    }

    mSaFviElementsData[CRID_NEW].Element.Version.BuildNum = (UINT16) McD0PciCfg8 (PCI_REVISION_ID_OFFSET);
    //
    // Check IGFX device
    //
    VbiosBuildNum = 0xFFFF;
    Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (void **) &LegacyBios);
    if (Status == EFI_SUCCESS) {
      RegSet.X.AX   = 0x5f01;
      Status        = LegacyBios->Int86 (LegacyBios, 0x15, &RegSet);
      VbiosBuildNum = (((RegSet.X.DX & 0x0F00) << 4) | ((RegSet.X.DX & 0x000F) << 8) | ((RegSet.X.BX & 0x0F00) >> 4) | (RegSet.X.BX & 0x000F));
    }
    mSaFviElementsData[VBIOS_VER].Element.Version.BuildNum = VbiosBuildNum;

    CreateRcFviDatahub (&mSaFviVersionData);
  }
}

VOID
EFIAPI
SaPciEnumCompleteCallback (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
/**
  This function gets registered as a callback to perform SA initialization before ExitPmAuth

  @param[in] Event     - A pointer to the Event that triggered the callback.
  @param[in] Context   - A pointer to private data registered with the callback function.
**/
{
  EFI_STATUS          Status;
  VOID                *ProtocolPointer;

  ///
  /// Check if this is first time called by EfiCreateProtocolNotifyEvent() or not,
  /// if it is, we will skip it until real event is triggered
  ///
  Status = gBS->LocateProtocol (&gEfiPciEnumerationCompleteProtocolGuid, NULL, (VOID **)&ProtocolPointer);
  if (EFI_SUCCESS != Status) {
     return;
  }

  gBS->CloseEvent (Event);

  Status = PegInitBeforeExitPmAuth ();
  if (EFI_SUCCESS != Status) {
    DEBUG ((EFI_D_ERROR, "[SA] Pcie initialization before ExitPmAuth Error, Status = %x \n", Status));
    ASSERT_EFI_ERROR (Status);
  }
}

VOID
EFIAPI
SaExitPmAuthCallback (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
/**
  This function gets registered as a callback to perform SA configuration security lock

  @param[in] Event     - A pointer to the Event that triggered the callback.
  @param[in] Context   - A pointer to private data registered with the callback function.
**/
{
  EFI_STATUS          Status;
  VOID                *ProtocolPointer;

  ///
  /// Check if this is first time called by EfiCreateProtocolNotifyEvent() or not,
  /// if it is, we will skip it until real event is triggered
  ///
  Status = gBS->LocateProtocol (&gExitPmAuthProtocolGuid, NULL, (VOID **)&ProtocolPointer);
  if (EFI_SUCCESS != Status) {
     return;
  }

  gBS->CloseEvent (Event);

  if (McD2PciCfg16 (R_SA_IGD_VID) != 0xFFFF) {
    Status = PostPmInitExitPmAuth();
    if (EFI_SUCCESS != Status) {
      DEBUG ((EFI_D_ERROR, "[SA] ExitPmAuth GraphicsInit Error, Status = %x \n", Status));
      ASSERT_EFI_ERROR (Status);
    }
  }

  UpdateDmarExitPmAuth();

  if (McD2PciCfg16 (R_SA_IGD_VID) != 0xFFFF) {
    Status = GetVBiosVbtExitPmAuth();
    if (EFI_SUCCESS != Status) {
      DEBUG ((EFI_D_ERROR, "[SA] ExitPmAuth Op Region Error, Status = %x \n", Status));
    }

    Status = UpdateIgdOpRegionExitPmAuth();
    if (EFI_SUCCESS != Status) {
      DEBUG ((EFI_D_ERROR, "[SA] ExitPmAuth Update Op Region Error, Status = %x \n", Status));
    }
  }

  SaFviExitPmAuth();

  Status = SaSecurityInit ();
  if (EFI_SUCCESS != Status) {
    DEBUG ((EFI_D_ERROR, "[SA] Security lock Error, Status = %x \n", Status));
    ASSERT_EFI_ERROR (Status);
  }
  return;
}

VOID
SaDxePolicyDump (
  IN DXE_PLATFORM_SA_POLICY_PROTOCOL *mDxePlatformSaPolicy
  )
/**
  This function prints the DXE phase platform policy.

  @param[in] mDxePlatformSaPolicy - SA DxePlatformPolicy protocol
**/
{
#ifdef EFI_DEBUG
  INTN  i;

  DEBUG ((EFI_D_INFO, "\n------------------------ SA Platform Policy (DXE) dump BEGIN -----------------\n"));
  DEBUG ((EFI_D_INFO, "Revision : %x\n", mDxePlatformSaPolicy->Revision));
  DEBUG ((EFI_D_INFO, "------------------------ SA_VTD_CONFIGURATION -----------------\n"));
  DEBUG ((EFI_D_INFO, " VtdEnable : %x\n", mDxePlatformSaPolicy->Vtd->VtdEnable));
  DEBUG ((EFI_D_INFO, " RmrrUsbBaseAddress : %x\n", mDxePlatformSaPolicy->Vtd->RmrrUsbBaseAddress));

  DEBUG ((EFI_D_INFO, " VtdBaseAddress[%d] :", SA_VTD_ENGINE_NUMBER));
  for (i = 0; i < SA_VTD_ENGINE_NUMBER; i++) {
    DEBUG ((EFI_D_INFO, " %x", mDxePlatformSaPolicy->Vtd->BaseAddress[i]));
  }
  DEBUG ((EFI_D_INFO, "\n"));

  DEBUG ((EFI_D_INFO, "------------------------ SA_MEMORY_CONFIGURATION -----------------\n"));
  DEBUG ((EFI_D_INFO, " SpdAddressTable[%d] :", SA_MC_MAX_SOCKETS));
  for (i = 0; i < SA_MC_MAX_SOCKETS; i++) {
    DEBUG ((EFI_D_INFO, " %x", mDxePlatformSaPolicy->MemoryConfig->SpdAddressTable[i]));
  }
  DEBUG ((EFI_D_INFO, "\n"));

  DEBUG ((EFI_D_INFO, " ChannelASlotMap : %x\n", mDxePlatformSaPolicy->MemoryConfig->ChannelASlotMap));
  DEBUG ((EFI_D_INFO, " ChannelBSlotMap : %x\n", mDxePlatformSaPolicy->MemoryConfig->ChannelBSlotMap));
  DEBUG ((EFI_D_INFO, " RmtBdatEnable : %x\n", mDxePlatformSaPolicy->MemoryConfig->RmtBdatEnable));
  DEBUG ((EFI_D_INFO, " MrcTimeMeasure : %x\n", mDxePlatformSaPolicy->MemoryConfig->MrcTimeMeasure));
  DEBUG ((EFI_D_INFO, " MrcFastBoot : %x\n", mDxePlatformSaPolicy->MemoryConfig->MrcFastBoot));

  DEBUG ((EFI_D_INFO, "------------------------ SA_PCIE_CONFIGURATION -----------------\n"));
  DEBUG ((EFI_D_INFO, " DmiAspm : %x\n", mDxePlatformSaPolicy->PcieConfig->DmiAspm));

  DEBUG ((EFI_D_INFO, " PegAspm[%d] :", SA_PEG_MAX_FUN));
  for (i = 0; i < SA_PEG_MAX_FUN; i++) {
    DEBUG ((EFI_D_INFO, " %x", mDxePlatformSaPolicy->PcieConfig->PegAspm[i]));
  }
  DEBUG ((EFI_D_INFO, "\n"));

  DEBUG ((EFI_D_INFO, " PegAspmL0s[%d] :", SA_PEG_MAX_FUN));
  for (i = 0; i < SA_PEG_MAX_FUN; i++) {
    DEBUG ((EFI_D_INFO, " %x", mDxePlatformSaPolicy->PcieConfig->PegAspmL0s[i]));
  }
  DEBUG ((EFI_D_INFO, "\n"));

  DEBUG ((EFI_D_INFO, " PegDeEmphasis[%d] :", SA_PEG_MAX_FUN));
  for (i = 0; i < SA_PEG_MAX_FUN; i++) {
    DEBUG ((EFI_D_INFO, " %x", mDxePlatformSaPolicy->PcieConfig->PegDeEmphasis[i]));
  }
  DEBUG ((EFI_D_INFO, "\n"));

  DEBUG ((EFI_D_INFO, " DmiExtSync : %x\n", mDxePlatformSaPolicy->PcieConfig->DmiExtSync));
  DEBUG ((EFI_D_INFO, " DmiDeEmphasis : %x\n", mDxePlatformSaPolicy->PcieConfig->DmiDeEmphasis));
  DEBUG ((EFI_D_INFO, " DmiIot : %x\n", mDxePlatformSaPolicy->PcieConfig->DmiIot));

  if (mDxePlatformSaPolicy->Revision >= DXE_SA_PLATFORM_POLICY_PROTOCOL_REVISION_2) {
    if (mDxePlatformSaPolicy->PcieConfig->PcieAspmDevsOverride != NULL) {
      DEBUG ((EFI_D_INFO, "------------------------ PCIE_ASPM_DEV_INFO -----------------\n"));
      DEBUG ((EFI_D_INFO, " VendorId DeviceId RevId RootApmcMask EndpointApmcMask\n"));
      i = 0;
      while ((mDxePlatformSaPolicy->PcieConfig->PcieAspmDevsOverride[i].VendorId != SA_PCIE_DEV_END_OF_TABLE) &&
             (i < MAX_PCIE_ASPM_OVERRIDE)) {
        DEBUG ((EFI_D_INFO, " %04x     %04x     %02x    %01x            %01x\n",
          mDxePlatformSaPolicy->PcieConfig->PcieAspmDevsOverride[i].VendorId,
          mDxePlatformSaPolicy->PcieConfig->PcieAspmDevsOverride[i].DeviceId,
          mDxePlatformSaPolicy->PcieConfig->PcieAspmDevsOverride[i].RevId,
          mDxePlatformSaPolicy->PcieConfig->PcieAspmDevsOverride[i].RootApmcMask,
          mDxePlatformSaPolicy->PcieConfig->PcieAspmDevsOverride[i].EndpointApmcMask));
        i++;
      }
      DEBUG ((EFI_D_INFO, "------------------------ END_OF_TABLE -----------------------\n"));
    }
    if (mDxePlatformSaPolicy->PcieConfig->PcieLtrDevsOverride != NULL) {
      DEBUG ((EFI_D_INFO, "------------------------ PCIE_LTR_DEV_INFO -----------------\n"));
      DEBUG ((EFI_D_INFO, " VendorId DeviceId RevId SnoopLatency NonSnoopLatency\n"));
      i = 0;
      while ((mDxePlatformSaPolicy->PcieConfig->PcieLtrDevsOverride[i].VendorId != SA_PCIE_DEV_END_OF_TABLE) &&
             (i < MAX_PCIE_LTR_OVERRIDE)) {
        DEBUG ((EFI_D_INFO, " %04x     %04x     %02x    %01x            %01x\n",
          mDxePlatformSaPolicy->PcieConfig->PcieLtrDevsOverride[i].VendorId,
          mDxePlatformSaPolicy->PcieConfig->PcieLtrDevsOverride[i].DeviceId,
          mDxePlatformSaPolicy->PcieConfig->PcieLtrDevsOverride[i].RevId,
          mDxePlatformSaPolicy->PcieConfig->PcieLtrDevsOverride[i].SnoopLatency,
          mDxePlatformSaPolicy->PcieConfig->PcieLtrDevsOverride[i].NonSnoopLatency));
        i++;
      }
      DEBUG ((EFI_D_INFO, "------------------------ END_OF_TABLE ----------------------\n"));
    }
  }

  for (i = 0; i < SA_PEG_MAX_FUN; i++) {
    DEBUG ((EFI_D_INFO, " PegPwrOpt[%d].LtrEnable            : %x\n", i, mDxePlatformSaPolicy->PcieConfig->PegPwrOpt[i].LtrEnable));
    DEBUG ((EFI_D_INFO, " PegPwrOpt[%d].LtrMaxSnoopLatency   : %x\n", i, mDxePlatformSaPolicy->PcieConfig->PegPwrOpt[i].LtrMaxSnoopLatency));
    DEBUG ((EFI_D_INFO, " PegPwrOpt[%d].ObffEnable           : %x\n", i, mDxePlatformSaPolicy->PcieConfig->PegPwrOpt[i].ObffEnable));
    DEBUG ((EFI_D_INFO, " PegPwrOpt[%d].LtrMaxNoSnoopLatency : %x\n", i, mDxePlatformSaPolicy->PcieConfig->PegPwrOpt[i].LtrMaxNoSnoopLatency));
  }

  if (mDxePlatformSaPolicy->Revision >= DXE_SA_PLATFORM_POLICY_PROTOCOL_REVISION_3) {
    DEBUG ((EFI_D_INFO, "------------------------ SA_SG_VBIOS_CONFIGURATION -----------------\n"));
    DEBUG ((EFI_D_INFO, " LoadVbios : %x\n", mDxePlatformSaPolicy->VbiosConfig->LoadVbios));
    DEBUG ((EFI_D_INFO, " ExecuteVbios : %x\n", mDxePlatformSaPolicy->VbiosConfig->ExecuteVbios));
    DEBUG ((EFI_D_INFO, " VbiosSource : %x\n", mDxePlatformSaPolicy->VbiosConfig->VbiosSource));
  }


  DEBUG ((EFI_D_INFO, "------------------------ SA_IGD_CONFIGURATION -----------------\n"));
  DEBUG ((EFI_D_INFO, " RenderStandby : %x\n", mDxePlatformSaPolicy->IgdConfig->RenderStandby));
  DEBUG ((EFI_D_INFO, "------------------------ SA_MISC_CONFIGURATION -----------------\n"));
  DEBUG ((EFI_D_INFO, " ChapDeviceEnable : %x\n", mDxePlatformSaPolicy->MiscConfig->ChapDeviceEnable));
  DEBUG ((EFI_D_INFO, " Device4Enable : %x\n", mDxePlatformSaPolicy->MiscConfig->Device4Enable));
  DEBUG ((EFI_D_INFO, " SubSystemVendorId : %x\n", mDxePlatformSaPolicy->MiscConfig->DefaultSvidSid->SubSystemVendorId));
  DEBUG ((EFI_D_INFO, " SubSystemId : %x\n", mDxePlatformSaPolicy->MiscConfig->DefaultSvidSid->SubSystemId));
  DEBUG ((EFI_D_INFO, " CridEnable : %x\n", mDxePlatformSaPolicy->MiscConfig->CridEnable));
  DEBUG ((EFI_D_INFO, " AudioEnable : %x\n", mDxePlatformSaPolicy->MiscConfig->AudioEnable));
  DEBUG ((EFI_D_INFO, " FviReport : %x\n", mDxePlatformSaPolicy->MiscConfig->FviReport));
  DEBUG ((EFI_D_INFO, " FviSmbiosType : %x\n", mDxePlatformSaPolicy->MiscConfig->FviSmbiosType));
  DEBUG ((EFI_D_INFO, "\n------------------------ SA Platform Policy (DXE) dump END -----------------\n"));
#endif
  return;
}
