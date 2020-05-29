/** @file
  Framework PEIM to PchMeUma

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 2010 - 2013 Intel Corporation. All rights
  reserved This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted
  by such license, no part of this software or documentation may
  be reproduced, stored in a retrieval system, or transmitted in
  any form or by any means without the express written consent
  of Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

**/

//
// Statements that include other files
//

#include "PchMeUma.h"

#ifdef PTT_FLAG
#include <Library/HobLib.h>
#include <Library/Ptt/Include/PttHciRegs.h>
#include <Library/Ptt/Include/PttHciDeviceLib.h>
#endif

//[-start-130416-IB10920021-add]//
#include <Library/PcdLib.h>
//[-end-130416-IB10920021-add]//

///
/// Function Declarations
///
static PCH_ME_UMA_PPI         mPchMeUmaPpi = {
  MeSendUmaSize,
  CpuReplacementCheck,
  MeConfigDidReg,
  HandleMeBiosAction
};

static EFI_PEI_PPI_DESCRIPTOR mPchMeUmaPpiList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gPchMeUmaPpiGuid,
    &mPchMeUmaPpi
  }
};

/**
  This procedure will read and return the amount of ME UMA requested
  by ME ROM from the HECI device.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] FfsHeader            Pointer to the FFS file header

  @retval UINT32                  Return ME UMA Size
  @retval EFI_SUCCESS             Do not check for ME UMA

**/
UINT32
MeSendUmaSize (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_FFS_FILE_HEADER         *FfsHeader
  )
{
  UINT32                  Timeout;
  HECI_MISC_SHDW_REGISTER MeMiscShdw;
  HECI_FWS_REGISTER       MeHfs;
  EFI_STATUS              Status;
  EFI_PEI_STALL_PPI       *StallPpi;

  MeMiscShdw.ul = HeciPciRead32 (R_ME_MISC_SHDW);
  MeHfs.ul      = HeciPciRead32 (R_ME_HFS);
  Timeout       = 0x0;

  if (MeHfs.r.MeOperationMode == ME_OPERATION_MODE_DEBUG) {
    DEBUG ((EFI_D_INFO, "ME debug mode, do not check for ME UMA. \n"));
    return EFI_SUCCESS;
  }

  if (MeHfs.r.ErrorCode != 0) {
    DEBUG ((EFI_D_INFO, "ME error, do not check for ME UMA. \n"));
    return EFI_SUCCESS;
  }
  ///
  /// Poll on MUSZV until it indicates a valid size is present or 5s timeout expires.
  ///
  Status = (*PeiServices)->LocatePpi (PeiServices, &gEfiPeiStallPpiGuid, 0, NULL, (VOID **) &StallPpi);
  ASSERT_EFI_ERROR (Status);

  PERF_START_EX (FfsHeader, "MUSZV", NULL, AsmReadTsc (), 0x4000);
  while ((MeMiscShdw.r.MUSZV == 0) && (Timeout < MUSZV_TIMEOUT_MULTIPLIER)) {
    StallPpi->Stall (PeiServices, StallPpi, STALL_1_MILLISECOND);
    MeMiscShdw.ul = HeciPciRead32 (R_ME_MISC_SHDW);
    Timeout++;
  }

  if (Timeout >= MUSZV_TIMEOUT_MULTIPLIER) {
    DEBUG ((EFI_D_INFO, "Timeout occurred waiting for MUSZV. \n"));
    return EFI_SUCCESS;
  }
  PERF_END_EX (FfsHeader, "MUSZV", NULL, AsmReadTsc (), 0x4001);

  ///
  /// Return MeUmaSize value
  ///
  DEBUG ((EFI_D_INFO, "ME UMA Size Requested: %x\n", MeMiscShdw.r.MUSZ));

  return MeMiscShdw.r.MUSZ;
}

