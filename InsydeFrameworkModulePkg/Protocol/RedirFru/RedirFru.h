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

Copyright (c)  1999 - 2001 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  RedirFru.h

Abstract:

  REDIR FRU Protocol as defined in Tiano

  This code abstracts the generic FRU Protocol

--*/

#ifndef _REDIR_FRU_H_
#define _REDIR_FRU_H_

#define EFI_SM_FRU_REDIR_PROTOCOL_GUID \
  { \
    0x28638cfa, 0xea88, 0x456c, 0x92, 0xa5, 0xf2, 0x49, 0xca, 0x48, 0x85, 0x35 \
  }

#define EFI_SM_FRU_REDIR_SIGNATURE  EFI_SIGNATURE_32 ('f', 'r', 'r', 'x')

EFI_FORWARD_DECLARATION (EFI_SM_FRU_REDIR_PROTOCOL);

//
//  Redir FRU Function Prototypes
//
typedef
EFI_STATUS
(EFIAPI *EFI_GET_FRU_REDIR_INFO) (
  IN EFI_SM_FRU_REDIR_PROTOCOL            * This,
  IN  UINTN                               FruSlotNumber,
  OUT EFI_GUID                            * FruFormatGuid,
  OUT UINTN                               *DataAccessGranularity,
  OUT CHAR16                              **FruInformationString
  );

typedef
EFI_STATUS
(EFIAPI *EFI_GET_FRU_SLOT_INFO) (
  IN EFI_SM_FRU_REDIR_PROTOCOL            * This,
  OUT EFI_GUID                            * FruTypeGuid,
  OUT UINTN                               *StartFruSlotNumber,
  OUT UINTN                               *NumSlots
  );

typedef
EFI_STATUS
(EFIAPI *EFI_GET_FRU_REDIR_DATA) (
  IN EFI_SM_FRU_REDIR_PROTOCOL            * This,
  IN  UINTN                               FruSlotNumber,
  IN  UINTN                               FruDataOffset,
  IN  UINTN                               FruDataSize,
  IN  UINT8                               *FruData
  );

typedef
EFI_STATUS
(EFIAPI *EFI_SET_FRU_REDIR_DATA) (
  IN EFI_SM_FRU_REDIR_PROTOCOL            * This,
  IN  UINTN                               FruSlotNumber,
  IN  UINTN                               FruDataOffset,
  IN  UINTN                               FruDataSize,
  IN  UINT8                               *FruData
  );

//
// REDIR FRU PROTOCOL
//
typedef struct _EFI_SM_FRU_REDIR_PROTOCOL {
  EFI_GET_FRU_REDIR_INFO  GetFruRedirInfo;
  EFI_GET_FRU_SLOT_INFO   GetFruSlotInfo;
  EFI_GET_FRU_REDIR_DATA  GetFruRedirData;
  EFI_SET_FRU_REDIR_DATA  SetFruRedirData;
} EFI_SM_FRU_REDIR_PROTOCOL;

extern EFI_GUID gEfiRedirFruProtocolGuid;

#endif
