/** @file
  Initializes PCH power management features.

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
  Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved
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
#include <PchInit.h>

//[-start-130708-IB05400427-add]//
UINT32 mThermalBaseB;
//[-end-130708-IB05400427-add]//

//[-start-130708-IB05400427-add]//
/**
 Wait for GlobalNvsAreaProtocol and update thermal base.

 @param[in]         Event               The Event associated with callback.
 @param[in]         Context             Context registered when Event was created.

*/
VOID
EFIAPI
GlobalNvsNotify (
  IN  EFI_EVENT                         Event,
  IN  VOID                              *Context
  )
{

  EFI_STATUS                      Status;
  EFI_GLOBAL_NVS_AREA_PROTOCOL    *GlobalNvsAreaProtocol;
  EFI_GLOBAL_NVS_AREA             *GlobalNvsArea;

  Status = gBS->LocateProtocol (&gEfiGlobalNvsAreaProtocolGuid, NULL, (VOID **)&GlobalNvsAreaProtocol);
  if (EFI_ERROR (Status)) {
    return;
  }

  DEBUG ((EFI_D_ERROR, "Set TBARB.\n"));

  GlobalNvsArea = GlobalNvsAreaProtocol->Area;
  GlobalNvsArea->TBARB = mThermalBaseB;

  gBS->CloseEvent (Event);
  
}
//[-end-130708-IB05400427-add]//