/**
  Init and Install ME Hob

  @param[in] PeiServices          General purpose services available to every PEIM.

  @retval EFI_SUCCESS
**/
EFI_STATUS
InstallMeHob (
  IN CONST EFI_PEI_SERVICES            **PeiServices
  )
{
  EFI_STATUS                      Status;
  ME_DATA_HOB                     *MeDataHob;

  ///
  /// Create HOB for ME Data
  ///
  Status = (**PeiServices).CreateHob (
                             PeiServices,
                             EFI_HOB_TYPE_GUID_EXTENSION,
                             sizeof (ME_DATA_HOB),
                             (VOID **)&MeDataHob
                             );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Initialize default HOB data
  ///
  MeDataHob->EfiHobGuidType.Name = gMeDataHobGuid;
  ZeroMem (&(MeDataHob->FtpmBufferAddress), sizeof (UINT64));

  DEBUG ((EFI_D_INFO, "ME Data HOB installed\n"));

  return EFI_SUCCESS;
}

#ifdef PTT_FLAG
/**
  Internal function performing Heci platform ME Debug Dump in PEI phase

  @param[in] PeiMePolicyPpi       Policy Ppi

  @retval None
**/
VOID
DumpMePlatformPolicyPei (
  IN PEI_ME_PLATFORM_POLICY_PPI   *PeiMePolicyPpi
  )
{
  DEBUG ((EFI_D_INFO, "\n------------------------ MePlatformPolicyPpi Dump Begin -----------------\n"));
  DEBUG ((EFI_D_INFO, " Revision : 0x%x\n", PeiMePolicyPpi->Revision));
  DEBUG ((EFI_D_INFO, " FTpmSwitch : 0x%x\n", PeiMePolicyPpi->FTpmSwitch));
  DEBUG ((EFI_D_INFO, "\n------------------------ MePlatformPolicyPpi Dump End -------------------\n"));
}
#endif

