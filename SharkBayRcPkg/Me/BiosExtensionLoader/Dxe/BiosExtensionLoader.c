/** @file

  Loads the AMT Bios Extensions and calls the module prior to boot.
  Setup options control whether the user is allowed to change the provisioning of AMT
  or not for boot speed optimization.

  Configuration and invokation of the AMT Bios Extensions is done as per
  the AMT Bios Writers Guide.

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "BiosExtensionLoader.h"
#include <MeLib.h>

//[-start-120411-IB06460386-add]//
#include <Protocol/ConsoleControl.h>
//[-end-120411-IB06460386-add]//


extern DXE_AMT_POLICY_PROTOCOL  *mDxePlatformAmtPolicy;
BOOLEAN                         mFirstBoot;

ME_BIOS_EXTENSION_SETUP         mMeBiosExtensionSetup = {0};

UINT32                          mMebxSetupVariableAttributes;
UINTN                           mMebxSetupVariableDataSize;
AMT_READY_TO_BOOT_PROTOCOL      mAmtReadyToBoot = { AmtReadyToBoot };
UINT8                           mFwImageType;
UINT8                           mFwPlatformBrand;
BOOLEAN                         mFwMediaTableReqAvail = FALSE;
BOOLEAN                         mFwMediaTablePush     = FALSE;
BOOLEAN                         mMebxLaunched         = FALSE;
EFI_HANDLE                      mImageHandle;

/**
  Signal a event for Amt ready to boot.

  @param[in] None

  @retval EFI_SUCCESS             Mebx launched or no controller
**/
EFI_STATUS
EFIAPI
AmtReadyToBoot (
  VOID
  )
{
  EFI_STATUS        Status;
  EFI_HECI_PROTOCOL *Heci;
  UINT32            MeStatus;

  ///
  /// only launch MEBx once during POST
  ///
  if (mMebxLaunched) {
    return EFI_SUCCESS;
  }

  mMebxLaunched = TRUE;

  ///
  /// Launch MEBx, do not assert on error as this may be valid case
  ///
  Status = MebxNotifyEvent ();

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );

  if (!EFI_ERROR (Status)) {
    ///
    /// Check ME Status
    ///
    Status = Heci->GetMeStatus (&MeStatus);
    ASSERT_EFI_ERROR (Status);

    ///
    /// Send WDT message when ME is ready.  Do not care about if ME FW INIT is completed.
    ///
    if (ME_STATUS_ME_STATE_ONLY (MeStatus) == ME_READY) {
      if (AmtWatchDog ()) {
        ///
        /// Stop BIOS ASF Watch Dog before ready to boot
        ///
        AsfStopWatchDog ();
        ///
        /// Start OS ASF Watch Dog
        ///
        AsfStartWatchDog (ASF_START_OS_WDT);
      }
      ///
      /// End of watch dog setup option
      ///
    }
  }
  //
  // End of EFI_ERROR of locate HECI driver
  //
  return EFI_SUCCESS;
}

/**
  Create the FRU table to send to AMT FW

  @param[in] AssetTableData       Buffer of all Asset tables to send to FW
  @param[in] AmtData              Structure holds BIOS pointers for Asset tables

  @retval EFI_SUCCESS             FRU table calculated
**/
EFI_STATUS
CalulateFruTable (
  TABLE_PUSH_DATA  *AssetTableData,
  AMT_DATA         *AmtData
  )
{

  PCI_DEV_INFO    PciDevInfoPtr;
  HWAI_FRU_TABLE  *FruHeader;
  HWAI_PCI_FRU    *PciFru;
  UINT32          TableAddress;
  UINT16          i;

  i = 0;
  AssetTableData->Tables[HWAI_TABLE_TYPE_INDEX_FRU_DEVICE].Offset = 0;

  FruHeader = (HWAI_FRU_TABLE *) &AssetTableData->TableData[0];
  PciFru = (HWAI_PCI_FRU *) &FruHeader->Data[0];

  CopyMem (&PciDevInfoPtr, (VOID *) (UINTN) AmtData->PciDevAssertInfoPtr, sizeof (PCI_DEV_INFO));

  if (AmtData->SupportedTablesFlags & MEBX_STF_PCI_DEV_TABLE) {
    FruHeader->StructureCount = PciDevInfoPtr.PciDevCount;
  } else {
    FruHeader->StructureCount = 0;
  }

  TableAddress = (UINT32) AmtData->PciDevAssertInfoPtr + sizeof (PCI_DEV_INFO);

  if (FruHeader->StructureCount) {
    FruHeader->TableByteCount = FruHeader->StructureCount * sizeof (HWAI_PCI_FRU);
    for (i = 0; i < FruHeader->StructureCount; i++) {
      PciFru->SmbiosType    = 0;
      PciFru->Length        = sizeof (HWAI_PCI_FRU);
      PciFru->SmbiosHandle  = 0;
      PciFru->FruType       = HWAI_FRU_TYPE_PCI;
      CopyMem (&PciFru->FruData, (VOID *) (UINTN) TableAddress, sizeof (HWAI_PCI_DATA));
      ++PciFru;
      TableAddress += sizeof (HWAI_PCI_DATA);
    }
  } else {
    FruHeader->TableByteCount = 0;
  }

  AssetTableData->Tables[HWAI_TABLE_TYPE_INDEX_FRU_DEVICE].Length = FruHeader->TableByteCount +
  sizeof (FruHeader->StructureCount) + sizeof (FruHeader->TableByteCount);

  return EFI_SUCCESS;

}

