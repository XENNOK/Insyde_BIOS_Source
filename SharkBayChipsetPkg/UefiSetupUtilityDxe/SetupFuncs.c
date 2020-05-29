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

#include <SetupFuncs.h>
//[-start-121108-IB05280008-remove]//
//#include <Protocol/OemServices.h>
//[-end-121108-IB05280008-remove]//
//[-start-121108-IB05280008-add]//
#include <Library/DxeOemSvcKernelLib.h>
//[-end-121108-IB05280008-add]//
//[-start-130403-IB09720138-remove]//
// #include <Protocol/ChipsetLibServices.h>
//[-end-130403-IB09720138-remove]//
#include <Protocol/MeBiosPayloadData.h>
#include <Guid/HobList.h>
#include <CpuRegs.h>
#include <CpuAccess.h>
#include <Library/BaseSetupDefaultLib.h>
STATIC LANGUAGE_DATA_BASE  *mSupportLangTable;
//[-start-120315-IB03090376-add]//
extern BOOLEAN mGetSetupNvDataFailed;
//[-end-120315-IB03090376-add]//
//[-start-120711-IB10820080-modify]//
//jdebug common code #include <Guid/SetupDefaultGuid.h>
extern EFI_GUID gSetupDefaultHobGuid;
//[-end-120711-IB10820080-modify]//

/**
 Given a token, return the string.

 @param [in]   HiiHandle        the handle the token is located
 @param [in]   Token            the string reference
 @param [in]   LanguageString   indicate what language string we want to get. if this is a
                                NULL pointer, using the current language setting to get string

 @retval *CHAR16                Returns the string corresponding to the token
 @retval NULL                   Cannot get string from Hii database

**/
CHAR16 *
GetTokenStringByLanguage (
  IN EFI_HII_HANDLE                             HiiHandle,
  IN STRING_REF                                 Token,
  IN CHAR8                                     *LanguageString
  )
{
  CHAR16                                      *Buffer;
  UINTN                                       BufferLength;
  EFI_STATUS                                  Status;
  EFI_HII_STRING_PROTOCOL                     *HiiString;

  HiiString = gSUBrowser->HiiString;
  //
  // Set default string size assumption at no more than 256 bytes
  //
  BufferLength = 0x100;

  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  BufferLength,
                  (VOID **)&Buffer
                  );
  if (EFI_ERROR (Status)) {
    return NULL;
  }
  ZeroMem (Buffer, BufferLength);
  Status = HiiString->GetString (
                        HiiString,
                        LanguageString,
                        HiiHandle,
                        Token,
                        Buffer,
                        &BufferLength,
                        NULL
                        );
  if (EFI_ERROR (Status)) {
    if (Status == EFI_BUFFER_TOO_SMALL) {
      //
      // Free the old pool
      //
      gBS->FreePool (Buffer);

      //
      // Allocate new pool with correct value
      //
      gBS->AllocatePool (
             EfiBootServicesData,
             BufferLength,
             (VOID **)&Buffer
             );
      Status = HiiString->GetString (
                            HiiString,
                            LanguageString,
                            HiiHandle,
                            Token,
                            Buffer,
                            &BufferLength,
                            NULL
                            );
      if (!EFI_ERROR (Status)) {
        //
        // return searched string
        //
        return Buffer;
      }
    }
    //
    // Cannot find string, free buffer and return NULL pointer
    //
    gBS->FreePool (Buffer);
    Buffer = NULL;
    return Buffer;
  }
  //
  // return searched string
  //
  return Buffer;
}


