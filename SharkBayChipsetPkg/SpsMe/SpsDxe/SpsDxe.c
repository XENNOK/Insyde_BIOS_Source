/** @file

  Server Platform Services Firmware ME Driver.

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

#include "SpsDxe.h"

#include <IndustryStandard/Acpi.h>
#include <Protocol/AcpiSupport.h>
#include <Library/AcpiPlatformLib.h>

EFI_HANDLE                             mSpsInfoHandle = NULL;
SPS_INFO_PROTOCOL                      mSpsInfoProtocol;

///
/// SPS Error handling.
/// please refer to Server Platform Services Firmware ME-BIOS Interface Rev 1.0.0 (#503664)
/// Ch.3 BIOS POST Requirements
///
BOOLEAN                                IsSpsErrorE2E3 = FALSE; /// SPS ME Disable or Timeout or ME-BIOS Incompatible
BOOLEAN                                IsSpsErrorE5   = FALSE; /// SPS ME in Recovery
BOOLEAN                                IsSpsNmSupport = FALSE; /// SPS NM not support

/**
  Check SPS Error status, and setting flags.

  @param None.

  @retval None.
**/
VOID
SpsCheckErrorFlag (
  VOID
  )
{

  if ((mSpsInfoProtocol.SpsMeDisabled) || (mSpsInfoProtocol.SpsMeTimeout) || (!mSpsInfoProtocol.SpsMeBiosCompat)) {
    IsSpsErrorE2E3 = TRUE;
  }
  if (mSpsInfoProtocol.SpsMeInRecovery) {
    IsSpsErrorE5 = TRUE;
  }

  if (mSpsInfoProtocol.SpsMeSku == SPS_SKU_NM) {
    IsSpsNmSupport = TRUE;
  }

}

/**
  Get Node Manager info hob.

  @param None.

  @retval EFI_SUCCESS            Get Node Manager info hob successfully.
  @retval EFI_NOT_FOUND          Node Manager info hob not found.
**/
EFI_STATUS
SpsGetSpsInfoHob (
  VOID
  )
{
  EFI_STATUS                 Status;
  EFI_HOB_GUID_TYPE          *GuidHob;
  SPS_INFO_HOB               *SpsInfoHob;

  Status = EFI_UNSUPPORTED;
  GuidHob = GetFirstGuidHob (&gSpsInfoHobGuid);
  if (GuidHob == NULL) {
    DEBUG ((DEBUG_INFO, "[DXE SPS] INFO: SPS INFO HOB Not found\n"));
    return EFI_NOT_FOUND;
  }

  SpsInfoHob = (SPS_INFO_HOB *) GET_GUID_HOB_DATA (GuidHob);

  DEBUG ((DEBUG_INFO, "[DXE SPS] INFO: SPS Info Hob\n"));
  DEBUG ((DEBUG_INFO, " |->SpsMeSku               : 0x%x\n", SpsInfoHob->SpsMeSku));
  DEBUG ((DEBUG_INFO, " |-SpsFwVersion\n"));
  DEBUG ((DEBUG_INFO, " | |->FwVerMajor           : 0x%x\n", SpsInfoHob->SpsFwVersion.FwVerMajor));
  DEBUG ((DEBUG_INFO, " | |->FwVerMinor           : 0x%x\n", SpsInfoHob->SpsFwVersion.FwVerMinor));
  DEBUG ((DEBUG_INFO, " | |->FwVerBuild           : 0x%x\n", SpsInfoHob->SpsFwVersion.FwVerBuild));
  DEBUG ((DEBUG_INFO, " | |->FwVerPatch           : 0x%x\n", SpsInfoHob->SpsFwVersion.FwVerPatch));
  DEBUG ((DEBUG_INFO, " |->SpsMeBiosCompat        : 0x%x\n", SpsInfoHob->SpsMeBiosCompat));
  DEBUG ((DEBUG_INFO, " |->SpsMeBiosIntfVer.Major : 0x%x\n", SpsInfoHob->SpsMeBiosIntfVer.Major));
  DEBUG ((DEBUG_INFO, " |->SpsMeBiosIntfVer.Minor : 0x%x\n", SpsInfoHob->SpsMeBiosIntfVer.Minor));
  DEBUG ((DEBUG_INFO, " |->SpsMeTimeout           : 0x%x\n", SpsInfoHob->SpsMeTimeout));
  DEBUG ((DEBUG_INFO, " |->SpsMeInRecovery        : 0x%x\n", SpsInfoHob->SpsMeInRecovery));
  DEBUG ((DEBUG_INFO, " |->BootingMode            : 0x%x\n", SpsInfoHob->BootingMode));
  DEBUG ((DEBUG_INFO, " |->CoreDisable            : 0x%x\n", SpsInfoHob->CoreDisable));

  CopyMem (&mSpsInfoProtocol, SpsInfoHob, sizeof (SPS_INFO_HOB));

  SpsCheckErrorFlag ();

  return EFI_SUCCESS;
}

