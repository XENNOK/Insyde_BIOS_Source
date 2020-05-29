/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/*++

Copyright (c) 2007, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

  SimpleTextInputEx.h

Abstract:

  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL from the UEFI 2.1 specification.

  This protocol defines an extension to the EFI_SIMPLE_TEXT_INPUT_PROTOCOL
  which exposes much more state and modifier information from the input device,
  also allows one to register a notification for a particular keystroke.

--*/

#ifndef __SIMPLE_TEXT_INPUT_EX_H__
#define __SIMPLE_TEXT_INPUT_EX_H__

#include EFI_PROTOCOL_DEFINITION (SimpleTextIn)

#define EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL_GUID \
  { \
    0xdd9e7534, 0x7762, 0x4698, {0x8c, 0x14, 0xf5, 0x85, 0x17, 0xa6, 0x25, 0xaa}\
  }

EFI_FORWARD_DECLARATION (EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL);

//
// Data structures
//

typedef UINT8          EFI_KEY_TOGGLE_STATE;
// 
// Any Shift or Toggle State that is valid should have 
// high order bit set.
//
typedef struct EFI_KEY_STATE {
  UINT32               KeyShiftState;
  EFI_KEY_TOGGLE_STATE KeyToggleState;
} EFI_KEY_STATE;

typedef struct {
  EFI_INPUT_KEY        Key;      
  EFI_KEY_STATE        KeyState;   
} EFI_KEY_DATA;

//
// Shift state
//
#define EFI_SHIFT_STATE_VALID     0x80000000
#define EFI_RIGHT_SHIFT_PRESSED   0x00000001
#define EFI_LEFT_SHIFT_PRESSED    0x00000002
#define EFI_RIGHT_CONTROL_PRESSED 0x00000004
#define EFI_LEFT_CONTROL_PRESSED  0x00000008
#define EFI_RIGHT_ALT_PRESSED     0x00000010
#define EFI_LEFT_ALT_PRESSED      0x00000020
#define EFI_RIGHT_LOGO_PRESSED    0x00000040
#define EFI_LEFT_LOGO_PRESSED     0x00000080
#define EFI_MENU_KEY_PRESSED      0x00000100
#define EFI_SYS_REQ_PRESSED       0x00000200
                              
//                            
// Toggle state               
//                            
#define EFI_TOGGLE_STATE_VALID    0x80
#define EFI_KEY_STATE_EXPOSED     0x40
#define EFI_SCROLL_LOCK_ACTIVE    0x01
#define EFI_NUM_LOCK_ACTIVE       0x02
#define EFI_CAPS_LOCK_ACTIVE      0x04
                              
//                            
// EFI Scan codes             
//                            
#define SCAN_PAUSE                0x0048
#define SCAN_F13                  0x0068
#define SCAN_F14                  0x0069
#define SCAN_F15                  0x006A
#define SCAN_F16                  0x006B
#define SCAN_F17                  0x006C
#define SCAN_F18                  0x006D
#define SCAN_F19                  0x006E
#define SCAN_F20                  0x006F
#define SCAN_F21                  0x0070
#define SCAN_F22                  0x0071
#define SCAN_F23                  0x0072
#define SCAN_F24                  0x0073
#define SCAN_MUTE                 0x007F
#define SCAN_VOLUME_UP            0x0080
#define SCAN_VOLUME_DOWN          0x0081
#define SCAN_BRIGHTNESS_UP        0x0100
#define SCAN_BRIGHTNESS_DOWN      0x0101
#define SCAN_SUSPEND              0x0102
#define SCAN_HIBERNATE            0x0103
#define SCAN_TOGGLE_DISPLAY       0x0104
#define SCAN_RECOVERY             0x0105
#define SCAN_EJECT                0x0106


//
// EFI Key Notfication Function
//
typedef 
EFI_STATUS
(EFIAPI *EFI_KEY_NOTIFY_FUNCTION) (
  IN  EFI_KEY_DATA                      *KeyData
  );

/**
 Reset the input device and optionaly run diagnostics

 @param [in]   This             Protocol instance pointer.
 @param [in]   ExtendedVerification  Driver may perform diagnostics on reset.

 @retval EFI_SUCCESS            The device was reset.
 @retval EFI_DEVICE_ERROR       The device is not functioning properly and could
                                not be reset.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_INPUT_RESET_EX) (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN BOOLEAN                            ExtendedVerification
  );

/**
 Reads the next keystroke from the input device. The WaitForKey Event can
 be used to test for existance of a keystroke via WaitForEvent () call.


 @retval EFI_SUCCESS            The keystroke information was returned.
 @retval EFI_NOT_READY          There was no keystroke data availiable.
 @retval EFI_DEVICE_ERROR       The keystroke information was not returned due to
                                hardware errors.
 @retval EFI_INVALID_PARAMETER  KeyData is NULL.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_INPUT_READ_KEY_EX) (
  IN  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
  OUT EFI_KEY_DATA                      *KeyData
  );

/**
 Set certain state for the input device.

 @param [in]   This             Protocol instance pointer.
 @param [in]   KeyToggleState   A pointer to the EFI_KEY_TOGGLE_STATE to set the
                                state for the input device.

 @retval EFI_SUCCESS            The device state was set successfully.
 @retval EFI_DEVICE_ERROR       The device is not functioning correctly and could
                                not have the setting adjusted.
 @retval EFI_UNSUPPORTED        The device does not have the ability to set its state.
 @retval EFI_INVALID_PARAMETER  KeyToggleState is NULL.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_SET_STATE) (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN EFI_KEY_TOGGLE_STATE               *KeyToggleState
  );

/**
 Register a notification function for a particular keystroke for the input device.

 @param [in]   This             Protocol instance pointer.
 @param [in]   KeyData          A pointer to a buffer that is filled in with the keystroke
                                information data for the key that was pressed.
 @param [in]   KeyNotificationFunction  Points to the function to be called when the key
                                sequence is typed specified by KeyData.
 @param [out]  NotifyHandle     Points to the unique handle assigned to the registered notification.

 @retval EFI_SUCCESS            The notification function was registered successfully.
 @retval EFI_OUT_OF_RESOURCES   Unable to allocate resources for necesssary data structures.
 @retval EFI_INVALID_PARAMETER  KeyData or NotifyHandle is NULL.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_REGISTER_KEYSTROKE_NOTIFY) (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN EFI_KEY_DATA                       *KeyData,
  IN EFI_KEY_NOTIFY_FUNCTION            KeyNotificationFunction,
  OUT EFI_HANDLE                        *NotifyHandle
  );

/**
 Remove a registered notification function from a particular keystroke.

 @param [in]   This             Protocol instance pointer.
 @param [in]   NotificationHandle  The handle of the notification function being unregistered.

 @retval EFI_SUCCESS            The notification function was unregistered successfully.
 @retval EFI_INVALID_PARAMETER  The NotificationHandle is invalid.
 @retval EFI_NOT_FOUND          Can not find the matching entry in database.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_UNREGISTER_KEYSTROKE_NOTIFY) (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN EFI_HANDLE                         NotificationHandle
  );

struct _EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL {
  EFI_INPUT_RESET_EX                    Reset;
  EFI_INPUT_READ_KEY_EX                 ReadKeyStrokeEx;
  EFI_EVENT                             WaitForKeyEx;
  EFI_SET_STATE                         SetState;
  EFI_REGISTER_KEYSTROKE_NOTIFY         RegisterKeyNotify;
  EFI_UNREGISTER_KEYSTROKE_NOTIFY       UnregisterKeyNotify;
};

extern EFI_GUID gEfiSimpleTextInputExProtocolGuid;

#endif
