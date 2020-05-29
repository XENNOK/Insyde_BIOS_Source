/** @file


;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/


#include "DynamicHotKeyDxe.h"
#include <ChipsetSetupConfig.h>
#include <Uefi/UefiBaseType.h>

#include <Include/Base.h>

#include <Library/HiiLib.h>
#include <Library/printlib.h>
#include <Library/FdSupportLib.h>
#include <Library/SpiAccessLib.h>

#include <Protocol/UgaDraw.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/StringTransform.h>

BOOLEAN
GetMeFwType (
  IN DYNAMIC_STRING      *Structure,
  OUT CHAR16             **StringData
  );

DYNAMIC_HOTKEY mHotKeyDefine[] = {
//
// KeyId      ScanCode    ShiftKey    AltKey    CtrlKey   Operation
//
  {KEYID0, F1_KEY,    0x0,     0x0,    0x0,    DEVICE_MANAGER_HOT_KEY},
  {KEYID1, F2_KEY,    0x0,     0x0,    0x0,    SETUP_HOT_KEY},
  {KEYID2, DEL_KEY,   0x0,     0x0,    0x0,    BOOT_MANAGER_HOT_KEY},
  {KEYID3, F10_KEY,   0x0,     0x0,    0x0,    BOOT_MAINTAIN_HOT_KEY},
  {KEYID4, ESC_KEY,   0x0,     0x0,    0x0,    FRONT_PAGE_HOT_KEY},
//[-start-140630-IB10301112-add]//
  {KEYID5, F3_KEY,    0x0,     0x0,    0x0,    FLASH_BOX_HOT_KEY}
//[-end-140630-IB10301112-add]//
};


DYNAMIC_STRING StringDefine[] = {
  {
    KEYID4,                                             ///< KeyId
    DSTRING_NOT_SYSTEM_INFO,                            ///< SystemInfoId
    STRING_LOCATION_MIN,                                ///< LocationX
    STRING_LOCATION_MAX,                                ///< LocationY
    {
      1,                                                ///< TextMode
      0,                                                ///< GraphicMode
      1,                                                ///< BeforePress
      0,                                                ///< AfterPress
      0                                                 ///< Reserved
    },
    {0xFF, 0xFF, 0xFF, 0},                              /// Foreground
    {0, 0, 0, 0},                                       /// Background
    STRING_TOKEN (STR_DYNAMIC_STR_ESC)                  ///< StringToken
  },
  {
    KEYID4,                                             ///< KeyId
    DSTRING_NOT_SYSTEM_INFO,                            ///< SystemInfoId
    STRING_LOCATION_MIN,                                ///< LocationX
    STRING_LOCATION_MAX,                                ///< LocationY
    {
      1,                                                ///< TextMode
      0,                                                ///< GraphicMode
      0,                                                ///< BeforePress
      1,                                                ///< AfterPress
      0                                                 ///< Reserved
    },
    {0xFF, 0xFF, 0xFF, 0},                              /// Foreground
    {0, 0, 0, 0},                                       /// Background
    STRING_TOKEN (STR_DYNAMIC_STR_ESC_SELECT)           ///< StringToken
   }, 
  {
    KEYID4,                                             ///< KeyId
    DSTRING_NOT_SYSTEM_INFO,                            ///< SystemInfoId
    STRING_LOCATION_CENTER,                             ///< LocationX
    STRING_LOCATION_CENTER,                             ///< LocationY
    {
      0,                                                ///< TextMode
      1,                                                ///< GraphicMode
      0,                                                ///< BeforePress
      1,                                                ///< AfterPress
      0                                                 ///< Reserved
    },
    {0xFF, 0xFF, 0xFF, 0},                              ///Foreground
    {0, 0, 0, 0},                                       /// Background    
    STRING_TOKEN (STR_DYNAMIC_STR_ESC_SELECT2)          ///< StringToken
  }
};

//
//  System information table
//
DYNAMIC_SYSTEM_INFO SystemInfoTable[] = {
  {BIOS_VERSION,      STRING_TOKEN (STR_DYNAMIC_STR_VERSION),    STRING_LOCATION_MIN, STRING_LOCATION_MIN},
  {BIOS_RELEASE_DATE, STRING_TOKEN (STR_DYNAMIC_STR_DATE),       STRING_LOCATION_MIN, STRING_LOCATION_MIN},
  {CPU_TYPE,          STRING_TOKEN (STR_DYNAMIC_STR_CPUTYPE),    STRING_LOCATION_MIN, STRING_LOCATION_MIN},
  {MEMORY_BUS_SPEED,  STRING_TOKEN (STR_DYNAMIC_STR_MEMSPEED),   STRING_LOCATION_MIN, STRING_LOCATION_MIN},
  {CPU_ID,            STRING_TOKEN (STR_DYNAMIC_STR_CPUID),      STRING_LOCATION_MIN, STRING_LOCATION_MIN}
};

//
//	Hot key operation table
//
DYNAMIC_OPERATION   OperationTable[] = {
  {NO_OPERATION,           STRING_TOKEN (STR_DYNAMIC_STR_NO_OPERATION)},
  {FRONT_PAGE_HOT_KEY,     STRING_TOKEN (STR_DYNAMIC_STR_FRONT_PAGE)},
  {SETUP_HOT_KEY,          STRING_TOKEN (STR_DYNAMIC_STR_SETUP)},
  {DEVICE_MANAGER_HOT_KEY, STRING_TOKEN (STR_DYNAMIC_STR_DEVICE_MANAGER)},
  {BOOT_MANAGER_HOT_KEY,   STRING_TOKEN (STR_DYNAMIC_STR_BOOT_MANAGER)},
  {BOOT_MAINTAIN_HOT_KEY,  STRING_TOKEN (STR_DYNAMIC_STR_BOOT_MAINTAINCE)},
  {SECURE_BOOT_HOT_KEY,    STRING_TOKEN (STR_DYNAMIC_STR_SECURE_BOOT)},
  {MEBX_HOT_KEY,           STRING_TOKEN (STR_DYNAMIC_STR_MEBX)},
  {REMOTE_ASST_HOT_KEY,    STRING_TOKEN (STR_DYNAMIC_STR_REMOTE_ASST)}
};

//
//  For variable editor.
//
DYNAMIC_HOTKEY_PACKAGE_HEAD  DynamicHotKeyInfo = {
  HOTKEY_INFO_SIGNATURE,
  1,  
  (UINT64)(UINTN)DynamicHotKeyDxeStrings,
  (UINT64)(UINTN)mHotKeyDefine,
  (UINT64)(UINTN)StringDefine,
  (UINT64)(UINTN)SystemInfoTable,
  (UINT64)(UINTN)OperationTable,
  (UINT8)GET_ARRAY_COUNT (mHotKeyDefine),
  (UINT8)GET_ARRAY_COUNT (StringDefine),
  (UINT8)GET_ARRAY_COUNT (SystemInfoTable),
  (UINT8)GET_ARRAY_COUNT (OperationTable),
  {0}
};

//
// Module variable
//
DYNAMIC_HOTKEY_PROTOCOL         mDynamicHotKey;
KERNEL_CONFIGURATION            *mSetupConfig = NULL;
KEY_ELEMENT                     *UpdatedHotKeyList = NULL;
DYNAMIC_HOTKEY_INTERNAL         *mHotKeyList = NULL;
DYNAMIC_STRING                  *mStringList = NULL;
UINTN                           mHotKeyListCount = 0;
UINTN                           mStringListCount = 0;
BOOLEAN                         mHotKeyPressed = FALSE;
UINT8                           mPressedKdyId = DSTRING_NO_HOTKEY;
EFI_HII_HANDLE                  mHotKeyStringPackHandle;
VOID                            *MultiConfigRegionBuffer = NULL;
BOOLEAN                         mIsPageType = FALSE;
UINTN                           mPageNum = 0;
HOTKEY_AND_STRING_TABLE         *mHotkeyStringTable = NULL;
EFI_EVENT                       mReadyToBootEvent = NULL;
VOID                            *mFEaddress = 0;


BOOLEAN
GetMeFwType (
  IN DYNAMIC_STRING      *Structure,
  OUT CHAR16             **StringData
  )
{
  EFI_STATUS                        Status;
  EFI_SETUP_UTILITY_PROTOCOL        *SetupUtility;
  CHIPSET_CONFIGURATION             *SetupVariable;

  *StringData = HiiGetPackageString (&gEfiCallerIdGuid, Structure->StringToken, NULL);

  Status = gBS->LocateProtocol ( &gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&SetupUtility );
  SetupVariable = ( CHIPSET_CONFIGURATION * )( SetupUtility->SetupNvData );

  if (SetupVariable->MeFwImageType == FW_IMAGE_TYPE_5MB) {
    return TRUE;
  }
  
  return FALSE;
}


VOID
BottomStringCount (
  IN     UINTN                        Operation,
  IN     UINT16                       Position,
  OUT    UINTN                        *Count
) 
{
  static UINTN                      LeftBootomStringCount;
  static UINTN                      CenterBootomStringCount;
  static UINTN                      RightBootomStringCount;
    
  switch (Operation) {

  case INITIAL_VALUE:
    LeftBootomStringCount = 0;
    CenterBootomStringCount = 0;
    RightBootomStringCount = 0;
    break;

  case SET_COUNT:
    if (Position == 0) {
      LeftBootomStringCount++;
    } else if (Position == (UINT16)-1) {
      CenterBootomStringCount++;
    } else if (Position == (UINT16)-2) {
      RightBootomStringCount++;
    }
    break;

  case GET_LEFT_COUNT:
    *Count = LeftBootomStringCount;
    break;

  case GET_CENTER_COUNT:
    *Count = CenterBootomStringCount;
    break;
    
  case GET_RIGHT_COUNT:
    *Count = RightBootomStringCount;
    break;

   default:
    break;
  }

}


VOID
FreeInternalResource (
  VOID
  )
{

  if (mHotKeyList != NULL) {
    FreePool (mHotKeyList);
  }

  if (mStringList != NULL) {
    FreePool (mStringList);
    mStringList = NULL;
  }
}

CHAR16 *
GetStringByStringToken (
  STRING_REF                                      StringToken
  )
{
  
  return HiiGetPackageString (&gEfiCallerIdGuid, StringToken, NULL);
  
}

BOOLEAN
IsExistHotkeySetting (
  UINTN                                           *AddrPtr                                             
)
{
  EFI_STATUS                                      Status;
  UINT32                                          Index;
  CONFIG_HEADER                                   *TempHeader;
  UINT16                                          ConfigOrder;
  VOID                                            *TempConfigData;
  UINTN                                           MultiConfigRegionBase;
  UINT64                                          Signature;
  UINT16                                          ConfigCount;
  BOOLEAN                                         IsFindSetting;
  CONFIG_HEADER                                   *TempConfigHeader;

  TempHeader = NULL;
  ConfigOrder = 0;
  TempConfigData = NULL;
  ConfigCount = 0;
  TempConfigHeader = NULL;
  //
  // Read from Flash Part
  //
  MultiConfigRegionBase = FLASH_REGION_MULTI_CONFIG_BASE;
  Status = FlashRead (
             (UINT8*)MultiConfigRegionBuffer,
             (UINT8*)MultiConfigRegionBase,
             (UINTN)FLASH_REGION_MULTI_CONFIG_SIZE
             );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  //
  // Verify Signature
  //
  Signature = MULTI_CONFIG_SIGNATURE;
  if (CompareMem (&Signature, MultiConfigRegionBuffer, sizeof (UINT64)) != 0) {
    return FALSE;
  }

  //
  // Check CRC32 for Multi Config Header
  //
  Status = CheckMultiConfigHeaderCrc32 (MultiConfigRegionBuffer);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  ConfigCount = GetConfigCount ();
  IsFindSetting = FALSE;
  for (Index = 0; Index < ConfigCount; Index++) {
    TempConfigHeader = GetConfigHeaderByOrder ((UINT16)Index);    
    Status = GetConfigHeaderOrder (
               MULTI_CONFIG_TYPE_HOTKEY_SETTING_VALUE, 
               TempConfigHeader->ConfigId, 
               MultiConfigRegionBuffer, 
               &TempHeader, 
               &ConfigOrder
               );
    //
    //  Check config data is active or not.
    //
    if ((Status == EFI_SUCCESS) && 
        (TempHeader->Type == MULTI_CONFIG_TYPE_HOTKEY_SETTING_VALUE) && 
        ((TempHeader->Attribute & MULTI_CONFIG_ATTRIBUTE_VALID) == MULTI_CONFIG_ATTRIBUTE_VALID) && 
        ((TempHeader->Attribute & MULTI_CONFIG_ATTRIBUTE_DEFAULT) == MULTI_CONFIG_ATTRIBUTE_DEFAULT)
        ) {
      IsFindSetting = TRUE;
      break;
    }
  }
  if (!IsFindSetting) {
    return FALSE;
  }

  //
  // Check CRC32 for Multi Config Data
  // Direct get Config Data by Config Data Offset
  //
  TempConfigData = (UINT8*)MultiConfigRegionBuffer + TempHeader->ConfigDataOffset;
  Status = CheckMultiConfigDataCrc32 (TempHeader->Crc32, TempHeader->ConfigDataSize, TempConfigData);
  if (EFI_ERROR (Status)) {
    //
    // Get Config Data by Config Order
    //
    TempConfigData = GetConfigDataByOrder (ConfigOrder, MultiConfigRegionBuffer);
    Status = CheckMultiConfigDataCrc32 (TempHeader->Crc32, TempHeader->ConfigDataSize, TempConfigData);
    if (EFI_ERROR (Status)) {
      return FALSE;
    }
  }

  *AddrPtr = (UINTN)TempConfigData;
  return TRUE;
}

BOOLEAN
IsExistMatchedHotkey (
  IN      DYNAMIC_HOTKEY         Hotkey,
  IN OUT  UINTN                  *IndexOfList
)
{
  UINTN                                           Index;
  
  for (Index = 0; Index < GET_ARRAY_COUNT (mHotKeyDefine); ++Index) { 
    //
    //  Check the Hot Key in Multi-Config data 
    //  region is matched with the default Hot Key or not.
    //
    if ((Hotkey.ScanCode == mHotKeyList[Index].HotKeyList.ScanCode) &&
        (Hotkey.AltKey   == mHotKeyList[Index].HotKeyList.AltKey) &&
        (Hotkey.ShiftKey == mHotKeyList[Index].HotKeyList.ShiftKey) &&
        (Hotkey.CtrlKey  == mHotKeyList[Index].HotKeyList.CtrlKey)) {
        
      *IndexOfList = Index;
      return TRUE;
    }
  }
  return FALSE;
}

BOOLEAN
IsExistMatchedSystemInfo (
  IN      DYNAMIC_STRING_EXTERNAL                 *SystemInfo,
  IN      UINTN                                   StringCount,
  IN OUT  UINTN                                   *IndexOfList
)
{
  UINTN                                           Index;
  
  for (Index = 0; Index < StringCount ; ++Index) {
    //
    //  Check the system information string in Multi-Config 
    //  data region is matched with the system information string or not.
    //
    if (SystemInfo->SystemInfoId == mStringList[Index].SystemInfoId) {
      *IndexOfList = Index;
      return TRUE;
    }
  }
  return FALSE;
}

VOID
ModifyStringList (
  IN UINT8                     KeyListId,
  IN DYNAMIC_HOTKEY            HotKey,
  IN DYNAMIC_STRING_EXTERNAL  *McStringPtr,
  IN UINTN                     McStringCount,
  IN OUT UINTN                *TotalStringCount
)
{
  UINTN                       McIndex;
  UINTN                       StrCmpIndex;
  CHAR16                      *UniStr;
  DYNAMIC_STRING_EXTERNAL     *DynamicStringPtr;

  UniStr = NULL;
  DynamicStringPtr = McStringPtr;
  //
  //Search string list (Multi-config side)
  //
  for (McIndex = 0; McIndex < McStringCount; McIndex++, DynamicStringPtr = GET_NEXT_DYNAMIC_STRING_PTR (DynamicStringPtr)) {
    //
    //Find string about assign key (Multi-config side)
    //
    if (DynamicStringPtr->KeyId == HotKey.KeyId) {
      //
      //Find the existed string to replace
      //
      for (StrCmpIndex = 0; StrCmpIndex < GET_ARRAY_COUNT (StringDefine); StrCmpIndex++) {
        //
        //Find it & replace it
        //
        if ((DynamicStringPtr->Flag.TextMode == mStringList[StrCmpIndex].Flag.TextMode) &&
            (DynamicStringPtr->Flag.GraphicMode == mStringList[StrCmpIndex].Flag.GraphicMode) &&
            (DynamicStringPtr->Flag.BeforePress == mStringList[StrCmpIndex].Flag.BeforePress) &&
            (DynamicStringPtr->Flag.AfterPress == mStringList[StrCmpIndex].Flag.AfterPress) &&
            (mStringList[StrCmpIndex].KeyId == KeyListId)) {

          mStringList[StrCmpIndex].LocationX = DynamicStringPtr->LocationX;
          mStringList[StrCmpIndex].LocationY = DynamicStringPtr->LocationY;
          CopyMem (&(mStringList[StrCmpIndex].Foreground), &(DynamicStringPtr->Foreground), sizeof (EFI_UGA_PIXEL));
          CopyMem (&(mStringList[StrCmpIndex].Background), &(DynamicStringPtr->Background), sizeof (EFI_UGA_PIXEL));
		  
          UniStr = (CHAR16 *)AllocateZeroPool (AsciiStrSize (&(DynamicStringPtr->String)));
          UniStr = AsciiStrToUnicodeStr (&(DynamicStringPtr->String), UniStr);
          HiiSetString (mHotKeyStringPackHandle, mStringList[StrCmpIndex].StringToken, UniStr, NULL);
          FREE (UniStr);
          break;
        }
      }

      if (StrCmpIndex < GET_ARRAY_COUNT (StringDefine)) {
        continue;         
      }
      mStringList[*TotalStringCount].KeyId        = KeyListId;
      mStringList[*TotalStringCount].SystemInfoId = DynamicStringPtr->SystemInfoId;
      mStringList[*TotalStringCount].LocationX    = DynamicStringPtr->LocationX;
      mStringList[*TotalStringCount].LocationY    = DynamicStringPtr->LocationY;
      mStringList[*TotalStringCount].Flag         = DynamicStringPtr->Flag;
      CopyMem (&(mStringList[*TotalStringCount].Foreground), &(DynamicStringPtr->Foreground), sizeof (EFI_UGA_PIXEL));
      CopyMem (&(mStringList[*TotalStringCount].Background), &(DynamicStringPtr->Background), sizeof (EFI_UGA_PIXEL));  
	      
      UniStr = (CHAR16 *)AllocateZeroPool (AsciiStrSize (&(DynamicStringPtr->String)));
      UniStr = AsciiStrToUnicodeStr (&(DynamicStringPtr->String), UniStr);
      mStringList[*TotalStringCount].StringToken = HiiSetString (mHotKeyStringPackHandle, 0, UniStr, NULL);
      FREE (UniStr);
      (*TotalStringCount)++;
    }
  }
  
}

VOID
UpdateList (
  IN DYNAMIC_HOTKEY            *McHotkeyPtr, 
  IN UINTN                     McHotkeyCount,
  IN DYNAMIC_STRING_EXTERNAL   *McStringPtr,
  IN UINTN                     McStringCount
)
{
  UINTN               ActualHotkeyCount;
  UINTN               ActualStringCount;
  UINTN               Index;
  UINTN               Num;
  CHAR16              *UniStr;

  ActualHotkeyCount = 0;
  ActualStringCount = 0;
  UniStr = NULL;
  
  if ((McHotkeyCount == 0) && (McStringCount == 0)) {
    return ;
  }

  ActualHotkeyCount = GET_ARRAY_COUNT (mHotKeyDefine);
  ActualStringCount = (GET_ARRAY_COUNT (StringDefine) + GET_ARRAY_COUNT (SystemInfoTable));

  //
  //Handle String with Hotkey 
  //
  for (Index = 0; Index < McHotkeyCount; Index++) {
    if (IsExistMatchedHotkey (McHotkeyPtr[Index], &Num)) {
      mHotKeyList[Num].HotKeyList.Operation = McHotkeyPtr[Index].Operation;
      ModifyStringList (mHotKeyList[Num].HotKeyList.KeyId, McHotkeyPtr[Index], McStringPtr, McStringCount, &ActualStringCount);
    } else {
      CopyMem ((UINT8 *)&mHotKeyList[ActualHotkeyCount].HotKeyList, (UINT8 *)&McHotkeyPtr[Index], sizeof (DYNAMIC_HOTKEY));
      mHotKeyList[ActualHotkeyCount].HotKeyList.KeyId |= 0x80;
      ModifyStringList (mHotKeyList[ActualHotkeyCount].HotKeyList.KeyId, McHotkeyPtr[Index], McStringPtr, McStringCount, &ActualStringCount);
      ActualHotkeyCount++;
    }
  }

  //
  //Handle String with No Hotkey 
  //
  for (Index = 0; Index < McStringCount; Index++, McStringPtr = GET_NEXT_DYNAMIC_STRING_PTR (McStringPtr)) {
    if ((McStringPtr->KeyId == DSTRING_NO_HOTKEY) && (McStringPtr->SystemInfoId == DSTRING_NOT_SYSTEM_INFO)) {
        mStringList[ActualStringCount].KeyId       = McStringPtr->KeyId;
        mStringList[ActualStringCount].SystemInfoId= McStringPtr->SystemInfoId;
        mStringList[ActualStringCount].LocationX   = McStringPtr->LocationX;
        mStringList[ActualStringCount].LocationY   = McStringPtr->LocationY;
        mStringList[ActualStringCount].Flag        = McStringPtr->Flag;
        CopyMem (&(mStringList[ActualStringCount].Foreground), &(McStringPtr->Foreground), sizeof (EFI_UGA_PIXEL));
        CopyMem (&(mStringList[ActualStringCount].Background), &(McStringPtr->Background), sizeof (EFI_UGA_PIXEL));
        
        UniStr = (CHAR16 *)AllocateZeroPool (AsciiStrSize (&(McStringPtr->String)));
        UniStr = AsciiStrToUnicodeStr (&(McStringPtr->String), UniStr);
        mStringList[ActualStringCount].StringToken = HiiSetString (mHotKeyStringPackHandle, 0, UniStr, NULL);
        FREE (UniStr);
        ActualStringCount++;
    } else if ((McStringPtr->KeyId == DSTRING_NO_HOTKEY) && (McStringPtr->SystemInfoId != DSTRING_NOT_SYSTEM_INFO)) {
      if (IsExistMatchedSystemInfo (McStringPtr, ActualStringCount, &Num)) {
        mStringList[Num].KeyId        = McStringPtr->KeyId;
        mStringList[Num].SystemInfoId = McStringPtr->SystemInfoId;
        mStringList[Num].LocationX    = McStringPtr->LocationX;
        mStringList[Num].LocationY    = McStringPtr->LocationY;
        mStringList[Num].Flag         = McStringPtr->Flag;
        UniStr = (CHAR16 *)AllocateZeroPool (AsciiStrSize (&(McStringPtr->String)));
        UniStr = AsciiStrToUnicodeStr (&(McStringPtr->String), UniStr);
        mStringList[Num].StringToken = HiiSetString (mHotKeyStringPackHandle, 0, UniStr, NULL);
        FREE (UniStr);
      } else {
        mStringList[ActualStringCount].KeyId       = McStringPtr->KeyId;
        mStringList[ActualStringCount].SystemInfoId= McStringPtr->SystemInfoId;
        mStringList[ActualStringCount].LocationX   = McStringPtr->LocationX;
        mStringList[ActualStringCount].LocationY   = McStringPtr->LocationY;
        mStringList[ActualStringCount].Flag        = McStringPtr->Flag;
        CopyMem (&(mStringList[ActualStringCount].Foreground), &(McStringPtr->Foreground), sizeof (EFI_UGA_PIXEL));
        CopyMem (&(mStringList[ActualStringCount].Background), &(McStringPtr->Background), sizeof (EFI_UGA_PIXEL));
        
        UniStr = (CHAR16 *)AllocateZeroPool (AsciiStrSize (&(McStringPtr->String)));
        UniStr = AsciiStrToUnicodeStr (&(McStringPtr->String), UniStr);
        mStringList[ActualStringCount].StringToken = HiiSetString (mHotKeyStringPackHandle, 0, UniStr, NULL);
        FREE (UniStr);
        ActualStringCount++;
      }
    }
  }
  
  mHotKeyListCount = ActualHotkeyCount;
  mStringListCount = ActualStringCount;
}

EFI_STATUS 
GetHotKeyAndString (
  VOID
  ) 
{
  UINTN                      Index;
  UINTN                      BaseIndex;
  UINT8                      *AddrPtr;
  UINTN                      MCAddr;
  UINTN                      McHotkeyCount;
  UINTN                      McStringCount;
  DYNAMIC_HOTKEY             *McHotkeyPtr;
  DYNAMIC_STRING_EXTERNAL    *McStringPtr;
  UINTN                      HotkeyCountOri;
  UINTN                      StringCountOri; 
  EFI_STATUS                 Status;

  AddrPtr = NULL;
  McHotkeyCount = 0; 
  McStringCount = 0; 
  McHotkeyPtr = NULL;
  McStringPtr = NULL;
  HotkeyCountOri = 0; 
  StringCountOri = 0;  
  
  //
  // 1. Get Multi-Config Address
  // 2. allocate space for mHotKeyList and mStringList
  //
  MultiConfigRegionBuffer = AllocateZeroPool (FLASH_REGION_MULTI_CONFIG_SIZE);
  if (IsExistHotkeySetting (&MCAddr)) {
    AddrPtr = (UINT8*)MCAddr;
    McHotkeyCount = (UINTN)(*AddrPtr);
    mHotKeyListCount += McHotkeyCount;
    ++AddrPtr;
    McStringCount = (UINTN)(*AddrPtr);
    mStringListCount += McStringCount;
    ++AddrPtr;
    if (McHotkeyCount != 0) {
      McHotkeyPtr = (DYNAMIC_HOTKEY *)AddrPtr;
    } else {
      McHotkeyPtr = NULL;
    }
    McStringPtr = (DYNAMIC_STRING_EXTERNAL *)((UINTN)AddrPtr + (sizeof (DYNAMIC_HOTKEY) * McHotkeyCount));
  } else {
    FREE (MultiConfigRegionBuffer);
  }

  //
  // Add Const HotKey
  //
  mHotKeyListCount += GET_ARRAY_COUNT (mHotKeyDefine);
  if (mHotKeyListCount != 0) {
    mHotKeyList = (DYNAMIC_HOTKEY_INTERNAL *)AllocateZeroPool (sizeof (DYNAMIC_HOTKEY_INTERNAL) * mHotKeyListCount);
    if (mHotKeyList == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto GetHotKeyAndStringEXIT;
    }
  }

  //
  // Add Const String
  //
  mStringListCount += (GET_ARRAY_COUNT (StringDefine) + GET_ARRAY_COUNT (SystemInfoTable));
  if (mStringListCount != 0) {
    mStringList = (DYNAMIC_STRING *)AllocateZeroPool (sizeof (DYNAMIC_STRING) * mStringListCount);
    if (mStringList == NULL) {
      FreePool (mHotKeyList);
      Status = EFI_OUT_OF_RESOURCES;
      goto GetHotKeyAndStringEXIT;
    }
  }

  //
  // 3. copy Bios HotKey to mHotKeyList and mStringList
  //

  for (Index = 0; Index < GET_ARRAY_COUNT (mHotKeyDefine); ++Index) {
    CopyMem (&mHotKeyList[Index].HotKeyList, &mHotKeyDefine[Index], sizeof (DYNAMIC_HOTKEY));
  }

  for (Index = 0; Index < GET_ARRAY_COUNT (StringDefine); ++Index) {
    mStringList[Index].KeyId = StringDefine[Index].KeyId;
    mStringList[Index].SystemInfoId = StringDefine[Index].SystemInfoId;
    mStringList[Index].LocationX = StringDefine[Index].LocationX;
    mStringList[Index].LocationY = StringDefine[Index].LocationY;
    mStringList[Index].Flag = StringDefine[Index].Flag;
    mStringList[Index].StringToken = StringDefine[Index].StringToken;
    CopyMem (&(mStringList[Index].Foreground), &(StringDefine[Index].Foreground), sizeof (EFI_UGA_PIXEL));
    CopyMem (&(mStringList[Index].Background), &(StringDefine[Index].Background), sizeof (EFI_UGA_PIXEL));
  }
  
  BaseIndex = Index;
  //
  //Copy system info to string list.
  //
  for (Index = 0; Index < GET_ARRAY_COUNT (SystemInfoTable); ++Index) {
    //
    //Because memory is ZeroPool, so Flag is all zero.(we can don't care it) 
    //
    mStringList[BaseIndex + Index].KeyId = DSTRING_NO_HOTKEY;
    mStringList[BaseIndex + Index].SystemInfoId = (UINT8)SystemInfoTable[Index].Type;
    mStringList[BaseIndex + Index].LocationX = SystemInfoTable[Index].LocationX;
    mStringList[BaseIndex + Index].LocationY = SystemInfoTable[Index].LocationY;
    mStringList[BaseIndex + Index].StringToken = SystemInfoTable[Index].StringToken;
  }
  
  HotkeyCountOri = mHotKeyListCount;
  StringCountOri = mStringListCount;
  //
  // 4. Update Dynamic HotKey to list, and recalculate List count.
  //
  UpdateList (
    McHotkeyPtr, 
    McHotkeyCount,
    McStringPtr,
    McStringCount
    );
  
  if (HotkeyCountOri != mHotKeyListCount) {
    mHotKeyList = (DYNAMIC_HOTKEY_INTERNAL *) ReallocatePool (
                                                HotkeyCountOri * sizeof (DYNAMIC_HOTKEY_INTERNAL),
                                                mHotKeyListCount * sizeof (DYNAMIC_HOTKEY_INTERNAL),
                                                mHotKeyList
                                                );
    if (mHotKeyList == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto GetHotKeyAndStringEXIT;
    }
  }
  
  if (StringCountOri != mStringListCount) {
    mStringList = (DYNAMIC_STRING *) ReallocatePool (
                                       StringCountOri * sizeof (DYNAMIC_STRING),
                                       mStringListCount * sizeof (DYNAMIC_STRING),
                                       mStringList
                                       );
    if (mStringList == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto GetHotKeyAndStringEXIT;
    }
  }
  
  Status = EFI_SUCCESS;
  
GetHotKeyAndStringEXIT:
  
  FREE (MultiConfigRegionBuffer);
  return Status;
}



EFI_STATUS
EFIAPI
GetDynamicHotKeyList (
  IN     DYNAMIC_HOTKEY_PROTOCOL    *This,
  IN OUT KEY_ELEMENT                **HotKeyList
  )
{

  KEY_ELEMENT    *OrigHotKeyList;
  UINTN          OrigHotKeyListCount;
  UINTN          Index;
  UINTN          BitIndex;
  BOOLEAN        Found;

  //
  // If there is no dynamic hot key list, do not change anything
  //
  if (mHotKeyListCount == 0) {
    return EFI_SUCCESS;
  }
  
  //
  // If we already updated hot key list, just return it
  //
  if (UpdatedHotKeyList != NULL) {
    *HotKeyList = UpdatedHotKeyList;
    return EFI_SUCCESS;
  }

  //
  // Calculate original hot key list count
  //
  OrigHotKeyList = *HotKeyList;
  OrigHotKeyListCount = 0;
  for (Index = 0; OrigHotKeyList[Index].ScanCode != 0x00; ++Index) {
    ++OrigHotKeyListCount;
  }
  ++OrigHotKeyListCount;

  //
  // Calculate max size and copy from original list
  //
  UpdatedHotKeyList = (KEY_ELEMENT *)AllocateZeroPool ((OrigHotKeyListCount + mHotKeyListCount) * sizeof (KEY_ELEMENT));
  if (UpdatedHotKeyList == NULL) {
    return EFI_UNSUPPORTED;
  }

  CopyMem (UpdatedHotKeyList, OrigHotKeyList, OrigHotKeyListCount * sizeof (KEY_ELEMENT));

  //
  // Add dynamic hot key list to newly created one
  //
  for (Index = 0; Index < mHotKeyListCount; ++Index) {

    Found = FALSE;
    for (BitIndex = 0; UpdatedHotKeyList[BitIndex].ScanCode != 0x00; ++BitIndex) {
      //
      // Compare each dynamic hot key to check if this hot key already existed or not
      // If it exist, we will not add another one but we will use dynamic hot key setting
      // to override original one
      //
      if (UpdatedHotKeyList[BitIndex].ScanCode == mHotKeyList[Index].HotKeyList.ScanCode &&
          UpdatedHotKeyList[BitIndex].Keyattribute.AltKey == mHotKeyList[Index].HotKeyList.AltKey &&
          UpdatedHotKeyList[BitIndex].Keyattribute.ShiftKey == mHotKeyList[Index].HotKeyList.ShiftKey &&
          UpdatedHotKeyList[BitIndex].Keyattribute.CtrlKey == mHotKeyList[Index].HotKeyList.CtrlKey) {
        mHotKeyList[Index].BitIndex = (UINT8)BitIndex;
        Found = TRUE;
        break;
      }
    }

    if (Found) {
      continue;
    }

    //
    // Add dynamic hot key list
    //
    UpdatedHotKeyList[BitIndex].ScanCode = mHotKeyList[Index].HotKeyList.ScanCode;
    UpdatedHotKeyList[BitIndex].Keyattribute.AltKey = mHotKeyList[Index].HotKeyList.AltKey;
    UpdatedHotKeyList[BitIndex].Keyattribute.ShiftKey = mHotKeyList[Index].HotKeyList.ShiftKey;
    UpdatedHotKeyList[BitIndex].Keyattribute.CtrlKey = mHotKeyList[Index].HotKeyList.CtrlKey;
    mHotKeyList[Index].BitIndex = (UINT8)BitIndex;
  }

  *HotKeyList = UpdatedHotKeyList;

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
GetDynamicHotKeyOperation (
  IN     DYNAMIC_HOTKEY_PROTOCOL    *This,
  IN     UINT32                     KeyDetected,
  OUT    UINTN                      *Operation
  )
{
  UINTN        Index;

  for (Index = 0; Index < mHotKeyListCount; ++Index) {
    //
    // Check bit to know which dynamic hot key is pressed
    //
    if (KeyDetected & (1 << mHotKeyList[Index].BitIndex)) {
      mHotKeyPressed = TRUE;
      mPressedKdyId = mHotKeyList[Index].HotKeyList.KeyId;
      *Operation = (UINTN)mHotKeyList[Index].HotKeyList.Operation;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
  
}

UINTN
GetDynamicStringCountInternal (
  IN     BOOLEAN                KeyPressed,
  OUT    UINTN                  *SelectedIndex OPTIONAL
  )
{
  UINTN                          Index;
  UINTN                          Count;
  UINT16                         Mask;
  EFI_STATUS                     Status;
  EFI_SETUP_UTILITY_PROTOCOL     *SetupUtility;
  DYNAMIC_STRING_FLAG            TempFlag;
  UINT16                         Temp;

  //
  // Get setup config to know QuietBoot
  //
  if (mSetupConfig == NULL) {
    Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&SetupUtility);
    if (!EFI_ERROR (Status)) {
      mSetupConfig = (KERNEL_CONFIGURATION *)SetupUtility->SetupNvData;
    } else {
      return 0;
    }
  }

  //
  // Use a Mask to know which string is we want
  //
  ZeroMem (&TempFlag, sizeof (DYNAMIC_STRING_FLAG));
  if (mSetupConfig->QuietBoot) {
    TempFlag.GraphicMode = 1;
  } else {
    TempFlag.TextMode = 1;
  }
  Mask = *(UINT16 *)&TempFlag;


  BottomStringCount (INITIAL_VALUE, 0, NULL);
  
  //
  // Find matched string and count it
  //
  Count = 0;
  for (Index = 0; Index < mStringListCount; ++Index) {
    Temp = *(UINT16 *)&mStringList[Index].Flag;
    //
    // Basic check mode
    //
    if (((Mask & Temp) == Mask) || (mStringList[Index].SystemInfoId != DSTRING_NOT_SYSTEM_INFO)) {
      //
      // If a string does not bond any hot key, always show it
      //
      if (mStringList[Index].KeyId == DSTRING_NO_HOTKEY) {
        if (SelectedIndex != NULL) {
          SelectedIndex[Count] = Index;
        }
        ++Count;
        if (mStringList[Index].LocationY == (UINT16)(-2)) {
          BottomStringCount (SET_COUNT, mStringList[Index].LocationX, NULL);
        }
        continue;
      }
        
      //
      // If a string bond a specified hot key, do advance check
      //
      if (KeyPressed == TRUE) {        
        if ((mStringList[Index].KeyId == mPressedKdyId) && mStringList[Index].Flag.AfterPress) {
          //
          // If key match, show string with AfterPress flag
          //
          if (SelectedIndex != NULL) {
            SelectedIndex[Count] = Index;
          }
          ++Count;
          if (mStringList[Index].LocationY == (UINT16)(-2)) {
            BottomStringCount (SET_COUNT, mStringList[Index].LocationX, NULL);
          }
        } else if ((mStringList[Index].KeyId != mPressedKdyId) && mStringList[Index].Flag.BeforePress) {
          //
          // If key does not match, show original string (with BeforePress)
          //
          if (SelectedIndex != NULL) {
            SelectedIndex[Count] = Index;
          }
          ++Count;
          if (mStringList[Index].LocationY == (UINT16)(-2)) {
            BottomStringCount (SET_COUNT, mStringList[Index].LocationX, NULL);
          }
        }
      } else {
        if (mStringList[Index].Flag.BeforePress) {
          //
          // Since there is no hot key pressed, show string with BeforePress
          //
          if (SelectedIndex != NULL) {
            SelectedIndex[Count] = Index;
          }
          ++Count;
          if (mStringList[Index].LocationY == (UINT16)(-2)) {
            BottomStringCount (SET_COUNT, mStringList[Index].LocationX, NULL);
          }
        }
      
      }
    }
  }

  return Count;
  
}

EFI_STATUS
EFIAPI
GetDynamicStringCount (
  IN     DYNAMIC_HOTKEY_PROTOCOL    *This,
  IN     BOOLEAN                    KeyPressed,
  OUT    UINTN                      *StringCount
  )
{
  
  UINTN                 Count;


  Count = GetDynamicStringCountInternal (KeyPressed, NULL);
  if (Count == 0) {
    return EFI_NOT_FOUND;
  }

  *StringCount = (UINTN)Count;

  return EFI_SUCCESS;
  
}
  
EFI_STATUS
EFIAPI
GetDynamicString (
  IN     DYNAMIC_HOTKEY_PROTOCOL    *This,
  IN     UINTN                      Index,
  IN     BOOLEAN                    KeyPressed,
  OUT    CHAR16                     **String,
  OUT    UINTN                      *CoordinateX,
  OUT    UINTN                      *CoordinateY
  )
{
  UINTN                 Count;
  UINTN                 StringIndex;
  UINTN                 SelectedIndex[0xFF];
  

  //
  // Get count first
  //
  Count = GetDynamicStringCountInternal (KeyPressed, SelectedIndex);
  if (Count == 0) {
    return EFI_NOT_FOUND;
  }

  //
  // Check selected range
  //
  if (Index >= Count) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Find specified string by Index and matched with flag
  //
  StringIndex = SelectedIndex[Index];

  //
  //System Information DO NOT display at this moment.
  //It will show in BootSystemInformation.c
  //
  if(mStringList[StringIndex].SystemInfoId != DSTRING_NOT_SYSTEM_INFO) {
    return EFI_ABORTED;
  }
  
  *String = GetStringByStringToken (mStringList[StringIndex].StringToken);    
  if (*String == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (mStringList[StringIndex].LocationX == ((UINT16)-1)) {
    *CoordinateX = STRING_LOCATION_CENTER;
  } else if (mStringList[StringIndex].LocationX == ((UINT16)-2)) {
    *CoordinateX = STRING_LOCATION_MAX;
  } else {
    *CoordinateX = (UINTN)mStringList[StringIndex].LocationX;
  }
  
  if (mStringList[StringIndex].LocationY == ((UINT16)-1)) {
    *CoordinateY = STRING_LOCATION_CENTER;
  } else if (mStringList[StringIndex].LocationY == ((UINT16)-2)) {
    *CoordinateY = STRING_LOCATION_MAX;
  } else {
    *CoordinateY = (UINTN)mStringList[StringIndex].LocationY;
  }
  
  return EFI_SUCCESS;
  
}

EFI_STATUS
EFIAPI
GetDynamicStringColor (
  IN     UINTN                      Index,
  IN     BOOLEAN                    KeyPressed,
  OUT    EFI_UGA_PIXEL              *Foreground,
  OUT    EFI_UGA_PIXEL              *Background    
  )
{  
  UINTN                 StringIndex;
  UINTN                 SelectedIndex[0xFF];
  
  //
  // Get SelectedIndex
  //
  GetDynamicStringCountInternal (KeyPressed, SelectedIndex);

  //
  // Find specified string by Index and matched with flag
  //
  StringIndex = SelectedIndex[Index];

  CopyMem (Foreground, &(mStringList[StringIndex].Foreground), sizeof (EFI_UGA_PIXEL));
  CopyMem (Background, &(mStringList[StringIndex].Background), sizeof (EFI_UGA_PIXEL));
  return EFI_SUCCESS;
}

BOOLEAN
EFIAPI
IsDynamicHotKeyPressed (
  IN     DYNAMIC_HOTKEY_PROTOCOL    *This
  )
{
  return mHotKeyPressed;
  
}


EFI_STATUS
EFIAPI 
ShowSystemInformation (
  IN OUT UINT8                      *PrintLine
  )
{
  EFI_STATUS                                      Status;
  UINTN                                           Index;
  CHAR16                                          *String;
  UINTN                                           MaxX;
  UINTN                                           MaxY;
  UINTN                                           LocX;
  UINTN                                           LocY;
  CHAR16                                          *StrPtr;
  STRING_TRANSFORM_PROTOCOL                       *mStringTransform;
  BOOLEAN                                         IsTransformed;
  
  String = NULL;
  StrPtr = NULL;
  IsTransformed = FALSE;
  
  Status = gBS->LocateProtocol (&gStringTransformProtocolGuid, NULL, (VOID **)&mStringTransform);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }
  
  Status = gST->ConOut->QueryMode (
                          gST->ConOut,
                          gST->ConOut->Mode->Mode,
                          &MaxX,
                          &MaxY
                          );

  for (Index = 0; Index < mStringListCount; Index++) {
    if ((mStringList[Index].KeyId == DSTRING_NO_HOTKEY) && (mStringList[Index].SystemInfoId != DSTRING_NOT_SYSTEM_INFO)) {
      String = GetStringByStringToken (mStringList[Index].StringToken);
      if (String == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      
      IsTransformed = FALSE;
      
      Status = mStringTransform->Transform (mStringTransform, String, &StrPtr);
      //
      //Because mStringTransform->Transform replace one keyword variable at one time.
      //So we try to transform another system info keyword.
      //
      while (Status == EFI_SUCCESS) {
        IsTransformed = TRUE;
        String = StrPtr;
        Status = mStringTransform->Transform (mStringTransform, String, &StrPtr);
        if (Status == EFI_SUCCESS) {
          FreePool (String);
          String = NULL;
        }
      }
      
      if (mStringList[Index].LocationX == (UINT16)-2) {
        LocX = (UINTN)-2;
      } else if (mStringList[Index].LocationX == (UINT16)-1){
        LocX = (UINTN)-1;
      } else {
        LocX = mStringList[Index].LocationX;
      }
	
      if (mStringList[Index].LocationY == (UINT16)-2) {
        LocY = (UINTN)-2;
      } else if (mStringList[Index].LocationY == (UINT16)-1){
        LocY = (UINTN)-1;
      } else {
        LocY = mStringList[Index].LocationY;
      }
      mDynamicHotKey.AdjustStringPosition (&mDynamicHotKey, FALSE, FALSE, (MaxY / 2), StrPtr, &LocX, &LocY);
      
      gST->ConOut->SetCursorPosition (gST->ConOut, LocX, LocY);
      //
      // For debug
      //
      gST->ConOut->EnableCursor (gST->ConOut, TRUE);

      gST->ConOut->OutputString (gST->ConOut, StrPtr);
      (*PrintLine)++;
      //
      //mStringTransform->Transform will allocate memory for new string,
      //So we need to free it.
      //
      if (IsTransformed) {
        FreePool (StrPtr);
        StrPtr = NULL;
      }
      Status = EFI_SUCCESS;
    }
  }

  return Status;
}

EFI_STATUS
EFIAPI
AdjustStringPosition (
  IN     DYNAMIC_HOTKEY_PROTOCOL    *This,
  IN     BOOLEAN                    IsClearCountMode,
  IN     BOOLEAN                    IsGraphicMode,
  IN     UINTN                      StrDestCenterY,
  IN     CHAR16                     *TmpStr,
  IN OUT UINTN                      *CoordinateX,
  IN OUT UINTN                      *CoordinateY
 ) 
{
  EFI_STATUS                            Status;
  static EFI_UGA_DRAW_PROTOCOL          *UgaDraw;
  static EFI_GRAPHICS_OUTPUT_PROTOCOL   *GraphicsOutput;
  static UINTN                          SizeOfX;
  static UINTN                          SizeOfY;
  UINT32                                ColorDepth;
  UINT32                                RefreshRate;
  static UINTN                          LeftTopCount;
  static UINTN                          CenterTopCount;
  static UINTN                          RightTopCount;
  static UINTN                          LeftCenterCount;
  static UINTN                          CenterCount;
  static UINTN                          RightCenterCount;
  static UINTN                          LeftBottomCount;
  static UINTN                          CenterBottomCount;
  static UINTN                          RightBottomCount;
  UINTN                                 Count;

  Status = EFI_SUCCESS;  
  Count = 0;
  
  if (IsClearCountMode) {
    LeftTopCount = 0;
    CenterTopCount = 0;
    RightTopCount = 0;
    LeftCenterCount = 0;
    CenterCount = 0;
    RightCenterCount = 0;
    LeftBottomCount = 0;
    CenterBottomCount = 0;
    RightBottomCount = 0;
    return EFI_SUCCESS;
  }

  if ((CoordinateX == NULL) || (CoordinateY == NULL)) {
    return EFI_INVALID_PARAMETER;    
  }
  
  if (((INTN)*CoordinateX > 0) && ((INTN)*CoordinateY > 0)) {
    return EFI_SUCCESS;    
  }

  
  if ((SizeOfX == 0) && (SizeOfY == 0)) {
    //
    // Get Screen size of x and y
    //
    
    if (IsGraphicMode) {

      if ((GraphicsOutput == NULL) && (UgaDraw == NULL)) {
        Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, (VOID **)&GraphicsOutput);
        if (EFI_ERROR (Status)) {
          GraphicsOutput = NULL;
          Status = gBS->HandleProtocol (
                          gST->ConsoleOutHandle,
                          &gEfiUgaDrawProtocolGuid,
                          (VOID **)&UgaDraw
                          );
          if (EFI_ERROR (Status)) {
            return EFI_UNSUPPORTED;
          }
        }
      }
      
      if (GraphicsOutput == NULL && UgaDraw == NULL) {
        return EFI_UNSUPPORTED;
      }

   
      if (GraphicsOutput != NULL) {
        SizeOfX = (UINTN)GraphicsOutput->Mode->Info->HorizontalResolution;
        SizeOfY = (UINTN)GraphicsOutput->Mode->Info->VerticalResolution;
      } else {
        Status = UgaDraw->GetMode (
                            UgaDraw,
                            &((UINT32)SizeOfX),
                            &((UINT32)SizeOfY),
                            &ColorDepth,
                            &RefreshRate
                            );
        if (EFI_ERROR (Status)) {
          return EFI_UNSUPPORTED;
        }
      }
    
    } else {
      Status = gST->ConOut->QueryMode (
                              gST->ConOut,
                              gST->ConOut->Mode->Mode,
                              &SizeOfX,
                              &SizeOfY
                              );
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
  }

  //
  // Set Location
  //
  if ((*CoordinateX == STRING_LOCATION_MIN) && (*CoordinateY == STRING_LOCATION_MIN)) {
    //
    // Left and Top
    //
    *CoordinateX = 0;
    *CoordinateY = IsGraphicMode ? (0 + LeftTopCount * EFI_GLYPH_HEIGHT) : (0 + LeftTopCount);
    LeftTopCount++;
    
  } else if ((*CoordinateX == STRING_LOCATION_CENTER) && (*CoordinateY == STRING_LOCATION_MIN)) {
    //
    // Center and Top
    //
    *CoordinateX = IsGraphicMode ? ((SizeOfX - StrLen (TmpStr) * EFI_GLYPH_WIDTH) / 2) : ((SizeOfX - StrLen (TmpStr)) / 2);
    *CoordinateY = IsGraphicMode ? (0 + CenterTopCount * EFI_GLYPH_HEIGHT) : (0 + CenterTopCount);
    CenterTopCount++;
    
  } else if ((*CoordinateX == STRING_LOCATION_MAX) && (*CoordinateY == STRING_LOCATION_MIN)) {
    //
    // Right and Top
    //
    *CoordinateX = IsGraphicMode ? ((SizeOfX - (StrLen (TmpStr) + 1) * EFI_GLYPH_WIDTH)) : ((SizeOfX - (StrLen (TmpStr) + 1)));
    *CoordinateY = IsGraphicMode ? (0 + RightTopCount * EFI_GLYPH_HEIGHT) : (0 + RightTopCount);
    RightTopCount++;
    
  } else if ((*CoordinateX == STRING_LOCATION_MIN) && (*CoordinateY == STRING_LOCATION_CENTER)) {
    //
    // Left and Center
    //
    *CoordinateX = 0;
    *CoordinateY = IsGraphicMode ? (StrDestCenterY + LeftCenterCount * EFI_GLYPH_HEIGHT) : (StrDestCenterY + LeftCenterCount);
    LeftCenterCount++;
    
  } else if ((*CoordinateX == STRING_LOCATION_CENTER) && (*CoordinateY == STRING_LOCATION_CENTER)) {
    //
    // Center
    //
    *CoordinateX = IsGraphicMode ? ((SizeOfX - StrLen (TmpStr) * EFI_GLYPH_WIDTH) / 2) : ((SizeOfX - StrLen (TmpStr)) / 2);
    *CoordinateY = IsGraphicMode ? (StrDestCenterY + CenterCount * EFI_GLYPH_HEIGHT) : (StrDestCenterY + CenterCount);
    CenterCount++;
    
  } else if ((*CoordinateX == STRING_LOCATION_MAX) && (*CoordinateY == STRING_LOCATION_CENTER)) {
    //
    // Right and Center
    //
    *CoordinateX = IsGraphicMode ? ((SizeOfX - (StrLen (TmpStr) + 1) * EFI_GLYPH_WIDTH)) : ((SizeOfX - (StrLen (TmpStr) + 1)));
    *CoordinateY = IsGraphicMode ? (StrDestCenterY + RightCenterCount * EFI_GLYPH_HEIGHT) : (StrDestCenterY + RightCenterCount);
    RightCenterCount++;
    
  } else if ((*CoordinateX == STRING_LOCATION_MIN) && (*CoordinateY == STRING_LOCATION_MAX)) {
    //
    // Left and Bottom
    //
    BottomStringCount (GET_LEFT_COUNT, 0, &Count);
    *CoordinateX = 0;
    *CoordinateY = IsGraphicMode ? (SizeOfY - (Count - LeftBottomCount) * EFI_GLYPH_HEIGHT) : (SizeOfY - (Count - LeftBottomCount));
    LeftBottomCount++;
    
    
  } else if ((*CoordinateX == STRING_LOCATION_CENTER) && (*CoordinateY == STRING_LOCATION_MAX)) {
    //
    // Center and Bottom
    //
    BottomStringCount (GET_CENTER_COUNT, 0, &Count);
    *CoordinateX = IsGraphicMode ? ((SizeOfX - StrLen (TmpStr) * EFI_GLYPH_WIDTH) / 2) : ((SizeOfX - StrLen (TmpStr)) / 2);
    *CoordinateY = IsGraphicMode ? (SizeOfY - (Count - CenterBottomCount) * EFI_GLYPH_HEIGHT) : (SizeOfY - (Count - CenterBottomCount));
    CenterBottomCount++;
    
  } else if ((*CoordinateX == STRING_LOCATION_MAX) && (*CoordinateY == STRING_LOCATION_MAX)) {
    //
    // Right and Bottom
    //
    BottomStringCount (GET_RIGHT_COUNT, 0, &Count);
    *CoordinateX = IsGraphicMode ? ((SizeOfX - (StrLen (TmpStr) + 1) * EFI_GLYPH_WIDTH)) : ((SizeOfX - (StrLen (TmpStr) + 1)));
    *CoordinateY = IsGraphicMode ? (SizeOfY - (Count - RightBottomCount) * EFI_GLYPH_HEIGHT) : (SizeOfY - (Count - RightBottomCount));
    RightBottomCount++;
    
  } else {
    switch (*CoordinateX) {

    case STRING_LOCATION_MIN:
      *CoordinateX = 0;
      break;

    case STRING_LOCATION_CENTER:
      *CoordinateX = IsGraphicMode ? ((SizeOfX - StrLen (TmpStr) * EFI_GLYPH_WIDTH) / 2) : ((SizeOfX - StrLen (TmpStr)) / 2);
      break;

    case STRING_LOCATION_MAX:
      *CoordinateX = IsGraphicMode ? ((SizeOfX - (StrLen (TmpStr) + 1) * EFI_GLYPH_WIDTH)) : ((SizeOfX - (StrLen (TmpStr) + 1)));
      break;

    default:
      break;
    }

    switch (*CoordinateY) {

    case STRING_LOCATION_MIN:
      *CoordinateY = 0;
      break;

    case STRING_LOCATION_CENTER:
      *CoordinateY = StrDestCenterY;
      break;

    case STRING_LOCATION_MAX:
      *CoordinateY = (SizeOfY - 1);
      break;

    default:
      break;
    }
  }

  return EFI_SUCCESS;
  
}

/**
  Get all list size.
	
  @param[in]  *TotalSize                         the size to filll
  @param[in]  *HotkeyCount                     the number of hot key 
  @param[in]  *StringCount                      the number of string
  @param[in]  *SystemInfoCount               the number of system information
  @param[in]  *OperationCount                 the number of hot key operation
	
  @retval       EFI_SUCCESS	                    Get all list size success.
  @retval       EFI_INVALID_PARAMETER      The parameter is NULL.  
**/
EFI_STATUS
EFIAPI
GetRequiredSize (
  OUT UINTN          *TotalSize,
  OUT UINTN          *HotkeyCount,
  OUT UINTN          *StringCount,
  OUT UINTN          *SystemInfoCount,
  OUT UINTN          *OperationCount  
  )
{
  UINTN                                           StrIndex;
  CHAR16                                          *StrPtr;
  
  StrPtr = NULL;

  if ((NULL == TotalSize) || (NULL == HotkeyCount) || (NULL == StringCount) || (NULL == SystemInfoCount) || (NULL == OperationCount)) {
    return EFI_INVALID_PARAMETER;
  }

  *TotalSize = sizeof (HOTKEY_AND_STRING_TABLE);

  //
  //  Calcuate hot key table size
  //
  *HotkeyCount =  GET_ARRAY_COUNT (mHotKeyDefine);
  *TotalSize   += *HotkeyCount * sizeof (DYNAMIC_HOTKEY);

  //
  //  Calcuate string table size
  //
  *StringCount =  GET_ARRAY_COUNT (StringDefine);
  *TotalSize   += *StringCount * (sizeof (DYNAMIC_STRING_EXTERNAL) - sizeof (CHAR8));  
  for (StrIndex = 0; StrIndex < *StringCount; StrIndex++) {
    StrPtr = GetStringByStringToken (StringDefine[StrIndex].StringToken);
    *TotalSize += (StrLen (StrPtr) + 1); 
  }

  //
  //  Calcuate system information table size
  //
  *SystemInfoCount =  GET_ARRAY_COUNT (SystemInfoTable);
  *TotalSize += *SystemInfoCount * (sizeof (DYNAMIC_SYSTEM_INFO_EXTERNAL) - sizeof (CHAR8));
  for (StrIndex = 0; StrIndex < *SystemInfoCount; StrIndex++) {
    StrPtr = GetStringByStringToken (SystemInfoTable[StrIndex].StringToken);
    *TotalSize += (StrLen (StrPtr) + 1); 
  }
  
  //
  //  Calcuate hot key operation table size
  //
  *OperationCount =  GET_ARRAY_COUNT (OperationTable);
  *TotalSize += *OperationCount * (sizeof (DYNAMIC_OPERATION_EXTERNAL) - sizeof (CHAR8));
  for (StrIndex = 0; StrIndex < *OperationCount; StrIndex++) {    
    StrPtr = GetStringByStringToken (OperationTable[StrIndex].StringToken);
    *TotalSize += (StrLen (StrPtr) + 1); 
  }
  return EFI_SUCCESS;
}

