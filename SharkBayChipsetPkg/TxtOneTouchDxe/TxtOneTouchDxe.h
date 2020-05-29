/** @file



;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
/**
  This file contains a 'Sample Driver' and is licensed as such  
  under the terms of your license agreement with Intel or your  
  vendor.  This file may be modified by the user, subject to    
  the additional terms of the license agreement                 
**/
/**

Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


@file:

  TxtOneTouchDxe.h

@brief:


**/
#ifndef _TXT_ONE_TOUCH_DXE_H_
#define _TXT_ONE_TOUCH_DXE_H_

///===============================================
///  MdePkg/Include/
///===============================================
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>

#include <Protocol/TcgService.h>

///===============================================
///  $(CHIPSET_REF_CODE_PKG)/Chipset/IntelPch/LynxPoint
///===============================================
#include <Protocol/PchReset.h>

///===============================================
///  $(CHIPSET_REF_CODE_PKG)/Chipset/Me/
///===============================================
#include <Guid/TxtOneTouch.h>
///===============================================
///  $(CHIPSET_PKG)/Include/Protocol
///===============================================
#include <Protocol/TxtOneTouchOp.h>

#define H2NL(x)             (H2NS ((x) >> 16) | (H2NS ((x) & 0xffff) << 16))
#define H2NS(x)             ((((x) << 8) | ((x) >> 8)) & 0xffff)
#define TPM_PP_USER_ABORT   ((TPM_RESULT) (-0x10))
#define TPM_PP_BIOS_FAILURE ((TPM_RESULT) (-0x0f))

///
/// TPM PPI Commands
///
#define ENABLE              1
#define ACTIVATE            3
#define ENABLE_ACTIVATE     6
#define DISABLE_DEACTIVATE  7

///
/// Definitions
///
#define TXT_ONE_TOUCH_VAR L"TxtOneTouch"
#pragma pack(push, 1)
typedef struct {
  BOOLEAN NoTxtMaintenance;
  BOOLEAN TpmEnable;
  BOOLEAN TpmActive;
  BOOLEAN VtEnable;
  BOOLEAN VtdEnable;
  BOOLEAN TxtEnable;
  BOOLEAN StmEnable;
  BOOLEAN VmxEnable;
  BOOLEAN VmxInSmxEnable;
  BOOLEAN VmxOutsideSmxEnable;
  BOOLEAN SenterEnable;
} TXT_ONE_TOUCH_SETUP;
#pragma pack(pop)

/**
  Extend PPI operation for TxT. 
  
  @param [in] This                Point of TXT_ONE_TOUCH_OP_PROTOCOL
  @param [in] Command             Operation value for TxT


  @retval EFI_SUCCESS             Execute the Command successful
  @retval EFI_UNSUPPORTED         Command is not supported
**/
EFI_STATUS
EFIAPI
TxtExecOperation (
  IN TXT_ONE_TOUCH_OP_PROTOCOL         *This,
  IN UINT8                             Command
  );

/**
  Confirmation dialog for TxT PPI
  
  @param [in] This                Point of TXT_ONE_TOUCH_OP_PROTOCOL
  @param [in] Command             Operation value for TxT
  @param [in] Confirm             User confirm


  @retval EFI_SUCCESS             Execute the Command successful
  @retval EFI_UNSUPPORTED         Command is not supported
**/
EFI_STATUS
EFIAPI
TxtConfirmationDialog (
  IN     TXT_ONE_TOUCH_OP_PROTOCOL     *This,
  IN     UINT8                         Command,
  IN OUT BOOLEAN                       *Confirm
  );

/**
  Reset system. 
  
  @param [in] This                Point of TXT_ONE_TOUCH_OP_PROTOCOL
  @param [in] Command             Operation value for TxT

  @retval EFI_SUCCESS             Always return EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
TxtResetState (
  IN TXT_ONE_TOUCH_OP_PROTOCOL         *This,
  IN UINT8                             Command
  );

/**
    Enable/Active TPM

  @param [in] Command                  The operation code for TxT One Touch function

  @retval EFI_SUCCESS                  TPM command lunch success
  @retval EFI_UNSUPPORTED              The Command is not supported
  @retval EFI_DEVICE_ERROR             Faile to lunch TPM command

**/
EFI_STATUS
TpmEnableActive (
  IN UINT8                             Command
  );

/**
  Read/Write variable for enable/disable TxT one
  touch functions 

  @param [in] VariableData             Point to Setup variable buffer
  @param [in] WriteData                TRUE, write changes to Setup Variable. FALSE, not to write variable.

  @retval EFI_SUCCESS                  Operation complete successful
  @retval EFI_INVALID_PARAMETER        VariableData is NULL
**/
EFI_STATUS
ReadWriteVariable (
  IN OUT TXT_ONE_TOUCH_SETUP           *VariableData,
  IN     BOOLEAN                       WriteData
  );

/**
  Verify the status of Chipset capaibility and Setup settings

  @param [in] Data                Point to TXT_ONE_TOUCH_SETUP

  @retval    EFI_SUCCESS          The system is able to lunch TxT
  @exception EFI_UNSUPPORTED      The system is not able to lunch TxT

**/
EFI_STATUS
ValidateTxtStates (
  IN  TXT_ONE_TOUCH_SETUP              *Data
  );

#endif
