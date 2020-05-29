/** @file
  PCH SMM IO Trap Control Protocol

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************  
  
@copyright
  Copyright (c) 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
**/
#ifndef _PCH_SMM_IO_TRAP_CONTROL_H_
#define _PCH_SMM_IO_TRAP_CONTROL_H_

///
/// GUID for the SMM IO Trap Control Protocol
///
/// EDK and EDKII have different GUID formats
///
#if !defined(EDK_RELEASE_VERSION) || (EDK_RELEASE_VERSION < 0x00020000)
#define PCH_SMM_IO_TRAP_CONTROL_GUID \
  { \
    0x514D2AFD, 0x2096, 0x4283, 0x9D, 0xA6, 0x70, 0x0C, 0xD2, 0x7D, 0xC7, 0xA5 \
  }
#else
#define PCH_SMM_IO_TRAP_CONTROL_GUID \
  { \
    0x514D2AFD, 0x2096, 0x4283, \
    { \
      0x9D, 0xA6, 0x70, 0x0C, 0xD2, 0x7D, 0xC7, 0xA5 \
    } \
  }
#endif

//
// Extern the GUID for protocol users.
//
extern EFI_GUID                                   gPchSmmIoTrapControlGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _PCH_SMM_IO_TRAP_CONTROL_PROTOCOL  PCH_SMM_IO_TRAP_CONTROL_PROTOCOL;

//
// Related Definitions
//

//
// Member functions
//

/**
  The Prototype of Pause and Resume IoTrap callback function.

  @param[in] This                 Pointer to the PCH_SMM_IO_TRAP_CONTROL_PROTOCOL instance.
  @param[in] DispatchHandle       Handle of the child service to change state.

  @retval EFI_SUCCESS             This operation is complete.
  @retval EFI_INVALID_PARAMETER   The DispatchHandle is invalid.
  @retval EFI_ACCESS_DENIED       The SMI status is alrady PAUSED/RESUMED.
**/
typedef
EFI_STATUS
(EFIAPI *PCH_SMM_IO_TRAP_CONTROL_FUNCTION) (
  IN PCH_SMM_IO_TRAP_CONTROL_PROTOCOL           * This,
  IN EFI_HANDLE                                 DispatchHandle
  );

/**
  Interface structure for the SMM IO trap pause and resume protocol
  This protocol provides the functions to runtime control the IoTrap SMI enabled/disable.
  This applys the capability to the DispatchHandle which returned by IoTrap callback
  registration, and the DispatchHandle which must be MergeDisable = TRUE and Address != 0.
  Besides, when S3 resuem, it only restores the state of IoTrap callback registration.
  The Paused/Resume state won't be restored after S3 resume.
**/
struct _PCH_SMM_IO_TRAP_CONTROL_PROTOCOL {
  PCH_SMM_IO_TRAP_CONTROL_FUNCTION      Pause;
  PCH_SMM_IO_TRAP_CONTROL_FUNCTION      Resume;
};

#endif
