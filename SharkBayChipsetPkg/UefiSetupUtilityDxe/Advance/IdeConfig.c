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

#include <SetupUtility.h>
//[-start-130716-IB07880077-add]//
#include <IndustryStandard/Pci22.h>
#include <Protocol/PciRootBridgeIo.h>
//[-end-130716-IB07880077-add]//

UINT16   gSaveItemMapping[0xf][2];
STATIC BOOLEAN  mIdeInit = FALSE;
//[-start-130626-IB10130054-add]//
BOOLEAN  mIsIDEModeFlag[8] = {FALSE};
//[-end-130626-IB10130054-add]//

EFI_STATUS
ForCombineMode (
  IN EFI_HII_HANDLE                        HiiHandle,
  IN CHIPSET_CONFIGURATION                  *SetupVariable
  );

EFI_STATUS
ForOtherMode (
  IN EFI_HII_HANDLE                        HiiHandle,
  IN CHIPSET_CONFIGURATION                  *SetupVariable
  );
VOID
IdeModelNameStrCpy (
  IN CHAR16   *Destination,
  IN CHAR16   *Source,
  IN UINTN    Length
  );

EFI_STATUS
SwapIdeConfig (
  IDE_CONFIG                              *IdeConfig1,
  IDE_CONFIG                              *IdeConfig2
  );

STATIC
EFI_STATUS
GetSecurityData (
//  IN  HDD_INFO       *HddInfoPtr,
  IDENTIFY_TAB       *IdentifyData,
//  IN  UINT8          Channel,
  IN  UINT16         HddPortNumber,
  IN  UINT8          Device,
  OUT BOOLEAN        *HaveSecurityData,
  OUT UINTN          *HarddiskInfoIndex
  );

EFI_STATUS
UpdateTransferMode (
  IN  EFI_HII_HANDLE                        HiiHandle,
  IN  EFI_ATAPI_IDENTIFY_DATA               *IdentifyInfo,
  IN  STRING_REF                            TokenToUpdate,
  IN  CHAR8                                 *LanguageString
  );