/**
  This procedure will determine whether or not the CPU was replaced
  during system power loss or via dynamic fusing.
  Calling this procedure could result in a warm reset (if ME FW is requesting one).

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] FfsHeader            Not used.
  @param[out] ForceFullTraining   When set = 0x1, MRC will be forced to perform a full
                                  memory training cycle.

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
CpuReplacementCheck (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_FFS_FILE_HEADER         *FfsHeader,
  OUT      UINT8                       *ForceFullTraining
  )
{
  HECI_GS_SHDW_REGISTER           MeFwsts2;
  EFI_STATUS                      Status;
  UINT32                          Timeout;
  EFI_PEI_STALL_PPI               *StallPpi;
  Status  = EFI_SUCCESS;
  Timeout = 0x0;

  ///
  /// Read ME FWS2
  ///
  MeFwsts2.ul = HeciPciRead32 (R_ME_GS_SHDW);
  DEBUG ((EFI_D_INFO, "MeFwsts2 = %x.\n", MeFwsts2.r));

  ///
  /// Locate Stall Ppi
  ///
  Status = (*PeiServices)->LocatePpi (PeiServices, &gEfiPeiStallPpiGuid, 0, NULL, (VOID **) &StallPpi);
  ASSERT_EFI_ERROR (Status);

  PERF_START_EX (FfsHeader, "CPURLOP", NULL, AsmReadTsc (), 0x4010);
  ///
  /// Poll 50 ms on CPU Replaced Valid bit
  ///
  while ((((MeFwsts2.r.CpuReplacedValid == 0) && Timeout < CPURV_TIMEOUT_MULTIPLIER))
  ) {
    StallPpi->Stall (PeiServices, StallPpi, STALL_1_MILLISECOND);
    MeFwsts2.ul = HeciPciRead32 (R_ME_GS_SHDW);
    Timeout++;
  }
  PERF_END_EX (FfsHeader, "CPURLOP", NULL, AsmReadTsc (), 0x4011);

  DEBUG ((EFI_D_INFO, "CpuReplacedValid = %x, ", MeFwsts2.r.CpuReplacedValid));
  DEBUG ((EFI_D_INFO, "CpuReplacedStatus = %x, ", MeFwsts2.r.CpuReplacedSts));
  DEBUG ((EFI_D_INFO, "WarmRstReqForDF = %x.\n", MeFwsts2.r.WarmRstReqForDF));

  if (Timeout >= CPURV_TIMEOUT_MULTIPLIER || MeFwsts2.r.CpuReplacedValid == 0x0) {
    DEBUG ((EFI_D_ERROR, "Timeout occurred, the CPU Replacement Valid Bit is not set.\n"));
    *ForceFullTraining = 0x1;
  } else {
    if (MeFwsts2.r.CpuReplacedValid == 0x1) {
      if (MeFwsts2.r.WarmRstReqForDF == 0x1) {
        ///
        /// Clear DISB and Issue a Non-Power Cycle Reset
        ///
        Status  = ClearDisb ();
        Status  = PerformReset (PeiServices, CBM_DIR_NON_PCR);
      }

      if ((MeFwsts2.r.CpuReplacedSts == 0x1 && MeFwsts2.r.WarmRstReqForDF == 0x0)) {
        *ForceFullTraining = 0x1;
      }
    }
  }

  return Status;
}

/**
  This procedure will configure the ME Host General Status register,
  indicating that DRAM Initialization is complete and ME FW may
  begin using the allocated ME UMA space.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] FfsHeader            Pointer to the FFS file header
  @param[in] MrcBootMode          MRC BootMode
  @param[in] InitStat             H_GS[27:24] Status
  @param[in] FtpmStolenBase       The base of FTPM
  @retval EFI_SUCCESS
**/
EFI_STATUS
MeConfigDidReg (
  IN CONST EFI_PEI_SERVICES       **PeiServices,
  IN EFI_FFS_FILE_HEADER          *FfsHeader,
  IN MRC_BOOT_MODE_T              MrcBootMode,
  IN UINT8                        InitStat,
  IN UINT32                       FtpmStolenBase,
  IN UINT32                       MeUmaSize
  )
{
  UINT32                          MeUmaBase;
  UINT32                          MeUmaBaseExt;
  UINT32                          MeHgs;
  UINT32                          Timeout;
  HECI_FWS_REGISTER               MeHfs;
  EFI_STATUS                      Status;
  EFI_PEI_STALL_PPI               *StallPpi;
#ifdef PTT_FLAG
  PEI_ME_PLATFORM_POLICY_PPI      *PeiMePolicyPpi;
  EFI_GUID                        gPeiMePlatformPolicyPpiGuid = PEI_ME_PLATFORM_POLICY_PPI_GUID;
  ME_DATA_HOB                     *MeDataHob;
  EFI_GUID                        gMeDataHobGuid   = ME_DATA_HOB_GUID;
  UINT32                          RegRead;
  UINT32                          WaitTime;
#endif //PTT_FLAG

#ifdef PTT_FLAG
  ///
  /// Get platform policy settings through the MePlatformPolicy PPI
  ///
  Status = (**PeiServices).LocatePpi (
                            PeiServices,
                            &gPeiMePlatformPolicyPpiGuid,
                            0,
                            NULL,
                            (VOID **) &PeiMePolicyPpi
                            );
  ASSERT_EFI_ERROR (Status);

  DumpMePlatformPolicyPei (PeiMePolicyPpi);
#endif //PTT_FLAG

  Status = (*PeiServices)->LocatePpi (PeiServices, &gEfiPeiStallPpiGuid, 0, NULL, (VOID **) &StallPpi);
  ASSERT_EFI_ERROR (Status);

  MeHgs     = 0x0;
  Timeout   = 0x0;
  MeHfs.ul  = HeciPciRead32 (R_ME_HFS);

  DEBUG ((EFI_D_INFO, "ME status: 0x%08x\n", MeHfs.ul));

  if (MeHfs.r.MeOperationMode == ME_OPERATION_MODE_DEBUG) {
    DEBUG ((EFI_D_INFO, "ME debug mode, do not check for ME UMA. \n"));
    return EFI_SUCCESS;
  }

  if (MeHfs.r.ErrorCode != 0) {
    DEBUG ((EFI_D_INFO, "ME error, do not check for ME UMA. \n"));
    return EFI_SUCCESS;
  }

  DEBUG ((EFI_D_INFO, "Entered ME DRAM Init Done procedure.\n"));

#ifdef PTT_FLAG
    if ((GetPchSeries() == PchLp) && 
      (MmioRead32 (R_PTT_HCI_BASE_ADDRESS + R_PTT_HCI_STS)& BIT0) && 
      (PeiMePolicyPpi->FTpmSwitch == 0) && 
	  (InitStat != 0x1)) {
        ///
        /// Install ME HOBs
        ///
        InstallMeHob (PeiServices);

        MeDataHob       = NULL;
        MeDataHob = GetFirstGuidHob (&gMeDataHobGuid);

        if (MeDataHob != NULL) {
          MeDataHob->FtpmBufferAddress = FtpmStolenBase;
          DEBUG ((EFI_D_INFO, "  Ftpm Allocated Buffer Address: %x\n", MeDataHob->FtpmBufferAddress ));

        ///
        /// Poll Ready Bit
        ///
        RegRead = 0;
        for (WaitTime = 0; WaitTime < PTT_HCI_TIMEOUT_B * 3; WaitTime += PTT_HCI_POLLING_PERIOD){
          RegRead = MmioRead32 (R_PTT_HCI_BASE_ADDRESS + R_PTT_HCI_STS);
          DEBUG ((EFI_D_INFO, " Ftpm Waiting on Ready Bit, HCI_STS Register = %x\n", RegRead));
          if((BIT1 & RegRead) != 0){
            MmioWrite64 ((UINTN)(R_PTT_HCI_BASE_ADDRESS + R_PTT_HCI_CRB),(UINTN) MeDataHob->FtpmBufferAddress );
            ///
            /// Issue command to start.
            ///
            MmioWrite32 ((UINTN)R_PTT_HCI_BASE_ADDRESS + R_PTT_HCI_CA_START, V_PTT_HCI_BUFFER_ADDRESS_RDY);
            if( MeUmaSize == 32 )
            {
              ///
              /// Trigger interrupt command processing only for 5MB sku
              ///
              MmioWrite32 ((UINTN)R_PTT_HCI_BASE_ADDRESS + R_PTT_HCI_CMD, 0);
            }
            break;
          }
          StallPpi->Stall (PeiServices, StallPpi, PTT_HCI_POLLING_PERIOD);
          }

        ///
        /// Poll state, wait for Ftpm to finish processing
        ///
        for (WaitTime = 0; WaitTime < PTT_HCI_TIMEOUT_B; WaitTime += PTT_HCI_POLLING_PERIOD){
          RegRead = MmioRead32 (R_PTT_HCI_BASE_ADDRESS + R_PTT_HCI_CA_START);
          if (RegRead == 0){
            break;
          }
          StallPpi->Stall (PeiServices, StallPpi, PTT_HCI_POLLING_PERIOD);
          }
        } else {
          DEBUG ((EFI_D_INFO, "ME DataHob error, MeDataHob not found\n"));
        }
      }
#endif

  ///
  /// Read MESEGBASE value
  ///
  MeUmaBase     = PciRead32 (PCI_LIB_ADDRESS (0, 0, 0, R_MESEG_BASE));
  MeUmaBaseExt  = PciRead32 (PCI_LIB_ADDRESS (0, 0, 0, R_MESEG_BASE + 0x04));
  DEBUG ((EFI_D_INFO, "  MeUmaBase read: %x\n", MeUmaBase));

  ///
  /// Write DRAM Init Done (DID) data to the ME H_GS[23:0].
  /// H_GS[23:16] = extended UMA base address (reserved)
  /// H_GS[15:0] = 1M aligned UMA base address
  /// ME FW will 0 extend these values to determine MeUmaBase
  ///
  MeUmaBase = ((MeUmaBaseExt << 28) + (MeUmaBase >> 4)) >> 16;
  MeHgs     = MeUmaBase;

  ///
  /// Set H_GS[31:28] = 0x1 indicating DRAM Init Done
  ///
  MeHgs |= B_ME_DID_TYPE_MASK;

  ///
  /// RapidStart
  ///
  if (InitStat & 0x80) {
    MeHgs |= B_ME_DID_RAPID_START_BIT;
  }

  InitStat &= 0x7F;

  ///
  /// Set H_GS[27:24] = Status
  ///   0x0 = Success
  ///   0x1 = No Memory in channels
  ///   0x2 = Memory Init Error
  ///   0x3 = Memory not preserved across reset
  ///   0x4-0xF = Reserved
  ///
  MeHgs |= (InitStat << 24);

  PERF_START_EX (FfsHeader, "DID", NULL, AsmReadTsc (), 0x4020);
  HeciPciAndThenOr32 (R_ME_H_GS, 0, MeHgs);
  DEBUG ((EFI_D_INFO, "  ME H_GS written: %x\n", MeHgs));

  ///
  /// ME FW typically responds with the DID ACK w/in 1ms
  /// Adding short delay to avoid wasting time in the timeout loop
  ///
  StallPpi->Stall (PeiServices, StallPpi, STALL_1_MILLISECOND + STALL_100_MICROSECONDS);

  ///
  /// Read the ME H_FS Register to look for DID ACK.
  ///
  MeHfs.ul = HeciPciRead32 (R_ME_HFS);
  DEBUG ((EFI_D_INFO, "  HFS read before DID ACK: %x\n", MeHfs.r));

  ///
  /// ~5 second Timeout for DID ACK
  ///
  while (((MeHfs.r.BiosMessageAck == 0) && Timeout < DID_TIMEOUT_MULTIPLIER)
  ) {
    StallPpi->Stall (PeiServices, StallPpi, STALL_1_MILLISECOND);
    MeHfs.ul = HeciPciRead32 (R_ME_HFS);
    Timeout++;
  }

  if ((Timeout >= DID_TIMEOUT_MULTIPLIER)
  ) {
    DEBUG ((EFI_D_ERROR, "Timeout occurred waiting for DID ACK.\n"));
  } else {
    DEBUG ((EFI_D_INFO, "ME DRAM Init Done ACK received.\n"));
    DEBUG ((EFI_D_INFO, "HFS read after DID ACK: %x\n", MeHfs.r));
  }
  PERF_END_EX (FfsHeader, "DID", NULL, AsmReadTsc (), 0x4021);

  DEBUG ((EFI_D_ERROR, "BiosAction = %x\n", MeHfs.r.AckData));

  return HandleMeBiosAction (PeiServices, MrcBootMode, (UINT8) MeHfs.r.AckData);
}

