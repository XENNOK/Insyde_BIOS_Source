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

#include "AtBadgingString.h"
#include <Protocol/ExitAtAuth.h>

BOOLEAN
MergeTheftTriggerMessage(
  IN  OEM_BADGING_STRING               *Structure,
  OUT CHAR16                           **StringData
);

BOOLEAN
MergeTimeLeftMessage(
  IN OEM_BADGING_STRING  *Structure,
  OUT CHAR16             **StringData
);

BOOLEAN
MergeIsvString(
  IN OEM_BADGING_STRING  *Structure,
  OUT CHAR16             **StringData
);

BOOLEAN
MergePlatformId(
  IN OEM_BADGING_STRING  *Structure,
  OUT CHAR16             **StringData
);

BOOLEAN
MergeIsvId(
  IN OEM_BADGING_STRING  *Structure,
  OUT CHAR16             **StringData
);

BOOLEAN
MergeUniCodeNonce(
  IN OEM_BADGING_STRING  *Structure,
  OUT CHAR16             **StringData
);

BOOLEAN
EnterSystemPassword(
  IN OEM_BADGING_STRING  *Structure,
  OUT CHAR16             **StringData
);

BOOLEAN
EnterServerRecovery(
  IN OEM_BADGING_STRING  *Structure,
  OUT CHAR16             **StringData
);

BOOLEAN
ShowSystemPassword(
  IN OEM_BADGING_STRING  *Structure,
  OUT CHAR16             **StringData
);

BOOLEAN
UpdateLocationY(
  IN OEM_BADGING_STRING  *Structure,
  OUT CHAR16             **StringData
);

BOOLEAN
EnterServerAddress(
  IN OEM_BADGING_STRING  *Structure,
  OUT CHAR16             **StringData
);


//
// Please note that # of item in OEM_BADGING_STRING CAN NOT exceed 16
// Because only lower nibble (4 bits) are used to identify each item.
//
OEM_BADGING_STRING mIsvScreen[] = {
  //
  // OEM can modify the background and foreground color of the OEM dadging string through through the below data
  // for example:
  //   { 50, 280, { 0x00, 0x00, 0x00, 0x00 }, { 0xFF, 0xFF, 0xFF, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_CPUID ), GetCpuId },
  //     { 0x00, 0x00, 0x00, 0x00 } indicate the foreground color { Blue, Green, Red, Reserved }
  //     { 0xFF, 0xFF, 0xFF, 0x00 } indicate the background color { Blue, Green, Red, Reserved }
  //
 
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 500, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_LOCKED_MESSAGE ), NULL},  
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 525, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_IF_FOUND_MESSAGE ), NULL},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 575, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_NULL ), MergeIsvString},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 600, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_PLATFROM_ID ), MergePlatformId},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 625, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_SERVICE_ID ), MergeIsvId}
};


OEM_BADGING_STRING mUserRecoverySelected[] = {
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y,       { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_INTEL_AT_SYSTEM_LOCK ), MergeTheftTriggerMessage },
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 25,  { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_TIME_LEFT_INPUT ), NULL},
  {       OEM_STRING_LOCATION_X + 200, OEM_STRING1_LOCATION_Y + 25,  { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_NULL ), MergeTimeLeftMessage},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 50,  { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_RECOVERY_MESSAGE ), NULL},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 75,  { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_RECOVERY_USER_PASSWORD ), NULL},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 100, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_RECOVERY_SERVER_TOKEN ), NULL},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 125, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_RECOVERY_WWAN_UNLOCK ), NULL},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 150, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_RECOVERY_SELECT_OPTION ), NULL},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 200, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_USER_PASSWORD_SELECTED ), NULL},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 200, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_SERVER_TOKEN_SELECTED ), NULL},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 200, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_WWAN_RECOVERY_SELECTED ), NULL},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 200, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_INVALID_SELECTION_123 ), NULL},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 200, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_INVALID_SELECTION_12 ), NULL},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 50,  { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_RECOVERY_ID ), MergeUniCodeNonce },
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 50,  { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_ENTER_SYSTEM_PASSWORD ), EnterSystemPassword},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 75,  { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_ENTER_SCREEN_RECOVERY ), EnterServerRecovery}
};


