/** @file
  Definition file for Management Component Transport Protocol implementation

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _DASH_MCTP_H_
#define _DASH_MCTP_H_

#include "DashCommon.h"


#define MCTP_SIGNATURE               SIGNATURE_32('M', 'C', 'T', 'P')


#pragma pack(1)

typedef struct {
  MCTP_HEADER Header;
  UINT8       Message[MCTP_BASELINE_TRANSMISSION_UNIT];
} MCTP_PACKET;


#pragma pack()

typedef struct _MCTP_CONTEXT {
  UINT32                              Signature;
  EFI_MCTP_PROTOCOL                   Mctp;
  MCTP_HEADER                         ResponseHeader;
  UINT8                               HostEndpointId;
  UINT8                               McEndpointId;
  UINT8                               PacketSequence;
  UINT8                               MessageTag;
  EFI_MCTP_BUS_PROTOCOL               *MctpBus;
  BOOLEAN                             ResponseMode;
  BOOLEAN                             TransactionInProgress;
} MCTP_CONTEXT;

#define MCTP_CONTEXT_FROM_THIS(a)     CR(a, MCTP_CONTEXT, Mctp, MCTP_SIGNATURE)


EFI_STATUS
EFIAPI
MctpSetTransportBinding (
  IN EFI_MCTP_PROTOCOL                 *This,
  EFI_MCTP_BUS_PROTOCOL  *MctpBus
);


EFI_STATUS
EFIAPI
MctpSendMessage (
  IN EFI_MCTP_PROTOCOL *This,
  IN VOID              *Message,
  IN UINTN             MessageSize
);

EFI_STATUS
EFIAPI
MctpReceiveMessage (
  IN EFI_MCTP_PROTOCOL *This,
  OUT VOID          *Message,
  IN OUT UINTN      *MessageSize
);

#endif
