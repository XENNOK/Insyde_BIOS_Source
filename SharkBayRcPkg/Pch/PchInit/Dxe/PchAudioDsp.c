/** @file
  Configures Audio DSP device

@copyright
  Copyright (c) 2012 Intel Corporation. All rights reserved
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
#include "PchInit.h"

#ifdef ADSP_FLAG
#include "PchAslUpdateLib.h"

//
//  AUDIO DSP Memory space definitions
//
#define V_PCH_ADSP_MEM_BASE_ADDRESS         0xFE000000
#define S_PCH_ADSP_ADBAR_LENGTH             0x100000
#define S_PCH_ADSP_SPCBAR_LENGTH            0x1000
#define N_PCH_ADSP_ADBAR_ALIGN              16
#define N_PCH_ADSP_SPCBAR_ALIGN             12

/**
  Configure AudioDSP SSP lines ownership

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance

  @retval EFI_SUCCESS             ADSP owns all I/O Buffers
**/
EFI_STATUS
ConfigureAudioDspSsp(
  IN DXE_PCH_PLATFORM_POLICY_PROTOCOL *PchPlatformPolicy
  )
{
  UINTN       PciAzaliaRegBase;

  ///
  /// Retrieve Azalia PCI Config base
  ///
  PciAzaliaRegBase = MmPciAddress (
                      0,
                      PchPlatformPolicy->BusNumber,
                      PCI_DEVICE_NUMBER_PCH_AZALIA,
                      PCI_FUNCTION_NUMBER_PCH_AZALIA,
                      0
                      );

  DEBUG ((EFI_D_INFO, "Audio DSP: Switch HDA pins to ADSP\n"));

  ///
  /// BIOS is required to set Ownership select of I/O Buffer to Audio DSP.
  /// Set D27:F0:42h[7:6] = 11b - Audio DSP subsystem owns all the I/O buffers.
  ///
  MmioOr8 ((UINTN)(PciAzaliaRegBase + R_PCH_HDA_AZIOBC), (UINT8)B_PCH_HDA_AZIOBC_OSEL);
  SCRIPT_MEM_WRITE (
     EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
     EfiBootScriptWidthUint8,
     (UINTN) (PciAzaliaRegBase + R_PCH_HDA_AZIOBC),
     1,
     (VOID *) (UINTN) (PciAzaliaRegBase + R_PCH_HDA_AZIOBC)
     );

  PchPlatformPolicy->DeviceEnabling->Azalia = PCH_DEVICE_DISABLE;

  return EFI_SUCCESS;
}

