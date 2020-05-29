/**
  This file contains an 'Intel Peripheral Driver' and uniquely        
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your   
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  PchLan.c
  
@brief
  Initializes PCH LAN Device

**/
#include "PchInit.h"

/**
  Enable GbE Controller

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] RootComplexBar       RootComplexBar value of this PCH device

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
EnableGbEController (
  DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy,
  UINT32                              RootComplexBar
  )
{
  ///
  /// PCH BIOS Spec Rev 0.5.0, section 10.2.1
  /// Done in PchInitPeimm.c PchGbeMandatedReset()
  ///
  return EFI_SUCCESS;
}

/**
  Disable GbE Controller

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] RootComplexBar       RootComplexBar value of this PCH device

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
DisableGbEController (
  DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy,
  UINT32                              RootComplexBar
  )
{
  ///
  /// PCH BIOS Spec Rev 0.5.0, section 10.2.2
  /// Done in PchInitPeimm.c PchGbeMandatedReset()
  ///
  return EFI_SUCCESS;
}

/**
  Initialize LAN device. Reference: PCH BIOS Spec Rev 0.5.0,
  10.2 Enabling / Disabling the Internal GbE Controller
  ** NOTE:
    - The platform reset mandated by GbE enabling / disabling is handled
      in PchInit PEIM. Platform PEI code is responsible for calling PCH Init PPI
    - BUC register setting is also done in the PCH Init PPI

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] RootComplexBar       RootComplexBar address of this PCH device

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
ConfigureLan (
  IN  DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy,
  IN  UINT32                              RootComplexBar
  )
{
  UINT32  RegData32;
  UINTN   PciD25F0RegBase;
  PCH_SERIES PchSeries;

  DEBUG ((EFI_D_INFO, "ConfigureLan() Start\n"));
  PchSeries = GetPchSeries();
  ///
  /// If SPI is used and in Descriptor mode, the PCIE Port X need to be disabled to use GbE
  /// if not, the GbE should be disabled
  ///
  if (PchIsSpiDescriptorMode (RootComplexBar)) {
    DEBUG ((EFI_D_INFO, "LAN can be enabled or disabled as SPI is in Descriptor Mode.\n"));

    if (PchPlatformPolicy->DeviceEnabling->Lan == PCH_DEVICE_DISABLE) {
      ///
      /// Disable LAN
      ///
      DisableGbEController (PchPlatformPolicy, RootComplexBar);
    } else {
      ///
      /// Enable LAN
      ///
      EnableGbEController (PchPlatformPolicy, RootComplexBar);

      ///
      /// PCH BIOS Spec Rev 0.5.6 Section 10.7.1 LTR Programming
      /// The maximum snoop/non-snoop platform latency values to 00000846h
      /// in the GbE controller's PCI LTR capability register at D25:F0:Reg 0A8h
      ///
      if (PchPlatformPolicy->PwrOptConfig->PchPwrOptGbe == PCH_DEVICE_ENABLE) {
        PciD25F0RegBase = MmPciAddress (
                            0,
                            PCI_BUS_NUMBER_PCH_LAN,
                            PCI_DEVICE_NUMBER_PCH_LAN,
                            PCI_FUNCTION_NUMBER_PCH_LAN,
                            0
                            );
        RegData32 = 0x00000000;
        if (PchSeries == PchH) {
          RegData32 = 0x00000846;
        }
        if (PchSeries == PchLp) {
          RegData32 = 0x00001003;
        }
        MmioWrite32 (PciD25F0RegBase + R_PCH_LAN_LTR_CAP, RegData32);
        SCRIPT_MEM_WRITE (
          EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
          EfiBootScriptWidthUint32,
          (UINTN) (PciD25F0RegBase + R_PCH_LAN_LTR_CAP),
          1,
          (VOID *) (UINTN) (PciD25F0RegBase + R_PCH_LAN_LTR_CAP)
          );
      }
    }
  } else {
    ///
    /// Non Descriptor mode: Disable LAN
    ///
    DEBUG ((EFI_D_ERROR, "LAN is disabled as SPI not in Descriptor Mode.\n"));
    ///
    /// Disable LAN
    ///
    DisableGbEController (PchPlatformPolicy, RootComplexBar);
  }

  DEBUG ((EFI_D_INFO, "ConfigureLan() End\n"));

  return EFI_SUCCESS;
}