/**

 @param [in]   HiiHandle
 @param [in]   SetupVariable
 @param [in]   UpdateIde

 @retval NONE

**/
EFI_STATUS
CheckIde (
  IN EFI_HII_HANDLE                        HiiHandle,
  IN CHIPSET_CONFIGURATION                  *SetupVariable,
  IN BOOLEAN                               UpdateIde
)
{
  EFI_STATUS                               Status;
  UINT16                                   HddCount, BbsTotalCount;
  BBS_TABLE                                *LocalBbsTable;
  HDD_INFO                                 *LocalHddInfo;
  EFI_LEGACY_BIOS_PROTOCOL                 *LegacyBios;
  BOOLEAN                                  DeviceExist;
  CHAR16                                   *TokenString;
  STRING_REF                               ModelNameToken;
  STRING_REF                               TransferModeToken;
  STRING_REF                               SecurityModeToken;
  UINT8                                    Channel;
  UINT8                                    Device;
  UINT8                                    *IdeDevice;
  UINTN                                    Index;
  UINTN                                    Index1;
  UINTN                                    Index2;
  UINT16                                   PswToken1;
  UINT16                                   PswToken2;
  UINT16                                   Temp;
  UINTN                                    TotalLanguageCount;
  UINTN                                    LanguageCount;
  CHAR8                                    *LangString;
  CHAR16                                   *NotInstallString;
  CHAR8                                    *Lang;
  BOOLEAN                                  HaveSecurityData;
  UINTN                                    HarddiskInfoIndex;

  EFI_DISK_INFO_PROTOCOL                   *DiskInfo;
  IDENTIFY_TAB                             *IdentifyData;
  UINT32                                   Size;
  UINT8                                 SataCnfigure;
  VOID                                  *StartOpCodeHandle;
  EFI_IFR_GUID_LABEL                    *StartLabel;
  EFI_IFR_ACTION                        *ActionOpCode;

  SataCnfigure = gSUBrowser->SUCInfo->PrevSataCnfigure;
  DeviceExist       = FALSE;
  ModelNameToken    = 0;
  TransferModeToken = 0;
  SecurityModeToken = 0;
  Channel           = 0;
  Device            = 0;
  Index1        = 0;
  Index2        = 0;
  PswToken1 = 0;
  PswToken2 = 0;
  Temp      = 0;
  TokenString = NULL;
  HddCount      = 0;
  BbsTotalCount = 0;

  LocalBbsTable = NULL;
  LocalHddInfo  = NULL;
  LegacyBios    = NULL;

  IdentifyData  = NULL;

  if (SetupVariable->BootTypeReserved != EFI_BOOT_TYPE) {
    Status = gBS->LocateProtocol (
                    &gEfiLegacyBiosProtocolGuid,
                    NULL,
                    (VOID **)&LegacyBios
                    );
    if (EFI_ERROR (Status)) {
      return Status;
      }
    Status = LegacyBios->GetBbsInfo(
                           LegacyBios,
                           &HddCount,
                           &LocalHddInfo,
                           &BbsTotalCount,
                           &LocalBbsTable
                           );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  GetLangDatabase (&TotalLanguageCount, (UINT8 **)&LangString);
  for (LanguageCount = 0; LanguageCount < TotalLanguageCount; LanguageCount++) {
    IdeDevice = (UINT8 *) &(SetupVariable->IdeDevice0);
    Lang = &LangString[LanguageCount * RFC_3066_ENTRY_SIZE];
    for (Index=0; Index <= 7; Index++) {

      //
      // Get Device Token
      //
      switch (Index + 1) {

      case 1:
        TokenString = GetTokenStringByLanguage (
                        HiiHandle,
                        STRING_TOKEN (STR_SERIAL_ATA_PORT0_MODEL_NAME),
                        Lang
                        );
        ASSERT (TokenString);
        if (TokenString == NULL) {
          continue;
        }
        ModelNameToken    = STRING_TOKEN (STR_SERIAL_ATA_PORT0_STRING);
        TransferModeToken = STRING_TOKEN (STR_SERIAL_ATA_PORT0_TRANSFER_MODE);
        SecurityModeToken = STRING_TOKEN (STR_SERIAL_ATA_PORT0_SECURITY_MODE);
        PswToken1         = STRING_TOKEN (STR_HDD_PASSWORD_PROMPT_1);
        PswToken2         = STRING_TOKEN (STR_HDD_PASSWORD_CBM_PROMPT_1);
        break;

      case 2:
        TokenString = GetTokenStringByLanguage (
                        HiiHandle,
                        STRING_TOKEN (STR_SERIAL_ATA_PORT1_MODEL_NAME),
                        Lang
                        );
        ASSERT (TokenString);
        if (TokenString == NULL) {
          continue;
        }
        ModelNameToken    = STRING_TOKEN (STR_SERIAL_ATA_PORT1_STRING);
        TransferModeToken = STRING_TOKEN (STR_SERIAL_ATA_PORT1_TRANSFER_MODE);
        SecurityModeToken = STRING_TOKEN (STR_SERIAL_ATA_PORT1_SECURITY_MODE);
        PswToken1         = STRING_TOKEN (STR_HDD_PASSWORD_PROMPT_2);
        PswToken2         = STRING_TOKEN (STR_HDD_PASSWORD_CBM_PROMPT_2);
        break;

      case 3:
        TokenString = GetTokenStringByLanguage (
                        HiiHandle,
                        STRING_TOKEN (STR_SERIAL_ATA_PORT2_MODEL_NAME),
                        Lang
                        );
        ASSERT (TokenString);
        if (TokenString == NULL) {
          continue;
        }
        ModelNameToken    = STRING_TOKEN (STR_SERIAL_ATA_PORT2_STRING);
        TransferModeToken = STRING_TOKEN (STR_SERIAL_ATA_PORT2_TRANSFER_MODE);
        SecurityModeToken = STRING_TOKEN (STR_SERIAL_ATA_PORT2_SECURITY_MODE);
        PswToken1         = STRING_TOKEN (STR_HDD_PASSWORD_PROMPT_3);
        PswToken2         = STRING_TOKEN (STR_HDD_PASSWORD_CBM_PROMPT_3);
        break;

      case 4:
        TokenString = GetTokenStringByLanguage (
                        HiiHandle,
                        STRING_TOKEN (STR_SERIAL_ATA_PORT3_MODEL_NAME),
                        Lang
                        );
        ASSERT (TokenString);
        if (TokenString == NULL) {
          continue;
        }
        ModelNameToken    = STRING_TOKEN (STR_SERIAL_ATA_PORT3_STRING);
        TransferModeToken = STRING_TOKEN (STR_SERIAL_ATA_PORT3_TRANSFER_MODE);
        SecurityModeToken = STRING_TOKEN (STR_SERIAL_ATA_PORT3_SECURITY_MODE);
        PswToken1         = STRING_TOKEN (STR_HDD_PASSWORD_PROMPT_4);
        PswToken2         = STRING_TOKEN (STR_HDD_PASSWORD_CBM_PROMPT_4);
        break;

      case 5:
        TokenString = GetTokenStringByLanguage (
                        HiiHandle,
                        STRING_TOKEN (STR_SERIAL_ATA_PORT4_MODEL_NAME),
                        Lang
                        );
        ASSERT (TokenString);
        if (TokenString == NULL) {
          continue;
        }
        ModelNameToken    = STRING_TOKEN (STR_SERIAL_ATA_PORT4_STRING);
        TransferModeToken = STRING_TOKEN (STR_SERIAL_ATA_PORT4_TRANSFER_MODE);
        SecurityModeToken = STRING_TOKEN (STR_SERIAL_ATA_PORT4_SECURITY_MODE);
        PswToken1         = STRING_TOKEN (STR_HDD_PASSWORD_PROMPT_5);
        PswToken2         = STRING_TOKEN (STR_HDD_PASSWORD_CBM_PROMPT_5);
        break;

      case 6:
        TokenString = GetTokenStringByLanguage (
                        HiiHandle,
                        STRING_TOKEN (STR_SERIAL_ATA_PORT5_MODEL_NAME),
                        Lang
                        );
        ASSERT (TokenString);
        if (TokenString == NULL) {
          continue;
        }
        ModelNameToken    = STRING_TOKEN (STR_SERIAL_ATA_PORT5_STRING);
        TransferModeToken = STRING_TOKEN (STR_SERIAL_ATA_PORT5_TRANSFER_MODE);
        SecurityModeToken = STRING_TOKEN (STR_SERIAL_ATA_PORT5_SECURITY_MODE);
        PswToken1         = STRING_TOKEN (STR_HDD_PASSWORD_PROMPT_6);
        PswToken2         = STRING_TOKEN (STR_HDD_PASSWORD_CBM_PROMPT_6);
        break;

      case 7:
        TokenString = GetTokenStringByLanguage (
                        HiiHandle,
                        STRING_TOKEN (STR_SERIAL_ATA_PORT6_MODEL_NAME),
                        Lang
                        );
        ASSERT (TokenString);
        if (TokenString == NULL) {
          continue;
        }
        ModelNameToken    = STRING_TOKEN (STR_SERIAL_ATA_PORT6_STRING);
        TransferModeToken = STRING_TOKEN (STR_SERIAL_ATA_PORT6_TRANSFER_MODE);
        SecurityModeToken = STRING_TOKEN (STR_SERIAL_ATA_PORT6_SECURITY_MODE);
        PswToken1         = STRING_TOKEN (STR_HDD_PASSWORD_PROMPT_7);
        PswToken2         = STRING_TOKEN (STR_HDD_PASSWORD_CBM_PROMPT_7);
        break;

      case 8:
        TokenString = GetTokenStringByLanguage (
                        HiiHandle,
                        STRING_TOKEN (STR_SERIAL_ATA_PORT7_MODEL_NAME),
                        Lang
                        );
        ASSERT (TokenString);
        if (TokenString == NULL) {
          continue;
        }
        ModelNameToken    = STRING_TOKEN (STR_SERIAL_ATA_PORT7_STRING);
        TransferModeToken = STRING_TOKEN (STR_SERIAL_ATA_PORT7_TRANSFER_MODE);
        SecurityModeToken = STRING_TOKEN (STR_SERIAL_ATA_PORT7_SECURITY_MODE);
        PswToken1         = STRING_TOKEN (STR_HDD_PASSWORD_PROMPT_8);
        PswToken2         = STRING_TOKEN (STR_HDD_PASSWORD_CBM_PROMPT_8);
        break;
      }
        mIdeConfig[Index].SecurityModeToken = SecurityModeToken;

        if (mIdeConfig[Index].IdeDevice == 1) {
          DeviceExist = TRUE;
          Channel = mIdeConfig[Index].Channel;
          Device  = mIdeConfig[Index].Device;
          StrCat (
            TokenString,
            mIdeConfig[Index].DevNameString
            );
          if (mIdeConfig[Index].DiskInfoHandle == NULL) {
            IdentifyData = (IDENTIFY_TAB *) &LocalHddInfo[Channel].IdentifyDrive[Device];
          } else {
            Status = gBS->HandleProtocol (
                            mIdeConfig[Index].DiskInfoHandle,
                            &gEfiDiskInfoProtocolGuid,
                            (VOID **)&DiskInfo
                            );
            if (EFI_ERROR (Status)) {
              continue;
            }

            Size   = sizeof(IDENTIFY_TAB);
            Status = gBS->AllocatePool (
                            EfiBootServicesData,
                            Size,
                            (VOID **)&IdentifyData
                            );

            Status = DiskInfo->Identify (
                                 DiskInfo,
                                 IdentifyData,
                                 &Size
                                 );
          }

          UpdateTransferMode (
            HiiHandle,
            (EFI_ATAPI_IDENTIFY_DATA *) IdentifyData,
            TransferModeToken,
            Lang
            );

//[-start-130626-IB10130054-modify]//
          //if ((SataCnfigure == IDE_MODE) ||
          //    ((SataCnfigure == AHCI_MODE) && (SetupVariable->AhciOptionRomSupport == 0))) {
          if (mIsIDEModeFlag[Index]) {
//[-end-130626-IB10130054-modify]//
            Index1 = HDD_PASSWORD_CH_1_MASTER_CBM_LABEL + Index;
            Temp = PswToken2;
            PswToken2 = PswToken1;
            PswToken1 = Temp;

            Index2 = HDD_PASSWORD_CH_1_MASTER_LABEL + Index;

            UpdateSecurityStatus(
              HiiHandle,
              IdentifyData,
              (UINT16)Index,
              Device,
              SecurityModeToken,
              (UINT16) Index1,
              Lang
              );
            mIdeConfig[Index].Formlabel = (UINT16) Index1;
          } else {
            Index2          = HDD_PASSWORD_CH_1_MASTER_LABEL + Index;
            Index1 = HDD_PASSWORD_CH_1_MASTER_CBM_LABEL + Index;
            UpdateSecurityStatus(
              HiiHandle,
              IdentifyData,
              (UINT16)Index,
              Device,
              SecurityModeToken,
              (UINT16) Index2,
              Lang
              );
            mIdeConfig[Index].Formlabel = (UINT16) Index2;
          }
//[-start-130626-IB10130054-modify]//
          //if ((SataCnfigure == IDE_MODE) ||
          //    ((SataCnfigure == AHCI_MODE) && (SetupVariable->AhciOptionRomSupport == 0))) {
//[-start-131004-IB08520088-modify]//
          if (!(mIsIDEModeFlag[Index])) {
//[-end-131004-IB08520088-modify]//
//[-end-130626-IB10130054-modify]//
            GetSecurityData (
              IdentifyData,
              (UINT16)Index,
              Device,
              &HaveSecurityData,
              &HarddiskInfoIndex
              );
            UpdateHdPswLabel ((UINT16) Index1, HaveSecurityData, HarddiskInfoIndex);

            GetSecurityData (
              IdentifyData,
              (UINT16)Index,
              Device,
              &HaveSecurityData,
              &HarddiskInfoIndex
              );
            UpdateHdPswLabel ((UINT16) Index2, HaveSecurityData, HarddiskInfoIndex);
          } else {

            GetSecurityData (
              IdentifyData,
              (UINT16)Index,
              Device,
              &HaveSecurityData,
              &HarddiskInfoIndex
              );
            UpdateHdPswLabel ((UINT16) Index2, HaveSecurityData, HarddiskInfoIndex);

            GetSecurityData (
              IdentifyData,
              (UINT16)Index,
              Device,
              &HaveSecurityData,
              &HarddiskInfoIndex
              );
            UpdateHdPswLabel ((UINT16) Index1, HaveSecurityData, HarddiskInfoIndex);

          }

          *IdeDevice = 1;
          if (mIdeConfig[Index].DiskInfoHandle != NULL) {
            gBS->FreePool (IdentifyData);
          }
        } else {
          Status = CheckSataPort (Index);
          if (EFI_ERROR (Status)) {
            *IdeDevice = 2;
          } else {
            StrCat (TokenString, L"[");
            NotInstallString = GetTokenStringByLanguage (
                                 HiiHandle,
                                 STRING_TOKEN (STR_NOT_INSTALLED_TEXT),
                                 Lang
                                 );
            ASSERT (NotInstallString);
            if (NotInstallString != NULL) {
              StrCat (TokenString, NotInstallString);
              gBS->FreePool (NotInstallString);
              NotInstallString = NULL;
            }
            StrCat (TokenString, L"]");
            *IdeDevice = 0;
          }
        }

        IdeDevice++;
        //
        // Update String
        //
        Status = gSUBrowser->HiiString->SetString (
                                          gSUBrowser->HiiString,
                                          HiiHandle,
                                          ModelNameToken,
                                          Lang,
                                          TokenString,
                                          NULL
                                          );
        gBS->FreePool (TokenString);
    }
  }
  if ((SetupVariable->SetUserPass == TRUE) &&
       (SetupVariable->UserAccessLevel == USER_PASSWORD_VIEW_ONLY)) {
    if (DeviceExist == 0) {
      //
      // When no any booting device ,the system will hang.
      // We have create dummy data to updatefrom on last line.
      //
      StartOpCodeHandle = HiiAllocateOpCodeHandle ();
      ASSERT (StartOpCodeHandle != NULL);

      StartLabel               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
      StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
      StartLabel->Number       = IDE_UPDATE_LABEL;

      ActionOpCode = (EFI_IFR_ACTION *) HiiCreateActionOpCode (
                                          StartOpCodeHandle,
                                          0,
                                          STRING_TOKEN(STR_BLANK_STRING),
                                          0,
                                          EFI_IFR_FLAG_CALLBACK,
                                          0
                                          );

      Status = HiiUpdateForm (
                HiiHandle,
                NULL,
                0x23,
                StartOpCodeHandle,
                NULL
                );

      HiiFreeOpCodeHandle (StartOpCodeHandle);
    }
  }

  gBS->FreePool (LangString);
  return EFI_SUCCESS;
}

EFI_STATUS
UpdateTransferMode (
  IN  EFI_HII_HANDLE                        HiiHandle,
  IN  EFI_ATAPI_IDENTIFY_DATA               *IdentifyInfo,
  IN  STRING_REF                            TokenToUpdate,
  IN  CHAR8                                 *LanguageString
  )
{
  UINTN                                 Mode, Index;
  STRING_REF                            Token;
//[-start-131004-IB08520088-modify]//
//  CHAR16                                *StringPtr, *NewString;
  CHAR16                                *StringPtr;
//[-end-131004-IB08520088-modify]//

  Token = 0;
  Index = 0;
  if (IdentifyInfo->field_validity & 4) {
    Mode = (UINTN)(IdentifyInfo->ultra_dma_select & 0x3F);

    do {
      Index++;
      Mode = Mode >> 1;
    } while (Mode != 0);
  }

  switch (Index) {

  case 0:
    //
    // FPIO
    //
    Token = STRING_TOKEN (STR_IDE_TRANSMODE_FAST_PIO);
    break;

  case 1:
  case 2:
  case 3:
    //
    // Ultra DMA 33
    //
    Token = STRING_TOKEN (STR_IDE_ULTRA_DMA_ATA_33);
    break;

  case 4:
  case 5:
    //
    // Ultra DMA 66
    //
    Token = STRING_TOKEN (STR_IDE_ULTRA_DMA_ATA_66);
    break;

  case 6:
    //
    // Ultra DMA 100
    //
    Token = STRING_TOKEN (STR_IDE_ULTRA_DMA_ATA_100);
    break;
  }

  if (Token != 0) {
    StringPtr = GetTokenStringByLanguage (
                  HiiHandle,
                  Token,
                  LanguageString
                  );
    ASSERT (StringPtr);
    if (!StringPtr) {
      return EFI_NOT_FOUND;
    }
//[-start-131004-IB08520088-remove]//
//    NewString = EfiLibAllocateZeroPool (0x100);
////[-start-130207-IB10870073-add]//
//    ASSERT (NewString != NULL);
//    if (NewString == NULL) {
//      return EFI_OUT_OF_RESOURCES;
//    }
////[-end-130207-IB10870073-add]//
//    NewString[0] = '<';
//    for (Index = 0; StringPtr[Index] !=0x0000; Index++) {
//      NewString[Index + 1] = StringPtr[Index];
//    }
//    NewString[Index + 1] = '>';
//    NewString[Index + 2] = 0x0000;
//[-end-131004-IB08520088-remove]//
    gSUBrowser->HiiString->SetString (
                             gSUBrowser->HiiString,
                             HiiHandle,
                             TokenToUpdate,
                             LanguageString,
                             StringPtr,
                             NULL
                             );
    gBS->FreePool (StringPtr);
//[-start-131004-IB08520088-remove]//
//    gBS->FreePool (NewString);
//[-end-131004-IB08520088-remove]//
  }

  return EFI_SUCCESS;
}

//[-start-121026-IB08520064-modify]//
/**
 Update Ide-Harddisk security status information.

 @param [in]   HiiHandle
 @param        IdentifyData
 @param [in]   HddPortNumber
 @param [in]   Device           if '0' is master, '1' is slave
 @param [in]   TokenToUpdate    EFI-String token
 @param [in]   CallBackFormId   The Ide device form ID number
 @param [in]   LanguageString

 @retval EFI_UNSUPPORTED        Could not found Harddisk
 @retval EFI_SUCCESS            Update form success;

**/
EFI_STATUS
UpdateSecurityStatus (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IDENTIFY_TAB                          *IdentifyData,
  IN  UINT16                            HddPortNumber,
  IN  UINT8                             Device,
  IN  STRING_REF                        TokenToUpdate,
  IN  UINT16                            CallBackFormId,
  IN  CHAR8                             *LanguageString
  )
{
  UINTN                                 Index;
  STRING_REF                            Token;
  CHAR16                                *StringPtr;
//[-start-131004-IB08520088-remove]//
//  CHAR16                                *NewString;
//[-end-131004-IB08520088-remove]//
  CHIPSET_CONFIGURATION                  *SetupVariable;
//  SECURITY_INFO_DATA                    *HardDataInfo;
  HDD_PASSWORD_INFO_DATA                *HddPasswordDataInfo;
  HDD_SECURITY_INFO_DATA                *HarddiskInfoPtr;
  UINTN                                 NumOfHarddisk;
  SETUP_UTILITY_CONFIGURATION           *SCUInfo;
  UINT8                                 SataCnfigure;
  EFI_STATUS                            Status;

  //
  // Transfer previous mode to Combine mode
  //
  SataCnfigure = gSUBrowser->SUCInfo->PrevSataCnfigure;



  SetupVariable = (CHIPSET_CONFIGURATION *)gSUBrowser->SCBuffer;
  SCUInfo       = gSUBrowser->SUCInfo;

//  HardDataInfo  = SCUInfo->HarddiskPassword;
  HddPasswordDataInfo = SCUInfo->HarddiskPassword;
  NumOfHarddisk = SCUInfo->NumOfHarddiskPswd;


//  if ((NumOfHarddisk == 0) || (HardDataInfo == NULL)) {
  if ((NumOfHarddisk == 0) || (HddPasswordDataInfo == NULL)) {
   return EFI_UNSUPPORTED;
  }


  Token = STRING_TOKEN (STR_NOT_INSTALLED_TEXT);

  for (Index = 0; Index < NumOfHarddisk; Index++) {
     //
     // Search match Harddisk info
     //
    HarddiskInfoPtr = HddPasswordDataInfo[Index].HddDataInfo;
//[-start-130626-IB10130054-modify]//
      //if (!((SataCnfigure == IDE_MODE) ||
      //    ((SataCnfigure == AHCI_MODE) && (SetupVariable->AhciOptionRomSupport == 0)))) {
//[-start-131004-IB08520088-modify]//
      if (!(mIsIDEModeFlag[Index])) {
//[-end-131004-IB08520088-modify]//
//[-end-130626-IB10130054-modify]//
        if ((IdentifyData->config & bit(15)) != 0) {
         //
         // This is Atapi device
         //
          continue;
        }

          if (HarddiskInfoPtr->PortNumber != HddPortNumber) {
            continue;
          }
      } else {

        if (HarddiskInfoPtr->MappedPort != HddPortNumber) {
          continue;
        }
      }

     //
     // Get string token
     //

//     if (HardDataInfo[Index].Flag == SECURITY_LOCK) {
     if (HddPasswordDataInfo[Index].Flag == SECURITY_LOCK) {
       //
       // Harddisk Security Enable [Lock]
       //
       Token = STRING_TOKEN (STR_HDD_PSW_LOCK);
//     } else if (HardDataInfo[Index].Flag == CHANGE_PASSWORD) {
     } else if (HddPasswordDataInfo[Index].Flag == CHANGE_PASSWORD) {
       //
       // Harddisk Security Enable [Change]
       //
       Token = STRING_TOKEN (STR_HDD_PSW_CHANGE);
//     } else if (HardDataInfo[Index].Flag == DISABLE_PASSWORD) {
     } else if (HddPasswordDataInfo[Index].Flag == DISABLE_PASSWORD) {
       //
       // Harddisk Security Enable [Disable]
       //
       Token = STRING_TOKEN (STR_HDD_PSW_DISABLE);
//     } else if (HardDataInfo[Index].Flag == ENABLE_PASSWORD) {
     } else if (HddPasswordDataInfo[Index].Flag == ENABLE_PASSWORD) {
       //
       // Harddisk Security Unlock
       //
       Token = STRING_TOKEN (STR_HDD_PSW_UNLOCK);
     } else {
       //
       //Harddisk don't access password
       //
       Token = STRING_TOKEN (STR_HDD_PSW_NONACCESS);
     }
//     HardDataInfo[Index].LabelIndex = CallBackFormId;
     HddPasswordDataInfo[Index].LabelIndex = CallBackFormId;
     break;
  }

  if (Token != 0) {
    StringPtr = GetTokenStringByLanguage (
                  HiiHandle,
                  Token,
                  LanguageString
                  );
    ASSERT (StringPtr);
    if (!StringPtr) {
      return EFI_NOT_FOUND;
    }
//[-start-131004-IB08520088-modify]//
//    NewString = EfiLibAllocateZeroPool (0x100);
////[-start-130207-IB10870073-add]//
//    ASSERT (NewString != NULL);
//    if (NewString == NULL) {
//      return EFI_OUT_OF_RESOURCES;
//    }
////[-end-130207-IB10870073-add]//
//    for (Index = 0; StringPtr[Index] !=0x0000; Index++) {
//      NewString[Index] = StringPtr[Index];
//    }
//    NewString[Index + 1] = 0x0000;
    Status = gSUBrowser->HiiString->SetString (
                                      gSUBrowser->HiiString,
                                      HiiHandle,
                                      TokenToUpdate,
                                      LanguageString,
                                      StringPtr,
                                      NULL
                                      );
    gBS->FreePool (StringPtr);
//    gBS->FreePool (NewString);
//[-end-131004-IB08520088-modify]//
  }
  return EFI_SUCCESS;
}
//[-end-121026-IB08520064-modify]//

/**
 Copy the Unicode string Source to Destination.

 @param [in]   Destination      Location to copy string
 @param [in]   Source           String to copy
 @param [in]   Length

 @retval NONE

**/
VOID
IdeModelNameStrCpy (
  IN CHAR16   *Destination,
  IN CHAR16   *Source,
  IN UINTN    Length
  )
{
  *(Destination++) = L'[';
  while (Length) {
    *(Destination++) = *(Source++);
    Length--;
  }
  *(Destination++) = L']';
  *Destination = 0;
}

/**
 To update the list of ide device and status.

 @param [in]   HiiHandle        the handle of Advance menu.
 @param [in]   Buffer           the SYSTEM CONFIGURATION of SetupBrowser.

 @retval EFI_SUCCESS            it is success to update the status of Ide device.

**/
EFI_STATUS
UpdateHDCConfigure (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  CHIPSET_CONFIGURATION              *Buffer
  )
{
  UINT8                                   PrevSataCnfigure;

  //
  // Transfer previous mode to Combine mode
  //
  PrevSataCnfigure = gSUBrowser->SUCInfo->PrevSataCnfigure;


  InitIdeConfig (mIdeConfig);

  //
  // transfer modoe form Combine mode to selected mode
  //
  CheckIde (
    HiiHandle,
    Buffer,
    TRUE
    );
  gSUBrowser->SUCInfo->DoRefresh = TRUE;
  return EFI_SUCCESS;
}

EFI_STATUS
GetIdeDevNameString (
  IN OUT CHAR16 *                        *DevNameString
)
{
  EFI_STATUS                            Status;
  BBS_TABLE                             *LocalBbsTable;
  HDD_INFO                              *LocalHddInfo;
  EFI_LEGACY_BIOS_PROTOCOL              *LegacyBios;
  EFI_ATAPI_IDENTIFY_DATA               *IdentifyDriveInfo;
  UINT16                                HddCount, BbsTotalCount;
  UINTN                                 BbsTableIndex;

  UINT8                                 Channel;
  UINT8                                 Device;
  UINT8                                 Index;
  CHAR16                                *ModelNametring;

  ModelNametring = NULL;
  Status = gBS->LocateProtocol (
                  &gEfiLegacyBiosProtocolGuid,
                  NULL,
                  (VOID **)&LegacyBios
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = LegacyBios->GetBbsInfo(
                         LegacyBios,
                         &HddCount,
                         &LocalHddInfo,
                         &BbsTotalCount,
                         &LocalBbsTable
                         );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // IDE Configuration
  //
  Index = 0;
  BbsTableIndex = 1;
  for (Channel=0; Channel<4; Channel++) {
    for (Device=0; Device<2; Device++) {
    //
    // Initial String
    //
      DevNameString[Index] = AllocateZeroPool (0x100);
      if (LocalBbsTable[BbsTableIndex].BootPriority != BBS_IGNORE_ENTRY) {
        ModelNametring = AllocateZeroPool (0x100);
//[-start-130207-IB10870073-add]//
        ASSERT (ModelNametring != NULL);
        if (ModelNametring == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }
//[-end-130207-IB10870073-add]//
        //
        // Get Device Model name
        //
        IdentifyDriveInfo = (EFI_ATAPI_IDENTIFY_DATA *)&LocalHddInfo[Channel].IdentifyDrive[Device];
        UpdateAtaString(
          IdentifyDriveInfo,
          &ModelNametring
          );
        IdeModelNameStrCpy(
          DevNameString[Index],
          ModelNametring,
          20
          );
        gBS->FreePool (ModelNametring);
      }
      Index++;
      BbsTableIndex++;
    }
  }
  return EFI_SUCCESS;
}

//[-start-130711-IB07880077-add]//
/**
  Search device which it is match the device path.

  @param[in] DevicePath          The list of device path.
  @param[in] DevicePathHeader    Path header to be found.

  @ret EFI_DEVICE_PATH_PROTOCOL* Matched device path.

**/
EFI_DEVICE_PATH_PROTOCOL *
GetDevicePathTypeMatch (
  IN  EFI_DEVICE_PATH_PROTOCOL  *DevicePath,
  IN  EFI_DEVICE_PATH_PROTOCOL  *DevicePathHeader
  )
{
  while (!IsDevicePathEnd (DevicePath)) {
    if ((DevicePath->Type == DevicePathHeader->Type) && (DevicePath->SubType == DevicePathHeader->SubType)) {
      return DevicePath;
    } else {
      DevicePath = NextDevicePathNode (DevicePath);
    }
  }
  return NULL;
}

/**
  Get PCI device path from the list of devices.

  @param[in] DevicePath         The list of device path.

  @ret PCI_DEVICE_PATH*         Matched device path.

**/
PCI_DEVICE_PATH *
GetPciDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL  PciDevicePath = { HARDWARE_DEVICE_PATH, HW_PCI_DP };

  return ((PCI_DEVICE_PATH*) GetDevicePathTypeMatch (DevicePath, &PciDevicePath));
}

/**
  Get PCI location(Bus/Device/Function) from the device path

  @param[in] DevicePath         The list of device path.
  @param[out] Bus               Bus number to be derived.

  @ret PCI_DEVICE_PATH*         Matched device path.

**/
PCI_DEVICE_PATH *
GetPciLocation(
  IN  EFI_DEVICE_PATH_PROTOCOL  *DevicePath,
  OUT UINT8                     *Bus
  )
{
  EFI_STATUS                         Status;
  PCI_DEVICE_PATH                    *PciDevicePath;
  PCI_DEVICE_PATH                    *ParentPciDevicePath;
  PCI_DEVICE_PATH                    *NextPciDevicePath;
  EFI_DEVICE_PATH_PROTOCOL           *NextDevicePath;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL    *PciRootBridgeIo;
  UINT8                              SataBusNumber;
  UINT64                             PciAddress;

  //
  // Get bus number by analysis Device Path and PCI Register - PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET
  //
  PciDevicePath = GetPciDevicePath (DevicePath);
//[-start-140625-IB05080432-add]//
  if (PciDevicePath == NULL) {
    return NULL;
  }
//[-end-140625-IB05080432-add]//
  ParentPciDevicePath = NULL;
  SataBusNumber = 0;
  while (TRUE) {
    NextDevicePath    = (EFI_DEVICE_PATH_PROTOCOL*) PciDevicePath;
    NextDevicePath    = NextDevicePathNode (NextDevicePath);
    NextPciDevicePath = GetPciDevicePath (NextDevicePath);

    if ((!((DevicePathType (NextDevicePath) == MESSAGING_DEVICE_PATH) &&
           ((DevicePathSubType (NextDevicePath) == MSG_ATAPI_DP) ||
            (DevicePathSubType (NextDevicePath) == MSG_SATA_DP)))) &&
           (!(DevicePathType (NextDevicePath) == HARDWARE_DEVICE_PATH) &&
             (DevicePathSubType (NextDevicePath) == HW_PCI_DP))) {
      //
      // If device path type is not MESSAGING_DEVICE_PATH but is HARDWARE_DEVICE_PATH,
      // it may be external SATA device. If it is neither of them, return NULL.
      //
      return NULL;
    }
    if (NextPciDevicePath == NULL) {
      break;
    }

    //
    // If found next PCI Device Path, current Device Path is a P2P bridge
    //
    ParentPciDevicePath = PciDevicePath;
    PciDevicePath = NextPciDevicePath;
    PciAddress = EFI_PCI_ADDRESS (
                   SataBusNumber,
                   ParentPciDevicePath->Device,
                   ParentPciDevicePath->Function,
                   PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET
                   );

    //
    // Locate PCI IO protocol for PCI registers read
    //
    Status = gBS->LocateProtocol (
                    &gEfiPciRootBridgeIoProtocolGuid,
                    NULL,
                    (VOID **)&PciRootBridgeIo
                    );
    if (EFI_ERROR (Status)) {
      return NULL;
    }

    Status = PciRootBridgeIo->Pci.Read (
                                    PciRootBridgeIo,
                                    EfiPciWidthUint8,
                                    PciAddress,
                                    1,
                                    &SataBusNumber
                                    );
    if (EFI_ERROR (Status)) {
      return NULL;
    }
  }

  *Bus = SataBusNumber;
  return ((PCI_DEVICE_PATH*) PciDevicePath);
}

/**
  According the Bus, Device, Function to check this controller is in Port Number Map table or not.
  If yes, then this is a on board PCI device.

  @param[in] Bus                PCI bus number
  @param[in] Device             PCI device number
  @param[in] Function           PCI function number

  @retval TRUE                  It is onboard device.
  @retval FALSE                 Not onboard device.

**/
BOOLEAN
IsOnBoardPciDevice (
  IN UINT32                     Bus,
  IN UINT32                     Device,
  IN UINT32                     Function
  )
{
  UINTN                         Index;
  PORT_NUMBER_MAP               *PortMappingTable;
  PORT_NUMBER_MAP               EndEntry;
  UINTN                         NoPorts;

  PortMappingTable = NULL;
  ZeroMem (&EndEntry, sizeof (PORT_NUMBER_MAP));

  PortMappingTable = (PORT_NUMBER_MAP*) PcdGetPtr (PcdPortNumberMapTable);

  NoPorts = 0;
  while (CompareMem (&EndEntry, &PortMappingTable[NoPorts], sizeof (PORT_NUMBER_MAP)) != 0) {
    NoPorts++;
  }

  if (NoPorts == 0) {
    return FALSE;
  }
  for (Index = 0; Index < NoPorts; Index++) {
    if ((PortMappingTable[Index].Bus == Bus) &&
        (PortMappingTable[Index].Device == Device) &&
        (PortMappingTable[Index].Function == Function)) {
      return TRUE;
    }
  }
  return FALSE;
}
//[-end-130711-IB07880077-add]//

/**
 Check Ide device and collect the device information in EFI Boot mode.

 @param        IdeConfig        the array of IDE_CONFIG, that record label number, device name, etc....

 @retval EFI_SUCCESS            it is success to check and get device information.

**/
EFI_STATUS
InitIdeConfigInEfiBootType (
  IDE_CONFIG                             *IdeConfig
)
{
  EFI_STATUS                            Status;
  UINTN                                 HandleCount;
  EFI_HANDLE                            *HandleBuffer;
  EFI_DISK_INFO_PROTOCOL                *DiskInfo;
  EFI_DEVICE_PATH_PROTOCOL              *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL              *DevicePathNode;
  CHIPSET_CONFIGURATION                  *SetupVariable;
  UINTN                                 Index;
  PCI_DEVICE_PATH                       *PciDevicePath;
  ATAPI_DEVICE_PATH                     *AtapiDevicePath;
  SATA_DEVICE_PATH                      *SataDevicePath;
//[-start-130225-IB08520070-add]//
  SCSI_DEVICE_PATH                      *ScsiDevicePath;
//[-end-130225-IB08520070-add]//
  UINTN                                 PortNum;
  ATAPI_IDENTIFY                        IdentifyDrive;
  UINT32                                Size;
  CHAR16                                *ModelNametring;
  UINT32                                IdeChannel;
  UINT32                                IdeDevice;
//[-start-120712-IB08050177-add]//
  UINT16                                PortMap;
  UINT16                                Port;
//[-end-120712-IB08050177-add]//
  UINT8                                 SataCnfigure;
//[-start-130625-IB10130054-add]//
  UINTN                                 PortNumOffset;
  BOOLEAN                               IsIDEModeFlag;
//[-end-130625-IB10130054-add]//
//[-start-130716-IB07880077-add]//
  BOOLEAN                               OnBoardCheck;
  UINT8                                 Bus;
//[-end-130716-IB07880077-add]//
//[-start-130225-IB08520070-remove]//
////[-start-120926-IB08520055-add]//
//  VOID                                  *EfiCheckRaidProtocol;
//
//  EfiCheckRaidProtocol = NULL;
////[-end-120926-IB08520055-add]//
//[-start-130225-IB08520070-remove]//
  SataCnfigure = gSUBrowser->SUCInfo->PrevSataCnfigure;
//[-start-120712-IB08050177-add]//
  PortMap = 0;
  Port    = 0;
//[-end-120712-IB08050177-add]//

  if (IdeConfig == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  SetupVariable = (CHIPSET_CONFIGURATION *)gSUBrowser->SCBuffer;
  if (SetupVariable->BootTypeReserved != EFI_BOOT_TYPE) {
    return EFI_UNSUPPORTED;
  }
  HandleCount = 0;
  //
  // Collect all disk device information
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiDiskInfoProtocolGuid,
                   NULL,
                  &HandleCount,
                  &HandleBuffer
                  );

  for (Index = 0; Index < HandleCount; Index++) {
    gBS->HandleProtocol (
           HandleBuffer[Index],
           &gEfiDiskInfoProtocolGuid,
           (VOID **)&DiskInfo
           );
    ASSERT_EFI_ERROR (Status);
//[-start-120727-IB10370012-add]//
//[-start-120820-IB10820110-modify]//
      if (!(CompareGuid (&DiskInfo->Interface, &gEfiDiskInfoIdeInterfaceGuid) ||
            CompareGuid (&DiskInfo->Interface, &gEfiDiskInfoAhciInterfaceGuid))) {
        continue;
      }
//[-end-120820-IB10820110-modify]//
//[-end-120727-IB10370012-add]//

    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID **) &DevicePath
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    DevicePathNode = DevicePath;
    Status = EFI_NOT_FOUND;
    PortNum = 0;
//    switch (SetupVariable->SataCnfigure) {

//[-start-130624-IB10130054-add]//
    IsIDEModeFlag = FALSE;
    while (!IsDevicePathEnd (DevicePathNode)) {
      if ((DevicePathType (DevicePathNode) == MESSAGING_DEVICE_PATH) &&
          (DevicePathSubType (DevicePathNode) == MSG_ATAPI_DP)) {
        IsIDEModeFlag = TRUE;
        break;
      }

      if ((DevicePathType (DevicePathNode) == MESSAGING_DEVICE_PATH) &&
          (DevicePathSubType (DevicePathNode) == MSG_SATA_DP)) {
        IsIDEModeFlag = FALSE;
        break;
      }
      DevicePathNode = NextDevicePathNode (DevicePathNode);
    }
    DevicePathNode = DevicePath;
//[-end-130624-IB10130054-add]//
//[-start-130626-IB10130054-modify]//
//    switch (SataCnfigure) {

//    case IDE_MODE:
    if (IsIDEModeFlag) {
      //
      // IDE Mode
      //
      PciDevicePath = NULL;
      AtapiDevicePath = NULL;
      while (!IsDevicePathEnd (DevicePathNode)) {
        if ((DevicePathType (DevicePathNode) == HARDWARE_DEVICE_PATH) &&
            (DevicePathSubType (DevicePathNode) == HW_PCI_DP)) {
          PciDevicePath = (PCI_DEVICE_PATH *) DevicePathNode;
        }
        if ((DevicePathType (DevicePathNode) == MESSAGING_DEVICE_PATH) &&
            (DevicePathSubType (DevicePathNode) == MSG_ATAPI_DP)) {
          AtapiDevicePath = (ATAPI_DEVICE_PATH *) DevicePathNode;
          break;;
        }
        DevicePathNode = NextDevicePathNode (DevicePathNode);
      }
      if (PciDevicePath != NULL && AtapiDevicePath != NULL) {
        Status = SearchMatchedPortNum (
                   0,
                   PciDevicePath->Device,
                   PciDevicePath->Function,
                   AtapiDevicePath->PrimarySecondary,
                   AtapiDevicePath->SlaveMaster,
                   &PortNum
                   );
      }
      //break;
      } else {
    //case AHCI_MODE:
    //case RAID_MODE:
//[-start-130225-IB08520070-modify]//
      SataDevicePath = NULL;
      ScsiDevicePath = NULL;
      PciDevicePath = NULL;
      PortNumOffset = 0;
//[-start-130716-IB07880077-add]//
      OnBoardCheck = FALSE;
//[-end-130716-IB07880077-add]//
      while (!IsDevicePathEnd (DevicePathNode)) {
//[-start-130716-IB07880077-add]//
        if (!OnBoardCheck) {
          PciDevicePath = NULL;
          Bus = 0xFF;
          PciDevicePath = GetPciLocation (DevicePath, &Bus);
          if ((PciDevicePath != NULL) &&
              (Bus != 0xFF) &&
              (!IsOnBoardPciDevice (Bus, PciDevicePath->Device, PciDevicePath->Function))) {
            //
            // Not onboard device.
            //
            break;
          }
          OnBoardCheck = TRUE;
        }
//[-end-130716-IB07880077-add]//
        if ((DevicePathType (DevicePathNode) == HARDWARE_DEVICE_PATH) &&
            (DevicePathSubType (DevicePathNode) == HW_PCI_DP)) {
          PciDevicePath = (PCI_DEVICE_PATH *) DevicePathNode;
        }

        if ((DevicePathType (DevicePathNode) == MESSAGING_DEVICE_PATH) &&
            (DevicePathSubType (DevicePathNode) == MSG_SATA_DP)) {
          SataDevicePath = (SATA_DEVICE_PATH *) DevicePathNode;
          break;;
        }
        if ((DevicePathType (DevicePathNode) == MESSAGING_DEVICE_PATH) &&
            (DevicePathSubType (DevicePathNode) == MSG_SCSI_DP)) {
          ScsiDevicePath = (SCSI_DEVICE_PATH *) DevicePathNode;
          break;
        }
        DevicePathNode = NextDevicePathNode (DevicePathNode);
      }
      if (SataDevicePath != NULL) {
//[-start-120604-IB08050171-modify]//
        //if (SataCnfigure == AHCI_MODE) {
        //  PortNum = SataDevicePath->HBAPortNumber;
        //} else if (SataCnfigure == RAID_MODE) {
//[-start-120712-IB08050177-modify]//
//[-start-120926-IB08520055-modify]//
//          Status = gBS->LocateProtocol (
//                          &gEfiCheckRaidProtocolGuid,
//                          NULL,
//                          &EfiCheckRaidProtocol
//                          );
//          if (!EFI_ERROR (Status)) {
          if (SataDevicePath->PortMultiplierPortNumber & SATA_HBA_DIRECT_CONNECT_FLAG) {
            PortMap = SataDevicePath->HBAPortNumber;
            for (Port = 0; PortMap != 0; Port++, PortMap >>= 1);
            Port--;
            PortNum = Port;
          } else {
            PortNum = SataDevicePath->HBAPortNumber;
          }
//[-end-120926-IB08520055-modify]//
//[-end-120712-IB08050177-modify]//
        //}
//[-end-120604-IB08050171-modify]//
        Status = EFI_SUCCESS;
      }
      if (ScsiDevicePath != NULL) {
        PortNum = ScsiDevicePath->Pun;
        Status = EFI_SUCCESS;
      }
//[-start-130225-IB08520070-modify]//
//[-start-140625-IB05080432-modify]//
      if (PciDevicePath != NULL) {
        SearchMatchedPortNum (
          0,
          PciDevicePath->Device,
          PciDevicePath->Function,
          0,
          0,
          &PortNumOffset
          );
        PortNum += PortNumOffset;
      }
//[-end-140625-IB05080432-modify]//
      //break;

    //default:

    //ASSERT (FALSE);
      //break;
    }
//[-end-130626-IB10130054-modify]//

    if (!EFI_ERROR (Status)) {
      Status = DiskInfo->WhichIde (DiskInfo, &IdeChannel, &IdeDevice);
      if (!EFI_ERROR (Status)) {
        Size = sizeof (ATAPI_IDENTIFY);
        Status = DiskInfo->Identify (
                             DiskInfo,
                             &IdentifyDrive,
                             &Size
                             );
        if (!EFI_ERROR (Status)) {
          IdeConfig[PortNum].DevNameString     = AllocateZeroPool (0x100);
          ModelNametring = AllocateZeroPool (0x100);
//[-start-130207-IB10870073-add]//
          ASSERT (ModelNametring != NULL);
          if (ModelNametring == NULL) {
            return EFI_OUT_OF_RESOURCES;
          }
//[-end-130207-IB10870073-add]//
          UpdateAtaString (
            (EFI_ATAPI_IDENTIFY_DATA *) &IdentifyDrive,
            &ModelNametring
            );
          IdeModelNameStrCpy(
            IdeConfig[PortNum].DevNameString,
            ModelNametring,
            20
            );
          IdeConfig[PortNum].IdeDevice = 1;
          IdeConfig[PortNum].DiskInfoHandle = HandleBuffer[Index];
//[-start-130626-IB10130054-add]//
          mIsIDEModeFlag[PortNum] = IsIDEModeFlag;
//[-end-130626-IB10130054-add]//
          gBS->FreePool (ModelNametring);
        }
      }
    }
  }
  return EFI_SUCCESS;

}

