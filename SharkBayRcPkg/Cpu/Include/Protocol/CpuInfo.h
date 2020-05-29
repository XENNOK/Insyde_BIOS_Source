/** @file
  Protocol used to report CPU information

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 1999 - 2014 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
#ifndef _CPU_INFO_H_
#define _CPU_INFO_H_

#define DXE_CPU_INFO_PROTOCOL_GUID \
  { 0xe223cf65, 0xf6ce, 0x4122, { 0xb3, 0xaf, 0x4b, 0xd1, 0x8a, 0xff, 0x40, 0xa1 } }

///
/// Protocol revision number
/// Any backwards compatible changes to this protocol will result in an update in the revision number
/// Major changes will require publication of a new protocol
///
#define DXE_CPU_INFO_REVISION_1 1

///
/// RCVersion[7:0] is the release number.
/// For example:
/// Version 0.7.1    should be 00 07 01 00 (0x00070100)
///
#define CPU_RC_VERSION  0x01080000

extern EFI_GUID gDxeCpuInfoProtocolGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _DXE_CPU_INFO_PROTOCOL DXE_CPU_INFO_PROTOCOL;

//
// CPU Common Feature Definition
//

///
/// The processor Command Feature Definition is a super set and abstracts from hardware.
///
#define TXT_SUPPORT   1
#define VMX_SUPPORT   (1 << 1)
#define XD_SUPPORT    (1 << 2)
#define DCA_SUPPORT   (1 << 3)
#define XAPIC_SUPPORT (1 << 4)
#define AES_SUPPORT   (1 << 5)
#define HT_SUPPORT    (1 << 6)
#define DEBUG_SUPPORT (1 << 7)
#define DEBUG_LOCK_SUPPORT (1 << 8)

///
/// Interface structure for the CPU Info Protocol
///
struct _DXE_CPU_INFO_PROTOCOL {
  UINT8  Revision;          ///< Revision for the protocol structure.
  UINT32 RcVersion;         ///< Processor Features information.
  //
  // Bit definition for functionality enable/disable
  //
  UINT64 CpuCommonFeatures; ///< CPU Supported Feature. Output from CPU code.
};

#endif
