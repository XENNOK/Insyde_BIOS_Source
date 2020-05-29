//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

    OemFormBrowser.h

Abstract:

  The EFI_FORM_BROWSER_PROTOCOL is the interface to the EFI
  Configuration Driver.  This will allow the caller to direct the
  configuration driver to use either the HII database or use the passed
  in packet of data.  This will also allow the caller to post messages
  into the configuration drivers internal mailbox.

--*/

#ifndef _OEM_FORM_BROWSER_H_
#define _OEM_FORM_BROWSER_H_

#include EFI_PROTOCOL_DEFINITION (Hii)
#include EFI_PROTOCOL_DEFINITION (FormBrowser)

#define EFI_OEM_FORM_BROWSER_PROTOCOL_GUID  \
  {0xbc4ea6f6, 0x2946, 0x4de7, 0x9e, 0x49, 0xda, 0x5b, 0x7e, 0x72, 0x65, 0xfa}

//
// Forward reference for pure ANSI compatability
//
EFI_FORWARD_DECLARATION (EFI_OEM_FORM_BROWSER_PROTOCOL);

typedef struct _EFI_OEM_FORM_BROWSER_PROTOCOL EFI_OEM_FORM_BROWSER_PROTOCOL;

//
// The following types are currently defined:
//
typedef
EFI_STATUS
(EFIAPI *EFI_SEND_OEM_FORM) (
  IN  EFI_OEM_FORM_BROWSER_PROTOCOL   *This,
  IN  BOOLEAN                         UseDatabase,
  IN  EFI_HII_HANDLE                  *Handle,
  IN  UINTN                           HandleCount,
  IN  EFI_IFR_PACKET                  *Packet,
  IN  EFI_HANDLE                      CallbackHandle,
  IN  UINT8                           *NvMapOverride,
  IN SCREEN_DESCRIPTOR                *ScreenDimensions,
  OUT BOOLEAN                         *ResetRequired OPTIONAL
  );

typedef
EFI_STATUS
(EFIAPI *EFI_CREATE_OEM_POP_UP) (
  IN  UINTN                           NumberOfLines,
  IN  BOOLEAN                         HotKey,
  IN  UINTN                           MaximumStringSize,
  OUT CHAR16                          *StringBuffer,
  OUT EFI_INPUT_KEY                   *KeyValue,
  IN  CHAR16                          *String,
  ...
  );

typedef
EFI_STATUS
(EFIAPI *EFI_DIALOG_ICON) (
  IN  UINTN                           NumberOfLines,
  IN  BOOLEAN                         HotKey,
  IN  UINTN                           MaximumStringSize,
  OUT CHAR16                          *StringBuffer,
  OUT EFI_INPUT_KEY                   *KeyValue,
  IN  CHAR16                          *String,
  ...
  );
typedef
EFI_STATUS
(EFIAPI *EFI_OPTION_ICON) (
  IN  UINTN                           NumberOfLines,
  IN  BOOLEAN                         HotKey,
  IN  EFI_INPUT_KEY                   *KeyList,
  OUT EFI_INPUT_KEY                   *EventKey,
  IN  UINTN                           MaximumStringSize,
  OUT CHAR16                          *TitleString,
  OUT UINTN                           *SelectIndex,
  IN  CHAR16                          **String,
  IN  UINTN                           Color
  ); 
typedef struct _EFI_OEM_FORM_BROWSER_PROTOCOL {
  EFI_SEND_OEM_FORM                  SendForm;
  EFI_CREATE_OEM_POP_UP             CreatePopUp;
  EFI_DIALOG_ICON                    DialogIcon;
  EFI_OPTION_ICON                    OptionIcon;
} EFI_OEM_FORM_BROWSER_PROTOCOL;

extern EFI_GUID gEfiOemFormBrowserProtocolGuid;

#endif