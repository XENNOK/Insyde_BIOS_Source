/** @file
  PCH Pcie SMM Driver Entry

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

@copyright
  Copyright (c) 2010 - 2013 Intel Corporation. All rights reserved
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
#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/S3BootScriptLib.h>
#include <Protocol/SmmIchnDispatch.h>
#include <Protocol/SmmBase.h>
#include <Protocol/SmmRuntime.h>
#include "PchPcieSmm.h"

//
// Global variables
//
EFI_SMM_SYSTEM_TABLE              *mSmst;
EFI_SMM_BASE_PROTOCOL             *mSmmBase;
PCH_PCI_EXPRESS_CONFIG            *mPciExpressConfig;
PCH_PWR_OPT_CONFIG                *mPchPwrOptConfig;
UINT8                             *mRevision;
UINT8                             mBusNumber;

/**
  Program Common Clock and ASPM of Downstream Devices

  @param[in] Function             Pcie Root Port Function Number

  @retval EFI_SUCCESS             Function complete successfully
**/
EFI_STATUS
PchPcieSmi (
  IN  UINT8         Function
  )
{
  UINT16                               Data16;
  UINT8                                SecBus;
  UINT8                                SubBus;
  BOOLEAN                              L1SubstatesSupported;
  EFI_HANDLE                           Handle;
  PCH_PCIE_EXPRESS_L1SUBSTATES_CONTROL L1SubVal;
  UINT8                                RootPortNumber;
  UINT32                               RootComplexBar;
  UINTN                                PciD31F0RegBase;
  BOOLEAN                              LtrSupported;
  PCH_SERIES                           PchSeries;

  Handle = NULL;
  PchSeries         = GetPchSeries();
  LtrSupported      = TRUE;
  PciD31F0RegBase = MmPciAddress (
                      0,
                      mBusNumber,
                      PCI_DEVICE_NUMBER_PCH_LPC,
                      PCI_FUNCTION_NUMBER_PCH_LPC,
                      0
                      );
  RootComplexBar  = MmioRead32 (PciD31F0RegBase + R_PCH_LPC_RCBA) & B_PCH_LPC_RCBA_BAR;
  RootPortNumber = GetPchPcieRpNumber(RootComplexBar, Function);

  if (RootPortNumber == 0xFF) {
    return EFI_DEVICE_ERROR;
  }
  ///
  /// Check for presense detect state
  ///
  Data16 = MmioRead16 (MmPciAddress (0, 0, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, Function, R_PCH_PCIE_SLSTS));
  Data16 &= B_PCH_PCIE_SLSTS_PDS;
  if (Data16) {
    SecBus  = MmioRead8 (MmPciAddress (0, 0, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, Function, 0x19));
    SubBus  = MmioRead8 (MmPciAddress (0, 0, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, Function, 0x1A));
    PchPcieInitRootPortDownstreamDevices (0, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, Function, SecBus, SubBus, NULL);
    MmioWrite8 (MmPciAddress (0, 0, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, Function, 0x19), SecBus);
    MmioWrite8 (MmPciAddress (0, 0, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, Function, 0x1A), SubBus);
    L1SubVal = PchPcieL1SubstatesL1_1_2;
    if ((*mRevision) >= DXE_PCH_PLATFORM_POLICY_PROTOCOL_REVISION_2) {
      L1SubVal = mPciExpressConfig->RootPort[RootPortNumber].L1Substates;
    }
    PcieSetPm (
      0,
      PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS,
      Function,
      mPciExpressConfig->RootPort[RootPortNumber].Aspm,
      mPciExpressConfig->NumOfDevAspmOverride,
      mPciExpressConfig->DevAspmOverride,
      mPciExpressConfig->TempRootPortBusNumMin,
      mPciExpressConfig->TempRootPortBusNumMax,
      mPchPwrOptConfig->NumOfDevLtrOverride,
      mPchPwrOptConfig->DevLtrOverride,
      &(mPchPwrOptConfig->PchPwrOptPcie[RootPortNumber]),
      &L1SubstatesSupported,
      L1SubVal,
      *mRevision,
      FALSE,
      FALSE,
      FALSE,
      &LtrSupported
      );
    
    if(!LtrSupported && (PchSeries == PchLp)) { 
      MmioAndThenOr32 ( (RootComplexBar + 0x3320), 0, 0x00010003);
    }
  } else {
    ///
    /// Clear CCC and LTSP bits when PCIe Card hot unplugged
    ///
    MmioAnd32 (MmPciAddress (0, 0, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, Function, R_PCH_PCIE_MPC2), (UINT32) ~BIT6);
    MmioAnd16 (MmPciAddress (0, 0, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, Function, R_PCH_PCIE_LCTL), (UINT16) ~B_PCH_PCIE_LCTL_CCC);
  }

  return EFI_SUCCESS;
}

