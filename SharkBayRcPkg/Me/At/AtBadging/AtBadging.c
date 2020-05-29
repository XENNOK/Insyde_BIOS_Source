/** @file

 Header file.

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

#include <Uefi.h>
#include "AtBadgingString.h"

#define OEM_LOGO_SUPPORT TRUE

extern UINT8      AtBadgingStrings[];
EFI_GUID          gAtBadgingStringsGuid = {0xd80db40, 0xe69c, 0x42eb, 0x8c, 0x5e, 0xab, 0x32, 0xc0, 0x4f, 0x69, 0xea};
EFI_HII_HANDLE    gAtBadgingStringsHandle;

// EFI_OEM_STRING_SUPPORT_GET_STRING   mGetOemString;
EFI_OEM_BADGING_SUPPORT_PROTOCOL    *Badging;

extern UINTN mStringCountArray[];
extern UINTN PassWordLocationY;
extern UINTN PassWordTypeflag1;
extern UINTN PassWordTypeflag2;
extern OEM_BADGING_STRING *mStringArray[];
extern OEM_BADGING_STRING mIsvScreen[];
extern OEM_BADGING_STRING mUserRecoverySelected[];
extern OEM_BADGING_STRING mUserTimeLeftMessage[];
extern OEM_BADGING_STRING mUserInitializingWWANInterface[];
extern OEM_BADGING_STRING mUserShowNullSystemPassword[];
extern OEM_BADGING_STRING mUserShowPassword[];
extern OEM_BADGING_STRING mShowSuspendMode[];
extern OEM_BADGING_STRING mServerAddress[];
extern OEM_BADGING_STRING mClearTopScreenString[];
extern OEM_BADGING_STRING mClearBottomScreenString[];

EFI_OEM_BADGING_LOGO_DATA mBadgingData[] = {
  {EFI_DEFAULT_PCX_LOGO_GUID,
   EfiBadgingSupportFormatPCX,
   EfiBadgingSupportDisplayAttributeCenter,
   0,
   0,
   NULL,
   EfiBadgingSupportImageLogo
  },
  
  ///
  /// BIOS Vendor Insyde Badge
  ///
  {EFI_INSYDE_BADGING_GUID,
   EfiBadgingSupportFormatTGA,
   EfiBadgingSupportDisplayAttributeRightBottom,
   0,
   0,
   NULL,
   EfiBadgingSupportImageLogo
  }
};

/**
  Returns the length of a Null-terminated Unicode string.

  @param [in] String              A pointer to a Null-terminated Unicode string.

  @retval The length of String.

**/
UINTN
EfiStrLen (
  IN CHAR16   *String
  );


/**
  Hook GetOemString and GetStringCount routine

  @param [in] Event               The event that triggered this notification function  
  @param [in] Context             Pointer to the notification functions context

  @retval EFI_SUCCESS             Hook GetOemString and GetStringCount routine Succesfully
  
**/
VOID
EFIAPI
HookForAtInfo (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  );


/**
  Get String information which should be shown on screen

  @param [in]  This               Protocol instance pointer.
  @param [in]  Index              String Index of mOemBadgingString array
  @param [in]  AfterSelect        Not use any more
  @param [in]  SelectedStringNum  String number 
                                  High nibble: Type 
                                  Low nibble : Index of string Type
  @param [out] StringData         String data to output on screen
  @param [out] CoordinateX        String data location on screen
  @param [out] CoordinateY        String data location on screen
  @param [out] Foreground         Foreground color information of the string
  @param [out] Background         Background color information of the string

  @retval TRUE                    Preferred string has be gotten
  @retval FALSE                   Cannot find preferred string.
  
**/
BOOLEAN
EFIAPI
GetOemStringHook (
  IN     EFI_OEM_BADGING_SUPPORT_PROTOCOL  *This,
  IN     UINTN                             Index,
  IN     BOOLEAN                           AfterSelect,
  IN     UINT8                             SelectedStringNum,
  OUT    CHAR16                            **StringData,
  OUT    UINTN                             *CoordinateX,
  OUT    UINTN                             *CoordinateY,
  OUT    EFI_UGA_PIXEL                     *Foreground,
  OUT    EFI_UGA_PIXEL                     *Background
  );

/**
  Returns the length of a Null-terminated Unicode string.

  @param [in] String              A pointer to a Null-terminated Unicode string.

  @retval The length of String.

**/
UINTN
EfiStrLen (
  IN CHAR16   *String
  )
{
  return StrLen (String);
}

