//;******************************************************************************
//;* Copyright (c) 2012-2014, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _TREE_PHYSICAL_PRESENCE_PROTOCOL_H_
#define _TREE_PHYSICAL_PRESENCE_PROTOCOL_H_

#include "EfiTpm.h"
#include "Numbers.h"

#define EFI_TREE_PHYSICAL_PRESENCE_PROTOCOL_GUID  \
  {0x469718df, 0x6f19, 0x43e7, 0x90, 0x3b, 0xea, 0x6b, 0x3, 0x55, 0x55, 0xbb}

EFI_FORWARD_DECLARATION (EFI_TREE_PHYSICAL_PRESENCE_PROTOCOL);

#ifndef EFI_TREE_PHYSICAL_PRESENCE_DATA_GUID
#define EFI_TREE_PHYSICAL_PRESENCE_DATA_GUID \
  {0xf24643c2, 0xc622, 0x494e, 0x8a, 0xd, 0x46, 0x32, 0x57, 0x9c, 0x2d, 0x5b}
#endif

#ifndef TREE_PHYSICAL_PRESENCE_VARIABLE
#define TREE_PHYSICAL_PRESENCE_VARIABLE         L"TrEEPhysicalPresence"
#endif

#ifndef TREE_PHYSICAL_PRESENCE_FLAGS_VARIABLE
#define TREE_PHYSICAL_PRESENCE_FLAGS_VARIABLE  L"TrEEPhysicalPresenceFlags"
#endif

#define TPM_PP_SUCCESS                          ((TPM_RESULT) 0x0)
#define TPM_PP_USER_ABORT                       ((TPM_RESULT)(-0x10))
#define TPM_PP_BIOS_FAILURE                     ((TPM_RESULT)(-0x0f))

//
// The definition bit of the flags
//
#define FLAG_NO_PPI_PROVISION                   BIT0
#define FLAG_NO_PPI_CLEAR                       BIT1
#define FLAG_NO_PPI_MAINTENANCE                 BIT2
#define FLAG_RESET_TRACK                        BIT3

//
// The definition of physical presence operation actions
//
#define TPM2_PPI_NOOP_0                         0
#define TPM2_PPI_NOOP_1                         1
#define TPM2_PPI_NOOP_2                         2
#define TPM2_PPI_NOOP_3                         3
#define TPM2_PPI_NOOP_4                         4
#define TPM2_PPI_CLEAR_CONTROL_CLEAR_5          5
#define TPM2_PPI_NOOP_6                         6
#define TPM2_PPI_NOOP_7                         7
#define TPM2_PPI_NOOP_8                         8
#define TPM2_PPI_NOOP_9                         9
#define TPM2_PPI_NOOP_10                        10
#define TPM2_PPI_NOOP_11                        11
#define TPM2_PPI_NOOP_12                        12
#define TPM2_PPI_NOOP_13                        13
#define TPM2_PPI_CLEAR_CONTROL_CLEAR_14         14
#define TPM2_PPI_NOOP_15                        15
#define TPM2_PPI_NOOP_16                        16
#define TPM2_PPI_SET_NO_PPI_CLEAR_FALSE         17
#define TPM2_PPI_SET_NO_PPI_CLEAR_TRUE          18
#define TPM2_PPI_NOOP_19                        19
#define TPM2_PPI_NOOP_20                        20
#define TPM2_PPI_CLEAR_CONTROL_CLEAR_21         21
#define TPM2_PPI_CLEAR_CONTROL_CLEAR_22         22

#pragma pack (push, 1)
//
// Physical Presence request command
//
typedef struct {
  UINT8                                         PPRequest;
  UINT8                                         LastPPRequest;
  UINT32                                        PPResponse;
} EFI_TREE_PHYSICAL_PRESENCE;
#pragma pack (pop)

typedef
EFI_STATUS
(EFIAPI *EFI_EXECUTE_TREE_PHYSICAL_PRESENCE) (
  IN      EFI_TREE_PHYSICAL_PRESENCE_PROTOCOL   *This,
  IN      UINT8                                 CommandCode,
  OUT     TPM_RESULT                            *LastReturnedCode,
  IN OUT  UINT8                                 *PpiFlags OPTIONAL
  );
/*++

  Routine Description:
    Execute physical presence operation requested by the OS.

  Arguments:
    This                    - EFI TrEE Protocol instance.
    CommandCode             - Physical presence operation value.
    LastReturnedCode        - Execute result
    PpiFlags                - The physical presence interface flags.

  Returns:
    EFI_SUCCESS             - Executed the command successfully.
    EFI_INVALID_PARAMETER   - Input is invalid.
    Others                  - Unexpected error.

--*/

typedef struct _EFI_TREE_PHYSICAL_PRESENCE_PROTOCOL {
  EFI_EXECUTE_TREE_PHYSICAL_PRESENCE            ExecutePhysicalPresence;
} EFI_TREE_PHYSICAL_PRESENCE_PROTOCOL;

extern EFI_GUID gEfiTrEEPhysicalPresenceProtocolGuid;
extern EFI_GUID gEfiTrEEPhysicalPresenceGuid;
#endif
