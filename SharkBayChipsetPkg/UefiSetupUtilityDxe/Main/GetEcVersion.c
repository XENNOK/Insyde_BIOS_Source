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
//[-start-130422-IB05400398-add]//
#include <Library/DxeOemSvcChipsetLib.h>
//[-end-130422-IB05400398-add]//
//[-start-130812-IB06720232-add]//
#include <Library/BaseOemSvcChipsetLib.h>
//[-end-130812-IB06720232-add]//

//[-start-120620-IB10540011-add]//
EC_INFO_DISPLAY_TABLE        mDisplayEcFunction[]       = {EC_INFO_DISPLAY_TABLE_LIST};
//[-end-120620-IB10540011-add]//


//[-start-120620-IB10540011-add]//
EFI_STATUS
GetEcVersion (
  IN     VOID                      *OpCodeHandle,
  IN     EFI_HII_HANDLE            MainHiiHandle,
  IN     EFI_HII_HANDLE            AdvanceHiiHandle,
  IN     CHAR16                    *StringBuffer  )
{
  EFI_STATUS                    Status = EFI_SUCCESS;
  UINTN                         Index;
  
  for (Index = 0; mDisplayEcFunction[Index].DisplayEcFunction != NULL; Index++) {
    if (mDisplayEcFunction[Index].Option == DISPLAY_ENABLE) {
      ZeroMem(StringBuffer, 0x100);
      Status = mDisplayEcFunction[Index].DisplayEcFunction (OpCodeHandle, MainHiiHandle, AdvanceHiiHandle, StringBuffer);
    }
  }

//[-start-120709-IB06460415-remove]//
//  gBS->FreePool (StringBuffer);
//[-end-120709-IB06460415-remove]//
  return Status;
}
//[-end-120620-IB10540011-add]//


//[-start-120620-IB10540011-modify]//
EFI_STATUS
GetEcVerIdFunc (
  IN     VOID                      *OpCodeHandle,
  IN     EFI_HII_HANDLE            MainHiiHandle,
  IN     EFI_HII_HANDLE            AdvanceHiiHandle,
  IN     CHAR16                    *StringBuffer
  )
//[-end-120620-IB10540011-modify]//
{
  EFI_STATUS          Status;
  UINT8               MajorNum;
  UINT8               MinorNum;
  STRING_REF          EcVersionText;
  STRING_REF          EcVersionString;
//[-start-130809-IB06720232-add]//
  EFI_STATUS          ReadEcVersionStatus;
//[-end-130809-IB06720232-add]//

  Status = EFI_UNSUPPORTED;
  MajorNum = 0;
  MinorNum = 0;
//[-start-130809-IB06720232-add]//
  ReadEcVersionStatus = EFI_UNSUPPORTED;
//[-end-130809-IB06720232-add]//
  
//[-start-130809-IB06720232-modify]//
  OemSvcEcVersion (&ReadEcVersionStatus, &MajorNum, &MinorNum);

  if (!EFI_ERROR(ReadEcVersionStatus)) {
    //
    // Update EC Version to SCU
    //
    UnicodeSPrint (StringBuffer, 0x100, L"%x.%02x", (UINTN)MajorNum, (UINTN)MinorNum);
    EcVersionString=HiiSetString (MainHiiHandle, 0, StringBuffer, NULL);
  } else {
    //
    // Update Default EC Version to SCU
    //
//[-start-121002-IB06460449-modify]//
//    StringBuffer=HiiGetString (AdvanceHiiHandle, STRING_TOKEN (STR_EC_VERSION_STRING), NULL);
//    
//    EcVersionString=HiiSetString (MainHiiHandle, 0, StringBuffer, NULL);
    NewStringToHandle (
      AdvanceHiiHandle,
      STRING_TOKEN (STR_EC_VERSION_STRING),
      MainHiiHandle,
      &EcVersionString
      );
//[-end-121002-IB06460449-modify]//
  }
//[-end-130809-IB06720232-modify]//

//[-start-121002-IB06460449-modify]//
//  StringBuffer=HiiGetString (AdvanceHiiHandle, STRING_TOKEN (STR_EC_VERSION_TEXT), NULL);
//  
//  EcVersionText=HiiSetString (MainHiiHandle, 0, StringBuffer, NULL);    
    NewStringToHandle (
      AdvanceHiiHandle,
      STRING_TOKEN (STR_EC_VERSION_TEXT),
      MainHiiHandle,
      &EcVersionText
      );
//[-end-121002-IB06460449-modify]//
  
  HiiCreateTextOpCode (OpCodeHandle,EcVersionText, 0, EcVersionString );

  return Status;
}


