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

  TcpSock.h

Abstract:

  This code abstracts TCP Socket Protocol

--*/

#ifndef _EFI_TCP_SOCK_PROTOCOL_
#define _EFI_TCP_SOCK_PROTOCOL_

#include "Tiano.h"
#include EFI_PROTOCOL_DEFINITION (Tcp)

#define EFI_TCP_SOCK_PROTOCOL_GUID \
  { \
    0x5187359b, 0x790d, 0x425b, 0xa5, 0x93, 0xca, 0x1c, 0xdb, 0x3c, 0xeb, 0xad \
  }

EFI_FORWARD_DECLARATION (EFI_TCP_SOCK_PROTOCOL);

typedef
VOID
(EFIAPI *FIN_CALLBACK_FUNCTION) ();

typedef
EFI_STATUS
(EFIAPI *EFI_TCP_SOCKET_WRITE_DATA) (
  IN EFI_TCP_SOCK_PROTOCOL             * This,
  IN UINT8                             SocketId,
  IN UINT8                             *Data,
  IN UINTN                             Size,
  IN BOOLEAN                           Blocked
  );

typedef
EFI_STATUS
(EFIAPI *EFI_TCP_SOCKET_READ_DATA) (
  IN EFI_TCP_SOCK_PROTOCOL            * This,
  IN UINT8                            SocketId,
  IN OUT UINT8                        *Data,
  IN OUT UINTN                        *Size,
  IN BOOLEAN                          Blocked
  );

typedef
EFI_STATUS
(EFIAPI *EFI_TCP_DISCONNECT_SOCKET) (
  IN EFI_TCP_SOCK_PROTOCOL              * This,
  UINT8                                 SocketId,
  BOOLEAN                               Forced
  );

typedef
EFI_STATUS
(EFIAPI *EFI_TCP_CONNECT_SOCKET) (
  IN EFI_TCP_SOCK_PROTOCOL            * This,
  IN EFI_IP_ADDRESS                   * DestIp,
  IN UINT16                           SrcPort,
  IN UINT16                           DestPort,
  IN FIN_CALLBACK_FUNCTION            FinCallback,
  OUT UINT8                           *SocketId
  );

//
// TCP SOCK PROTOCOL
//
typedef struct _EFI_TCP_SOCK_PROTOCOL {
  EFI_TCP_CONNECT_SOCKET    ConnectSocket;
  EFI_TCP_DISCONNECT_SOCKET DisconnectSocket;
  EFI_TCP_SOCKET_READ_DATA  ReadSocketData;
  EFI_TCP_SOCKET_WRITE_DATA WriteSocketData;
} EFI_TCP_SOCK_PROTOCOL;

extern EFI_GUID gEfiTcpSockProtocolGuid;

#endif
