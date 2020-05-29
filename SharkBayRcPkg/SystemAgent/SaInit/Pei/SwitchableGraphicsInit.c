/** @file

  SwitchableGraphics Pei driver.
  This Pei driver initialize GPIO programming
  for the platform.

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

#include "SwitchableGraphicsInit.h"
#include <Guid/SaDataHob.h>

//[-start-140224-IB10920078-modify]//
//[-start-120627-IB05330350-modify]//
#ifdef SG_SUPPORT
//[-end-120627-IB05330350-modify]//
//[-end-140224-IB10920078-modify]//
//[-start-140226-IB10920078-add]//
//-#include <Library/PcdLib.h>
//[-end-140226-IB10920078-add]//

/**
  Initialize the SwitchableGraphics support (PEI).

  @param[in] PeiServices          - Pointer to the PEI services table
  @param[in] SaPlatformPolicyPpi  - SaPlatformPolicyPpi to access the GtConfig related information
**/
VOID
SwitchableGraphicsInit (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SA_PLATFORM_POLICY_PPI      *SaPlatformPolicyPpi
  )
{
  EFI_STATUS              Status;
  EFI_PEI_STALL_PPI       *StallPpi;
  EFI_GUID                StallPpiGuid             = EFI_PEI_STALL_PPI_GUID;
  UINT16                  GpioAddress;
  SA_DATA_HOB             *SaDataHob;
  BOOLEAN                 SgDgpuPrsntGpioIsValid   = TRUE;
  CPU_FAMILY              CpuFamilyId;

//[-start-140227-IB10920078-add]//
  BOOLEAN                 DgpuMode = Inactive;
//[-end-140227-IB10920078-add]//

  CpuFamilyId = GetCpuFamily();

  ///
  /// Get SaDataHob HOB
  ///
  SaDataHob = NULL;
  SaDataHob = (SA_DATA_HOB *) GetFirstGuidHob (&gSaDataHobGuid);

  if (SaDataHob != NULL) {
    SaDataHob->SgInfo.SgMode = SaPlatformPolicyPpi->PlatformData->SgMode;
    ///
    /// GPIO Assigned from policy
    ///
    SaDataHob->SgInfo.SgGpioSupport  = SaPlatformPolicyPpi->SgGpioData->GpioSupport;

//[-start-140225-IB10920078-modify]//
    if ((SaPlatformPolicyPpi->PlatformData->SgMode == SgModeMuxless) ||
              (SaPlatformPolicyPpi->PlatformData->SgMode == SgModeDgpu)) {
    //if (SaPlatformPolicyPpi->SgGpioData->GpioSupport == 1) {
//[-end-140225-IB10920078-modify]//
      ///
      /// Get GPIO Base Address
      ///
      GpioAddress = MmPci16 (0, 0, PCI_DEVICE_NUMBER_PCH_LPC, 0, R_PCH_LPC_GPIO_BASE) &~BIT0;

      SaDataHob->SgInfo.SgDgpuPwrOK     = SaPlatformPolicyPpi->SgGpioData->SgDgpuPwrOK->Value;
      SaDataHob->SgInfo.SgDgpuHoldRst   = SaPlatformPolicyPpi->SgGpioData->SgDgpuHoldRst->Value;
      SaDataHob->SgInfo.SgDgpuPwrEnable = SaPlatformPolicyPpi->SgGpioData->SgDgpuPwrEnable->Value;
      SaDataHob->SgInfo.SgDgpuPrsnt     = SaPlatformPolicyPpi->SgGpioData->SgDgpuPrsnt->Value;

      ///
      /// Set Bit7 as indicator for GPIO Active Low/High
      ///
      SaDataHob->SgInfo.SgDgpuPwrOK     |= (SaPlatformPolicyPpi->SgGpioData->SgDgpuPwrOK->Active     << 7);
      SaDataHob->SgInfo.SgDgpuHoldRst   |= (SaPlatformPolicyPpi->SgGpioData->SgDgpuHoldRst->Active   << 7);
      SaDataHob->SgInfo.SgDgpuPwrEnable |= (SaPlatformPolicyPpi->SgGpioData->SgDgpuPwrEnable->Active << 7);
      SaDataHob->SgInfo.SgDgpuPrsnt     |= (SaPlatformPolicyPpi->SgGpioData->SgDgpuPrsnt->Active     << 7);

      if (CpuFamilyId == EnumCpuHswUlt) {
        DEBUG ((EFI_D_INFO, "SgDgpuPrsntGpioIsValid = FALSE\n"));
        SgDgpuPrsntGpioIsValid = FALSE;
      }

      ///
      /// Locate PPI stall service
      ///
      Status = (**PeiServices).LocatePpi (
                                PeiServices,
                                &StallPpiGuid,
                                0,
                                NULL,
                                (VOID **)&StallPpi
                                );
      if (!EFI_ERROR (Status)) {
        ///
        /// if DGPU PRSNT is Disabled, it means that MXM card was not detected, and
        /// DGPU HOLD RST must be driven high to allow the board to support a normal PEG card
        ///
        if (    (SgDgpuPrsntGpioIsValid)
             && (GpioRead (PeiServices, CpuFamilyId, GpioAddress, SaDataHob->SgInfo.SgDgpuPrsnt) == GP_DISABLE)) {
          GpioWrite (PeiServices, CpuFamilyId, GpioAddress, SaDataHob->SgInfo.SgDgpuHoldRst, GP_DISABLE);
            ///
            /// Set SG mode as disabled
            ///
            SaDataHob->SgInfo.SgMode = SgModeDisabled;
        } else {
          ///
          /// DGPU PRSNT Enabled. MXM is present.
          ///      If PEG Mode or SG Muxless
          ///              Power on MXM
          ///              Configure GPIOs to drive MXM in PEG mode or SG Muxless
          ///      else
          ///              Do Nothing
          ///
          if ((SaPlatformPolicyPpi->PlatformData->SgMode == SgModeMuxless) ||
              (SaPlatformPolicyPpi->PlatformData->SgMode == SgModeDgpu)) {
            DEBUG ((EFI_D_INFO, "Configure GPIOs for driving the dGPU.\n"));
            ///
            ///  Drive DGPU HOLD RST Enable to make sure we hold reset
            ///
            GpioWrite (PeiServices, CpuFamilyId, GpioAddress, SaDataHob->SgInfo.SgDgpuHoldRst, GP_ENABLE);
            ///
            /// wait 100ms
            ///
            StallPpi->Stall (
                        PeiServices,
                        StallPpi,
                        SG_DELAY_HOLD_RST
                        );

            ///
            /// Drive DGPU PWR EN to Power On MXM
            ///
            GpioWrite (PeiServices, CpuFamilyId, GpioAddress, SaDataHob->SgInfo.SgDgpuPwrEnable, GP_ENABLE);

            ///
            /// wait 300ms
            ///
            StallPpi->Stall (
                        PeiServices,
                        StallPpi,
                        SG_DELAY_PWR_ENABLE
                        );

            ///
            /// Drive DGPU HOLD RST Disabled to remove reset
            ///
            GpioWrite (PeiServices, CpuFamilyId, GpioAddress, SaDataHob->SgInfo.SgDgpuHoldRst, GP_DISABLE);

            ///
            /// wait 100ms
            ///
            StallPpi->Stall (
                      PeiServices,
                      StallPpi,
                      SG_DELAY_HOLD_RST
                      );
          }
        }
      }
    }
  }
  ///
  /// Program SubsystemID for IGFX
  ///
  DEBUG ((EFI_D_INFO, "Program SDID [Subsystem ID] for IGFX: 0x%x\n", SaPlatformPolicyPpi->PlatformData->SgSubSystemId));
  McD2PciCfg16Or (PCI_SID, SaPlatformPolicyPpi->PlatformData->SgSubSystemId);

//[-start-140226-IB10920078-add]//
      //
      // Select the discrete GPU to control output when discrete GPU only platform,
      // or select the internal GPU to control output when internal GPU only or dual VGA platform.
      //
      if (SaPlatformPolicyPpi->PlatformData->SgMode == SgModeDgpu) {
        DgpuMode = Active;
      }
      GpioAddress = MmPci16 (0, 0, PCI_DEVICE_NUMBER_PCH_LPC, 0, R_PCH_LPC_GPIO_BASE) &~BIT0;              
      
      if (PcdGetBool (PcdSgGpioDataSgDgpuEdidSelExist)) {
        GpioWrite (PeiServices, CpuFamilyId, GpioAddress, PcdGet8 (PcdSgGpioDataSgDgpuEdidSelValue), DgpuMode);
      }
      if (PcdGetBool (PcdSgGpioDataSgDgpuSelExist)) {
        GpioWrite (PeiServices, CpuFamilyId, GpioAddress, PcdGet8 (PcdSgGpioDataSgDgpuSelValue), DgpuMode);
      }
      if (PcdGetBool (PcdSgGpioDataSgDgpuPwmSelExist)) {
        GpioWrite (PeiServices, CpuFamilyId, GpioAddress, PcdGet8 (PcdSgGpioDataSgDgpuPwmSelValue), DgpuMode);
      }
//[-end-140226-IB10920078-add]//    
}