OEM_BADGING_STRING mUserShowNullSystemPassword[] = {
  {       OEM_STRING_LOCATION_X + 260, OEM_STRING1_LOCATION_Y + 50,  { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN (STR_SPACE_NULL), ShowSystemPassword}
};


OEM_BADGING_STRING mUserShowPassword[] = {
  {       OEM_STRING_LOCATION_X + 260, OEM_STRING1_LOCATION_Y + 50,  { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN (STR_NULL), ShowSystemPassword},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 75,  { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN (STR_USER_SHOW_ESCAPED), UpdateLocationY},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 75,  { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN (STR_USER_CHECK_PASSWORD), UpdateLocationY},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 75,  { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN (STR_USER_ENTER_EXCEEDED), UpdateLocationY},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 75,  { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN (STR_USER_ENTER_EXCEEDED2), UpdateLocationY},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 125, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN (STR_NO_PASSWORD_ENTERED), NULL},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 125, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN (STR_WRONG_PASSWORD_ENTERED), NULL},  
};


OEM_BADGING_STRING mUserTimeLeftMessage[] = {
  {       OEM_STRING_LOCATION_X + 300, OEM_STRING1_LOCATION_Y + 25,  { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_NULL ), MergeTimeLeftMessage}
};


OEM_BADGING_STRING mUserInitializingWWANInterface[] = {
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 25,  { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_INITIAL_WWAN_INTERFACE ), NULL },
  {       OEM_STRING_LOCATION_X + 300, OEM_STRING1_LOCATION_Y + 25,  { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_INITIAL_WWAN_FAIL ), NULL },
  {       OEM_STRING_LOCATION_X + 300, OEM_STRING1_LOCATION_Y + 25,  { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_INITIAL_WWAN_COMPLETE ), NULL },
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 50,  { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_WAIT_RADIO_NETWORK ), NULL },
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 75,  { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_INITIAL_WWAN_DOWN), NULL },
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 75,  { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_INITIAL_WWAN_SUCCESS), NULL },
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 100, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_WWAN_CHECK_SIM_RADIO), NULL },
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 100, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_WAIT_SMS_RECOVERY ), NULL},
  {       OEM_STRING_LOCATION_X + 260, OEM_STRING1_LOCATION_Y + 100, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_WWAN_TIMEOUT ), NULL},
  {       OEM_STRING_LOCATION_X + 260, OEM_STRING1_LOCATION_Y + 100, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_INITIAL_WWAN_COMPLETE ), NULL},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 150, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_WWAN_RECOVERY_SUCCESS ), NULL},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 150, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_WWAN_RECOVERY_FAIL ), NULL},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 200, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_INTELAT_RECOVERY_FAIL ), NULL},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 225, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_SYSTEM_SHUTDOWN ), NULL},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 200, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_INTELAT_RECOVERY_SUCCESS ), NULL},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 225, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_SYSTEM_BOOT ), NULL}
};


//
// using "        "(empty line) to clear the screen text
//
OEM_BADGING_STRING mClearTopScreenString[] = {
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y,       { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_CLEAR_MAXLINE ), NULL },
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 25,  { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_CLEAR_MAXLINE ), NULL},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 50,  { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_CLEAR_MAXLINE ), NULL},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 75,  { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_CLEAR_MAXLINE ), NULL},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 100, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_CLEAR_MAXLINE ), NULL},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 125, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_CLEAR_MAXLINE ), NULL},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 150, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_CLEAR_MAXLINE ), NULL},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 175, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_CLEAR_MAXLINE ), NULL},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 200, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_CLEAR_MAXLINE ), NULL}
};

//
// using "        "(empty line) to clear the screen text
//
OEM_BADGING_STRING mClearBottomScreenString[] = {
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 625, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_CLEAR_LINE ), NULL},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 650, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_CLEAR_LINE ), NULL},
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 675, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_CLEAR_LINE ), NULL}
};


