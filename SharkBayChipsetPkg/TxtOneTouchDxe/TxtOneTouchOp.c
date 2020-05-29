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

  TxtOneTouchOp.c

@brief:


**/
#include "TxtOneTouchDxe.h"
///===============================================
///  MdePkg/Include/
///===============================================
#include <IndustryStandard/UefiTcgPlatform.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
///===============================================
///  $(CHIPSET_REF_CODE_PKG)/Include/
///===============================================
#include <CpuDataStruct.h>

extern EFI_TCG_PROTOCOL *mTcgProtocol;

/**
  Execute TPM command

  @param [in] TcgProtocol              Point to EFI_TCG_PROTOCOL
  @param [in] Ordinal                  TPM Command code
  @param [in] AdditionalParameterSize  Size of additional parameters
  @param [in] AdditionalParameters     Point to the buffer saves additional parameters

  @retval EFI_SUCCESS                  TPM command lunch success
  @retval TPM_PP_BIOS_FAILURE          BIOS fail to execute TPM command

**/
TPM_RESULT
TpmCommandNoReturnData (
  IN EFI_TCG_PROTOCOL                  *TcgProtocol,
  IN TPM_COMMAND_CODE                  Ordinal,
  IN UINTN                             AdditionalParameterSize,
  IN VOID                             *AdditionalParameters
  )
{
  EFI_STATUS          Status;
  TPM_RQU_COMMAND_HDR *TpmRqu;
  TPM_RSP_COMMAND_HDR TpmRsp;
  UINT32              Size;

  TpmRqu = (TPM_RQU_COMMAND_HDR *) AllocatePool (sizeof (*TpmRqu) + AdditionalParameterSize);
  if (TpmRqu == NULL) {
    return TPM_PP_BIOS_FAILURE;
  }

  TpmRqu->tag       = H2NS (TPM_TAG_RQU_COMMAND);
  Size              = (UINT32) (sizeof (*TpmRqu) + AdditionalParameterSize);
  TpmRqu->paramSize = H2NL (Size);
  TpmRqu->ordinal   = H2NL (Ordinal);
  CopyMem (TpmRqu + 1, AdditionalParameters, AdditionalParameterSize);

  Status = TcgProtocol->PassThroughToTpm (
                          TcgProtocol,
                          Size,
                          (UINT8 *) TpmRqu,
                          (UINT32) sizeof (TpmRsp),
                          (UINT8 *) &TpmRsp
                          );
  FreePool (TpmRqu);
  if (EFI_ERROR (Status) || (TpmRsp.tag != H2NS (TPM_TAG_RSP_COMMAND))) {
    return TPM_PP_BIOS_FAILURE;
  }

  return H2NL (TpmRsp.returnCode);
}

/**
  Enable/Active TPM

  @param [in] Command            The operation code for TxT One Touch function

  @retval EFI_SUCCESS            TPM command lunch success
  @retval EFI_UNSUPPORTED        The Command is not supported
  @retval EFI_DEVICE_ERROR       Faile to lunch TPM command

**/
EFI_STATUS
TpmEnableActive (
  IN UINT8                             Command
  )
{
  TPM_RESULT  TpmResponse;
  EFI_STATUS  Status;
  BOOLEAN     BoolVal;

  BoolVal     = FALSE;
  TpmResponse = 0;
  Status      = EFI_SUCCESS;

  switch (Command) {
  case ENABLE:
    TpmResponse = TpmCommandNoReturnData (
                    mTcgProtocol,
                    TPM_ORD_PhysicalEnable,
                    0,
                    NULL
                    );
    break;

  case ACTIVATE:
    BoolVal = FALSE;
    TpmResponse = TpmCommandNoReturnData (
                    mTcgProtocol,
                    TPM_ORD_PhysicalSetDeactivated,
                    sizeof (BoolVal),
                    &BoolVal
                    );
    break;

  case ENABLE_ACTIVATE:
    Status = TpmEnableActive (ENABLE);
    if (Status == EFI_SUCCESS) {
      Status = TpmEnableActive (ACTIVATE);
    }

    return Status;

  default:
    Status = EFI_UNSUPPORTED;
    break;
  }

  if (TpmResponse != 0) {
    Status = EFI_DEVICE_ERROR;
  }

  return Status;
}

/**
  Verify the status of Chipset capaibility and Setup settings

  @param [in] Data                     Point to TXT_ONE_TOUCH_SETUP

  @retval EFI_SUCCESS                  The system is able to lunch TxT
  @exception EFI_UNSUPPORTED           The system is not able to lunch TxT

**/
EFI_STATUS
ValidateTxtStates (
  IN TXT_ONE_TOUCH_SETUP               *Data
  )
{
  EFI_CPUID_REGISTER  CpuidRegs;

  AsmCpuid (
    1,
    &CpuidRegs.RegEax,
    &CpuidRegs.RegEbx,
    &CpuidRegs.RegEcx,
    &CpuidRegs.RegEdx
    );

  if (Data->VtEnable) {
    ///
    /// Check if VMX supported
    ///
    if ((CpuidRegs.RegEcx & 0x020) == 0) {
      return EFI_UNSUPPORTED;
    }
  }

  if (Data->TxtEnable) {
    ///
    /// Check if TxT & VMX supported
    ///
    if ((CpuidRegs.RegEcx & 0x060) == 0) {
      return EFI_UNSUPPORTED;
    }
  }

  return EFI_SUCCESS;
}