EFI_STATUS
CheckLanguage (
  VOID
  )
{
  EFI_STATUS                            Status;
  CHAR8                                 *LangCode;
  UINT8                                 *BackupLang;
  UINTN                                 BufferSize;
  UINTN                                 BackupBufferSize;


  LangCode   = GetVariableAndSize (L"PlatformLang", &gEfiGlobalVariableGuid, &BufferSize);
  BackupLang = GetVariableAndSize (L"BackupPlatformLang", &gEfiGenericVariableGuid, &BackupBufferSize);
  if (LangCode == NULL || !SetupUtilityLibIsLangCodeSupport (LangCode)) {
    //
    // if cannot find current language, set default language as english
    //
    if (BackupLang == NULL || !SetupUtilityLibIsLangCodeSupport (BackupLang)) {
      Status = gRT->SetVariable (
                      L"PlatformLang",
                      &gEfiGlobalVariableGuid,
                      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                      AsciiStrSize ((CHAR8 *) PcdGetPtr (PcdUefiVariableDefaultPlatformLang)),
                      (VOID *) PcdGetPtr (PcdUefiVariableDefaultPlatformLang)
                      );
    } else {
      Status = gRT->SetVariable (
                      L"PlatformLang",
                      &gEfiGlobalVariableGuid,
                      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                      BackupBufferSize,
                      BackupLang
                      );
    }

    gRT->SetVariable (
           L"BackupPlatformLang",
           &gEfiGenericVariableGuid,
           EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
           0,
           NULL
           );
    if (LangCode != NULL) {
      gBS->FreePool (LangCode);
    }
    if (BackupLang != NULL) {
      gBS->FreePool (BackupLang);
    }
    return Status;
  }

  if (BackupLang != NULL) {
    if (BackupBufferSize != BufferSize || CompareMem (BackupLang, LangCode, BufferSize)) {
      Status = gRT->SetVariable (
                      L"PlatformLang",
                      &gEfiGlobalVariableGuid,
                      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                      BackupBufferSize,
                      BackupLang
                      );
    }

    Status = gRT->SetVariable (
                    L"BackupPlatformLang",
                    &gEfiGenericVariableGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    0,
                    NULL
                    );
    gBS->FreePool (BackupLang);
  }

  gBS->FreePool (LangCode);

  return EFI_SUCCESS;
}

/**
 Read the EFI variable (VendorGuid/Name) and return a dynamically allocated
 buffer, and the size of the buffer. If failure return NULL.

 Name           String part of EFI variable name
 VendorGuid     GUID part of EFI variable name
 VariableSize   Returns the size of the EFI variable that was read

 @return Dynamically allocated memory that contains a copy of the EFI variable.
 @return Caller is responsible freeing the buffer.
 @retval NULL                   Variable was not read

**/
VOID *
GetVariableAndSize (
  IN  CHAR16              *Name,
  IN  EFI_GUID            *VendorGuid,
  OUT UINTN               *VariableSize
  )
{
  EFI_STATUS  Status;
  UINTN       BufferSize;
  VOID        *Buffer;

  Buffer = NULL;

  //
  // Pass in a zero size buffer to find the required buffer size.
  //
  BufferSize  = 0;

  Status      = gRT->GetVariable (Name, VendorGuid, NULL, &BufferSize, Buffer);
  if (Status == EFI_BUFFER_TOO_SMALL) {
    //
    // Allocate the buffer to return
    //

    Buffer = AllocateZeroPool (BufferSize);
    if (Buffer == NULL) {
      return NULL;
    }

    //
    // Read variable into the allocated buffer.
    //

    Status = gRT->GetVariable (Name, VendorGuid, NULL, &BufferSize, Buffer);
    if (EFI_ERROR (Status)) {
      BufferSize = 0;
      gBS->FreePool (Buffer);
      Buffer = NULL;
    }

  }

  *VariableSize = BufferSize;

  return Buffer;
}

/**
 Function to update the ATA strings into Model Name -- Size

 @param [in]        IdentifyDriveInfo
 @param [in, out]   NewString

 @return Will return model name and size (or ATAPI if nonATA)

**/
EFI_STATUS
UpdateAtaString (
  IN      EFI_ATAPI_IDENTIFY_DATA     *IdentifyDriveInfo,
  IN OUT  CHAR16                      **NewString
  )
{
  CHAR8                       *TempString;
  UINT16                      Index;
  CHAR8                       Temp8;

  TempString = AllocateZeroPool (0x100);
//[-start-130207-IB10870073-add]//
  ASSERT (TempString != NULL);
  if (TempString == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130207-IB10870073-add]//

  CopyMem (TempString, IdentifyDriveInfo->ModelName, sizeof (IdentifyDriveInfo->ModelName));

  //
  // Swap the IDE string since Identify Drive format is inverted
  //
  Index = 0;
  while (TempString[Index] != 0 && TempString[Index+1] != 0) {
    Temp8 = TempString[Index];
    TempString[Index] = TempString[Index+1];
    TempString[Index+1] = Temp8;
    Index +=2;
  }
  AsciiToUnicode (TempString, *NewString);

  if (TempString != NULL) {
    gBS->FreePool(TempString);
  }

  return EFI_SUCCESS;
}

