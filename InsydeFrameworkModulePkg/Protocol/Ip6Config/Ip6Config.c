//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

/*++

Copyright (c) 2006, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  Ip6Config.c

Abstract:

--*/

#include "EfiSpec.h"
#include EFI_PROTOCOL_DEFINITION (Ip6Config)

EFI_GUID gEfiIp6ConfigProtocolGuid  = EFI_IP6_CONFIG_PROTOCOL_GUID;

EFI_GUID_STRING (
  &gEfiIp6ConfigProtocolGuid,
  "Ip6Config Protocol",
  "Ip6Config Protocol"
  );
