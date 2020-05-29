/** @file
  Defines and prototypes for the IoTrap SMM driver

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corporation. All Rights Reserved.
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
  Copyright (c) 2006 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  IoTrap.h

@brief
  Defines and prototypes for the IoTrap SMM driver
  
**/
#ifndef _IO_TRAP_H_
#define _IO_TRAP_H_

//
// Include files
//
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/S3BootScriptLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>

//
// Driver Consumed Protocol
//
#include <Protocol/SmmBase.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/SmmIchnDispatch.h>

//
// Driver Produced Protocol
//
#include <Protocol/SmmIoTrapDispatch.h>
#include <Protocol/PchSmmIoTrapControl.h>
#include <PchAccess.h>

#define IO_TRAP_HANDLER_NUM 4

///
/// Driver private data
///
#define IO_TRAP_INSTANCE_SIGNATURE  SIGNATURE_32 ('I', 'O', 'T', 'P')

//
//  Bad pointer value to use in check builds.
//  if you see this value you are using uninitialized or free'ed data
//
#define EFI_BAD_POINTER          0xAFAFAFAF
#define EFI_BAD_POINTER_AS_BYTE  0xAF

typedef struct {
  UINT32                            Signature;
  EFI_HANDLE                        Handle;
  EFI_SMM_IO_TRAP_DISPATCH_PROTOCOL EfiSmmIoTrapDispatchProtocol;
  EFI_HANDLE                        IchnIoTrapHandle[IO_TRAP_HANDLER_NUM];
  LIST_ENTRY                        CallbackDataBase;
  UINT32                            TrapUsedLength[IO_TRAP_HANDLER_NUM];
  BOOLEAN                           MergeDisable[IO_TRAP_HANDLER_NUM];  ///< Determine if IoTrap can be merged with other IoTrap
  PCH_SMM_IO_TRAP_CONTROL_PROTOCOL  PchSmmIoTrapControlProtocol;        ///< Protocol for runtime control the IoTrap state
} IO_TRAP_INSTANCE;

#define IO_TRAP_INSTANCE_FROM_THIS(a) CR (a, IO_TRAP_INSTANCE, EfiSmmIoTrapDispatchProtocol, IO_TRAP_INSTANCE_SIGNATURE)

///
/// "IOTRAP" RECORD
/// Linked list data structures
///
#define IO_TRAP_RECORD_SIGNATURE  SIGNATURE_32 ('I', 'T', 'R', 'C')

typedef struct _IO_TRAP_RECORD {
  UINT32                                    Signature;
  LIST_ENTRY                                Link;
  EFI_SMM_IO_TRAP_DISPATCH_REGISTER_CONTEXT Context;
  EFI_SMM_IO_TRAP_DISPATCH_CALLBACK         Callback;
} IO_TRAP_RECORD;

#define IO_TRAP_RECORD_FROM_LINK(_record) CR (_record, IO_TRAP_RECORD, Link, IO_TRAP_RECORD_SIGNATURE)

//
// Prototypes
//
/**
  IO Trap SMM driver entry point function.

  @param[in] ImageHandle          Image handle for this driver image
  @param[in] SystemTable          Pointer to the EFI System Table

  @retval EFI_SUCCESS             Driver initialization completed successfully
**/
EFI_STATUS
EFIAPI
InstallIoTrap (
  IN EFI_HANDLE                     ImageHandle,
  IN EFI_SYSTEM_TABLE               *SystemTable
  );

