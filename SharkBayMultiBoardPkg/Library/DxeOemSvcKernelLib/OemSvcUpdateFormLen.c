/** @file
  Adjust the width of blocks.

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <SetupConfig.h>
#include <Protocol/HiiDatabase.h>
#include <Guid/DebugMask.h>
#include <Guid/HiiPlatformSetupFormset.h>
#include <Guid/MdeModuleHii.h>
#include <Guid/GlobalVariable.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HiiLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/DxeOemSvcKernelLib.h>

//
// data type definitions
//
#define  NON_INIT                     0xFF
#define  FORM_NUM                     0x06
#define  DEFAULT_OPTION_BLOCK_WIDTH   0x1A
#define  DEFAULT_PROMPT_BLOCK_WIDTH   0x1A

#define  MAX_ITEMS                    10

typedef struct {
  UINT8          CurRoot;
  UINT8          PromptBlockWidth;
  UINT8          OptionBlockWidth;
} OEM_FORM_LENGTH_INFO;

typedef struct {
  UINT8                   Language;
  CHAR8                   *LangCode;
  OEM_FORM_LENGTH_INFO    FormLen[FORM_NUM];
} LANG_MAP_TABLE;

typedef struct _ECP_EFI_IFR_FORM_SET {
  EFI_IFR_OP_HEADER        Header;
  EFI_GUID                 Guid;
  EFI_STRING_ID            FormSetTitle;
  EFI_STRING_ID            Help;
  UINT8                    Flags;
  EFI_GUID                 ClassGuid[1];
} ECP_EFI_IFR_FORM_SET;

typedef struct _EFI_HII_FORM_PACKAGE {
  EFI_HII_PACKAGE_HEADER       Header;
  // EFI_IFR_OP_HEADER         OpCodeHeader;
  // More op-codes follow
} ECP_EFI_HII_FORM_PACKAGE;

//
// module variables
//

//
//  The text mode is 80x25 the PromptBlockWidth + OptionBlockWidth should be 0x34
//  The text mode is 100x31 the PromptBlockWidth + OptionBlockWidth should be 0x42
//  The text mode is 128x40 the PromptBlockWidth + OptionBlockWidth should be 0x54
//  If does't follow this rule, it will use default setting (PromptBlockWidth == OptionBlockWidth)
//  The following are the examples for other text resolution
//

//
// Resolution 640x480 (Full screen text mode is 80x25)
//
//                                       {{0x00, 0x1A, 0x1A},  //Exit
//                                       {0x01, 0x1A, 0x1A},  //Boot
//                                       {0x02, 0x1A, 0x1A},  //Power
//                                       {0x03, 0x1A, 0x1A},  //Security
//                                       {0x04, 0x1A, 0x1A},  //Advanced
//                                       {0x05, 0x1A, 0x1A}   //Main

//
// Resolution 800x600 (Full screen text mode is 100x31)
//
//                                       {{0x00, 0x21, 0x21},  //Exit
//                                       {0x01, 0x21, 0x21},  //Boot
//                                       {0x02, 0x21, 0x21},  //Power
//                                       {0x03, 0x21, 0x21},  //Security
//                                       {0x04, 0x21, 0x21},  //Advanced
//                                       {0x05, 0x21, 0x21}   //Main

//
// Resolution 1024x768(Full screen text mode is 128x40)
//
LANG_MAP_TABLE   OemLangMapTable[] = { {NON_INIT, {"en-US"},
                                       {{0x00, 0x2A, 0x2A},  //Exit
                                       {0x01, 0x2A, 0x2A},  //Boot
                                       {0x02, 0x2A, 0x2A},  //Power
                                       {0x03, 0x2A, 0x2A},  //Security
                                       {0x04, 0x2B, 0x29},  //Advanced
                                       {0x05, 0x2A, 0x2A}   //Main
                                      }},

                                       {NON_INIT, {"fr-FR"},
                                       {{0x00, 0x54, 0x00},  //Exit
                                       {0x01, 0x2A, 0x2A},  //Boot
                                       {0x02, 0x2A, 0x2A},  //Power
                                       {0x03, 0x2F, 0x25},  //Security
                                       {0x04, 0x2A, 0x2A},  //Advanced
                                       {0x05, 0x2C, 0x28}   //Main
                                      }},
                                       {NON_INIT, {"zh-TW"},
                                       {{0x00, 0x54, 0x00},  //Exit
                                       {0x01, 0x2A, 0x2A},  //Boot
                                       {0x02, 0x2A, 0x2A},  //Power
                                       {0x03, 0x2A, 0x2A},  //Security
                                       {0x04, 0x2A, 0x2A},  //Advanced
                                       {0x05, 0x2A, 0x2A}   //Main
                                      }},
                                       {NON_INIT, {"ja-JP"},
                                       {{0x00, 0x54, 0x00},  //Exit
                                       {0x01, 0x30, 0x24},  //Boot
                                       {0x02, 0x2A, 0x2A},  //Power
                                       {0x03, 0x2A, 0x2A},  //Security
                                       {0x04, 0x2A, 0x2A},  //Advanced
                                       {0x05, 0x2A, 0x2A}   //Main
                                      }},
                                       {NON_INIT, {"it-IT"},
                                       {{0x00, 0x54, 0x00},  //Exit
                                       {0x01, 0x30, 0x24},  //Boot
                                       {0x02, 0x34, 0x20},  //Power
                                       {0x03, 0x2A, 0x2A},  //Security
                                       {0x04, 0x34, 0x20},  //Advanced
                                       {0x05, 0x30, 0x24}   //Main
                                      }},
                                       {NON_INIT, {"es-ES"},
                                       {{0x00, 0x54, 0x00},  //Exit
                                       {0x01, 0x30, 0x24},  //Boot
                                       {0x02, 0x34, 0x20},  //Power
                                       {0x03, 0x2F, 0x25},  //Security
                                       {0x04, 0x34, 0x20},  //Advanced
                                       {0x05, 0x32, 0x22}   //Main
                                      }},
                                       {NON_INIT, {"de-DE"},
                                       {{0x00, 0x54, 0x00},  //Exit
                                       {0x01, 0x30, 0x24},  //Boot
                                       {0x02, 0x34, 0x20},  //Power
                                       {0x03, 0x2A, 0x2A},  //Security
                                       {0x04, 0x34, 0x20},  //Advanced
                                       {0x05, 0x32, 0x22}   //Main
                                      }},
                                       {NON_INIT, {"pt-PT"},
                                       {{0x00, 0x54, 0x00},  //Exit
                                       {0x01, 0x30, 0x24},  //Boot
                                       {0x02, 0x34, 0x20},  //Power
                                       {0x03, 0x2F, 0x25},  //Security
                                       {0x04, 0x34, 0x20},  //Advanced
                                       {0x05, 0x32, 0x22}   //Main
                                      }}
                                      };

BOOLEAN          mInitialize = FALSE;
UINTN            mLangNum = 0;

/**
  Compares two Ascii strings

  @param  String                Pointer to a string
  @param  String2               Pointer to a string
  @param  Len                   Length of strings to compare
 
  @retval TRUE                  String is not identical
  @retval False                 String is identical 
**/
BOOLEAN
OemStrnCmp (
  IN CHAR8   *String,
  IN CHAR8   *String2,
  IN UINTN    Len
  )
{
  while (Len != 0) {
    if (*String != *String2) {
      return TRUE;
    }

    String += 1;
    String2 += 1;
    Len--;
  }

  return FALSE;
}