/**
  Allocate memory to store Hotkey and String Table

  @param[in]  NeedSize          the size of all data.
  @param[in]  *AllocateSize    the actual size by allocating.
  
  @retval EFI_SUCCESS          Allocate success
  @retval other                     Allocate fail
**/
EFI_STATUS
EFIAPI
AllocateReservedMemory (
  IN  UINTN          NeedSize,
  OUT UINTN          *AllocateSize
  )
{
  EFI_STATUS              Status;
  VOID                    *TempAddress;
  EFI_PHYSICAL_ADDRESS    PhyAddress;

  Status = EFI_SUCCESS;
  TempAddress = NULL;
  PhyAddress = 0;
  
  if (NULL == AllocateSize) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->AllocatePool (
                  EfiReservedMemoryType,
                  NeedSize,
                  (VOID **)&TempAddress
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  //  Check whether the address is smaller than the 4G
  //
  if ( (UINTN) TempAddress > HOTKEY_AND_STRING_TABLE_MAX_ADDRESS) {
    Status = gBS->FreePool (TempAddress);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    PhyAddress = HOTKEY_AND_STRING_TABLE_MAX_ADDRESS;
    Status = gBS->AllocatePages (
                    AllocateMaxAddress,
                    EfiReservedMemoryType,
                    EFI_SIZE_TO_PAGES (NeedSize),
                    &PhyAddress
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    mIsPageType = TRUE;
    mPageNum = EFI_SIZE_TO_PAGES (NeedSize);
    *AllocateSize = EFI_PAGES_TO_SIZE (EFI_SIZE_TO_PAGES (NeedSize));
    mHotkeyStringTable = (HOTKEY_AND_STRING_TABLE *)(UINTN)PhyAddress;

  } else {
    mIsPageType = FALSE;
    *AllocateSize = NeedSize;
    mHotkeyStringTable = (HOTKEY_AND_STRING_TABLE *)TempAddress;
  }

  return EFI_SUCCESS;
}

/**
  Fill data into hot key and string table

  @param[in]  HotkeyCount         the number of hot Key
  @param[in]  StringCount          the number of string
  @param[in]  SystemInfoCount   the number of system information
  @param[in]  OperationCount     the number of hot key operation

  @retval EFI_SUCCESS              Fill success
**/
EFI_STATUS
EFIAPI
FillDataToTable (
  IN UINTN                                        HotkeyCount,
  IN UINTN                                        StringCount,
  IN UINTN                                        SystemInfoCount,
  IN UINTN                                        OperationCount
  )
{
  UINTN                           Index;
  VOID                            *FillDataPtr;
  DYNAMIC_STRING_EXTERNAL         *DyStrPtr;
  DYNAMIC_SYSTEM_INFO_EXTERNAL    *DySysInfoPtr;
  CHAR8                           *TempAsciiString;
  CHAR16                          *TempUniString;
  DYNAMIC_OPERATION_EXTERNAL      *DyOperationPtr;

  Index = 0;
  FillDataPtr = NULL; 
  DyStrPtr = NULL;
  DySysInfoPtr = NULL;
  TempAsciiString = NULL;
  TempUniString = NULL;
  DyOperationPtr = NULL;
  
  mHotkeyStringTable->Signature   = HOTKEY_TABLE_PTR_SIGNATURE;
  mHotkeyStringTable->HotkeyCount = (UINT8)HotkeyCount;
  mHotkeyStringTable->StringCount = (UINT8)StringCount;
  mHotkeyStringTable->SystemInfoCount = (UINT8)SystemInfoCount;
  mHotkeyStringTable->OperationCount = (UINT8)OperationCount;
  //
  //Point to Hotkey table location.
  //
  FillDataPtr = (VOID *) (mHotkeyStringTable + 1);

  //
  //  Fill hot key data
  //
  CopyMem (FillDataPtr, mHotKeyDefine, sizeof (mHotKeyDefine));

  //
  //  Fill string data
  //
  DyStrPtr = (DYNAMIC_STRING_EXTERNAL *)((UINTN)FillDataPtr + sizeof (mHotKeyDefine));
  for (Index = 0; Index < GET_ARRAY_COUNT (StringDefine); Index++) {
    DyStrPtr->KeyId = StringDefine[Index].KeyId;
    DyStrPtr->SystemInfoId = StringDefine[Index].SystemInfoId;
    DyStrPtr->LocationX = StringDefine[Index].LocationX;
    DyStrPtr->LocationY = StringDefine[Index].LocationY;
    DyStrPtr->Flag = StringDefine[Index].Flag;
    CopyMem (&(DyStrPtr->Foreground), &StringDefine[Index].Foreground, sizeof (EFI_UGA_PIXEL));
    CopyMem (&(DyStrPtr->Background), &StringDefine[Index].Background, sizeof (EFI_UGA_PIXEL));    

    TempUniString = GetStringByStringToken (mStringList[Index].StringToken);
    TempAsciiString = AllocateZeroPool (StrSize (TempUniString));
    TempAsciiString = UnicodeStrToAsciiStr (TempUniString, TempAsciiString);
    CopyMem (&(DyStrPtr->String), TempAsciiString, AsciiStrSize (TempAsciiString));
    DyStrPtr = (DYNAMIC_STRING_EXTERNAL *)((UINTN)DyStrPtr + sizeof (DYNAMIC_STRING_EXTERNAL) - sizeof (CHAR8) + AsciiStrSize (TempAsciiString));
    FreePool (TempAsciiString);
  }

  //
  //  Fill system information
  //
  DySysInfoPtr = (DYNAMIC_SYSTEM_INFO_EXTERNAL *)DyStrPtr;
  for (Index = 0; Index < GET_ARRAY_COUNT (SystemInfoTable); Index++) {
    DySysInfoPtr->SystemInfoId = (UINT8)SystemInfoTable[Index].Type;
    DySysInfoPtr->LocationX = SystemInfoTable[Index].LocationX;
    DySysInfoPtr->LocationY = SystemInfoTable[Index].LocationY;
    TempUniString = GetStringByStringToken (SystemInfoTable[Index].StringToken);
    TempAsciiString = AllocateZeroPool (StrSize (TempUniString));
    TempAsciiString = UnicodeStrToAsciiStr (TempUniString, TempAsciiString);
    CopyMem (&(DySysInfoPtr->String), TempAsciiString, AsciiStrSize (TempAsciiString));
    DySysInfoPtr = (DYNAMIC_SYSTEM_INFO_EXTERNAL *)((UINTN)DySysInfoPtr + sizeof (DYNAMIC_SYSTEM_INFO_EXTERNAL) - sizeof (CHAR8) + AsciiStrSize (TempAsciiString));
    FreePool (TempAsciiString);
  }
  //
  //  Fill hot key operation
  //
  DyOperationPtr = (DYNAMIC_OPERATION_EXTERNAL *)DySysInfoPtr;
  for (Index = 0; Index < GET_ARRAY_COUNT (OperationTable); Index++) {
    DyOperationPtr->OperationId = OperationTable[Index].OperationId;
    TempUniString = GetStringByStringToken (OperationTable[Index].StringToken);
    TempAsciiString = AllocateZeroPool (StrSize (TempUniString));
    TempAsciiString = UnicodeStrToAsciiStr (TempUniString, TempAsciiString);
    CopyMem (&(DyOperationPtr->String), TempAsciiString, AsciiStrSize (TempAsciiString));
    DyOperationPtr = (DYNAMIC_OPERATION_EXTERNAL *)((UINTN)DyOperationPtr + sizeof (DYNAMIC_OPERATION_EXTERNAL) - sizeof (CHAR8) + AsciiStrSize (TempAsciiString));
    FreePool (TempAsciiString);
  }
  return EFI_SUCCESS;
}

/**
  Free the allocated memory of hot key and string table.

  None
  
  @retval   None
**/
VOID
EFIAPI
FreeHotkeyStringTable (
  VOID
  )
{
  if (NULL == mHotkeyStringTable) {
    return;
  }

  if (mIsPageType == TRUE) {
    gBS->FreePages ((EFI_PHYSICAL_ADDRESS)mHotkeyStringTable, mPageNum);

  } else {
    gBS->FreePool (mHotkeyStringTable);
  }

  return;
}

/**
  Find a suitable memory space to store Dynamic Hotkey and String Table Ptr
  find the E/F segment first. If there is no memory space, allocate it in high memory then store
  the address in the Variable - DynamicHotkey

  @param[in]  AllocateSize      the Hotkey and String table size
  @param[in]  **FEAddress         the address to store the signature

  @retval EFI_SUCCESS                    Install protocol success.
  @retval other                               Install protocol fail.
**/
EFI_STATUS
EFIAPI
GetFEsegmentTableAddress (
  IN  UINTN         AllocateSize,
  OUT VOID          **FEAddress
  )
{
  EFI_STATUS                     Status;
  LEGACY_BIOS_INSTANCE           *BiosPrivate;
  EFI_LEGACY_BIOS_PROTOCOL       *LegacyBios;
  EFI_LEGACY_REGION2_PROTOCOL    *LegacyRegion;
  EFI_IA32_REGISTER_SET          Regs;
  HOTKEY_AND_STRING_PTR          *Point;

  BiosPrivate = NULL;
  LegacyBios = NULL;
  LegacyRegion = NULL;
  Point = NULL;  
  ZeroMem (&Regs, sizeof (EFI_IA32_REGISTER_SET));
  
  if (NULL == FEAddress) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // LocateProtocol : LegacyBios
  //
  Status = gBS->LocateProtocol (
                  &gEfiLegacyBiosProtocolGuid,
                  NULL,
                  (VOID **)&LegacyBios
                  );
  if (EFI_ERROR (Status)) {
    //
    //  Pure EFI Boot. Allocate high memory to store the signature, and store the address in the variable
    //
    Status = gBS->AllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (HOTKEY_AND_STRING_PTR),
                    (VOID **)&Point
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Point->Signature  = HOTKEY_TABLE_PTR_SIGNATURE;
    Point->Address    = (UINT32)(UINTN)mHotkeyStringTable;
    Point->Size       = (UINT32)AllocateSize;

    *FEAddress = (VOID *)Point;

    DEBUG ( (EFI_D_INFO, "GetFEsegmentTableAddress:  Table          at 0x%x\n", *FEAddress));

    Status = gRT->SetVariable (
                    L"DynamicHotkey",
                    &gDynamicHotKeyGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    sizeof (HOTKEY_AND_STRING_PTR),
                    (VOID *)Point
                    );

    if (EFI_ERROR (Status)) {
      return Status;
    }
    
    return EFI_SUCCESS;
  }

  //
  // LocateProtocol : LegacyRegion
  //
  Status = gBS->LocateProtocol (
                  &gEfiLegacyRegion2ProtocolGuid,
                  NULL,
                  (VOID **)&LegacyRegion
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  //  Get FE segment Table Address
  //
  LegacyRegion->UnLock (LegacyRegion, 0xE0000, 0x20000, NULL);

  ZeroMem (&Regs, sizeof (EFI_IA32_REGISTER_SET));
  Regs.X.AX = Legacy16GetTableAddress;
  Regs.X.BX = F0000Region;
  Regs.X.CX = (UINT16)sizeof (HOTKEY_AND_STRING_PTR);
  Regs.X.DX = HOTKEY_TABLE_PTR_ALIGNMENT;

  BiosPrivate = LEGACY_BIOS_INSTANCE_FROM_THIS (LegacyBios);
  Status = LegacyBios->FarCall86 (
                         LegacyBios,
                         BiosPrivate->Legacy16CallSegment,
                         BiosPrivate->Legacy16CallOffset,
                         &Regs,
                         NULL,
                         0
                         );

  if (!EFI_ERROR (Regs.X.AX)) {
    Point = (HOTKEY_AND_STRING_PTR *)(UINTN)(Regs.X.DS * 16 + Regs.X.BX);
    Point->Signature  = HOTKEY_TABLE_PTR_SIGNATURE;
    Point->Address    = (UINT32)(UINTN)mHotkeyStringTable;
    Point->Size       = (UINT32)AllocateSize;
  }

  LegacyRegion->Lock (LegacyRegion, 0xE0000, 0x20000, NULL);

  if (EFI_ERROR (Regs.X.AX)) {
    return Regs.X.AX;
  }

  //
  //  Return value
  //
  *FEAddress = (VOID *)Point;

  return EFI_SUCCESS;
}

/**
 Initialize hot key and string table

 @param[in]         Event             Event whose notification function is being invoked.
 @param[in]         *Context         Pointer to the notification function's context.

 @retval              None
*/  
VOID
EFIAPI
InitializeHotkeyAndStringTable (
  IN  EFI_EVENT   Event,
  IN  VOID       *Context
  )
{
	
  EFI_STATUS          Status;
  UINTN               TotalSize;
  UINTN               HotkeyCount;
  UINTN               StringCount;
  UINTN               SystemInfoCount;
  UINTN               OperationCount;
  UINTN               AllocateSize;
  
  TotalSize = 0;
  HotkeyCount = 0;
  StringCount = 0;
  SystemInfoCount = 0;
  OperationCount = 0;	
  AllocateSize = 0;
  
  DEBUG ( (EFI_D_INFO, "[InitializeHotkeyAndStringTable] %a Start\n", __FUNCTION__));
	
  //
  // Off events to prevent re-entry into the function
  //
  Status = gBS->CloseEvent (mReadyToBootEvent);
  if (EFI_ERROR (Status)) {
    //
    //  There is no need to free memory, this function Automatically free memory
    //
    return;
  }
	
	
  //
  //  Get size and count
  //
  Status = GetRequiredSize (&TotalSize, &HotkeyCount, &StringCount, &SystemInfoCount, &OperationCount);
  if (EFI_ERROR (Status)) {
    //
    //  There is no need to free memory, this function Automatically free memory
    //
    return;
  }
	
  //
  //  Allocate Reserved Memory
  //
  Status = AllocateReservedMemory (TotalSize, &AllocateSize);
  if (EFI_ERROR (Status)) {
    //
    //  There is no need to free memory, this function Automatically free memory
    //
    return;
  }
	
  //
  //  Copy String, VfrBin
  //
  Status = FillDataToTable (HotkeyCount, StringCount, SystemInfoCount, OperationCount);
  if (EFI_ERROR (Status)) {
    //
    //  It should free memory
    //
    FreeHotkeyStringTable ();
    return;
  }
	
  //
  //  Fill FE segment
  //
  Status = GetFEsegmentTableAddress (AllocateSize, &mFEaddress);
  if (EFI_ERROR (Status)) {
    //
    //  It should free memory
    //
    FreeHotkeyStringTable ();
    return;
  }
  DEBUG ( (EFI_D_INFO, "[InitializeVariableEditTable] %a End\n", __FUNCTION__));
	
  //
  // Print Information
  //
  DEBUG ( (EFI_D_INFO, "Dynamic HotKey PTR   at 0x%x\n", mFEaddress));
  DEBUG ( (EFI_D_INFO, "Dynamic HotKey TABLE at 0x%x\n", mHotkeyStringTable));

  return;
}


/**
 Entrypoint of this module.

 @param[in]         ImageHandle         The firmware allocated handle for the EFI image.
 @param[in]         SystemTable         A pointer to the EFI System Table.

 @retval EFI_SUCCESS                    Install protocol success.
 @retval other                               Install protocol fail.
*/
EFI_STATUS
EFIAPI
DynamicHotKeyEntry (
  IN  EFI_HANDLE                        ImageHandle,
  IN  EFI_SYSTEM_TABLE                  *SystemTable
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            Handle;
  DYNAMIC_HOTKEY_PROTOCOL               *DynamicHotKey;

  //
  // Check if the custom-define protocol is installed or not
  //
  Status = gBS->LocateProtocol (
                  &gDynamicHotKeyProtocolGuid,
                  NULL,
                  (VOID **)&DynamicHotKey
                  );
  if (!EFI_ERROR (Status)) {
    return EFI_ALREADY_STARTED;
  }

  //
  // Create "ReadyToBoot" Event
  //
  Status = EfiCreateEventReadyToBootEx (
             TPL_NOTIFY,
             InitializeHotkeyAndStringTable,
             NULL,
             &mReadyToBootEvent
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }


  //
  // Fill context
  //
  mDynamicHotKey.GetDynamicHotKeyList = GetDynamicHotKeyList;
  mDynamicHotKey.GetDynamicHotKeyOperation = GetDynamicHotKeyOperation;
  mDynamicHotKey.GetDynamicStringCount = GetDynamicStringCount;
  mDynamicHotKey.GetDynamicString = GetDynamicString;
  mDynamicHotKey.GetDynamicStringColor = GetDynamicStringColor;  
  mDynamicHotKey.IsDynamicHotKeyPressed = IsDynamicHotKeyPressed;
  mDynamicHotKey.AdjustStringPosition = AdjustStringPosition;
  mDynamicHotKey.ShowSystemInformation = ShowSystemInformation;
  
  //
  // Initialize strings to HII database
  //
  mHotKeyStringPackHandle = HiiAddPackages (
                              &gEfiCallerIdGuid,
                              NULL,
                              DynamicHotKeyDxeStrings,
                              NULL
                              );
  ASSERT (mHotKeyStringPackHandle != NULL);

  Status = GetHotKeyAndString ();
  
  //
  // Install protocol
  //
  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gDynamicHotKeyProtocolGuid,
                  &mDynamicHotKey,
                  NULL
                  );

  return Status;

}