/**
  Get Node Manager cores disable from firmware.

  @param None.

  @retval EFI_SUCCESS            Get Node Manager cores disable successfully.
  @retval EFI_NOT_FOUND          Get Node Manager cores disable not found.
**/
EFI_STATUS
SpsNmCoresDisable (
  VOID
  )
{
  EFI_STATUS            Status;
  UINT8                 NumberOfCoreDisable;

  ///
  /// Get Cores Disable from NMFS register in HECI-2.
  /// B0:D22:F1 40h [7:1] Number of Cores Disabled.
  ///
  Status = SpsNmGetCoresDisableNumber (&NumberOfCoreDisable);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ///
  /// Save the number of physical processor cores
  /// that should be disabled on each processor socket.
  ///
  mSpsInfoProtocol.CoreDisable = NumberOfCoreDisable;

  return EFI_SUCCESS;
}

/**
  Collect SPS information.
  SPS fw version etc...

  @param None.

  @retval EFI_SUCCESS            SPS info protocol install successfully.
**/
EFI_STATUS
SpsCollectSpsInfo (
  VOID
  )
{
  EFI_STATUS                 Status;
  SPS_GET_FW_VER_RSP_DATA    FwVersionData;

  ZeroMem (&FwVersionData, sizeof (SPS_GET_FW_VER_RSP_DATA));

  Status = SpsHeciGetFwVersion ((VOID *)&FwVersionData);
  if (!EFI_ERROR (Status)) {
    mSpsInfoProtocol.SpsFwVersion.FwVerMajor = FwVersionData.ActFw.MajorNumber;
    mSpsInfoProtocol.SpsFwVersion.FwVerMinor = FwVersionData.ActFw.MinorNumber;
    mSpsInfoProtocol.SpsFwVersion.FwVerBuild = FwVersionData.ActFw.BuildNumber;
    mSpsInfoProtocol.SpsFwVersion.FwVerPatch = FwVersionData.ActFw.PatchNumber;
    if (mSpsInfoProtocol.SpsMeInRecovery) {
      mSpsInfoProtocol.SpsFwVersion.FwVerMajor = FwVersionData.RcvFw.MajorNumber;
      mSpsInfoProtocol.SpsFwVersion.FwVerMinor = FwVersionData.RcvFw.MinorNumber;
      mSpsInfoProtocol.SpsFwVersion.FwVerBuild = FwVersionData.RcvFw.BuildNumber;
      mSpsInfoProtocol.SpsFwVersion.FwVerPatch = FwVersionData.RcvFw.PatchNumber;
    }
    if ((FwVersionData.BkpFw.MajorNumber == 0) && \
        (FwVersionData.BkpFw.MinorNumber == 0) && \
        (FwVersionData.BkpFw.BuildNumber == 0) && \
        (FwVersionData.BkpFw.PatchNumber == 0) && \
        (FwVersionData.BkpFw.ServerSegmentCode == 0)) {
      mSpsInfoProtocol.SpsMeDualImage = FALSE;
    } else {
      mSpsInfoProtocol.SpsMeDualImage = TRUE;
    }
  }

  DEBUG ((DEBUG_INFO, "[DXE SPS] INFO: SPS Info Protocol\n"));
  DEBUG ((DEBUG_INFO, " |->SpsMeSku               : 0x%x\n", mSpsInfoProtocol.SpsMeSku));
  DEBUG ((DEBUG_INFO, " |-SpsFwVersion\n"));
  DEBUG ((DEBUG_INFO, " | |->FwVerMajor           : 0x%x\n", mSpsInfoProtocol.SpsFwVersion.FwVerMajor));
  DEBUG ((DEBUG_INFO, " | |->FwVerMinor           : 0x%x\n", mSpsInfoProtocol.SpsFwVersion.FwVerMinor));
  DEBUG ((DEBUG_INFO, " | |->FwVerBuild           : 0x%x\n", mSpsInfoProtocol.SpsFwVersion.FwVerBuild));
  DEBUG ((DEBUG_INFO, " | |->FwVerPatch           : 0x%x\n", mSpsInfoProtocol.SpsFwVersion.FwVerPatch));
  DEBUG ((DEBUG_INFO, " |->SpsMeBiosCompat        : 0x%x\n", mSpsInfoProtocol.SpsMeBiosCompat));
  DEBUG ((DEBUG_INFO, " |->SpsMeBiosIntfVer.Major : 0x%x\n", mSpsInfoProtocol.SpsMeBiosIntfVer.Major));
  DEBUG ((DEBUG_INFO, " |->SpsMeBiosIntfVer.Minor : 0x%x\n", mSpsInfoProtocol.SpsMeBiosIntfVer.Minor));
  DEBUG ((DEBUG_INFO, " |->SpsMeTimeout           : 0x%x\n", mSpsInfoProtocol.SpsMeTimeout));
  DEBUG ((DEBUG_INFO, " |->SpsMeInRecovery        : 0x%x\n", mSpsInfoProtocol.SpsMeInRecovery));
  DEBUG ((DEBUG_INFO, " |->SpsMeDualImage         : 0x%x\n", mSpsInfoProtocol.SpsMeDualImage));
  DEBUG ((DEBUG_INFO, " |->BootingMode            : 0x%x\n", mSpsInfoProtocol.BootingMode));
  DEBUG ((DEBUG_INFO, " |->CoreDisable            : 0x%x\n", mSpsInfoProtocol.CoreDisable));

  Status = gBS->InstallProtocolInterface (
                  &mSpsInfoHandle,
                  &gSpsInfoProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mSpsInfoProtocol
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[DXE SPS] ERROR: Install mSpsInfoProtocol => (%r)\n", Status));
  }

  return Status;
}