//[-start-120620-IB10540011-add]//
EFI_STATUS
GetBoardFunc (
  IN     VOID                      *OpCodeHandle,
  IN     EFI_HII_HANDLE            MainHiiHandle,
  IN     EFI_HII_HANDLE            AdvanceHiiHandle,
  IN     CHAR16                    *StringBuffer
  )
{
//[-start-130613-IB05400415-modify]//
  EFI_STATUS                          Status;
  STRING_REF                          BoardIdText;
  STRING_REF                          BoardIdString;
  UINT8                               BoardFabId;
  PLATFORM_INFO_PROTOCOL              *PlatformInfoProtocol;


  Status = gBS->LocateProtocol (&gEfiPlatformInfoProtocolGuid, NULL, (VOID **)&PlatformInfoProtocol);
  ASSERT_EFI_ERROR (Status);
  BoardFabId = PlatformInfoProtocol->PlatInfo.BoardId;

  //
  // Get board name from OEM services otherwise we don't know this board name
  //
  UnicodeSPrint (StringBuffer, 0x100, L"Unknown");
  Status = OemSvcUpdateBoardIdString (
             BoardFabId,
             StringBuffer,
             0x100,
             AdvanceHiiHandle,
             STRING_TOKEN (STR_BOARD_ID_TEXT),
             MainHiiHandle
             );

  if (Status != EFI_SUCCESS) {
    BoardIdString = HiiSetString (MainHiiHandle, 0, StringBuffer, NULL);
    
    NewStringToHandle (
      AdvanceHiiHandle,
      STRING_TOKEN (STR_BOARD_ID_TEXT),
      MainHiiHandle,
      &BoardIdText
      );

    HiiCreateTextOpCode (OpCodeHandle, BoardIdText, 0, BoardIdString );
  }
//[-end-130613-IB05400415-modify]//

  return Status;
}


EFI_STATUS
GetFabFunc (
  IN     VOID                      *OpCodeHandle,
  IN     EFI_HII_HANDLE            MainHiiHandle,
  IN     EFI_HII_HANDLE            AdvanceHiiHandle,
  IN     CHAR16                    *StringBuffer
  )
{
  EFI_STATUS     Status = EFI_SUCCESS;
  STRING_REF     FabIdText;
  STRING_REF     FabIdString;
  UINT8          BoardFabId;
  UINT8          FabId = 0;

//[-start-121004-IB10370017-add]//
  PLATFORM_INFO_PROTOCOL              *PlatformInfoProtocol;
//[-end-121004-IB10370017-add]//
//[-start-121004-IB10370017-modify]//
  Status = gBS->LocateProtocol (&gEfiPlatformInfoProtocolGuid, NULL, (VOID **)&PlatformInfoProtocol);
  ASSERT_EFI_ERROR (Status);
  BoardFabId = PlatformInfoProtocol->PlatInfo.FabId;
//  GetBoradID (&BoardFabId);
//[-end-121004-IB10370017-modify]//

//[-start-121004-IB10370017-modify]//
  switch (BoardFabId) {
//[-end-121004-IB10370017-modify]//
    case CRB_BOARD_ID_FAB_ID_FAB2:
      FabId = FAB2_ID;
      break;

    case CRB_BOARD_ID_FAB_ID_FAB3:
      FabId = FAB3_ID;
      break;

    case CRB_BOARD_ID_FAB_ID_FAB4:
      FabId = FAB4_ID;
      break;

    default:
      break;
  }

  UnicodeSPrint (StringBuffer, 0x100, L"%x", FabId);
      
  FabIdString = HiiSetString (MainHiiHandle, 0, StringBuffer, NULL);
  
//[-start-121002-IB06460449-modify]//
//  StringBuffer = HiiGetString (AdvanceHiiHandle, STRING_TOKEN (STR_FAB_ID_TEXT), NULL);
//
//  FabIdText = HiiSetString (MainHiiHandle, 0, StringBuffer, NULL);
  NewStringToHandle (
    AdvanceHiiHandle,
    STRING_TOKEN (STR_FAB_ID_TEXT),
    MainHiiHandle,
    &FabIdText
    );
//[-end-121002-IB06460449-modify]//

  HiiCreateTextOpCode (OpCodeHandle, FabIdText, 0, FabIdString);

  return Status;
}
//[-end-120620-IB10540011-add]//