OEM_BADGING_STRING mShowSuspendMode[] = {
  {       OEM_STRING_LOCATION_X,   OEM_STRING1_LOCATION_Y,       { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN (STR_AT_SUSPEND_STATE), NULL},
  {       OEM_STRING_LOCATION_X,   OEM_STRING1_LOCATION_Y + 25,  { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN (STR_EXIT_SUSPEND_MODE), NULL},
  {       OEM_STRING_LOCATION_X,   OEM_STRING1_LOCATION_Y + 25,  { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN (STR_STAY_SUSPEND_MODE), NULL},
  {       OEM_STRING_LOCATION_X,   OEM_STRING1_LOCATION_Y,       { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN (STR_REQUEST_SUSPEND_MODE), NULL},
  {       OEM_STRING_LOCATION_X,   OEM_STRING1_LOCATION_Y + 50,  { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN (STR_ERROR_NONCE_FW), NULL},
  {       OEM_STRING_LOCATION_X,   OEM_STRING1_LOCATION_Y + 50,  { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN (STR_RECOVERY_ID), MergeUniCodeNonce},
  {       OEM_STRING_LOCATION_X,   OEM_STRING1_LOCATION_Y + 75,  { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_ENTER_SERVER_SUSPEND ), NULL},
  {       OEM_STRING_LOCATION_X,   OEM_STRING1_LOCATION_Y + 100, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_CHECK_AUTHENTICATION ), NULL},
  {       OEM_STRING_LOCATION_X,   OEM_STRING1_LOCATION_Y + 150, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN (STR_FAIL_AUTHENTICATE), NULL},
  {       OEM_STRING_LOCATION_X,   OEM_STRING1_LOCATION_Y + 150, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN (STR_FAIL_INTO_SUSPEND), NULL},
  {       OEM_STRING_LOCATION_X,   OEM_STRING1_LOCATION_Y + 150, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN (STR_SUCCESS_INTO_SUSPEND), NULL},
  {       OEM_STRING_LOCATION_X,   OEM_STRING1_LOCATION_Y + 175, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN (STR_FAIL_AUTHENTICATE), NULL},
  {       OEM_STRING_LOCATION_X,   OEM_STRING1_LOCATION_Y + 175, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN (STR_SET_SUSPEND_FAIL), NULL},
  {       OEM_STRING_LOCATION_X,   OEM_STRING1_LOCATION_Y + 175, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN (STR_SUCCESS_EXIT_SUSPEND), NULL},
  {       OEM_STRING_LOCATION_X,   OEM_STRING1_LOCATION_Y + 175, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN (STR_EXCEEDED_MAX_ATTEMPT), NULL},  
  {       OEM_STRING_LOCATION_X,   OEM_STRING1_LOCATION_Y + 75,  { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_ENTER_SERVER_RESUME ), NULL}
};

OEM_BADGING_STRING mServerAddress[] = {
  {       OEM_STRING_LOCATION_X,   OEM_STRING1_LOCATION_Y + 125, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN (STR_ENTER_ADDRESS), EnterServerAddress}
};


OEM_BADGING_STRING mSystemDetectingWWAN[] = {
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 125, { 0x68, 0x68, 0x68, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_WWAN_NOT_DETECT ), NULL },
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 125, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_WWAN_DETECT_PENDING ), NULL },
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 125, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_WWAN_DETECTED ), NULL },
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 125, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_WWAN_UNEXPECTED_STATE ), NULL },
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y + 200, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_CLEAR_MAXLINE ), NULL }
};



UINTN mStringCountArray[] = {
  TOP_SCREEN_STRINGS_COUNT,
  BOTTOM_SCREEN_STRINGS_COUNT
 };

//
// Add comment to map typedef {EFI_SHOW_TYPE} in AtGraphicLib.h
// When new OEM_BADGING_STRING string array added, 
// coresponsed {EFI_SHOW_TYPE} also need to add a new type.
//
OEM_BADGING_STRING *mStringArray[] = {
  mClearTopScreenString,                // map to TOP_SCREEN_STRINGS
  mClearBottomScreenString,             // map to BOTTOM_SCREEN_STRINGS
  mUserShowNullSystemPassword,          // map to SYSTEM_PASSWORD_STRINGS
  mUserRecoverySelected,                // map to SYSTEM_RECOVERY_STRINGS
  mUserInitializingWWANInterface,       // map to INITIALWWAN_STRINGS
  mUserShowPassword,                    // map to SHOW_PASSWORD_STRINGS
  mShowSuspendMode,                     // map to SHOW_SUSPEND_MODE_STRINGS
  mUserTimeLeftMessage,                 // map to USER_TIMER_LEFT_STRINGS
  mIsvScreen,                           // map to ISV_SCREEN_STRINGS
  mServerAddress,                       // map to SERVER_ADDRESS_STRINGS
  mSystemDetectingWWAN                  // map to DETECT_WWAN_STRINGS
};