/**
  Install callback routine for EXIT_AT_AUTH_PROTOCOL_GUID installed.

  @param [in] ImageHandle         The image handle of the DXE Driver, DXE Runtime Driver, DXE SMM Driver, or UEFI Driver.
  @param [in] SystemTable         A pointer to the EFI System Table.

  @retval EFI_SUCCESS             The function completed successfully.

**/
EFI_STATUS
EFIAPI
AtBadgingEntry (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
  )
{
  EFI_STATUS  Status;
  VOID        *Registration;
  EFI_EVENT   Event;

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  HookForAtInfo,
                  NULL,                      /// Notification Context
                  &Event
                  );
  
  if (EFI_ERROR(Status)) {
    return EFI_ABORTED;
  }
  
  Status = gBS->RegisterProtocolNotify (
                  &gExitAtAuthProtocolGuid,
                  Event,
                  &Registration
                  );
  ASSERT_EFI_ERROR (Status);
  
  if (EFI_ERROR(Status)) {
    return EFI_ABORTED;
  }
  return EFI_SUCCESS;
}



/**
  Get Number of string that should be shown on screen

  @param [in]  This               Protocol instance pointer.
  @param [out] StringCount        Address of string count array that should be shown on screen

  @retval EFI_SUCCESS             Get Address of string count array Succesfully
  @retval EFI_UNSUPPORTED         There is no string should be shown.
  
**/
EFI_STATUS
GetStringCountHook (
  IN   EFI_OEM_BADGING_SUPPORT_PROTOCOL   *This,
  OUT  UINTN                              *StringCount
  )
{

  *StringCount = (UINTN)mStringCountArray;

  return EFI_SUCCESS;
}



/**
  Hook GetOemString and GetStringCount routine

  @param [in] Event               The event that triggered this notification function  
  @param [in] Context             Pointer to the notification functions context

  @retval EFI_SUCCESS             Hook GetOemString and GetStringCount routine Succesfully
  
**/
VOID
EFIAPI
HookForAtInfo (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS            Status;
  EFI_EXIT_AT_PROTOCOL  *IntelATData;
  CHAR16                VendorString[] = { L'I',L'n',L's',L'y',L'd',L'e',L'H',L'2',L'O',L'\0'};
  OEM_BADGING_INFO      *OemBadgingInfo;
  
  Status = gBS->LocateProtocol (
                  &gEfiOEMBadgingSupportProtocolGuid, 
                  NULL, 
                  (VOID **)&Badging
                  );

  if (EFI_ERROR (Status)) {
    return ;
  }
  
  ///
  /// Due to GetOemString and GetOemStringCount function will be temporary override...
  /// Here we save both original function pointers into volatile variable.
  ///
  gRT->SetVariable (
        L"OemBadgingStringFunPtr",
        &gEfiGlobalVariableGuid,
        EFI_VARIABLE_BOOTSERVICE_ACCESS,
        sizeof (Badging->GetOemString),
        &(Badging->GetOemString)
        );

  gRT->SetVariable (
        L"OemBadgingStringCountFunPtr",
        &gEfiGlobalVariableGuid,
        EFI_VARIABLE_BOOTSERVICE_ACCESS,
        sizeof (Badging->GetStringCount),
        &(Badging->GetStringCount)
        );
  
  OemBadgingInfo = EFI_OEM_BADGING_INFO_FROM_THIS (Badging);
  ///
  /// Since some private data will be temporary overrided in GetOemString()...
  /// we save original into variable.
  ///  
  gRT->SetVariable (
         L"OemBadgingInfoStrCount",
         &gEfiGlobalVariableGuid,
         EFI_VARIABLE_BOOTSERVICE_ACCESS,
         sizeof (UINTN),
         &(OemBadgingInfo->BadginStringCount)
         );

  gRT->SetVariable (
         L"OemBadgingInfoString",
         &gEfiGlobalVariableGuid,
         EFI_VARIABLE_BOOTSERVICE_ACCESS,
         sizeof (OemBadgingInfo->OemBadgingString),
         &(OemBadgingInfo->OemBadgingString)
         );
  
  gRT->SetVariable (
         L"OemBadgingInfoBadgingData",
         &gEfiGlobalVariableGuid,
         EFI_VARIABLE_BOOTSERVICE_ACCESS,
         sizeof (OemBadgingInfo->BadgingData),
         &(OemBadgingInfo->BadgingData)
         );

  gRT->SetVariable (
         L"OemBadgingInfoInstCount",
         &gEfiGlobalVariableGuid,
         EFI_VARIABLE_BOOTSERVICE_ACCESS,
         sizeof (OemBadgingInfo->InstanceCount),
         &(OemBadgingInfo->InstanceCount)
         );
  
  Status = gBS->LocateProtocol (
                  &gExitAtAuthProtocolGuid, 
                  NULL, 
                  (VOID **)&IntelATData
                  );

  StrCpy(IntelATData->VendorString,VendorString);    

  ///
  /// Override:
  ///    GetOemString -> GetOemStringHook
  ///    GetStringCount -> GetStringCountHook
  ///
  Badging->GetOemString = GetOemStringHook;
  Badging->GetStringCount = GetStringCountHook;

  InitializeStringSupport ();
  
  ASSERT_EFI_ERROR (Status);
}