/**
  Create the Media table to send to AMT FW

  @param[in] AssetTableData       Buffer of all Asset tables to send to FW
  @param[in] TableOffset          Offset into AssetTableData that Media table will be located
  @param[in] AmtDataPtr           Structure holds BIOS pointers for Asset tables

  @retval EFI_SUCCESS             Media table created
**/
EFI_STATUS
CalulateMediaTable (
  TABLE_PUSH_DATA  *AssetTableData,
  UINT16           TableOffset,
  AMT_DATA         *AmtDataPtr
  )
{

  HWAI_MEDIA_TABLE  *MediaHeaderPtr;
  HWAI_MEDIA_ENTRY  *MediaEntryPtr;
  MEDIA_DEV_INFO    MediaDevStruct;
  UINT32            TableAddress;
  UINT16            i;

  i = 0;

  AssetTableData->Tables[HWAI_TABLE_TYPE_INDEX_MEDIA_DEVICE].Offset = TableOffset;

  MediaHeaderPtr = (HWAI_MEDIA_TABLE *) &AssetTableData->TableData[TableOffset];
  MediaEntryPtr = (HWAI_MEDIA_ENTRY *) &MediaHeaderPtr->Data[0];

  CopyMem (&MediaDevStruct, (VOID *) (UINTN) AmtDataPtr->MediaDevAssetInfoPtr, sizeof (MEDIA_DEV_INFO));
  TableAddress = (UINT32) AmtDataPtr->MediaDevAssetInfoPtr + sizeof (MEDIA_DEV_INFO);

  if (!(AmtDataPtr->SupportedTablesFlags & MEBX_STF_MEDIA_DEV_TABLE)) {
    MediaHeaderPtr->TableByteCount = 0;
    AssetTableData->Tables[HWAI_TABLE_TYPE_INDEX_MEDIA_DEVICE].Length = 0;
    return EFI_SUCCESS;
  }

  MediaHeaderPtr->StructureCount = MediaDevStruct.MediaDevCount;
  MediaHeaderPtr->TableByteCount = (MediaHeaderPtr->StructureCount * sizeof (HWAI_MEDIA_ENTRY));
  for (i = 0; i < MediaHeaderPtr->StructureCount; i++) {
    MediaEntryPtr->Length       = sizeof (HWAI_MEDIA_ENTRY);
    MediaEntryPtr->SmbiosHandle = 0;
    MediaEntryPtr->SmbiosType   = 0;

    CopyMem (&MediaEntryPtr->MediaData, (VOID *) (UINTN) TableAddress, sizeof (HWAI_MEDIA_DATA));
    ++MediaEntryPtr;
    TableAddress += sizeof (HWAI_MEDIA_DATA);
  }

  AssetTableData->Tables[HWAI_TABLE_TYPE_INDEX_MEDIA_DEVICE].Length = MediaHeaderPtr->TableByteCount +
  sizeof (MediaHeaderPtr->StructureCount) + sizeof (MediaHeaderPtr->TableByteCount);

  return EFI_SUCCESS;

}

/**
  Create the SMBIOS table to send to AMT FW

  @param[in] AssetTableData       Buffer of all Asset tables to send to FW
  @param[in] TableOffset          Offset into AssetTableData that the SMBIOS table will be located
  @param[in] PtrSmbiosTable       Pointer to BIOS SMBIOS tables

  @retval EFI_ABORTED             PtrSmbiosTable data is invalid.
  @retval EFI_SUCCESS             Smbios table created.
**/
EFI_STATUS
CalulateSmbiosTable (
  TABLE_PUSH_DATA  *AssetTableData,
  UINT16           TableOffset,
  UINT32           PtrSmbiosTable
  )
{

  SMBIOS_ENTRY_POINT              SmbEntryStruct;
  SMBIOS_HEADER                   *SmbiosHeaderPtr;
  SMBIOS_TABLE_TYPE_ONE           *Type1Ptr;
  SMBIOS_TABLE_TYPE_THREE         *Type3Ptr;
  SMBIOS_TABLE_TYPE_FOUR          *Type4Ptr;
  SMBIOS_TABLE_TYPE_TWENTY_THREE  *Type23Ptr;
  UINT32                          TableAddress;
  UINT16                          OrignalTableOffset;
  UINT16                          StringCounter;
  UINT8                           DataByte;
  BOOLEAN                         EndOfTable;
  BOOLEAN                         EndOfEntry;
  BOOLEAN                         PreviousNull;
  BOOLEAN                         KeepEntry;

  OrignalTableOffset  = 0;
  StringCounter       = 1;
  DataByte            = 0;
  EndOfTable          = FALSE;
  EndOfEntry          = FALSE;
  PreviousNull        = FALSE;
  KeepEntry           = FALSE;

  OrignalTableOffset  = TableOffset;
  AssetTableData->Tables[HWAI_TABLE_TYPE_INDEX_SMBIOS].Offset = OrignalTableOffset;

  CopyMem (&SmbEntryStruct, (VOID *) (UINTN) PtrSmbiosTable, sizeof (SMBIOS_ENTRY_POINT));

  if (SmbEntryStruct.Signature != SMB_SIG) {
    DEBUG ((EFI_D_ERROR, "CalulateSmbiosTable Error:  SmbEntryStruct.Signature != SMB_SIG\n"));
    return EFI_ABORTED;
  }

  if (SmbEntryStruct.TableLength == 0) {
    DEBUG ((EFI_D_ERROR, "CalulateSmbiosTable Error:  TableSize == 0\n"));
    return EFI_ABORTED;
  }

  if (SmbEntryStruct.TableLength > (MAX_ASSET_TABLE_ALLOCATED_SIZE - TableOffset)) {
    DEBUG (
      (EFI_D_ERROR,
      "SMBIOS Tables Are Larger Than 0x%x\n",
      (MAX_ASSET_TABLE_ALLOCATED_SIZE - TableOffset))
      );
    return EFI_ABORTED;
  }

  TableAddress = SmbEntryStruct.TableAddress;

  while (!EndOfTable) {

    CopyMem (&AssetTableData->TableData[TableOffset], (VOID *) (UINTN) TableAddress, sizeof (SMBIOS_HEADER));

    SmbiosHeaderPtr = (SMBIOS_HEADER *) &AssetTableData->TableData[TableOffset];

    switch (SmbiosHeaderPtr->Type) {
    case 13:
    case 15:
    case 25:
    case 32:
      KeepEntry = FALSE;
      ///
      /// Not needed by AMT
      ///
      break;

    case 127:
      KeepEntry   = TRUE;
      EndOfTable  = TRUE;
      break;

    default:
      KeepEntry = TRUE;

    }

    if (KeepEntry) {
      TableOffset += sizeof (SMBIOS_HEADER);
      TableAddress += sizeof (SMBIOS_HEADER);

      CopyMem (
        &AssetTableData->TableData[TableOffset],
        (VOID *) (UINTN) TableAddress,
        (SmbiosHeaderPtr->Length - sizeof (SMBIOS_HEADER))
        );

      ///
      /// Make any need modifications to entrys with changing fields
      ///
      switch (SmbiosHeaderPtr->Type) {
      case 1:
        Type1Ptr          = (SMBIOS_TABLE_TYPE_ONE *) SmbiosHeaderPtr;
        Type1Ptr->WakeUp  = 0;
        break;

      case 3:
        Type3Ptr                    = (SMBIOS_TABLE_TYPE_THREE *) SmbiosHeaderPtr;
        Type3Ptr->BootState         = 0;
        Type3Ptr->PowerSupplyState  = 0;
        Type3Ptr->ThermalState      = 0;
        Type3Ptr->SecurityStatus    = 0;
        break;

      case 4:
        Type4Ptr                = (SMBIOS_TABLE_TYPE_FOUR *) SmbiosHeaderPtr;
        Type4Ptr->MaxSpeed      = Type4Ptr->MaxSpeed - (Type4Ptr->MaxSpeed % 100);
        Type4Ptr->CurrentSpeed  = Type4Ptr->CurrentSpeed - (Type4Ptr->CurrentSpeed % 100);
        break;

      case 23:
        Type23Ptr             = (SMBIOS_TABLE_TYPE_TWENTY_THREE *) SmbiosHeaderPtr;
        Type23Ptr->ResetCount = 0;
        break;

      default:
        break;
      }
      ///
      /// update both table pointer and scratch offset to be beyond this entry
      ///
      TableOffset += (SmbiosHeaderPtr->Length - sizeof (SMBIOS_HEADER));
      TableAddress += (SmbiosHeaderPtr->Length - sizeof (SMBIOS_HEADER));

    } else {
      ///
      /// skip this entry
      /// Move table address to beyond this entry, do not change scratch table offset
      ///
      TableAddress += SmbiosHeaderPtr->Length;
    }
    ///
    /// Copy any remaining unformatted data til end of type structure
    /// that is marked by double NULL bytes.
    ///
    EndOfEntry    = FALSE;
    PreviousNull  = FALSE;
    do {
      ///
      /// Read byte from bios data
      ///
      CopyMem (&DataByte, (VOID *) (UINTN) TableAddress++, 1);
      if (DataByte == 0x00) {
        if (SmbiosHeaderPtr->Type == 0) {
          StringCounter++;

        }

        if (PreviousNull) {
          ///
          /// this null marks end of entry
          ///
          EndOfEntry = TRUE;
        } else {
          ///
          /// flag we have seen first null
          ///
          PreviousNull = TRUE;
        }
      } else {
        ///
        /// clear null that terminated string
        ///
        PreviousNull = FALSE;

      }

      if (KeepEntry) {
        AssetTableData->TableData[TableOffset++] = DataByte;

      }

    } while (!EndOfEntry);

  }
  ///
  /// while !EndOfTable
  ///
  AssetTableData->Tables[HWAI_TABLE_TYPE_INDEX_SMBIOS].Length = TableOffset - OrignalTableOffset;

  return EFI_SUCCESS;

}

