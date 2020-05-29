/** @file
  Header file for support functions can work both in SMM mode.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SMM_FUNCTIONS_H_
#define _SMM_FUNCTIONS_H_
#include "FvbServicesRuntimeDxe.h"
#include <SmiTable.h>

#include <Protocol/SmmFwBlockService.h>

#define SW_SMI_PORT      PcdGet16(PcdSoftwareSmiPort)

typedef struct {
  UINT32                                Signature;
  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL     *SmmFwbService;
  EFI_SMM_CPU_PROTOCOL                  *SmmCpu;
  UINTN                                 CurrentlyExecutingCpu;
} SMM_FW_VOL_INSTANCE;


/**
  Initailize SMM FVB access service.

  1.Initialize moudle SmmFwVolInstance.
  2.Register SMM callback function to provide SMM FVB access interface.

  @retval EFI_SUCCESS   Initialize SMM FVB access service.
  @retval Other         Any error occurred while initializing SMM FVB service.
**/
EFI_STATUS
InitializeSmmFvbAccess (
  VOID
  );

//
// Access FV through SMI relative functions
//
/**
  Function to do flash read through SMI.

  @param[in]      ReadAddress  Target address to be read.
  @param[in, out] ReadLength   In:  Input buffer size in bytes.
                               Out: Total read data size in bytes.
  @param[out]     ReadBuffer   Output buffer to contains read data.

  @retval EFI_SUCCESS       Data successfully read from flash device.
  @retval EFI_UNSUPPORTED   The flash device is not supported.
  @retval EFI_DEVICE_ERROR  Failed to read the blocks.
**/
EFI_STATUS
ReadFdThroughSmi (
  IN       UINTN      ReadAddress,
  IN OUT   UINTN      *ReadLength,
  OUT      VOID       *ReadBuffer
  );

/**
  Function to do flash write through SMI.

  @param[in]      WriteAddress  Target address to write.
  @param[in, out] WriteLength   In:  Input buffer size in bytes.
                                Out: Total write data size in bytes.
  @param[out]     WriteBuffer   input buffer to write.

  @retval EFI_SUCCESS       Data successfully write to flash device.
  @retval EFI_UNSUPPORTED   The flash device is not supported.
  @retval EFI_DEVICE_ERROR  Failed to write the blocks.
**/
EFI_STATUS
WriteFdThroughSmi (
  IN       UINTN      WriteAddress,
  IN OUT   UINTN      *WriteLength,
  OUT      VOID       *WriteBuffer
  );

/**
  Function to do flash erase through SMI..

  @param[in] EraseAddress   Target address to erase.
  @param[in] EraseLength    Erase size in bytes.

  @retval EFI_SUCCESS           Erase flash block successful.
  @retval EFI_UNSUPPORTED       The flash device is not supported.
  @retval EFI_DEVICE_ERROR      Failed to erase blocks.
**/
EFI_STATUS
EraseFdThroughSmi (
  IN  UINTN      EraseAddress,
  IN  UINTN      EraseLength
  );


#endif