/**
  Read the EFI variable (VendorGuid/Name) and return a dynamically allocated
  buffer, and the size of the buffer. If failure return NULL.

  @param[in]  *Name             String part of EFI variable name
  @param[in]  *VendorGuid       GUID part of EFI variable name
  @param[out] *VariableSize     Returns the size of the EFI variable that was read
 
  @retval VOID*                 Dynamically allocated memory that contains a copy of the EFI variable.
                                Caller is responsible freeing the buffer.
  @retval NULL                  Variable was not read
**/
VOID *
OemGetVariableAndSize (
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
      FreePool (Buffer);
      Buffer = NULL;
    }

  }

  *VariableSize = BufferSize;

  return Buffer;
}

/**
  Update module variable OemLangMapTable.

  @param[in]  CurRoot         Value to update. 
  @param[in]  MenuIndex       Index of the Form to be updated.
 
  @retval     EFI_SUCCESS               Always returns success.
  @retval     EFI_INVALID_PARAMETER     MenuIndex is out of range.
**/
EFI_STATUS
UpdateCurRoot (
  UINT8        CurRoot,
  UINTN        MenuIndex
  )
{
  UINTN        Index;
  UINTN        MapTableSize;

  MapTableSize = sizeof (OemLangMapTable) / sizeof (LANG_MAP_TABLE);

  if (MenuIndex >= FORM_NUM) {
    //
    // Index of OemLangMapTable[Index].FormLen exceeds limit
    //
    return EFI_INVALID_PARAMETER;
  }

  for (Index = 0; Index < mLangNum && Index < MapTableSize; Index++) {
    OemLangMapTable[Index].FormLen[MenuIndex].CurRoot = CurRoot;
  }
  return EFI_SUCCESS;
}


