/** @file

  SA Dmi PEI Initialization library

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

#include "SaDmiPeim.h"
#include "PciExpressInit.h"

///
/// Functions
///
/**
  Initialize DMI Tc/Vc mapping through SA-PCH.

  @param[in] PeiServices          - General purpose services available to every PEIM.
  @param[in] SaPlatformPolicyPpi  - Instance of SA_PLATFORM_POLICY_PPI

  @retval EFI_SUCCESS
**/
EFI_STATUS
SaDmiTcVcInit (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SA_PLATFORM_POLICY_PPI      *SaPlatformPolicyPpi
  )
{
  EFI_STATUS        Status;
  UINT64            MchBar;
  UINT64            DmiBar;
  PCH_INIT_PPI      *PchInitPpi;
  PCH_DMI_TC_VC_PPI *PchDmiTcVcMapPpi;
  CPU_FAMILY        CpuFamilyId;
  UINT8             i;

  MchBar  = McD0PciCfg64 (R_SA_MCHBAR) &~BIT0;
  DmiBar  = McD0PciCfg64 (R_SA_DMIBAR) &~BIT0;
  CpuFamilyId = GetCpuFamily();

  ///
  /// Locate PchInitPpi and PchDmiTcVcMapPpi
  ///
  Status = (*PeiServices)->LocatePpi (PeiServices, &gPchInitPpiGuid, 0, NULL, (VOID **) &PchInitPpi);
  ASSERT_EFI_ERROR (Status);

  Status = (*PeiServices)->LocatePpi (PeiServices, &gPchDmiTcVcMapPpiGuid, 0, NULL, (VOID **) &PchDmiTcVcMapPpi);
  ASSERT_EFI_ERROR (Status);

  ///
  /// SA OPI Initialization
  ///
  if (CpuFamilyId == EnumCpuHswUlt) {
    MmioOr8 ((UINTN) (DmiBar + 0xA78), BIT1);
  }

  ///
  /// Update DmiTcVcMapping based on Policy
  ///
  PchDmiTcVcMapPpi->DmiVc[DmiVcTypeVc1].Enable  = (BOOLEAN) SaPlatformPolicyPpi->PcieConfig->DmiVc1;
  PchDmiTcVcMapPpi->DmiVc[DmiVcTypeVcp].Enable  = (BOOLEAN) SaPlatformPolicyPpi->PcieConfig->DmiVcp;
  PchDmiTcVcMapPpi->DmiVc[DmiVcTypeVcm].Enable  = (BOOLEAN) SaPlatformPolicyPpi->PcieConfig->DmiVcm;

  for (i = 0; i < DmiTcTypeMax; i++) {
    if (((PchDmiTcVcMapPpi->DmiTc[i].Vc == DmiVcTypeVc1) && (PchDmiTcVcMapPpi->DmiVc[DmiVcTypeVc1].Enable == FALSE)) ||
        ((PchDmiTcVcMapPpi->DmiTc[i].Vc == DmiVcTypeVcp) && (PchDmiTcVcMapPpi->DmiVc[DmiVcTypeVcp].Enable == FALSE)) ||
        ((PchDmiTcVcMapPpi->DmiTc[i].Vc == DmiVcTypeVcm) && (PchDmiTcVcMapPpi->DmiVc[DmiVcTypeVcm].Enable == FALSE))
        ) {
      PchDmiTcVcMapPpi->DmiTc[i].Vc = DmiVcTypeVc0;
    }
  }
  ///
  /// Program NB TC/VC mapping
  ///
  SaSetDmiTcVcMapping (PchDmiTcVcMapPpi, DmiBar);

  ///
  /// Call PchDmiTcVcProgPoll
  ///
  Status = PchInitPpi->DmiTcVcProgPoll (PeiServices);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Poll NB negotiation completion
  ///
  SaPollDmiVcStatus (PchDmiTcVcMapPpi, DmiBar);
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;

}

