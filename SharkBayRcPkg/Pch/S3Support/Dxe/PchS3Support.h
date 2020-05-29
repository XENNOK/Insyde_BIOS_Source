/** @file
  Header file for PCH S3 Support driver

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

/**
  This file contains an 'Intel Peripheral Driver' and uniquely        
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your   
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  PchS3Support.h

@brief
  Header file for PCH S3 Support driver

**/
#ifndef _PCH_S3_SUPPORT_DRIVER_H_
#define _PCH_S3_SUPPORT_DRIVER_H_


//
// Driver Consumed Protocol Prototypes
//
#include <Protocol/LoadPe32Image.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/FirmwareVolume.h>
#include <Protocol/DevicePath.h>

//
// Driver Produced Protocol Prototypes
//
#include <Protocol/PchS3Support.h>

#include <PchAccess.h>
#include <Library/PchPlatformLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>

#include <PchInitVar.h>

#define PCH_S3_PEIM_VARIABLE_NAME L"PchS3Peim"

typedef struct {
  EFI_PHYSICAL_ADDRESS  EntryPointAddress;
  UINTN                 PeimSize;
} PCH_S3_PEIM_VARIABLE;

#define EFI_PCH_S3_IMAGE_GUID \
  { \
    0x271dd6f2, 0x54cb, 0x45e6, \
    { \
      0x85, 0x85, 0x8c, 0x92, 0x3c, 0x1a, 0xc7, 0x6 \
    } \
  }
//
// Function prototypes
//

/**
  Set an item to be dispatched at S3 resume time. At the same time, the entry point
  of the PCH S3 support image is returned to be used in subsequent boot script save
  call

  @param[in] This                       Pointer to the protocol instance.
  @param[in] DispatchItem               The item to be dispatched.
  @param[out] S3DispatchEntryPoint      The entry point of the PCH S3 support image.

  @retval EFI_STATUS                    Successfully completed.
  @retval EFI_OUT_OF_RESOURCES          Out of resources.
**/
EFI_STATUS
EFIAPI
PchS3SetDispatchItem (
  IN     EFI_PCH_S3_SUPPORT_PROTOCOL   *This,
  IN     EFI_PCH_S3_DISPATCH_ITEM      *DispatchItem,
  OUT    EFI_PHYSICAL_ADDRESS          *S3DispatchEntryPoint
  );

/**
  Set the Pch Init Variable for consumption by PchInitS3 PEIM.
  @bug: expect to extend the variable service to support <4G EfiReservedMemory
  variable storage, such that memory consumption is flexible and more economical.

  @param[in] VOID

  @retval None
**/
EFI_STATUS
SetPchInitVariable (
  VOID
  );

/**
  Load the PCH S3 Image into Efi Reserved Memory below 4G.

  @param[out] ImageEntryPoint     The ImageEntryPoint after success loading

  @retval EFI_STATUS
**/
EFI_STATUS
LoadPchS3Image (
  OUT   UINT32          *ImageEntryPoint
  );

#endif
