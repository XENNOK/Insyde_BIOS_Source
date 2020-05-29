/**
  This file contains an 'Intel Peripheral Driver' and uniquely        
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your   
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the
  license agreement
**/

/**

Copyright (c) 2011 - 2012 Intel Corporation. All rights reserved.
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file
  PlatformGopPolicy.h

@brief
  Interface definition for PlatformGopPolicy Protocol.

**/
#ifndef _PLATFORM_GOP_POLICY_PROTOCOL_H_
#define _PLATFORM_GOP_POLICY_PROTOCOL_H_

#define EFI_PLATFORM_GOP_POLICY_PROTOCOL_GUID \
  { \
    0xec2e931b, 0x3281, 0x48a5, 0x81, 0x7, 0xdf, 0x8a, 0x8b, 0xed, 0x3c, 0x5d \
  }

#define PLATFORM_GOP_POLICY_PROTOCOL_REVISION_01  0x01

typedef enum {
  LidClosed,
  LidOpen,
  LidStatusMax
} LID_STATUS;

typedef
EFI_STATUS
(EFIAPI *GET_PLATFORM_LID_STATUS) (
  OUT LID_STATUS * CurrentLidStatus
  );

typedef
EFI_STATUS
(EFIAPI *GET_VBT_DATA) (
  OUT EFI_PHYSICAL_ADDRESS * VbtAddress,
  OUT UINT32               *VbtSize
  );

typedef struct _PLATFORM_GOP_POLICY_PROTOCOL {
  UINT32                  Revision;
  GET_PLATFORM_LID_STATUS GetPlatformLidStatus;
  GET_VBT_DATA            GetVbtData;
} PLATFORM_GOP_POLICY_PROTOCOL;

extern EFI_GUID gPlatformGopPolicyProtocolGuid;

#endif