/**
  Retrieves the array of all the HII Handles.
  This function allocates the returned array using AllocatePool().
  The caller is responsible for freeing the array with FreePool().

  @param[in]  HiiDatabase   Hii data base.

  @retval NULL              The array of HII Handles could not be retrieved
  @retval Other             A pointer to the NULL terminated array of HII Handles

**/
EFI_HII_HANDLE *
EFIAPI
HiiGetAllOfHiiHandles (
  EFI_HII_DATABASE_PROTOCOL        *HiiDatabase
  )
{
  EFI_STATUS      Status;
  UINTN           HandleBufferLength;
  EFI_HII_HANDLE  TempHiiHandleBuffer;
  EFI_HII_HANDLE  *HiiHandleBuffer;

  //
  // Retrieve the size required for the buffer of all HII handles.
  //
  HandleBufferLength = 0;
  Status = HiiDatabase->ListPackageLists (
                          HiiDatabase,
                          EFI_HII_PACKAGE_TYPE_ALL,
                          NULL,
                          &HandleBufferLength,
                          &TempHiiHandleBuffer
                          );

  //
  // If ListPackageLists() returns EFI_SUCCESS for a zero size, 
  // then there are no HII handles in the HII database.  If ListPackageLists() 
  // returns an error other than EFI_BUFFER_TOO_SMALL, then there are no HII 
  // handles in the HII database.
  //
  if (Status != EFI_BUFFER_TOO_SMALL) {
    //
    // Return NULL if the size can not be retrieved, or if there are no HII 
    // handles in the HII Database
    //
    return NULL;
  }

  //
  // Allocate the array of HII handles to hold all the HII Handles and a NULL terminator
  //
  HiiHandleBuffer = AllocateZeroPool (HandleBufferLength + sizeof (EFI_HII_HANDLE));
  if (HiiHandleBuffer == NULL) {
    //
    // Return NULL if allocation fails.
    //
    return NULL;
  }

  //
  // Retrieve the array of HII Handles in the HII Database
  //
  Status = HiiDatabase->ListPackageLists (
                          HiiDatabase,
                          EFI_HII_PACKAGE_TYPE_ALL,
                          NULL,
                          &HandleBufferLength,
                          HiiHandleBuffer
                           );
  if (EFI_ERROR (Status)) {
    //
    // Free the buffer and return NULL if the HII handles can not be retrieved.
    //
    FreePool (HiiHandleBuffer);
    return NULL;
  }

  //
  // Return the NULL terminated array of HII handles in the HII Database
  //
  return HiiHandleBuffer;
}