/**
  Allocates static memory pool for Audio DSP BAR and Shadowed PCI Configuration Space.

  @retval EFI_SUCCESS                 The function completed successfully
  @retval EFI_ERROR                   Error occured on initialization
**/
EFI_STATUS
AllocateAudioDspBar (
)
{
  EFI_PHYSICAL_ADDRESS   AdspMemBaseAddress;
  EFI_PHYSICAL_ADDRESS   AdspBar;
  EFI_PHYSICAL_ADDRESS   AdspShadowedPciBar;
  UINT32                 PciAdspRegBase;
  EFI_STATUS             Status;

  DEBUG ((EFI_D_INFO, "Audio DSP: Allocate fixed memory space\n"));

  AdspMemBaseAddress = V_PCH_ADSP_MEM_BASE_ADDRESS;

  Status = gDS->AddMemorySpace (
                  EfiGcdMemoryTypeReserved,
                  AdspMemBaseAddress,
                  S_PCH_ADSP_ADBAR_LENGTH + S_PCH_ADSP_SPCBAR_LENGTH,
                  0
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  AdspBar = AdspMemBaseAddress;

  Status = gDS->AllocateMemorySpace (
                  EfiGcdAllocateAddress,
                  EfiGcdMemoryTypeReserved,
                  N_PCH_ADSP_ADBAR_ALIGN,
                  S_PCH_ADSP_ADBAR_LENGTH,
                  &AdspBar,
                  mImageHandle,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  ASSERT(AdspBar == V_PCH_ADSP_MEM_BASE_ADDRESS);

  AdspShadowedPciBar = AdspMemBaseAddress + S_PCH_ADSP_ADBAR_LENGTH;

  Status = gDS->AllocateMemorySpace (
                  EfiGcdAllocateAddress,
                  EfiGcdMemoryTypeReserved,
                  N_PCH_ADSP_SPCBAR_ALIGN,
                  S_PCH_ADSP_SPCBAR_LENGTH,
                  &AdspShadowedPciBar,
                  mImageHandle,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    return Status;
  }  
  ASSERT(AdspShadowedPciBar == V_PCH_ADSP_MEM_BASE_ADDRESS + S_PCH_ADSP_ADBAR_LENGTH);

  PciAdspRegBase = MmPciAddress (0, 0, PCI_DEVICE_NUMBER_PCH_ADSP, PCI_FUNCTION_NUMBER_PCH_ADSP, 0);

  DEBUG ((EFI_D_INFO, "Audio DSP: Base Address (ADBA) = 0x%04x; Length = 0x%x\n", AdspBar, S_PCH_ADSP_ADBAR_LENGTH));

  MmioWrite32 ((UINTN)(PciAdspRegBase + R_PCH_ADSP_ADBA), (UINT32)AdspBar);
  PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (PciAdspRegBase + R_PCH_ADSP_ADBA),
    1,
    (VOID *)(UINTN) (PciAdspRegBase + R_PCH_ADSP_ADBA)
    );

  DEBUG ((EFI_D_INFO, "Audio DSP: Shadowed PCI Configuration Base Address (SPCBA) = 0x%04x; Length = 0x%x\n", AdspShadowedPciBar, S_PCH_ADSP_SPCBAR_LENGTH));

  MmioWrite32 ((UINTN)(PciAdspRegBase + R_PCH_ADSP_SPCBA), (UINT32)AdspShadowedPciBar);
  PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (PciAdspRegBase + R_PCH_ADSP_SPCBA),
    1,
    (VOID *)(UINTN) (PciAdspRegBase + R_PCH_ADSP_SPCBA)
    );

  ///
  /// Enable memory space access
  /// Program D19:F0:04h[2:1] = 11b
  ///
  MmioOr32 ((UINTN)(PciAdspRegBase + R_PCH_ADSP_COMMAND), (B_PCH_ADSP_COMMAND_BME | B_PCH_ADSP_COMMAND_MSE));
  PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint16,
    (UINTN) (PciAdspRegBase + R_PCH_ADSP_COMMAND),
    1,
    (VOID *) (UINTN) (PciAdspRegBase + R_PCH_ADSP_COMMAND)
    );

  return Status;
}

