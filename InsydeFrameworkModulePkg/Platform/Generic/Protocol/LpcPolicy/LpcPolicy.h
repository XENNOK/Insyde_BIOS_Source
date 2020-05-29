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
  LpcPolicy.h

Abstract:
  This file defines platform policies for LPC chip driver.

--*/

#ifndef _LPC_POLICY_H_
#define _LPC_POLICY_H_

#include "Tiano.h"
#include "TianoTypes.h"

#define EFI_LPC_POLICY_PROTOCOL_GUID \
  { \
    0x64021DFE, 0xA62C, 0x42A7, 0xBF, 0x46, 0x15, 0x07, 0x8C, 0xDF, 0x9F, 0x89 \
  }

EFI_FORWARD_DECLARATION (EFI_LPC_POLICY_PROTOCOL);

#define EFI_LPC_POLICY_PROTOCOL_COM_MAX 4

typedef struct {
  BOOLEAN Enable[EFI_LPC_POLICY_PROTOCOL_COM_MAX];
  UINT16  Address[EFI_LPC_POLICY_PROTOCOL_COM_MAX];
  UINT16  Irq[EFI_LPC_POLICY_PROTOCOL_COM_MAX];
} EFI_LPC_POLICY_PROTOCOL_COM;

#define EFI_LPC_POLICY_PROTOCOL_PARALLEL_MAX  4

typedef enum {
  EfiLpcPolicyParallelOutOnly,
  EfiLpcPolicyParallelBiDir,
  EfiLpcPolicyParallelEpp,
  EfiLpcPolicyParallelEcp,
  EfiLpcPolicyParallelLast
} EFI_LPC_POLICY_PROTOCOL_PARALLEL_MODE;

typedef struct {
  BOOLEAN                               Enable[EFI_LPC_POLICY_PROTOCOL_PARALLEL_MAX];
  UINT16                                Address[EFI_LPC_POLICY_PROTOCOL_PARALLEL_MAX];
  UINT16                                Irq[EFI_LPC_POLICY_PROTOCOL_PARALLEL_MAX];
  UINT16                                DmaChannel[EFI_LPC_POLICY_PROTOCOL_PARALLEL_MAX];
  EFI_LPC_POLICY_PROTOCOL_PARALLEL_MODE Mode[EFI_LPC_POLICY_PROTOCOL_PARALLEL_MAX];
} EFI_LPC_POLICY_PROTOCOL_PARALLEL;

typedef struct {
  BOOLEAN Enable;
} EFI_LPC_POLICY_PROTOCOL_KEYBOARD;

typedef struct {
  BOOLEAN Enable;
} EFI_LPC_POLICY_PROTOCOL_MOUSE;

typedef enum {
  EfiLpcPolicyFloppy0M36,
  EfiLpcPolicyFloppy1M20,
  EfiLpcPolicyFloppy0M72,
  EfiLpcPolicyFloppy1M44,
  EfiLpcPolicyFloppy2M88,
  EfiLpcPolicyFloppyLast
} EFI_LPC_POLICY_PROTOCOL_FLOPPY_TYPE;

#define EFI_LPC_POLICY_PROTOCOL_FLOPPY_MAX  4

typedef struct {
  BOOLEAN                             Enable[EFI_LPC_POLICY_PROTOCOL_FLOPPY_MAX];
  EFI_LPC_POLICY_PROTOCOL_FLOPPY_TYPE Type[EFI_LPC_POLICY_PROTOCOL_FLOPPY_MAX];
} EFI_LPC_POLICY_PROTOCOL_FLOPPY;

typedef struct _EFI_LPC_POLICY_PROTOCOL {
  EFI_LPC_POLICY_PROTOCOL_COM       PolicyCom;
  EFI_LPC_POLICY_PROTOCOL_PARALLEL  PolicyParallel;
  EFI_LPC_POLICY_PROTOCOL_FLOPPY    PolicyFloppy;
  EFI_LPC_POLICY_PROTOCOL_KEYBOARD  PolicyKeyboard;
  EFI_LPC_POLICY_PROTOCOL_MOUSE     PolicyMouse;
} EFI_LPC_POLICY_PROTOCOL;

extern EFI_GUID gEfiLpcPolicyProtocolGuid;

#endif
