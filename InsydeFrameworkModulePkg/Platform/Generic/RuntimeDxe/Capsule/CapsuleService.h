//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Copyright (c) 2006, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

  CapsuleService.h

Abstract:

  Capsule Runtime Service

--*/

#ifndef  _CAPSULE_RUNTIME_H_
#define  _CAPSULE_RUNTIME_H_

#include "Tiano.h"
#include "EfiRuntimeLib.h"
#include EFI_GUID_DEFINITION(Capsule)
#include EFI_ARCH_PROTOCOL_DEFINITION (Capsule)
#include EFI_GUID_DEFINITION(EfiSystemResourceTable)

#define MAX_SIZE_POPULATE              0xA00000
#define MAX_SIZE_NON_POPULATE          0x6400000


EFI_STATUS
EFIAPI
UpdateCapsule(
  IN EFI_CAPSULE_HEADER      **CapsuleHeaderArray,
  IN UINTN                   CapsuleCount,
  IN EFI_PHYSICAL_ADDRESS    ScatterGatherList OPTIONAL
  );

EFI_STATUS
EFIAPI
QueryCapsuleCapabilities(
  IN  EFI_CAPSULE_HEADER   **CapsuleHeaderArray,
  IN  UINTN                CapsuleCount,
  OUT UINT64               *MaxiumCapsuleSize,
  OUT EFI_RESET_TYPE       *ResetType
  );

BOOLEAN
EFIAPI
SupportUpdateCapsuleRest (
  VOID
  );

VOID
EFIAPI
SupportCapsuleSize (
  IN OUT UINT32 *MaxSizePopulate,
  IN OUT UINT32 *MaxSizeNonPopulate
  );


EFI_STATUS
EFIAPI
PersistCapsuleImage (
  IN EFI_CAPSULE_HEADER *CapsuleHeader
  );
  
/**
  Get system firmware revision for ESRT from capsule image

  @param  CapsuleHeader         Points to a capsule header.

  @return                       The system firmware revision from the capsule image
                                If the signature cannot be found, 0x00000000 will
                                be returned
**/
UINT32
EFIAPI
GetCapsuleSystemFirmwareVersion (
  IN EFI_CAPSULE_HEADER  *CapsuleHeader
  );
  
/**
  Pre-installation check for Capsule Update

  @param  CapsuleHeader                      Points to a capsule header.

  @retval ESRT_SUCCESS                       The Capsule passed the pre-installation criteria
  @retval ESRT_ERROR_UNSUCCESSFUL            The pre-installation criteria check failed
  @retval ESRT_ERROR_INSUFFICIENT_RESOURCES  Out of memory or persistent storage
  @retval ESRT_ERROR_INCORRECT_VERSION       Incorrect/incompatible firmware version
  @retval ESRT_ERROR_INVALID_IMAGE_FORMAT    Invalid Capsule image format
  @retval ESRT_ERROR_AUTHENTICATION          Capsule image authentication failed
  @retval ESRT_ERROR_AC_NOT_CONNECTED        The system is not connected to the AC power
  @retval ESRT_ERROR_INSUFFICIENT_BATTERY    The battery capacity is low

**/
ESRT_STATUS
EFIAPI
PreInstallationCheck (
  EFI_CAPSULE_HEADER *Capsule
  );
  
/**
  Post-installation check for Capsule Update

  @param  CapsuleHeader                      Points to a capsule header.

  @retval ESRT_SUCCESS                       The Capsule passed the pre-installation criteria
  @retval ESRT_ERROR_UNSUCCESSFUL            The pre-installation criteria check failed
  @retval ESRT_ERROR_INSUFFICIENT_RESOURCES  Out of memory or persistent storage
  @retval ESRT_ERROR_AUTHENTICATION          Capsule image authentication failed

**/
ESRT_STATUS
EFIAPI
PostInstallationCheck (
  EFI_CAPSULE_HEADER *Capsule
  );
  
  
VOID
EFIAPI
TriggerFirmwareUpdate (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  );
  
EFI_STATUS
EnableCapsuleSecurityCheck (
  VOID
  );
EFI_STATUS
DisableCapsuleSecurityCheck (
  VOID
  );    

EFI_STATUS
EFIAPI
InstallEfiSystemResourceTable (
  VOID
  );
#endif

