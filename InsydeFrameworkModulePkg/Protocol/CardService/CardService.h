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

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

    CardService.h

Abstract:

    EFI PC Card Card Service Protocol

Revision History

--*/

#ifndef _EFI_PCCARD_CS_H
#define _EFI_PCCARD_CS_H

//
// Global GUID for PC Card Card Service Protocol
//
#define EFI_PCCARD_CS_PROTOCOL_GUID \
  { \
    0xca54f443, 0x1ef2, 0x4dab, 0x9e, 0x7e, 0x6d, 0xb7, 0xb7, 0x20, 0xb5, 0x87 \
  }

EFI_FORWARD_DECLARATION (EFI_PCCARD_CS_PROTOCOL);

//
// Prototypes for PC Card Card Service Protocol
//
typedef
EFI_STATUS
(EFIAPI *EFI_PCCARD_CS_GET_TUPLE) (
  IN EFI_PCCARD_CS_PROTOCOL               * This,
  IN     UINT8                            TupleCode,
  IN OUT UINTN                            *BufferSize,
  OUT UINT8                               *Buffer,
  IN OUT VOID                             **Key
  );

typedef
EFI_STATUS
(EFIAPI *EFI_PCCARD_CS_REGISTER_EVENT) (
  IN EFI_PCCARD_CS_PROTOCOL                        * This,
  IN     BOOLEAN                                   Enable OPTIONAL,
  IN     EFI_EVENT                                 Event
  );

//
// EFI PC Card Event bits
//
#define EFI_PCCARD_CS_EVENT_READY   0x00000001
#define EFI_PCCARD_CS_EVENT_BATTERY 0x00000002
//
// #define  EFI_PCCARD_CS_EVENT_BATTERY_DEAD       0x00000004
//
#define EFI_PCCARD_CS_EVENT_WRITE_PROTECT 0x00000008
#define EFI_PCCARD_CS_EVENT_GWAKE         0x00000010

typedef
EFI_STATUS
(EFIAPI *EFI_PCCARD_CS_GET_EVENT) (
  IN EFI_PCCARD_CS_PROTOCOL                * This,
  OUT    UINT32                            *Event
  );

//
// EFI PC Card State bits
//
#define EFI_PCCARD_CS_STATE_READY           0x00000001
#define EFI_PCCARD_CS_STATE_BATTERY_WARNING 0x00000002
#define EFI_PCCARD_CS_STATE_BATTERY_DEAD    0x00000004
#define EFI_PCCARD_CS_STATE_WRITE_PROTECT   0x00000008
#define EFI_PCCARD_CS_STATE_GWAKE           0x00000010

typedef
EFI_STATUS
(EFIAPI *EFI_PCCARD_CS_GET_STATE) (
  IN EFI_PCCARD_CS_PROTOCOL            * This,
  OUT UINT32                           *State
  );

typedef struct _EFI_PCCARD_CS_PROTOCOL {
  EFI_PCCARD_CS_GET_TUPLE       GetTuple;
  EFI_PCCARD_CS_REGISTER_EVENT  RegisterHandler;
  EFI_PCCARD_CS_GET_EVENT       GetEvent;
  EFI_PCCARD_CS_GET_STATE       GetState;
} EFI_PCCARD_CS_PROTOCOL;

extern EFI_GUID gEfiPcCardCsProtocolGuid;

#endif
