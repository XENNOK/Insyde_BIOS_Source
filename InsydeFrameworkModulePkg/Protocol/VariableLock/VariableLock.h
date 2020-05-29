//;******************************************************************************
//;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/** @file
  Variable Lock Protocol is related to EDK II-specific implementation of variables
  and intended for use as a means to mark a variable read-only after the event
  EFI_END_OF_DXE_EVENT_GUID is signaled.

  Copyright (c) 2013, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __VARIABLE_LOCK_H__
#define __VARIABLE_LOCK_H__


#define EDKII_VARIABLE_LOCK_PROTOCOL_GUID \
  { \
    0xcd3d0a05, 0x9e24, 0x437c, { 0xa8, 0x91, 0x1e, 0xe0, 0x53, 0xdb, 0x76, 0x38 } \
  }

typedef struct _EDKII_VARIABLE_LOCK_PROTOCOL  EDKII_VARIABLE_LOCK_PROTOCOL;

/**
  Mark a variable that will become read-only after leaving the DXE phase of execution.
  Write request coming from SMM environment through EFI_SMM_VARIABLE_PROTOCOL is allowed.

  @param[in] This          The EDKII_VARIABLE_LOCK_PROTOCOL instance.
  @param[in] VariableName  A pointer to the variable name that will be made read-only subsequently.
  @param[in] VendorGuid    A pointer to the vendor GUID that will be made read-only subsequently.

  @retval EFI_SUCCESS           The variable specified by the VariableName and the VendorGuid was marked
                                as pending to be read-only.
  @retval EFI_INVALID_PARAMETER VariableName or VendorGuid is NULL.
                                Or VariableName is an empty string.
  @retval EFI_ACCESS_DENIED     EFI_END_OF_DXE_EVENT_GROUP_GUID or EFI_EVENT_GROUP_READY_TO_BOOT has
                                already been signaled.
  @retval EFI_OUT_OF_RESOURCES  There is not enough resource to hold the lock request.
**/
typedef
EFI_STATUS
(EFIAPI * EDKII_VARIABLE_LOCK_PROTOCOL_REQUEST_TO_LOCK) (
  IN CONST EDKII_VARIABLE_LOCK_PROTOCOL *This,
  IN       CHAR16                       *VariableName,
  IN       EFI_GUID                     *VendorGuid
  );

///
/// Variable Lock Protocol is related to EDK II-specific implementation of variables
/// and intended for use as a means to mark a variable read-only after the event
/// EFI_END_OF_DXE_EVENT_GUID is signaled.
///
struct _EDKII_VARIABLE_LOCK_PROTOCOL {
  EDKII_VARIABLE_LOCK_PROTOCOL_REQUEST_TO_LOCK RequestToLock;
};

extern EFI_GUID gEdkiiVariableLockProtocolGuid;

#endif