/**
  Map SA DMI TCs to VC

  @param[in] PchDmiTcVcMapPpi     - Instance of PCH_DMI_TC_VC_PPI
  @param[in] DmiBar               - DMIBAR address

  @retval EFI_SUCCESS            -  Succeed.
  @retval EFI_INVALID_PARAMETER  -  Wrong phase parameter passed in.
**/
EFI_STATUS
SaSetDmiTcVcMapping (
  IN    PCH_DMI_TC_VC_PPI   *PchDmiTcVcMapPpi,
  IN    UINT64              DmiBar
  )
{
  UINT32  Data32And;
  UINT32  Data32Or;
  UINT8   Data8And;
  UINT8   Data8Or;
  UINT8   Index;
  UINT16  Register;
  UINT8   VcId;
  UINT8   VcMap[DmiVcTypeMax] = { 0 };

  ///
  /// Set the TC/VC mappings
  ///
  for (Index = 0; Index < DmiTcTypeMax; Index++) {
    VcMap[PchDmiTcVcMapPpi->DmiTc[Index].Vc] |= (BIT0 << Index);
  }
  ///
  /// System BIOS must perform the following steps for VC0 configuration.
  ///   Program the TCs/VC0 map by setting DMIBAR offset 014h [7:1] = '0111 101b'.
  ///
  /// Private Virtual Channel Configuration
  /// Step1. Assign Virtual Channel ID 2 to VCp:
  ///    Programming the DMIVCPRCTL DMI Port Register DMIBAR Offset 02Ch[26:24] = '010b'.
  ///
  /// Step2. Set TC2 to VCp:
  ///    Program the DMIVCPRCTL DMI Port Register DMIBAR offset 02Ch [7:1] = '0000 010b'.
  ///
  /// Step3. Enable VCp by programming the DMIVCPRCTL DMI Port Register DMIBAR Offset 02Ch[31] = '1b'.
  ///
  /// Virtual Channel for ME (VCm) Configuration
  /// This is configured by ConfigMemMe
  ///
  /// Step1. Assign Virtual Channel ID 7 to VCm:
  ///    Programming the DMIVCMRCTL DMI Port Register DMIBAR Offset 038h[26:24] = '111b'.
  ///
  /// Step2. Enable VCm:
  ///    Programming the DMIVMPRCTL DMI Port Register DMIBAR Offset 038h[31] = '1b'.
  ///
  /// Step3. Enable VCm by programming the DMIVCMRCTL DMI Port Register DMIBAR Offset 038h[31] = '1b'.
  ///
  for (Index = 0; Index < DmiVcTypeMax; Index++) {
    if (PchDmiTcVcMapPpi->DmiVc[Index].Enable == PCH_DEVICE_ENABLE) {
      ///
      /// Map TCs to VC, Set the VC ID, Enable VC
      ///
      VcId = PchDmiTcVcMapPpi->DmiVc[Index].VcId,

      Data32And = (UINT32) (~(V_SA_DMIBAR_DMIVCCTL_ID | B_SA_DMIBAR_DMIVCCTL_TVM_MASK));
      Data32Or = VcId << N_SA_DMIBAR_DMIVCCTL_ID;
      Data32Or |= VcMap[Index];
      Data32Or |= N_SA_DMIBAR_DMIVCCTL_EN;

      switch (Index) {
      case DmiVcTypeVc0:
        Register = R_SA_DMIBAR_DMIVC0RCTL_OFFSET;
        break;

      case DmiVcTypeVc1:
        Register = R_SA_DMIBAR_DMIVC1RCTL_OFFSET;
        break;

      case DmiVcTypeVcp:
        Register = R_SA_DMIBAR_DMIVCPRCTL_OFFSET;
        break;

      case DmiVcTypeVcm:
        Register = R_SA_DMIBAR_DMIVCMRCTL_OFFSET;
        break;

      default:
        return EFI_INVALID_PARAMETER;
      }

      MmioAndThenOr32 ((UINTN) (DmiBar + Register), Data32And, Data32Or);
    }
  }
  ///
  /// System BIOS must program the extended VC Count:
  ///    Set the DMI Port Register DMIBAR Offset 004h[2:0]=001b
  ///
  Data8And = (UINT8) (~0x07);
  if (PchDmiTcVcMapPpi->DmiVc[DmiVcTypeVc1].Enable == TRUE) {
    Data8Or = 1;
  } else {
    Data8Or = 0;
  }

  MmioAndThenOr8 ((UINTN) (DmiBar + R_SA_DMIBAR_DMIPVCCAP1_OFFSET), Data8And, Data8Or);

  return EFI_SUCCESS;
}