/**
  SG GPIO Read

  @param[in] PeiServices - General purpose services available to every PEIM
  @param[in] CpuFamilyId - Specifies the CPU family
  @param[in] GpioAddress - GPIO base address
  @param[in] Value       - PCH GPIO number

  @retval GPIO read value (0/1)
**/
BOOLEAN
GpioRead (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       CPU_FAMILY                  CpuFamilyId,
  IN       UINT16                      GpioAddress,
  IN       UINT8                       Value
  )
{
  BOOLEAN Active;
  UINT16  BitOffset=0;
  UINT32  Data;

  Active = (BOOLEAN) (Value >> 7);
  Value &= 0x7F;

  ASSERT (GpioAddress != 0);

  if (CpuFamilyId == EnumCpuHswUlt) {
      GpioAddress += R_PCH_GP_N_CONFIG0 + (Value * 0x08);
      BitOffset = 30; //GPI_LVL
  } else {
    if (Value < 0x20) {
      GpioAddress += R_PCH_GPIO_LVL;
      BitOffset = Value;
    } else if (Value < 0x40) {
      GpioAddress += R_PCH_GPIO_LVL2;
      BitOffset = Value - 0x20;
    } else {
      GpioAddress += R_PCH_GPIO_LVL3;
      BitOffset = Value - 0x40;
    }
  }

  Data = IoRead32 (GpioAddress);
  Data >>= BitOffset;

  if (Active == 0) {
    Data = ~Data;
  }

  return (BOOLEAN) (Data & 0x1);
}