/**
  Set an PCH IOBP programming S3 dispatch item, this function may assert if any error happend

  @param[in] RootComplexBar       RootComplexBar value of this PCH device
  @param[in] Address              Address of the IOBP register block
  @param[in] AndMask              Mask to AND with the register
  @param[in] OrMask               Mask to OR with the register

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_OUT_OF_RESOURCES    Out of resources
  @retval EFI_INVALID_PARAMETER   Invalid parameter
  @retval EFI_NOT_FOUND           Protocol interface not found
**/
EFI_STATUS
SetProgramIobpS3Item (
  IN UINT32               RootComplexBar,
  IN UINT32               Address,
  IN UINT32               AndMask,
  IN UINT32               OrMask
  )
{
  EFI_STATUS                            Status;
#ifdef EFI_S3_RESUME
  STATIC EFI_PCH_S3_SUPPORT_PROTOCOL    *PchS3Support;
  STATIC EFI_PCH_S3_PARAMETER_PROG_IOBP S3ParameterProgramIobp;
  STATIC EFI_PCH_S3_DISPATCH_ITEM       S3DispatchItem = {
    PchS3ItemTypeProgramIobp,
    &S3ParameterProgramIobp
  };
  EFI_PHYSICAL_ADDRESS                  S3DispatchEntryPoint;
//[-start-121112-IB10920000-add]//
  EFI_S3_BOOT_SCRIPT_DISPATCH_PROTOCOL   *S3ScriptDispatch;
  Status = gBS->LocateProtocol (
                  &gEfiS3BootScriptDispatchProtocolGuid,
                  NULL,
                   (VOID **)&S3ScriptDispatch
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  } 
//[-end-121113-IB10920000-add]//

  if (!PchS3Support) {
    ///
    /// Get the PCH S3 Support Protocol
    ///
    Status = gBS->LocateProtocol (
                    &gEfiPchS3SupportProtocolGuid,
                    NULL,
                    (VOID **) &PchS3Support
                    );
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  S3ParameterProgramIobp.RootComplexBar = RootComplexBar;
  S3ParameterProgramIobp.Address        = Address;
  S3ParameterProgramIobp.AndMask        = AndMask;
  S3ParameterProgramIobp.OrMask         = OrMask;
  Status = PchS3Support->SetDispatchItem (
                          PchS3Support,
                          &S3DispatchItem,
                          &S3DispatchEntryPoint
                          );
  ASSERT_EFI_ERROR (Status);
  ///
  /// Save the script dispatch item in the Boot Script
  ///
  //SCRIPT_DISPATCH (EFI_ACPI_S3_RESUME_SCRIPT_TABLE, (VOID*)(UINTN)S3DispatchEntryPoint);
  //S3BootScriptSaveDispatch((VOID*)(UINTN)S3DispatchEntryPoint);
//[-start-121113-IB10920000-add]//
  Status = S3ScriptDispatch->Dispatch (
                                     S3ScriptDispatch,
                                     //EFI_BOOT_SCRIPT_DISPATCH_OPCODE,
                                     (VOID *)(UINTN)S3DispatchEntryPoint
                                     );
//[-end-121113-IB10920000-add]//
#else
  Status = EFI_SUCCESS;
#endif
  return Status;
}

/**
  Locking Thermal Reporting Settings

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] GpioBase             GPIO base address of this PCH device

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
ThermalLockDown (
  IN  DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy,
  UINT16                                  GpioBase
  )
{
  UINTN                 PciD31F6RegBase;
  UINTN                 PciD31F2RegBase;
  UINT32                ThermalBaseB;
  UINT32                ThermalBase;
  EFI_PHYSICAL_ADDRESS  MemBaseAddress;
  EFI_STATUS            Status;
  UINT8                 Index;
  UINT8                 RegData8;
  UINT16                RegData16;
  UINT32                RegData32;
  UINT32                RootComplexBar;
  UINT32                Data32And;
  UINT32                Data32Or = 0;
  UINT16                Data16And;
  UINT16                Data16Or;
  UINT32                PchTTLevels = 0;
  BOOLEAN               PchHotEnable;
  PCH_SERIES            PchSeries;
  EFI_CPUID_REGISTER    Cpuid;
  MSR_REGISTER          TempMsr;
  UINT32                temperature;
  UINT8                 MaxSataPortNum; 
  
//[-start-111222-IB05300263-add]//
  EFI_GLOBAL_NVS_AREA_PROTOCOL              *GlobalNvsAreaProtocol;
  EFI_GLOBAL_NVS_AREA                       *GlobalNvsArea;
//[-end-111222-IB05300263-add]//

  DEBUG ((EFI_D_INFO, "ThermalLockDown() Start\n"));

  PchSeries = GetPchSeries();
//[-start-130708-IB05400427-modify]//
  Status = gBS->LocateProtocol (&gEfiGlobalNvsAreaProtocolGuid, NULL, (VOID **)&GlobalNvsAreaProtocol);
  if (EFI_ERROR (Status)) {
    //
    // If GlobalNvsAreaProtocol is not ready, use notify to wait for it
    //
    DEBUG ((EFI_D_ERROR, "GlobalNvsAreaProtocol is not ready, wait for it.\n"));
    GlobalNvsArea = NULL;
    EfiNamedEventListen (&gEfiGlobalNvsAreaProtocolGuid, TPL_CALLBACK, GlobalNvsNotify, NULL, NULL);
  } else {
    GlobalNvsArea = GlobalNvsAreaProtocol->Area;
  }
//[-end-130708-IB05400427-modify]//
  ///
  /// Check if TBARB is already initialized by platform code
  ///
  PciD31F6RegBase = MmPciAddress (
                      0,
                      PchPlatformPolicy->BusNumber,
                      PCI_DEVICE_NUMBER_PCH_THERMAL,
                      PCI_FUNCTION_NUMBER_PCH_THERMAL,
                      0
                      );
  PciD31F2RegBase = MmPciAddress (
                      0,
                      PchPlatformPolicy->BusNumber,
                      PCI_DEVICE_NUMBER_PCH_SATA,
                      PCI_FUNCTION_NUMBER_PCH_SATA,
                      0
                      );
  RootComplexBar  = PCH_RCRB_BASE;
  ThermalBaseB    = MmioRead32 (PciD31F6RegBase + R_PCH_THERMAL_TBARB);
  MemBaseAddress  = 0x0ffffffff;

  if (ThermalBaseB & B_PCH_THERMAL_SPTYPEN) {
    ///
    /// Check if TBARB is already initialized and if so use it.
    ///
    ThermalBaseB &= B_PCH_THERMAL_TBARB_MASK;
  } else {
    Status = gDS->AllocateMemorySpace (
                    EfiGcdAllocateMaxAddressSearchBottomUp,
                    EfiGcdMemoryTypeMemoryMappedIo,
                    N_PCH_THREMAL_TBARB_ALIGNMENT,
                    V_PCH_THERMAL_TBARB_SIZE,
                    &MemBaseAddress,
                    mImageHandle,
                    NULL
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    ThermalBaseB = (UINT32) MemBaseAddress;
//[-start-130708-IB05400427-modify]//
    if (GlobalNvsArea != NULL) {
      GlobalNvsArea->TBARB = ThermalBaseB;
    } else {
      mThermalBaseB = ThermalBaseB;
    }
//[-end-130708-IB05400427-modify]//
    MmioWrite32 (PciD31F6RegBase + R_PCH_THERMAL_TBARB, ThermalBaseB);
    MmioWrite32 (PciD31F6RegBase + R_PCH_THERMAL_TBARBH, 0);
    MmioOr32 (PciD31F6RegBase + R_PCH_THERMAL_TBARB, (UINT32) B_PCH_THERMAL_SPTYPEN);

    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (PciD31F6RegBase + R_PCH_THERMAL_TBARB),
      1,
      (VOID *) (UINTN) (PciD31F6RegBase + R_PCH_THERMAL_TBARB)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (PciD31F6RegBase + R_PCH_THERMAL_TBARBH),
      1,
      (VOID *) (UINTN) (PciD31F6RegBase + R_PCH_THERMAL_TBARBH)
      );
  }
  ///
  /// PCH BIOS Spec Rev 0.5.0, 17.3.1 Initializing Lynx Point Thermal Sensors
  /// Step 1
  /// TSC must then be written to 0x81 to enable the power down and lock the register.
  ///
  RegData8 = MmioRead8 (ThermalBaseB + R_PCH_TBARB_TSC);
  ///
  /// Enable Catastrophic Power Down
  ///
  RegData8 |= (UINT8) B_PCH_TBARB_TSC_CPDE;
  ///
  /// Step 8.1
  /// It is recommended that TSC [7] set to 1 to lock the CAT Trip behavior.
  ///
  if (PchPlatformPolicy->ThermalConfig->ThermalAlertEnable.TscLock == PCH_DEVICE_ENABLE) {
    RegData8 |= (UINT8) B_PCH_TBARB_TSC_PLD;
  }

  MmioWrite8 (ThermalBaseB + R_PCH_TBARB_TSC, RegData8);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint8,
    (UINTN) (ThermalBaseB + R_PCH_TBARB_TSC),
    1,
    (VOID *) (UINTN) (ThermalBaseB + R_PCH_TBARB_TSC)
    );
  ///
  /// Step 8.2
  /// TSMIC [7] locks SMI reporting of thermal events
  ///
  if (PchPlatformPolicy->ThermalConfig->ThermalAlertEnable.TsmicLock == PCH_DEVICE_ENABLE) {
    MmioWrite8 (ThermalBaseB + R_PCH_TBARB_TSMIC, 0x80);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint8,
      (UINTN) (ThermalBaseB + R_PCH_TBARB_TSMIC),
      1,
      (VOID *) (UINTN) (ThermalBaseB + R_PCH_TBARB_TSMIC)
      );
  }

  ///
  /// Step 5
  /// If the PCH_Hot pin reporting is supported, then write the temperature value and set the enable in PHL.
  ///
  PchHotEnable = FALSE;
#ifdef TRAD_FLAG
  if (PchSeries == PchH) {
    ///
    /// Note: For PCHHOT# support, we need to make sure if GPIO74 is set to native mode and PCHSTRP9[22] is
    /// set to 1.
    /// Check if GPIO74 is set to native mode.
    ///
    RegData8 = (UINT8)((IoRead32 ((UINTN) (GpioBase + R_PCH_GPIO_USE_SEL3)) & BIT10) >> 10);
  }
#endif // TRAD_FLAG

#ifdef ULT_FLAG
  if (PchSeries == PchLp) {
    ///
    /// Note: For PCHHOT# support, we need to make sure if GPIO73 is set to native mode and PCHSTRP9[22] is
    /// set to 1.
    /// Check if GPIO73 is set to native mode.
    ///
    RegData8 = (UINT8)(IoRead32 ((UINTN) (GpioBase + R_PCH_GP_73_CONFIG0)) & B_PCH_GPIO_OWN0_GPIO_USE_SEL);
  }
#endif // ULT_FLAG

  if(RegData8 == 0x00) {
    ///
    /// Check if PCHSTRP9[22] is set to 1 (PCHHOT# is the native functionality of GPIO74)
    ///
    if ((MmioRead16 (RootComplexBar + R_PCH_SPI_HSFS) & B_PCH_SPI_HSFS_FDV) == B_PCH_SPI_HSFS_FDV) {
      MmioAnd32 ((RootComplexBar + R_PCH_SPI_FDOC), (UINT32) (~(B_PCH_SPI_FDOC_FDSS_MASK | B_PCH_SPI_FDOC_FDSI_MASK)));
      MmioOr32 ((RootComplexBar + R_PCH_SPI_FDOC), (UINT32) (V_PCH_SPI_FDOC_FDSS_PCHS | R_PCH_SPI_STRP9));
      if (MmioRead32 (RootComplexBar + R_PCH_SPI_FDOD) & B_PCH_SPI_STRP9_HOT_SML1_SEL) {
        PchHotEnable = TRUE;
      }
    }
  }

  ///
  /// The value in PHL register is valid only if it is between 00h and 1FFh.
  ///
  if ((PchHotEnable == TRUE) && (PchPlatformPolicy->ThermalConfig->PchHotLevel < 0x0200)) {
    ///
    /// Program PHL register according to PchHotLevel setting.
    ///
    RegData16 = (PchPlatformPolicy->ThermalConfig->PchHotLevel | B_PCH_TBARB_PHLE);
    MmioWrite16 (ThermalBaseB + R_PCH_TBARB_PHL, RegData16);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint16,
      (UINTN) (ThermalBaseB + R_PCH_TBARB_PHL),
      1,
      &RegData16
      );
  }
  ///
  /// Step 8.3
  /// PHLC [0] locks the PHL and PHLC registers for PCH_Hot#
  ///
  if (PchPlatformPolicy->ThermalConfig->ThermalAlertEnable.PhlcLock == PCH_DEVICE_ENABLE) {
    MmioWrite8 (ThermalBaseB + R_PCH_TBARB_PHLC, 0x01);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint8,
      (UINTN) (ThermalBaseB + R_PCH_TBARB_PHLC),
      1,
      (VOID *) (UINTN) (ThermalBaseB + R_PCH_TBARB_PHLC)
      );
  }
  ///
  /// PCH BIOS Spec Rev 0.5.0, Section 17.5 Thermal Throttling
  /// Step 1
  /// Additional programming to initialize Thermal Throttle device
  /// For LPT-H,
  /// a. Program ThermalBAR + 0xC0 to 8000390Bh
  /// b. Program ThermalBAR + 0xC4 to C11F0201h
  /// c. Program ThermalBAR + 0xC8 to 05800000h
  /// d. Program ThermalBAR + 0xCC to 0000C000h
  /// e. Program ThermalBAR + 0xD0 to 00000320h
  /// f. Program ThermalBAR + 0xE0 to 80001E4Fh
  /// g. Program ThermalBAR + 0xF0 to 00000003h
  /// For LPT-LP,
  /// a. Program ThermalBar + 0xC0 to 8000390Bh
  /// b. Program ThermalBar + 0xC4 to C11F0401h
  /// c. Program ThermalBAR + 0xC8 to 05800000h
  /// d. Program ThermalBar + 0xCC to 0000C000h
  /// e. Program ThermalBar + 0xD0 to 00000320h
  /// f. Program ThermalBar + 0xE0 to 80001EDCh
  /// g. Program ThermalBar + 0xF0 to 00000003h
  ///

  if (PchSeries == PchH) {
    MmioWrite32 (ThermalBaseB + 0xC4, 0xC11F0201);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (ThermalBaseB + 0xC4),
      1,
      (VOID *) (UINTN) (ThermalBaseB + 0xC4)
      );

    MmioWrite32 (ThermalBaseB + 0xE0, 0x80001E4F);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (ThermalBaseB + 0xE0),
      1,
      (VOID *) (UINTN) (ThermalBaseB + 0xE0)
      );
  }

  if (PchSeries == PchLp) {
    MmioWrite32 (ThermalBaseB + 0xC4, 0xC11F0401);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (ThermalBaseB + 0xC4),
      1,
      (VOID *) (UINTN) (ThermalBaseB + 0xC4)
      );

    MmioWrite32 (ThermalBaseB + 0xE0, 0x80001EDC);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (ThermalBaseB + 0xE0),
      1,
      (VOID *) (UINTN) (ThermalBaseB + 0xE0)
      );
  }

  MmioWrite32 (ThermalBaseB + 0xC0, 0x8000390B);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (ThermalBaseB + 0xC0),
    1,
    (VOID *) (UINTN) (ThermalBaseB + 0xC0)
    );

  MmioWrite32 (ThermalBaseB + 0xC8, 0x05800000);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (ThermalBaseB + 0xC8),
    1,
    (VOID *) (UINTN) (ThermalBaseB + 0xC8)
    );

  MmioWrite32 (ThermalBaseB + 0xCC, 0x0000C000);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (ThermalBaseB + 0xCC),
    1,
    (VOID *) (UINTN) (ThermalBaseB + 0xCC)
    );

  MmioWrite32 (ThermalBaseB + 0xD0, 0x00000320);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (ThermalBaseB + 0xD0),
    1,
    (VOID *) (UINTN) (ThermalBaseB + 0xD0)
    );

  MmioWrite32 (ThermalBaseB + 0xF0, 0x00000003);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (ThermalBaseB + 0xF0),
    1,
    (VOID *) (UINTN) (ThermalBaseB + 0xF0)
    );
  ///
  ///  HSW BWG 15.8 : Processor PCH-LP cross throttling
  ///
  if (PchSeries == PchLp){
    AsmCpuid (CPUID_VERSION_INFO, &Cpuid.RegEax, &Cpuid.RegEbx, &Cpuid.RegEcx, &Cpuid.RegEdx);
    if (((Cpuid.RegEax & CPUID_FULL_FAMILY_MODEL) == CPUID_FULL_FAMILY_MODEL_HASWELL_ULT) &&
        ((PchPlatformPolicy->ThermalConfig->ThermalThrottling.TTLevels.PchCrossThrottling == PCH_DEVICE_ENABLE) ||
         (PchPlatformPolicy->ThermalConfig->ThermalThrottling.TTLevels.SuggestedSetting == PCH_DEVICE_ENABLE))) {
      //
      // Read MSR 0x1A2 TEMPERATURE_TARGET
      //
      TempMsr.Qword = AsmReadMsr64 (MSR_TEMPERATURE_TARGET);
      ///
      ///  Tcc activation offset in temperature target MSR changes from 4 bits [27:24] to 6 bits [29:24] on ULT C step onwards
      ///  since Tcc will never be more than 205C, thus the calculation for PHL will never overflow
      ///
      if ((Cpuid.RegEax & CPUID_FULL_STEPPING) >= EnumHswUltC0) {
        temperature = (TempMsr.Bytes.ThirdByte - (TempMsr.Bytes.FouthByte & 0x3F));
      } else {
        temperature = (TempMsr.Bytes.ThirdByte - (TempMsr.Bytes.FouthByte & 0xF));
      }

      if (PchPlatformPolicy->ThermalConfig->ThermalThrottling.TTLevels.SuggestedSetting == PCH_DEVICE_ENABLE) {
        ///
        /// PCH T0/T1/T2 Level (MMIO TBARB+40h) :
        /// T0L = (((MSR TEMPERATURE_TARGET[23:16] - TEMPERATURE_TARGET) + 50) * 2)
        /// T1L = T0L + 5C
        /// T2L = T1L + 5C
        ///
        PchTTLevels = (UINT32)((( temperature + 10 + 50) * 2) << 20) |
                      (UINT32)(((temperature + 5 + 50) * 2) << 10)  |
                      (UINT32)((temperature + 50) * 2);
        Data32Or    = BIT31 | BIT29;
      } else {
        PchTTLevels = (UINT32) (((PchPlatformPolicy->ThermalConfig->ThermalThrottling.TTLevels.T2Level + 10 + 50) * 2) << 20) |
                      (UINT32) (((PchPlatformPolicy->ThermalConfig->ThermalThrottling.TTLevels.T1Level + 5 + 50) * 2) << 10) |
                      (UINT32)  ((PchPlatformPolicy->ThermalConfig->ThermalThrottling.TTLevels.T0Level + 50) * 2);
        Data32Or  = (UINT32) (PchPlatformPolicy->ThermalConfig->ThermalThrottling.TTLevels.TTLock << 31) |
                    (UINT32) (PchPlatformPolicy->ThermalConfig->ThermalThrottling.TTLevels.TTState13Enable << 30) |
                    (UINT32) (PchPlatformPolicy->ThermalConfig->ThermalThrottling.TTLevels.TTEnable << 29);
      }
    }
  } else if (PchSeries == PchH) {
    if (PchPlatformPolicy->ThermalConfig->ThermalThrottling.TTLevels.SuggestedSetting == PCH_DEVICE_ENABLE) {
      ///
      /// Set TBARB + 40h = 0B485093Ch
      /// Program TBARB + 40h[31:28] in separate write
      ///
      PchTTLevels = 0xB485093C;
      Data32Or    = BIT31 | BIT29;
    }else {
      ///
      /// PCH BIOS Spec Rev 0.5.5, Section 17.3.1 Initializing Lynx Point Thermal Sensors
      /// Trip Point Temperature = (Trip Point Register [8:0]) / 2 - 50 centigrade degree
      /// If Trip Point Temperature <= T0Level, the system will be in T0 state.
      /// If T1Level >= Trip Point Temperature > T0Level, the system will be in T1 state.
      /// If T2Level >= Trip Point Temperature > T1Level, the system will be in T2 state.
      /// If Trip Point Temperature > T2Level, the system will be in T3 state.
      ///
      PchTTLevels = (UINT32) (((PchPlatformPolicy->ThermalConfig->ThermalThrottling.TTLevels.T2Level + 50) * 2) << 20) |
                    (UINT32) (((PchPlatformPolicy->ThermalConfig->ThermalThrottling.TTLevels.T1Level + 50) * 2) << 10) |
                    (UINT32)  ((PchPlatformPolicy->ThermalConfig->ThermalThrottling.TTLevels.T0Level + 50) * 2);
      Data32Or  = (UINT32) (PchPlatformPolicy->ThermalConfig->ThermalThrottling.TTLevels.TTLock << 31) |
                  (UINT32) (PchPlatformPolicy->ThermalConfig->ThermalThrottling.TTLevels.TTState13Enable << 30) |
                  (UINT32) (PchPlatformPolicy->ThermalConfig->ThermalThrottling.TTLevels.TTEnable << 29);
    }
  }

  ///
  /// Program TBARB + 40h[27:0]
  ///
  MmioWrite32 (ThermalBaseB + R_PCH_TBARB_TL, PchTTLevels);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (ThermalBaseB + R_PCH_TBARB_TL),
    1,
    (VOID *) (UINTN) (ThermalBaseB + R_PCH_TBARB_TL)
    );
  ///
  /// Program TBARB + 40h[31:28]
  ///
  /// PCH BIOS Spec Rev 0.5.0, 17.3.1 Initializing Lynx Point Thermal Sensors
  /// Step 8.4
  /// TL [31] locks the thermal throttling registers
  ///
  MmioOr32 (ThermalBaseB + R_PCH_TBARB_TL, Data32Or);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (ThermalBaseB + R_PCH_TBARB_TL),
    1,
    (VOID *) (UINTN) (ThermalBaseB + R_PCH_TBARB_TL)
    );

  if (PchSeries == PchLp) {
    ///
    ///  Processor PCH-LP cross throttling - Set RCBA MMIO offset 0x33C4[26:24] = 101b
    ///
    AsmCpuid (CPUID_VERSION_INFO, &Cpuid.RegEax, &Cpuid.RegEbx, &Cpuid.RegEcx, &Cpuid.RegEdx);
    if (((Cpuid.RegEax & CPUID_FULL_FAMILY_MODEL) == CPUID_FULL_FAMILY_MODEL_HASWELL_ULT) &&
        ((PchPlatformPolicy->ThermalConfig->ThermalThrottling.TTLevels.PchCrossThrottling == PCH_DEVICE_ENABLE) ||
         (PchPlatformPolicy->ThermalConfig->ThermalThrottling.TTLevels.SuggestedSetting == PCH_DEVICE_ENABLE))) {
      Data32And = (UINT32)~(BIT26 | BIT25 | BIT24);
      Data32Or = BIT26 | BIT24;
      MmioAndThenOr32 ((UINTN) (RootComplexBar + PMSYNC_TPR_CONFIG), Data32And, Data32Or);
      SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (RootComplexBar + PMSYNC_TPR_CONFIG),
        1,
        (VOID *) (UINTN) (RootComplexBar + PMSYNC_TPR_CONFIG)
        );
    }
    ///
    /// Lock PMSYNC_TPR_CFG and PMSYNC_TPR_CFG2
    /// Set RCBA + 0x33C4[31] = 1b.
    ///
    MmioOr32 (RootComplexBar + PMSYNC_TPR_CONFIG,B_PMSYNC_TPR_CONFIG_LOCK);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + PMSYNC_TPR_CONFIG),
      1,
      (VOID *) (UINTN) (RootComplexBar + PMSYNC_TPR_CONFIG)
      );
  }

  if (PchSeries == PchH) {
    ///
    /// PCH BIOS Spec Rev 0.5.0, Section 17.5 Thermal Throttling
    /// Step 3
    /// Set Chipset Initialization Register 30 (RCBA + 2238h) = 00000941h
    ///
    if (PchPlatformPolicy->ThermalConfig->ThermalThrottling.DmiHaAWC.SuggestedSetting == PCH_DEVICE_ENABLE) {
      RegData32  = 0x00000941;
    } else {
      RegData32  = (UINT32) (PchPlatformPolicy->ThermalConfig->ThermalThrottling.DmiHaAWC.TS3TW << 10) |
                   (UINT32) (PchPlatformPolicy->ThermalConfig->ThermalThrottling.DmiHaAWC.TS2TW << 8) |
                   (UINT32) (PchPlatformPolicy->ThermalConfig->ThermalThrottling.DmiHaAWC.TS1TW << 6) |
                   (UINT32) (PchPlatformPolicy->ThermalConfig->ThermalThrottling.DmiHaAWC.TS0TW << 4) |
                   (UINT32)  PchPlatformPolicy->ThermalConfig->ThermalThrottling.DmiHaAWC.DmiTsawEn;
    }
    ///
    /// If DMI IOT is enabled, set chipset Initialization Register 30 (RCBA + 2238h) = 00000551h
    ///
    if (PchPlatformPolicy->DmiConfig->DmiIot == PCH_DEVICE_ENABLE) {
      RegData32 = 0x00000551;
    }
    MmioWrite32 ((UINTN) (RootComplexBar + R_PCH_RCRB_CIR2238), RegData32);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + R_PCH_RCRB_CIR2238),
      1,
      (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_CIR2238)
      );
  }

  ///
  /// Step 4
  /// Program SATA Indexed Register Index and Data:
  /// a. If port 0 is empty, set D31:F2:A0h=A0h and D31:F2:A4h[15:00] = 0000h
  ///    else if Port 0 has a HDD, set D31:F2:A4h[15:00] = 0039h
  ///    else if Port 0 has a SSD, set D31:F2:A4h[15:00] = 0F39h
  /// b. If port 1 is empty, set D31:F2:A0h=A0h and D31:F2:A4h[31:16] = 0000h
  ///    else if Port 1 has a HDD, set D31:F2:A4h[31:16] = 0039h
  ///    else if Port 1 has a SSD, set D31:F2:A4h[31:16] = 0F39h
  /// c. If port 2 is empty, set D31:F2:A0h=A4h and D31:F2:A4h[15:00] = 0000h
  ///    else if Port 2 has a HDD, set D31:F2:A4h[15:00] = 0039h
  ///    else if Port 2 has a SSD, set D31:F2:A4h[15:00] = 0F39h
  /// d. If port 3 is empty, set D31:F2:A0h=A4h and D31:F2:A4h[31:16] = 0000h
  ///    else if Port 3 has a HDD, set D31:F2:A4h[31:16] = 0039h
  ///    else if Port 3 has a SSD, set D31:F2:A4h[31:16] = 0F39h
  /// e. If port 4 is empty, set D31:F2:A0h=A8h and D31:F2:A4h[15:00] = 0000h
  ///    else if Port 4 has a HDD, set D31:F2:A4h[15:00] = 0039h
  ///    else if Port 4 has a SSD, set D31:F2:A4h[15:00] = 0F39h
  /// f. If port 5 is empty, set D31:F2:A0h=A8h and D31:F2:A4h[31:16] = 0000h
  ///    else if Port 5 has a HDD, set D31:F2:A4h[31:16] = 0039h
  ///    else if Port 5 has a SSD, set D31:F2:A4h[31:16] = 0F39h
  ///
  MaxSataPortNum = GetPchMaxSataPortNum();
  RegData16 = MmioRead16 (PciD31F2RegBase + R_PCH_SATA_PCS);
  for (Index = 0; Index < (MaxSataPortNum / 2); Index++) {
    Data32And = 0x70C070C0;
    Data32Or  = 0x00000000;
    if (PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.SuggestedSetting == PCH_DEVICE_ENABLE) {
      if ((RegData16 & (B_PCH_SATA_PCS_PORT0_DET << (Index * 2))) != 0) {
        Data32Or |= 0x00000039;
        if (PchPlatformPolicy->SataConfig->PortSettings[0 + (Index * 2)].SolidStateDrive == PCH_DEVICE_ENABLE) {
          Data32Or |= 0x00000F00;
        }
      }

      if ((RegData16 & (B_PCH_SATA_PCS_PORT0_DET << ((Index * 2) + 1))) != 0) {
        Data32Or |= 0x00390000;
        if (PchPlatformPolicy->SataConfig->PortSettings[1 + (Index * 2)].SolidStateDrive == PCH_DEVICE_ENABLE) {
          Data32Or |= 0x0F000000;
        }
      }
    } else {
      Data32Or  = (UINT32) (PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P1TDispFinit << 31) |
                  (UINT32) (PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P1Tinact << 26) |
                  (UINT32) (PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P1TDisp << 24) |
                  (UINT32) (PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P1T3M << 20) |
                  (UINT32) (PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P1T2M << 18) |
                  (UINT32) (PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P1T1M << 16) |
                  (UINT32) (PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P0TDispFinit << 15) |
                  (UINT32) (PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P0Tinact << 10) |
                  (UINT32) (PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P0TDisp << 8) |
                  (UINT32) (PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P0T3M << 4) |
                  (UINT32) (PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P0T2M << 2) |
                  (UINT32)  PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P0T1M;
    }

    MmioWrite8 (PciD31F2RegBase + R_PCH_SATA_SIRI, (0xA0 + (Index * 4)));
    MmioAndThenOr32 ((UINTN) (PciD31F2RegBase + R_PCH_SATA_STRD), Data32And, Data32Or);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint8,
      (UINTN) (PciD31F2RegBase + R_PCH_SATA_SIRI),
      1,
      (VOID *) (UINTN) (PciD31F2RegBase + R_PCH_SATA_SIRI)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (PciD31F2RegBase + R_PCH_SATA_STRD),
      1,
      (VOID *) (UINTN) (PciD31F2RegBase + R_PCH_SATA_STRD)
      );
  }

  ///
  /// Program ThermalBar + 0xA4 [1:0] = 11b
  ///
  MmioOr8 (ThermalBaseB + 0xA4, (UINT8) (BIT1 | BIT0));
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint8,
    (UINTN) (ThermalBaseB + 0xA4),
    1,
    (VOID *) (UINTN) (ThermalBaseB + 0xA4)
    );
  ///
  /// PCH BIOS Spec Rev 0.5.0, 17.3.1 Initializing Lynx Point Thermal Sensors
  /// Step 7
  /// Enable thermal sensor by programming TSEL register to 01h
  /// This should be done after all thermal initialization steps are finished.
  ///
  RegData8 = MmioRead8 (ThermalBaseB + R_PCH_TBARB_TSEL);
  RegData8 |= (UINT8) B_PCH_TBARB_TSEL_ETS;
  ///
  /// Step 8.5
  /// TSEL [7] locks the thermal sensor enable, after TAHV and TAHL are programmed by BIOS or driver
  /// later in case.
  ///
  if (PchPlatformPolicy->ThermalConfig->ThermalAlertEnable.TselLock == PCH_DEVICE_ENABLE) {
    RegData8 |= (UINT8) B_PCH_TBARB_TSEL_PLD;
  }

  MmioWrite8 (ThermalBaseB + R_PCH_TBARB_TSEL, RegData8);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint8,
    (UINTN) (ThermalBaseB + R_PCH_TBARB_TSEL),
    1,
    (VOID *) (UINTN) (ThermalBaseB + R_PCH_TBARB_TSEL)
    );
  ///
  /// Step 8.6
  /// Program ThermalBAR + 0x0A [7] = 1b
  /// For LP, Program ThermalBar + 0x0A [7, 0] = 1b, 1b
  ///
  RegData8 = BIT7;
  if (PchSeries == PchLp) {
    RegData8 |= BIT0;
  }
  MmioOr8 (ThermalBaseB + R_PCH_TBARB_TSREL, RegData8);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint8,
    (UINTN) (ThermalBaseB + R_PCH_TBARB_TSREL),
    1,
    (VOID *) (UINTN) (ThermalBaseB + R_PCH_TBARB_TSREL)
    );

  ///
  /// For LP, Program ThermalBar + 0x1C [14:0] = 48C8h
  /// For LP, Program ThermalBar + 0x1C [15] = 1h
  ///
  if (PchSeries == PchLp) {
    Data16And = (B_PCH_TBARB_TSPM_LTT | B_PCH_TBARB_TSPM_MAXTSST | B_PCH_TBARB_TSPM_MINTSST | B_PCH_TBARB_TSPM_DTSSIC0 |
                 B_PCH_TBARB_TSPM_DTSSS0EN);
    Data16Or = (V_PCH_TBARB_TSPM_LTT | V_PCH_TBARB_TSPM_MAXTSST | B_PCH_TBARB_TSPM_DTSSS0EN);
    MmioAndThenOr16 (ThermalBaseB + R_PCH_TBARB_TSPM,
                     (UINT16) Data16And,
                     (UINT16) Data16Or);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint16,
      (UINTN) (ThermalBaseB + R_PCH_TBARB_TSPM),
      1,
      (VOID *) (UINTN) (ThermalBaseB + R_PCH_TBARB_TSPM)
      );

    MmioOr16 (ThermalBaseB + R_PCH_TBARB_TSPM, (UINT16) B_PCH_TBARB_TSPM_TSPMLOCK);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint16,
      (UINTN) (ThermalBaseB + R_PCH_TBARB_TSPM),
      1,
      (VOID *) (UINTN) (ThermalBaseB + R_PCH_TBARB_TSPM)
      );
  }

  ///
  /// PCH BIOS Spec Rev 0.5.0, Section 17.2, Thermal Subsystem Device Initialization
  /// Step 5
  /// BIOS needs to perform the following steps prior to end of POST to free up the PCI resources
  /// and hide the thermal subsystem device(OPTIONAL), except on mobile platforms that support
  /// Intel DPPM. Also, BIOS may keep the TBARBH programmed if BIOS needs runtime access to PCH
  /// thermal subsystem device data. In that case, BIOS must ensure TBARBH memory is reserved and
  /// reported to the OS as motherboard resources to avoid memory allocation conflicts.
  ///
  if (PchPlatformPolicy->ThermalConfig->ThermalDeviceEnable == FALSE) {
    ///
    /// Step 5.1
    /// Clear the Memory and Bus Master enable bit of D31:F6
    ///
    MmioAnd16 (
      PciD31F6RegBase + R_PCH_THERMAL_COMMAND,
      (UINT16)~(B_PCH_THERMAL_COMMAND_MSE | B_PCH_THERMAL_COMMAND_BME)
      );
    ///
    /// Step 5.2
    /// Clear and release memory resource assigned in TBAR (D31:F6:10h-13h)
    ///
    ThermalBase = MmioRead32 (PciD31F6RegBase + R_PCH_THERMAL_TBAR) & B_PCH_THERMAL_TBAR_MASK;

    if ((ThermalBase != 0) && (ThermalBase != B_PCH_THERMAL_TBAR_MASK)) {
      MmioWrite32 (PciD31F6RegBase + R_PCH_THERMAL_TBAR, 0);
      MmioWrite32 (PciD31F6RegBase + R_PCH_THERMAL_TBARH, 0);

      SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (PciD31F6RegBase + R_PCH_THERMAL_TBAR),
        1,
        (VOID *) (UINTN) (PciD31F6RegBase + R_PCH_THERMAL_TBAR)
        );

      SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (PciD31F6RegBase + R_PCH_THERMAL_TBARH),
        1,
        (VOID *) (UINTN) (PciD31F6RegBase + R_PCH_THERMAL_TBARH)
        );

      gDS->FreeMemorySpace (
            (EFI_PHYSICAL_ADDRESS) ThermalBase,
            V_PCH_THERMAL_TBAR_SIZE
            );
    }
    ///
    /// Step 5.3
    /// Optionally, release and clear memory resource assigned in TBARB (D31:F6:40h-48h) if BIOS/ASL
    /// implementation does not require access to PCH thermal subsystem device data during run time.
    /// Left this to platform code
    ///
    /// Step 5.4
    /// Hide D31:F6 PCI configuration space by setting FD.TTD (RCBA + 3418h[24])
    ///
    Data32And = 0xFFFFFFFF;
    Data32Or  = (UINT32) B_PCH_RCRB_FUNC_DIS_THERMAL;
    MmioOr32 (RootComplexBar + R_PCH_RCRB_FUNC_DIS, Data32Or);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + R_PCH_RCRB_FUNC_DIS),
      &Data32Or,  // Data to be ORed
      &Data32And  // Data to be ANDed
      );
    ///
    /// Reads back for posted write to take effect
    ///
    Data32Or = MmioRead32 ((UINTN) (RootComplexBar + R_PCH_RCRB_FUNC_DIS));
    SCRIPT_MEM_POLL (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + R_PCH_RCRB_FUNC_DIS),
      &Data32Or,  // BitMask
      &Data32Or,  // BitValue
      1,          // Duration
      1           // LoopTimes
      );
  }

  DEBUG ((EFI_D_INFO, "ThermalLockDown() End\n"));

  return EFI_SUCCESS;
}

/**
  Perform Clock Gating programming
  Enables clock gating in various PCH interfaces and the registers must be restored during S3 resume.

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] RootComplexBar       RootComplexBar value of this PCH device
  @param[in] FuncDisableReg       The Function Disable Register

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
ConfigureClockGating (
  IN  DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy,
  IN  UINT32                              RootComplexBar,
  IN  UINT32                              FuncDisableReg
  )
{
  UINT8                 RegData8;
  UINT32                RegData32;
  UINT32                RegDataOr32;
  UINT32                RegDataAnd32;
  UINTN                 PciD31F0RegBase;
  UINT16                LpcDeviceId;
  UINT16                Data16Or;
  PCH_SERIES            PchSeries;
  UINT32                D2F0Base;

  DEBUG ((EFI_D_INFO, "ConfigureClockGating() Start\n"));

  PchSeries = GetPchSeries();
  PciD31F0RegBase = MmPciAddress (0, PchPlatformPolicy->BusNumber, 31, 0, 0);
  LpcDeviceId     = MmioRead16 (PciD31F0RegBase + R_PCH_LPC_DEVICE_ID);

  ///
  /// PCH BIOS Spec Rev 0.5.5, section 19.10
  /// 1
  /// DMI interface
  /// Enable Dynamic Clock Gating in the DMIC register by programming
  /// For PCH LP
  /// RCBA + 2234h [2] to 1b, RCBA + 2234h [0] to 1b
  /// RCBA + 2234h [3:0] to 1111b
  /// Set D31:F0:A0h[5] = 1b
  /// Set D31:F0:A0h[6] = 1b
  /// Set D31:F0:A0h[7] = 1b
  /// Set D31:F0:A0h[11] = 0b
  /// Set D31:F0:A0h[12] = 1b
  /// For PCH H
  /// RCBA + 2234h [3:0] to 1111b
  /// Enable Dynamic Clock Gating in the DMIC register by programming RCBA + 2234h [3:0] to 1111b
  /// before enabling ASPM.
  /// Set D31:F0:A0h[11] = 1b
  /// Set D31:F0:A0h[12] = 1b
  /// Set D31:F0:A0h[14] = 1b.
  /// System BIOS is also required to set following bit.
  /// Dekstop: "Pseudo CLKRUN_EN (PSEUDO_CLKRUN_EN)" bit (D31:F0:A0h[3]) = 1b
  /// Mobile: "PCI CLKRUN# Enable" bit (D31:F0:A0h[2]) = 1b
  ///
  switch (PchSeries) {
    case PchLp:
      MmioOr8 ((UINTN) (RootComplexBar + R_PCH_RCRB_DMIC), (UINT8) (B_PCH_LP_RCRB_DMIC_DMICGEN));
      break;

    case PchH:
      MmioOr8 ((UINTN) (RootComplexBar + R_PCH_RCRB_DMIC), (UINT8) (B_PCH_H_RCRB_DMIC_DMICGEN));
      break;

    default:
      break;
  }

  if (PchSeries == PchLp) {
    Data16Or = (UINT16)(BIT12 | BIT7 | BIT6 | BIT5);
    Data16Or |= BIT11;
    MmioOr16 ((UINTN) (PciD31F0RegBase + R_PCH_LPC_GEN_PMCON_1), Data16Or);
  } else if (PchSeries == PchH) {
    MmioOr16 ((UINTN) (PciD31F0RegBase + R_PCH_LPC_GEN_PMCON_1), (UINT16) (BIT14 | BIT12 | BIT11));
  }

  if (PchPlatformPolicy->DeviceEnabling->PciClockRun == PCH_DEVICE_DISABLE) {
    if (IS_PCH_LPT_LPC_DEVICE_ID_MOBILE (LpcDeviceId)) {
      MmioAnd16 ((UINTN) (PciD31F0RegBase + R_PCH_LPC_GEN_PMCON_1), (UINT16) (~B_PCH_LPC_GEN_PMCON_CLKRUN_EN));
    } else {
      MmioAnd16 ((UINTN) (PciD31F0RegBase + R_PCH_LPC_GEN_PMCON_1), (UINT16) (~B_PCH_LPC_GEN_PMCON_PSEUDO_CLKRUN_EN));
    }
  } else {
    if (IS_PCH_LPT_LPC_DEVICE_ID_MOBILE (LpcDeviceId)) {
      MmioOr16 ((UINTN) (PciD31F0RegBase + R_PCH_LPC_GEN_PMCON_1), (UINT16) (B_PCH_LPC_GEN_PMCON_CLKRUN_EN));
    } else {
      MmioOr16 ((UINTN) (PciD31F0RegBase + R_PCH_LPC_GEN_PMCON_1), (UINT16) (B_PCH_LPC_GEN_PMCON_PSEUDO_CLKRUN_EN));
    }
  }

  RegData8 = MmioRead8 (RootComplexBar + R_PCH_RCRB_DMIC);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint8,
    (UINTN) (RootComplexBar + R_PCH_RCRB_DMIC),
    1,
    &RegData8
    );

  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint16,
    (UINTN) (PciD31F0RegBase + R_PCH_LPC_GEN_PMCON_1),
    1,
    (VOID *) (UINTN) (PciD31F0RegBase + R_PCH_LPC_GEN_PMCON_1)
    );

  if (PchSeries == PchLp) {
    ///
    /// Set RCBA + 2614h[27:25],[14:13],[10],[8] = 101b, 11b, 1b, 1b respectively
    /// Set RCBA + 2614h[23:16] = 0x20
    /// Set RCBA + 2614h[30:28] = 0b
    /// Set RCBA + 2614h[26] = 1b if D2F0+8 >= 0x0B
    ///
    D2F0Base = (UINT32) MmPciAddress (0, 0, 2, 0, 0);
    RegDataAnd32 = (UINT32) ~(BIT30 | BIT29 | BIT28 | BIT26 | 0x00FF0000);
    RegDataOr32 = (UINT32) (BIT27 | BIT25 | BIT21 | BIT14 | BIT13 | BIT10 | BIT8);
    if ((MmioRead16 (D2F0Base) != 0xFFFF) && (MmioRead8 (D2F0Base + 8) >= 0x0B)) {
       RegDataOr32 |= (UINT32) BIT26;
    }
    MmioAndThenOr32 (
      (UINTN) (RootComplexBar + 0x2614),
      RegDataAnd32,
      RegDataOr32
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + 0x2614),
      1,
      (VOID *) (UINTN) (RootComplexBar + 0x2614)
      );
    ///
    /// Set Chipset Initialization Register 2 [4:0] (RCBA + 900h) = 11111b
    /// Set Chipset Initialization Register 2 [9:8] (RCBA + 900h) = 11b
    ///
    MmioOr32 (
      (UINTN) (RootComplexBar + R_PCH_RCRB_CIR0900),
      (UINT32) (BIT9 | BIT8 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + R_PCH_RCRB_CIR0900),
      1,
      (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_CIR0900)
      );
  } else if (PchSeries == PchH) {
    ///
    /// Set Chipset Initialization Register 2 [14] (RCBA + 900h) = 1b
    ///
    MmioOr32 (
      (UINTN) (RootComplexBar + R_PCH_RCRB_CIR0900),
      (UINT32) (BIT14)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + R_PCH_RCRB_CIR0900),
      1,
      (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_CIR0900)
      );
  }
  ///
  /// 2
  /// PCI Express* interface
  /// 2.1
  /// For each enabled PCI Express* root port, program D28:F0~F7:Reg E1h[1:0] to 3h to enable dynamic clock gating.
  /// System BIOS also require to set D28:F0~F7:Reg E8h[0] = 1b
  /// 2.2
  /// Additionally, if port 0 is in x2 mode, these bits should not be set for port 1.
  /// Likewise, if port 0 is in x4 mode, these bits should not be set for ports 1, 2, or 3
  /// Done in PchRootPorts.c PcieEnableClockGating
  /// 2.2.1
  /// If PCIe root ports 0-3 are all disabled, set B0:D28:F0 + E2h[0] = 1b
  /// if PCIe root ports 4-7 are all disabled, set B0:D28:F4 + E2h[0] = 1b
  /// 2.3
  /// Set B0:D28:F0&F4 + E1h [5:2] = 1111b
  /// 2.4
  /// Set B0:D28:F0&F4:E1h[7] = 1b
  /// 2.6
  /// Set B0:D28:F0~F7 + 324h[5] = 1b
  /// Done in PchRootPorts.c PcieEnableClockGating
  ///
  /// Reg RCBA+341C is modified at multiple places, save at the end of the function
  ///
  /// 3
  /// Serial ATA*
  /// - Set bits D31:F2:94h[29:24] to 3Fh as part of the chipset initialization before disabling
  ///   the SATA function when the SATA interface is not supported on the platform. BIOS can also
  ///   set PCD bits to disable clocks for the un-routed ports on the platform.
  /// - After configuring Port and Control Status (PCS) Register Port x Enabled (PxE) bits accordingly,
  ///   wait 1.4 micro second, then the PCD bits (D31:F2:Reg 94h[29:24]) should be set to be the inverse
  ///   of the Port and Control Status (PCS) Register Port x Enabled (PxE) bits
  ///   Please note that PCS should be set and PCD should not be set when ports are enabled for hot
  ///   plug support or used for SATA testing in test mode.
  ///   Done in ConfigureSata();
  /// - Program D31:F2:98h[29] to 1b
  ///   Done in PchInitBeforeBoot()
  /// - Set SATA Initialization Register 70h[31:0] = 3F00BF1Fh (Done in ConfigureMiscPm)
  ///   Set SATA Initialization Register 54h[31:0] = CF000F0Fh (Done in ConfigureMiscPm)
  ///   Set SATA Initialization Register 58h[31:0] = 190000h (Done in ConfigureMiscPm)
  ///
  /// 4
  /// USB 1.1 / USB 2.0 / USB 3.0
  ///
  /// ConfigureUsbClockGating() has been moved to ConfigureMiscPm() to run before Function Disable
  ///
  /// 5
  /// Intel High Definition Audio (HDA) controller.
  ///
  if (FuncDisableReg & B_PCH_RCRB_FUNC_DIS_AZALIA) {
    ///
    /// 5.1
    /// If the HD Audio Controller is not being used, D27:F0 can be disabled and statically gated. Only statically
    /// gate the Intel High Definition Audio controller if it is not being used in the system by setting RCBA + 341Ch[21].
    ///
    MmioOr32 ((UINTN) (RootComplexBar + R_PCH_RCRB_CG), (UINT32) (B_PCH_RCRB_CG_EN_SCG_HDA));
  } else {
    ///
    /// 5.2
    /// When the Intel High Definition Audio controller is used in the system,
    /// dynamic clock gating can be used by setting RCBA + 341Ch[22].
    ///
    MmioOr32 ((UINTN) (RootComplexBar + R_PCH_RCRB_CG), (UINT32) (B_PCH_RCRB_CG_EN_DCG_HDA));
  }
  if (PchSeries == PchLp) {
    ///
    /// For PchLp, set RCBA + 341Ch[22]
    ///
    MmioOr32 ((UINTN) (RootComplexBar + R_PCH_RCRB_CG), (UINT32) (B_PCH_RCRB_CG_EN_DCG_HDA));
    ///
    /// 5.3
    /// Set D27:F0:43h[6:5][2:0] = 11b, 111b (Done in ConfigureMiscPm)
    ///
  }
  ///
  /// Reg RCBA+341C is modified at multiple places, save at the end of the function
  ///
  /// 7
  /// LPC.
  /// Enable dynamic clock gating by setting RCRB+341C[31] to 1b.
  ///
  MmioOr32 ((UINTN) (RootComplexBar + R_PCH_RCRB_CG), (UINT32) (B_PCH_RCRB_CG_EN_DCG_LPC));
  if (PchSeries == PchH) {
    ///
    /// Reg RCBA+341C is modified at multiple places, save at the end of the function
    ///
    /// 8
    /// PCI Interface.
    /// Enable PCI dynamic clock gating by setting RCBA + 341Ch[16].
    ///
    MmioOr32 ((UINTN) (RootComplexBar + R_PCH_RCRB_CG), (UINT32) (B_PCH_RCRB_CG_EN_DCG_PCI));
  } else if (PchSeries == PchLp) {
    ///
    /// Set RCRB+341Ch[30][28:26] to 1b, 111b
    ///
    MmioOr32 (
      (UINTN) (RootComplexBar + R_PCH_RCRB_CG),
      (UINT32) (B_PCH_RCRB_CG_EN_DCG_BLA | B_PCH_RCRB_CG_EN_DCG_GPIO | B_PCH_RCRB_CG_EN_DCG_HPET |
                B_PCH_RCRB_CG_EN_CG_GPEC));
    ///
    /// Set RCRB+3434h[2:0] to 111b
    ///
    MmioOr8 (
      (UINTN) (RootComplexBar + 0x3434),
      (UINT8) (BIT2 | BIT1 | BIT0)
      );
    RegData8 = MmioRead8 (RootComplexBar + 0x3434);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint8,
      (UINTN) (RootComplexBar + 0x3434),
      1,
      &RegData8
      );
    ///
    /// If RCRB+3454h[4] is 0b, then set RCRB+341C[29] to 1b, else set RCRB+341C[29] to 0b
    ///
    if ((MmioRead32 (RootComplexBar + R_PCH_RCRB_GSX_CTRL) & B_PCH_RCRB_GSX_BAR_ENABLE) == 0) {
      MmioOr32 ((UINTN) (RootComplexBar + R_PCH_RCRB_CG), (UINT32) (B_PCH_RCRB_CG_EN_SCG_GSX));
    } else {
      MmioAnd32 ((UINTN) (RootComplexBar + R_PCH_RCRB_CG), (UINT32) (~B_PCH_RCRB_CG_EN_SCG_GSX));
    }
  }
  ///
  /// PCH BIOS Spec Rev 0.5.0, Section 10.2.1/10.2.2 Enable/Disable the GbE Clock Gating
  /// Set RCBA + 341Ch[23]
  ///
  if (PchPlatformPolicy->DeviceEnabling->Lan == PCH_DEVICE_ENABLE) {
    MmioAnd32 ((UINTN) (RootComplexBar + R_PCH_RCRB_CG), (UINT32) (~B_PCH_RCRB_CG_EN_SCG_LAN));
  } else {
    MmioOr32 ((UINTN) (RootComplexBar + R_PCH_RCRB_CG), (UINT32) (B_PCH_RCRB_CG_EN_SCG_LAN));
  }
  if (PchSeries == PchLp) {
    ///
    /// 9
    /// SPI Clock gating.
    /// Enable SPI clock gating by programming RCBA + 38C0h [13:10][2:0] to 1111b, 111b
    ///
    MmioOr32 (
      (UINTN) (RootComplexBar + R_PCH_SPI_AFC),
      (UINT32) (BIT13 | BIT12 | BIT11 | BIT10 | B_PCH_SPI_AFC_INF_DCGE | B_PCH_SPI_AFC_CORE_DCGE)
      );
    RegData32 = MmioRead32 (RootComplexBar + R_PCH_SPI_AFC);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + R_PCH_SPI_AFC),
      1,
      &RegData32
      );
  } else if (PchSeries == PchH) {
    ///
    /// 9
    /// SPI Clock gating.
    /// Enable SPI clock gating by programming RCBA + 38C0h [2:0] to 111b
    ///
    MmioOr32 (
      (UINTN) (RootComplexBar + R_PCH_SPI_AFC),
      (UINT32) (B_PCH_SPI_AFC_INF_DCGE | B_PCH_SPI_AFC_CORE_DCGE)
      );
    RegData32 = MmioRead32 (RootComplexBar + R_PCH_SPI_AFC);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + R_PCH_SPI_AFC),
      1,
      &RegData32
      );
  }
  ///
  /// 10
  /// SMBus
  /// Enable SMBus dynamic clock gating by setting D31:F3:80h [8, 10, 12 and 14] = 0b respectively  (Done in ConfigureMiscPm)
  ///
  ///
  /// PCH BIOS Spec Rev 0.5.5, section 19.10
  /// 11
  /// Misc
  /// Set D31:F2:300h [17:16] = 11b (Done in ConfigureMiscPm)
  ///

  if (PchSeries == PchLp) {
    ///
    /// Set D31:F2:98h [31:30], [23] to 00b, 1b (Done in ConfigureMiscPm)
    ///
    /// Set iobp register CE00C000h[0] to 0b
    ///
    ProgramIobpWithScript (
      RootComplexBar,
      0xCE00C000,
      (UINT32)~(BIT0),
      0
      );

    ///
    /// Disable legacy DMA (8237) if desired
    /// Set RCBA + Offset 0x341C[24] = 1
    ///
    if ((PchPlatformPolicy->Revision >= DXE_PCH_PLATFORM_POLICY_PROTOCOL_REVISION_4)
       && PchPlatformPolicy->PwrOptConfig->LegacyDmaDisable)
    {
      MmioOr32 ((UINTN) (RootComplexBar + R_PCH_RCRB_CG), (UINT32) (B_PCH_RCRB_CG_EN_SCG_8237));
    }
  }
  ///
  /// Save 341C value to the S3 script table. This register is modified at multiple places in this function. So instead of saving
  /// at each location read the value once at the end of the function and save in S3 resume script.
  ///
  RegData32 = MmioRead32 (RootComplexBar + R_PCH_RCRB_CG);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (RootComplexBar + R_PCH_RCRB_CG),
    1,
    &RegData32
    );

  DEBUG ((EFI_D_INFO, "ConfigureClockGating() End\n"));

  return EFI_SUCCESS;
}

/**
  Configure miscellaneous power management settings

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] RootComplexBar       RootComplexBar value of this PCH device
  @param[in] GpioBase             GPIO base address of this PCH device

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
ConfigureMiscPm (
  IN  DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy,
  IN  UINT32                              RootComplexBar,
  IN  UINT16                              GpioBase
  )
{
  UINT8       Data8Or;
  UINT8       Data8And;
  UINT32      RegData32;
  UINT16      RegData16;
  UINT32      RegData32Tmp;
  UINTN       PciD31F0RegBase;
  UINTN       PciD31F3RegBase;
  UINTN       PciD31F2RegBase;
  UINT16      LpcDeviceId;
  UINTN       PciD27F0RegBase;
  PCH_SERIES  PchSeries;
  UINT32      DsxCfg;

  DEBUG ((EFI_D_INFO, "ConfigureMiscPm() Start\n"));

  PchSeries = GetPchSeries();
  PciD31F0RegBase = MmPciAddress (0, PchPlatformPolicy->BusNumber, 31, 0, 0);
  PciD31F3RegBase = MmPciAddress (0, PchPlatformPolicy->BusNumber, 31, 3, 0);
  PciD31F2RegBase = MmPciAddress (0, PchPlatformPolicy->BusNumber, 31, 2, 0);
  LpcDeviceId     = MmioRead16 (PciD31F0RegBase + R_PCH_LPC_DEVICE_ID);
  PciD27F0RegBase = 0;
  if (PchSeries == PchLp) {
    PciD27F0RegBase = MmPciAddress (0, PchPlatformPolicy->BusNumber, 27, 0, 0);
  }

  ///
  /// Clear power / reset status bits on PCH Corporate
  ///
  RegData32 = 0;
  if (PchPlatformPolicy->MiscPmConfig->PowerResetStatusClear.MeWakeSts) {
    RegData32 |= B_PCH_RCRB_PRSTS_ME_WAKE_STS;
  }

  if (PchPlatformPolicy->MiscPmConfig->PowerResetStatusClear.MeHrstColdSts) {
    RegData32 |= B_PCH_RCRB_PRSTS_ME_HRST_COLD_STS;
  }

  if (PchPlatformPolicy->MiscPmConfig->PowerResetStatusClear.MeHrstWarmSts) {
    RegData32 |= B_PCH_RCRB_PRSTS_ME_HRST_WARM_STS;
  }

  if (PchPlatformPolicy->MiscPmConfig->PowerResetStatusClear.MeHostPowerDn) {
    RegData32 |= B_PCH_RCRB_PRSTS_ME_HOST_PWRDN;
  }

  if (PchPlatformPolicy->MiscPmConfig->PowerResetStatusClear.WolOvrWkSts) {
    RegData32 |= B_PCH_RCRB_PRSTS_WOL_OVR_WK_STS;
  }

  MmioOr32 (RootComplexBar + R_PCH_RCRB_PRSTS, RegData32);
  RegData32Tmp = 0xFFFFFFFF;
  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    RootComplexBar + R_PCH_RCRB_PRSTS,
    &RegData32,   // OR mask
    &RegData32Tmp // AND mask
    );

  ///
  /// We need to enable GP27_PIN_DSX_EN for Wake from both SX and DSX
  ///
  DsxCfg = MmioRead32(RootComplexBar + 0x3334);
  if (PchPlatformPolicy->MiscPmConfig->WakeConfig.Gp27WakeFromDeepSx == PCH_DEVICE_ENABLE) {
    DsxCfg |= BIT0;
  } else {
    DsxCfg &= ~BIT0;
  }

  ///
  /// Enable WAKE_PIN__DSX_EN for Wake
  ///
  if(PchPlatformPolicy->Revision >= DXE_PCH_PLATFORM_POLICY_PROTOCOL_REVISION_5) {
    if (PchPlatformPolicy->MiscPmConfig->WakeConfig.PcieWakeFromDeepSx == PCH_DEVICE_ENABLE) {
      DsxCfg |= BIT2;
    } else {
      DsxCfg &= ~BIT2;
    }
  }
  MmioWrite32 ((RootComplexBar + 0x3334), DsxCfg);

  ///
  /// Handle wake policy
  /// Don't need to record in S3 script as R_PCH_LPC_GEN_PMCON_3 is in RTC and SUS power well
  ///
  RegData16 = MmioRead16 (PciD31F0RegBase + R_PCH_LPC_GEN_PMCON_3) &
                (UINT16) (~(B_PCH_LPC_GEN_PMCON_PME_B0_S5_DIS +
                            B_PCH_LPC_GEN_PMCON_WOL_ENABLE_OVERRIDE));

  if (PchPlatformPolicy->MiscPmConfig->WakeConfig.PmeB0S5Dis) {
    RegData16 |= B_PCH_LPC_GEN_PMCON_PME_B0_S5_DIS;
    MmioWrite16 (PciD31F0RegBase + R_PCH_LPC_GEN_PMCON_3, RegData16);
  }

  if (PchPlatformPolicy->MiscPmConfig->WakeConfig.WolEnableOverride) {
    ///
    /// PCH BIOS Spec Rev 0.5.0 Section 10.4 Wake-On-LAN (WOL) Implementation
    /// Step 1
    /// Clear D31:F0:A2h[14] = 0b to ensure the LAN PHY will be powered for WOL
    /// when the power source is either the AC or the DC battery.
    ///
    RegData16 = MmioRead16 (PciD31F0RegBase + R_PCH_LPC_GEN_PMCON_2);
    RegData16 &= (UINT16) ~B_PCH_LPC_GEN_PMCON_DC_PP_DIS;

    ///
    /// Step 2
    /// Clear D31:F0:A2h[13] = 0b to ensure the LAN PHY will be powered for WOL in Deep Sx.
    ///
    RegData16 &= (UINT16) ~B_PCH_LPC_GEN_PMCON_DSX_PP_DIS;

    ///
    /// Step 3
    /// Set D31:F0:A2h[12] = 1b to ensure the LAN PHY will be powered for WOL after a G3 transition.
    ///
    RegData16 |= (UINT16) B_PCH_LPC_GEN_PMCON_AG3_PP_EN;

    ///
    /// Step 4
    /// Set D31:F0:A2h[11] = 1b to ensure the LAN PHY will be powered for WOL from Sx.
    ///
    RegData16 |= (UINT16) B_PCH_LPC_GEN_PMCON_SX_PP_EN;
    MmioWrite16 (PciD31F0RegBase + R_PCH_LPC_GEN_PMCON_2, RegData16);

    ///
    /// Step 5
    /// "PME_B0_EN", PMBASE + Offset 28h[13], bit must be programmed to enable wakes
    /// from S1-S4 at the Power Management Controller
    /// Done in ASL code(_PRW)
    ///
    ///
    /// Step 6
    /// Set "WOL Enable Override", D31:F0:A4h:[13], bit to 1b to guarantee the
    /// LAN-Wakes are enabled at the Power Management Controller, even in surprise
    /// S5 cases such as power loss/return and Power Button Override
    ///
    RegData16 = MmioRead16 (PciD31F0RegBase + R_PCH_LPC_GEN_PMCON_3);
    RegData16 |= (UINT16) B_PCH_LPC_GEN_PMCON_WOL_ENABLE_OVERRIDE;
    MmioWrite16 (PciD31F0RegBase + R_PCH_LPC_GEN_PMCON_3, RegData16);

    ///
    /// Step 7
    /// Moreover, system BIOS also require to enables in the LAN device by performing
    /// the WOL configuration requirements in the GbE region of the SPI flash.
    /// Done in PchSmmSxGoToSleep() SMM handler.
    ///
  } else {
    ///
    /// D31:F0:A2h[14:11] and D31:F0:A4h[13] are all in RTC or DSW well, so BIOS also
    /// needs to program them while WOL setup option is disabled.
    ///
    RegData16 = MmioRead16 (PciD31F0RegBase + R_PCH_LPC_GEN_PMCON_2);
    RegData16 |= (UINT16) (B_PCH_LPC_GEN_PMCON_DC_PP_DIS | B_PCH_LPC_GEN_PMCON_DSX_PP_DIS);
    RegData16 &= (UINT16) ~(B_PCH_LPC_GEN_PMCON_AG3_PP_EN | B_PCH_LPC_GEN_PMCON_SX_PP_EN);
    MmioWrite16 (PciD31F0RegBase + R_PCH_LPC_GEN_PMCON_2, RegData16);

    RegData16 = MmioRead16 (PciD31F0RegBase + R_PCH_LPC_GEN_PMCON_3);
    RegData16 &= (UINT16) ~(B_PCH_LPC_GEN_PMCON_WOL_ENABLE_OVERRIDE);
    MmioWrite16 (PciD31F0RegBase + R_PCH_LPC_GEN_PMCON_3, RegData16);
  }

  ///
  /// Configure On DC PHY Power Diable according to policy SlpLanLowDc.
  /// When this bit is set, SLP_LAN# will be driven low when ACPRESENT is low.
  /// This indicates that LAN PHY should be powered off on battery mode.
  /// This will override the DC_PP_DIS setting by WolEnableOverride.
  ///
  if (PchPlatformPolicy->Revision >= DXE_PCH_PLATFORM_POLICY_PROTOCOL_REVISION_7) {
    RegData16 = MmioRead16 (PciD31F0RegBase + R_PCH_LPC_GEN_PMCON_2);
    if (PchPlatformPolicy->MiscPmConfig->SlpLanLowDc) {
      if ((RegData16 & B_PCH_LPC_GEN_PMCON_DC_PP_DIS) == 0) {
        RegData16 |= (UINT16) (B_PCH_LPC_GEN_PMCON_DC_PP_DIS);
        MmioWrite16 (PciD31F0RegBase + R_PCH_LPC_GEN_PMCON_2, RegData16);
      }
    } else {
      if ((RegData16 & B_PCH_LPC_GEN_PMCON_DC_PP_DIS) != 0) {
        RegData16 &= (UINT16) ~(B_PCH_LPC_GEN_PMCON_DC_PP_DIS);
        MmioWrite16 (PciD31F0RegBase + R_PCH_LPC_GEN_PMCON_2, RegData16);
      }
    }
  }

  ///
  /// - Set SATA Initialization Register 70h[31:0] = 3F00BF1Fh
  ///
  MmioWrite8 (PciD31F2RegBase + R_PCH_SATA_SIRI, 0x70);
  MmioWrite32 (PciD31F2RegBase + R_PCH_SATA_STRD, 0x3F00BF1F);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint8,
    (UINTN) (PciD31F2RegBase + R_PCH_SATA_SIRI),
    1,
    (VOID *) (UINTN) (PciD31F2RegBase + R_PCH_SATA_SIRI)
    );
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (PciD31F2RegBase + R_PCH_SATA_STRD),
    1,
    (VOID *) (UINTN) (PciD31F2RegBase + R_PCH_SATA_STRD)
    );
  if (PchSeries == PchLp) {
    ///
    /// Set SATA Initialization Register 54h[31:0] = CF000F0Fh
    ///
    MmioWrite8 (PciD31F2RegBase + R_PCH_SATA_SIRI, 0x54);
    MmioWrite32 (PciD31F2RegBase + R_PCH_SATA_STRD, 0xCF000F0F);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint8,
      (UINTN) (PciD31F2RegBase + R_PCH_SATA_SIRI),
      1,
      (VOID *) (UINTN) (PciD31F2RegBase + R_PCH_SATA_SIRI)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (PciD31F2RegBase + R_PCH_SATA_STRD),
      1,
      (VOID *) (UINTN) (PciD31F2RegBase + R_PCH_SATA_STRD)
      );
    ///
    /// Set SATA Initialization Register 58h[31:0] = 190000h
    ///
    MmioWrite8 (PciD31F2RegBase + R_PCH_SATA_SIRI, 0x58);
    MmioWrite32 (PciD31F2RegBase + R_PCH_SATA_STRD, 0x190000);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint8,
      (UINTN) (PciD31F2RegBase + R_PCH_SATA_SIRI),
      1,
      (VOID *) (UINTN) (PciD31F2RegBase + R_PCH_SATA_SIRI)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (PciD31F2RegBase + R_PCH_SATA_STRD),
      1,
      (VOID *) (UINTN) (PciD31F2RegBase + R_PCH_SATA_STRD)
      );
  }
  ///
  /// 4
  /// USB 1.1 / USB 2.0 / USB 3.0
  ///
  ConfigureUsbClockGating (PchPlatformPolicy, RootComplexBar);

  if (PchSeries == PchLp) {
    ///
    /// 5.3
    /// Set D27:F0:43h[6:5][3:0] = 11b, 111b
    ///
    Data8And = (UINT8) ~0x0;
    Data8Or  = (BIT6 | BIT5 | BIT3 | BIT2 | BIT1 | BIT0);
    MmioOr8 ((UINTN) (PciD27F0RegBase + 0x43), Data8Or);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint8,
      (UINTN) (PciD27F0RegBase + 0x43),
      &Data8Or,  // Data to be ORed
      &Data8And  // Data to be ANDed
      );
  }
  ///
  /// 10
  /// SMBus
  /// Enable SMBus dynamic clock gating by setting D31:F3:80h [8, 10, 12 and 14] = 0b respectively
  ///
  MmioAnd16 ((UINTN) (PciD31F3RegBase + 0x80), (UINT16) ~(BIT14 | BIT12 | BIT10 | BIT8));
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint16,
    (UINTN) (PciD31F3RegBase + 0x80),
    1,
    (VOID *) (UINTN) (PciD31F3RegBase + 0x80)
    );
  ///
  /// PCH BIOS Spec Rev 0.5.5, section 19.10
  /// 11
  /// Misc
  ///
  /// Set D31:F2:300h [31:29] to 111b and [19] to 1b
  ///
  MmioOr32 ((UINTN) (PciD31F2RegBase + 0x300), BIT31 | BIT30 | BIT29 | BIT19);
  ///
  /// Set D31:F2:300h [17:16] = 11b
  ///
  MmioOr32 ((UINTN) (PciD31F2RegBase + 0x300), BIT17 | BIT16);

  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (PciD31F2RegBase + 0x300),
    1,
    (VOID *) (UINTN) (PciD31F2RegBase + 0x300)
    );

  if (PchSeries == PchLp) {
    ///
    /// Set D31:F2:98h [31:30], [23] to 00b, 1b
    ///
    MmioAndThenOr32(PciD31F2RegBase + 0x98, (UINT32)~(BIT31 | BIT30), BIT23);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (PciD31F2RegBase + 0x98),
      1,
      (VOID *) (UINTN) (PciD31F2RegBase + 0x98)
      );

    ///
    /// Set RCBA + 0x333C[23:20] to 1100b
    ///
    MmioAndThenOr32 (
      (UINTN) (RootComplexBar + R_PCH_RCRB_PM_CFG2),
      (UINT32)~(BIT21 | BIT20),
      (UINT32) (BIT22 | BIT23)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + R_PCH_RCRB_PM_CFG2),
      1,
      (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_PM_CFG2)
      );
  }
  DEBUG ((EFI_D_INFO, "ConfigureMiscPm() End\n"));

  return EFI_SUCCESS;
}

/**
  Configure additional power management settings

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] RootComplexBar       RootComplexBar value of this PCH device

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
ConfigureAdditionalPm (
  IN  DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy,
  IN  UINT32                              RootComplexBar
  )
{
  UINTN       PciD31F0RegBase;
  UINTN       PciD31F2RegBase;
  UINTN       PciD28F0RegBase;
  UINT32      Data32;
  UINT8       Data8;
  UINT16      LpcDeviceId;
  UINT32      Data32And;
  UINT32      Data32Or;
  EFI_STATUS  Status;
  PCH_SERIES  PchSeries;

  Data32          = 0x0;
  PchSeries       = GetPchSeries();
  PciD31F0RegBase = MmPciAddress (0, PchPlatformPolicy->BusNumber, 31, 0, 0);
  PciD31F2RegBase = MmPciAddress (0, PchPlatformPolicy->BusNumber, 31, 2, 0);
  PciD28F0RegBase = MmPciAddress (0, PchPlatformPolicy->BusNumber, 28, 0, 0);
  LpcDeviceId     = MmioRead16 (PciD31F0RegBase + R_PCH_LPC_DEVICE_ID);

  ///
  /// PCH BIOS Spec Rev 0.5.0, Section 19.4 Additional Power Management Programming
  /// Step 1
  /// Set D31:F0:A9h[7:0] = 46h
  ///
  MmioWrite8 (
    (UINTN) (PciD31F0RegBase + R_PCH_LPC_CIR4),
    (UINT8) (0x46)
    );
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint8,
    (UINTN) (PciD31F0RegBase + R_PCH_LPC_CIR4),
    1,
    (VOID *) (UINTN) (PciD31F0RegBase + R_PCH_LPC_CIR4)
    );
  ///
  /// Step 2
  /// Set Power Management Initialization Register (PMIR) Field 1, D31:F0:ACh[31] = 1b
  /// Done in Intel Management Engine Framework Reference Code
  /// Step 3
  /// Set GEN_PMCON_LOCK register, D31:F0:A6h = 06h, after stretch and ACPI base programming completed.
  /// Done in PchInitBeforeBoot()
  if (PchSeries == PchH) {
    ///
    /// Step 4
    /// Set RCBA + Offset 2238h[0] = 1b
    ///
    MmioOr32 (
      (UINTN) (RootComplexBar + R_PCH_RCRB_CIR2238),
      (UINT32) (BIT0)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + R_PCH_RCRB_CIR2238),
      1,
      (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_CIR2238)
      );
  }
  ///
  /// Step 5
  /// Set RCBA + Offset 232Ch[0] = 1b
  ///
  if (PchSeries == PchH) {
    MmioOr32 (
      (UINTN) (RootComplexBar + 0x232C),
      (UINT32) (BIT0)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + 0x232C),
      1,
      (VOID *) (UINTN) (RootComplexBar + 0x232C)
      );
  }

  if (PchSeries == PchLp) {
    ///
    /// Step 5
    /// Set RCBA + Offset 232Ch[0] = 0b
    ///
    MmioAnd32 (
      (UINTN) (RootComplexBar + 0x232C),
      (UINT32) ~(BIT0)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + 0x232C),
      1,
      (VOID *) (UINTN) (RootComplexBar + 0x232C)
      );
    ///
    /// Step 6
    /// If Trunk Clock Gating is enabled:
    ///   set RCBA + Offset 1100h[15,14,8,5,4,3,2,1,0] all 1b
    /// If Trunk Clock Gating is disabled:
    ///   set RCBA + Offset 1100h[15] = 0b
    ///   set RCBA + Offset 1100h[14,8,5,4,3,2,1,0] all 1b
    ///
    
    if((PchPlatformPolicy->UsbConfig->Usb30Settings.Mode != PCH_XHCI_MODE_ON) && 
       (PchPlatformPolicy->UsbConfig->Usb30Settings.Btcg == PCH_DEVICE_DISABLE)) {
      MmioAndThenOr32 (
      (UINTN) (RootComplexBar + R_PCH_RCRB_CIR1100),
      (UINT32)~(BIT15),
      (UINT32) (BIT14 | BIT8 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)
      );      
    } else {
      MmioOr32 (
      (UINTN) (RootComplexBar + R_PCH_RCRB_CIR1100),
      (UINT32) (BIT15 | BIT14 | BIT8 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)
      );
    }
    
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + R_PCH_RCRB_CIR1100),
      1,
      (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_CIR1100)
      );
  } else if (PchSeries == PchH) {
    ///
    /// Step 6
    /// If Truck Clock Gating is enabled:
    ///   set RCBA + Offset 1100h[14:13] = 11b
    /// If Truck Clock Gating is disabled:
    ///   set RCBA + Offset 1100h[14:13] = 10b
    ///
    if((PchPlatformPolicy->UsbConfig->Usb30Settings.Mode != PCH_XHCI_MODE_ON) && 
       (PchPlatformPolicy->UsbConfig->Usb30Settings.Btcg == PCH_DEVICE_DISABLE)) {
      MmioAndThenOr32 ((UINTN) (RootComplexBar + R_PCH_RCRB_CIR1100), (UINT32) ~(BIT13), (UINT32) (BIT14));
    } else {
      MmioOr32 (
        (UINTN) (RootComplexBar + R_PCH_RCRB_CIR1100),
        (UINT32) (BIT14 | BIT13)
      );
    }     
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + R_PCH_RCRB_CIR1100),
      1,
      (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_CIR1100)
      );
  }

  if (PchSeries == PchLp) {
    ///
    /// Set IOBP register 0xCF000000[14:12] = 111b
    /// Set IOBP register 0XCF000000[0] = 1b
    ///
    Data32And = (UINT32)~(0);
    Data32Or = 0x7001;
    Status = ProgramIobp (
              RootComplexBar,
              0xCF000000,
              Data32And,
              Data32Or
              );
    ASSERT_EFI_ERROR (Status);
    Status = PCH_INIT_COMMON_SCRIPT_SAVE_IOBP_S3_ITEM (
              RootComplexBar,
              0xCF000000,
              Data32And,
              Data32Or
              );
    ASSERT_EFI_ERROR (Status);

    ///
    /// Set IOBP register 0xCA000000[3] = 1b
    /// Set IOBP register 0XCA000000[0] = 1b
    ///
    Data32And = (UINT32)~(0);
    Data32Or = (UINT32) (0x09);
    Status = ProgramIobp (
              RootComplexBar,
              0xCA000000,
              Data32And,
              Data32Or
              );
    ASSERT_EFI_ERROR (Status);
    Status = PCH_INIT_COMMON_SCRIPT_SAVE_IOBP_S3_ITEM (
              RootComplexBar,
              0xCA000000,
              Data32And,
              Data32Or
              );
    ASSERT_EFI_ERROR (Status);
  } else if (PchSeries == PchH) {
    ///
    /// Step 7
    /// Set RCBA + Offset 2304h[31:0] = 0xC07B8400
    ///
    MmioWrite32 (
      (UINTN) (RootComplexBar + R_PCH_RCRB_DMC),
      (UINT32) (0xC07B8400)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + R_PCH_RCRB_DMC),
      1,
      (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_DMC)
      );
    ///
    /// Step 8
    /// Set RCBA + Offset 2314h[23 and 5] = 1b
    ///
    MmioOr32 (
      (UINTN) (RootComplexBar + R_PCH_RCRB_CIR2314),
      (UINT32) (BIT23 | BIT5)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + R_PCH_RCRB_CIR2314),
      1,
      (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_CIR2314)
      );
  }

  ///
  /// PCH BIOS Spec Rev 0.5.5, Section 19.4 Additional Power Management Programming
  /// Step 9
  /// Set B0:D28:F0 + F5h[3:0] = 0101b
  ///
  MmioAndThenOr8 (
    (UINTN) (PciD28F0RegBase + 0xF5),
    (UINT8) ~(BIT3 | BIT1),
    (UINT8) (BIT2 | BIT0)
    );
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint8,
    (UINTN) (PciD28F0RegBase + 0xF5),
    1,
    (VOID *) (UINTN) (PciD28F0RegBase + 0xF5)
    );

  if (PchSeries == PchH) {
    ///
    /// Step 10
    /// Set RCBA + Offset 2320h [1] = 1b
    ///
    MmioOr32 ((UINTN) (RootComplexBar + R_PCH_RCRB_CIR2320), (UINT32) (BIT1));
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + R_PCH_RCRB_CIR2320),
      1,
      (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_CIR2320)
      );
  }
  if (PchSeries == PchLp) {
    ///
    /// Step 10
    /// Set RCBA + Offset 2320h [6:4] = 001b
    ///
    MmioAndThenOr32 (
      (UINTN) (RootComplexBar + R_PCH_RCRB_CIR2320),
      (UINT8) ~(BIT6 | BIT5),
      (UINT8) (BIT4)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + R_PCH_RCRB_CIR2320),
      1,
      (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_CIR2320)
      );
  }
  switch (PchSeries) {
    case PchLp:
      ///
      /// Step 11
      /// Set RCBA + Offset 3314h[31:0] = 0x00012FFF
      ///
      Data32 = 0x00012FFF;
      break;

    case PchH:
    default:
      ///
      /// Step 11
      /// Set RCBA + Offset 3314h[31:0] = 0x000007BF
      ///
      Data32 = 0x000007BF;
      break;
  }
  MmioWrite32 (
    (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3314),
    (UINT32) (Data32)
    );
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3314),
    1,
    (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3314)
    );

  switch (PchSeries) {
    case PchLp:
      ///
      /// Step 12
      /// Set RCBA + Offset 3318h[31:0] = 0x0DCF0400
      ///
      Data32 = 0x0DCF0400;
      break;

    case PchH:
    default:
      ///
      /// Step 12
      /// Set RCBA + Offset 3318h[31:0] = 0x0DCF0020 (Note: Keep BIT5 unchanged)
      ///
      Data32 = 0x0DCF0020;
      break;
  }
  ///
  /// Note: RCBA + 3318h[19:16] are platform dependent settings (0Fh provides longest assertion),
  /// please consult with your board design engineers for correct values to be programmed to.
  ///
  /// For RCBA + 3318h[9:8] Reset Power Cycle Duration could be customized, please refer to EDS
  /// and make sure the setting correct, which never less than the following register.
  /// - GEN_PMCON_3.SLP_S3_MIN_ASST_WDTH
  /// - GEN_PMCON_3.SLP_S4_MIN_ASST_WDTH
  /// - PM_CFG.SLP_A_MIN_ASST_WDTH
  /// - PM_CFG.SLP_LAN_MIN_ASST_WDTH
  ///
  Data32 &= (UINT32)~(B_PCH_RCRB_PM_CFG_SSMAW_MASK | B_PCH_RCRB_PM_CFG_SAMAW_MASK);
  if (PchPlatformPolicy->Revision >= DXE_PCH_PLATFORM_POLICY_PROTOCOL_REVISION_4) {
    Data32 &= (UINT32)~(B_PCH_RCRB_PM_CFG_RPCD_MASK);
  }

  switch (PchPlatformPolicy->MiscPmConfig->PchSlpSusMinAssert) {
    case PchSlpSus0ms:
      Data32 |= V_PCH_RCRB_PM_CFG_SSMAW_0S;
      break;

    case PchSlpSus500ms:
      Data32 |= V_PCH_RCRB_PM_CFG_SSMAW_0_5S;
      break;

    case PchSlpSus1s:
      Data32 |= V_PCH_RCRB_PM_CFG_SSMAW_1S;
      break;

    case PchSlpSus4s:
    default:
      Data32 |= V_PCH_RCRB_PM_CFG_SSMAW_4S;
      break;
  }
  switch (PchPlatformPolicy->MiscPmConfig->PchSlpAMinAssert) {
    case PchSlpA0ms:
      Data32 |= V_PCH_RCRB_PM_CFG_SAMAW_0S;
      break;

    case PchSlpA4s:
      Data32 |= V_PCH_RCRB_PM_CFG_SAMAW_4S;
      break;

    case PchSlpA98ms:
      Data32 |= V_PCH_RCRB_PM_CFG_SAMAW_98ms;
      break;

    case PchSlpA2s:
    default:
      Data32 |= V_PCH_RCRB_PM_CFG_SAMAW_2S;
      break;
  }
  if (PchPlatformPolicy->Revision >= DXE_PCH_PLATFORM_POLICY_PROTOCOL_REVISION_4) {
    switch (PchPlatformPolicy->MiscPmConfig->PchPwrCycDur) {
      case 0:  // treat as PCH default
        Data32 |= V_PCH_RCRB_PM_CFG_RPCD_4S;
        break;

      case 1:
        Data32 |= V_PCH_RCRB_PM_CFG_RPCD_1S;
        break;

      case 2:
        Data32 |= V_PCH_RCRB_PM_CFG_RPCD_2S;
        break;

      case 3:
        Data32 |= V_PCH_RCRB_PM_CFG_RPCD_3S;
        break;

      case 4:
        Data32 |= V_PCH_RCRB_PM_CFG_RPCD_4S;
        break;

      default:
        Data32 |= V_PCH_RCRB_PM_CFG_RPCD_4S;
        DEBUG ((EFI_D_ERROR, "Error. Not a valid PCH reset power cycle duration setting.\n"));
        break;
    }
  }
  ///
  /// For LP, force bit 5 = 0
  /// For LPT-H, preserve bit 5
  ///
  if (PchSeries == PchLp) {
    Data32  &= (UINT32) ~(BIT5);
    MmioWrite32 (
      (UINTN) (RootComplexBar + R_PCH_RCRB_PM_CFG),
      Data32
      );
  } else {
    MmioOr32 (
      (UINTN) (RootComplexBar + R_PCH_RCRB_PM_CFG),
      Data32
      );
  }
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (RootComplexBar + R_PCH_RCRB_PM_CFG),
    1,
    (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_PM_CFG)
    );
  ///
  /// PCH BIOS Spec Rev 0.5.0, Section 19.4 Additional Power Management Programming
  /// Step 13
  /// Set RCBA + Offset 3324h[31:0] = 0x04000000
  ///
  MmioWrite32 (
    (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3324),
    (UINT32) (0x04000000)
    );
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3324),
    1,
    (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3324)
    );

  if (PchSeries == PchH) {
    ///
    /// Step 14
    /// Set RCBA + Offset 3340h[31:0] = 0x020DDBFF
    ///
    MmioOr32 (
      (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3340),
      (UINT32) (0x020DDBFF)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3340),
      1,
      (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3340)
      );
    ///
    /// Step 15
    /// Set RCBA + Offset 3344h[0] = 1b
    ///
    MmioOr32 (
      (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3344),
      (UINT32) (BIT0)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3344),
      1,
      (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3344)
      );
  }
  switch (PchSeries) {
    case PchLp:
      ///
      /// Step 16
      /// Set RCBA + Offset 3368h[31:0] = 0x00041400
      ///
      Data32 = 0x00041400;
      break;

    case PchH:
    default:
      ///
      /// Step 16
      /// Set RCBA + Offset 3368h[31:0] = 0x00041000
      ///
      Data32 = 0x00041000;
      break;
  }
  MmioWrite32 (
    (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3368),
    (UINT32) (Data32)
    );
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3368),
    1,
    (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3368)
    );

  if (PchSeries == PchH) {
    ///
    /// Step 17
    /// Set RCBA + Offset 3378h[31:0] = 3F8DDBFFh
    ///
    MmioWrite32 (
      (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3378),
      (UINT32) (0x3F8DDBFF)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3378),
      1,
      (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3378)
      );
    ///
    /// Step 18
    /// Set RCBA + Offset 337Ch[31:0] = 000001E1h
    ///
    MmioWrite32 (
      (UINTN) (RootComplexBar + R_PCH_RCRB_CIR337C),
      (UINT32) (0x000001E1)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + R_PCH_RCRB_CIR337C),
      1,
      (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_CIR337C)
      );
  }
  switch (PchSeries) {
    case PchLp:
      ///
      /// Step 19
      /// Set RCBA + Offset 3388h[31:0] = 0x3F8DDBFF
      ///
      Data32 = 0x3F8DDBFF;
      break;

    case PchH:
    default:
      ///
      /// Step 19
      /// Set RCBA + Offset 3388h[31:0] = 0x00001000
      ///
      Data32 = 0x00001000;
      break;
  }
  MmioWrite32 (
    (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3388),
    (UINT32) (Data32)
    );
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3388),
    1,
    (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3388)
    );
  if (PchSeries == PchH) {
    ///
    /// Step 20
    /// Set RCBA + Offset 33A0h[31:0] = 00000800h
    ///
    MmioWrite32 (
      (UINTN) (RootComplexBar + R_PCH_RCRB_CIR33A0),
      (UINT32) (0x00000800)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + R_PCH_RCRB_CIR33A0),
      1,
      (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_CIR33A0)
      );
  }
  switch (PchSeries) {
    case PchLp:
      ///
      /// Step 21
      /// Set RCBA + Offset 33ACh[31:0] = 0x00007001
      ///
      MmioWrite32 (
        (UINTN) (RootComplexBar + 0x33AC),
        (UINT32) (0x00007001)
        );
      break;

    case PchH:
    default:
      ///
      /// Step 21
      /// Set RCBA + Offset 33ACh[31:0] = 00001000h
      ///
      MmioWrite32 (
        (UINTN) (RootComplexBar + 0x33AC),
        (UINT32) (0x00001000)
        );
      break;
  }
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (RootComplexBar + 0x33AC),
    1,
    (VOID *) (UINTN) (RootComplexBar + 0x33AC)
    );
  switch (PchSeries) {
    case PchLp:
      ///
      /// Step 22
      /// Set RCBA + Offset 33B0h[31:0] = 0x00181900
      ///
      Data32 = 0x00181900;
      break;

    case PchH:
    default:
      ///
      /// Step 22
      /// Set RCBA + Offset 33B0h[31:0] = 0x00001000
      ///
      Data32 = 0x00001000;
      break;
  }
  MmioWrite32 (
    (UINTN) (RootComplexBar + R_PCH_RCRB_CIR33B0),
    (UINT32) (Data32)
    );
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (RootComplexBar + R_PCH_RCRB_CIR33B0),
    1,
    (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_CIR33B0)
    );
  switch (PchSeries) {
    case PchLp:
      ///
      /// Step 23
      /// Set RCBA + Offset 33C0h[31:0] = 0x00060A00
      ///
      Data32 = 0x00060A00;
      break;

    case PchH:
    default:
      ///
      /// Step 23
      /// Set RCBA + Offset 33C0h[31:0] = 0x00011900
      ///
      Data32 = 0x00011900;
      break;
  }
  MmioWrite32 (
    (UINTN) (RootComplexBar + R_PCH_RCRB_CIR33C0),
    (UINT32) (Data32)
    );
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (RootComplexBar + R_PCH_RCRB_CIR33C0),
    1,
    (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_CIR33C0)
    );
  ///
  /// Step 24
  /// LP Set RCBA + Offset 33D0h[31:0] = 0x06200840
  /// LP Set RCBA + Offset 33D0h[31:0] = 06004622h for LPT LP A0/A1 only
  /// LPT-H Set RCBA + Offset 33D0h[31:0] = 06000802h
  switch (PchSeries) {
    case PchLp:
    if (PchSeries == PchLp && PchStepping() < LptLpB0) {
      Data32 = 0x06004622;
    } else {
      Data32 = 0x06200840;
    }
    break;

  case PchH:
  default:
    Data32 = 0x06000802;
    break;
  }

  MmioWrite32 (
    (UINTN) (RootComplexBar + R_PCH_RCRB_CIR33D0),
    (UINT32) (Data32)
    );
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (RootComplexBar + R_PCH_RCRB_CIR33D0),
    1,
    (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_CIR33D0)
    );
  ///
  /// Step 25 -- Note, this step has been moved to meet programming sequence requirements
  /// Register 3A80 - 3A88 must be program after 3A00-3A3F and before 3A6C
  /// Set RCBA + 3A88h[31:0] = 0x00000001
  ///
  switch (PchSeries) {
  case PchLp:
    ///
    /// Step 26
    /// Set RCBA + Offset 3A28h[31:0] = 01010101h
    ///
    MmioWrite32 (
      (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3A28),
      (UINT32) (0x01010101)
      );
    break;

  case PchH:
  default:
    ///
    /// Step 26
    /// Set RCBA + Offset 3A28h[31:0] = 01010000h
    ///
    MmioWrite32 (
      (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3A28),
      (UINT32) (0x01010000)
      );
    break;
  }
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3A28),
    1,
    (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3A28)
    );
  switch (PchSeries) {
    case PchLp:
      ///
      /// Step 27
      /// Set RCBA + Offset 3A2Ch[31:0] = 04040404h
      ///
      MmioWrite32 (
        (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3A2C),
        (UINT32) (0x04040404)
        );
      break;

    case PchH:
    default:
      ///
      /// Step 27
      /// Set RCBA + Offset 3A2Ch[31:0] = 01010404h
      ///
      MmioWrite32 (
        (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3A2C),
        (UINT32) (0x01010404)
        );
      break;
  }
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3A2C),
    1,
    (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3A2C)
    );
  ///
  /// Step 29
  /// Set RCBA + Offset 3A6Ch[31:0] = 00000001h, after all steps in this routine are done
  /// Done in PchInitBeforeBoot()
  ///
  /// Step 30
  /// For PCH H
  /// Set RCBA + Offset 2344h[31:24] = 0FFh
  /// Set RCBA + Offset 2344h[7:0] = 0Ch
  /// Done in PchInitBeforeBoot()
  ///
  /// Step 31
  /// For LPT-H set RCBA + Offset 33A4h[0] = 1b
  ///
  if (PchSeries == PchH) {
    MmioOr32 (
      (UINTN) (RootComplexBar + 0x33A4),
      (UINT32) (BIT0)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + 0x33A4),
      1,
      (VOID *) (UINTN) (RootComplexBar + 0x33A4)
      );
  }

  if (PchSeries == PchLp) {
    ///
    /// Step 32
    ///
    /// Set RCBA + Offset 2B1Ch
    /// [31:30] =2'b00
    /// [29] =0 if Audio DSP is enabled. 1 if disabled (RCBA offset x3418[1]=1). ConfigureAudioDsp will take care of this bit, which is executed later
    ///        Note1: Must assume enable in this flow because ConfigureAudioDsp only program this in the "audio disable flow" only
    /// [28:22] = 7'b0001110
    /// [21:16]=corresponding bit has to be set for each SRC[5:0]CLKRQ# pin that is enabled (ie attached to a PCIe device)
    /// [15:0]=0x8033h
    ///
    Data32 = 0x03808033;
    MmioWrite32 (
      (UINTN) (RootComplexBar + 0x2B1C),
      (UINT32) (Data32)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + 0x2B1C),
      1,
      (VOID *) (UINTN) (RootComplexBar + 0x2B1C)
      );
    ///
    /// Step 33
    /// Set RCBA + Offset 2B34[31:0] = 80000009h
    /// Set bit 3 and 0, PMC shutdown time = 16us
    ///
    Data32 = 0x80000009;
    MmioWrite32 (
      (UINTN) (RootComplexBar + 0x2B34),
      (UINT32) (Data32)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + 0x2B34),
      1,
      (VOID *) (UINTN) (RootComplexBar + 0x2B34)
      );

    ///
    /// Step 34
    /// Set RCBA + Offset 3348[31:0] = 022DDFFFh
    ///
    Data32 = 0x022DDFFF;
    MmioWrite32 (
      (UINTN) (RootComplexBar + 0x3348),
      (UINT32) (Data32)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + 0x3348),
      1,
      (VOID *) (UINTN) (RootComplexBar + 0x3348)
      );
    ///
    /// Step 35
    /// Set RCBA + Offset 334C[31:0] = 00000001h
    ///
    Data32 = 0x00000001;
    MmioWrite32 (
      (UINTN) (RootComplexBar + 0x334C),
      (UINT32) (Data32)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + 0x334C),
      1,
      (VOID *) (UINTN) (RootComplexBar + 0x334C)
      );
    ///
    /// Step 36
    /// Set RCBA + Offset 3358[31:0] = 0001C000h
    ///
    Data32 = 0x0001C000;
    MmioWrite32 (
      (UINTN) (RootComplexBar + 0x3358),
      (UINT32) (Data32)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + 0x3358),
      1,
      (VOID *) (UINTN) (RootComplexBar + 0x3358)
      );
    ///
    /// Step 37
    /// Set RCBA + Offset 3380[31:0] = 3F8DDBFFh
    ///
    Data32 = 0x3F8DDBFF;
    MmioWrite32 (
      (UINTN) (RootComplexBar + 0x3380),
      (UINT32) (Data32)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + 0x3380),
      1,
      (VOID *) (UINTN) (RootComplexBar + 0x3380)
      );
    ///
    /// Step 38
    /// Set RCBA + Offset 3384[31:0] = 0001C7E1h
    ///
    Data32 = 0x0001C7E1;
    MmioWrite32 (
      (UINTN) (RootComplexBar + 0x3384),
      (UINT32) (Data32)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + 0x3384),
      1,
      (VOID *) (UINTN) (RootComplexBar + 0x3384)
      );
    ///
    /// Step 39
    /// Set RCBA + Offset 338C[31:0] = 0x0001C7E1
    ///
    Data32 = 0x0001C7E1;
    MmioWrite32 (
      (UINTN) (RootComplexBar + 0x338C),
      (UINT32) (Data32)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + 0x338C),
      1,
      (VOID *) (UINTN) (RootComplexBar + 0x338C)
      );
    ///
    /// Step 40
    /// Set RCBA + Offset 3398[31:0] = 0001C000h
    ///
    Data32 = 0x0001C000;
    MmioWrite32 (
      (UINTN) (RootComplexBar + 0x3398),
      (UINT32) (Data32)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + 0x3398),
      1,
      (VOID *) (UINTN) (RootComplexBar + 0x3398)
      );
    ///
    /// Step 41
    /// Set RCBA + Offset 33A8[31:0] = 0x00181900
    ///
    Data32 = 0x00181900;
    MmioWrite32 (
      (UINTN) (RootComplexBar + 0x33A8),
      (UINT32) (Data32)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + 0x33A8),
      1,
      (VOID *) (UINTN) (RootComplexBar + 0x33A8)
      );
    ///
    /// Step 42
    /// Set RCBA + Offset 33DC[31:0] = 00080000h
    ///
    Data32 = 0x00080000;
    MmioWrite32 (
      (UINTN) (RootComplexBar + 0x33DC),
      (UINT32) (Data32)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + 0x33DC),
      1,
      (VOID *) (UINTN) (RootComplexBar + 0x33DC)
      );
    ///
    /// Step 43
    /// Set RCBA + Offset 33E0[31:0] = 00000001h
    ///
    Data32 = 0x00000001;
    MmioWrite32 (
      (UINTN) (RootComplexBar + 0x33E0),
      (UINT32) (Data32)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + 0x33E0),
      1,
      (VOID *) (UINTN) (RootComplexBar + 0x33E0)
      );
    ///
    /// Step 44
    /// Set RCBA + Offset 3A20[31:0] = 00000404h
    ///
    Data32 = 0x00000404;
    MmioWrite32 (
      (UINTN) (RootComplexBar + 0x3A20),
      (UINT32) (Data32)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + 0x3A20),
      1,
      (VOID *) (UINTN) (RootComplexBar + 0x3A20)
      );
    ///
    /// Step 45
    /// Set RCBA + Offset 3A24[31:0] = 01010101h
    ///
    Data32 = 0x01010101;
    MmioWrite32 (
      (UINTN) (RootComplexBar + 0x3A24),
      (UINT32) (Data32)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + 0x3A24),
      1,
      (VOID *) (UINTN) (RootComplexBar + 0x3A24)
      );
    ///
    /// Step 46
    /// Set RCBA + Offset 3A30[31:0] = 01010101h
    ///
    Data32 = 0x01010101;
    MmioWrite32 (
      (UINTN) (RootComplexBar + 0x3A30),
      (UINT32) (Data32)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + 0x3A30),
      1,
      (VOID *) (UINTN) (RootComplexBar + 0x3A30)
      );
    ///
    /// Step 47
    /// Set D31:F0:ACh[21] = 0b
    ///
    MmioAnd32 (
      (UINTN) (PciD31F0RegBase + R_PCH_LPC_PMIR),
      (UINT32) ~(BIT21)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (PciD31F0RegBase + R_PCH_LPC_PMIR),
      1,
      (VOID *) (UINTN) (PciD31F0RegBase + R_PCH_LPC_PMIR)
      );
    ///
    /// Step 48
    /// set RCBA + Offset 410h[1:0] = 11b
    ///
    MmioOr32 (
      (UINTN) (RootComplexBar + 0x410),
      (UINT32) (BIT1 | BIT0)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + 0x410),
      1,
      (VOID *) (UINTN) (RootComplexBar + 0x410)
      );
    ///
    /// Step 49
    /// Set RCBA + 2618h[27] = 1b
    ///
    MmioOr32 (
      (UINTN) (RootComplexBar + 0x2618),
      (UINT32) BIT27
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + 0x2618),
      1,
      (VOID *) (UINTN) (RootComplexBar + 0x2618)
      );
    ///
    /// Step 50
    /// Set RCBA + 2300h[1] = 1b
    ///
    Data32And = 0xFFFFFFFF;
    Data32Or  = (UINT32) BIT1;
    MmioOr32 (RootComplexBar + 0x2300 , Data32Or);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + 0x2300),
      &Data32Or,  // Data to be ORed
      &Data32And  // Data to be ANDed
      );
    ///
    /// Step 51
    /// Set RCBA + 2600h[3] = 1b
    ///
    MmioOr32 (
      (UINTN) (RootComplexBar + 0x2600),
      (UINT32) BIT3
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + 0x2600),
      1,
      (VOID *) (UINTN) (RootComplexBar + 0x2600)
      );
    ///
    /// Step 52
    /// Set RCBA + 33B4h[0] = 0x00007001
    ///
    Data32 = 0x00007001;
    MmioWrite32 (
      (UINTN) (RootComplexBar + 0x33B4),
      (UINT32) (Data32)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + 0x33B4),
      1,
      (VOID *) (UINTN) (RootComplexBar + 0x33B4)
      );
    /// Step 53
    /// Set RCBA + Offset 3350[31:0] = 0x022DDFFF
    ///
    Data32 = 0x022DDFFF;
    MmioWrite32 (
      (UINTN) (RootComplexBar + 0x3350),
      (UINT32) (Data32)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + 0x3350),
      1,
      (VOID *) (UINTN) (RootComplexBar + 0x3350)
      );
    ///
    /// Step 54
    /// Set RCBA + Offset 3354[31:0] = 0x00000001
    ///
    Data32 = 0x00000001;
    MmioWrite32 (
      (UINTN) (RootComplexBar + 0x3354),
      (UINT32) (Data32)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + 0x3354),
      1,
      (VOID *) (UINTN) (RootComplexBar + 0x3354)
      );
  }

  if ((PchPlatformPolicy->PwrOptConfig->PchPwrOptDmi == PCH_DEVICE_ENABLE)) {
     ///
     /// PCH BIOS Spec Rev 0.5.0, Section 19.13 Power Optimizer Considerations (MB Only)
     /// Notes: Settings is not recommended for Lynx Point Power on ES0 samples
     /// Step 1
     /// Enable PM SYNC State 12
     /// Program RCBA + 33D4h[27] = 1b
     /// For PCH LP
     /// Program RCBA + 2B14[31:0] = 1E0A4616h
     /// Program RCBA + 2B24[31:0] = 40000005h
     /// For PCH H
     /// Program RCBA + 2B14[31:0] = 1E0A0317h
     /// Program RCBA + 2B24[31:0] = 4000000Bh
     /// Program RCBA + 2B28[31:0] = 00000002h
     /// Program RCBA + 2B2C[31:0] = 00008813h
     ///
     MmioOr32 (
       (UINTN) (RootComplexBar + R_PCH_RCRB_CIR33D4),
       (UINT32) BIT27
       );
     SCRIPT_MEM_WRITE (
       EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
       EfiBootScriptWidthUint32,
       (UINTN) (RootComplexBar + R_PCH_RCRB_CIR33D4),
       1,
       (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_CIR33D4)
       );
     ///
     /// Program RCBA + 33C8h[27] = 1b
     ///
     Data32Or = BIT27;
     if (PchSeries == PchLp) {
       ///
       /// Program RCBA + 33C8h[7] = 1b
       ///
       Data32Or = BIT7;
     }
     MmioOr32 (
       (UINTN) (RootComplexBar + R_PCH_RCRB_PMSYNC),
       (UINT32) Data32Or
       );
     SCRIPT_MEM_WRITE (
       EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
       EfiBootScriptWidthUint32,
       (UINTN) (RootComplexBar + R_PCH_RCRB_PMSYNC),
       1,
       (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_PMSYNC)
       );
     ///
     /// For LPT-LP, Program RCBA + 2B10[31:0] = 0000883Ch
     ///
     if (PchSeries == PchLp) {
       MmioWrite32 (
         (UINTN) (RootComplexBar + 0x2B10),
         (UINT32) (0x0000883C)
         );
       SCRIPT_MEM_WRITE (
         EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
         EfiBootScriptWidthUint32,
         (UINTN) (RootComplexBar + 0x2B10),
         1,
         (VOID *) (UINTN) (RootComplexBar + 0x2B10)
         );
     }
     ///
     /// Program RCBA + 2B14[31:0] = 1E0A0317h
     /// For LP Program RCBA + 2B14[31:0] = 1E0A4616h
     ///
     Data32 = 0x1E0A0317;
     if (PchSeries == PchLp) {
       Data32 = 0x1E0A4616;
     }
     if (PchPlatformPolicy->PwrOptConfig->MemCloseStateEn == PCH_DEVICE_DISABLE) {
       Data32  &= (UINT32) ~(BIT2);
     }
     if (PchPlatformPolicy->PwrOptConfig->InternalObffEn == PCH_DEVICE_DISABLE) {
       Data32  &= (UINT32) ~(BIT1);
     }
     if (PchPlatformPolicy->PwrOptConfig->ExternalObffEn == PCH_DEVICE_DISABLE) {
       Data32  &= (UINT32) ~(BIT0);
     }
     MmioWrite32 (
       (UINTN) (RootComplexBar + 0x2B14),
       (UINT32) (Data32)
       );
     SCRIPT_MEM_WRITE (
       EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
       EfiBootScriptWidthUint32,
       (UINTN) (RootComplexBar + 0x2B14),
       1,
       (VOID *) (UINTN) (RootComplexBar + 0x2B14)
       );
     if (PchSeries == PchLp) {
       ///
       /// Set RCBA + Offset 2B24[31:0] = 0x40000005
       ///
       Data32 = 0x40000005;
     } else {
       ///
       /// Set RCBA + Offset 2B24[31:0] = 0x4000000B
       ///
       Data32 = 0x4000000B;
     }
     MmioWrite32 (
       (UINTN) (RootComplexBar + 0x2B24),
       Data32
       );
     SCRIPT_MEM_WRITE (
       EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
       EfiBootScriptWidthUint32,
       (UINTN) (RootComplexBar + 0x2B24),
       1,
      (VOID *) (UINTN) (RootComplexBar + 0x2B24)
       );
     if (PchSeries == PchH) {
       ///
       /// Set RCBA + Offset 2B28[31:0] = 0x00000002
       ///
       MmioWrite32 (
         (UINTN) (RootComplexBar + 0x2B28),
         (UINT32) (0x00000002)
         );
       SCRIPT_MEM_WRITE (
         EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
         EfiBootScriptWidthUint32,
         (UINTN) (RootComplexBar + 0x2B28),
         1,
         (VOID *) (UINTN) (RootComplexBar + 0x2B28)
         );
     }
     if (PchSeries == PchH) {
       ///
       /// Set RCBA + Offset 2B2C[31:0] = 0x00008813
       ///
       Data32 = 0x00008813;
     MmioWrite32 (
       (UINTN) (RootComplexBar + 0x2B2C),
       Data32
     );
     SCRIPT_MEM_WRITE (
       EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
       EfiBootScriptWidthUint32,
       (UINTN) (RootComplexBar + 0x2B2C),
       1,
       (VOID *) (UINTN) (RootComplexBar + 0x2B2C)
      );
     }
     /// Step 7
     /// Enable PM Demand in Cx States
     /// For LPT-H, use default
     /// For LPT-LP Program RCBA + 02B20h[1:0] = 0x0005DB01
     ///
     if (PchSeries == PchLp) {
       Data32 = 0x0005DB01;
       MmioWrite32 (
         (UINTN) (RootComplexBar + 0x2B20),
         Data32
       );
       SCRIPT_MEM_WRITE (
         EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
         EfiBootScriptWidthUint32,
         (UINTN) (RootComplexBar + 0x2B20),
         1,
         (VOID *) (UINTN) (RootComplexBar + 0x2B20)
         );
    }
  }
  switch (PchSeries) {
    case PchLp:
      ///
      /// Step 55
      /// Set RCBA + 3A80h[31:0] = 05145005h
      ///
      MmioWrite32 (
        (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3A80),
        (UINT32) (0x05145005)
        );
      break;
    case PchH:
    default:
      ///
      /// Step 55
      /// Set RCBA + 3A80h[31:0] = 01040000h
      ///
      MmioWrite32 (
        (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3A80),
        (UINT32) (0x01040000)
        );
      break;
  }
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3A80),
    1,
    (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3A80)
    );
  ///
  /// Step 56
  /// Ensure this is done after 3A00-3A3C and before 3A6C
  /// if PchLp, Set RCBA + Offset 3A84h[31:0] = 0x00001005
  /// if PchH,  Set RCBA + Offset 3A84h[31:0] = 0x01041001
  /// if PCS.P0E and PCS.P1E = 0b, Set RCBA + Offset 3A84h[20,18] = 1b, 1b
  /// if PCS.P2E and PCS.P3E = 0b, Set RCBA + Offset 3A84h[24,26] = 1b, 1b
  ///
  if (PchSeries == PchLp) {
    Data32 = 0x00001005;
  } else {
    Data32 = 0x01041001;
  }
  Data8 = MmioRead8 (PciD31F2RegBase + R_PCH_SATA_PCS);
  if((Data8 & (UINT8) (B_PCH_SATA_PCS_PORT0_EN | B_PCH_SATA_PCS_PORT1_EN)) == 0) {
    Data32 |= (BIT20 | BIT18);
  }
  if((Data8 & (UINT8) (B_PCH_SATA_PCS_PORT2_EN | B_PCH_SATA_PCS_PORT3_EN)) == 0) {
    Data32 |= (BIT24 | BIT26);
  }
  MmioWrite32 (
    (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3A84),
    (UINT32) (Data32)
    );
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3A84),
    1,
    (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3A84)
    );
  ///
  /// Step 57
  /// Set RCBA + 3A88h[31:0] = 0x00000001
  ///
  if (PchSeries == PchH) {
    MmioWrite32 (
      (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3A88),
      (UINT32) (0x00000001)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3A88),
      1,
      (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_CIR3A88)
      );
  }
  ///
  /// Step 58
  /// For LPT-LP Set RCBA + Offset 33D4h = 0x2FFF2FB1, after step #14 to #24 and D31:F0:A9h are done
  /// Note for LP only: Preserve bits 31,30,28,15,14,12, which are platform specific
  /// For LPT-H Set RCBA + Offset 33D4h = 0xC00BC000, after step #14 to #24 and D31:F0:A9h are done
  ///
  switch (PchSeries) {
    case PchLp:
    MmioOr32 (
      (UINTN) (RootComplexBar + 0x33D4),
      (UINT32) (0x2FFF2FB1)
      );
      break;

    case PchH:
    default:
    MmioWrite32 (
      (UINTN) (RootComplexBar + 0x33D4),
      (UINT32) (0xC00BC000)
      );
      break;
  }
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (RootComplexBar + 0x33D4),
    1,
    (VOID *) (UINTN) (RootComplexBar + 0x33D4)
    );

  if (PchSeries == PchLp) {
    ///
    /// This is the last step which only apply for LPT-LP
    /// Set RCBA + Offset 33C8h[15] = 1b
    ///
    MmioOr32 (
      (UINTN) (RootComplexBar + 0x33C8),
      (UINT32) (BIT15)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + 0x33C8),
      1,
      (VOID *) (UINTN) (RootComplexBar + 0x33C8)
      );
  }
  return EFI_SUCCESS;
}

/**
  Configures PCH DMI according to policies specified in PCH Platform Policy protocol

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] RootComplexBar       RootComplexBar value of this PCH device

  @retval EFI_SUCCESS             Successfully completed.
**/
EFI_STATUS
EFIAPI
ConfigureDmiPm (
  IN  DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy,
  IN  UINT32                              RootComplexBar
  )
{
  UINT32                        Data32And;
  UINT32                        Data32Or;
  UINT16                        Data16And;
  UINT16                        Data16Or;
  UINTN                         PciD28F0RegBase;
  PCH_PCI_EXPRESS_ASPM_CONTROL  DmiAspmCtrl;
  PCH_SERIES                    PchSeries;

  DEBUG ((EFI_D_INFO, "ConfigureDmi() Start\n"));

  PchSeries       = GetPchSeries();
  PciD28F0RegBase = MmPciAddress (0, PchPlatformPolicy->BusNumber, 28, 0, 0);

  ///
  /// PCH BIOS Spec Rev 0.5.0 Section 7.1.5 Additional PCH DMI Programming Steps
  ///
  if (PchSeries == PchH) {
    /// Step 4.1
    /// If RCBA + Offset 2320h[1] = 0 and B0:D28:F0 + F5h[0] = 0, set RCBA + Offset 21A4h[17:15] = 010b
    /// Else set RCBA + Offset 21A4h[17:15] = 100b
    ///
    if (((MmioRead32 ((UINTN) RootComplexBar + R_PCH_RCRB_CIR2320) & (UINT32) (BIT1)) == 0) &&
        ((MmioRead8 (PciD28F0RegBase + 0xF5) & BIT0) == 0)) {
      Data32Or = BIT16;
    } else {
      Data32Or = BIT17;
    }
    ///
    /// Step 4.2
    /// Set RCBA + Offset 21A4h[14:12] = 011b
    ///
    Data32Or |= BIT13 | BIT12;
    Data32And = (UINT32)~(B_PCH_RCRB_LCAP_EL1 | B_PCH_RCRB_LCAP_EL0);
    MmioAndThenOr32 (RootComplexBar + R_PCH_RCRB_LCAP, Data32And, Data32Or);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + R_PCH_RCRB_LCAP),
      &Data32Or,  // Data to be ORed
      &Data32And  // Data to be ANDed
      );
  }
  ///
  /// Step 4.3
  /// Set RCBA + 2348[3:0] = 0h
  ///
  Data32Or  = 0;
  Data32And = (UINT32) ~(BIT0 | BIT1 | BIT2 | BIT3);
  MmioAnd32 (RootComplexBar + 0x2348, Data32And);
  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (RootComplexBar + 0x2348),
    &Data32Or,  // Data to be ORed
    &Data32And  // Data to be ANDed
    );

  ///
  /// Enable DMI ASPM
  ///
  /// PCH BIOS Spec Rev 0.5.0, Section 8.3.1 ASPM on DMI and the PCI Express* Root Ports
  ///
  if (PchPlatformPolicy->DmiConfig->DmiAspm == PCH_DEVICE_ENABLE) {
    ///
    /// While DmiAspm is enabled, DMI ASPM will be set to Intel recommended value.
    /// PCH BIOS Spec Rev 0.5.0, Section 8.3.1 ASPM on DMI and the PCI Express* Root Ports
    /// Note: We recommend PCH platforms to enable L0s and L1, but unless both sides of the link have L0s and/or
    /// L1 enabled they will be disabled by the link.
    ///
        DmiAspmCtrl = PchPcieAspmL0sL1;
  } else {
    DmiAspmCtrl = PchPcieAspmDisabled;
  }

  if (DmiAspmCtrl != PchPcieAspmDisabled) {
    if (PchSeries == PchH) {
      ///
      /// PCH BIOS Spec Rev 0.5.0, Section 8.3.1
      /// BIOS should set RCBA + 2304h[10] to 0b prior to enabling DMI ASPM.
      ///
      Data32And = (UINT32)~(BIT10);
      Data32Or  = 0;
      MmioAnd32 (RootComplexBar + R_PCH_RCRB_DMC, Data32And);
      SCRIPT_MEM_READ_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (RootComplexBar + R_PCH_RCRB_DMC),
        &Data32Or,  // Data to be ORed
        &Data32And  // Data to be ANDed
        );

      ///
      /// PCH BIOS Spec Rev 0.5.0, Section 8.3.1
      ///
      /// Step 1
      /// RCBA + 21A4h[11:10] = 11b
      ///
      Data32And = 0xFFFFFFFF;
      Data32Or  = B_PCH_RCRB_LCAP_APMS;
      MmioOr32 (RootComplexBar + R_PCH_RCRB_LCAP, Data32Or);
      SCRIPT_MEM_READ_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (RootComplexBar + R_PCH_RCRB_LCAP),
        &Data32Or,  // Data to be ORed
        &Data32And  // Data to be ANDed
        );
    }

    if (DmiAspmCtrl == PchPcieAspmL0sL1) {
      ///
      /// Step 2
      /// Enable L0s/L1 on DMI by setting RCBA + offset 21A8h[1:0] to 11b
      ///
      Data16And = (UINT16) (~(BIT1 + BIT0));
      Data16Or  = (UINT16) (BIT1 + BIT0);

    } else {
      //
      // Do nothing
      //
      Data16And = 0xFFFF;
      Data16Or  = 0;
    }
    if (PchSeries == PchH) {
      ///
      /// Program RCBA + offset 21A8h[1:0]
      ///
      MmioAndThenOr16 (RootComplexBar + R_PCH_RCRB_LCTL, Data16And, Data16Or);
      SCRIPT_MEM_READ_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint16,
        (UINTN) (RootComplexBar + R_PCH_RCRB_LCTL),
        &Data16Or,  // Data to be ORed
        &Data16And  // Data to be ANDed
        );
      ///
      /// BIOS should set RCBA + 2304h[10] back to 1b after enabling DMI ASPM.
      ///
      Data32And = 0xFFFFFFFF;
      Data32Or  = (UINT32) (BIT10);
      MmioOr32 (RootComplexBar + R_PCH_RCRB_DMC, Data32Or);
      SCRIPT_MEM_READ_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (RootComplexBar + R_PCH_RCRB_DMC),
        &Data32Or,  // Data to be ORed
        &Data32And  // Data to be ANDed
        );
    }
  }

  if (PchSeries == PchH) {
    if (PchPlatformPolicy->DmiConfig->DmiExtSync == PCH_DEVICE_ENABLE) {
      Data16And = (UINT16) (~(B_PCH_RCRB_LCTL_ES));
      Data16Or  = (UINT16) B_PCH_RCRB_LCTL_ES;
      MmioAndThenOr16 (RootComplexBar + R_PCH_RCRB_LCTL, Data16And, Data16Or);
      SCRIPT_MEM_READ_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint16,
        (UINTN) (RootComplexBar + R_PCH_RCRB_LCTL),
        &Data16Or,  // Data to be ORed
        &Data16And  // Data to be ANDed
        );
    }
  }

  DEBUG ((EFI_D_INFO, "ConfigureDmi() End\n"));

  return EFI_SUCCESS;
}

