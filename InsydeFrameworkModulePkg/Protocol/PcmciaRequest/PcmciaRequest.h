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

    PcmciaRequest.h

Abstract:

    EFI PCMCIA Request Protocol

Revision History

--*/

#ifndef _EFI_PCMCIA_REQUEST_H
#define _EFI_PCMCIA_REQUEST_H

//
// Global GUID for PCMCIA Request Protocol
//
#define EFI_PCMCIA_REQUEST_PROTOCOL_GUID \
  { \
    0xf238f47c, 0x1de9, 0x4e8b, 0x81, 0xb9, 0xcc, 0x92, 0x4e, 0x6b, 0x5b, 0xe5 \
  }

EFI_FORWARD_DECLARATION (EFI_PCMCIA_REQUEST_PROTOCOL);

//
// Prototypes for PCMCIA Request Protocol
//
typedef enum {
  EfiPCMCIARequestStart,
  EfiPCMCIARequestStop,
  EfiPCMCIARequestStatusChange
} EFI_PCMCIA_OPERATION;

typedef
EFI_STATUS
(EFIAPI *EFI_PCMCIA_REQUEST_NOTIFY) (
  IN EFI_PCMCIA_REQUEST_PROTOCOL                * This,
  IN     EFI_PCMCIA_OPERATION                   Operation,
  IN     EFI_HANDLE                             SocketHandle,
  IN     EFI_DEVICE_PATH_PROTOCOL               * RemainingDevicePath OPTIONAL,
  IN OUT UINT8                                  *NumberOfChildren,
  IN OUT EFI_HANDLE                             * ChildHandleBuffer
  );

typedef struct _EFI_PCMCIA_REQUEST_PROTOCOL {

  EFI_PCMCIA_REQUEST_NOTIFY Notify;

} EFI_PCMCIA_REQUEST_PROTOCOL;

extern EFI_GUID gEfiPcmciaRequestProtocolGuid;

#endif