/**
  Poll SA DMI negotiation completion

  @param[in] PchDmiTcVcMapPpi     - Instance of PCH_DMI_TC_VC_PPI
  @param[in] DmiBar               - DMIBAR address

  @retval EFI_SUCCESS            -  Succeed.
  @retval EFI_INVALID_PARAMETER  -  Wrong phase parameter passed in.
**/
EFI_STATUS
SaPollDmiVcStatus (
  IN    PCH_DMI_TC_VC_PPI   *PchDmiTcVcMapPpi,
  IN    UINT64              DmiBar
  )
{
  UINT8   Index;
  UINT16  Register;

  ///
  /// 6.2.3.2 - Step 4, Poll until VC1 has been negotiated
  ///    Read the DMIVC1RSTS DMI Port Register Offset 026h until [1]==0
  ///
  /// 6.2.3.3 - Step4. Poll the VCp Negotiation Pending bit until it reads 0:
  ///    Read the DMIVCPRSTS DMI Port Register Offset 032h until [1]==0
  ///
  /// 6.2.3.4 - Step4. Poll the VCm Negotiation Pending bit until it reads 0:
  ///    Read the DMIVCMRSTS DMI Port Register Offset 03Eh until [1]==0
  ///
  for (Index = 0; Index < DmiVcTypeMax; Index++) {
    if (PchDmiTcVcMapPpi->DmiVc[Index].Enable == PCH_DEVICE_ENABLE) {
      switch (Index) {
      case DmiVcTypeVc0:
        Register = R_SA_DMIBAR_DMIVC0RSTS_OFFSET;
        break;

      case DmiVcTypeVc1:
        Register = R_SA_DMIBAR_DMIVC1RSTS_OFFSET;
        break;

      case DmiVcTypeVcp:
        Register = R_SA_DMIBAR_DMIVCPRSTS_OFFSET;
        break;

      case DmiVcTypeVcm:
        Register = R_SA_DMIBAR_DMIVCMRSTS_OFFSET;
        break;

      default:
        return EFI_INVALID_PARAMETER;
      }
      ///
      /// Wait for negotiation to complete
      ///
      while ((MmioRead16 ((UINTN) (DmiBar + Register)) & B_SA_DMIBAR_DMISTS_NP) != 0);
    }
  }

  return EFI_SUCCESS;
}

#ifdef DMI_FLAG
/**
  Initialize DMI.

  @param[in] PeiServices          - General purpose services available to every PEIM.
  @param[in] SaPlatformPolicyPpi  - Instance of SA_PLATFORM_POLICY_PPI

  @retval EFI_SUCCESS
**/
EFI_STATUS
DmiInit (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SA_PLATFORM_POLICY_PPI      *SaPlatformPolicyPpi
  )
{
  EFI_STATUS              Status;
  UINT64                  MchBar;
  UINT64                  DmiBar;
  PCH_INIT_PPI            *PchInitPpi;
  PCH_PLATFORM_POLICY_PPI *PchPlatformPolicyPpi;
  UINT8                   CpuSteppingId;
  BOOLEAN                 DmiGen2Enable;
  UINT16                  LinkStatus;
  UINT32                  Data32Or;

  ///
  /// Read the CPU stepping
  ///
  CpuSteppingId = GetCpuStepping();

  ///
  /// BridgeId = (UINT8) (McD0PciCfg16 (R_MC_DEVICE_ID) & 0xF0);
  /// BridgeSteppingId = BridgeId + CpuSteppingId;
  ///
  MchBar  = McD0PciCfg64 (R_SA_MCHBAR) &~BIT0;
  DmiBar  = McD0PciCfg64 (R_SA_DMIBAR) &~BIT0;

  ///
  /// Get RCBA through the PchPlatformPolicy PPI
  ///
  Status = (**PeiServices).LocatePpi (
                            PeiServices,
                            &gPchPlatformPolicyPpiGuid,
                            0,
                            NULL,
                            (VOID **)&PchPlatformPolicyPpi
                            );
  ASSERT_EFI_ERROR (Status);

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Can't locate PchPlatformPolicy PPI - exiting.\n"));
    return Status;
  }
  ///
  /// Perform DMI Recipe steps
  ///
  DEBUG ((EFI_D_INFO, "DMI Recipe...\n"));
  PegDmiRecipe (SaPlatformPolicyPpi, (UINT32) MchBar, (UINT32) DmiBar, 0, 0);

  ///
  /// Additional DMI steps. See SA BIOS Spec.
  ///
  DEBUG ((EFI_D_INFO, "Run AdditionalDmiProgramSteps!\n"));
  AdditionalDmiProgramSteps (SaPlatformPolicyPpi, (UINT32) MchBar, (UINT32) DmiBar);

  DmiGen2Enable = TRUE;
  if ((SaPlatformPolicyPpi->PcieConfig->DmiGen2 == 0) ||
      ((MmioRead8 ((UINTN) PchPlatformPolicyPpi->Rcba + R_PCH_RCRB_LCAP) & (BIT0 | BIT1 | BIT2 | BIT3)) == 0x1) ||
      (McDevFunPciCfg32 (0, 0, 0, R_SA_MC_CAPID0_A_OFFSET) & BIT22)
     ) {
    DEBUG ((EFI_D_WARN, "DMI Gen2 is Disabled or not capable, staying at Gen1 !\n"));
    DmiGen2Enable = FALSE;
  }

  if (DmiGen2Enable) {
    ///
    /// Locate PchInitPpi
    ///
    Status = (*PeiServices)->LocatePpi (PeiServices, &gPchInitPpiGuid, 0, NULL, (VOID **)&PchInitPpi);
    ASSERT_EFI_ERROR (Status);

    ///
    /// Program PCH TLS to Gen 2
    ///
    PchInitPpi->DmiGen2Prog (PeiServices);

    ///
    /// Program CPU Max Link Speed to Gen 2
    ///
    MmioAndThenOr32 ((UINTN) (DmiBar + R_SA_DMIBAR_LCAP_OFFSET), (UINT32)~0xF, 2);
  }
  Data32Or = (MmioRead32 ((UINTN) (DmiBar + R_SA_DMIBAR_LCAP_OFFSET)) & (BIT3 | BIT2 | BIT1 | BIT0));
  MmioAndThenOr32 ((UINTN) (DmiBar + R_SA_DMIBAR_LCTL2_OFFSET), (UINT32)~(BIT3 | BIT2 | BIT1 | BIT0), Data32Or);

  ///
  /// Retrain link
  ///
  DmiLinkTrain (DmiBar);

  ///
  /// Retrain link if it is GEN2 Capable and it is not yet set to GEN2
  ///
  if (DmiGen2Enable &&
      ((((MmioRead16 ((UINTN) (DmiBar + R_SA_DMIBAR_LSTS_OFFSET))) & 0x0F) != DMI_GEN2) ||
       (((MmioRead16 ((UINTN) PchPlatformPolicyPpi->Rcba + R_PCH_RCRB_LSTS)) & 0x0F) != DMI_GEN2))
     ) {
    DEBUG ((EFI_D_INFO, "DMI Link re-train to set GEN2\n"));
    DmiLinkTrain (DmiBar);
  }
  ///
  /// Get the current link status
  ///
  LinkStatus = MmioRead16 ((UINTN) (DmiBar + R_SA_DMIBAR_LSTS_OFFSET));
  DEBUG ((EFI_D_INFO, "DMI trained to x%d at Gen%d\n", (LinkStatus >> 4) & 0x3F, LinkStatus & 0x0F));

  return EFI_SUCCESS;
}

