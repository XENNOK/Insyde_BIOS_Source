/** @file
  Management Component Transport Binding definition for SMBUS intefarce

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

#ifndef _MCTP_SMBUS_H_
#define _MCTP_SMBUS_H_
#include <Uefi.h>
#include <Protocol/SmbusHc.h>

#define MCTP_SMBUS_PROTOCOL_GUID \
  { 0xe330b506, 0x10a5, 0x422a, 0xb5, 0x4e, 0x67, 0x32, 0x54, 0x58, 0x12, 0x64 }


typedef struct  _MCTP_SMBUS_PROTOCOL MCTP_SMBUS_PROTOCOL;

typedef
BOOLEAN 
(EFIAPI *SMBUS_BUFFER_FULL) (
  IN  MCTP_SMBUS_PROTOCOL *This
);


typedef 
VOID
(EFIAPI *SMBUS_SLAVE_SET_LISTEN_ADDRESS) (
  IN MCTP_SMBUS_PROTOCOL        *This,
  IN UINT8                      ListenAddr
);

typedef 
EFI_STATUS
(EFIAPI *SMBUS_MASTER_SEND) (
  IN  MCTP_SMBUS_PROTOCOL       *This,
  IN  UINT8                     SmbusAddr,
  IN  VOID                      *Buffer,
  IN  UINTN                     BufferSize
);

typedef 
EFI_STATUS
(EFIAPI *SMBUS_SLAVE_RECEIVE) (
  IN  MCTP_SMBUS_PROTOCOL       *This,
  OUT VOID                      *Buffer,
  IN OUT UINTN                  *BufferSize
);

struct _MCTP_SMBUS_PROTOCOL {
  SMBUS_SLAVE_SET_LISTEN_ADDRESS SetListenAddress;
  SMBUS_BUFFER_FULL              BufferFull;
  SMBUS_MASTER_SEND              MasterSend;
  SMBUS_SLAVE_RECEIVE            SlaveReceive;
};

extern EFI_GUID gMctpSmbusProtocolGuid;

#endif