/**
  Disables/hides or enables/unhides Audio DSP PCI Configuration Space

  @param[in] PciConfigurationDisable  If TRUE, PCI Config Space will be disabled.
                                      If FALSE, PCI Config Space will be enabled.
  @param[in] RootComplexBar           RootComplexBar value of this PCH device

  @retval EFI_SUCCESS                 The function completed successfully
  @retval EFI_DEVICE_ERROR            Transaction fail
**/
EFI_STATUS
DisableAudioDspPciConfigSpace(
  IN BOOLEAN                          PciConfigurationDisable,
  IN UINT32                           RootComplexBar
  )
{
  UINT32        Data32Or;
  EFI_STATUS    Status;

  if (PciConfigurationDisable) {
    DEBUG ((EFI_D_INFO, "Audio DSP: Hiding PCI Config Space\n"));
    Data32Or = B_PCH_ADSP_PCICFGCTL_PCICD;
  } else {
    DEBUG ((EFI_D_INFO, "Audio DSP: Unhiding PCI Config Space\n"));
    Data32Or = 0;
  }

  Status = ProgramIobp (
            RootComplexBar,
            (UINT32) (R_PCH_RCRB_IOBPIRI_IOBPIS_ADSP + R_PCH_ADSP_PCICFGCTL),
            (UINT32)~(B_PCH_ADSP_PCICFGCTL_PCICD),
            (UINT32) Data32Or
            );
  ASSERT_EFI_ERROR (Status);
  Status = PCH_INIT_COMMON_SCRIPT_SAVE_IOBP_S3_ITEM (
            RootComplexBar,
            (UINT32) (R_PCH_RCRB_IOBPIRI_IOBPIS_ADSP + R_PCH_ADSP_PCICFGCTL),
            (UINT32)~(B_PCH_ADSP_PCICFGCTL_PCICD),
            (UINT32) Data32Or
            );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  Finalize Audio DSP initialization after PCI enumeration.
  In particular configure ADSP in ACPI or PCI mode:
  ACPI - patches ACPI table, sets ACPI IRQ and hides PCI config space.
  PCI - sets PCI IRQ, does not hide PCI config space.

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] RootComplexBar       RootComplexBar value of this PCH device

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_UNSUPPORTED         Audio DSP not found or not enabled
**/
EFI_STATUS
ConfigureAudioDspBeforeBoot(
  IN DXE_PCH_PLATFORM_POLICY_PROTOCOL *PchPlatformPolicy,
  IN UINT32                           RootComplexBar
  )
{
  UINT8                          Data8;
  UINT32                         Data32;  
  UINT32                         PciAdspRegBase;
  UINT16                         AdspDeviceId;
  EFI_STATUS                     Status;
  EFI_STATUS                     AcpiTablePresent;

  Status = EFI_SUCCESS;
  AcpiTablePresent = EFI_NOT_FOUND;

  DEBUG ((EFI_D_INFO, "ConfigureAudioDspBeforeBoot() Start\n"));

  if (PchPlatformPolicy->DeviceEnabling->AudioDsp == PCH_DEVICE_ENABLE) {

    PciAdspRegBase = MmPciAddress (
                       0,
                       PchPlatformPolicy->BusNumber,
                       PCI_DEVICE_NUMBER_PCH_ADSP,
                       PCI_FUNCTION_NUMBER_PCH_ADSP,
                       0
                       );

    // Unhide ADSP PCI Config Space to do the final initialization
    Status = DisableAudioDspPciConfigSpace (FALSE, RootComplexBar);

    AdspDeviceId = MmioRead16 (PciAdspRegBase + R_PCH_LPTLP_ADSP_DEVICE_ID);

    if (AdspDeviceId == V_PCH_LPTLP_ADSP_DEVICE_ID) {

      if (PchPlatformPolicy->AudioDspConfig->AudioDspAcpiMode) {
        //
        // Locate ACPI table
        //
        AcpiTablePresent = InitializePchAslUpdateLib();

        ///
        /// Assign DSP BARs for ACPI use
        ///
        if(!EFI_ERROR(AcpiTablePresent)) {
          DEBUG ((EFI_D_INFO, "Audio DSP: Updating ACPI tables\n"));
          Data32 = (MmioRead32(PciAdspRegBase + R_PCH_ADSP_ADBA) & MMIO_ADDR_MASK);
          UpdateResourceTemplateAslCode((EFI_SIGNATURE_32 ('A', 'D', 'S', 'P')),
                                          (EFI_SIGNATURE_32 ('R', 'B', 'U', 'F')),
                                          AML_MEMORY32_FIXED_OP,
                                          1,
                                          0x04,
                                          &Data32,
                                          sizeof(Data32)
                                          );

          Data32 = (MmioRead32(PciAdspRegBase + R_PCH_ADSP_SPCBA) & MMIO_ADDR_MASK);
          UpdateResourceTemplateAslCode((EFI_SIGNATURE_32 ('A', 'D', 'S', 'P')),
                                          (EFI_SIGNATURE_32 ('R', 'B', 'U', 'F')),
                                          AML_MEMORY32_FIXED_OP,
                                          2,
                                          0x04,
                                          &Data32,
                                          sizeof(Data32)
                                          );

          if (PchPlatformPolicy->AudioDspConfig->AudioDspBluetoothSupport) {
            DEBUG ((EFI_D_INFO, "Audio DSP: Bluetooth support enabled\n"));
            Data8 = PCH_DEVICE_ENABLE;
            UpdateNameAslCode(EFI_SIGNATURE_32('A','B','T','H'), &Data8, sizeof(Data8));
          }

          if (PchPlatformPolicy->AudioDspConfig->AudioDspAcpiInterruptMode == PCH_DEVICE_DISABLE) {
            Data8 = 23; // PCI IRQ 23
            UpdateResourceTemplateAslCode((EFI_SIGNATURE_32 ('A', 'D', 'S', 'P')),
                                            (EFI_SIGNATURE_32 ('R', 'B', 'U', 'F')),
                                            AML_INTERRUPT_DESC_OP,
                                            1,
                                            0x05,
                                            &Data8,
                                            sizeof(Data8)
                                            );
          }
        }
      }

      if (PchPlatformPolicy->AudioDspConfig->AudioDspAcpiInterruptMode) {
        DEBUG ((EFI_D_INFO, "Audio DSP: ACPI Interrupt mode\n"));

        ///
        /// Set Interrupt De-assert/Assert Opcode Override to IRQ3
        ///
        Data32 = V_PCH_ADSP_VDLDAT2_IRQ3;
        Status = ProgramIobp (
                  RootComplexBar,
                  (UINT32) (R_PCH_RCRB_IOBPIRI_IOBPIS_ADSP + R_PCH_ADSP_VDLDAT2),
                  (UINT32)~(V_PCH_ADSP_VDLDAT2_MASK),
                  (UINT32) (Data32)
                  );
        ASSERT_EFI_ERROR (Status);
        Status = PCH_INIT_COMMON_SCRIPT_SAVE_IOBP_S3_ITEM (
                  RootComplexBar,
                  (UINT32) (R_PCH_RCRB_IOBPIRI_IOBPIS_ADSP + R_PCH_ADSP_VDLDAT2),
                  (UINT32)~(V_PCH_ADSP_VDLDAT2_MASK),
                  (UINT32) (Data32)
                  );
        ASSERT_EFI_ERROR (Status);

        // Enable IRQ3 in RCRB
        MmioOr32 ((UINTN)(RootComplexBar + R_PCH_RCRB_INT_ACPIIRQEN), B_PCH_RCRB_INT_ACPIIRQEN_A3E);
        PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
          EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
          EfiBootScriptWidthUint32,
          (UINTN) (RootComplexBar + R_PCH_RCRB_INT_ACPIIRQEN),
          1,
          (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_INT_ACPIIRQEN)
          );

        // Set ACPI Interrupt Enable bit
        Data32 = B_PCH_ADSP_PCICFGCTL_ACPIIE;

      } else {
        DEBUG ((EFI_D_INFO, "Audio DSP: PCI Interrupt mode\n"));

        /// Program D19:F0:3Ch = 23 - INTLN (Interrupt Line) to IRQ23
        DEBUG ((EFI_D_INFO, "Audio DSP: Set INTLN to IRQ23\n"));
        MmioWrite32 ((UINTN)(PciAdspRegBase + 0x3C), (UINT32)(0x17));
        PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
          EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
          EfiBootScriptWidthUint32,
          (UINTN) (PciAdspRegBase + 0x3C),
          1,
          (VOID *)(UINTN) (PciAdspRegBase + 0x3C)
          );

        // Do not set ACPI Interrupt Enable bit
        Data32 = 0;
      }

      ///
      /// Configure ADSP in ACPI or PCI interrupt mode
      /// Update ACPI Interrupt Enable (bit 1) in PCICFGCTL (offset 0x500) accordingly
      ///
      Status = ProgramIobp (
                RootComplexBar,
                (UINT32) (R_PCH_RCRB_IOBPIRI_IOBPIS_ADSP + R_PCH_ADSP_PCICFGCTL),
                (UINT32)~(B_PCH_ADSP_PCICFGCTL_ACPIIE | B_PCH_ADSP_PCICFGCTL_SPCBAD),
                (UINT32) (Data32)
                );
      ASSERT_EFI_ERROR (Status);
      Status = PCH_INIT_COMMON_SCRIPT_SAVE_IOBP_S3_ITEM (
                RootComplexBar,
                (UINT32) (R_PCH_RCRB_IOBPIRI_IOBPIS_ADSP + R_PCH_ADSP_PCICFGCTL),
                (UINT32)~(B_PCH_ADSP_PCICFGCTL_ACPIIE | B_PCH_ADSP_PCICFGCTL_SPCBAD),
                (UINT32) (Data32)
                );

      if (PchPlatformPolicy->AudioDspConfig->AudioDspAcpiMode) {
        ///
        /// Configure ADSP in ACPI mode
        /// Set PCI Configuration Disable (bit 0) in PCICFGCTL (offset 0x500)
        ///
        DEBUG ((EFI_D_INFO, "Audio DSP: ACPI mode\n"));
        Status = DisableAudioDspPciConfigSpace(TRUE, RootComplexBar);
      }
    }
  }

  DEBUG ((EFI_D_INFO, "ConfigureAudioDspBeforeBoot() End\n"));  

  return Status;
}