/**
  Create the ASF table to send to AMT FW

  @param[in] AssetTableData       Buffer of all Asset tables to send to FW
  @param[in] TableOffset          Offset into AssetTableData that the ASF table will be located
  @param[in] PtrAcpiRsdt          Pointer to BIOS ASF constructed tables

  @retval EFI_ABORTED             AssetTableData data is invalid.
  @retval EFI_SUCCESS             ASF table created.
**/
EFI_STATUS
CalulateAsfTable (
  TABLE_PUSH_DATA  *AssetTableData,
  UINT16           TableOffset,
  UINT32           PtrAcpiRsdt
  )
{

  ACPI_HEADER *AcpiHeaderPtr;
  UINT32      TableAddress;
  UINT32      ListAddress;
  UINT16      ArrayLength;

  TableAddress  = 0;

  AssetTableData->Tables[HWAI_TABLE_TYPE_INDEX_ASF].Offset = TableOffset;

  CopyMem ((UINT8 *) &AssetTableData->TableData[TableOffset], (VOID *) (UINTN) PtrAcpiRsdt, sizeof (ACPI_HEADER));

  AcpiHeaderPtr = (ACPI_HEADER *) &AssetTableData->TableData[TableOffset];

  if (AcpiHeaderPtr->Signature != RSDT_SIG) {
    DEBUG ((EFI_D_ERROR, "ASF ACPI Signature Does Not Match\n"));
    return EFI_ABORTED;
  }

  ArrayLength = (((UINT16) AcpiHeaderPtr->Length) - sizeof (ACPI_HEADER)) / sizeof (UINT32);
  ListAddress = PtrAcpiRsdt + sizeof (ACPI_HEADER);

  while (ArrayLength) {
    ///
    /// copy header at this new table address into scratch area
    ///
    CopyMem (&TableAddress, (VOID *) (UINTN) ListAddress, sizeof (UINT32));
    CopyMem (&AssetTableData->TableData[TableOffset], (VOID *) (UINTN) TableAddress, sizeof (ACPI_HEADER));

    if (AcpiHeaderPtr->Signature == ASF_SIG) {
      break;

    }

    ArrayLength--;
    ListAddress += sizeof (UINT32);
  }

  if (!ArrayLength) {
    ///
    /// We hit end of table
    ///
    DEBUG ((EFI_D_ERROR, "Did Not Find ASF Signature \n"));
    return EFI_ABORTED;
  }

  CopyMem (&AssetTableData->TableData[TableOffset], (VOID *) (UINTN) TableAddress, AcpiHeaderPtr->Length);

  AssetTableData->Tables[HWAI_TABLE_TYPE_INDEX_ASF].Length = (UINT16) AcpiHeaderPtr->Length;

  return EFI_SUCCESS;

}

