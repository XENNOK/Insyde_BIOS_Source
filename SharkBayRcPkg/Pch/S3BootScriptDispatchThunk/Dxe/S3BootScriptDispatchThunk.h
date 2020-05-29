/** @file
  
  Header file for S3 Boot Script Saver thunk driver.
  
;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

;******************************************************************************
;*Copyright (c) 2010 - 2011, Intel Corporation. All rights reserved.<BR>
;*This program and the accompanying materials
;*are licensed and made available under the terms and conditions of the BSD License
;*which accompanies this distribution.  The full text of the license may be found at
;*http://opensource.org/licenses/bsd-license.php
;*THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
;*WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
;******************************************************************************

*/

#ifndef __S3_BOOT_SCRIPT_DISPATCH_THUNK_H__
#define __S3_BOOT_SCRIPT_DISPATCH_THUNK_H__

#include <FrameworkDxe.h>

#include <Protocol/S3BootScriptDispatch.h>
#include <Protocol/S3SaveState.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/PeCoffLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/LockBoxLib.h>

typedef struct {
  EFI_PHYSICAL_ADDRESS  BootScriptThunkBase;
  EFI_PHYSICAL_ADDRESS  BootScriptThunkLength;
} BOOT_SCRIPT_THUNK_DATA;

typedef
EFI_STATUS
(EFIAPI *DISPATCH_ENTRYPOINT_FUNC) (
  IN EFI_HANDLE ImageHandle,
  IN VOID       *Context
  );

/**
  Internal function to add Save jmp address according to DISPATCH_OPCODE. 
  We ignore "Context" parameter.
  We need create thunk stub to convert PEI entrypoint (used in Framework version)
  to DXE entrypoint (defined in PI spec).

  @param  Marker                The variable argument list to get the opcode
                                and associated attributes.

  @retval EFI_OUT_OF_RESOURCES  Not enough resource to do operation.
  @retval EFI_SUCCESS           Opcode is added.

**/
EFI_STATUS
S3BootScriptDispatch (
  IN EFI_S3_BOOT_SCRIPT_DISPATCH_PROTOCOL   *This,
  IN VOID                                   *EntryPoint
  );

  /**
  Internal function to add Save jmp address according to DISPATCH_OPCODE2. 
  The "Context" parameter is not ignored.

  @param  Marker                The variable argument list to get the opcode
                                and associated attributes.

  @retval EFI_OUT_OF_RESOURCES  Not enough resource to do operation.
  @retval EFI_SUCCESS           Opcode is added.

**/
EFI_STATUS
S3BootScriptDispatch2 (
  IN EFI_S3_BOOT_SCRIPT_DISPATCH_PROTOCOL    *This,
  IN VOID                                    *EntryPoint,
  IN VOID                                    *COntext
  );

#endif
