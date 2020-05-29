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

    SocketService.h

Abstract:

    EFI PC Card Socket Service Protocol

Revision History

--*/

#ifndef _EFI_PCCARD_SS_H
#define _EFI_PCCARD_SS_H

//
// Global GUID for PC Card Socket Service Protocol
//
#define EFI_PCCARD_SS_PROTOCOL_GUID \
  { \
    0xc38e6d34, 0x5a7f, 0x4bf9, 0xbe, 0x57, 0x94, 0xdd, 0x30, 0x38, 0x02, 0x76 \
  }

EFI_FORWARD_DECLARATION (EFI_PCCARD_SS_PROTOCOL);

//
// Prototypes for PC Card Socket Service Protocol
//
//
// EFI PC Card Socket Event Bits
//
#define EFI_PCCARD_SS_EVENT_DETECT      0x00000001
#define EFI_PCCARD_SS_EVENT_POWERCYCLE  0x00000002
#define EFI_PCCARD_SS_EVENT_STSCHG      0x00000004

typedef
EFI_STATUS
(EFIAPI *EFI_PCCARD_SS_GET_EVENT) (
  IN EFI_PCCARD_SS_PROTOCOL               * This,
  OUT UINT32                              *Event
  );

//
// EFI PC Card Status Bits
//
#define EFI_PCCARD_SS_STATUS_DETECT     0x00000001
#define EFI_PCCARD_SS_STATUS_POWERCYCLE 0x00000002
#define EFI_PCCARD_SS_STATUS_STSCHG     0x00000004
#define EFI_PCCARD_SS_STATUS_BADVCCREQ  0x00000008
#define EFI_PCCARD_SS_STATUS_READY      0x00000010
#define EFI_PCCARD_SS_STATUS_DATALOST   0x00000020

typedef
EFI_STATUS
(EFIAPI *EFI_PCCARD_SS_GET_STATUS) (
  IN EFI_PCCARD_SS_PROTOCOL              * This,
  OUT UINT32                             *Status
  );

//
// EFI PC Card Capability Object
//
typedef enum {
  EfiPccardCapabilitySocket,
  EfiPccardCapabilityCard,
  EfiPccardCapabilityMaximum
} EFI_PCCARD_CAPABILITY_OBJECT;

//
// EFI PC Card Capability Bits
//
#define EFI_PCCARD_CAPABILITY_YV          0x00000001
#define EFI_PCCARD_CAPABILITY_XV          0x00000002
#define EFI_PCCARD_CAPABILITY_3V          0x00000004
#define EFI_PCCARD_CAPABILITY_5V          0x00000008
#define EFI_PCCARD_CAPABILITY_16BIT       0x00000100
#define EFI_PCCARD_CAPABILITY_32BIT       0x00000200
#define EFI_PCCARD_CAPABILITY_UNKOWNCARD  0x80000000

typedef
EFI_STATUS
(EFIAPI *EFI_PCCARD_SS_CAPABILITY) (
  IN EFI_PCCARD_SS_PROTOCOL                  * This,
  IN     EFI_PCCARD_CAPABILITY_OBJECT        Object,
  OUT UINT32                                 *Capability
  );

//
// EFI PC Card Attribute Operation
//
typedef enum {
  EfiPccardAttributeGet,
  EfiPccardAttributeSet,
  EfiPccardAttributeMaximum
} EFI_PCCARD_ATTRIBUTE_OPERATION;

//
// EFI PC Card Attibute Bits
//
#define EFI_PCCARD_ATTRIBUTE_VCC_YV       0x00000001
#define EFI_PCCARD_ATTRIBUTE_VCC_XV       0x00000002
#define EFI_PCCARD_ATTRIBUTE_VCC_3V       0x00000004
#define EFI_PCCARD_ATTRIBUTE_VCC_5V       0x00000008
#define EFI_PCCARD_ATTRIBUTE_VPP_YV       0x00000100
#define EFI_PCCARD_ATTRIBUTE_VPP_XV       0x00000200
#define EFI_PCCARD_ATTRIBUTE_VPP_3V       0x00000400
#define EFI_PCCARD_ATTRIBUTE_VPP_5V       0x00000800
#define EFI_PCCARD_ATTRIBUTE_VPP_12V      0x00001000
#define EFI_PCCARD_ATTRIBUTE_ATUOPOWER    0x00010000
#define EFI_PCCARD_ATTRIBUTE_STOPCLK      0x00020000
#define EFI_PCCARD_ATTRIBUTE_FORCE_DETECT 0x00040000

typedef
EFI_STATUS
(EFIAPI *EFI_PCCARD_SS_ATTRIBUTE) (
  IN EFI_PCCARD_SS_PROTOCOL                  * This,
  IN     EFI_PCCARD_ATTRIBUTE_OPERATION      Operation,
  IN OUT UINT64                              *Attribute
  );

typedef
EFI_STATUS
(EFIAPI *EFI_PCCARD_SS_RESET) (
  IN EFI_PCCARD_SS_PROTOCOL      * This
  );

typedef struct _EFI_PCCARD_SS_PROTOCOL {
  EFI_PCCARD_SS_GET_EVENT   GetEvent;
  EFI_PCCARD_SS_GET_STATUS  GetStatus;
  EFI_PCCARD_SS_CAPABILITY  Capability;
  EFI_PCCARD_SS_ATTRIBUTE   Attribute;
  EFI_PCCARD_SS_RESET       Reset;
} EFI_PCCARD_SS_PROTOCOL;

extern EFI_GUID gEfiPcCardSsProtocolGuid;

#endif