/**
  Get Number of string that should be shown on screen

  @param none

  @retval EFI_SUCCESS            Always return EFI_SUCCESS.
  
**/
EFI_STATUS
InitializeStringSupport (
  VOID
  )
{
  EFI_HANDLE                             DriverHandle;

  DriverHandle = NULL;
  gAtBadgingStringsHandle = HiiAddPackages (
                              &gAtBadgingStringsGuid,
                              DriverHandle,
                              AtBadgingStrings,
                              NULL
                              );
  ASSERT (gAtBadgingStringsHandle != NULL);
  return EFI_SUCCESS;
}



/**
  Get string by string id from HII Interface

  @param [in] Id                  String ID.

  @retval String from ID.
  
**/
CHAR16 *
GetStringById (
  IN EFI_STRING_ID   Id
  )
{
  CHAR16            *String;

  String = HiiGetString (gAtBadgingStringsHandle, Id, NULL);
  return String;
}


/**
  Get String information which should be shown on screen

  @param [in]  This               Protocol instance pointer.
  @param [in]  Index              String Index of mOemBadgingString array
  @param [in]  AfterSelect        Not use any more
  @param [in]  SelectedStringNum  String number 
                                  High nibble: Type 
                                  Low nibble : Index of string Type
  @param [out] StringData         String data to output on screen
  @param [out] CoordinateX        String data location on screen
  @param [out] CoordinateY        String data location on screen
  @param [out] Foreground         Foreground color information of the string
  @param [out] Background         Background color information of the string

  @retval TRUE                    Preferred string has be gotten
  @retval FALSE                   Cannot find preferred string.
  
**/
BOOLEAN
EFIAPI
GetOemStringHook (
  IN     EFI_OEM_BADGING_SUPPORT_PROTOCOL  *This,
  IN     UINTN                             Index,
  IN     BOOLEAN                           AfterSelect,
  IN     UINT8                             SelectedStringNum,
  OUT    CHAR16                            **StringData,
  OUT    UINTN                             *CoordinateX,
  OUT    UINTN                             *CoordinateY,
  OUT    EFI_UGA_PIXEL                     *Foreground,
  OUT    EFI_UGA_PIXEL                     *Background
  )
{

  EFI_STATUS          Status;
  UINTN               *StringCountArray;
  BOOLEAN             Flag = TRUE;
  OEM_BADGING_INFO    *OemBadgingInfo;
  OEM_BADGING_STRING  *OemBadgingString;
  
  OemBadgingString = NULL;  
  OemBadgingInfo = EFI_OEM_BADGING_INFO_FROM_THIS (This);
  OemBadgingInfo->BadginStringCount = mStringCountArray[TOP_SCREEN_STRINGS-1];;
  OemBadgingInfo->BadgingData = mBadgingData;

#ifdef OEM_LOGO_SUPPORT
  OemBadgingInfo->InstanceCount = sizeof (mBadgingData) / sizeof (EFI_OEM_BADGING_LOGO_DATA);  
#else
  OemBadgingInfo->InstanceCount = 1;  
#endif

  OemBadgingInfo->OemBadgingString = mUserRecoverySelected;
  Status = gBS->AllocatePool (
                      EfiBootServicesData,
                      sizeof (UINTN),
                      (VOID **)&StringCountArray
                      );
  
  *StringCountArray = (UINTN)mStringCountArray;  
  OemBadgingString = GetOemString(
                              This,
                              Index,
                              AfterSelect,
                              SelectedStringNum,
                              StringData,
                              CoordinateX,
                              CoordinateY,
                              Foreground,
                              Background,
                              OemBadgingInfo,
                              StringCountArray,
                              mStringArray,
                              &PassWordTypeflag1,
                              &PassWordTypeflag2
                              );
  
  if (OemBadgingString[Index].Fun == NULL) {
    *StringData = GetStringById (OemBadgingString[Index].StringToken);

    if (*StringData != NULL) {
      return TRUE;
    }
    return FALSE;
  }
  return Flag;
}