/**
  This procedure will enforce the BIOS Action that was requested by ME FW
  as part of the DRAM Init Done message.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] MrcBootMode          MRC BootMode
  @param[in] BiosAction           Me requests BIOS to act

  @retval EFI_SUCCESS             Always return EFI_SUCCESS
**/
EFI_STATUS
HandleMeBiosAction (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN MRC_BOOT_MODE_T              MrcBootMode,
  IN UINT8                        BiosAction
  )
{
  EFI_STATUS                      Status;
  HECI_GS_SHDW_REGISTER           MeFwsts2;
  ///
  /// Read ME FWS2
  ///
  MeFwsts2.ul = HeciPciRead32 (R_ME_GS_SHDW);
  DEBUG ((EFI_D_INFO, "MeFwsts2 = %x.\n", MeFwsts2.r));

  switch (BiosAction) {
  case 0:
    ///
    /// Case: DID ACK was not received
    ///
    DEBUG ((EFI_D_ERROR, "DID Ack was not received, no BIOS Action to process.\n"));
    break;

  case CBM_DIR_NON_PCR:
    ///
    /// Case: Perform Non-Power Cycle Reset
    ///
    DEBUG ((EFI_D_ERROR, "ME FW has requested a Non-PCR.\n"));
    Status = PerformReset (PeiServices, CBM_DIR_NON_PCR);
    break;

  case CBM_DIR_PCR:
    ///
    /// Case: Perform Power Cycle Reset
    ///
    DEBUG ((EFI_D_ERROR, "ME FW has requested a PCR.\n"));
    Status = PerformReset (PeiServices, CBM_DIR_PCR);
    break;

  case 3:
    ///
    /// Case: Go To S3
    ///
    DEBUG ((EFI_D_INFO, "ME FW DID ACK has requested entry to S3.  Not defined, continuing to POST.\n"));
    break;

  case 4:
    ///
    /// Case: Go To S4
    ///
    DEBUG ((EFI_D_INFO, "ME FW DID ACK has requested entry to S4.  Not defined, continuing to POST.\n"));
    break;

  case 5:
    ///
    /// Case: Go To S5
    ///
    DEBUG ((EFI_D_INFO, "ME FW DID ACK has requested entry to S5.  Not defined, continuing to POST.\n"));
    break;

  case CBM_DIR_GLOBAL_RESET:
    ///
    /// Case: Perform Global Reset
    ///
    DEBUG ((EFI_D_ERROR, "ME FW has requested a Global Reset.\n"));
    Status = PerformReset (PeiServices, CBM_DIR_GLOBAL_RESET);
    break;

  case CBM_DIR_CONTINUE_POST:
    ///
    /// Case: Continue to POST
    ///
    DEBUG ((EFI_D_INFO, "ME FW DID Ack requested to continue to POST.\n"));
    break;
  }

  return EFI_SUCCESS;
}