/**
  SG GPIO Write

  @param[in] PeiServices - General purpose services available to every PEIM
  @param[in] CpuFamilyId - Specifies the CPU family
  @param[in] GpioAddress - GPIO base address
  @param[in] Value       - PCH GPIO number
  @param[in] Level       - Write SG GPIO value (0/1)

  @retval none
**/
VOID
GpioWrite (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       CPU_FAMILY                  CpuFamilyId,
  IN       UINT16                      GpioAddress,
  IN       UINT8                       Value,
  IN       BOOLEAN                     Level
  )
{
  BOOLEAN Active;
  UINT32  Data;
  UINT16  BitOffset=0;

  Active = (BOOLEAN) (Value >> 7);
  Value &= 0x7F;

  if (Active == 0) {
    Level = (~Level) & 0x1;
  }

  ASSERT (GpioAddress != 0);

  if (CpuFamilyId == EnumCpuHswUlt) {
      GpioAddress += R_PCH_GP_N_CONFIG0 + (Value * 0x08);
      BitOffset = 31; //GPO_LVL
  } else {
    if (Value < 0x20) {
      GpioAddress += R_PCH_GPIO_LVL;
      BitOffset = Value;
    } else if (Value < 0x40) {
      GpioAddress += R_PCH_GPIO_LVL2;
      BitOffset = Value - 0x20;
    } else {
      GpioAddress += R_PCH_GPIO_LVL3;
      BitOffset = Value - 0x40;
    }
  }

  Data = IoRead32 (GpioAddress);
  Data &= ~(0x1 << BitOffset);
  Data |= (Level << BitOffset);

  IoWrite32 (GpioAddress, Data);
  return ;
}

#endif //SG_SUPPORT