//
// The array is offset with X-coordinate for different language
//   ShowTimeOffsetX,      (p1)- display time left in recovery screen
//   ShowPasswordOffsetX1, (p2)- display key-in Password in recovery screen
//   ShowPasswordOffsetX2, (p3)- display key-in token in resume
//   ShowPasswordOffsetX3  (p4)- display key-in token in recovery screen
//
// Note: the order of language sequence SHOULD MAP with order of NVRam->Language.
//       Otherwise, LanguageOffset does not match.
//
LANGUAGE_COORDINATE_OFFSET LanguageOffset[]= { {110, 130, 135, 105},     // English
                                               {215, 225, 195, 175},     // French
                                               { 10, 105,  70, 120},     // Chinese
                                               {100, 210, 215, 155},     // Japanese
                                               {210, 175, 290, 245},     // German
                                               {240, 330, 310, 255},     // Spanish
                                               {220, 205, 160, 160},     // Italian
                                               {135, 205, 190, 195},     // Portuguese
                                               };

UINTN      PassWordLocationY = 0;
UINTN      PassWordTypeflag1 = 0;
UINTN      PassWordTypeflag2 = 0;
BOOLEAN    OverLineString = TRUE;

BOOLEAN
MergeTheftTriggerMessage(
  IN OEM_BADGING_STRING  *Structure,
  OUT CHAR16             **StringData
)
/*++

Routine Description:
  
  Merge TheftTrigger Message
    
Arguments:

  Structure : Pointer to OEM_BADGING_STRING
  StringData: Poniter to StringToken
    
Returns:

  TRUE : Merge TheftTrigger Message Successfully 

--*/
{
  EFI_STATUS                            Status;
  EFI_EXIT_AT_PROTOCOL                  *IntelATData; 
  CHAR16                                *Str2;
  UINT8                                 Index;

EFI_STRING_ID TheftTriggerString[] = {
  {        STRING_TOKEN ( STR_DTIMER_EXPIRED )},
  {        STRING_TOKEN ( STR_STOLEN_RECEIVED )},
  {        STRING_TOKEN ( STR_LOGON_EXCEEDED )},
  {        STRING_TOKEN ( STR_PLATFORM_ATTACK )},
  {        STRING_TOKEN ( STR_UNKNOWN )}};

  Str2 = AllocateZeroPool (100);
  Status = gBS->LocateProtocol (&gExitAtAuthProtocolGuid, NULL, (VOID **)&IntelATData);
  *StringData = GetStringById (Structure->StringToken);
  Index = IntelATData->AtLastTheftTrigger;

  if (Index < 5) {
    Str2 = GetStringById (TheftTriggerString[Index - 1]);
  } else {
    Str2 = GetStringById (TheftTriggerString[4]);
  }
  
  StrCat (*StringData, Str2);
  gBS->FreePool (Str2);

  return TRUE;
}


