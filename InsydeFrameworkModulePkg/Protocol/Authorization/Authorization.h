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

  Authorization.h

Abstract:

  EFI Authorization Protocol

--*/

#ifndef _EFI_AUTHORIZATION_PROTOCOL_H_
#define _EFI_AUTHORIZATION_PROTOCOL_H_

#include <Tiano.h>

EFI_FORWARD_DECLARATION (EFI_AUTH);

//
//  EFI authorization protocol
//
#define EFI_AUTHORIZATION_PROTOCOL_GUID \
  { \
    0x995188b1, 0x9f96, 0x11d4, 0x87, 0xae, 0x00, 0x06, 0x29, 0x2e, 0x8a, 0x3b \
  }

#define EFI_AUTHERR   (val) EFIERR_OEM (0x10000 | (val))
#define EFI_AUTHWARN  (val) EFIWARN (0x10000 | val)

//
//  In addition to standard EFI status codes, the following
//  EFI_AUTHWARNING and EFI_AUTHERR based retun values are
//  defined which are compatible with the EFI_ERROR() macro.
//
#define EFI_AUTHWARN_NO_DATABASE  EFI_AUTHWARN (1)

#define EFI_AUTHERR_INIT          EFI_AUTHERR (1)
#define EFI_AUTHERR_FAILURE       EFI_AUTHERR (2)
#define EFI_AUTHERR_DUPLICATE     EFI_AUTHERR (3)
#define EFI_AUTHERR_COMPROMISED   EFI_AUTHERR (4)

//
//  Bit definitions of authorization methods
//
#define AUTH_METHOD_DEFAULT             0x0000
#define AUTH_METHOD_PASSWORD            0x0001
#define AUTH_METHOD_CHALLENGE_RESPONSE  0x0002
#define AUTH_METHOD_CERTIFICATE         0x0004

typedef
EFI_STATUS
(EFIAPI *EFI_AUTHINIT) (
  IN EFI_AUTH                                  * This,
  IN  UINT64                                   Method,
  IN  VOID                                     *MethodInitData OPTIONAL,
  IN  UINTN                                    MethodInitDataLen    OPTIONAL,
  IN  VOID                                     *ModificationToken OPTIONAL,
  IN  UINTN                                    ModificationTokenLen OPTIONAL
  )
/*++

Routine Description:

  Authorization initialization service.

Arguments:

  This                 - Pointer to this EFI_AUTH_INTERFACE instance.
  Method               - Specifies the desired Authorization to be used.
  MethodInitData       - Pointer to Authorization method specific data.
  MethodInitDataLen    - Length of Authorization method specific data.
  ModificationToken    - Pointer to token needed to modify database.
  ModificationTokenLen - Length of token needed to modify database.

Returns:

  EFI_STATUS

--*/
;

typedef
EFI_STATUS
(EFIAPI *EFI_AUTHVALIDATE) (
  IN EFI_AUTH             * This,
  IN  VOID                *Id,
  IN  UINTN               IdLen,
  IN  VOID                *Key,
  IN  UINTN               KeyLen,
  OUT UINT32              *Privilege OPTIONAL
  )
/*++

Routine Description:

  Validate Authorization credentials

Arguments:

  This   - Pointer to this EFI_AUTH_INTERFACE instance.
  Id     - Pointer to the identifier of the Authorization key.
  IdLen  - Length of Authorization ID.
  Key    - Pointer to Authorization key associated with the ID.
  KeyLen - Length of Authorization key associated with the ID.

Returns:

  EFI_STATUS

--*/
;

typedef
EFI_STATUS
(EFIAPI *EFI_AUTHPROBE) (
  IN EFI_AUTH             * This,
  IN  VOID                *Id,
  IN  UINTN               IdLen,
  OUT UINT32              *Privilege OPTIONAL
  )
/*++

Routine Description:

  Probe account to see if it exists and returns privilege

Arguments:

  This   - Pointer to this EFI_AUTH_INTERFACE instance.
  Id     - Pointer to the identifier of the Authorization key.
  IdLen  - Length of Authorization ID.

Returns:

  EFI_STATUS

--*/
;

typedef
EFI_STATUS
(EFIAPI *EFI_AUTHCREATE) (
  IN EFI_AUTH             * This,
  IN  VOID                *Id,
  IN  UINTN               IdLen,
  IN  VOID                *Key,
  IN  UINTN               KeyLen,
  IN  UINT32              Privilege,
  IN  VOID                *ModificationToken,
  IN  UINTN               ModificationTokenLen
  )
/*++

Routine Description:

  Create Authorization credentials

Arguments:

  This                 - Pointer to this EFI_AUTH_INTERFACE instance.
  Id                   - Pointer to the identifier of the Authorization key.
  IdLen                - Length of Authorization ID.
  Key                  - Pointer to Authorization key associated with the ID.
  KeyLen               - Length of Authorization key associated with the ID.
  ModificationToken    - Pointer to token needed to modify database.
  ModificationTokenLen - Length of token needed to modify database.

Returns:

  EFI_STATUS

--*/
;

typedef
EFI_STATUS
(EFIAPI *EFI_AUTHDELETE) (
  IN EFI_AUTH             * This,
  IN  VOID                *Id,
  IN  UINTN               IdLen,
  IN  VOID                *Key,
  IN  UINTN               KeyLen,
  IN  VOID                *ModificationToken,
  IN  UINTN               ModificationTokenLen
  )
/*++

Routine Description:

  Delete Authorization credentials

Arguments:

  This                 - Pointer to this EFI_AUTH_INTERFACE instance.
  Id                   - Pointer to the identifier of the Authorization key.
  IdLen                - Length of Authorization ID.
  Key                  - Pointer to Authorization key associated with the ID.
  KeyLen               - Length of Authorization key associated with the ID.
  ModificationToken    - Pointer to token needed to modify database.
  ModificationTokenLen - Length of token needed to modify database.

Returns:

  EFI_STATUS

--*/
;

typedef struct _EFI_AUTH {
  EFI_AUTHINIT      Init;
  EFI_AUTHVALIDATE  Validate;
  EFI_AUTHPROBE     Probe;
  EFI_AUTHCREATE    Create;
  EFI_AUTHDELETE    Delete;

  UINT64            Methods;

} EFI_AUTH_INTERFACE;

extern EFI_GUID gEfiAuthorizationProtocolGuid;
#endif // _EFI_AUTHORIZATION_PROTOCOL_H_