/**
  DMI link training

  @param[in] DmiBar - DMIBAR address
**/
VOID
DmiLinkTrain (
  IN  UINT64 DmiBar
  )
{
  ///
  /// Retrain link
  ///
  MmioOr8 ((UINTN) (DmiBar + R_SA_DMIBAR_LCTL_OFFSET), BIT5);

  ///
  /// Wait for link training complete
  ///
  while ((MmioRead16 ((UINTN) (DmiBar + R_SA_DMIBAR_LSTS_OFFSET)) & BIT11) != 0)
    ;
}

/**
  Additional DMI Programming Steps at PEI

  @param[in] SaPlatformPolicyPpi - pointer to SA_PLATFORM_POLICY_PPI
  @param[in] MchBar              - MCHBAR address
  @param[in] DmiBar              - DMIBAR address
**/
VOID
AdditionalDmiProgramSteps (
  IN SA_PLATFORM_POLICY_PPI    *SaPlatformPolicyPpi,
  IN UINT32                    MchBar,
  IN UINT32                    DmiBar
  )
{
  UINT32  Data32And;
  UINT32  Data32Or;

  ///
  /// Disable DMI and PEG Debug Align Message - set 0x258[29] = '1b'
  ///
  Data32And = (UINT32) ~BIT29;
  Data32Or  = BIT29;
  Mmio32AndThenOr (DmiBar, R_SA_DMIBAR_CFG4_OFFSET, Data32And, Data32Or);

  ///
  /// Overwrite DMICC (DMIBAR offset 0x208) to 0x6B5
  ///
  Data32And = (UINT32)~(BIT10 | BIT9 | BIT8 | BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
  Data32Or  = 0x6B5;
  Mmio32AndThenOr (DmiBar, R_SA_DMIBAR_DMICC_OFFSET, Data32And, Data32Or);

  ///
  /// Set L0SLAT[15:0] to 0x2020
  ///
  Data32And = (UINT32) ~(0xFFFF);
  Data32Or  = 0x00002020;
  Mmio32AndThenOr (DmiBar, R_SA_DMIBAR_L0SLAT_OFFSET, Data32And, Data32Or);
}
#endif // DMI_FLAG