BOOLEAN
MergeTimeLeftMessage(
  IN OEM_BADGING_STRING  *Structure,
  OUT CHAR16             **StringData
)
/*++

Routine Description:

  Merge Time Left Message
    
Arguments:

  Structure : Pointer to OEM_BADGING_STRING
  StringData: Poniter to StringToken
    
Returns:

  TRUE : Merge Time Left Message Successfully 

--*/
{
  EFI_STATUS            Status;
  EFI_EXIT_AT_PROTOCOL  *IntelATData; 
  CHAR16                *StrSec;
  CHAR16                *StrMin;
  CHAR16                *StrHr;
  UINT16                Hr;
  UINT16                Min;
  UINT16                Sec;


  EFI_STRING_ID TimeString[] = {
                             {STRING_TOKEN (STR_TIME_SEC)},
                             {STRING_TOKEN (STR_TIME_MIN)},
                             {STRING_TOKEN (STR_TIME_HOUR)}
                            };
 
  StrSec = AllocateZeroPool (100);
  StrMin = AllocateZeroPool (100);
  StrHr  = AllocateZeroPool (100);
  Status = gBS->LocateProtocol (&gExitAtAuthProtocolGuid, NULL, (VOID **)&IntelATData);
  *StringData = GetStringById (Structure->StringToken);

  Sec = (IntelATData->TimeLeft) % 60;
  Min = ((UINT16)IntelATData->TimeLeft / 60) % 60;
  Hr  = (UINT16)IntelATData->TimeLeft / 3600;
  
  UnicodeValueToString (StrSec,Sec, COMMA_TYPE, 0);
  UnicodeValueToString (StrMin,Min, COMMA_TYPE, 0);
  UnicodeValueToString (StrHr,Hr, COMMA_TYPE, 0);

  //
  // Only display Hour when number of hour > 0 
  // 
  if (Hr > 0) {
    StrCat (*StringData, StrHr);
    StrHr = GetStringById (TimeString[2]);
    StrCat (*StringData, StrHr);

    //
    // attach " "(empty space) to align with "Min/Sec" prompt word
    //
    if (Min < 10) {
      StrCat (*StringData, L" ");
    }
    StrCat (*StringData, StrMin);
    StrMin = GetStringById (TimeString[1]);
    StrCat (*StringData, StrMin);

    if (Sec < 10) {
      StrCat (*StringData, L" ");
    }
    StrCat (*StringData, StrSec);
    StrSec = GetStringById (TimeString[0]);
    StrCat (*StringData, StrSec);
    
  } else {
    StrCat (*StringData, L"     ");
    
    if (Min > 0) {
      if (Min < 10) {
        StrCat (*StringData, L" ");
      }
      StrCat (*StringData, StrMin);
      StrMin = GetStringById (TimeString[1]);
      StrCat (*StringData, StrMin);
      
      if (Sec < 10) {
        StrCat (*StringData, L" ");
      }
      StrCat (*StringData, StrSec);
      StrSec = GetStringById (TimeString[0]);
      StrCat (*StringData, StrSec);      
      
    } else {
      StrCat (*StringData, L"       ");
      //
      // No min left, do not display it.
      // Only display sec.
      //    
      if (Sec < 10) {
        StrCat (*StringData, L" ");
      }
      StrCat (*StringData, StrSec);
      StrSec = GetStringById (TimeString[0]);
      StrCat (*StringData, StrSec);
    }
  }
  
  gBS->FreePool (StrSec);
  gBS->FreePool (StrMin);
  gBS->FreePool (StrHr);
  Structure->X = OEM_STRING_LOCATION_X + 150 + LanguageOffset[IntelATData->Language].ShowTimeOffsetX;

  return TRUE;
}


BOOLEAN
MergeIsvId(
  IN OEM_BADGING_STRING  *Structure,
  OUT CHAR16             **StringData
)
/*++

Routine Description:

  Merge Isv Id string
    
Arguments:

  Structure : Pointer to OEM_BADGING_STRING
  StringData: Poniter to StringToken
    
Returns:

  TRUE : Merge Isv Id String Successfully 

--*/
{
  EFI_STATUS            Status;
  EFI_EXIT_AT_PROTOCOL  *IntelATData; 
  CHAR16                *Str2;

  Str2 = AllocateZeroPool (100);

  Status = gBS->LocateProtocol (&gExitAtAuthProtocolGuid, NULL, (VOID **)&IntelATData);
  *StringData = GetStringById (Structure->StringToken);

  UnicodeValueToString (Str2,(INT64)IntelATData->IsvId, COMMA_TYPE, 0);
  StrCat (*StringData, Str2);

  gBS->FreePool (Str2);

  return TRUE;
}

BOOLEAN
MergePlatformId(
  IN OEM_BADGING_STRING  *Structure,
  OUT CHAR16             **StringData
)
/*++

Routine Description:

  Merge Platform Id string
    
Arguments:

  Structure : Pointer to OEM_BADGING_STRING
  StringData: Poniter to StringToken
    
Returns:

  TRUE : Merge Platform Id String Successfully 

--*/
{
  EFI_STATUS            Status;
  EFI_EXIT_AT_PROTOCOL  *IntelATData;
  
  Status = gBS->LocateProtocol (&gExitAtAuthProtocolGuid, NULL, (VOID **)&IntelATData);
  *StringData = GetStringById (Structure->StringToken);
  StrCat (*StringData, IntelATData->IsvPlatformId);

  return TRUE;
}