/**
  SPS host configuration handler.

  @param None.

  @retval EFI_SUCCESS            Send host configuration to firmware successfully.
  @retval EFI_UNSUPPORTED        SPS firmware not supported.
**/
EFI_STATUS
SpsNmHostConfiguration (
  VOID
  )
{
  EFI_STATUS                           Status;
  SPS_NM_HOST_CONFIGURATION_DATA       HostConfigureData;

  if (SpsNmHostConfigEnabled ()) {
    Status = SpsNmPrepareHostConfigureData ((VOID *) &HostConfigureData);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = SpsHeciNmHostConfiguration ((VOID *) &HostConfigureData);
  } else {
    Status = EFI_UNSUPPORTED;
  }

  return Status;
}

/**
  Send END_OF_POST message to firmware.

  @param None.

  @retval EFI_SUCCESS            Send END_OF_POST to firmware successfully.
**/
EFI_STATUS
SpsEndOfPostEvent (
  VOID
  )
{
  EFI_STATUS                 Status;

  if (SpsEndOfPostEnabled ()) {
    Status = SpsHeciEndOfPost ();
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  Disable global reset.

  @param None.

  @retval EFI_SUCCESS            Allways return success.
**/
EFI_STATUS
SpsDisableGlobalReset (
  VOID
  )
{

  SpsMeCF9Configure (Disable);

  return EFI_SUCCESS;
}

/**
  Update SPS ACPI table and aml code.

  @param None.

  @retval EFI_SUCCESS             Update SPS ASL successfully.
**/
EFI_STATUS
SpsUpdateASL (
  VOID
  )
{
  UINT8                           AddressPrefix;
  UINT32                          *Address;
  UINT8                           *DsdtPointer;
  EFI_STATUS                      Status;
  EFI_ACPI_SUPPORT_PROTOCOL       *AcpiSupport;
  EFI_ACPI_TABLE_VERSION          Version;
  UINTN                           TableHandle = 0;
  INTN                            Index;
  EFI_ACPI_DESCRIPTION_HEADER     *Table;
  UINT32                          *Signature;
  UINT64                          HeciBar;

  ///
  /// Find the AcpiSupport protocol
  ///
  Status = gBS->LocateProtocol (&gEfiAcpiSupportProtocolGuid, NULL, (VOID **)&AcpiSupport);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[DXE SPS] ERROR: Locate ACPI Support Protocol (%r)\n", Status));
    return Status;
  }

  Index = 0;
  while (TRUE) {
    Status = AcpiSupport->GetAcpiTable (AcpiSupport, Index, &Table, &Version, &TableHandle);
    if (Table->Signature == EFI_ACPI_3_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE) {
      ///
      /// Search for DSDT Table.
      ///
      break;
    }
    if (Status == EFI_NOT_FOUND) {
      DEBUG ((DEBUG_ERROR, "[DXE SPS] ERROR: ACPI Talbe (%r)\n", Status));
      return Status;
    }
    Index++;
  }

  for (DsdtPointer = (UINT8 *)((UINTN)Table + sizeof (EFI_ACPI_DESCRIPTION_HEADER));
       DsdtPointer <= (UINT8 *)((UINTN)Table + (UINTN)(Table->Length));
       DsdtPointer++) {
    Signature = (UINT32*) DsdtPointer;
    ///
    /// Update HECI#1 BAR.
    ///
    if (*Signature == SIGNATURE_32 ('H', '1', 'C', 'S')) {
      ///
      /// Search for HECI-1 OperationRegion.
      ///
      if (IsAmlOpRegionObject (DsdtPointer)) {
        AddressPrefix = *(DsdtPointer + 5);
        Address = (UINT32*) (DsdtPointer + 6);
        ///
        /// Update OperationRegion Address.
        ///
        HeciBar = 0;
        SpsGetHeci1Bar (&HeciBar);
        DEBUG ((DEBUG_INFO, "[DXE SPS] INFO: Set HECI1 BAR (0x%lx) to (0x%lx)\n", *Address, HeciBar));
        if (AddressPrefix == AML_DWORD_PREFIX) {
          (*Address) = (UINT32)(HeciBar);
        } else if (AddressPrefix == AML_QWORD_PREFIX) {
          (*(UINT64*)Address) = (UINT64)(HeciBar);
        }
      }
    }
    ///
    /// Update HECI#2 BAR. ("H2CS")
    ///
    if (*Signature == SIGNATURE_32 ('H', '2', 'C', 'S')) {
      ///
      /// Search for HECI-2 OperationRegion.
      ///
      if (IsAmlOpRegionObject (DsdtPointer)) {
        AddressPrefix = *(DsdtPointer + 5);
        Address = (UINT32*) (DsdtPointer + 6);
        ///
        /// Update OperationRegion Address.
        ///
        HeciBar = 0;
        SpsGetHeci2Bar (&HeciBar);
        DEBUG ((DEBUG_INFO, "[DXE SPS] INFO: Set HECI2 BAR (0x%lx) to (0x%lx)\n", *Address, HeciBar));
        if (AddressPrefix == AML_DWORD_PREFIX) {
          (*Address) = (UINT32)(HeciBar);
        } else if (AddressPrefix == AML_QWORD_PREFIX) {
          (*(UINT64*)Address) = (UINT64)(HeciBar);
        }
      }
    }
    ///
    /// Update ("THNU") Number of processor threads in the system .
    ///
    if (*Signature == SIGNATURE_32 ('T', 'H', 'N', 'U')) {
      ///
      /// Search for ("THNU") Name Object.
      ///
      if ((*(DsdtPointer - 1) == AML_NAME_OP) && (*(DsdtPointer + 4) == AML_BYTE_PREFIX)) {
        SPS_NM_HOST_CONFIGURATION_DATA       HostConfigureData;
        Status = SpsNmPrepareHostConfigureData ((VOID *) &HostConfigureData);
        if (!EFI_ERROR (Status)) {
          *(UINT8 *)(DsdtPointer + 5) = (UINT8)HostConfigureData.ProcThreadsEnabled;
          DEBUG ((DEBUG_INFO, "[DXE SPS] INFO: Set (THNU)Number of processor threads in the system to (0x%02x)\n", *(UINT8 *)(DsdtPointer + 5)));
        }
      }
    }
    ///
    /// Update ("SMES") SPS ME Sku. [0] SiEn [1] Node Manger Enable.
    ///
    if (*Signature == SIGNATURE_32 ('S', 'M', 'E', 'S')) {
      ///
      /// Search for ("SMES") Name Object.
      ///
      if ((*(DsdtPointer - 1) == AML_NAME_OP) && (*(DsdtPointer + 4) == AML_BYTE_PREFIX)) {
        *(UINT8 *)(DsdtPointer + 5) = IsSpsNmSupport;
        DEBUG ((DEBUG_INFO, "[DXE SPS] INFO: Set (SMES)SPS ME Sku to (0x%02x)\n", IsSpsNmSupport));
      }
    }

//[-start-131115-IB10310039-add]//
    ///
    /// Update LoadTable's parameter.
    ///
    ///
    /// AML_EXT_LOAD_TABLE_OP - 5B 1F
    /// AML_STRING_PREFIX     - 0D
    /// SignatureString       - 50 52 41 44 00
    /// AML_STRING_PREFIX     - 0D
    /// OEMIDString           - 50 52 41 44 49 44 00
    /// AML_STRING_PREFIX     - 0D
    /// OEMTableIDString      - 50 52 41 44 54 42 49 44 00
    /// AML_STRING_PREFIX     - 0D
    /// RootPathString        - 00
    /// AML_STRING_PREFIX     - 0D
    /// ParameterPathString   - 00
    /// ParameterData         - 00
    ///
    if ((*DsdtPointer == AML_EXT_OP) && (*(DsdtPointer + 1) == AML_EXT_LOAD_TABLE_OP)) {
      ASSERT (*(DsdtPointer + 2) == AML_STRING_PREFIX);

      DEBUG ((DEBUG_ERROR, "[DXE SPS] INFO: Found LoadTable OpCode!\n"));
      if (*(UINT32*)(DsdtPointer + 3) == SIGNATURE_32 ('P', 'R', 'A', 'D')) {
        UINTN                           Length;
        UINT8                           *OemIdStart;
        UINT8                           *OemTableIdStart;

        DEBUG ((DEBUG_ERROR, "[DXE SPS] INFO: SignatureString  : %a\n", (DsdtPointer + 3)));
        DEBUG ((DEBUG_ERROR, "[DXE SPS] INFO: OEMIDString      : %a\n", (DsdtPointer + 9)));
        DEBUG ((DEBUG_ERROR, "[DXE SPS] INFO: OEMTableIDString : %a\n", (DsdtPointer + 17)));

        ASSERT (*(DsdtPointer + 8) == AML_STRING_PREFIX);
        ///
        /// Update OEMIDString
        ///   AML_STRING_PREFIX (0x0D)
        ///   OEMIDString       (Varies)
        ///   "\0"              (0x00)
        ///
        OemIdStart = DsdtPointer + 9;
        for (Index = 0; *(OemIdStart + Index) != AML_NOOP_OP; Index++) {
          *(OemIdStart + Index) = AML_NOOP_OP;
        }
        Length = AsciiStrLen ((CHAR8 *) PcdGetPtr (PcdOemId));
        CopyMem (OemIdStart, (CHAR8 *)PcdGetPtr (PcdOemId), Length);
        *(OemIdStart + Length)     = 0; // "\0"

        ///
        /// Update OEMTableIDString
        ///   AML_STRING_PREFIX (0x0D)
        ///   OEMTableIDString  (Varies)
        ///   "\0"              (0x00)
        ///
        *(OemIdStart + Length + 1) = AML_STRING_PREFIX; // AML_STRING_PREFIX
        OemTableIdStart = OemIdStart + Length + 2;
        Length = AsciiStrLen ((CHAR8 *) PcdGetPtr (PcdOemTableId));
        CopyMem (OemTableIdStart, (CHAR8 *)PcdGetPtr (PcdOemTableId), Length);
        *(OemTableIdStart + Length)     = 0; // "\0"

        ///
        /// Update RootPathString
        ///   AML_STRING_PREFIX (0x0D)
        ///   RootPathString    (Varies)
        ///   "\0"              (0x00)
        ///
        *(OemTableIdStart + Length + 1) = AML_STRING_PREFIX;
        *(OemTableIdStart + Length + 2) = 0;
        ///
        /// Update ParameterPathString
        ///   AML_STRING_PREFIX   (0x0D)
        ///   ParameterPathString (Varies)
        ///   "\0"                (0x00)
        ///
        *(OemTableIdStart + Length + 3) = AML_STRING_PREFIX;
        *(OemTableIdStart + Length + 4) = 0;
        ///
        /// Update ParameterData
        ///   (0x00)
        ///
        *(OemTableIdStart + Length + 5) = 0; // Zero

        DEBUG ((DEBUG_ERROR, "\n[DXE SPS] INFO: After modify\n"));
        DEBUG ((DEBUG_ERROR, "[DXE SPS] INFO: OEMIDString      : %a\n", OemIdStart));
        DEBUG ((DEBUG_ERROR, "[DXE SPS] INFO: OEMTableIDString : %a\n", OemTableIdStart));
      }
    }
//[-end-131115-IB10310039-add]//
  }

  Status = AcpiSupport->SetAcpiTable (AcpiSupport, (void *)Table, TRUE, Version, &TableHandle);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "[DXE SPS] ERROR: Set DSDT table fail!! Status = %r\n", Status));
  }

  return EFI_SUCCESS;
}

