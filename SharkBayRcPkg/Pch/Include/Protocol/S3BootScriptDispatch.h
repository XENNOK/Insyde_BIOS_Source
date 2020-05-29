/** @file
  This protocol is used to store or record various boot scripts into boot 
  script tables.

Copyright (c) 2007 - 2010, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under 
the terms and conditions of the BSD License that accompanies this distribution.  
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.                                          
    
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

  @par Revision Reference:
  This protocol defined in the Boot Script Specification, Version 0.91.

**/

#ifndef _S3_BOOT_SCRIPT_DISPATCH_PROTOCOL_H_
#define _S3_BOOT_SCRIPT_DISPATCH_PROTOCOL_H_

///
/// S3 Boot Script Dispatch Protocol GUID.
///
#define EFI_S3_BOOT_SCRIPT_DISPATCH_PROTOCOL_GUID \
  { \
    0x53a30577, 0x5a14, 0x4cfa, {0xa0, 0xa1, 0x47, 0x91, 0x1f, 0x08, 0x14, 0xd8}\
  }

typedef struct _EFI_S3_BOOT_SCRIPT_DISPATCH_PROTOCOL EFI_S3_BOOT_SCRIPT_DISPATCH_PROTOCOL;

/**
  Adds a Dispatch record into a specified Framework boot script table.

  @param  This                  A pointer to the EFI_BOOT_SCRIPT_SAVE_PROTOCOL instance.
  @param  EntryPoint         
 
  @retval EFI_SUCCESS           The operation succeeded. A record was added into the specified script table.
  @retval EFI_INVALID_PARAMETER The parameter is illegal, or the given boot script is not supported.
  @retval EFI_OUT_OF_RESOURCES  There is insufficient memory to store the boot script.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_BOOT_SCRIPT_DISPATCH)(
  IN EFI_S3_BOOT_SCRIPT_DISPATCH_PROTOCOL   *This,
  IN VOID                                   *EntryPoint
  );

/**
  Adds a Dispatch record into a specified Framework boot script table.

  @param  This                  A pointer to the EFI_BOOT_SCRIPT_SAVE_PROTOCOL instance.
  @param  EntryPoint         
 
  @retval EFI_SUCCESS           The operation succeeded. A record was added into the specified script table.
  @retval EFI_INVALID_PARAMETER The parameter is illegal, or the given boot script is not supported.
  @retval EFI_OUT_OF_RESOURCES  There is insufficient memory to store the boot script.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_BOOT_SCRIPT_DISPATCH2)(
  IN EFI_S3_BOOT_SCRIPT_DISPATCH_PROTOCOL   *This,
  IN VOID                                   *EntryPoint,
  IN VOID                                   *Context
  );  
  
///
/// The EFI_S3_BOOT_SCRIPT_DISPATCH_PROTOCOL publishes the Framework boot script abstractions
/// to store or record various boot scripts into boot script tables.
///
struct _EFI_S3_BOOT_SCRIPT_DISPATCH_PROTOCOL {
  EFI_BOOT_SCRIPT_DISPATCH        Dispatch;      ///< Dispatch various boot scripts to a boot script table.
  EFI_BOOT_SCRIPT_DISPATCH2       Dispatch2;
};

extern EFI_GUID gEfiS3BootScriptDispatchProtocolGuid;

#endif