/**
  Constructs each of the lower level asset tables

  @param[in] AssetTablesData      Buffer of all Asset tables to send to FW
  @param[in] TablesSize           Size of all tables combined
  @param[in] AmtData              Structure that holds all the BIOS constructed tables

  @retval EFI_SUCCESS             Tables crated.
  @retval EFI_ABORTED             AssetTableData data is invalid.
**/
EFI_STATUS
CalculateTableData (
  TABLE_PUSH_DATA*AssetTablesData,
  UINT16         *TablesSize,
  AMT_DATA       *AmtData
  )
{

  EFI_STATUS  Status;
  UINT16      TableOffset;

  Status      = EFI_SUCCESS;
  TableOffset = 0;

  Status      = CalulateFruTable (AssetTablesData, AmtData);
  if (EFI_ERROR (Status)) {
    return Status;

  }

  TableOffset = AssetTablesData->Tables[HWAI_TABLE_TYPE_INDEX_FRU_DEVICE].Length;
  Status      = CalulateMediaTable (AssetTablesData, TableOffset, AmtData);
  if (EFI_ERROR (Status)) {
    return Status;

  }

  TableOffset = TableOffset + AssetTablesData->Tables[HWAI_TABLE_TYPE_INDEX_MEDIA_DEVICE].Length;
  Status      = CalulateSmbiosTable (AssetTablesData, TableOffset, AmtData->PtrSmbiosTable);
  if (EFI_ERROR (Status)) {
    return Status;

  }

  TableOffset = TableOffset + AssetTablesData->Tables[HWAI_TABLE_TYPE_INDEX_SMBIOS].Length;
  Status      = CalulateAsfTable (AssetTablesData, TableOffset, AmtData->PtrAcpiRsdt);
  if (EFI_ERROR (Status)) {
    return Status;

  }

  TableOffset = TableOffset + AssetTablesData->Tables[HWAI_TABLE_TYPE_INDEX_ASF].Length;

  *TablesSize = TableOffset;

  return EFI_SUCCESS;

}

/**
  Constructs all asset tables and send them to FW

  @param[in] AmtData              Structure that holds all the BIOS constructed tables

  @retval EFI_ABORTED             Unable to allocate necessary AssetTableData data structure.
**/
EFI_STATUS
SendAssetTables2Firmware (
  AMT_DATA  *AmtData
  )
{

  EFI_HECI_PROTOCOL *Heci;
  TABLE_PUSH_DATA   *AssetTablesData;
  EFI_STATUS        Status;
  UINT16            TableOffset;
  UINT32            MeMode;
  UINT32            MeStatus;

  Status      = EFI_SUCCESS;
  TableOffset = 0;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  AssetTablesData = AllocateZeroPool (sizeof (TABLE_PUSH_DATA) + MAX_ASSET_TABLE_ALLOCATED_SIZE);

  if (AssetTablesData == NULL) {
    DEBUG ((EFI_D_ERROR, "SendAssetTables2Firmware Error: Could not allocate Memory\n"));
    return EFI_ABORTED;
  }

  Status = CalculateTableData (AssetTablesData, &TableOffset, AmtData);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "SendAssetTables2Firmware: Error calculating Asset tables - No Data Pushed\n"));
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "SendAssetTables2Firmware: Could not read FW Mode\n"));
    return Status;
  }

  Status = Heci->GetMeStatus (&MeStatus);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "SendAssetTables2Firmware: Could not read FW Status"));
    return Status;
  }

  if ((MeMode == ME_MODE_NORMAL) && (ME_STATUS_ME_STATE_ONLY (MeStatus) == ME_READY)) {
    Status = HeciAssetUpdateFwMsg (mImageHandle, AssetTablesData, TableOffset);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "SendAssetTables2Firmware Error: AssetUpdateFwMsg() returned Status %x\n", Status));
    }
  }

  FreePool (AssetTablesData);

  return Status;
}

