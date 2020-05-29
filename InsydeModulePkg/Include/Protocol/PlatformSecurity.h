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

Copyright (c)  2005 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PlatformSecurity.h

Abstract:

  Platform Security Protocol

--*/

#ifndef _PLATFORM_SECURITY_PROTOCOL_H_
#define _PLATFORM_SECURITY_PROTOCOL_H_


#define PLATFORM_SECURITY_PROTOCOL_GUID  \
  {0x98b49027, 0x8273, 0x4f51, 0xeb, 0xa9, 0x80, 0x69, 0x52, 0xd4, 0x88, 0xc0}


typedef struct _PLATFORM_SECURITY_PROTOCOL PLATFORM_SECURITY_PROTOCOL;


//
// Image type definitions
//
#define IMAGE_UNKNOWN                         0x00000000
#define IMAGE_FROM_FV                         0x00000001
#define IMAGE_FROM_OPTION_ROM                 0x00000002
#define IMAGE_FROM_REMOVABLE_MEDIA            0x00000003
#define IMAGE_FROM_FIXED_MEDIA                0x00000004

//
// Authorization policy bit definition
//
#define ALWAYS_EXECUTE                         0x00000000
#define NEVER_EXECUTE                          0x00000001
#define ALLOW_EXECUTE_ON_SECURITY_VIOLATION    0x00000002
#define DEFER_EXECUTE_ON_SECURITY_VIOLATION    0x00000003
#define DENY_EXECUTE_ON_SECURITY_VIOLATION     0x00000004
#define QUERY_USER_ON_SECURITY_VIOLATION       0x00000005


typedef
EFI_STATUS
(EFIAPI *GET_PLATFORM_IMAGE_EXEC_POLICY) (
  IN      UINT32                     ImageType,
  OUT     UINT32                    *Policy
  );

typedef
EFI_STATUS
(EFIAPI *GET_PLATFORM_SECURE_VAR_STORE_ADDR) (
  OUT     EFI_PHYSICAL_ADDRESS       *Address
  );

typedef
EFI_STATUS
(EFIAPI *GET_PLATFORM_SECURE_VAR_STORE_SIZE) (
  OUT     UINT64                    *Size
  );

typedef
EFI_STATUS
(EFIAPI *GET_PLATFORM_CLEAR_PK_STATE) (
  OUT     BOOLEAN                    *ClearPlatformKey
  );


struct _PLATFORM_SECURITY_PROTOCOL {
  GET_PLATFORM_IMAGE_EXEC_POLICY         GetPlatformImageExecPolicy;
  GET_PLATFORM_SECURE_VAR_STORE_ADDR     GetPlatformSecureVarStoreAddr;
  GET_PLATFORM_SECURE_VAR_STORE_SIZE     GetPlatformSecureVarStoreSize;
  GET_PLATFORM_CLEAR_PK_STATE            GetPlatformClearPkState;
};

extern EFI_GUID gPlatformSecurityProtocolGuid;

#endif
