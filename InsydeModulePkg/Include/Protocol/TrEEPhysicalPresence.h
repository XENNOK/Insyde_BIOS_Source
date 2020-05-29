/** @file
  TrEE Physical Presence Protocol

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _TREE_PHYSICAL_PRESENCE_PROTOCOL_H_
#define _TREE_PHYSICAL_PRESENCE_PROTOCOL_H_

#include "IndustryStandard/Tpm12.h"

#define EFI_TREE_PHYSICAL_PRESENCE_PROTOCOL_GUID  \
  {0x469718df, 0x6f19, 0x43e7, 0x90, 0x3b, 0xea, 0x6b, 0x3, 0x55, 0x55, 0xbb}

typedef struct _EFI_TREE_PHYSICAL_PRESENCE_PROTOCOL EFI_TREE_PHYSICAL_PRESENCE_PROTOCOL;

/**
  Execute physical presence operation requested by the OS.

  @param[in]      This                        EFI TrEE Protocol instance.
  @param[in]      CommandCode                 Physical presence operation value.
  @param[out]     LastReturnedCode            Execute result
  @param[in, out] PpiFlags                    The physical presence interface flags.

  @retval EFI_SUCCESS                         Operation completed successfully.
  @retval EFI_INVALID_PARAMETER               One or more of the parameters are incorrect.
  @retval Others                              Unexpected error.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_EXECUTE_TREE_PHYSICAL_PRESENCE) (
  IN      EFI_TREE_PHYSICAL_PRESENCE_PROTOCOL *This,
  IN      UINT8                               CommandCode,
  OUT     TPM_RESULT                          *LastReturnedCode,
  IN OUT  UINT8                               *PpiFlags OPTIONAL
  );

struct _EFI_TREE_PHYSICAL_PRESENCE_PROTOCOL {
  EFI_EXECUTE_TREE_PHYSICAL_PRESENCE          ExecutePhysicalPresence;
};

extern EFI_GUID gEfiTrEEPhysicalPresenceProtocolGuid;
#endif //_TREE_PHYSICAL_PRESENCE_PROTOCOL_H_