/**
  Initialize Audio DSP subsystem

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] RootComplexBar       RootComplexBar value of this PCH device
  @param[in, out] FuncDisableReg  The value of Function disable register

  @retval EFI_SUCCESS             Codec is detected and initialized
  @retval EFI_UNSUPPORTED         Audio DSP disabled
  @retval EFI_OUT_OF_RESOURCES    Failed to allocate resources to initialize the codec
**/
EFI_STATUS
ConfigureAudioDsp (
  IN      DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy,
  IN      UINT32                              RootComplexBar,
  IN OUT  UINT32                              *FuncDisableReg
  )
{
  UINT32                         Data32;
  UINT32                         PciAdspRegBase;
  UINT32                         AdspBar;
  UINT16                         AdspDeviceId;
  EFI_STATUS                     Status;

  DEBUG ((EFI_D_INFO, "ConfigureAudioDsp() Start\n"));

  if (PchPlatformPolicy->DeviceEnabling->AudioDsp == PCH_DEVICE_ENABLE) {

    PciAdspRegBase = MmPciAddress (
                       0,
                       PchPlatformPolicy->BusNumber,
                       PCI_DEVICE_NUMBER_PCH_ADSP,
                       PCI_FUNCTION_NUMBER_PCH_ADSP,
                       0
                       );

    AdspDeviceId = MmioRead16 (PciAdspRegBase + R_PCH_LPTLP_ADSP_DEVICE_ID);

    if (AdspDeviceId == V_PCH_LPTLP_ADSP_DEVICE_ID) {
      DEBUG ((EFI_D_INFO, "Audio DSP: Found and Enabled\n"));

      Status = AllocateAudioDspBar();
      if (EFI_ERROR(Status)) {
        return Status;
      }

      AdspBar = MmioRead32 (PciAdspRegBase + R_PCH_ADSP_ADBA);

      ///
      /// Set LTR value in DSP Shim LTR Control register to 3ms
      /// SNOOP_REQ[13] = 1b, SNOOP_SCALE[12:10] = 100b (1ms), SNOOP_VAL[9:0] = 3h
      ///
      MmioWrite32 ((UINTN)(AdspBar + (R_PCH_ADSP_SHIM_BASE + R_PCH_ADSP_SHIM_LTRC)), (UINT32)V_PCH_ADSP_SHIM_LTRC);
      PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (AdspBar + (R_PCH_ADSP_SHIM_BASE + R_PCH_ADSP_SHIM_LTRC)),
        1,
        (VOID *)(UINTN) (AdspBar + (R_PCH_ADSP_SHIM_BASE + R_PCH_ADSP_SHIM_LTRC))
        );

      ///
      /// Program VDRTCTL2 D19:F0:A8h[31:0] = FFFh
      ///
      MmioWrite32 (PciAdspRegBase + R_PCH_ADSP_VDRTCTL2, (UINT32)V_PCH_ADSP_VDRTCTL2);
      PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (PciAdspRegBase + R_PCH_ADSP_VDRTCTL2),
        1,
        (VOID *)(UINTN) (PciAdspRegBase + R_PCH_ADSP_VDRTCTL2)
        );

      ///
      /// Set DSP IOBP register VDLDAT1 (0x624) to 0x040100
      ///
      Status = ProgramIobp (
                RootComplexBar,
                (UINT32) (R_PCH_RCRB_IOBPIRI_IOBPIS_ADSP + R_PCH_ADSP_VDLDAT1),
                (UINT32)~(V_PCH_ADSP_VDLDAT1_CCO),
                (UINT32) (V_PCH_ADSP_VDLDAT1_CCO)
                );
      ASSERT_EFI_ERROR (Status);
      Status = PCH_INIT_COMMON_SCRIPT_SAVE_IOBP_S3_ITEM (
                RootComplexBar,
                (UINT32) (R_PCH_RCRB_IOBPIRI_IOBPIS_ADSP + R_PCH_ADSP_VDLDAT1),
                (UINT32)~(V_PCH_ADSP_VDLDAT1_CCO),
                (UINT32) (V_PCH_ADSP_VDLDAT1_CCO)
                );
      ASSERT_EFI_ERROR (Status);

      ///
      /// Set D3 Power Gating Enable
      /// Program D19:F0:A0h[2:1] = 00b
      ///
      Data32 = MmioRead32 (PciAdspRegBase + R_PCH_ADSP_VDRTCTL0);
      if (PchPlatformPolicy->AudioDspConfig->AudioDspD3PowerGating) {
        DEBUG ((EFI_D_INFO, "Audio DSP: D3 Power Gating Enabled\n"));
        Data32 &= ~(B_PCH_ADSP_VDRTCTL0_D3PGD | B_PCH_ADSP_VDRTCTL0_D3SRAMPGD);
      } else {
        DEBUG ((EFI_D_INFO, "Audio DSP: D3 Power Gating Disabled\n"));
        Data32 |= B_PCH_ADSP_VDRTCTL0_D3PGD | B_PCH_ADSP_VDRTCTL0_D3SRAMPGD;
      }

      MmioWrite32 ((UINTN)(PciAdspRegBase + R_PCH_ADSP_VDRTCTL0), Data32);
      PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (PciAdspRegBase + R_PCH_ADSP_VDRTCTL0),
        1,
        &Data32
        );

      ///
      /// Set PSF Snoop to SA
      /// Program RCBA + 0x3350[10] = 1b
      ///
      MmioOr32 ((UINTN)(RootComplexBar + R_PCH_RCRB_CIR3350), (UINT32) BIT10);
      PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3350),
        1,
        (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3350)
        );

      ///
      /// Switch I/O Buffers ownership to ADSP
      ///
      ConfigureAudioDspSsp (PchPlatformPolicy);

      ///
      /// Disable ADSP PCI Configuration Space in order
      /// to avoid Base Adresses override on PCI enumeration
      ///
      DisableAudioDspPciConfigSpace (TRUE, RootComplexBar);

      }

      Status = EFI_SUCCESS;
  }
  else {
    DEBUG ((EFI_D_INFO, "Audio DSP: Disabled\n"));
    ///
    /// Set RCBA + 2B1Ch[29] = 1b
    ///
    MmioOr32 ((UINTN)(RootComplexBar + 0x2B1C), (UINT32)BIT29);
    PCH_INIT_COMMON_SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + 0x2B1C),
      1,
      (VOID *) (UINTN) (RootComplexBar + 0x2B1C)
      );

    ///
    /// Set Audio DSP function disable by programming RCBA + 3418h[1] = 1b
    ///
    *FuncDisableReg |= B_PCH_RCRB_FUNC_DIS_ADSP;
    Status = EFI_SUCCESS;
  }

  ///
  /// Set DSP IOBP register PMCTL (0x1E0) to 0x3F
  /// This should be set for both: ADSP enabled and disabled
  ///
  Status = ProgramIobp (
            RootComplexBar,
            (UINT32) (R_PCH_RCRB_IOBPIRI_IOBPIS_ADSP + R_PCH_ADSP_PMCTL),
            (UINT32)~(V_PCH_ADSP_PMCTL),
            (UINT32) (V_PCH_ADSP_PMCTL)
            );
  ASSERT_EFI_ERROR (Status);
  Status = PCH_INIT_COMMON_SCRIPT_SAVE_IOBP_S3_ITEM (
            RootComplexBar,
            (UINT32) (R_PCH_RCRB_IOBPIRI_IOBPIS_ADSP + R_PCH_ADSP_PMCTL),
            (UINT32)~(V_PCH_ADSP_PMCTL),
            (UINT32) (V_PCH_ADSP_PMCTL)
            );
  ASSERT_EFI_ERROR (Status);

  DEBUG ((EFI_D_INFO, "ConfigureAudioDsp() End\n"));

  return Status;
}

#endif // ADSP_FLAG