/**
  This routine is run at boot time.
  1) Initialize AMT library.
  2) Check if MEBx is required to be launched by user.
  3) Build and send asset tables to ME FW.
  4) Check USB provision.
  5) Hide unused AMT devices in prior to boot.

  @param[in] None.

  @retval EFI_OUT_OF_RESOURCES    Unable to allocate necessary data structure.
  @retval EFI_NOT_FOUND           1.5M FW SKU detected - there is no MEBx on 1.5 FW SKU
**/
EFI_STATUS
MebxNotifyEvent (
  VOID
  )
{
  EFI_STATUS                                    Status;
  UINT8                                         *CoreImage;
  UINT8                                         *CallBackImage;
  MEBX_BPF                                      *MebxBiosParams;
  AMT_DATA                                      AmtData;
  EFI_ACPI_3_0_ROOT_SYSTEM_DESCRIPTION_POINTER  *Acpi3RsdPtr;
  EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_POINTER  *Acpi2RsdPtr;
  EFI_ACPI_1_0_ROOT_SYSTEM_DESCRIPTION_POINTER  *Acpi1RsdPtr;
  EFI_ACTIVE_MANAGEMENT_PROTOCOL                *Amt;
  VOID                                          *TempPointer;
  USB_KEY_PROVISIONING                          *UsbKeyProvisioningData;
  EFI_HANDLE                                    Handle;
  UINT8                                         InvokeMebx;
  UINT8                                         Data8;
  EFI_BOOT_MODE                                 BootMode;
  BOOLEAN                                       WarmReset;
  BOOLEAN                                       SendTables;
//[-start-120411-IB06460386-add]//
  EFI_CONSOLE_CONTROL_PROTOCOL                  *ConsoleControl;

  Status = gBS->LocateProtocol (&gEfiConsoleControlProtocolGuid, NULL, (VOID **)&ConsoleControl);
//[-end-120411-IB06460386-add]//

  DEBUG ((EFI_D_ERROR, "Entering BiosExtensionLoader Driver\n"));

  MebxBiosParams  = NULL;
  CoreImage       = NULL;
  CallBackImage   = NULL;
  Amt             = NULL;
  Handle          = NULL;
  InvokeMebx      = 0;
  Data8           = 0;

  ///
  /// Verify FW SKU - dispatch correct images 5MB FW SKU only
  ///
  if (mFwImageType == FW_IMAGE_TYPE_1_5MB) {
    ///
    /// 1.5M FW SKU detected - there is no MEBx on 1.5 FW SKU
    ///
    Status = EFI_NOT_FOUND;
    goto Done;
  }

  ///
  /// Initialize MEBX Setup Variable
  ///
  mMebxSetupVariableAttributes = EFI_VARIABLE_BOOTSERVICE_ACCESS |
    EFI_VARIABLE_RUNTIME_ACCESS |
    EFI_VARIABLE_NON_VOLATILE;
  mMebxSetupVariableDataSize = sizeof (ME_BIOS_EXTENSION_SETUP);

  Status = gST->RuntimeServices->GetVariable (
                                  gEfiMeBiosExtensionSetupName,
                                  &gEfiMeBiosExtensionSetupGuid,
                                  &mMebxSetupVariableAttributes,
                                  &mMebxSetupVariableDataSize,
                                  &mMeBiosExtensionSetup
                                  );

  if (EFI_ERROR (Status) || mMeBiosExtensionSetup.InterfaceVersion == 0) {
    DEBUG ((EFI_D_ERROR, "MeBiosExtensionSetup variable does not exist: create with default values\n"));
    ///
    /// create the variable when not exist
    ///
    mFirstBoot = TRUE;

    mMeBiosExtensionSetup.InterfaceVersion = MEBX_SETUP_VERSION;
    mMeBiosExtensionSetup.Flags = 0;
    mMeBiosExtensionSetup.PlatformMngSel = MEBX_SETUP_PLATFORM_MNT_DEFAULT;
    mMeBiosExtensionSetup.AmtSolIder = MEBX_SETUP_SOL_IDER_DEFAULT;
    mMeBiosExtensionSetup.RemoteAssistanceTriggerAvailablilty = 0;
    mMeBiosExtensionSetup.KvmEnable = 0;
    mMeBiosExtensionSetup.MebxDefaultSolIder = TRUE;

    Status = gST->RuntimeServices->SetVariable (
                                    gEfiMeBiosExtensionSetupName,
                                    &gEfiMeBiosExtensionSetupGuid,
                                    mMebxSetupVariableAttributes,
                                    mMebxSetupVariableDataSize,
                                    &mMeBiosExtensionSetup
                                    );
    ASSERT_EFI_ERROR (Status);
  } else {
    mFirstBoot = FALSE;
  }

#ifdef EFI_DEBUG
  DEBUG ((EFI_D_ERROR, "mMeBiosExtensionSetup before calling MEBx:\n"));
  DxeMebxSetupVariableDebugDump (&mMeBiosExtensionSetup);
#endif

#ifdef EFI_DEBUG
  ///
  /// Dump the AMT platform policy
  ///
  /// 
  /// The global mDxePlatformAmtPolicy is initialized by the driver
  /// entry point.
  ///
  DxeAmtPolicyDebugDump ();
#endif

  ///
  /// Allocate memory for BPF
  ///
  MebxBiosParams = AllocatePool (sizeof (MEBX_BPF));
  ASSERT (MebxBiosParams != NULL);
  if (MebxBiosParams == NULL) {
      return EFI_OUT_OF_RESOURCES;
  }

  ZeroMem (MebxBiosParams, sizeof (MEBX_BPF));

  ///
  /// Init AMT Library, still launch MEBx even if AMT Library Init fail
  ///
  Status = AmtLibInit ();

  ZeroMem ((VOID *) &AmtData, sizeof (AMT_DATA));

  ///
  /// Check if MEBx hotkey has been pressed in BIOS
  ///
  if (AmtHotkeyPressed ()) {
    MebxBiosParams->OemFlags |= MEBX_USER_ENTERED_RESPONSE;
  }
  ///
  /// Check if MEBx selection screen enabled
  ///
  if (AmtSelectionScreen ()) {
    MebxBiosParams->OemFlags |= MEBX_RA_SELECTION_MENU;
  }
  ///
  /// Check if UnConfigureMe request
  ///
  if (AmtUnConfigureMe ()) {
    MebxBiosParams->OemFlags |= MEBX_UNCONFIGURE;
  }
  ///
  /// Check if 'Hide UnConfigureMe Confirmation Prompt' request
  ///
  if (AmtHideUnConfigureMeConfPrompt ()) {
    MebxBiosParams->OemFlags |= MEBX_HIDE_UNCONFIGURE_CONF_PROMPT;
  }
  ///
  /// Check if want to show MEBx debug message
  ///
  if (AmtMebxDebugMsg ()) {
    MebxBiosParams->OemFlags |= MEBX_DEBUG_MSG;
  }

  MebxBiosParams->BpfVersion            = MEBX_VERSION;
  MebxBiosParams->CpuReplacementTimeout = mDxePlatformAmtPolicy->AmtConfig.CpuReplacementTimeout;
  MebxBiosParams->MeBiosSyncDataPtr     = (UINT32) (UINTN) &mMeBiosExtensionSetup;
  MebxBiosParams->MebxDebugFlags.Port80 = 1;

  ///
  /// pass OEM MEBx resolution settings through BPF
  ///
  MebxBiosParams->OemResolutionSettings.MebxGraphicsMode  = mDxePlatformAmtPolicy->AmtConfig.MebxGraphicsMode;
  MebxBiosParams->OemResolutionSettings.MebxNonUiTextMode = mDxePlatformAmtPolicy->AmtConfig.MebxNonUiTextMode;
  MebxBiosParams->OemResolutionSettings.MebxUiTextMode    = mDxePlatformAmtPolicy->AmtConfig.MebxUiTextMode;

  AmtData.PciDevAssertInfoPtr = (UINT32) (UINTN) &mAmtPciFru;
  AmtData.MediaDevAssetInfoPtr = (UINT32) (UINTN) &mAmtMediaFru;
  AmtData.VersionInfo = AMT_DATA_VERSION;

  ///
  /// Setup CIRA data
  ///
  if (AmtCiraRequestTrigger ()) {
//[-start-120411-IB06460386-add]//
    if (ConsoleControl != NULL) {
      ConsoleControl->SetMode (ConsoleControl, EfiConsoleControlScreenText);
    }
//[-end-120411-IB06460386-add]//
    MebxBiosParams->ActiveRemoteAssistanceProcess = 1;
    MebxBiosParams->CiraTimeout                   = AmtCiraRequestTimeout ();
  }
  ///
  /// Now set the PCI devices
  ///
  Status = BuildPciFru ();
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  if (mAmtPciFru.PciDevicesHeader.DevCount != 0) {
    AmtData.SupportedTablesFlags |= MEBX_STF_PCI_DEV_TABLE;
  }

  ///
  /// HW asset tables need to be sent when:
  /// - FW asked for it, or
  /// - FW is alive (brand!=0) and platform isn't booting from a warm reset unless it's first boot
  ///
  Data8 = PciRead8 (PCI_LIB_ADDRESS (ME_BUS, PCI_DEVICE_NUMBER_PCH_LPC, PCI_FUNCTION_NUMBER_PCH_LPC, R_PCH_LPC_GEN_PMCON_2));
  WarmReset = (BOOLEAN) !!(Data8 & B_PCH_LPC_GEN_PMCON_MEM_SR);

  SendTables = (mFwMediaTableReqAvail && mFwMediaTablePush) || (mFwPlatformBrand != NO_BRAND && (!WarmReset || mFirstBoot));

  ///
  /// ME BWG HWT_PushBIOSTables
  /// Built Media List for 8.1 firmware onwards only if Firmware request it or full BIOS boot path (Note 2, 4)
  /// Always built media list with older firmware (indicated by missing mFwMediaTableReqAvail)
  /// Above all, don't waste time building media list if tables are not going to be sent.
  ///
  BootMode = GetBootModeHob();

  if (SendTables &&
      ((BootMode != BOOT_WITH_MINIMAL_CONFIGURATION && BootMode != BOOT_ON_S4_RESUME) ||
       !mFwMediaTableReqAvail || (mFwMediaTableReqAvail && mFwMediaTablePush))) {
    //
    // Build Media List
    //
    BuildMediaList ();
    AmtData.SupportedTablesFlags |= MEBX_STF_MEDIA_DEV_TABLE;
  } else {
    DEBUG ((EFI_D_INFO, "No Media Asset Table is sent\n"));
  }

  mUsbProvsionDone = FALSE;
  ///
  /// If user selected for USB provisioning, then only use the provisioning file.
  ///
  if (USBProvisionSupport ()) {
    ///
    /// Check USB Key Provision
    ///
//[-start-120411-IB06460386-add]//
    if (ConsoleControl != NULL) {
      ConsoleControl->SetMode (ConsoleControl, EfiConsoleControlScreenText);
    }
//[-end-120411-IB06460386-add]//
    UsbKeyProvisioning ();
  }
  ///
  /// fill in the USB provisioning data...
  ///
  if (mUsbProvsionDone == TRUE) {
    UsbKeyProvisioningData = (USB_KEY_PROVISIONING *) (UINTN) AllocatePool (mUsbProvDataSize + sizeof (USB_KEY_PROVISIONING));
    ASSERT (UsbKeyProvisioningData != NULL);
    UsbKeyProvisioningData->USBKeyLocationInfo = (mUsbKeyPort << 16) |
      (mUsbKeyBus << 8) |
      (mUsbKeyDevice << 3) |
      mUsbKeyFunction;
    MebxBiosParams->UsbKeyDataStructurePtr = (UINT32) (UINTN) UsbKeyProvisioningData;
    CopyMem (
      (VOID *) ((UINTN) MebxBiosParams->UsbKeyDataStructurePtr + sizeof (USB_KEY_PROVISIONING)),
      mUsbProvData,
      mUsbProvDataSize
      );
    UsbConsumedDataRecordRemove ();
  }
  ///
  /// Get SMBIOS table pointer.
  ///
  EfiGetSystemConfigurationTable (&gEfiSmbiosTableGuid, (VOID **) &TempPointer);
  AmtData.PtrSmbiosTable = (UINT32) (UINTN) TempPointer;

  ///
  /// Find the AcpiSupport protocol returns RSDP (or RSD PTR) address.
  ///
  Status = EfiGetSystemConfigurationTable (&gEfiAcpi20TableGuid, (VOID *) &Acpi3RsdPtr);

  if (EFI_ERROR (Status) || (Acpi3RsdPtr == NULL)) {
    Status = EfiGetSystemConfigurationTable (&gEfiAcpi20TableGuid, (VOID *) &Acpi2RsdPtr);

    if (EFI_ERROR (Status) || (Acpi2RsdPtr == NULL)) {
      Status = EfiGetSystemConfigurationTable (&gEfiAcpiTableGuid, (VOID *) &Acpi1RsdPtr);
      if (EFI_ERROR (Status) || (Acpi1RsdPtr == NULL)) {
        DEBUG ((EFI_D_ERROR, "Error getting ACPI Table -> %r\n", Status));
        goto Done;
      } else {
        AmtData.PtrAcpiRsdt = Acpi1RsdPtr->RsdtAddress;
      }
    } else {
      AmtData.PtrAcpiRsdt = Acpi2RsdPtr->RsdtAddress;
    }
  } else {
    AmtData.PtrAcpiRsdt = Acpi3RsdPtr->RsdtAddress;
  }

  ///
  /// ME BWG HWT_PushBIOSTables
  /// Send Tables to Firmware as long as not on warm reset and AMT not permanently disabled (brand = 0)
  /// 8.1 firmware onwards, still send table to firmware as long as not on warm reset and AMT not permanently disable (note 2, 3)
  /// 8.1 firmware onwards, warm reset with MediaPush set will send asset table to firmware (Note 1)
  ///
  if (SendTables) {
    Status = SendAssetTables2Firmware (&AmtData);
    DEBUG ((EFI_D_INFO, "Send Asset Tables to AMT FW - Status = 0x%x %r\n", Status));
  }

  ///
  /// Check if firmware INVOKE_MEBX bit is set
  ///
  Data8       = 0;
  Data8       = PciRead8 (PCI_LIB_ADDRESS (ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER, R_ME_GS_SHDW));
  InvokeMebx  = (Data8 & INVOKE_MEBX_BIT) >> 3;
  DEBUG ((EFI_D_ERROR, "InvokeMebx = 0x%x %r\n", InvokeMebx, Status));

  ///
  /// Check for BIOS invoke reason
  ///
  if (!InvokeMebx) {
    CheckForBiosInvokeReason (&InvokeMebx, MebxBiosParams);

  }

  if (InvokeMebx) {
//[-start-120411-IB06460386-add]//
    if (ConsoleControl != NULL) {
      ConsoleControl->SetMode (ConsoleControl, EfiConsoleControlScreenText);        
    }
//[-end-120411-IB06460386-add]//
    PERF_START_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8000);
    Status = AdjustAndExecuteMebxImage (MebxBiosParams);
    PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8001);
  }

