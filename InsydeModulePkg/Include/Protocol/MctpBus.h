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

#ifndef _MCTP_BUS_H_
#define _MCTP_BUS_H_

#include <Uefi.h>
#include <Protocol/SmbusHc.h>
#include <Protocol/Mctp.h>

#define EFI_MCTP_BUS_PROTOCOL_GUID \
  { 0xf76db796, 0xaa3c, 0x48e8, 0x93, 0xc5, 0x60, 0x89, 0x55, 0xa3, 0x80, 0xc9 }


#pragma pack (1)
typedef struct {
  UINT8 EndpointId;
  EFI_SMBUS_DEVICE_ADDRESS SlaveAddress;
} MCTP_ENDPOINT_MAP;
#pragma pack ()


typedef
BOOLEAN
(EFIAPI *MCTP_BUS_NEW_MESSAGE_RECEIVED) (
  EFI_MCTP_BUS_PROTOCOL *This
);

typedef
EFI_STATUS
(EFIAPI *SEND_MCTP_PACKET) (
  IN  EFI_MCTP_BUS_PROTOCOL *This,
  IN  VOID                  *PhysicalAddress,
  IN  MCTP_HEADER           MctpHeader,
  IN  VOID                  *Payload,
  IN  UINTN                 PayloadSize
);

typedef 
EFI_STATUS
(EFIAPI *RECEIVE_MCTP_PACKET) (
  IN  EFI_MCTP_BUS_PROTOCOL *This,
  IN  VOID                  *PhysicalAddress,
  OUT MCTP_HEADER           *MctpHeader,  
  OUT VOID                  *Payload,
  OUT UINTN                 *PayloadSize
);



struct _EFI_MCTP_BUS_PROTOCOL {
  MCTP_BUS_NEW_MESSAGE_RECEIVED NewMessageReceived; 
  SEND_MCTP_PACKET              SendMctpPacket;
  RECEIVE_MCTP_PACKET           ReceiveMctpPacket;
};

extern EFI_GUID gMctpBusProtocolGuid;

#endif