BOOLEAN
MergeIsvString(
  IN OEM_BADGING_STRING  *Structure,
  OUT CHAR16             **StringData
)
/*++

Routine Description:

  Merge Isv string
    
Arguments:

    Structure : Pointer to OEM_BADGING_STRING
    StringData: Poniter to StringToken
    
Returns:

    TRUE : Merge Isv String Successfully 

--*/
{
  EFI_STATUS            Status;
  EFI_EXIT_AT_PROTOCOL  *IntelATData; 
  CHAR16                TmepString[128];
  UINTN                 StringLen;

  ZeroMem (TmepString, (sizeof(CHAR16) * 128));
  Status = gBS->LocateProtocol (&gExitAtAuthProtocolGuid, NULL, (VOID **)&IntelATData); 
  StringLen = StrLen (IntelATData->IsvString);
  *StringData = GetStringById (Structure->StringToken);

  if (StringLen > MAX_STRING_LANE) {
    StrCat (TmepString, IntelATData->IsvString);

    if (OverLineString) {
      StrnCpy (*StringData, TmepString, MAX_STRING_LANE);
      Structure->Y = 650; 
      OverLineString = FALSE;
    } else {
      StrnCpy (*StringData, &TmepString[MAX_STRING_LANE], StringLen - MAX_STRING_LANE);
      Structure->Y = 675;
      OverLineString = TRUE;
    }
  } else {
    StrCat (*StringData, IntelATData->IsvString);
  }
  return TRUE;
}


BOOLEAN
MergeUniCodeNonce(
  IN OEM_BADGING_STRING  *Structure,
  OUT CHAR16             **StringData
)
/*++

Routine Description:

  Merge UniCodeNonce string
    
Arguments:

  Structure : Pointer to OEM_BADGING_STRING
  StringData: Poniter to StringToken
    
Returns:

  TRUE : Merge UniCodeNonce String Successfully 

--*/
{
  EFI_STATUS            Status;
  EFI_EXIT_AT_PROTOCOL  *IntelATData; 
  
  Status = gBS->LocateProtocol (&gExitAtAuthProtocolGuid, NULL, (VOID **)&IntelATData); 
  *StringData = GetStringById (Structure->StringToken);  
  StrCat (*StringData, IntelATData->UniCodeNonceStr);

  return TRUE;
}


BOOLEAN
EnterSystemPassword(
  IN OEM_BADGING_STRING  *Structure,
  OUT CHAR16             **StringData
)
/*++
Routine Description:

   Get System Password string
  
Arguments:

  Structure : Pointer to OEM_BADGING_STRING
  StringData: Poniter to StringToken
  
Returns:

  TRUE : Get System Password String Successfully
  
--*/
{
  *StringData = GetStringById (Structure->StringToken);
  PassWordTypeflag1 = SYSTEM_PASSWORD_STRINGS;
  PassWordTypeflag2 = SYSTEM_PASSWORD_STRINGS;
  
  return TRUE;
}


BOOLEAN
EnterServerRecovery(
  IN OEM_BADGING_STRING  *Structure,
  OUT CHAR16             **StringData
)
/*++

Routine Description:

  Get Server Recovery string
  
Arguments:

  Structure : Pointer to OEM_BADGING_STRING
  StringData: Poniter to StringToken
  
Returns:

  TRUE : Get Server Recovery String Successfully 

--*/
{
  *StringData = GetStringById (Structure->StringToken);
  PassWordTypeflag1 = SYSTEM_RECOVERY_STRINGS;
  PassWordTypeflag2 = SYSTEM_RECOVERY_STRINGS;
  
  return TRUE;
}


