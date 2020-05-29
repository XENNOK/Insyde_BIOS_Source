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

  SmmRuntime.h

Abstract:

  Implementation specific SMM Runtime stuff

--*/

#ifndef _SMM_RUNTIME_H_
#define _SMM_RUNTIME_H_

#include "Tiano.h"
#include "EfiRuntimeLib.h"
#include "EfiMgmtModeRuntimeLib.h"
#include "PeiHob.h"
#include "IoAccess.h"

#include EFI_PROTOCOL_DEFINITION (SmmBase)
#include EFI_PROTOCOL_DEFINITION (LoadedImage)
#include EFI_PROTOCOL_DEFINITION (SmmRtProtocol)

#define MAX_SMM_PROTOCOL    100
#define MAX_CALLBACK        100
#define MAX_HANDLES         100
#define MAX_CONFIG_TABLE    100
#define MAX_SM_RT_CALLBACK  100

typedef struct {
  BOOLEAN             Valid;
  EFI_GUID            ProtocolGuid;
  EFI_INTERFACE_TYPE  InterfaceType;
  VOID                *Protocol;
  EFI_HANDLE          Handle;
} EFI_SMM_PROTO_SERVICES;

typedef struct {
  BOOLEAN           Valid;
  VOID              *Context;
  EFI_GUID          ProtocolGuid;
  EFI_EVENT_NOTIFY  CallbackFunction;
} EFI_SMM_CALLBACK_SERVICES;

typedef struct {
  VOID                      *Context;
  EFI_SMM_RUNTIME_CALLBACK  CallbackFunction;
} EFI_SMM_RT_CALLBACK_SERVICES;

typedef struct {
  UINTN                         Signature;
  UINTN                         CurrentIndex;
  EFI_SMM_PROTO_SERVICES        Services[MAX_SMM_PROTOCOL];
  EFI_SMM_CALLBACK_SERVICES     Callback[MAX_CALLBACK];
  EFI_SMM_RT_CALLBACK_SERVICES  RtCallback[MAX_SM_RT_CALLBACK];
  EFI_HANDLE                    HandleBuffer[MAX_HANDLES];
  EFI_CONFIGURATION_TABLE       ConfigTable[MAX_CONFIG_TABLE];
  EFI_SMM_RUNTIME_PROTOCOL      SmmRtServices;
  EFI_SMM_CALLBACK_ENTRY_POINT  CallbackEntryPoint;
} EFI_SMM_RT_GLOBAL;

//
// BMC Elog Instance signature
//
#define SMM_RT_SIGNATURE                  EFI_SIGNATURE_32 ('s', 'm', 'r', 't')

#define INSTANCE_FROM_EFI_SMM_RT_THIS(a)  CR (a, EFI_SMM_RT_GLOBAL, SmmRtServices, SMM_RT_SIGNATURE)

#endif
