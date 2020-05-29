/** @file
  Data structures for FAT recovery PEIM

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

  FatPeim.h

Abstract:

  Data structures for FAT recovery PEIM

--*/

#ifndef _FAT_PEIM_H_
#define _FAT_PEIM_H_

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/BlockIo.h>
#include <Ppi/RecoveryModule.h>
#include <Ppi/DeviceRecoveryModule.h>
#include <Ppi/FileAccessPei.h>
#include <Guid/RecoveryDevice.h>

//
// Definitions
//
#define PEI_FAT_CACHE_SIZE        4
#define PEI_FAT_MAX_BLOCK_SIZE    8192
#define FAT_MAX_FILE_NAME_LENGTH  128
#define FAT_MAX_FILE_PATH_LENGTH  256
#define PEI_FAT_MAX_BLOCK_DEVICE  128
#define PEI_FAT_MAX_BLOCK_IO_PPI  32
#define PEI_FAT_MAX_VOLUME        64
#define PEI_FAT_MAX_FILE          32
 
#define PEI_FAT_MEMMORY_PAGE_SIZE 0x1000


//
// Private Data.
// This structure abstracts the whole memory usage in FAT PEIM.
// The entry point routine will get a chunk of memory (by whatever
// means) whose size is sizeof(PEI_FAT_PRIVATE_DATA), which is clean
// in both 32 and 64 bit environment. The boundary of the memory chunk
// should be 64bit aligned.
//
#define PEI_FAT_PRIVATE_DATA_SIGNATURE  SIGNATURE_32 ('p', 'f', 'a', 't')

typedef struct {

  UINTN                                 Signature;
  EFI_PEI_SERVICES                      **PeiServices;
  EFI_PEI_DEVICE_RECOVERY_MODULE_PPI    DeviceRecoveryPpi;
  EFI_PEI_PPI_DESCRIPTOR                PpiDescriptor;
  PEI_FILE_ACCESS_PPI                   *PeiFileAccessPpi;  
} PEI_FAT_PRIVATE_DATA;

#define PEI_FAT_PRIVATE_DATA_FROM_THIS(a) \
  CR (a, \
          PEI_FAT_PRIVATE_DATA, \
          DeviceRecoveryPpi, \
          PEI_FAT_PRIVATE_DATA_SIGNATURE \
      )
  
/**
  Returns the number of DXE capsules residing on the device.

  This function searches for DXE capsules from the associated device and returns
  the number and maximum size in bytes of the capsules discovered. Entry 1 is 
  assumed to be the highest load priority and entry N is assumed to be the lowest 
  priority.

  @param[in]  PeiServices              General-purpose services that are available 
                                       to every PEIM
  @param[in]  This                     Indicates the EFI_PEI_DEVICE_RECOVERY_MODULE_PPI
                                       instance.
  @param[out] NumberRecoveryCapsules   Pointer to a caller-allocated UINTN. On 
                                       output, *NumberRecoveryCapsules contains 
                                       the number of recovery capsule images 
                                       available for retrieval from this PEIM 
                                       instance.

  @retval EFI_SUCCESS        One or more capsules were discovered.
  @retval EFI_DEVICE_ERROR   A device error occurred.
  @retval EFI_NOT_FOUND      A recovery DXE capsule cannot be found.

**/
EFI_STATUS
EFIAPI
GetNumberRecoveryCapsules (
  IN EFI_PEI_SERVICES                               **PeiServices,
  IN EFI_PEI_DEVICE_RECOVERY_MODULE_PPI             *This,
  OUT UINTN                                         *NumberRecoveryCapsules
  );

/**
  Returns the size and type of the requested recovery capsule.

  This function gets the size and type of the capsule specified by CapsuleInstance.

  @param[in]  PeiServices       General-purpose services that are available to every PEIM
  @param[in]  This              Indicates the EFI_PEI_DEVICE_RECOVERY_MODULE_PPI 
                                instance.
  @param[in]  CapsuleInstance   Specifies for which capsule instance to retrieve 
                                the information.  This parameter must be between 
                                one and the value returned by GetNumberRecoveryCapsules() 
                                in NumberRecoveryCapsules.
  @param[out] Size              A pointer to a caller-allocated UINTN in which 
                                the size of the requested recovery module is 
                                returned.
  @param[out] CapsuleType       A pointer to a caller-allocated EFI_GUID in which 
                                the type of the requested recovery capsule is 
                                returned.  The semantic meaning of the value 
                                returned is defined by the implementation.

  @retval EFI_SUCCESS        One or more capsules were discovered.
  @retval EFI_DEVICE_ERROR   A device error occurred.
  @retval EFI_NOT_FOUND      A recovery DXE capsule cannot be found.

**/
EFI_STATUS
EFIAPI
GetRecoveryCapsuleInfo (
  IN  EFI_PEI_SERVICES                              **PeiServices,
  IN  EFI_PEI_DEVICE_RECOVERY_MODULE_PPI            *This,
  IN  UINTN                                         CapsuleInstance,
  OUT UINTN                                         *Size,
  OUT EFI_GUID                                      *CapsuleType
  );

/**
  Loads a DXE capsule from some media into memory.

  This function, by whatever mechanism, retrieves a DXE capsule from some device
  and loads it into memory. Note that the published interface is device neutral.

  @param[in]     PeiServices       General-purpose services that are available 
                                   to every PEIM
  @param[in]     This              Indicates the EFI_PEI_DEVICE_RECOVERY_MODULE_PPI
                                   instance.
  @param[in]     CapsuleInstance   Specifies which capsule instance to retrieve.
  @param[out]    Buffer            Specifies a caller-allocated buffer in which 
                                   the requested recovery capsule will be returned.

  @retval EFI_SUCCESS        The capsule was loaded correctly.
  @retval EFI_DEVICE_ERROR   A device error occurred.
  @retval EFI_NOT_FOUND      A requested recovery DXE capsule cannot be found.

**/
EFI_STATUS
EFIAPI
LoadRecoveryCapsule (
  IN OUT EFI_PEI_SERVICES                         **PeiServices,
  IN EFI_PEI_DEVICE_RECOVERY_MODULE_PPI           *This,
  IN UINTN                                        CapsuleInstance,
  OUT VOID                                        *Buffer
  );

#endif