Done:
  ///
  /// Free up used RAM for MEBx as done now.
  ///
  if (MebxBiosParams != NULL) {
    FreePool (MebxBiosParams);
  }
  ///
  /// Hide the unused ME device. This will handle both cases MEBx running or not
  ///
  AmtDeviceConfigure (&mMeBiosExtensionSetup);

  return Status;
}

/**
  Check the status of Amt devices

  @param[in] MeBiosExtensionSetupData   MEBx setting data
**/
VOID
AmtDeviceConfigure (
  IN  ME_BIOS_EXTENSION_SETUP     *MeBiosExtensionSetupData
  )
{
  if (mFirstBoot || (MeBiosExtensionSetupData->AmtSolIder & SOL_ENABLE) == 0) {
    SolDisable ();
  }

  if (mFirstBoot || (MeBiosExtensionSetupData->AmtSolIder & IDER_ENABLE) == 0) {
    IderDisable ();
  }
}

/**
  Detect EFI MEBx support; Loading and execute.

  @param[in] MebxBiosParamsBuffer MebxBiosParams Flat pointer
**/
EFI_STATUS
AdjustAndExecuteMebxImage (
  IN  VOID                        *MebxBiosParamsBuffer
  )
{
  EFI_STATUS        Status;
  EFI_HECI_PROTOCOL *Heci;
  MEBX_BPF          *MebxBiosParams;
  EFI_MEBX_PROTOCOL *MebxProtocol;
  DATA32_UNION       MebxExitCode;

  MebxBiosParams        = (MEBX_BPF *) MebxBiosParamsBuffer;

  MebxExitCode.Data32  = 0;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  DEBUG ((EFI_D_ERROR, "Calling MEBx\n"));

  ///
  /// Stop ASF Watch Dog before entering MEBx Setup
  ///
  AsfStopWatchDog ();

  ///
  /// Locate MEBX protocol
  ///
  Status = gBS->LocateProtocol (&gEfiMebxProtocolGuid, NULL, (VOID **) &MebxProtocol);
  if (!EFI_ERROR (Status)) {
#ifdef EFI_DEBUG
    ///
    /// Dump MebxBiosParams before launching MEBx
    ///
    DxeMebxBiosParamsDebugDump (MebxBiosParams);
#endif
    MebxProtocol->CoreMebxEntry ((UINT32) (UINTN) MebxBiosParams, (UINT32 *) &MebxExitCode);
  }
  ///
  ///
  /// Re-Start ASF Watch Dog after exiting MEBx Setup
  ///
  AsfStartWatchDog (ASF_START_BIOS_WDT);

  DEBUG ((EFI_D_ERROR, "MEBx return BIOS_CMD_DATA:%x, BIOS_CMD:%x)\n", MebxExitCode.Data8[1], MebxExitCode.Data8[0]));
  DEBUG ((EFI_D_ERROR, "MEBx return MEBX_STATUS_CODE:%x)\n", MebxExitCode.Data16[1]));

  ///
  /// Restore data record when needed
  ///
  if (mUsbProvsionDone == TRUE) {
    CopyMem (
      mUsbProvDataBackup,
      (VOID *) ((UINTN) MebxBiosParams->UsbKeyDataStructurePtr + sizeof (USB_KEY_PROVISIONING)),
      mUsbProvDataSize
      );
    UsbConsumedDataRecordRestore ();
  }
  ///
  /// BIOS Sync-up DATA
  ///
  if (mMeBiosExtensionSetup.Flags == 1 || mMeBiosExtensionSetup.MebxDefaultSolIder == TRUE) {
    mMeBiosExtensionSetup.Flags = 0;
    mMeBiosExtensionSetup.MebxDefaultSolIder = FALSE;
    ///
    /// Update the variable to sync with MEBX Setup
    ///
    Status = gST->RuntimeServices->SetVariable (
                                    gEfiMeBiosExtensionSetupName,
                                    &gEfiMeBiosExtensionSetupGuid,
                                    mMebxSetupVariableAttributes,
                                    mMebxSetupVariableDataSize,
                                    &mMeBiosExtensionSetup
                                    );
    ASSERT_EFI_ERROR (Status);
  }

  switch (MebxExitCode.Data8[0] & MEBX_RET_CODE_MASK) {
  ///
  /// mask off reserved bits 3-7 from mebx exit status code
  ///
  case MEBX_RET_ACTION_CONTINUE_TO_BOOT:
    REPORT_STATUS_CODE (
      EFI_PROGRESS_CODE,
      EFI_SOFTWARE_UNSPECIFIED | EFI_SW_DXE_MEBX_OPROM_DONE
      );
    break;

  case MEBX_RET_ACTION_RESET:
    REPORT_STATUS_CODE (
      EFI_ERROR_CODE,
      EFI_SOFTWARE_UNSPECIFIED | EFI_SW_DXE_MEBX_RESET_ACTION
      );
    if (!(MebxExitCode.Data8[1] & MEBX_RET_ACTION_GLOBAL_RESET)) {
      DEBUG ((EFI_D_ERROR, "MEBx requires Global Reset.\n"));
      HeciSendCbmResetRequest (CBM_RR_REQ_ORIGIN_BIOS_POST, CBM_HRR_GLOBAL_RESET);
    } else {
      DEBUG ((EFI_D_ERROR, "MEBx requires Host Reset.\n"));
      gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
    }

    DEBUG ((EFI_D_ERROR, "No reset occurs after we get a reset cmd from MEBx.\n"));
    CpuDeadLoop ();
    break;

  default:
    REPORT_STATUS_CODE (
      EFI_ERROR_CODE,
      EFI_SOFTWARE_UNSPECIFIED | EFI_SW_DXE_MEBX_OTHER_UNSPECD
      );
    Status = EFI_NOT_STARTED;
    break;
  }

#ifdef EFI_DEBUG
  Status = gST->RuntimeServices->GetVariable (
                                  gEfiMeBiosExtensionSetupName,
                                  &gEfiMeBiosExtensionSetupGuid,
                                  &mMebxSetupVariableAttributes,
                                  &mMebxSetupVariableDataSize,
                                  &mMeBiosExtensionSetup
                                  );
  DEBUG ((EFI_D_ERROR, "mMeBiosExtensionSetup after calling MEBx:\n"));
  DxeMebxSetupVariableDebugDump (&mMeBiosExtensionSetup);
#endif
  ///
  /// Send PET Alert Message
  ///
  /// BIOS Specific Code
  ///
  /// Indicate OS BOOT handoff so that PET/ASF Push msg can be sent out to indicate
  /// all done now booting os.
  ///
  REPORT_STATUS_CODE (
    EFI_PROGRESS_CODE,
    EFI_SOFTWARE_UNSPECIFIED | EFI_SW_DXE_BS_PC_LEGACY_BOOT_EVENT
    );

  return Status;
}

