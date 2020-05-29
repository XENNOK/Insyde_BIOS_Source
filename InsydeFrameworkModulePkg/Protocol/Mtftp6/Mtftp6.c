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
//;   Module name : Mtftp6.c
//;   Follow EDK2 Mtftp6 defination
//;
/** @file
  UEFI Multicast Trivial File Tranfer Protocol v6 Definition, which is built upon
  the EFI UDPv6 Protocol and provides basic services for client-side unicast and/or
  multicast TFTP operations.

  Copyright (c) 2008 - 2010, Intel Corporation
  All rights reserved. This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  Mtftp6.c

Abstract:

  UEFI Multicast Trivial File Transfer Protocol GUID Declaration.

**/

#include "EfiSpec.h"
#include EFI_PROTOCOL_DEFINITION (Mtftp6)

EFI_GUID gEfiMtftp6ServiceBindingProtocolGuid  = EFI_MTFTP6_SERVICE_BINDING_PROTOCOL_GUID;
EFI_GUID gEfiMtftp6ProtocolGuid = EFI_MTFTP6_PROTOCOL_GUID;

EFI_GUID_STRING (
  &gEfiMtftp6ServiceBindingProtocolGuid,
  "MTFTP6 Service Binding Protocol",
  "MTFTP6 Service Binding Protocol"
  );

EFI_GUID_STRING (
  &gEfiMtftp6ProtocolGuid,
  "MTFTP6 Protocol",
  "MTFTP6 Protocol"
  );
