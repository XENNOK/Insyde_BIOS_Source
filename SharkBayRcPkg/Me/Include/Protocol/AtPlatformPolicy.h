/** @file
  This driver implements the AT Platform Policy protocol.
  
;******************************************************************************
;* Copyright (c) 1983 - 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 2004 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
#ifndef _DXE_AT_PLATFORM_POLICY_H_
#define _DXE_AT_PLATFORM_POLICY_H_

#include <Uefi.h>

///
/// AT policy provided by platform for DXE phase
///
#define DXE_PLATFORM_AT_POLICY_GUID \
  { 0x3F4EA4D4, 0xB465, 0x4F3F, { 0x9C, 0x92, 0xB3, 0x40, 0xAB, 0x58, 0x96, 0xB5 }}

///
/// Protocol revision number
/// Any backwards compatible changes to this protocol will result in an update in the revision number
/// Major changes will require publication of a new protocol
///
#define DXE_PLATFORM_AT_POLICY_PROTOCOL_REVISION_1 1
#define DXE_PLATFORM_AT_POLICY_PROTOCOL_REVISION_2 2

extern EFI_GUID gDxePlatformAtPolicyGuid;
#pragma pack(1)
typedef struct {
  UINT8 AtAmBypass;           ///< Byte 0, bit definition for functionality enable/disable
  UINT8 AtEnterSuspendState;
  UINT8 AtSupported;
  UINT8 AtPba;
  UINT8 ByteReserved[28];   ///< Bytes 4-31 Reserved and make AT_CONFIG as 32 bit alignment
} AT_CONFIG;
#pragma pack()
///
/// AMT DXE Platform Policy
///
typedef struct _DXE_AT_POLICY_PROTOCOL {
  UINT8       Revision;
  AT_CONFIG   At;
} DXE_AT_POLICY_PROTOCOL;
#endif