/**
  The driver entry point - MEBx Driver main body.

  @param[in] ImageHandle          Handle for this drivers loaded image protocol.
  @param[in] SystemTable          EFI system table.

  @retval EFI_SUCCESS             The driver installed without error.
  @exception EFI_UNSUPPORTED      The chipset is unsupported by this driver.
**/
EFI_STATUS
EFIAPI
MebxDriverEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS            Status;
  EFI_HECI_PROTOCOL     *Heci;
  UINT32                MeMode;
  EFI_HANDLE            Handle;
  DXE_MBP_DATA_PROTOCOL *MbpData;

  mImageHandle  = ImageHandle;
  mFwImageType  = FW_IMAGE_TYPE_5MB;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Heci->GetMeMode (&MeMode);
  if (MeMode != ME_MODE_NORMAL) {
    return EFI_UNSUPPORTED;
  }

  ///
  /// Init AMT Policy Library
  ///
  Status = AmtPolicyLibInit ();
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  ///
  /// Install an Amt ready to boot protocol.
  ///
  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gAmtReadyToBootProtocolGuid,
                  &mAmtReadyToBoot,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);
  if (!EFI_ERROR (Status)) {
    ///
    /// Get the MBP Data.
    ///
    Status = gBS->LocateProtocol (&gMeBiosPayloadDataProtocolGuid, NULL, (VOID **) &MbpData);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "MEBx: No MBP Data Protocol available\n"));
      return Status;
    } else {
      ///
      /// Pass FW SKU Type
      ///
      mFwImageType      = (UINT8) MbpData->MeBiosPayload.FwPlatType.RuleData.Fields.IntelMeFwImageType;
      mFwPlatformBrand  = (UINT8) MbpData->MeBiosPayload.FwPlatType.RuleData.Fields.PlatformBrand;
      ///
      /// Save for Later use when MBPdata is deallocated
      ///
      mFwMediaTableReqAvail = MbpData->MeBiosPayload.HwaRequest.Available;
      mFwMediaTablePush = (BOOLEAN)MbpData->MeBiosPayload.HwaRequest.Data.Fields.MediaTablePush;
    }
  }

  return Status;
}