/**
  Get Info from Setup variable, and call UpdateCurRoot.

  @param  None.
 
  @retval None.
**/
EFI_STATUS
InitCurRoot (
  VOID
  )
{
  EFI_STATUS                       Status;
  EFI_GUID                         GuidId = SYSTEM_CONFIGURATION_GUID;
  EFI_GUID                         FormSetGuid = EFI_HII_PLATFORM_SETUP_FORMSET_GUID;
  UINTN                            BufferSize;
  UINTN                            Index;
  SYSTEM_CONFIGURATION             *SetupNvData;
  EFI_HII_HANDLE                   *HiiHandleBuffer;
  EFI_HII_PACKAGE_LIST_HEADER      *Buffer;
  UINT8                            *TempPtr;
  ECP_EFI_IFR_FORM_SET             *FormSetPtr;
  UINT16                           Class;
  UINT16                           SubClass;
  EFI_HII_DATABASE_PROTOCOL        *HiiDatabase;
  UINTN                            MenuItemCount;
  UINTN                            TotalMenuItemCount;

  Status = gBS->LocateProtocol (&gEfiHiiDatabaseProtocolGuid, NULL, (VOID **)&HiiDatabase);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  HiiHandleBuffer = NULL;
  HiiHandleBuffer = HiiGetAllOfHiiHandles (HiiDatabase);
  if (HiiHandleBuffer == NULL) {
    return Status;
  }
  
  SetupNvData = OemGetVariableAndSize (
                  L"Setup",
                 &GuidId,
                 &BufferSize
                 );
  if (SetupNvData == NULL) {
    return Status;
  }

  Buffer = NULL;
  MenuItemCount = 0;
  TotalMenuItemCount = 0;
  for (Index = 0; HiiHandleBuffer[Index] != NULL; Index++) {
    Status = HiiDatabase->ExportPackageLists (
                            HiiDatabase,
                            HiiHandleBuffer[Index],
                            &BufferSize,
                            Buffer
                            );
    if (Status == EFI_NOT_FOUND) {
      break;
    }

    //
    // BufferSize should have the real size of the forms now
    //
    Buffer = AllocateZeroPool (BufferSize);
    if (Buffer == NULL) {
      return Status;
    }

    //
    // Am not initializing Buffer since the first thing checked is the size
    // this way I can get the real buffersize in the smallest code size
    //
    Status = HiiDatabase->ExportPackageLists (
                            HiiDatabase,
                            HiiHandleBuffer[Index],
                            &BufferSize,
                            Buffer
                            );
    //
    // Skips the header, now points to data
    //
    TempPtr = (UINT8 *) (Buffer + 1);
    if (((EFI_HII_PACKAGE_HEADER *) TempPtr)->Type != EFI_HII_PACKAGE_FORMS) {
      BufferSize = 0;
      FreePool(Buffer);
      continue;
    }
    TempPtr = (UINT8 *) ((ECP_EFI_HII_FORM_PACKAGE *) TempPtr + 1);
    FormSetPtr = (ECP_EFI_IFR_FORM_SET *) TempPtr;
    if (CompareGuid (FormSetPtr->ClassGuid, &FormSetGuid)) {
      TempPtr = (UINT8 *) ((EFI_IFR_FORM_SET *) TempPtr + 1);
      Class = ((EFI_IFR_GUID_CLASS *) TempPtr)->Class;
      TempPtr += sizeof (EFI_IFR_GUID_CLASS);
      SubClass = ((EFI_IFR_GUID_SUBCLASS *) TempPtr)->SubClass;
      if (Class == EFI_NON_DEVICE_CLASS) {
        BufferSize = 0;
        FreePool(Buffer);
        continue;
      } else {
        TotalMenuItemCount++;
        //
        // Limited
        //
        if (SubClass == EFI_USER_ACCESS_THREE) {
          if ((SetupNvData->SetUserPass == 1) && (SetupNvData->UserAccessLevel == 3)) {
            UpdateCurRoot (0xff, TotalMenuItemCount - 1);
            BufferSize = 0;
            FreePool(Buffer);
            continue;
          }
        }
        //
        // View Only
        //
        if (SubClass == EFI_USER_ACCESS_TWO) {
          if ((SetupNvData->SetUserPass == 1) && (SetupNvData->UserAccessLevel == 2)) {
            UpdateCurRoot (0xff, TotalMenuItemCount - 1);
            BufferSize = 0;
            FreePool(Buffer);
            continue;
          }
        }
        if (TotalMenuItemCount < MAX_ITEMS) {
          UpdateCurRoot ((UINT8) MenuItemCount, TotalMenuItemCount - 1);
          MenuItemCount++;
        }
      }
    }
    BufferSize = 0;
    FreePool (Buffer);
  }
  FreePool (HiiHandleBuffer);
  FreePool (SetupNvData);
  return EFI_SUCCESS;
}