/**
  PCIE Hotplug SMI call back function for each Root port

  @param[in] DispatchHandle       Handle of this dispatch function
  @param[in] DispatchContext      Pointer to the dispatch function's context.
                                  The DispatchContext fields are filled in by the dispatching driver
                                  prior to invoke this dispatch function
**/
VOID
EFIAPI
PchPcieSmiHandlerFunction (
  IN  EFI_HANDLE                              DispatchHandle,
  IN  EFI_SMM_ICHN_DISPATCH_EX_CONTEXT        *DispatchContext
  )
{
  PchPcieSmi ((UINT8) (DispatchContext->Type - IchnExPcie0Hotplug));
  return;
}

/**
  PCIE Link Active State Change Hotplug SMI call back function for all Root ports

  @param[in] DispatchHandle       Handle of this dispatch function
  @param[in] DispatchContext      Pointer to the dispatch function's context.
                                  The DispatchContext fields are filled in by the dispatching driver
                                  prior to invoke this dispatch function
**/
VOID
EFIAPI
PchPcieLinkActiveStateChange (
  IN  EFI_HANDLE                              DispatchHandle,
  IN  EFI_SMM_ICHN_DISPATCH_EX_CONTEXT        *DispatchContext
  )
{
  return;
}

/**
  Register PCIE Hotplug SMI dispatch function to handle Hotplug enabling

  @param[in] ImageHandle          The image handle of this module
  @param[in] SystemTable          The EFI System Table

  @retval EFI_SUCCESS             The function completes successfully
**/
EFI_STATUS
EFIAPI
InitializePchPcieSmm (
  IN      EFI_HANDLE            ImageHandle,
  IN      EFI_SYSTEM_TABLE      *SystemTable
  )
{
  EFI_STATUS                            Status;
  UINT8                                 Index;
  UINT8                                 Data8;
  UINT32                                Data32Or;
  UINT32                                Data32And;
  UINTN                                 RPBase;
  EFI_HANDLE                            PcieHandle;
  static CONST EFI_SMM_ICHN_EX_SMI_TYPE PchHPcieHandlerList[LPTH_PCIE_MAX_ROOT_PORTS * 2] = {
    IchnExPcie0Hotplug,
    IchnExPcie1Hotplug,
    IchnExPcie2Hotplug,
    IchnExPcie3Hotplug,
    IchnExPcie4Hotplug,
    IchnExPcie5Hotplug,
    IchnExPcie6Hotplug,
    IchnExPcie7Hotplug,
    IchnExPcie0LinkActive,
    IchnExPcie1LinkActive,
    IchnExPcie2LinkActive,
    IchnExPcie3LinkActive,
    IchnExPcie4LinkActive,
    IchnExPcie5LinkActive,
    IchnExPcie6LinkActive,
    IchnExPcie7LinkActive
  };
  static CONST EFI_SMM_ICHN_EX_SMI_TYPE PchLpPcieHandlerList[LPTLP_PCIE_MAX_ROOT_PORTS * 2] = {
    IchnExPcie0Hotplug,
    IchnExPcie1Hotplug,
    IchnExPcie2Hotplug,
    IchnExPcie3Hotplug,
    IchnExPcie4Hotplug,
    IchnExPcie5Hotplug,
    IchnExPcie0LinkActive,
    IchnExPcie1LinkActive,
    IchnExPcie2LinkActive,
    IchnExPcie3LinkActive,
    IchnExPcie4LinkActive,
    IchnExPcie5LinkActive
  };
  EFI_SMM_ICHN_DISPATCH_EX_PROTOCOL     *mIchnDispatch;
  EFI_SMM_ICHN_DISPATCH_EX_CONTEXT      PchPcieContext;
  UINTN                                 PortIndex;
  DXE_PCH_PLATFORM_POLICY_PROTOCOL      *PchPlatformPolicy;
  PCH_PCIE_DEVICE_LTR_OVERRIDE          *DevLtrOverrideTbl;
  UINT32                                TableSize;
  PCH_SERIES                            PchSeries;
  BOOLEAN                               InSmm;

  ///
  /// Locate SmmBase protocol
  ///
  Status = gBS->LocateProtocol (&gEfiSmmBaseProtocolGuid, NULL, (VOID **) &mSmmBase);
  ASSERT_EFI_ERROR (Status);

  InSmm = FALSE;
  if (!EFI_ERROR (Status)) {
    mSmmBase->InSmm (mSmmBase, &InSmm);
  }
  if (!InSmm) {
    return EFI_SUCCESS;
  }

  DEBUG ((EFI_D_INFO, "InitializePchPcieSmm() Start\n"));

  PchSeries         = GetPchSeries();
  DevLtrOverrideTbl = NULL;

  ///
  /// Initialize our module variables
  ///
  Status = mSmmBase->GetSmstLocation (mSmmBase, &mSmst);
  ASSERT_EFI_ERROR (Status);
  ///
  /// Locate the ICHnEx Dispatch protocol
  ///
  Status = gBS->LocateProtocol (&gEfiSmmIchnDispatchExProtocolGuid, NULL, (VOID **) &mIchnDispatch);
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol (&gDxePchPlatformPolicyProtocolGuid, NULL, (VOID **) &PchPlatformPolicy);
  ASSERT_EFI_ERROR (Status);

  Status = mSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (PCH_PCI_EXPRESS_CONFIG),
                    (VOID **) &mPciExpressConfig
                    );
  ASSERT_EFI_ERROR (Status);
  Status = mSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (PCH_PWR_OPT_CONFIG),
                    (VOID **) &mPchPwrOptConfig
                    );
  ASSERT_EFI_ERROR (Status);
  mPciExpressConfig->NumOfDevAspmOverride = PchPlatformPolicy->PciExpressConfig->NumOfDevAspmOverride;
  TableSize = PchPlatformPolicy->PciExpressConfig->NumOfDevAspmOverride * sizeof (PCH_PCIE_DEVICE_ASPM_OVERRIDE);

  ///
  /// Allocate and copy ASPM override table to SMM memory
  ///
  Status = mSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    TableSize,
                    (VOID **) &mPciExpressConfig->DevAspmOverride
                    );
  ASSERT_EFI_ERROR (Status);
  CopyMem (mPciExpressConfig->DevAspmOverride, PchPlatformPolicy->PciExpressConfig->DevAspmOverride, TableSize);

  Status = mSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (PchPlatformPolicy->Revision),
                    (VOID **)&mRevision
                    );
  ASSERT_EFI_ERROR (Status);
  *mRevision = PchPlatformPolicy->Revision;
  mBusNumber = PchPlatformPolicy->BusNumber;
  ///
  /// Allocate and copy LTR override table to SMM memory
  ///
  mPchPwrOptConfig->NumOfDevLtrOverride = PchPlatformPolicy->PwrOptConfig->NumOfDevLtrOverride;
  DevLtrOverrideTbl = PchPlatformPolicy->PwrOptConfig->DevLtrOverride;
  if ((DevLtrOverrideTbl != NULL) && (PchPlatformPolicy->PwrOptConfig->NumOfDevLtrOverride != 0)) {
    TableSize = PchPlatformPolicy->PwrOptConfig->NumOfDevLtrOverride * sizeof (PCH_PCIE_DEVICE_LTR_OVERRIDE);
    Status = mSmst->SmmAllocatePool (
                      EfiRuntimeServicesData,
                      TableSize,
                      (VOID **) &mPchPwrOptConfig->DevLtrOverride
                      );
    ASSERT_EFI_ERROR (Status);
    CopyMem (mPchPwrOptConfig->DevLtrOverride, DevLtrOverrideTbl, TableSize);
   }

  for (PortIndex = 0; PortIndex < GetPchMaxPciePortNum (); PortIndex++) {
    mPciExpressConfig->RootPort[PortIndex].Aspm                     = PchPlatformPolicy->PciExpressConfig->RootPort[PortIndex].Aspm;
    if (PchPlatformPolicy->Revision >= DXE_PCH_PLATFORM_POLICY_PROTOCOL_REVISION_2) {
      mPciExpressConfig->RootPort[PortIndex].L1Substates              = PchPlatformPolicy->PciExpressConfig->RootPort[PortIndex].L1Substates;
    }
    mPchPwrOptConfig->PchPwrOptPcie[PortIndex].LtrEnable            = PchPlatformPolicy->PwrOptConfig->PchPwrOptPcie[PortIndex].LtrEnable;
    mPchPwrOptConfig->PchPwrOptPcie[PortIndex].ObffEnable           = PchPlatformPolicy->PwrOptConfig->PchPwrOptPcie[PortIndex].ObffEnable;
    if (PchPlatformPolicy->Revision >= DXE_PCH_PLATFORM_POLICY_PROTOCOL_REVISION_7) {
      mPchPwrOptConfig->PchPwrOptPcie[PortIndex].LtrMaxSnoopLatency   = PchPlatformPolicy->PwrOptConfig->PchPwrOptPcie[PortIndex].LtrMaxSnoopLatency;
      mPchPwrOptConfig->PchPwrOptPcie[PortIndex].LtrMaxNoSnoopLatency = PchPlatformPolicy->PwrOptConfig->PchPwrOptPcie[PortIndex].LtrMaxNoSnoopLatency;
    }
  }

  //
  // Throught all PCIE root port function and register the SMI Handler for enabled ports.
  //
  for (Index = 0; Index < GetPchMaxPciePortNum (); Index++) {
    RPBase = MmPciAddress (
              0,
              0,
              PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS,
              Index,
              0
              );
    //
    // Skip the root port function which is not enabled
    //
    if (MmioRead32 (RPBase) == 0xFFFFFFFF) {
      continue;
    }

    Data8 = MmioRead8 (RPBase + R_PCH_PCIE_SLCAP);
    if (Data8 & B_PCH_PCIE_SLCAP_HPC) {
      switch (PchSeries) {
        case PchLp:
          PchPcieContext.Type = PchLpPcieHandlerList[Index];
          break;

        case PchH:
          PchPcieContext.Type = PchHPcieHandlerList[Index];
          break;
        default:
          break;
      }
      PcieHandle          = NULL;
      Status = mIchnDispatch->Register (
                                mIchnDispatch,
                                PchPcieSmiHandlerFunction,
                                &PchPcieContext,
                                &PcieHandle
                                );
      ASSERT_EFI_ERROR (Status);

      switch (PchSeries) {
        case PchLp:
          PchPcieContext.Type = PchLpPcieHandlerList[Index + LPTLP_PCIE_MAX_ROOT_PORTS];
          break;

        case PchH:
          PchPcieContext.Type = PchHPcieHandlerList[Index + LPTH_PCIE_MAX_ROOT_PORTS];
          break;
        default:
          break;
      }
      Status = mIchnDispatch->Register (
                                mIchnDispatch,
                                PchPcieLinkActiveStateChange,
                                &PchPcieContext,
                                &PcieHandle
                                );
      ASSERT_EFI_ERROR (Status);

      Data32Or  = B_PCH_PCIE_MPC_HPME;
      Data32And = (UINT32)~B_PCH_PCIE_MPC_HPME;
      SCRIPT_MEM_READ_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (RPBase + R_PCH_PCIE_MPC),
        &Data32Or,  // Data to be ORed
        &Data32And  // Data to be ANDed
        );
    }
  }

  DEBUG ((EFI_D_INFO, "InitializePchPcieSmm() End\n"));

  return EFI_SUCCESS;
}