/**
  This procedure will issue a Non-Power Cycle, Power Cycle, or Global Rest.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] ResetType            Type of reset to be issued.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
PerformReset (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  UINT8                           ResetType
  )
{
  EFI_STATUS                      Status;
  UINT32                          ETR3;
  UINT32                          GpioBase;
  UINT8                           Reset;
  WDT_PPI                         *Wdt;
  PCH_SERIES                      PchSeries;
//[-start-130416-IB10920021-add]//
  UINT32                          PchRootComplexBar;
//[-end-130416-IB10920021-add]//

  ETR3      = 0;
  Reset     = 0;
  GpioBase  = 0;
  Wdt       = NULL;
  PchSeries = GetPchSeries();


//[-start-130416-IB10920021-add]//
  PchRootComplexBar = MmioRead32 (
                        MmPciAddress (
                          0,
                          DEFAULT_PCI_BUS_NUMBER_PCH,
                          PCI_DEVICE_NUMBER_PCH_LPC,
                          0,
                          R_PCH_LPC_RCBA
                          )
                        )
                      &~BIT0;
//[-end-130416-IB10920021-add]//

  ///
  /// Locate WDT PPI for access to Wdt->AllowKnownReset()
  ///
  Status = (*PeiServices)->LocatePpi (
                            PeiServices,
                            &gWdtPpiGuid,
                            0,
                            NULL,
                            (VOID **) &Wdt
                            );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Clear CF9GR of PCH (B0/D31/f0 offset 0x0AC[20] = 1b) to  indicate Host reset
  /// Make sure CWORWRE (CF9 Without Resume Well Reset Enable) is cleared
  ///
  ETR3  = PciRead32 (PCI_LIB_ADDRESS (0, PCI_DEVICE_NUMBER_PCH_LPC, 0, R_PCH_LPC_PMIR));
  ETR3  = ETR3 &~BIT20;
  ETR3  = ETR3 &~BIT18;
  PciWrite32 (PCI_LIB_ADDRESS (0, PCI_DEVICE_NUMBER_PCH_LPC, 0, R_PCH_LPC_PMIR), ETR3);

  Reset = IoRead8 (R_PCH_RST_CNT);
  Reset &= 0xF1;

  ///
  /// If global reset required
  ///
  if (ResetType == CBM_DIR_GLOBAL_RESET) {
    ///
    /// Get GPIO Base Address
    ///
    GpioBase = PciRead32 (PCI_LIB_ADDRESS (0, PCI_DEVICE_NUMBER_PCH_LPC, 0, R_PCH_LPC_GPIO_BASE)) &~BIT0;
  }

  switch (ResetType) {
  case CBM_DIR_NON_PCR:
    ///
    /// Case: Non Power Cycle Reset requested
    ///
    DEBUG ((EFI_D_ERROR, "ME FW DID ACK has requested a Non Power Cycle Reset.\n"));
    Reset |= 0x06;
    break;

  case CBM_DIR_PCR:
    ///
    /// Case: Power Cycle Reset requested
    ///
    Wdt->ReloadAndStart (2);
    DEBUG ((EFI_D_INFO, "ME FW DID ACK has requested a Power Cycle Reset.\n"));
    Reset |= 0x0E;
    break;

  case CBM_DIR_GLOBAL_RESET:
    ///
    /// Case: Global Reset
    ///
    DEBUG ((EFI_D_ERROR, "ME FW DID Ack requested a global reset.\n"));

    ///
    /// Drive GPIO[30] (SPDNA#) low prior to 0xCF9 write
    ///
    if (PchSeries == PchH) {
      /// 1. Set GPIOBASE + offset 00h[30] = 1b (for non-Deep Sx enabled platforms)
      /// 2. Set GPIOBASE + offset 04h[30] = 0b (for non-Deep Sx enabled platforms)
      /// 3. Set GPIOBASE + offset 0Ch[30] = 0b (for non-Deep Sx enabled platforms)
      /// 4. Set GPIOBASE + offset 60h[30] = 1b (for non-Deep Sx enabled platforms)
      /// NOTE: For Deep Sx enabled platforms steps 1,2 and 3 should be skipped and pin should be left in native mode
      /// 5. Set CF9GR bit, D31:F0:ACh[20], issue a Global Reset through a 0xCF9 write of either 06h or 0Eh commands.
      /// Global Reset MEI Message
      /// 1. BIOS makes sure GPIO30 is left in native mode (default mode) before sending a Global Reset MEI message.
      ///
      IoOr32 ((UINTN) (GpioBase + R_PCH_GPIO_USE_SEL), (UINT32) (BIT30));
      IoAnd32 ((UINTN) (GpioBase + R_PCH_GPIO_IO_SEL), (UINT32) (~BIT30));
      IoAnd32 ((UINTN) (GpioBase + R_PCH_GPIO_LVL), (UINT32) (~BIT30));
    }

    if (PchSeries == PchLp) {
      /// 1. Set GPIOBASE + offset 1F0h[0]  = 1b (for non-Deep Sx enabled platforms)
      /// 2. Set GPIOBASE + offset 1F0h[2]  = 0b (for non-Deep Sx enabled platforms)
      /// 3. Set GPIOBASE + offset 1F0h[31] = 0b (for non-Deep Sx enabled platforms)
      /// 4. Set GPIOBASE + offset 60h[30]  = 1h (for non-Deep Sx enabled platforms)
      /// NOTE: For Deep Sx enabled platforms steps 1,2 and 3 should be skipped and pin should be left in native mode
      /// 5. Set CF9GR bit, D31:F0:ACh[20], issue a Global Reset through a 0xCF9 write of either 06h or 0Eh commands.
      /// Global Reset MEI Message
      /// 1. BIOS makes sure GPIO30 is left in native mode (default mode) before sending a Global Reset MEI message.
      ///
      IoOr32 ((UINTN) (GpioBase + R_PCH_GP_30_CONFIG0), (UINT32) (B_PCH_GPIO_OWN0_GPIO_USE_SEL));
      IoAnd32 ((UINTN) (GpioBase + R_PCH_GP_30_CONFIG0), (UINT32) (~B_PCH_GPIO_OWN0_GPIO_IO_SEL));
      IoAnd32 ((UINTN) (GpioBase + R_PCH_GP_30_CONFIG0), (UINT32) (~B_PCH_GPIO_OWN0_GPO_LVL));
    }
    
//[-start-130416-IB10920021-modify]//
    if ((MmioRead16 (PchRootComplexBar + R_PCH_SPI_HSFS) & B_PCH_SPI_HSFS_FDV) == B_PCH_SPI_HSFS_FDV) {
      MmioAnd32 ((PchRootComplexBar + R_PCH_SPI_FDOC), (UINT32) (~(B_PCH_SPI_FDOC_FDSS_MASK | B_PCH_SPI_FDOC_FDSI_MASK)));
      MmioOr32 ((PchRootComplexBar + R_PCH_SPI_FDOC), (UINT32) (V_PCH_SPI_FDOC_FDSS_PCHS | R_PCH_SPI_STRP10));
      if (!(MmioRead32 (PchRootComplexBar + R_PCH_SPI_FDOD) & B_PCH_SPI_STRP10_DEEP_SX_EN)) {    

        IoOr32 ((UINTN) (GpioBase + R_PCH_GP_RST_SEL), (UINT32) (BIT30));
      }
    }
//[-end-130416-IB10920021-modify]//

    PciOr32 (
        PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH,
        PCI_DEVICE_NUMBER_PCH_LPC,
        PCI_FUNCTION_NUMBER_PCH_LPC,
        R_PCH_LPC_PMIR),
        (UINT32) (B_PCH_LPC_PMIR_CF9GR)
        );

    ///
    /// Issue global reset CF9 = 0x0E
    ///
    DEBUG ((EFI_D_ERROR, "Issuing global reset.\n"));
    Reset |= 0x0E;
    break;
  }
  ///
  /// Write PCH RST CNT, Issue Reset
  ///
  Wdt->AllowKnownReset ();

  IoWrite8 (R_PCH_RST_CNT, Reset);

  return EFI_SUCCESS;
}

/**
  This procedure will clear the DISB.

  @param[in] None

  @retval EFI_SUCCESS             Always return EFI_SUCCESS
**/
EFI_STATUS
ClearDisb (
  VOID
  )
{
  UINT16  Data16;

  Data16  = PciRead16 (PCI_LIB_ADDRESS (0, PCI_DEVICE_NUMBER_PCH_LPC, 0, R_PCH_LPC_GEN_PMCON_2));
  Data16  = Data16 &~B_PCH_LPC_GEN_PMCON_DRAM_INIT;
  PciWrite16 (PCI_LIB_ADDRESS (0, PCI_DEVICE_NUMBER_PCH_LPC, 0, R_PCH_LPC_GEN_PMCON_2), Data16);

  return EFI_SUCCESS;
}

