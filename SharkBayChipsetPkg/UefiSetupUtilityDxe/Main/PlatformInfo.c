/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PlatformInfo.h>

PLATFORM_INFO_DISPLAY_TABLE mDisplayPlatformInfoFunction[] = {
  PLATFORM_INFO_DISPLAY_OPTION_TABLE_LIST
  };

//[-start-120911-IB06460449-add]//
EFI_HII_STRING_PROTOCOL   *mIfrLibHiiString;

/**

 Update new string to Hii Handle.

 @param [in]   SrcHiiHandle   New Hii Handle.
 @param [in]   SrcStringId    New String Token.
 @param [in]   DstHiiHandle   Hii Handle of the package to be updated.
 @param [out]   DstStringId   String Token to be updated.

 @retval EFI_SUCCESS          String update successfully.

**/
EFI_STATUS
NewStringToHandle (
  IN  EFI_HII_HANDLE           SrcHiiHandle,
  IN  EFI_STRING_ID            SrcStringId,
  IN  EFI_HII_HANDLE           DstHiiHandle,
  OUT EFI_STRING_ID            *DstStringId
  )
{
  EFI_STATUS                   Status;
  CHAR16                       *String;
  UINTN                        Size;
  UINTN                        StringSize;
  CHAR8                        *Languages;
  CHAR8                        *LangStrings;
  CHAR8                        Lang[RFC_3066_ENTRY_SIZE];


  StringSize = 0x200;
  String = AllocateZeroPool (StringSize);
  if (String == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Use english string as the default string.
  //
  Size = StringSize;
  Status = mIfrLibHiiString->GetString (
                               mIfrLibHiiString,
                               "en-US",
                               SrcHiiHandle,
                               SrcStringId,
                               String,
                               &Size,
                               NULL
                               );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    gBS->FreePool (String);
    StringSize = Size;
    String = AllocateZeroPool (StringSize);
    Status = mIfrLibHiiString->GetString (
                                 mIfrLibHiiString,
                                 "eng",
                                 SrcHiiHandle,
                                 SrcStringId,
                                 String,
                                 &Size,
                                 NULL
                                 );
    if (EFI_ERROR (Status)) {
      gBS->FreePool (String);
      return Status;
    }
  } else if (EFI_ERROR (Status)) {
    return Status;
  }

  *DstStringId = HiiSetString (DstHiiHandle, 0, String, NULL);

  //
  // Update string by each language.
  //
  Languages = HiiGetSupportedLanguages (DstHiiHandle);
  LangStrings = Languages;
  while (*LangStrings != 0) {
    SetupUtilityLibGetNextLanguage (&LangStrings, Lang);

    Size = StringSize;
    Status = mIfrLibHiiString->GetString (
                                 mIfrLibHiiString,
                                 Lang,
                                 SrcHiiHandle,
                                 SrcStringId,
                                 String,
                                 &Size,
                                 NULL
                                 );
    if (!EFI_ERROR (Status)) {
      mIfrLibHiiString->SetString (
                          mIfrLibHiiString,
                          DstHiiHandle,
                          *DstStringId,
                          Lang,
                          String,
                          NULL
                          );
    }
  }

  gBS->FreePool (String);
  gBS->FreePool (Languages);

  return EFI_SUCCESS;
}
//[-end-120911-IB06460449-add]//

EFI_STATUS
DisplayPlatformInfo (
  IN  SETUP_UTILITY_BROWSER_DATA            *SUBrowser
  )
{
  EFI_STATUS                Status;
//[-start-130717-IB05400432-remove]//
//  CHAR16                    *StringBuffer;
//[-end-130717-IB05400432-remove]//
  EFI_HII_HANDLE            LocalMainHiiHandle;
  EFI_HII_HANDLE            LocalAdvanceHiiHandle;
  STRING_REF                PlatformInfoTitleString;
  STRING_REF                PlatformInfoHelp;
  STRING_REF                BlankString;
  VOID                      *StartOpCodeHandle;
  EFI_IFR_GUID_LABEL        *StartLabel;

  Status                  = EFI_SUCCESS;
//[-start-130717-IB05400432-remove]//
//  StringBuffer            = NULL;
//[-end-130717-IB05400432-remove]//
  PlatformInfoHelp        = 0;
  PlatformInfoTitleString = 0;
  LocalMainHiiHandle      = SUBrowser->SUCInfo->MapTable[MainHiiHandle].HiiHandle;
  LocalAdvanceHiiHandle   = SUBrowser->SUCInfo->MapTable[AdvanceHiiHandle].HiiHandle;

//[-start-120911-IB06460449-add]//
  Status = gBS->LocateProtocol (&gEfiHiiStringProtocolGuid, NULL, (VOID **)&mIfrLibHiiString);
  ASSERT_EFI_ERROR (Status);
//[-end-120911-IB06460449-add]//

  //
  // Allocate space for creation of Buffer
  //
  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);
  
  StartLabel               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = MAIN_PAGE_PLATFORM_INFO_LABEL; 
    
  //
  // In order to prevent to grayout the label MAIN_PAGE_PLATFORM_INFO_LABEL,
  // the below precedure
  //
