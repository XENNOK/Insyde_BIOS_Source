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

Copyright (c)  2006 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PlatformManagementPolicy.h

Abstract:

--*/

#ifndef _PROTOCOL_PLATFORM_MANAGEMENT_POLICY_H__
#define _PROTOCOL_PLATFORM_MANAGEMENT_POLICY_H__

//
// Global ID for the Platform Policy Protocol
//
#define EFI_PLATFORM_MANAGEMENT_POLICY_PROTOCOL_GUID \
  { \
    0xec5e6d1f, 0xd079, 0x4f46, 0xa9, 0x39, 0x5e, 0xf2, 0x65, 0x30, 0xfb, 0xbd \
  }

extern EFI_GUID gEfiPlatformManagementPolicyProtocolGuid;

//
// Forward reference for pure ANSI compatability
//
EFI_FORWARD_DECLARATION (EFI_PLATFORM_MANAGEMENT_POLICY_PROTOCOL);

typedef
EFI_STATUS
(EFIAPI *EFI_GET_PLATFORM_MANAGEMENT_POLICY) (
  IN EFI_PLATFORM_MANAGEMENT_POLICY_PROTOCOL * This,
  IN CHAR16                       * Name,
  OUT VOID                        * Value,
  IN OUT UINT32                   * Length
  );

typedef
EFI_STATUS
(EFIAPI *EFI_SET_PLATFORM_MANAGEMENT_POLICY) (
  IN EFI_PLATFORM_MANAGEMENT_POLICY_PROTOCOL * This,
  IN CHAR16                       * Name,
  IN VOID                         * Value,
  IN UINT32                       Length
  );

typedef struct _EFI_PLATFORM_MANAGEMENT_POLICY_PROTOCOL {
  EFI_GET_PLATFORM_MANAGEMENT_POLICY  GetValue;
  EFI_SET_PLATFORM_MANAGEMENT_POLICY  SetValue;
} EFI_PLATFORM_MANAGEMENT_POLICY_PROTOCOL;

#endif