/**
  This procedure will clear the DISB.

  @param[in] None

  @retval EFI_SUCCESS             Always return EFI_SUCCESS
**/
EFI_STATUS
SetDISB (
  VOID
  )
{
  UINT16  Data16;

  Data16  = PciRead16 (PCI_LIB_ADDRESS (0, PCI_DEVICE_NUMBER_PCH_LPC, 0, R_PCH_LPC_GEN_PMCON_2));
  Data16  = Data16 & B_PCH_LPC_GEN_PMCON_DRAM_INIT;
  PciWrite16 (PCI_LIB_ADDRESS (0, PCI_DEVICE_NUMBER_PCH_LPC, 0, R_PCH_LPC_GEN_PMCON_2), Data16);

  return EFI_SUCCESS;
}

/**
  This function is the entry point for this PEI.

  @param[in] FileHandle           Handle of the file being invoked. 
  @param[in] PeiServices          Describes the list of possible PEI Services.

  @retval Return Status based on errors that occurred while waiting for time to expire.

**/
EFI_STATUS
EFIAPI
PchMeUmaEntry (
  IN       EFI_PEI_FILE_HANDLE         FileHandle,
  IN CONST EFI_PEI_SERVICES            **PeiServices
  )
{
  EFI_STATUS  Status;
  Status  = EFI_SUCCESS;
  Status  = (*PeiServices)->InstallPpi (PeiServices, mPchMeUmaPpiList);

  return Status;
}
