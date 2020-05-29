/** @file
  This file defines the PowerMgmtInitDone Protocol.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _POWER_MGMT_INIT_DONE_H_
#define _POWER_MGMT_INIT_DONE_H_

#define EFI_POWER_MGMT_INIT_DONE_PROTOCOL_GUID \
  { 0xd71db106, 0xe32d, 0x4225, { 0xbf, 0xf4, 0xde, 0x6d, 0x77, 0x87, 0x17, 0x61 } }

///
/// Extern the GUID for protocol users.
///
extern EFI_GUID gEfiPowerMgmtInitDoneProtocolGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _EFI_POWER_MGMT_INIT_DONE_PROTOCOL EFI_POWER_MGMT_INIT_DONE_PROTOCOL;

#endif
