/** @file
  This is the PEIM that performs the S3 resume tasks.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**

@copyright
  Copyright (c) 2008 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

**/
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <PchS3Peim.h>

PCH_S3_PARAMETER_HEADER  *S3Parameter = NULL;

/**
  PCH S3 PEIM entry point

  @param[in] FfsHeader            Header for FFS
  @param[in] PeiServices          PEI Services table pointer

  @retval EFI_SUCCESS             Successfully completed
**/
EFI_STATUS
EFIAPI
InitializePchS3Peim (
  IN EFI_PEI_FILE_HANDLE                   FileHandle,
  IN CONST EFI_PEI_SERVICES                **PeiServices
  )
{
  EFI_STATUS                    Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI    *PeiVar;
  UINTN                         VarSize;
  EFI_BOOT_MODE                 BootMode;
  UINT32                        RootComplexBar;
  UINT32                        TypeSize;
  UINT32                        ParameterSize;
  UINT32                        Size;
  UINT8                         *CurrentPos;
  EFI_PCH_S3_DISPATCH_ITEM_TYPE ItemType;
  UINT32                        HeaderSize;

  DEBUG ((EFI_D_INFO, "InitializePchS3Peim() Start\n"));

  ///
  /// If not in S3 boot path. do nothing
  ///
  Status = (*PeiServices)->GetBootMode (PeiServices, &BootMode);
  if (EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  if (BootMode != BOOT_ON_S3_RESUME) {
    return EFI_SUCCESS;
  }
  ///
  /// Get the Root Complex Bar
  ///
  RootComplexBar = PCH_RCRB_BASE;

  if (S3Parameter == NULL) {
    ///
    /// Get PCH S3 Parameters
    ///
    Status = (**PeiServices).LocatePpi (
                              PeiServices,
                              &gEfiPeiReadOnlyVariable2PpiGuid,
                              0,
                              NULL,
                              (VOID **) &PeiVar
                              );
    ASSERT_EFI_ERROR (Status);
    VarSize = sizeof (UINT32);
    Status = PeiVar->GetVariable (
                      PeiVar,
                      PCH_INIT_VARIABLE_NAME,
                      &gPchInitVariableGuid,
                      NULL,
                      &VarSize,
                      &S3Parameter
                      );

    if (EFI_ERROR (Status)) {
      return Status;
    }
  }
  ///
  /// Get the current Execute Position
  ///
  CurrentPos  = (UINT8 *) S3Parameter + S3Parameter->ExecutePosition;
  ItemType    = *(EFI_PCH_S3_DISPATCH_ITEM_TYPE *) CurrentPos;

  ///
  /// Round up TypeSize to be 8 byte aligned
  ///
  TypeSize  = sizeof (EFI_PCH_S3_DISPATCH_ITEM_TYPE);
  TypeSize  = (TypeSize + 7) / 8 * 8;
  CurrentPos += TypeSize;

  ///
  /// Calculate the size required;
  /// ** Always round up to be 8 byte aligned
  ///
  switch (ItemType) {
  case PchS3ItemTypeSendCodecCommand:
    ParameterSize = sizeof (EFI_PCH_S3_PARAMETER_SEND_CODEC_COMMAND);
    ParameterSize = (ParameterSize + 7) / 8 * 8;
    Status        = PchS3SendCodecCommand ((EFI_PCH_S3_PARAMETER_SEND_CODEC_COMMAND *) CurrentPos);
    break;

  case PchS3ItemTypeInitPcieRootPortDownstream:
    ParameterSize = sizeof (EFI_PCH_S3_PARAMETER_INIT_PCIE_ROOT_PORT_DOWNSTREAM);
    ParameterSize = (ParameterSize + 7) / 8 * 8;
    Status        = PchS3InitPcieRootPortDownstream ((EFI_PCH_S3_PARAMETER_INIT_PCIE_ROOT_PORT_DOWNSTREAM *) CurrentPos);
    if (Status == EFI_NOT_FOUND) {
      ///
      /// EFI_NOT_FOUND is not an error here
      ///
      Status = EFI_SUCCESS;
    }
    break;

  case PchS3ItemTypePcieSetPm:
    ParameterSize = sizeof (EFI_PCH_S3_PARAMETER_PCIE_SET_PM);
    ParameterSize = (ParameterSize + 7) / 8 * 8;
    Status        = PchS3PcieSetPm ((EFI_PCH_S3_PARAMETER_PCIE_SET_PM *) CurrentPos);
    if (Status == EFI_NOT_FOUND) {
      ///
      /// EFI_NOT_FOUND is not an error here
      ///
      Status = EFI_SUCCESS;
    }
    break;

  case PchS3ItemTypeProgramIobp:
    ParameterSize = sizeof (EFI_PCH_S3_PARAMETER_PROG_IOBP);
    ParameterSize = (ParameterSize + 7) / 8 * 8;
    Status        = PchS3ProgramIobp ((EFI_PCH_S3_PARAMETER_PROG_IOBP *) CurrentPos);
    break;

  default:
    ParameterSize = 0;
    ASSERT (FALSE);
    break;
  }
  ///
  /// Total size is TypeSize + ParameterSize
  ///
  Size = TypeSize + ParameterSize;

  ///
  /// Advance the Execution Position
  ///
  S3Parameter->ExecutePosition += Size;
  if (S3Parameter->ExecutePosition >= S3Parameter->StorePosition) {
    ///
    /// We are beyond end, wrap for the next S3 resume path
    ///
    HeaderSize                    = sizeof (PCH_S3_PARAMETER_HEADER);
    HeaderSize                    = (HeaderSize + 7) / 8 * 8;
    S3Parameter->ExecutePosition  = HeaderSize;
  }

  DEBUG ((EFI_D_INFO, "InitializePchS3Peim() End\n"));

  return Status;
}

#define AZALIA_MAX_LOOP_TIME  10
#define AZALIA_WAIT_PERIOD    100

/**
  Polling the Status bit

  @param[in] StatusReg            The regsiter address to read the status
  @param[in] PollingBitMap        The bit mapping for polling
  @param[in] PollingData          The Data for polling

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_TIMEOUT             Polling the bit map time out
**/
static
EFI_STATUS
CodecStatusPolling (
  IN      UINT32          StatusReg,
  IN      UINT16          PollingBitMap,
  IN      UINT16          PollingData
  )
{
  UINT32  LoopTime;

  for (LoopTime = 0; LoopTime < AZALIA_MAX_LOOP_TIME; LoopTime++) {
    if ((MmioRead16 (StatusReg) & PollingBitMap) == PollingData) {
      break;
    } else {
      PchPmTimerStall (AZALIA_WAIT_PERIOD);
    }
  }

  if (LoopTime >= AZALIA_MAX_LOOP_TIME) {
    return EFI_TIMEOUT;
  }

  return EFI_SUCCESS;
}

/**
  Send Codec command on S3 resume

  @param[in] Parameter            Parameters passed in from DXE

  @retval EFI_DEVICE_ERROR        Device status error, operation failed
  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
PchS3SendCodecCommand (
  EFI_PCH_S3_PARAMETER_SEND_CODEC_COMMAND      *Parameter
  )
{
  UINT32      HdaBar;
  UINT32      *CodecCommandData;
  EFI_STATUS  Status;

  HdaBar            = Parameter->HdaBar;
  CodecCommandData  = &Parameter->CodecCmdData;

  DEBUG ((EFI_D_INFO, "Going to SendCodecCommand: %08x! \n", *CodecCommandData));
  Status = CodecStatusPolling (HdaBar + R_HDA_IRS, (UINT16) B_HDA_IRS_ICB, (UINT16) 0);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "ICB bit is not zero before SendCodecCommand! \n"));
    return EFI_DEVICE_ERROR;
  }

  MmioWrite32 (HdaBar + R_HDA_IC, *CodecCommandData);
  MmioOr16 ((UINTN) (HdaBar + R_HDA_IRS), (UINT16) ((B_HDA_IRS_IRV | B_HDA_IRS_ICB)));

  Status = CodecStatusPolling (HdaBar + R_HDA_IRS, (UINT16) B_HDA_IRS_ICB, (UINT16) 0);
  if (EFI_ERROR (Status)) {
    MmioAnd16 ((UINTN) (HdaBar + R_HDA_IRS), (UINT16)~(B_HDA_IRS_ICB));
    DEBUG ((EFI_D_ERROR, "SendCodecCommand: SendCodecCommand:%08x fail! \n"));
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  Perform Init Root Port Downstream devices on S3 resume

  @param[in] Parameter            Parameters passed in from DXE

  @retval EFI_STATUS
**/
EFI_STATUS
PchS3InitPcieRootPortDownstream (
  EFI_PCH_S3_PARAMETER_INIT_PCIE_ROOT_PORT_DOWNSTREAM     *Parameter
  )
{
  EFI_STATUS  Status;

  Status = PchPcieInitRootPortDownstreamDevices (
            Parameter->RootPortBus,
            Parameter->RootPortDevice,
            Parameter->RootPortFunc,
            Parameter->TempBusNumberMin,
            Parameter->TempBusNumberMax,
            NULL
            );
  ///
  /// Not checking the error status here - downstream device not present does not
  /// mean an error of this root port. Our return status of EFI_SUCCESS means this
  /// port is enabled and outer function depends on this return status to do
  /// subsequent initializations.
  ///
  return Status;
}

/**
  Perform Root Port Downstream devices PCIE ASPM and LTR override on S3 resume

  @param[in] Parameter            Parameters passed in from DXE

  @retval EFI_STATUS
**/
EFI_STATUS
PchS3PcieSetPm (
  EFI_PCH_S3_PARAMETER_PCIE_SET_PM    *Parameter
  )
{
  EFI_STATUS                    Status;
  PCH_PCIE_DEVICE_ASPM_OVERRIDE *DevAspmOverride;
  PCH_PCIE_DEVICE_LTR_OVERRIDE  *DevLtrOverride;
  PCH_PCIE_PWR_OPT              *S3PchPwrOptPcie;
  BOOLEAN                       L1SubstatesSupported;
  BOOLEAN                       LtrSupported;

  DevAspmOverride = (PCH_PCIE_DEVICE_ASPM_OVERRIDE *) (UINTN) Parameter->DevAspmOverrideAddr;
  DevLtrOverride  = (PCH_PCIE_DEVICE_LTR_OVERRIDE *) (UINTN) Parameter->DevLtrOverrideAddr;
  S3PchPwrOptPcie = (PCH_PCIE_PWR_OPT *) (UINTN) Parameter->PchPwrOptPcie;
  Status = PcieSetPm (
            Parameter->RootPortBus,
            Parameter->RootPortDevice,
            Parameter->RootPortFunc,
            Parameter->RootPortAspm,
            Parameter->NumOfDevAspmOverride,
            DevAspmOverride,
            Parameter->TempBusNumberMin,
            Parameter->TempBusNumberMax,
            Parameter->NumOfDevLtrOverride,
            DevLtrOverride,
            S3PchPwrOptPcie,
            &L1SubstatesSupported,
            Parameter->L1SubstatesConfig,
            Parameter->PolicyRevision,
            Parameter->FirstRPToSetPm,
            Parameter->L1SupportedInAllEnabledPorts,
            Parameter->ClkreqSupportedInAllEnabledPorts,
            &LtrSupported
            );
  ///
  /// Not checking the error status here - downstream device not present does not
  /// mean an error of this root port. Our return status of EFI_SUCCESS means this
  /// port is enabled and outer function depends on this return status to do
  /// subsequent initializations.
  ///
  return Status;
}

/**
  Perform PCH IOBP programming on S3 resume

  @param[in] Parameter            Parameters passed in from DXE

  @retval EFI_STATUS
**/
EFI_STATUS
PchS3ProgramIobp (
  EFI_PCH_S3_PARAMETER_PROG_IOBP      *Parameter
  )
{
  EFI_STATUS  Status;

  Status = ProgramIobp (
            Parameter->RootComplexBar,
            Parameter->Address,
            Parameter->AndMask,
            Parameter->OrMask
            );

  return Status;
}
