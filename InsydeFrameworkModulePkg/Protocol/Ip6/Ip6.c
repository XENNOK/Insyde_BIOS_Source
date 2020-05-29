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
//;   Module name : Ip6.c
//;   Follow EDK2 Ip6 defination
//;
/** @file
  This file defines the EFI IPv6 (Internet Protocol version 6)
  Protocol interface. It is split into the following three main
  sections:
  - EFI IPv6 Service Binding Protocol
  - EFI IPv6 Variable
  - EFI IPv6 Protocol
  The EFI IPv6 Protocol provides basic network IPv6 packet I/O services,
  which includes support for Neighbor Discovery Protocol (ND), Multicast
  Listener Discovery Protocol (MLD), and a subset of the Internet Control
  Message Protocol (ICMPv6).

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
#include EFI_PROTOCOL_DEFINITION (Ip6)

EFI_GUID gEfiIp6ServiceBindingProtocolGuid
           = EFI_IP6_SERVICE_BINDING_PROTOCOL_GUID;

EFI_GUID gEfiIp6ProtocolGuid = EFI_IP6_PROTOCOL_GUID;

EFI_GUID_STRING (
  &gEfiIp6ServiceBindingProtocolGuid,
  "IP6 Service Binding Protocol",
  "IP6 Service Binding Protocol"
  );

EFI_GUID_STRING (
  &gEfiIp6ProtocolGuid,
  "IP6 Protocol",
  "IP6 Protocol"
  );