/**
  Get Info from PlatformLangCodes variable, and update module variable mLangNum.

  @param  None.
 
  @retval EFI_SUCCESS     Always returns success.
**/
EFI_STATUS
UpdateLangDatabase (
  VOID
  )
{

//[-start-130717-IB05400432-modify]//
  UINTN                                 BufferSize, Index, LangIndex;
  UINT8                                 *LangBuffer;

  LangBuffer = OemGetVariableAndSize (L"PlatformLangCodes", &gEfiGlobalVariableGuid, &BufferSize);

  LangIndex = 0;
  if (LangBuffer != NULL) {
    LangIndex = 1;
    for (Index = 0; LangBuffer[Index] != 0; Index++) {
      if (LangBuffer[Index] == ';') {
        LangIndex++;
      }
    }
    FreePool (LangBuffer);
  }
  mLangNum = LangIndex;

  
  return  EFI_SUCCESS;
//[-end-130717-IB05400432-modify]//
}

/**
  According to the setting of language and the menu of SCU to adjust the width of blocks 
  (PromptBlock, OptionBlock, and HelpBlock).

  @param[in]   *Data                 Point to EFI_IFR_DARA_ARRAY. It provides service to get current language setting.
  @param[in]   KeepCurRoot           The index of current menu (Exit, Boot, Power, etc..) in SCU.
  @param[out]  *PromptBlockWidth     The width of Prompt Block in the menu. The detail refers to the following graph.
  @param[out]  *OptionBlockWidth     The width of Option Block in the menu. The detail refers to the following graph.
  @param[out]  *HelpBlockWidth       The width of Help Block in the menu. The detail refers to the following graph.
   |--------------------------------------------------|
   |   SCU menu        (Boot, Exit..etc)              |
   |--------------------------------------------------|
   |                |                |                |
   |                |                |                |
   |PromptBlockWidth|OptionBlockWidth| HelpBlockWidth |
   |                |                |                |
   |                |                |                |
   |                |                |                |
   |--------------------------------------------------|
   
  @retval      EFI_NOT_FOUND         Output default block width.
  @retval      EFI_MEDIA_CHANGED     Get the block width success.
  @retval      Others                Base on OEM design.
**/
EFI_STATUS
OemSvcUpdateFormLen (
  IN     UINT8                          *Data,
  IN     UINT8                          KeepCurRoot,
  IN OUT CHAR8                          *PromptBlockWidth,
  IN OUT CHAR8                          *OptionBlockWidth,
  IN OUT CHAR8                          *HelpBlockWidth
  )
{
  UINT8                                 *LangCode;
  UINTN                                 Index;
  UINTN                                 LangIndex;
  UINTN                                 Size;
  BOOLEAN                               LenFoundFlag = FALSE;
  UINTN                                 MapTableSize;

  MapTableSize = sizeof (OemLangMapTable) / sizeof (LANG_MAP_TABLE);

  if (!mInitialize) {
    UpdateLangDatabase ();
    InitCurRoot ();
    mInitialize = TRUE;
  }

  LangCode = OemGetVariableAndSize ( L"PlatformLang", &gEfiGlobalVariableGuid, &Size);
  if (LangCode == NULL) {
    return EFI_UNSUPPORTED;
  }

  for (LangIndex = 0; LangIndex < mLangNum && LangIndex < MapTableSize; LangIndex++) {
    if (!OemStrnCmp (OemLangMapTable[LangIndex].LangCode, LangCode, AsciiStrSize (LangCode))) {
      for (Index = 0; Index < FORM_NUM; Index++) {

        if (OemLangMapTable[LangIndex].FormLen[Index].CurRoot == KeepCurRoot){
          *OptionBlockWidth = OemLangMapTable[LangIndex].FormLen[Index].OptionBlockWidth;
          *PromptBlockWidth = OemLangMapTable[LangIndex].FormLen[Index].PromptBlockWidth;
          LenFoundFlag = TRUE;
          break;
        }
      }
      break;
    }
  }
  FreePool (LangCode);
  
  if (!LenFoundFlag) {
    //
    // The IN OUT parameters were not changed.
    //
    return EFI_UNSUPPORTED;
  }

  return EFI_MEDIA_CHANGED;
}
