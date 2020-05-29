//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; ----------------------------------------------------------------------------
//;
//; Abstract :
//;
//;   Module name : Tcp6.c
//;   Follow EDK2 Tcp6 defination
//;
/** @file
  EFI TCPv6(Transmission Control Protocol version 6) Protocol Definition
  The EFI TCPv6 Service Binding Protocol is used to locate EFI TCPv6 Protocol drivers to create
  and destroy child of the driver to communicate with other host using TCP protocol.
  The EFI TCPv6 Protocol provides services to send and receive data stream.

  Copyright (c) 2008 - 2010, Intel Corporation
  All rights reserved. This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

  @par Revision Reference:
  This Protocol is introduced in UEFI Specification 2.2

**/

#include "EfiSpec.h"
#include "Tcp6.h"

EFI_GUID  gEfiTcp6ServiceBindingProtocolGuid = EFI_TCP6_SERVICE_BINDING_PROTOCOL_GUID;
EFI_GUID  gEfiTcp6ProtocolGuid               = EFI_TCP6_PROTOCOL_GUID;

EFI_GUID_STRING (
  &gEfiTcp6ServiceBindingProtocolGuid,
  "TCP6 Service Binding Protocol",
  "TCP6 Service Binding Protocol"
  );

EFI_GUID_STRING (
  &gEfiTcp6ProtocolGuid,
  "TCP6 Protocol",
  "TCP6 Protocol"
  );
