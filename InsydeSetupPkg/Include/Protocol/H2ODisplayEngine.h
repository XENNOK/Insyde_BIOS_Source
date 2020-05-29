/** @file
  H2O Display Engine Protocol Header

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _H2O_DISPLAY_ENGINE_PROTOCOL_H_
#define _H2O_DISPLAY_ENGINE_PROTOCOL_H_

#include <Protocol/H2OFormBrowser.h>

#define H2O_DISPLAY_ENGINE_PROTOCOL_GUID \
  { \
   0xb1af258f, 0x1e72, 0x4c7a, 0x80, 0x3f, 0x43, 0x5e, 0x49, 0x03, 0x3d, 0xbb \
  }

typedef struct _H2O_DISPLAY_ENGINE_PROTOCOL H2O_DISPLAY_ENGINE_PROTOCOL;

//
// Protocol definitions
//
/**
  Add new console to the console list.

  @param[in] This                A pointer to the H2O_DISPLAY_ENGINE_PROTOCOL instance.
  @param[in] ConsoleDev          A pointer to input H2O_FORM_BROWSER_CONSOLE_DEV instance.

  @retval EFI_SUCCESS            Add new console to console list successful.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_DISPLAY_ENGINE_INIT_CONSOLE) (
  IN     H2O_DISPLAY_ENGINE_PROTOCOL       *This,
  IN     H2O_FORM_BROWSER_CONSOLE_DEV      *ConsoleDev
  );

/**
  Attach a specific console to this display engine.

  @param[in] This                A pointer to the H2O_DISPLAY_ENGINE_PROTOCOL instance.
  @param[in] ConsoleDev          A pointer to input H2O_FORM_BROWSER_CONSOLE_DEV instance.

  @retval EFI_SUCCESS            Attach new console successful.
  @retval EFI_INVALID_PARAMETER  This is NULL, or ConsoleDev is NULL.
  @retval EFI_UNSUPPORTED        This display engine doesn't support input console device.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_DISPLAY_ENGINE_ATTACH_CONSOLE) (
  IN     H2O_DISPLAY_ENGINE_PROTOCOL       *This,
  IN     H2O_FORM_BROWSER_CONSOLE_DEV      *ConsoleDev
  );


/**
  Detach a specific console from this display engine.

  @param[in] This               A pointer to the H2O_DISPLAY_ENGINE_PROTOCOL instance.
  @param[in] ConsoleDev         A pointer to input H2O_FORM_BROWSER_CONSOLE_DEV instance.

  @retval EFI_SUCCESS            Detach a console device from the device engine successful.
  @retval EFI_INVALID_PARAMETER  This is NULL, or ConsoleDev is NULL.
  @retval EFI_NOT_FOUND          The input device console isn't attached to the display engine.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_DISPLAY_ENGINE_DETACH_CONSOLE) (
  IN     H2O_DISPLAY_ENGINE_PROTOCOL       *This,
  IN     H2O_FORM_BROWSER_CONSOLE_DEV      *ConsoleDev
  );


/**
  Check if input console device is supported by this display engine.

  @param[in] This                A pointer to the H2O_DISPLAY_ENGINE_PROTOCOL instance.
  @param[in] ConsoleDev          A pointer to input H2O_FORM_BROWSER_CONSOLE_DEV instance.

  @retval EFI_SUCCESS            This display engine supports input console device.
  @retval EFI_UNSUPPORTED        This display engine doesn't support input console device.
  @retval EFI_INVALID_PARAMETER  This is NULL or ConsoleDev is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_DISPLAY_ENGINE_SUPPORT_CONSOLE) (
  IN     H2O_DISPLAY_ENGINE_PROTOCOL       *This,
  IN     H2O_FORM_BROWSER_CONSOLE_DEV      *ConsoleDev
  );

/**
  Add the notification to the notification queue and signal the Notification event.

  @param[in] This                A pointer to the H2O_DISPLAY_ENGINE_PROTOCOL instance.
  @param[in] Notify              A pointer to the H2O_DISPLAY_ENGINE_EVT instance.

  @retval EFI_SUCCESS            Register notify successful.
  @retval EFI_INVALID_PARAMETER  This is NULL or Notify is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_DISPLAY_ENGINE_NOTIFY) (
  IN       H2O_DISPLAY_ENGINE_PROTOCOL     *This,
  IN CONST H2O_DISPLAY_ENGINE_EVT          *Notify
  );

struct _H2O_DISPLAY_ENGINE_PROTOCOL {
  UINT32                              Size;
  EFI_GUID                            Id;
  H2O_DISPLAY_ENGINE_INIT_CONSOLE     InitConsole;
  H2O_DISPLAY_ENGINE_ATTACH_CONSOLE   AttachConsole;
  H2O_DISPLAY_ENGINE_DETACH_CONSOLE   DetachConsole;
  H2O_DISPLAY_ENGINE_SUPPORT_CONSOLE  SupportConsole;
  H2O_DISPLAY_ENGINE_NOTIFY           Notify;
};

extern GUID gH2ODisplayEngineProtocolGuid;

#endif