BOOLEAN
ShowSystemPassword(
  IN OEM_BADGING_STRING  *Structure,
  OUT CHAR16             **StringData
)
/*++

Routine Description:

  Show SystemPassword string
  
Arguments:

  Structure : Pointer to OEM_BADGING_STRING
  StringData: Poniter to StringToken
  
Returns:

  TRUE : Get Show SystemPassword String Successfully 
  
--*/
{
  EFI_STATUS            Status;
  EFI_EXIT_AT_PROTOCOL  *IntelATData;
  
  Status = gBS->LocateProtocol (&gExitAtAuthProtocolGuid, NULL, (VOID **)&IntelATData);
  *StringData = GetStringById (Structure->StringToken);
  
  if (IntelATData->StrIndex >= IntelATData->LastStrIndex) {
    StrCat (*StringData, &(IntelATData->UniCodeStr));
  }
  
  Structure->Y = PassWordLocationY;
  
  switch (PassWordTypeflag1) {
    case SYSTEM_PASSWORD_STRINGS:
        Structure->X = OEM_STRING_LOCATION_X + 126 + LanguageOffset[IntelATData->Language].ShowPasswordOffsetX1;
        if (PassWordTypeflag2 == SYSTEM_PASSWORD_STRINGS) {
          Structure->Y = OEM_STRING1_LOCATION_Y + 50;
        }
    break;
    
    case SHOW_SUSPEND_MODE_STRINGS:
        Structure->X = OEM_STRING_LOCATION_X + 126 + LanguageOffset[IntelATData->Language].ShowPasswordOffsetX2;
        if ((PassWordTypeflag2 == SYSTEM_RECOVERY_STRINGS) || (PassWordTypeflag2 == SHOW_SUSPEND_MODE_STRINGS)) {
          Structure->Y = OEM_STRING1_LOCATION_Y+75;
        }
    break;
    
    case SYSTEM_RECOVERY_STRINGS:
        Structure->X = OEM_STRING_LOCATION_X + 126 + LanguageOffset[IntelATData->Language].ShowPasswordOffsetX3;
        if ((PassWordTypeflag2 == SYSTEM_RECOVERY_STRINGS) || (PassWordTypeflag2 == SHOW_SUSPEND_MODE_STRINGS)) {
          Structure->Y = OEM_STRING1_LOCATION_Y + 75;
        }
    break;
    
    case SERVER_ADDRESS_STRINGS:      
        Structure->X = OEM_STRING_LOCATION_X + 240;
        if (PassWordTypeflag2 == SERVER_ADDRESS_STRINGS) {
          Structure->Y = OEM_STRING1_LOCATION_Y + 125;
        }      
    break;
  }
  
  IntelATData->LastStrIndex = IntelATData->StrIndex;
  
  return TRUE;
}


BOOLEAN
UpdateLocationY(
  IN OEM_BADGING_STRING  *Structure,
  OUT CHAR16             **StringData
)
/*++

Routine Description:

   Update LocationY of string
  
Arguments:

  Structure : Pointer to OEM_BADGING_STRING
  StringData: Poniter to StringToken
  
Returns:

  TRUE : Update LocationY of String Successfully
  
--*/

{
  *StringData = GetStringById (Structure->StringToken);
  
  switch (PassWordTypeflag2) {
    case SYSTEM_RECOVERY_STRINGS:
         Structure->Y = OEM_STRING1_LOCATION_Y + 100;
    break;
    
    case SHOW_SUSPEND_MODE_STRINGS:
         Structure->Y = OEM_STRING1_LOCATION_Y + 75;
    break;
    
    case SERVER_ADDRESS_STRINGS: 
         Structure->Y = OEM_STRING1_LOCATION_Y + 125;
    break;
  }
  PassWordTypeflag2 = 0;

  return TRUE;
}


BOOLEAN
EnterServerAddress(
  IN OEM_BADGING_STRING  *Structure,
  OUT CHAR16             **StringData
)
/*++

Routine Description:

  Get Enter Server Address string
  
Arguments:

  Structure : Pointer to OEM_BADGING_STRING
  StringData: Poniter to StringToken
  
Returns:

  TRUE : Get Enter Server Address string Successfully
  
--*/
{
  *StringData = GetStringById (Structure->StringToken);
  PassWordTypeflag1 = SERVER_ADDRESS_STRINGS;
  PassWordTypeflag2 = SERVER_ADDRESS_STRINGS;
  
  return TRUE;
}