/**
  Detect BIOS invoke reasons.

  @param[in] InvokeMebx           Pointer to the Invoke MEBx flag
  @param[in] MebxBiosParamsBuffer MebxBiosParams Flat pointer
**/
VOID
CheckForBiosInvokeReason (
  IN  UINT8       *InvokeMebx,
  IN  MEBX_BPF    *MebxBiosParams
  )
{
  ///
  /// Check for BIOS invoke reason
  ///
  if (MebxBiosParams->OemFlags & MEBX_USER_ENTERED_RESPONSE) {
    DEBUG ((EFI_D_ERROR, "InvokeMebx Reason = MEBX_USER_ENTERED_RESPONSE %r\n"));
    *InvokeMebx = 1;
  }

  if (MebxBiosParams->OemFlags & MEBX_UNCONFIGURE) {
    DEBUG ((EFI_D_ERROR, "InvokeMebx Reason = MEBX_UNCONFIGURE %r\n"));
    *InvokeMebx = 1;
  }

  if (MebxBiosParams->UsbKeyDataStructurePtr) {
    DEBUG ((EFI_D_ERROR, "InvokeMebx Reason = UsbKeyDataStructurePtr %r\n"));
    *InvokeMebx = 1;
  }

  if (MebxBiosParams->ActiveRemoteAssistanceProcess) {
    DEBUG ((EFI_D_ERROR, "InvokeMebx Reason = ActiveRemoteAssistanceProcess %r\n"));
    *InvokeMebx = 1;
  }

  if (AmtForcMebxSyncUp ()) {
    DEBUG ((EFI_D_ERROR, "InvokeMebx Reason = forcing ME-BIOS sync-up %r\n"));
    MebxBiosParams->OemFlags  = 0;
    *InvokeMebx               = 1;
  }

  return ;
}