//[-start-120906-IB10820118-remove]//  
//HiiCreateEndOpCode (StartOpCodeHandle);
//[-end-120906-IB10820118-remove]//
//[-start-120808-IB10820099-remove]//
//  HiiCreateSuppressIfOpCode (StartOpCodeHandle);
//  HiiCreateFalseOpCode (StartOpCodeHandle);
//[-end-120808-IB10820099-remove]//
//[-start-120911-IB06460449-modify]//
//  StringBuffer=HiiGetString (LocalAdvanceHiiHandle, STRING_TOKEN (STR_PLATFORM_CONFIG_FORM_TITLE), NULL);
//  PlatformInfoTitleString=HiiSetString (LocalMainHiiHandle, 0, StringBuffer, NULL); 
//  gBS->FreePool (StringBuffer);
//
//  StringBuffer=HiiGetString (LocalAdvanceHiiHandle, STRING_TOKEN (STR_PLATFORM_CONFIG_FORM_HELP), NULL);
//  PlatformInfoHelp=HiiSetString (LocalMainHiiHandle, 0, StringBuffer, NULL);   
//  gBS->FreePool (StringBuffer);
//  
//  StringBuffer=HiiGetString (LocalAdvanceHiiHandle, STRING_TOKEN (STR_BLANK_STRING), NULL);
//  BlankString=HiiSetString (LocalMainHiiHandle, 0, StringBuffer, NULL);  
  NewStringToHandle (
    LocalAdvanceHiiHandle,
    STRING_TOKEN (STR_PLATFORM_CONFIG_FORM_TITLE),
    LocalMainHiiHandle,
    &PlatformInfoTitleString
    );

  NewStringToHandle (
    LocalAdvanceHiiHandle,
    STRING_TOKEN (STR_PLATFORM_CONFIG_FORM_HELP),
    LocalMainHiiHandle,
    &PlatformInfoHelp
    );

  NewStringToHandle (
    LocalAdvanceHiiHandle,
    STRING_TOKEN (STR_BLANK_STRING),
    LocalMainHiiHandle,
    &BlankString
    );
//[-end-120911-IB06460449-modify]//

  HiiCreateTextOpCode (StartOpCodeHandle,PlatformInfoTitleString, PlatformInfoHelp, BlankString );

  PlatformInfoInit (StartOpCodeHandle, LocalMainHiiHandle, LocalAdvanceHiiHandle);
  HiiUpdateForm (
    LocalMainHiiHandle,
    NULL,
    ROOT_FORM_ID,
    StartOpCodeHandle,  
    NULL
    );
//[-start-130717-IB05400432-remove]//
//  gBS->FreePool (StringBuffer);
//[-end-130717-IB05400432-remove]//
  HiiFreeOpCodeHandle (StartOpCodeHandle);
  return Status;
}

/**
 Init Platform Infomation Setup Form
 To display the following Data of platform infomation:
 1. CPUID
 2. Number of Core
 3. Number of Thread per core
 4. Microcode Version
 5. TXT
 6. VT-d
 7. VT-x
 8. PCH Reversion
 9. SA Reversion
 10.VBIOS Version
 11.EC Version

 @param [in]   OpCodeHandle
 @param [in]   MainHiiHandle
 @param [in]   AdvanceHiiHandle

 @retval EFI_SUCCESS

**/
EFI_STATUS
PlatformInfoInit (
  IN  VOID                         *OpCodeHandle,
  IN  EFI_HII_HANDLE               MainHiiHandle,
  IN  EFI_HII_HANDLE               AdvanceHiiHandle
  )
{
  EFI_STATUS             Status;
  UINTN                  Index;
  CHAR16                 *StringDataBuffer;

  Status           = EFI_SUCCESS;
  StringDataBuffer = NULL;

  StringDataBuffer = AllocateZeroPool (0x100);
  if (StringDataBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  for (Index = 0; mDisplayPlatformInfoFunction[Index].DisplayPlatformInfoFunction != NULL; Index++) {
    if (mDisplayPlatformInfoFunction[Index].Option == DISPLAY_ENABLE) {
      ZeroMem(StringDataBuffer, 0x100);
      mDisplayPlatformInfoFunction[Index].DisplayPlatformInfoFunction (OpCodeHandle, MainHiiHandle, AdvanceHiiHandle, StringDataBuffer);
    }
  }

  gBS->FreePool (StringDataBuffer);
  return Status;
}

//UINT8 *
//EFIAPI
//HiiCreateSuppressIfOpCode (
//  IN VOID       *OpCodeHandle
//  )
//{
//  EFI_IFR_SUPPRESS_IF     OpCode;
//
//  return InternalHiiCreateOpCode (OpCodeHandle, &OpCode, EFI_IFR_SUPPRESS_IF_OP, sizeof (OpCode));
//}

//UINT8 *
//EFIAPI
//HiiCreateFalseOpCode (
//  IN VOID       *OpCodeHandle,
//  )
//{
//  EFI_IFR_FALSE     OpCode;
//  
//  return InternalHiiCreateOpCode (OpCodeHandle, &OpCode, EFI_IFR_FALSE_OP, sizeof (OpCode));
//}
