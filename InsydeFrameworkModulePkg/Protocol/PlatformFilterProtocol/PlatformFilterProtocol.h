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

  PlatformFilterProtocol.h

Abstract:

  Platform Filter Protocol as defined in EFI 2.0

  This code abstracts the Platform Filter Protocol

--*/

#ifndef _PLATFORM_FILTER_PROTOCOL_
#define _PLATFORM_FILTER_PROTOCOL_

#define EFI_PLATFORM_FILTER_PROTOCOL_GUID \
  { \
    0xaacda104, 0xe5b4, 0x4430, 0xaf, 0x9e, 0xa7, 0x4b, 0x8c, 0x63, 0xff, 0xd \
  }

#define PLATFORM_FILTER_SIGNATURE EFI_SIGNATURE_32 ('p', 'i', 'n', 'f')

EFI_FORWARD_DECLARATION (EFI_PLATFORM_FILTER_PROTOCOL);

//
// *******************************************************
// EFI_GUID_TYPE
// *******************************************************
//
typedef enum {
  EfiRuntime,
  SalRuntime,
  EfiBootTime,
  Unknown
} EFI_GUID_TYPE;

//
// *******************************************************
// EFI_PLATFORM_PROC
// *******************************************************
//
typedef
EFI_STATUS
(EFIAPI *EFI_PLATFORM_PROC) (
  IN  EFI_GUID                                    * SourceGuid,
  IN  EFI_GUID                                    * ConsumedGuid,
  IN  VOID                                        *ProtocolInstance1,
  IN  VOID                                        *ProtocolInstance2,
  IN  EFI_GUID_TYPE                               ExecutionType,
  IN  VOID                                        *PlatformGlobal
  );

//
// *******************************************************
// EFI_REGISTER_PLATFORM_BINDING
// *******************************************************
//
typedef
EFI_STATUS
EFI_RUNTIMESERVICE
(EFIAPI *EFI_PERFORM_PLATFORM_BINDING) (
  IN EFI_PLATFORM_FILTER_PROTOCOL                 * This,
  IN  EFI_GUID_TYPE                               ExecutionType,
  IN  EFI_GUID                                    * ProducerGuid,
  IN  VOID                                        *ProducerProto,
  IN  EFI_GUID                                    * ConsumerGuid,
  OUT VOID                                        *ConsumerProto
  );

//
// *******************************************************
// EFI_REGISTER_PLATFORM_BINDING
// *******************************************************
//
typedef
EFI_STATUS
EFI_RUNTIMESERVICE
(EFIAPI *EFI_REGISTER_PLATFORM_BINDING) (
  IN EFI_PLATFORM_FILTER_PROTOCOL                 * This,
  IN  BOOLEAN                                     Register,
  IN  EFI_GUID_TYPE                               ExecutionType,
  IN  EFI_GUID                                    * SourceGuid,
  IN  EFI_GUID                                    * ConsumedGuid,
  IN  VOID                                        *PlatformGlobal,
  IN  EFI_PLATFORM_PROC                           PlatformProc
  );

//
// *******************************************************
// EFI_CPU_IO_PROTOCOL
// *******************************************************
//
typedef struct _EFI_PLATFORM_FILTER_PROTOCOL {
  EFI_REGISTER_PLATFORM_BINDING RegisterPlatformBinding;
  EFI_PERFORM_PLATFORM_BINDING  PerformPlatformBinding;
} EFI_PLATFORM_FILTER_PROTOCOL;

extern EFI_GUID gEfiPlatformFilterProtocolGuid;

#endif
