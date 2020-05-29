/** @file

  Server Platform Services (SPS) Policy for SPS ME Drivers.

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SPS_POLICY_PROTOCOL_H_
#define _SPS_POLICY_PROTOCOL_H_

#include <SpsPolicy.h>

///
/// Define the Protocol GUID
///
/// {F6F6D435-B70A-45EA-B841-B0C0030C8555}
#define SPS_POLICY_PPI_GUID \
  { 0xF6F6D435, 0xB70A, 0x45EA, { 0xB8, 0x41, 0xB0, 0xC0, 0x03, 0x0C, 0x85, 0x55 }}

typedef struct _SPS_POLICY             SPS_POLICY_PROTOCOL;

///
/// Extern the GUID for Protocol users.
///
extern EFI_GUID gSpsPolicyProtocolGuid;

#endif
