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
//;   Module name : IpSec.c
//;   Follow EDK2 IpSec defination
//;
/*++

Copyright (c) 2006, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  IpSec.c

Abstract:

--*/

#include "EfiSpec.h"
#include EFI_PROTOCOL_DEFINITION (IpSec)

EFI_GUID gEfiIpSecProtocolGuid  = EFI_IPSEC_PROTOCOL_GUID;

EFI_GUID_STRING (
  &gEfiIpSecProtocolGuid,
  "IpSec Protocol",
  "IpSec Protocol"
  );