/**
  Signal a event for Me ready to boot.

  @param[in] Event                The event that triggered this notification function
  @param[in] Context              Pointer to the notification functions context

  @retval None.
**/
VOID
EFIAPI
SpsReadyToBootEvent (
  IN EFI_EVENT                         Event,
  IN VOID                              *Context
  )
{
  EFI_STATUS            Status;

  DEBUG ((DEBUG_INFO, "[DXE SPS] Ready To Boot Event...Start\n"));
  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  ///  for Denlow Revision 1.0.1(#503664) - 3  BIOS POST Requirements
  ///
  ///  /* BIOS POST Requirements */
  ///     Step (A12) : If NM s enabled in ME firmware, the BIOS sends the host 
  ///                  configuration information to ME. (See section 4.2.) This message should
  ///                  be sent after BIOS_RESET_CPL is being configured by BIOS POST. The
  ///                  information whether NM functionality is enabled can be found in Get
  ///                  ME-BIOS Interface Version response (Table 3-8).
  ///
  DEBUG ((DEBUG_INFO, "[DXE SPS] PROGRESS(A12) If NM enabled send Host Configuration information\n"));
  if ((!IsSpsErrorE2E3) && (!IsSpsErrorE5) && (IsSpsNmSupport)) {
    Status = SpsNmHostConfiguration ();
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "[DXE SPS] INFO: Send Host Configuration information (%r)\n", Status));
    }
  } else {
    DEBUG ((DEBUG_INFO, "[DXE SPS] INFO: SPS NM not support. skip send Host Configuration information\n"));
  }

  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  ///  for Denlow Revision 1.0.1(#503664) - 3  BIOS POST Requirements
  ///
  ///  /* BIOS POST Requirements */
  ///     Step (A14) : BIOS sends END_OF_POST message to ME to indicate that OS is to be loaded.
  ///
  DEBUG ((DEBUG_INFO, "[DXE SPS] PROGRESS(A14) BIOS send END_OF_POST message to ME\n"));
  if ((!IsSpsErrorE2E3)) {
    Status = SpsEndOfPostEvent ();
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "[DXE SPS] INFO: Send END_OF_POST message (%r)\n", Status));
    }
  } else {
    DEBUG ((DEBUG_INFO, "[DXE SPS] INFO: SPS ME Disable. skip sends END_OF_POST message\n"));
  }

  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  ///  for Denlow Revision 1.0.1(#503664) - 3  BIOS POST Requirements
  ///
  ///  /* BIOS POST Requirements */
  ///     Step (A15)  BIOS disables or hides ME devices.
  ///
  DEBUG ((DEBUG_INFO, "[DXE SPS] PROGRESS(A15) BIOS disables or hides ME devices as described\n"));
  Status = SpsMeDeviceConfigure (SpsDxeReadyToBoot);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "[DXE SPS] INFO: BIOS disables or hides ME devices. (%r)\n", Status));
  }

  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  ///  for Denlow Revision 1.0.1(#503664) - 3  BIOS POST Requirements
  ///
  ///  /* BIOS POST Requirements */
  ///     Step (A16)  BIOS disables Global Platform Reset capability in PCH to prevent OS from
  ///                 initiating Global Platform Reset (i.e. joint reset of ME and host).
  ///
  DEBUG ((DEBUG_INFO, "[DXE SPS] PROGRESS(A16) Disable Global Platform Reset\n"));
  Status = SpsDisableGlobalReset ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "[DXE SPS] INFO: BIOS disables Global Platform Reset capability. (%r)\n", Status));
  }

  Status = SpsUpdateASL ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "[DXE SPS] INFO: Update ASL => (%r)\n", Status));
  }

  gBS->CloseEvent (Event);

  DEBUG ((DEBUG_INFO, "[DXE SPS] Ready To Boot Event...End\n"));

  return;
}

