/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement

@copyright
  Copyright (c) 2005 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  SmmIoTrapDispatch.h

@brief
  PCH SMM IO Trap Dispatch Protocol

**/
#ifndef _EFI_SMM_IO_TRAP_DISPATCH_H_
#define _EFI_SMM_IO_TRAP_DISPATCH_H_

///
/// GUID for the SMM IO Trap Dispatch Protocol
///
/// EDK and EDKII have different GUID formats
///
#if !defined(EDK_RELEASE_VERSION) || (EDK_RELEASE_VERSION < 0x00020000)
#define EFI_SMM_IO_TRAP_DISPATCH_PROTOCOL_GUID \
  { \
    0xdb7f536b, 0xede4, 0x4714, 0xa5, 0xc8, 0xe3, 0x46, 0xeb, 0xaa, 0x20, 0x1d \
  }
#else
#define EFI_SMM_IO_TRAP_DISPATCH_PROTOCOL_GUID \
  { \
    0xdb7f536b, 0xede4, 0x4714, \
    { \
      0xa5, 0xc8, 0xe3, 0x46, 0xeb, 0xaa, 0x20, 0x1d \
    } \
  }
#endif
//
// Extern the GUID for protocol users.
//
extern EFI_GUID                                   gEfiSmmIoTrapDispatchProtocolGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _EFI_SMM_IO_TRAP_DISPATCH_PROTOCOL EFI_SMM_IO_TRAP_DISPATCH_PROTOCOL;

//
// Related Definitions
//
///
/// IO Trap valid types
///
typedef enum {
  WriteTrap,
  ReadTrap,
  ReadWriteTrap,
  IoTrapTypeMaximum
} EFI_SMM_IO_TRAP_DISPATCH_TYPE;

///
/// IO Trap context structure containing information about the IO trap event that should invoke the callback
///
typedef struct {
  UINT16                        Address;      ///< IO Trap range base address (NULL means allocate)
  UINT16                        Length;       ///< IO Trap range length
  EFI_SMM_IO_TRAP_DISPATCH_TYPE Type;         ///< Access types to trap on
  VOID                          *Context;     ///< Callback function context
  BOOLEAN                       MergeDisable; ///< Determine if IoTrap needs to be merged with other registered IoTrap
} EFI_SMM_IO_TRAP_DISPATCH_REGISTER_CONTEXT;

///
/// IO Trap context structure containing information about the IO trap that occurred
///
typedef struct {
  UINT16                        Address;    ///< IO address trapped
  EFI_SMM_IO_TRAP_DISPATCH_TYPE Type;       ///< IO access type
  UINT32                        WriteData;  ///< Data written (contents undefined for read trap)
  VOID                          *Context;   ///< Callback function context
} EFI_SMM_IO_TRAP_DISPATCH_CALLBACK_CONTEXT;

//
// Member functions
//

/**
  Dispatch function for an IO Trap specific SMI handler.

  @param[in] DispatchHandle       Handle of this dispatch function.
  @param[in] CallbackContext      Pointer to the dispatched function's context.
                                  The CallbackContext fields are updated
                                  by the dispatching driver prior to
                                  invoking this callback function.

  @retval None
**/
typedef
VOID
(EFIAPI *EFI_SMM_IO_TRAP_DISPATCH_CALLBACK) (
  IN EFI_HANDLE                                 DispatchHandle,
  IN EFI_SMM_IO_TRAP_DISPATCH_CALLBACK_CONTEXT  * CallbackContext
  );

/**
  Register an IO trap SMI child handler for a specified SMI.
  This service will register a child for a given SMI source.
  The caller will provide information about the IO trap characteristics via the context.
  This includes base address, length, and type (read, write, read/write).
  The service will allocate the IO range if the base address is 0, and the RegisterContext
  Address field will be updated and returned to the caller.
  The service will allocate system resources via GCD services for the requested IO trap range and type.
  An error will be returned if insufficient resources are available to fulfill the request.
  The service will not perform GCD allocation if the base address is non-zero.  In this case,
  the caller is responsible for the existence and allocation of the specific IO range.
  An error may be returned if some or all of the requested resources conflict with an existing IO trap child handler.
  It is not required that implementations will allow multiple children for a single IO trap SMI source.
  Some implementations may support multiple children.

  @param[in] This                 Pointer to the EFI_SMM_IO_TRAP_DISPATCH_PROTOCOL instance.
  @param[in] DispatchFunction     Pointer to the dispatch function to be invoked for this SMI source.
  @param[in, out] RegisterContext Pointer to the dispatch function's context.
                                  The caller fills this context in before calling the register function to indicate to the
                                  register function the IO trap SMI source for which the dispatch function should be invoked.
                                  This may not be NULL.
  @param[out] DispatchHandle      Handle of the dispatch function, for when interfacing with the parent SMM driver.
                                  Type EFI_HANDLE is defined in InstallProtocolInterface() in the EFI 1.10 Specification.
                                  This may not be NULL.

  @retval EFI_SUCCESS             The dispatch function has been successfully registered.
  @retval EFI_DEVICE_ERROR        The driver was unable to complete due to hardware error.
  @retval EFI_OUT_OF_RESOURCES    Insufficient resources are available to fulfill
                                  the IO trap range request.
  @retval EFI_INVALID_PARAMETER   RegisterContext is invalid.  The input value is not within a valid range.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_SMM_IO_TRAP_DISPATCH_REGISTER) (
  IN     EFI_SMM_IO_TRAP_DISPATCH_PROTOCOL            * This,
  IN     EFI_SMM_IO_TRAP_DISPATCH_CALLBACK            DispatchFunction,
  IN OUT EFI_SMM_IO_TRAP_DISPATCH_REGISTER_CONTEXT    * RegisterContext,
  OUT EFI_HANDLE                                      * DispatchHandle
  );

/**
  Unregister a child SMI source dispatch function with a parent SMM driver

  This service removes a previously installed child dispatch handler.
  This does not guarantee that the system resources will be freed from the GCD.

  @param[in] This                 Pointer to the EFI_SMM_IO_TRAP_DISPATCH_PROTOCOL instance.
  @param[in] DispatchHandle       Handle of the child service to remove.
                                  Type EFI_HANDLE is defined in InstallProtocolInterface() in the EFI 1.10 Specification.

  @retval EFI_SUCCESS             The dispatch function has been successfully
                                  unregistered.
  @retval EFI_INVALID_PARAMETER   Handle is invalid.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_SMM_IO_TRAP_DISPATCH_UNREGISTER) (
  IN EFI_SMM_IO_TRAP_DISPATCH_PROTOCOL          * This,
  IN EFI_HANDLE                                 * DispatchHandle
  );

///
/// Interface structure for the SMM IO trap specific SMI Dispatch Protocol
///
/// This protocol provides the ability to install child handlers for IO trap SMI.
/// These handlers will be invoked to respond to specific IO trap SMI. IO trap SMI
/// would typically be generated on reads or writes to specific processor IO space
/// addresses or ranges. This protocol will typically abstract a limited hardware
/// resource, so callers should handle errors gracefully.
///
struct _EFI_SMM_IO_TRAP_DISPATCH_PROTOCOL {
  EFI_SMM_IO_TRAP_DISPATCH_REGISTER   Register;   ///< Installs a child service to be dispatched when the requested IO trap SMI occurs.
  EFI_SMM_IO_TRAP_DISPATCH_UNREGISTER UnRegister; ///< Removes a previously registered child service.
};

#endif