/**
  Register a new IO Trap SMI dispatch function with a parent SMM driver.
  The caller will provide information about the IO trap characteristics via
  the context.  This includes base address, length, read vs. r/w, etc.
  This function will autoallocate IO base address from a common pool if the base address is 0,
  and the RegisterContext Address field will be updated.
  The service will not perform GCD allocation if the base address is non-zero.
  In this case, the caller is responsible for the existence and allocation of the
  specific IO range.
  This function looks for the suitable handler and Register a new IchnIoTrap handler
  if the IO Trap handler is not used. It also enable the IO Trap Range to generate
  SMI.

  @param[in] This                 Pointer to the  EFI_SMM_IO_TRAP_DISPATCH_PROTOCOL instance.
  @param[in] DispatchFunction     Pointer to dispatch function to be invoked for
                                  this SMI source.
  @param[in, out] RegisterContext Pointer to the dispatch function's context.
                                  The caller fills this context in before calling
                                  the register function to indicate to the register
                                  function the IO trap SMI source for which the dispatch
                                  function should be invoked.  This may not be NULL.
  @param[out] DispatchHandle      Handle of dispatch function, for when interfacing
                                  with the parent SMM driver, will be the address of linked
                                  list link in the call back record.  This may not be NULL.

  @retval EFI_SUCCESS             The dispatch function has been successfully
                                  registered and the SMI source has been enabled.
  @retval EFI_DEVICE_ERROR        The driver was unable to enable the SMI source.
  @retval EFI_OUT_OF_RESOURCES    Insufficient resources are available
  @retval EFI_INVALID_PARAMETER   Address requested is already in use.
**/
static
EFI_STATUS
EFIAPI
IoTrapRegister (
  IN     EFI_SMM_IO_TRAP_DISPATCH_PROTOCOL            *This,
  IN     EFI_SMM_IO_TRAP_DISPATCH_CALLBACK            DispatchFunction,
  IN OUT EFI_SMM_IO_TRAP_DISPATCH_REGISTER_CONTEXT    *RegisterContext,
  OUT EFI_HANDLE                                      *DispatchHandle
  );

/**
  Unregister a child SMI source dispatch function with a parent SMM driver.

  @param[in] This                 Pointer to the  EFI_SMM_IO_TRAP_DISPATCH_PROTOCOL instance.
  @param[in] DispatchHandle       Handle of dispatch function to deregister.

  @retval EFI_SUCCESS             The dispatch function has been successfully
                                  unregistered and the SMI source has been disabled
                                  if there are no other registered child dispatch
                                  functions for this SMI source.
  @retval EFI_INVALID_PARAMETER   Handle is invalid.
**/
EFI_STATUS
EFIAPI
IoTrapUnRegister (
  IN EFI_SMM_IO_TRAP_DISPATCH_PROTOCOL    *This,
  IN EFI_HANDLE                           *DispatchHandle
  );

/**
  This I/O Trap SMI handler invokes the ACPI reference code to handle the SMI.
  It currently assumes it owns all of the IO trap SMI.

  @param[in] DispatchHandle       Not used
  @param[in] DispatchContext      Not used

  @retval None
**/
VOID
EFIAPI
IoTrapCallback (
  IN  EFI_HANDLE                      DispatchHandle,
  IN  EFI_SMM_ICHN_DISPATCH_CONTEXT   *DispatchContext
  );

/**
  Pause IoTrap callback function.

  This function disables the SMI enable of IoTrap according to the DispatchHandle,
  which is returned by IoTrap callback registration. It only supports the DispatchHandle
  with MergeDisable TRUE and address not zero.

  @param[in] This                 Pointer to the PCH_SMM_IO_TRAP_CONTROL_PROTOCOL instance.
  @param[in] DispatchHandle       Handle of the child service to change state.

  @retval EFI_SUCCESS             This operation is complete.
  @retval EFI_INVALID_PARAMETER   The DispatchHandle is invalid.
  @retval EFI_ACCESS_DENIED       The SMI status is alrady PAUSED.
**/
EFI_STATUS
EFIAPI
IoTrapControlPause (
  IN PCH_SMM_IO_TRAP_CONTROL_PROTOCOL * This,
  IN EFI_HANDLE                       DispatchHandle
  );

/**
  Resume IoTrap callback function.

  This function enables the SMI enable of IoTrap according to the DispatchHandle,
  which is returned by IoTrap callback registration. It only supports the DispatchHandle
  with MergeDisable TRUE and address not zero.

  @param[in] This                 Pointer to the PCH_SMM_IO_TRAP_CONTROL_PROTOCOL instance.
  @param[in] DispatchHandle       Handle of the child service to change state.

  @retval EFI_SUCCESS             This operation is complete.
  @retval EFI_INVALID_PARAMETER   The DispatchHandle is invalid.
  @retval EFI_ACCESS_DENIED       The SMI status is alrady RESUMED.
**/
EFI_STATUS
EFIAPI
IoTrapControlResume (
  IN PCH_SMM_IO_TRAP_CONTROL_PROTOCOL * This,
  IN EFI_HANDLE                       DispatchHandle
  );

#endif