/**
  Initialize SPS ME.

  @param[in] ImageHandle          The image handle of the DXE Driver, DXE Runtime Driver, DXE
                                  SMM Driver, or UEFI Driver.
  @param[in] SystemTable          A pointer to the EFI System Table.

  @retval EFI_SUCCESS             SPS driver initialization completed successfully.
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
SpsMeDxeEntry (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS            Status;
  EFI_EVENT             ReadyToBootEvent;

  mSpsInfoHandle = ImageHandle;

  Status = SpsGetSpsInfoHob ();
  if (EFI_ERROR (Status)) {
    DEBUG ( (DEBUG_ERROR, "[DXE SPS] ERROR: Get SPS INFO HOB (%r)\n", Status));
    DEBUG ( (DEBUG_ERROR, "[DXE SPS] ERROR: Disable SPS\n"));
    return Status;
  }

  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  ///  for Denlow Revision 1.0.1(#503664) - 3  BIOS POST Requirements
  ///
  ///  /* BIOS POST Requirements */
  ///     Step (A10) : If NM is enabled BIOS reads from the NMFS the number of processor
  ///                  cores that should be disabled in each processor package
  ///
  DEBUG ((DEBUG_INFO, "[DXE SPS] PROGRESS(A10) NM coers disable request.\n"));
  if ((!IsSpsErrorE2E3) && (!IsSpsErrorE5) && (IsSpsNmSupport)) {
    DEBUG ((DEBUG_INFO, "[DXE SPS] INFO: Proceeds to NM coers disable request.\n"));
    Status = SpsNmCoresDisable ();
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "[DXE SPS] ERROR: SPS Get NM Cores Disable (%r)\n", Status));
    }
  } else {
    DEBUG ((DEBUG_INFO, "[DXE SPS] INFO: SPS NM not support. skip Get Cores Disable number.\n"));
  }

  DEBUG ((DEBUG_INFO, "[DXE SPS] Proceeds to Collect Sps Infomation.\n"));
  Status = SpsCollectSpsInfo ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[DXE SPS] ERROR: SpsCollectSpsInfo => (%r)\n", Status));
  }

  ///
  /// Create a Ready to Boot event.
  ///
//[-start-131206-IB10310041-modify]//
  Status = EfiCreateEventReadyToBootEx (
             TPL_NOTIFY,
             SpsReadyToBootEvent,
             (VOID *) &ImageHandle,
             &ReadyToBootEvent
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[DXE SPS] ERROR: Create Ready to Boot Event => (%r)\n", Status));
  }
//[-end-131206-IB10310041-modify]//

  return EFI_SUCCESS;
}
