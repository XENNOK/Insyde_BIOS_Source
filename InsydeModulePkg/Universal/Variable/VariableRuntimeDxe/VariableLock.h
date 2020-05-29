/** @file
    The header file for variable lock service.

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _VARIABLE_LOCK_H_
#define _VARIABLE_LOCK_H_
#include "Variable.h"

typedef struct {
  EFI_GUID    Guid;
  CHAR16      *Name;
  LIST_ENTRY  Link;
} VARIABLE_ENTRY;

/**
  Mark a variable that will become read-only after leaving the DXE phase of execution.

  @param[in] This          The VARIABLE_LOCK_PROTOCOL instance.
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
EFI_STATUS
EFIAPI
VariableLockRequestToLock (
  IN CONST EDKII_VARIABLE_LOCK_PROTOCOL *This,
  IN       CHAR16                       *VariableName,
  IN       EFI_GUID                     *VendorGuid
  );

/**
  Convert all of variables in variable list to virtual address.
**/
VOID
ConvertVariablLockList (
  VOID
  );

/**
  Function uses to check this variable is whether locked.

  @param[in] VariableName  Name of Variable to be found.
  @param[in] VendorGuid    Variable vendor GUID.

  @retval TRUE             This variable is locked.
  @retval FALSE            This variable isn't locked.
**/
BOOLEAN
IsVariableLocked (
  IN CHAR16                  *VariableName,
  IN EFI_GUID                *VendorGuid
  );

/**
  This function uses to send SMI to create vriable lock list in SMM mode.
**/
VOID
CreateVariableLockListThroughSmi (
  VOID
  );

/**
  This function uses to create variable lock list.

  @return EFI_SUCCESS    Disable all secure boot SMI functions successful.
  @return Other          Any erro occured while disabling all secure boot SMI functions successful.
**/
EFI_STATUS
SmmCreateVariableLockList (
  VOID
  );

#endif
