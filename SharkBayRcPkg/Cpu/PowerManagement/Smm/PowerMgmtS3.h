/** @file
  
  Header file for PpmS3 Smm Driver.  

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _POWER_MGMT_S3_SMM_H_
#define _POWER_MGMT_S3_SMM_H_

#include <PchAccess.h>
#include <Library/DebugLib.h>
#include <Library/PchPlatformLib.h>
#include <Library/UefiBootServicesTableLib.h>

#define PPM_WAIT_PERIOD 15

typedef UINT16                    STRING_REF;

typedef struct _EFI_MSR_VALUES {
  UINT16  Index;
  UINT64  Value;
  UINT64  BitMask;
  BOOLEAN RestoreFlag;
} EFI_MSR_VALUES;

/**
  Runs the specified procedure on all logical processors, passing in the
  parameter buffer to the procedure.

  @param[in] Procedure     The function to be run.
  @param[in] Buffer        Pointer to a parameter buffer.

  @retval EFI_SUCCESS
**/
STATIC
EFI_STATUS
RunOnAllLogicalProcessors (
  IN OUT EFI_AP_PROCEDURE Procedure,
  IN OUT VOID             *Buffer
  );
/**
  This function will restore MSR settings.

  This function must be MP safe.

  @param[in] Buffer        Unused

  @retval EFI_SUCCESS   MSR restored
**/
VOID
EFIAPI
ApSafeRestoreMsr (
  IN OUT VOID *Buffer
  );

#endif