/**
  Configure deep Sx programming

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] RootComplexBar       RootComplexBar value of this PCH device

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
ProgramDeepSx (
  DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy,
  UINT32                              RootComplexBar
  )
{
  UINT32      S3Data32;
  UINT32      S4Data32;
  UINT32      S5Data32;
  UINTN       PciD31F0RegBase;
  UINT16      LpcDeviceId;

  PciD31F0RegBase = MmPciAddress (0, PchPlatformPolicy->BusNumber, 31, 0, 0);
  LpcDeviceId     = MmioRead16 (PciD31F0RegBase + R_PCH_LPC_DEVICE_ID);

  ///
  /// PCH BIOS Spec Rev 0.5.0, Section 19.11  Deep Sx Power Policies
  /// The System BIOS can perform the following register programming guidelines to enable system
  /// enter Deep S4 or Deep S5.
  ///
  ///                            DPS3_EN_DC       DPS3_EN_AC        DPS4_EN_DC        DPS4_EN_AC        DPS5_EN_DC        DPS5_EN_AC
  ///                           RCBA+3328h[1]   RCBA + 3328h[0]    RCBA + 332Ch[1]   RCBA + 332Ch[0]   RCBA + 3330h[15]  RCBA + 3330h[14]
  /// Deep Sx disabled                 0              0                   0                 0                 0                 0
  ///
  /// Enabled in S5                    0              0                   0                 0                 1                 1
  ///
  /// Enabled in S4 and S5             0              0                   1                 1                 1                 1
  ///
  /// Enabled in S3, S4 and S5         1              1                   1                 1                 1                 1
  ///
  /// Configuration supported by MOBILE:
  /// Enabled in S5                    0              0                   0                 0                 1                 0
  /// (Battery mode)
  /// Enabled in S4 and S5             0              0                   1                 0                 1                 0
  /// (Battery Mode)
  /// Enabled in S3, S4 and S5         1              0                   1                 0                 1                 0
  /// (Battery Mode)
  ///
  /// NOTE: Mobile platforms support Deep S4/S5 in DC ONLY,
  /// Desktop and Intel C206 Chipset (LPC Dev ID 0x1C56) platforms support Deep S4/S5 in AC ONLY,
  /// Intel C204 Chipset (LPC Dev ID 0x1C54) and Intel C202 Chipset (LPC Dev ID 0x1C52) platforms DO NOT support Deep S4/S5.
  ///
  /// Deep Sx disabled                 0              0                   0                 0                 0                 0
  ///
  if (IS_PCH_LPT_LPC_DEVICE_ID_DESKTOP (LpcDeviceId) ||
      IS_PCH_LPT_LPC_DEVICE_ID_SERVER (LpcDeviceId)  ||
      IS_PCH_LPT_LPC_DEVICE_ID_WS (LpcDeviceId)) {
    if ((PchPlatformPolicy->MiscPmConfig->PchDeepSxPol == PchMobileDpS5En) ||
        (PchPlatformPolicy->MiscPmConfig->PchDeepSxPol == PchMobileDpS4S5En) ||
        (PchPlatformPolicy->MiscPmConfig->PchDeepSxPol == PchMobileDpS3S4S5En)) {
      ///
      /// Set PchDeepSxPol to PchDeepSxPolDisable for unsupported deep Sx policy
      ///
      PchPlatformPolicy->MiscPmConfig->PchDeepSxPol = PchDeepSxPolDisable;
      DEBUG ((EFI_D_ERROR, "Unsupported Deep Sx policy for desktop system\n"));
    }

  } else {
    if ((PchPlatformPolicy->MiscPmConfig->PchDeepSxPol == PchDesktopDpS5En) ||
        (PchPlatformPolicy->MiscPmConfig->PchDeepSxPol == PchDesktopDpS4S5En) ||
        (PchPlatformPolicy->MiscPmConfig->PchDeepSxPol == PchDesktopDpS3S4S5En)) {
      ///
      /// Set PchDeepSxPol to PchDeepSxPolDisable for unsupported deep Sx policy
      ///
      PchPlatformPolicy->MiscPmConfig->PchDeepSxPol = PchDeepSxPolDisable;
      DEBUG ((EFI_D_ERROR, "Unsupported Deep Sx policy for mobile system\n"));
    }
  }

  switch (PchPlatformPolicy->MiscPmConfig->PchDeepSxPol) {
  case PchDesktopDpS5En:
    ///
    /// Configuration 2: Enabled in S5/AC-DC
    /// DEEP_S3_POL.DPS3_EN_DC = 0; DEEP_S3_POL.DPS3_EN_AC = 0;
    /// DEEP_S4_POL.DPS4_EN_DC = 0; DEEP_S4_POL.DPS4_EN_AC = 0;
    /// DEEP_S5_POL.DPS5_EN_DC = 1; DEEP_S5_POL.DPS5_EN_AC = 1;
    ///
    S3Data32  = 0;
    S4Data32  = 0;
    S5Data32  = (UINT32) (B_PCH_RCRB_DEEP_S5_POL_DPS5_EN_AC | B_PCH_RCRB_DEEP_S5_POL_DPS5_EN_DC);
    break;

  case PchDesktopDpS4S5En:
    ///
    /// Configuration 4: Enabled only in S4-S5
    /// DEEP_S3_POL.DPS3_EN_DC = 0; DEEP_S3_POL.DPS3_EN_AC = 0;
    /// DEEP_S4_POL.DPS4_EN_DC = 1; DEEP_S4_POL.DPS4_EN_AC = 1;
    /// DEEP_S5_POL.DPS5_EN_DC = 1; DEEP_S5_POL.DPS5_EN_AC = 1;
    ///
    S3Data32  = 0;
    S4Data32  = (UINT32) (B_PCH_RCRB_DEEP_S4_POL_DPS4_EN_AC | B_PCH_RCRB_DEEP_S4_POL_DPS4_EN_DC);
    S5Data32  = (UINT32) (B_PCH_RCRB_DEEP_S5_POL_DPS5_EN_AC | B_PCH_RCRB_DEEP_S5_POL_DPS5_EN_DC);
    break;

  case PchDesktopDpS3S4S5En:
    ///
    /// Configuration 6: Enabled only in S3-S4-S5
    /// DEEP_S3_POL.DPS3_EN_DC = 1; DEEP_S3_POL.DPS3_EN_AC = 1;
    /// DEEP_S4_POL.DPS4_EN_DC = 1; DEEP_S4_POL.DPS4_EN_AC = 1;
    /// DEEP_S5_POL.DPS5_EN_DC = 1; DEEP_S5_POL.DPS5_EN_AC = 1;
    ///
    S3Data32  = (UINT32) (B_PCH_RCRB_DEEP_S3_POL_DPS3_EN_AC | B_PCH_RCRB_DEEP_S3_POL_DPS3_EN_DC);
    S4Data32  = (UINT32) (B_PCH_RCRB_DEEP_S4_POL_DPS4_EN_AC | B_PCH_RCRB_DEEP_S4_POL_DPS4_EN_DC);
    S5Data32  = (UINT32) (B_PCH_RCRB_DEEP_S5_POL_DPS5_EN_AC | B_PCH_RCRB_DEEP_S5_POL_DPS5_EN_DC);
    break;

  case PchMobileDpS5En:
    ///
    /// Configuration 1: Enabled in S5/Battery only
    /// DEEP_S3_POL.DPS3_EN_DC = 0; DEEP_S3_POL.DPS3_EN_AC = 0;
    /// DEEP_S4_POL.DPS4_EN_DC = 0; DEEP_S4_POL.DPS4_EN_AC = 0;
    /// DEEP_S5_POL.DPS5_EN_DC = 1; DEEP_S5_POL.DPS5_EN_AC = 0;
    ///
    S3Data32  = 0;
    S4Data32  = 0;
    S5Data32  = (UINT32) (B_PCH_RCRB_DEEP_S5_POL_DPS5_EN_DC);
    break;

  case PchMobileDpS4S5En:
    ///
    /// Configuration 3: Enabled only in S4-S5/Battery Mode
    /// DEEP_S3_POL.DPS3_EN_DC = 0; DEEP_S3_POL.DPS3_EN_AC = 0;
    /// DEEP_S4_POL.DPS4_EN_DC = 1; DEEP_S4_POL.DPS4_EN_AC = 0;
    /// DEEP_S5_POL.DPS5_EN_DC = 1; DEEP_S5_POL.DPS5_EN_AC = 0;
    ///
    S3Data32  = 0;
    S4Data32  = (UINT32) (B_PCH_RCRB_DEEP_S4_POL_DPS4_EN_DC);
    S5Data32  = (UINT32) (B_PCH_RCRB_DEEP_S5_POL_DPS5_EN_DC);
    break;

  case PchMobileDpS3S4S5En:
    ///
    /// Configuration 5: Enabled only in S4-S5/Battery Mode
    /// DEEP_S3_POL.DPS3_EN_DC = 1; DEEP_S3_POL.DPS3_EN_AC = 0;
    /// DEEP_S4_POL.DPS4_EN_DC = 1; DEEP_S4_POL.DPS4_EN_AC = 0;
    /// DEEP_S5_POL.DPS5_EN_DC = 1; DEEP_S5_POL.DPS5_EN_AC = 0;
    ///
    S3Data32  = (UINT32) (B_PCH_RCRB_DEEP_S3_POL_DPS3_EN_DC);
    S4Data32  = (UINT32) (B_PCH_RCRB_DEEP_S4_POL_DPS4_EN_DC);
    S5Data32  = (UINT32) (B_PCH_RCRB_DEEP_S5_POL_DPS5_EN_DC);
    break;

  case PchDeepSxPolDisable:
  default:
    ///
    /// Configuration 5: DeepSx Disabled
    /// DEEP_S3_POL.DPS3_EN_DC = 0; DEEP_S3_POL.DPS3_EN_AC = 0;
    /// DEEP_S4_POL.DPS4_EN_DC = 0; DEEP_S4_POL.DPS4_EN_AC = 0;
    /// DEEP_S5_POL.DPS5_EN_DC = 0; DEEP_S5_POL.DPS5_EN_AC = 0;
    ///
    S3Data32  = 0;
    S4Data32  = 0;
    S5Data32  = 0;
    break;
  }

  MmioWrite32 ((RootComplexBar + R_PCH_RCRB_DEEP_S3_POL), S3Data32);
  MmioWrite32 ((RootComplexBar + R_PCH_RCRB_DEEP_S4_POL), S4Data32);
  MmioWrite32 ((RootComplexBar + R_PCH_RCRB_DEEP_S5_POL), S5Data32);

  return EFI_SUCCESS;
}
