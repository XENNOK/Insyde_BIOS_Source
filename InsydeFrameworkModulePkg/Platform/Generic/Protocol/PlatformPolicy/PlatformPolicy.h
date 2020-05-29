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

Copyright (c)  1999 - 2001 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PlatformPolicy.h

Abstract:

  Protocol used for Platform Policy definition.

--*/

#ifndef _PLATFORM_POLICY_PROTOCOL_H_
#define _PLATFORM_POLICY_PROTOCOL_H_

EFI_FORWARD_DECLARATION (EFI_PLATFORM_POLICY_PROTOCOL);

#define EFI_PLATFORM_POLICY_PROTOCOL_GUID \
  { \
    0x2977064f, 0xab96, 0x4fa9, 0x85, 0x45, 0xf9, 0xc4, 0x02, 0x51, 0xe0, 0x7f \
  }

//
// Protocol to describe various platform information. Add to this as needed.
//
typedef struct _EFI_PLATFORM_POLICY_PROTOCOL {
  UINT8 NumRsvdSmbusAddresses;
  UINT8 *RsvdSmbusAddresses;
} EFI_PLATFORM_POLICY_PROTOCOL;

extern EFI_GUID gEfiPlatformPolicyProtocolGuid;

#endif
