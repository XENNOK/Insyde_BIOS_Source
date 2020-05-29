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

Copyright (c)  2005 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  TpmPhysicalPresenceProtocol.h

Abstract:

  Tpm Physical Presence Protocol

--*/

#ifndef _TPM_PHYSICAL_PRESENCE_PROTOCOL_H_
#define _TPM_PHYSICAL_PRESENCE_PROTOCOL_H_

#include EFI_PROTOCOL_CONSUMER (TcgService)

#define EFI_TPM_PHYSICAL_PRESENCE_PROTOCOL_GUID  \
  {0x15f81fc9, 0x9304, 0x407b, 0x89, 0xc4, 0x2c, 0xac, 0x35, 0x86, 0x10, 0xa7}

EFI_FORWARD_DECLARATION (EFI_TPM_PHYSICAL_PRESENCE_PROTOCOL);

#define TPM_PP_USER_ABORT           ((TPM_RESULT)(-0x10))
#define TPM_PP_BIOS_FAILURE         ((TPM_RESULT)(-0x0f))

#pragma pack (push, 1)
//
// BIOS Management Flags (TCG Physical Presence Interface 1.2) 
//
typedef struct _EFI_TCG_TPM_MANAGEMENT_FLAG {
  BOOLEAN                           NoPPIProvision;         //For Enable, Disable, Activate, Deactivate, SetOwnerInstall_True, SetOwnerInstall_False, SetOperatorAuth
  BOOLEAN                           NoPPIClear;             //For TPM_ForceClear
  BOOLEAN                           NoPPIMaintenance;       //For Deferred Physical Presence - unownedFieldUpgrade
} EFI_TCG_TPM_MANAGEMENT_FLAG;
#pragma pack (pop)

typedef
EFI_STATUS
(EFIAPI *EFI_TPM_PHYSICAL_PRESENCE) (
  IN      EFI_TCG_PROTOCOL                 *TcgProtocol,
  IN      UINT8                            CommandCode,
  OUT     TPM_RESULT                       *Result
  );

typedef
EFI_STATUS
(EFIAPI *EFI_TCG_GET_TPM_MANAGEMENT_FLAG) (
  IN      EFI_TCG_PROTOCOL                 *TcgProtocol,
  OUT     EFI_TCG_TPM_MANAGEMENT_FLAG      *TpmMangFlag
  );
  
typedef
EFI_STATUS
(EFIAPI *EFI_TCG_MANIPULATE_PHYSICAL_PRESENCE) (
  IN      EFI_TCG_PROTOCOL                 *TcgProtocol,
  IN      TPM_PHYSICAL_PRESENCE            PhysicalPresence
  );

typedef
TPM_RESULT
(EFIAPI *EFI_TPM_NV_LOCK) (
  IN      EFI_TCG_PROTOCOL                 *TcgProtocol
  );

typedef struct _EFI_TPM_PHYSICAL_PRESENCE_PROTOCOL {
  EFI_TPM_PHYSICAL_PRESENCE                TpmPhysicalPresence;
  EFI_TCG_GET_TPM_MANAGEMENT_FLAG          TcgGetTpmMangFlag;
  EFI_TCG_MANIPULATE_PHYSICAL_PRESENCE     ManipulatePhysicalPresence;
  EFI_TPM_NV_LOCK                          TpmNvLock;
} EFI_TPM_PHYSICAL_PRESENCE_PROTOCOL;

extern EFI_GUID gEfiTpmPhysicalPresenceProtocolGuid;

#endif