/**
 Function to convert ASCII string to Unicode

 @param [in]   AsciiString
 @param [in]   UnicodeString


**/
EFI_STATUS
AsciiToUnicode (
  IN    CHAR8     *AsciiString,
  IN    CHAR16    *UnicodeString
  )
{
  UINT8           Index;

  Index = 0;
  while (AsciiString[Index] != 0) {
    UnicodeString[Index] = (CHAR16)AsciiString[Index];
    Index++;
  }
  UnicodeString[Index] = 0;

  return EFI_SUCCESS;
}

/**
 Control Event Timer start and end.

 @param [in]   Timeout          Event time, Zero is Stop the timer

 @retval EFI_SUCCESS            Control Event Timer is success.

**/
EFI_STATUS
EventTimerControl (
  IN UINT64                     Timeout
  )
{

  //
  // If the system health event is create for this Key value ,we can stop event
  //
  if (gSCUSystemHealth == HAVE_CREATE_SYSTEM_HEALTH_EVENT) {
    //
    // Set the timer event
    //
    gBS->SetTimer (
              gSCUTimerEvent,
              TimerPeriodic,
              Timeout
              );
  }

  return EFI_SUCCESS;

}

/**
 According Bus, Device, Function, PrimarySecondary, SlaveMaster to get corresponding
 SATA port number

 @param [in]   Bus              PCI bus number
 @param [in]   Device           PCI device number
 @param [in]   Function         PCI function number
 @param [in]   PrimarySecondary  primary or scondary
 @param [in]   SlaveMaster      slave or master
 @param [in, out] PortNum       An address to save SATA port number.

 @retval EFI_SUCCESS            get corresponding port number successfully
 @retval EFI_INVALID_PARAMETER  input parameter is invalid
 @retval EFI_NOT_FOUND          can't get corresponding port number

**/
EFI_STATUS
SearchMatchedPortNum (
  IN     UINT32                              Bus,
  IN     UINT32                              Device,
  IN     UINT32                              Function,
  IN     UINT8                               PrimarySecondary,
  IN     UINT8                               SlaveMaster,
  IN OUT UINTN                               *PortNum
  )
{
  UINTN                 Index;
  PORT_NUMBER_MAP       *PortMappingTable;
  UINTN                 NoPorts;
//[-start-121108-IB05280008-modify]//
  PORT_NUMBER_MAP                   EndEntry;

  PortMappingTable      = NULL;

  ZeroMem (&EndEntry, sizeof (PORT_NUMBER_MAP));

  PortMappingTable = (PORT_NUMBER_MAP *)PcdGetPtr (PcdPortNumberMapTable);

  NoPorts = 0;
  while (CompareMem (&EndEntry, &PortMappingTable[NoPorts], sizeof (PORT_NUMBER_MAP)) != 0) {
    NoPorts++;
  }

  if (NoPorts == 0) {
//[-end-121108-IB05280008-modify]//
    return EFI_NOT_FOUND;
  }

  for (Index = 0; Index < NoPorts; Index++) {
    if ((PortMappingTable[Index].Bus == Bus) &&
        (PortMappingTable[Index].Device == Device) &&
        (PortMappingTable[Index].Function == Function) &&
        (PortMappingTable[Index].PrimarySecondary == PrimarySecondary) &&
        (PortMappingTable[Index].SlaveMaster == SlaveMaster)) {
      *PortNum = PortMappingTable[Index].PortNum;
      return EFI_SUCCESS;

    }
  }
  return EFI_NOT_FOUND;
}

/**
 Check this SATA port number is whther support

 @param [in]   PortNum          SATA port number

 @retval EFI_SUCCESS            platform supports this SATA port number
 @retval EFI_UNSUPPORTED        platform unsupports this SATA port number

**/
EFI_STATUS
CheckSataPort (
  IN UINTN                                  PortNum
)
{
  UINTN                 Index;
  PORT_NUMBER_MAP       *PortMappingTable;
  UINTN                 NoPorts;
//[-start-121108-IB05280008-modify]//
  PORT_NUMBER_MAP                   EndEntry;

  PortMappingTable      = NULL;

  ZeroMem (&EndEntry, sizeof (PORT_NUMBER_MAP));

  PortMappingTable = (PORT_NUMBER_MAP *)PcdGetPtr (PcdPortNumberMapTable);

  NoPorts = 0;
  while (CompareMem (&EndEntry, &PortMappingTable[NoPorts], sizeof (PORT_NUMBER_MAP)) != 0) {
    NoPorts++;
  }
  if (NoPorts == 0) {
//[-end-121108-IB05280008-modify]//
    return EFI_UNSUPPORTED;;
  }

  for (Index = 0; Index < NoPorts; Index++) {
    if (PortMappingTable[Index].PortNum == PortNum) {
      return EFI_SUCCESS;
    }
  }
  return EFI_UNSUPPORTED;
}