/**
 to check Ide device and collect the device information.

 @param        IdeConfig        the array of IDE_CONFIG, that record label number, device name, etc....

 @retval EFI_SUCCESS            it is success to check and get device information.

**/
EFI_STATUS
InitIdeConfig (
  IDE_CONFIG                             *IdeConfig
)
{
  EFI_STATUS                            Status;
  BBS_TABLE                             *LocalBbsTable;
  HDD_INFO                              *LocalHddInfo;
  EFI_LEGACY_BIOS_PROTOCOL              *LegacyBios;
  UINT16                                HddCount, BbsTotalCount;
  UINTN                                 BbsTableIndex;

  UINT8                                 Channel;
  UINT8                                 Device;
  UINT8                                 Index;
  CHAR16                                *ModelNametring;
  CHIPSET_CONFIGURATION                  *SetupVariable;
  UINTN                                 PortNum;
  EFI_ATAPI_IDENTIFY_DATA               *IdentifyTable;
  UINT8                                 SataCnfigure;

  SataCnfigure = gSUBrowser->SUCInfo->PrevSataCnfigure;
  if (mIdeInit && gSUBrowser->IdeConfig != NULL) {
    return EFI_SUCCESS;
  }
  SetupVariable = (CHIPSET_CONFIGURATION *)gSUBrowser->SCBuffer;
  if (SetupVariable->BootTypeReserved == EFI_BOOT_TYPE) {
    Status = InitIdeConfigInEfiBootType (IdeConfig);
    if (!EFI_ERROR (Status)) {
      mIdeInit = TRUE;
    }
    return Status;
  } else {
    Status = gBS->LocateProtocol (
                    &gEfiLegacyBiosProtocolGuid,
                    NULL,
                    (VOID **)&LegacyBios
                    );

    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = LegacyBios->GetBbsInfo(
                           LegacyBios,
                           &HddCount,
                           &LocalHddInfo,
                           &BbsTotalCount,
                           &LocalBbsTable
                           );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Index         = 0;
    BbsTableIndex = 1;
    //
    // IDE Configuration
    //
    ModelNametring = AllocateZeroPool (0x100);
//[-start-130207-IB10870073-add]//
    ASSERT (ModelNametring != NULL);
    if (ModelNametring == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
//[-end-130207-IB10870073-add]//
    for (Channel=0; Channel<4; Channel++) {
      for (Device=0; Device<2; Device++) {
        //
        // Initial String
        //
        IdentifyTable = (EFI_ATAPI_IDENTIFY_DATA *) &LocalHddInfo[Channel].IdentifyDrive[Device];
        if (IdentifyTable->ModelName[0] != 0) {
//[-start-130625-IB10130054-modify]//
          //
          // BbsTable 1 ~ 8 is IDE mode HDD,
          // then BootPriority == 0xffff is Indicates invalid.
          //
          if (LocalBbsTable[BbsTableIndex].BootPriority == 0xFFFF) {
            PortNum = Index;
            mIsIDEModeFlag[PortNum] = FALSE;
//[-start-130716-IB07880077-add]//
            if (!IsOnBoardPciDevice (LocalBbsTable[BbsTableIndex].Bus,
                                     LocalBbsTable[BbsTableIndex].Device,
                                     LocalBbsTable[BbsTableIndex].Function)) {
              continue;
            }
//[-end-130716-IB07880077-add]//
          }
          else {
            //
            // IDE mode
            //
            Status = SearchMatchedPortNum (
                       LocalBbsTable[BbsTableIndex].Bus,
                       LocalBbsTable[BbsTableIndex].Device,
                       LocalBbsTable[BbsTableIndex].Function,
                       Channel % 2,
                       Device,
                       &PortNum
                       );
            if (EFI_ERROR (Status)) {
              BbsTableIndex++;
              continue;
            }
            mIsIDEModeFlag[PortNum] = TRUE;
          }

          if (IdeConfig[PortNum].DevNameString != NULL) {
            gBS->FreePool (IdeConfig[PortNum].DevNameString);
            IdeConfig[PortNum].DevNameString = NULL;
          }
          IdeConfig[PortNum].DevNameString     = AllocateZeroPool (0x100);
          IdeConfig[PortNum].IdeDevice         = 1;
          IdeConfig[PortNum].Device            = Device;
          IdeConfig[PortNum].Channel           = Channel;
          //
          // Get Device Model name
          //
          UpdateAtaString(
            (EFI_ATAPI_IDENTIFY_DATA *) &LocalHddInfo[Channel].IdentifyDrive[Device],
            &ModelNametring
            );
          IdeModelNameStrCpy(
            IdeConfig[PortNum].DevNameString,
            ModelNametring,
            20
            );
//[-end-130625-IB10130054-modify]//
        }

        Index++;
        BbsTableIndex++;
      }
    }
    gBS->FreePool (ModelNametring);
    mIdeInit = TRUE;
  }


  return EFI_SUCCESS;
}

EFI_STATUS
SwapIdeConfig (
  IDE_CONFIG                              *IdeConfig1,
  IDE_CONFIG                              *IdeConfig2
)
{
  UINT8                                   *Temp;

  Temp = AllocateZeroPool (sizeof(IDE_CONFIG));
  CopyMem (
    Temp,
    IdeConfig1,
    sizeof (IDE_CONFIG)
    );
  CopyMem (
    IdeConfig1,
    IdeConfig2,
    sizeof (IDE_CONFIG)
    );

  CopyMem (
    IdeConfig2,
    Temp,
    sizeof (IDE_CONFIG)
    );

  gBS->FreePool (Temp);
  return EFI_SUCCESS;
}

//[-start-121026-IB08520064-modify]//
/**
 According to Channel and device to get secruity data.

 @param        IdentifyData
 @param [in]   HddPortNumber
 @param [in]   Device             Device number of this channel.
 @param [out]  HaveSecurityData   TRUE: Have security data
                                  FALSE: Doesn't have security data.
 @param [out]  HarddiskInfoIndex  If have Security data, this value is the index in SECURITY_INFO_DATA array.

 @retval EFI_SUCCESS            Check have security data successful

**/
STATIC
EFI_STATUS
GetSecurityData (
  IDENTIFY_TAB       *IdentifyData,
  IN  UINT16         HddPortNumber,
  IN  UINT8          Device,
  OUT BOOLEAN        *HaveSecurityData,
  OUT UINTN          *HarddiskInfoIndex
  )
{
  UINTN                              Index;
  UINTN                              NumOfHarddisk;
//  SECURITY_INFO_DATA                 *HardDataInfo;
  HDD_PASSWORD_INFO_DATA             *HddPasswordDataInfo;
  CHIPSET_CONFIGURATION               *SetupVariable;
  SETUP_UTILITY_CONFIGURATION        *SCUInfo;
  HDD_SECURITY_INFO_DATA             *HarddiskInfoPtr;
  UINT8                                 SataCnfigure;

  SataCnfigure = gSUBrowser->SUCInfo->PrevSataCnfigure;
  SCUInfo       = gSUBrowser->SUCInfo;
  SetupVariable = (CHIPSET_CONFIGURATION *)gSUBrowser->SCBuffer;
  NumOfHarddisk = SCUInfo->NumOfHarddiskPswd;
//  HardDataInfo  = SCUInfo->HarddiskPassword;
  HddPasswordDataInfo = SCUInfo->HarddiskPassword;

  *HaveSecurityData = FALSE;

//  if ((NumOfHarddisk == 0) || (HardDataInfo == NULL)) {
  if ((NumOfHarddisk == 0) || (HddPasswordDataInfo == NULL)) {
    return EFI_UNSUPPORTED;
  }

  for (Index = 0; Index < NumOfHarddisk; Index++) {
    //
    // Search match Harddisk info
    //
    HarddiskInfoPtr = HddPasswordDataInfo[Index].HddDataInfo;
//[-start-130626-IB10130054-modify]//
    //if (!((SataCnfigure == IDE_MODE) ||
    //    ((SataCnfigure == AHCI_MODE) && (SetupVariable->AhciOptionRomSupport == 0)))) {
//[-start-131004-IB08520088-modify]//
    if (!mIsIDEModeFlag[Index]) {
//[-end-131004-IB08520088-modify]//	
//[-end-130626-IB10130054-modify]//

      if ((IdentifyData->config & bit(15)) != 0) {
       //
       // This is Atapi device
       //
        continue;
      }

          if (HarddiskInfoPtr->PortNumber != HddPortNumber) {
          continue;
        }

    } else {

      if (HarddiskInfoPtr->MappedPort != HddPortNumber) {
        continue;
      }
    }

    *HaveSecurityData = TRUE;
    *HarddiskInfoIndex = Index;
    break;
  }
  return EFI_SUCCESS;
}
//[-end-121026-IB08520064-modify]//
