/** @file
  Management Component Transport Binding Protocol

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

#ifndef _DASH_MCTP_BUS_H_
#define _DASH_MCTP_BUS_H_

#include "DashCommon.h"


#define MCTP_BUS_SIGNATURE                   SIGNATURE_32('M', 'C', 'T', 'B')
#define MAX_MCTP_PACKET_SIZE                 (MCTP_BASELINE_TRANSMISSION_UNIT + 5)

typedef struct _MCTP_BUS_CONTEXT {
  UINT32 Signature;
  EFI_MCTP_BUS_PROTOCOL MctpBus;
  UINT8 McAddr;
  UINT8 HostSlaveAddr;
  MCTP_SMBUS_PROTOCOL *MctpSmbus;
} MCTP_BUS_CONTEXT;

#define MCTP_BUS_CONTEXT_FROM_THIS(a)   CR(a, MCTP_BUS_CONTEXT, MctpBus, MCTP_BUS_SIGNATURE)

#endif