/**
 Save Setup Configuration to NV storage and call KERNEL_CALCULATE_WRITE_CMOS_CHECKSUM
 OEM service

 @param [in]   VariableName     The name of variable to load
 @param [in]   VendorGuid       It's vendor GUID
 @param [in]   Attributes       attributes of the data to be stored
 @param [in]   DataSize         The size of the data
 @param [in]   Buffer           Space to store data to be written to NVRam

 @retval EFI_SUCCESS            Save Setup Configuration successful
 @retval EFI_INVALID_PARAMETER  The input parameter is invalid
 @return Other                  Some other error occured

**/
EFI_STATUS
SaveSetupConfig (
  IN     CHAR16             *VariableName,
  IN     EFI_GUID           *VendorGuid,
  IN     UINT32             Attributes,
  IN     UINTN              DataSize,
  IN     VOID               *Buffer
  )
{
  EFI_STATUS                Status;
//[-start-121108-IB05280008-remove]//
//  OEM_SERVICES_PROTOCOL     *OemServices;
//[-end-121108-IB05280008-remove]//


  if (Buffer == NULL || VariableName == NULL || VendorGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gRT->SetVariable (
                  VariableName,
                  VendorGuid,
                  Attributes,
                  DataSize,
                  Buffer
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

//[-start-121108-IB05280008-remove]//
//  Status = gBS->LocateProtocol (
//                  &gOemServicesProtocolGuid,
//                  NULL,
//                  &OemServices
//                  );
//  if (EFI_ERROR (Status)) {
//    return Status;
//  }
//[-end-121108-IB05280008-remove]//
  //Status = OemServices->Funcs[KERNEL_CALCULATE_WRITE_CMOS_CHECKSUM] (
//[-start-121108-IB05280008-modify]//
  //Status = OemServices->Funcs[COMMON_CALCULATE_WRITE_CMOS_CHECKSUM] (
  //                        OemServices,
  //                        COMMON_CALCULATE_WRITE_CMOS_CHECKSUM_ARG_COUNT
  //                        );
  Status = OemSvcCalculateWriteCmosChecksum ();
  DEBUG ( ( EFI_D_ERROR | EFI_D_INFO, "DxeOemSvcKernelLib CalculateWriteCmosChecksum, Status : %r\n", Status ) );
//[-end-121108-IB05280008-modify]//

  return EFI_SUCCESS;
}
/**
 According the priority of langdef in UNI file to add the supported language code
 to supported language database.

 @param None

 @retval EFI_SUCEESS            Initialize supported language database successful

**/
STATIC
EFI_STATUS
InitializeSupportLanguage (
  VOID
  )
{
  UINT8           *LanguageString;
  UINT8           *Language;
  UINT8           Lang[RFC_3066_ENTRY_SIZE];
  UINT8           *SupportedLanguage;
  UINT8           *TempSupportedLanguage;
  UINTN           SupportedLangCnt;
  UINTN           SavedLang;
  UINTN           Index;
  UINTN           BufferSize;


  SupportedLanguage = GetVariableAndSize (L"PlatformLangCodes", &gEfiGlobalVariableGuid, &BufferSize);
//[-start-130207-IB10870073-add]//
  ASSERT (SupportedLanguage != NULL);
  if (SupportedLanguage == NULL) {
    return EFI_NOT_FOUND;
  }
//[-end-130207-IB10870073-add]//
  SupportedLangCnt = 1;
  for (Index = 0; SupportedLanguage[Index] != 0; Index++) {
    if (SupportedLanguage[Index] == ';') {
      SupportedLangCnt++;
      SupportedLanguage[Index] = 0;
    }
  }
  mSupportLangTable = AllocateZeroPool (SupportedLangCnt * RFC_3066_ENTRY_SIZE + sizeof (UINTN));
//[-start-130207-IB10870073-add]//
  ASSERT (mSupportLangTable != NULL);
  if (mSupportLangTable == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130207-IB10870073-add]//
  LanguageString = HiiGetSupportedLanguages (gSUBrowser->SUCInfo->MapTable[ExitHiiHandle].HiiHandle);
//[-start-130207-IB10870073-add]//
  ASSERT (LanguageString != NULL);
  if (LanguageString == NULL) {
    return EFI_NOT_FOUND;
  }
//[-end-130207-IB10870073-add]//
  Language = LanguageString;

  SavedLang = 0;
  while (*Language != 0) {
    SetupUtilityLibGetNextLanguage (&Language, Lang);
    TempSupportedLanguage = SupportedLanguage;
    for (Index = 0; Index < SupportedLangCnt; Index++) {
      if (!AsciiStrCmp ((CHAR8 *)Lang, (CHAR8 *)TempSupportedLanguage)) {
        AsciiStrCpy (&mSupportLangTable->LangString[SavedLang * RFC_3066_ENTRY_SIZE], (CHAR8 *)Lang);
        SavedLang++;
        break;
      }
      TempSupportedLanguage += AsciiStrSize ((CHAR8 *)TempSupportedLanguage);
    }
  }
  mSupportLangTable->LangNum = SavedLang;
  gBS->FreePool(LanguageString);
  gBS->FreePool(SupportedLanguage);
  return EFI_SUCCESS;

}

/**
 Get supported language database. This funciton will return supported language number
 and language string

 @param [out]  LangNumber       Pointer to supported language number
 @param [out]  LanguageString   A double pointer to save the start of supported language string

 @retval EFI_SUCEESS            Initialize supported language database successful

**/
EFI_STATUS
GetLangDatabase (
  OUT UINTN            *LangNumber,
  OUT UINT8            **LanguageString
  )
{
  UINTN       TotalSize;

  if (LangNumber == NULL || LanguageString == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if (mSupportLangTable == NULL) {
    InitializeSupportLanguage ();
  }
  *LangNumber = mSupportLangTable->LangNum;
  TotalSize = *LangNumber * RFC_3066_ENTRY_SIZE;
  *LanguageString = AllocateZeroPool (TotalSize);
  CopyMem (*LanguageString, mSupportLangTable->LangString, TotalSize);

  return EFI_SUCCESS;
}

/**
 Get string from input HII handle and add this string

 @param [in]   InputHiiHandle
 @param [in]   OutputHiiHandle
 @param [in]   InputToken
 @param [out]  OutputToken

 @retval EFI_SUCCESS            Update system bus speed success

**/
EFI_STATUS
AddNewString (
  IN   EFI_HII_HANDLE           InputHiiHandle,
  IN   EFI_HII_HANDLE           OutputHiiHandle,
  IN   STRING_REF               InputToken,
  OUT  STRING_REF               *OutputToken
  )
{
  UINTN                      LanguageCount;
  UINTN                      TotalLanguageCount;
  CHAR8                     *LanguageString;
  CHAR16                     *TokenString;
  EFI_STATUS                 Status;
  STRING_REF                 AddedToken;


  //
  // Get all of supported language
  //
  Status = GetLangDatabase (&TotalLanguageCount, (UINT8 **)&LanguageString);
    if (EFI_ERROR (Status)) {
    return Status;
  }

  AddedToken = 0;
  //
  // Create new string token for this string
  //
  TokenString=HiiGetString (InputHiiHandle, InputToken, NULL);
  ASSERT (TokenString != NULL);
  AddedToken=HiiSetString (OutputHiiHandle, 0, TokenString, NULL);
  gBS->FreePool (TokenString);
  //
  // according to different language to get string token from input HII handle and
  // add this string to oupt string hanlde
  //
  for (LanguageCount = 0; LanguageCount < TotalLanguageCount; LanguageCount++) {
    TokenString = GetTokenStringByLanguage (
                    InputHiiHandle,
                    InputToken,
                    &LanguageString[LanguageCount * RFC_3066_ENTRY_SIZE]
                    );
    if (TokenString != NULL) {
      Status = gSUBrowser->HiiString->SetString (
                                        gSUBrowser->HiiString,
                                        OutputHiiHandle,
                                        AddedToken,
                                        &LanguageString[LanguageCount * RFC_3066_ENTRY_SIZE],
                                        TokenString,
                                        NULL
                                        );
      gBS->FreePool (TokenString);
    }
  }
  gBS->FreePool (LanguageString);
  *OutputToken = AddedToken;

  return EFI_SUCCESS;
}


//[-start-130709-IB05400426-add]//
/**
 Update GBE and root port value from PCH soft strap.

 @param[in]         SetupNvData         Buffer that contains setup data.

*/
VOID
UpdateValueByStrap (
  IN  CHIPSET_CONFIGURATION          *SetupNvData
  )
{
  UINT32                                    RootComplexBar;
  UINT32                                    RegData32;
  UINT8                                     GbePciePortSelect;


  //
  // Get GBE Port from SPI Soft Strap 9
  //
  RootComplexBar = PchLpcPciCfg32 (R_PCH_LPC_RCBA) & B_PCH_LPC_RCBA_BAR;
  MmioWrite32 (RootComplexBar + R_PCH_SPI_FDOC, V_PCH_SPI_FDOC_FDSS_PCHS | R_PCH_SPI_STRP9);
  RegData32 = MmioRead32 (RootComplexBar + R_PCH_SPI_FDOD);
  GbePciePortSelect = (UINT8) ((RegData32 & B_PCH_SPI_STRP9_GBE_PCIE_PSC) >> N_PCH_SPI_STRP9_GBE_PCIE_PSC);

  SetupNvData->GbePeiEPortSelect = GbePciePortSelect;
  DEBUG ((EFI_D_ERROR, "GbePciePortSelect = %x\n", GbePciePortSelect));
  //
  // Get PCH PCIe Root Port Info
  //
  SetupNvData->PcieRpStrap0 = (UINT8) (RegData32 & (B_PCH_SPI_STRP9_PCIEPCS1));
  SetupNvData->PcieRpStrap1 = (UINT8) ((RegData32 & (B_PCH_SPI_STRP9_PCIEPCS2)) >> 2);
  
  DEBUG ((EFI_D_ERROR, "PcieRpStrap0 = %x\n", SetupNvData->PcieRpStrap0));
  DEBUG ((EFI_D_ERROR, "PcieRpStrap1 = %x\n", SetupNvData->PcieRpStrap1));

  
}
//[-end-130709-IB05400426-add]//


/**
 Build a default variable value an save to a buffer according to platform requirement

 @param [out]  SetupNvData

 @retval EFI_SUCCESS            Build default setup successful.
 @retval EFI_INVALID_PARAMETER  Input value is invalid.

**/
EFI_STATUS
DefaultSetup (
  OUT CHIPSET_CONFIGURATION          *SetupNvData
  )
{
  VOID                                      *HobList;
  UINT8                                     *Raw;
  UINTN                                     Size=0;
//[-start-121005-IB05330380-add]//
  EFI_STATUS                                Status;
  PLATFORM_INFO_PROTOCOL                   *PlatformInfoProtocol;
//[-end-121005-IB05330380-add]//
  if (SetupNvData == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
//[-start-120229-IB03090376-modify]//
  if (mGetSetupNvDataFailed) {
    HobList = GetHobList ();
    Raw = GetNextGuidHob (&gSetupDefaultHobGuid, HobList);
    if (Raw != NULL) {
      Size = (UINTN) PcdGet32 (PcdSetupConfigSize);
      Raw = GET_GUID_HOB_DATA (Raw);
      CopyMem (SetupNvData, Raw, Size);
      SetupNvData->SetupVariableInvalid = 0;
    }
  } else {
    ExtractSetupDefault ((UINT8 *) SetupNvData );
  }
//[-end-120229-IB03090376-modify]//
//[-start-121005-IB05330380-modify]//
  Status = gBS->LocateProtocol (&gEfiPlatformInfoProtocolGuid, NULL, (VOID **)&PlatformInfoProtocol);
  ASSERT_EFI_ERROR (Status);    
  if (PlatformInfoProtocol->PlatInfo.UltPlatform) {
    SetupNvData->GbePeiEPortSelect = 4;    // Default GBE is on Function 4
  } else {
    //
    // Default GBE is on Function 
    // 5 (SharkBay NB)
    // 2 (SharkBay DT FHC)
    // 3 (SharkBay DT SFF)
    //
    SetupNvData->GbePeiEPortSelect = PcdGet8 (PcdGbePeiEPortSelect);
  }
//[-end-121005-IB05330380-modify]//

//[-start-130709-IB05400426-add]//
  UpdateValueByStrap (SetupNvData);
//[-end-130709-IB05400426-add]//

  SetupNvData->DefaultBootType   = DEFAULT_BOOT_FLAG;
  SetupNvData->Timeout = (UINT16) PcdGet16 (PcdPlatformBootTimeOut);
  GetLangIndex ((CHAR8 *) PcdGetPtr (PcdUefiVariableDefaultPlatformLang), &SetupNvData->Language);

  return EFI_SUCCESS;
}

//[-start-120301-IB03090376-remove]//
////[-start-120109-IB03090372-add]//
//VOID
//ClearSetupVariableInvalid (
//  VOID
//  )
///*++
//
//Routine Description:
//
//  Clear SetupVariableInvalid flag in SetupDefault HOB to indicat the Setup Variable is created.
//
//Arguments:
//
//  None
//
//Returns:
//
//  None
//
//--*/
//{
//  EFI_STATUS                                Status;
//  VOID                                      *HobList;
//  UINT8                                     *Raw;
//  UINTN                                     Size;
//
//  EfiLibGetSystemConfigurationTable (&gEfiHobListGuid, (VOID *) &HobList);
//  Status = GetNextGuidHob (&HobList, &gSetupDefaultHobGuid, &Raw, &Size);
//  if (!EFI_ERROR (Status)) {
//    ((CHIPSET_CONFIGURATION *)Raw)->SetupVariableInvalid = 0;
//  }
//  
//  return;
//}
////[-end-120109-IB03090372-add]//
//[-end-120301-IB03090376-remove]//
EFI_STATUS
SetupRuntimeDetermination (
  IN OUT  CHIPSET_CONFIGURATION        *SetupNvData
  )
{
  EFI_STATUS                Status;
  MEM_INFO_PROTOCOL         *MemoryInfo;
  EFI_CPUID_REGISTER        CpuidRegisters;

  MemoryInfo            = NULL;
  CpuidRegisters.RegEax = 0;
  CpuidRegisters.RegEbx = 0;
  CpuidRegisters.RegEcx = 0;
  CpuidRegisters.RegEdx = 0;

  if ( ( SetupNvData->PrimaryPlaneCurrentLimit != 0 ) && ( SetupNvData->PrimaryPlaneCurrentLimit < 256 ) ) {
    SetupNvData->PrimaryPlaneCurrentLimit = 256;
  }
  if ( ( SetupNvData->SecondaryPlaneCurrentLimit != 0 ) && ( SetupNvData->SecondaryPlaneCurrentLimit < 128 ) ) {
    SetupNvData->SecondaryPlaneCurrentLimit = 128;
  }

  Status = gBS->LocateProtocol ( &gMemInfoProtocolGuid, NULL, (VOID **)&MemoryInfo );
  ASSERT_EFI_ERROR ( Status );

  if ( MemoryInfo->MemInfoData.memSize >= 256 ) {
    SetupNvData->MemoryLess256MB = 0;
  } else {
    SetupNvData->MemoryLess256MB = 1;
  }

  //
  // If memory size <= 512 MB, only pre-allocate 32 MB for DVMT.
  //
  if ( MemoryInfo->MemInfoData.memSize <= 512 ) {
    SetupNvData->DvmtPreAllocated = 1;
  }

  //
  // Turbo Mode Capability
  //
  AsmCpuid ( CPUID_POWER_MANAGEMENT_PARAMS, &CpuidRegisters.RegEax, &CpuidRegisters.RegEbx, &CpuidRegisters.RegEcx, &CpuidRegisters.RegEdx );
  if ( !( CpuidRegisters.RegEax & B_CPUID_POWER_MANAGEMENT_EAX_TURBO ) && !( AsmReadMsr64 ( MSR_IA32_MISC_ENABLE ) & B_MSR_IA32_MISC_DISABLE_TURBO ) ) {
    SetupNvData->TurboCap = 0;
  } else {
    SetupNvData->TurboCap = 1;
  }
  if ( !SetupNvData->TurboCap ) {
    SetupNvData->EnableTurboMode = 0;
  }

  //
  // AES Capability
  //
  AsmCpuid ( CPUID_VERSION_INFO, &CpuidRegisters.RegEax, &CpuidRegisters.RegEbx, &CpuidRegisters.RegEcx, &CpuidRegisters.RegEdx );
  if ( CpuidRegisters.RegEcx & B_CPUID_VERSION_INFO_ECX_AES ) {
    SetupNvData->AESSupport = 1;
  } else {
    SetupNvData->AESSupport = 0;
  }

  PEGDetect ( SetupNvData );

  if (FeaturePcdGet (PcdMeSupported)) {
    SetupNvData->MeFwImageType = MeFwTypeDetect ();
  }

//[-start-120403-IB05300308-modify]//
  if ( SetupNvData->EnableDptf ) {
    SetupNvData->Device4Enable = 1;
    SetupNvData->ThermalDevice = 1;
  }
//[-end-120403-IB05300308-modify]//

  SetupNvData->ConfigTDPCapability = ( ( AsmReadMsr64 ( MSR_PLATFORM_INFO ) & CONFIG_TDP_NUM_LEVELS_MASK ) == 0 ) ? FALSE : TRUE;

//[-start-130307-IB03780481-add]//
  if (FeaturePcdGet (PcdThunderBoltSupported)) {
    if (SetupNvData->TbtWakeFromDevice) {
      SetupNvData->WakeOnPME = 1;
    }
  }
//[-end-130307-IB03780481-add]//

  return EFI_SUCCESS;
}

/**

 @param [in]   SetupNvData

 @retval Nothing

**/
EFI_STATUS
PEGDetect (
  IN  CHIPSET_CONFIGURATION              *SetupNvData
  )
{

  UINT16                                PegVid;
  UINT8                                 PegClass;
  UINT32                                OrgBus;
  UINT32                                OrgSubBus;
  UINT32                                HostBridgeDev;        // Host PCI Express Bridge Dev#
  UINT16                                HostBridgeVid;

  HostBridgeDev = 1;
  HostBridgeVid = MmPci16 (0, 0, HostBridgeDev, 0, PCI_VID);
  OrgBus = MmPci32 (0, 0, HostBridgeDev, 0, PCI_PBUS);
  MmPci32 (0, 0, HostBridgeDev, 0, PCI_PBUS) = 0x00010100;    // Assume bus 1 for PEG device
  PegVid = MmPci16 (0, 1, 0, 0, PCI_VID);

  if (PegVid != 0xFFFF) {
    PegClass = MmPci8 (0, 1, 0, 0, PCI_BCC);                  // Class code
    if (PegClass == 0x03) {                                   // VGA?
      SetupNvData->PEGFlag = 1;                               // Don't show IGD options
    } else if (PegClass == 0x06) {                            // Bridge?
      //
      // x1 VGA card is beneath the PCI-PCIE bridge
      // Assume bus 1 for PCI-PCIE bridge
      // Assume bus 2 for PEG device
      //
      MmPci32 (0, 0, HostBridgeDev, 0, PCI_PBUS) = 0x00020100;
      OrgSubBus = MmPci32 (0, 1, 0, 0, PCI_PBUS);
      MmPci32 (0, 1, 0, 0, PCI_PBUS) = 0x00020200;            // Assume bus 2 for PEG device
      PegClass = MmPci8 (0, 2, 0, 0, PCI_BCC);
      if (PegClass == 0x03) {                                 // VGA?
        SetupNvData->PEGFlag = 1;                             // Don't show IGD options
      } else {
        SetupNvData->PEGFlag = 0;                             // Show IGD options
      }
      MmPci32 (0, 1, 0, 0, PCI_PBUS) = OrgSubBus;
    } else {
      SetupNvData->PEGFlag = 0;                               // Show IGD options
    }
  } else {
    SetupNvData->PEGFlag = 0;                                 // Show IGD options
  }

  MmPci32 (0, 0, HostBridgeDev, 0, PCI_PBUS) = OrgBus;
  return EFI_SUCCESS;
}

//[-start-120301-IB07360173-modify]//
UINT8
MeFwTypeDetect (
  )
{
  EFI_STATUS                   Status;
  DXE_MBP_DATA_PROTOCOL        *MBPDataProtocol;

  Status = gBS->LocateProtocol (
             &gMeBiosPayloadDataProtocolGuid,
             NULL,
             (VOID **)&MBPDataProtocol
             );

  return ( UINT8 )( MBPDataProtocol->MeBiosPayload.FwPlatType.RuleData.Fields.IntelMeFwImageType );
}
//[-end-120301-IB07360173-modify]//
