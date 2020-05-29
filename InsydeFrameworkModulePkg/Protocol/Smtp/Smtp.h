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

  Smtp.h

Abstract:

  This code abstracts SMTP Protocol

--*/

#ifndef _EFI_SMTP_PROTOCOL_
#define _EFI_SMTP_PROTOCOL_

#include "Tiano.h"

#define EFI_SMTP_PROTOCOL_GUID \
  { \
    0x5f67d40c, 0x1d06, 0x4e3e, 0x8b, 0x4a, 0xd2, 0x71, 0x3f, 0x46, 0xe1, 0xdc \
  }

#define SMTP_PORT 25

EFI_FORWARD_DECLARATION (EFI_SMTP_PROTOCOL);

typedef
EFI_STATUS
(EFIAPI *EFI_SMTP_GET_SERVER) (
  IN EFI_SMTP_PROTOCOL                * This,
  IN OUT UINT32                       *UrlStringSize,
  OUT EFI_IP_ADDRESS                  * ServerAddress,
  OUT UINT8                           *ServerUrl
  );

typedef
EFI_STATUS
(EFIAPI *EFI_SMTP_SET_SERVER) (
  IN EFI_SMTP_PROTOCOL                * This,
  IN OUT EFI_IP_ADDRESS               * ServerAddress,
  IN OUT UINT8                        *ServerUrl
  );

typedef
EFI_STATUS
(EFIAPI *EFI_SMTP_SEND_MAIL) (
  IN EFI_SMTP_PROTOCOL                * This,
  IN UINT8                            *Message,
  IN UINT8                            *Subject,
  IN UINT8                            *Sender,
  IN UINT8                            *Receiver
  );

//
// SMTP PROTOCOL
//
typedef struct _EFI_SMTP_PROTOCOL {
  EFI_SMTP_SEND_MAIL  SendMail;
  EFI_SMTP_SET_SERVER SetMailServer;
  EFI_SMTP_GET_SERVER GetMailServer;
} EFI_SMTP_PROTOCOL;

extern EFI_GUID gEfiSmtpProtocolGuid;

#endif
