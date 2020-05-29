/** @file
  SwitchableGraphics Dxe driver.
  This DXE driver loads SwitchableGraphics acpi tables
  for the platform.

@copyright
  Copyright (c) 2010 - 2013 Intel Corporation. All rights reserved.
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

#include "SwitchableGraphicsInit.h"

//[-start-140218-IB10920078-add]//
#include <Protocol/SwitchableGraphicsEvent.h>

#include <Protocol/SetupUtility.h>

#include <Library/CommonPciLib.h>
#include <Library/PciExpressLib.h>
#include <Library/AcpiPlatformLib.h>
#include <Library/DxeServicesLib.h>
#include <ChipsetSetupConfig.h>

NVIDIA_OPREGION       NvidiaOpRegion;
AMD_OPREGION          AmdOpRegion;
OPERATION_REGION_SG   *SgOpRegion;

#define DGPU_DEVICE_NUM   0x00
#define DGPU_FUNCTION_NUM 0x00

//[-end-140218-IB10920078-add]//

extern DXE_PLATFORM_SA_POLICY_PROTOCOL *mDxePlatformSaPolicy;

EFI_BOOT_SERVICES     *gBS;
SA_DATA_HOB           *SaDataHob;

//[-start-140218-IB10920078-remove]//
//EFI_GUID              gSaDataHobGuid = SA_DATA_HOB_GUID;
//[-end-140218-IB10920078-remove]//
EFI_GUID              gSgAcpiTableStorageGuid = SG_ACPI_TABLE_STORAGE_GUID;
EFI_GUID              gSgAcpiTablePchStorageGuid = SG_ACPI_TABLE_PCH_STORAGE_GUID;

VOID                  *VbiosAddress = NULL;
BOOLEAN               DgpuOpRomCopied;
UINT32                VbiosSize;

UINT8                 EndpointBus;
UINT16                GpioBaseAddress;
UINT8                 GpioSupport;

UINT8                 RootPortDev;
UINT8                 RootPortFun;

CPU_FAMILY            CpuFamilyId;

/**
  Initialize the SwitchableGraphics support (DXE).

  @param[in] ImageHandle         - Handle for the image of this driver
  @param[in] DxePlatformSaPolicy - SA DxePlatformPolicy protocol

  @retval EFI_SUCCESS         - SwitchableGraphics initialization complete
  @retval EFI_OUT_OF_RESOURCES - Unable to allocated memory
  @retval EFI_NOT_FOUND        - SA DataHob not found
  @retval EFI_DEVICE_ERROR     - Error Accessing SG GPIO
**/
EFI_STATUS
SwitchableGraphicsInit (
  IN EFI_HANDLE                      ImageHandle,
  IN EFI_SYSTEM_TABLE                *SystemTable,
  IN DXE_PLATFORM_SA_POLICY_PROTOCOL *DxePlatformSaPolicy
  )
{
  EFI_STATUS  Status;
  VOID        *Registration;
  UINTN       PciD31F0RegBase;
  UINT32      RootComplexBar;
  UINT32      RpFn;

  CpuFamilyId = GetCpuFamily();

  if (CpuFamilyId == EnumCpuHswUlt) {
      /// For SwitchableGraphics support the dGPU is present on PCH RootPort
      RootPortDev = PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS;

      PciD31F0RegBase = MmPciAddress (
                          0,
                          DEFAULT_PCI_BUS_NUMBER_PCH,
                          PCI_DEVICE_NUMBER_PCH_LPC,
                          PCI_FUNCTION_NUMBER_PCH_LPC,
                          0
                          );
      RootComplexBar  = MmioRead32 (PciD31F0RegBase + R_PCH_LPC_RCBA) & B_PCH_LPC_RCBA_BAR;
      RpFn = MmioRead32 (RootComplexBar + R_PCH_RCRB_RPFN);
      /// dGPU sits on Root Port 5 [1-based]
      /// Root Port 5 Function Number (RP5FN) = RPFN[18:16]
      RootPortFun = (UINT8) ((RpFn >> (4 * S_PCH_RCRB_PRFN_RP_FIELD)) & B_PCH_RCRB_RPFN_RP1FN);
  } else {
      /// Assume: For SwitchableGraphics support the dGPU is present on PEG RootPort by default
      RootPortDev = SA_PEG10_DEV_NUM;
      RootPortFun = SA_PEG10_FUN_NUM;
  }

  DEBUG ((EFI_D_INFO, "dGPU Rootport info[B/D/F] : [0x00/0x%x/0x%x]\n", RootPortDev, RootPortFun));

  gBS = SystemTable->BootServices;

  ///
  /// Get SG GPIO info from SA HOB.
  ///
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **) &SaDataHob);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SaDataHob = GetNextGuidHob (&gSaDataHobGuid, SaDataHob);
  if (SaDataHob == NULL) {
    return EFI_NOT_FOUND;
  }

  GpioSupport = SaDataHob->SgInfo.SgGpioSupport;

  ///
  /// Read GPIO base
  ///
  GpioBaseAddress = McDevFunPciCfg16 (0, PCI_DEVICE_NUMBER_PCH_LPC, 0, R_PCH_LPC_GPIO_BASE) &~BIT0;
  if (GpioBaseAddress == 0) {
    return EFI_DEVICE_ERROR;
  }

  ///
  /// Update GlobalNvs data for runtime usage
  ///
  Status = UpdateGlobalNvsData (SaDataHob->SgInfo, DxePlatformSaPolicy);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ///
  /// Load Intel SG SSDT tables
  ///
  Status = LoadAcpiTables ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ///
  /// Check to see if Switchable Graphics Mode is enabled
  ///
  if (SaDataHob->SgInfo.SgMode == SgModeMuxless) {
  ///
  /// Create ReadyToBoot callback for SG
  ///
//[-start-140225-IB10920078-modify]//
  EfiCreateProtocolNotifyEvent (
    &gH2OSwitchableGraphicsEventProtocolGuid,
    //&gExitPmAuthProtocolGuid,
    TPL_CALLBACK,
    SgExitPmAuthCallback,
    NULL,
    &Registration
    );
  }
//[-end-140225-IB10920078-modify]//

  return Status;
}

/**
  Initialize the runtime SwitchableGraphics support data for ACPI tables in GlobalNvs.

  @param[in] SaDataHob->SgInfo   - Pointer to Hob for SG system details.
  @param[in] DxePlatformSaPolicy - Pointer to the loaded image protocol for this driver.

  @retval EFI_SUCCESS - The data updated successfully.
**/
EFI_STATUS
UpdateGlobalNvsData (
  IN SG_INFO_HOB                     SgInfo,
  IN DXE_PLATFORM_SA_POLICY_PROTOCOL *DxePlatformSaPolicy
  )
{
  SYSTEM_AGENT_GLOBAL_NVS_AREA_PROTOCOL  *SaGlobalNvsArea;
  UINT8                                  CapOffset;
  UINT16                                 ExtendedCapOffset;
  EFI_STATUS                             Status;
  UINT32                                 Data32;

  ///
  ///  Locate the SA Global NVS Protocol.
  ///
  Status = gBS->LocateProtocol (
                  &gSaGlobalNvsAreaProtocolGuid,
                  NULL,
                  (VOID **) &SaGlobalNvsArea
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ///
  /// SG Mode for ASL usage
  ///
  SaGlobalNvsArea->Area->SgMode               |= SaDataHob->SgInfo.SgMode;
  SaGlobalNvsArea->Area->GpioBaseAddress      = GpioBaseAddress;

  SaGlobalNvsArea->Area->SgGPIOSupport = SaDataHob->SgInfo.SgGpioSupport;

  DEBUG ((EFI_D_INFO, "SG:: Switchable Graphics Mode : 0x%x\n", SaDataHob->SgInfo.SgMode));

  if (SaDataHob->SgInfo.SgMode == SgModeMuxless) {
    ///
    /// SG Feature List for ASL usage
    ///
    //SaGlobalNvsArea->Area->SgFeatureList |= DxePlatformSaPolicy->FeatureList->WirelessDisplay;

    if (SaDataHob->SgInfo.SgGpioSupport) {
      ///
      /// GPIO Assignment for ASL usage
      ///
      SaGlobalNvsArea->Area->SgDgpuPwrOK      = SaDataHob->SgInfo.SgDgpuPwrOK;
      SaGlobalNvsArea->Area->SgDgpuHoldRst    = SaDataHob->SgInfo.SgDgpuHoldRst;
      SaGlobalNvsArea->Area->SgDgpuPwrEnable  = SaDataHob->SgInfo.SgDgpuPwrEnable;
      SaGlobalNvsArea->Area->SgDgpuPrsnt      = SaDataHob->SgInfo.SgDgpuPrsnt;

      DEBUG ((EFI_D_INFO, "SG:: dGPU_PWROK GPIO   GPIO assigned = %d\n", SaDataHob->SgInfo.SgDgpuPwrOK & 0x7f));
      DEBUG ((EFI_D_INFO, "SG:: dGPU_HOLD_RST#    GPIO assigned = %d\n", SaDataHob->SgInfo.SgDgpuHoldRst & 0x7f));
      DEBUG ((EFI_D_INFO, "SG:: dGPU_PWR_EN#      GPIO assigned = %d\n", SaDataHob->SgInfo.SgDgpuPwrEnable & 0x7f));
      DEBUG ((EFI_D_INFO, "SG:: dGPU_PRSNT#       GPIO assigned = %d\n", SaDataHob->SgInfo.SgDgpuPrsnt & 0x7f));
    }

    DEBUG ((EFI_D_INFO, "SG:: VBIOS Configurations:\n"));
    DEBUG (
      (
      EFI_D_INFO, "SG:: Load VBIOS    (0=No Vbios;1=Load VBIOS)         =%d\n", DxePlatformSaPolicy->VbiosConfig->
      LoadVbios
      )
      );
    DEBUG (
      (
      EFI_D_INFO, "SG:: Execute VBIOS (0=Do not execute;1=Execute Vbios)  =%d\n", DxePlatformSaPolicy->VbiosConfig->
      ExecuteVbios
      )
      );
    DEBUG (
      (
      EFI_D_INFO, "SG:: VBIOS Source  (0=PCIE Card;1=FW Volume)         =%d\n", DxePlatformSaPolicy->VbiosConfig->
      VbiosSource
      )
      );

    ///
    /// PEG Endpoint Base Addresses and Capability Structure Offsets for ASL usage
    ///

    ///
    /// Save bus numbers on the PEG/PCH bridge.
    ///
    Data32 = MmPci32 (0, 0, RootPortDev, RootPortFun, PCI_PBUS);
    Data32 &= 0x00FFFF00;

    ///
    /// Set PEG/PCH PortBus = 1 to Read Endpoint.
    ///
    MmPci32AndThenOr (0, 0, RootPortDev, RootPortFun, PCI_PBUS, 0xFF0000FF, 0x00010100);

    ///
    /// A config write is required in order for the device to re-capture the Bus number,
    /// according to PCI Express Base Specification, 2.2.6.2
    /// Write to a read-only register VendorID to not cause any side effects.
    ///
    McDevFunPciCfg16 (1, 0, 0, PCI_VID) = 0;

    EndpointBus = MmPci8 (0, 0, RootPortDev, RootPortFun, PCI_SBUS);

    if (EndpointBus != 0xFF) {
      SaGlobalNvsArea->Area->CapStrPresence = 0;

      CapOffset = (UINT8) PcieFindCapId (EndpointBus, 0, 0, PEG_CAP_ID);
      SaGlobalNvsArea->Area->EndpointPcieCapOffset = CapOffset;
      DEBUG ((EFI_D_INFO, "SG:: Endpoint PCI Express Capability Offset : 0x%x\n", SaGlobalNvsArea->Area->EndpointPcieCapOffset));

      ExtendedCapOffset = (UINT16) PcieFindExtendedCapId (EndpointBus, 0, 0, PEG_CAP_VER);
      if (ExtendedCapOffset != 0) {
        SaGlobalNvsArea->Area->CapStrPresence |= BIT0;
        SaGlobalNvsArea->Area->EndpointVcCapOffset = ExtendedCapOffset;
        DEBUG ((EFI_D_INFO, "SG:: Endpoint Virtual Channel Capability Offset : 0x%x\n", SaGlobalNvsArea->Area->EndpointVcCapOffset));
      }
   }

    ///
    /// Restore bus numbers on the PEG/PCH bridge.
    ///
    MmPci32AndThenOr (0, 0, RootPortDev, RootPortFun, PCI_PBUS, 0xFF0000FF, Data32);
  } else {
    DEBUG ((EFI_D_ERROR, "SG:: Switchable Graphics Mode disabled!!!\n"));
    Status = EFI_LOAD_ERROR;
  }

  return Status;
}

/**
  Load and execute the dGPU VBIOS.

  @param[in] VbiosConfig - Pointer to VbiosConfig policy for Load/Execute and VBIOS Source.
      LoadVbios    - 0 = Do Not Load   ; 1 = Load VBIOS
      ExecuteVbios - 0 = Do Not Execute; 1 = Execute VBIOS
      VbiosSource  - 0 = PCIE Device   ; 1 = FirmwareVolume => TBD

  @retval EFI_SUCCESS     - Load and execute successful.
  @exception EFI_UNSUPPORTED - Secondary VBIOS not loaded.
**/
EFI_STATUS
LoadAndExecuteDgpuVbios (
  IN SA_SG_VBIOS_CONFIGURATION    *VbiosConfig
  )
{
  EFI_HANDLE                *HandleBuffer;
  UINTN                     HandleCount;
  UINTN                     Index;
  VBIOS_PCIR_STRUCTURE      *PcirBlockPtr;
  EFI_STATUS                Status;
  EFI_PCI_IO_PROTOCOL       *PciIo;
  VBIOS_OPTION_ROM_HEADER   *VBiosRomImage;
  EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios;
  EFI_IA32_REGISTER_SET     RegSet;
  EFI_PHYSICAL_ADDRESS      ImageLocation;
  UINTN                     Offset;

  HandleBuffer = NULL;
  DgpuOpRomCopied = FALSE;

  DEBUG ((EFI_D_INFO, "SG:: LoadAndExecuteDgpuVbios\n"));

  ///
  /// Endpoint Device Bus#
  ///
  EndpointBus = MmPci8 (0, 0, RootPortDev, RootPortFun, PCI_SBUS);

  ///
  ///  Endpoint Device Not found
  ///
  if (EndpointBus == 0xFF) {
    DEBUG ((EFI_D_ERROR, "SG:: 0x00/0x%x/0x%x Rootport's Endpoint Device Not found\n", RootPortDev, RootPortFun));
    return EFI_UNSUPPORTED;
  }

  ///
  /// Check Policy setting for loading VBIOS
  ///
  if (VbiosConfig->LoadVbios != 0) {

    DEBUG ((EFI_D_INFO, "SG:: Start to load dGPU VBIOS if available\n"));

    ///
    /// Set as if an umcompressed video BIOS image was not obtainable.
    ///
    VBiosRomImage = NULL;

    ///
    /// Get all PCI IO protocols
    ///
    Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gEfiPciIoProtocolGuid,
                    NULL,
                    &HandleCount,
                    &HandleBuffer
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    ///
    /// Find the video BIOS by checking each PCI IO handle for DGPU video
    /// BIOS OPROM.
    ///
    for (Index = 0; Index < HandleCount; Index++) {
      Status = gBS->HandleProtocol (
                      HandleBuffer[Index],
                      &gEfiPciIoProtocolGuid,
                      (VOID **) &PciIo
                      );
      if (EFI_ERROR (Status) || (PciIo->RomImage == NULL)) {
        ///
        /// If this PCI device doesn't have a ROM image, skip to the next device.
        ///
        continue;
      }

      VBiosRomImage = PciIo->RomImage;

      ///
      /// Get pointer to PCIR structure
      ///
      PcirBlockPtr = (VBIOS_PCIR_STRUCTURE *) ((UINTN) VBiosRomImage + VBiosRomImage->PcirOffset);

      ///
      /// Check if we have an video BIOS OPROM for DGPU.
      ///
      if ((VBiosRomImage->Signature == OPTION_ROM_SIGNATURE) &&
          (McDevFunPciCfg16 (EndpointBus, 0, 0, PCI_VID) == PcirBlockPtr->VendorId) &&
          (PcirBlockPtr->ClassCode[2] == 0x03)
          ) {

        DEBUG ((EFI_D_INFO, "SG:: Loading dGPU VBIOS...\n"));

        ///
        /// Allocate space for copying Oprom
        ///
        VbiosSize = (PcirBlockPtr->ImageLength) * 512;
        Status    = (gBS->AllocatePool) (EfiBootServicesData, VbiosSize, &VbiosAddress);
        if (EFI_ERROR (Status)) {
          break;
        }
	  
        ///
        /// Execute VBIOS based on Policy setting
        ///
        if (VbiosConfig->ExecuteVbios) {
          DEBUG ((EFI_D_INFO, "SG:: Execute dGPU VBIOS...\n"));
          ///
          /// Memory IO Bus Master needs to be enabled when we execute the vbios
          ///
          ///
          /// Enable Memory Access, IO Access Bus Master enable on PEG/PCH ROOT PORT
          ///
          MmPci16Or (0, 0, RootPortDev, RootPortFun, PCI_CMD, BIT0 + BIT1 + BIT2);

          ///
          /// Enable Memory Access, IO Access Bus Master enable and Rom Enable on Peg/PCH Endpoint device
          ///
          McDevFunPciCfg16Or (EndpointBus, 0, 0, PCI_CMD, BIT0 + BIT1 + BIT2);

          ///
          /// Allocate 64kb under 1MB memory region
          ///
          Status = AllocateLegacyMemory (
                    AllocateMaxAddress,
                    CONVENTIONAL_MEMORY_TOP,
                    (BIN_FILE_SIZE_MAX / 4096),
                    &ImageLocation
                    );
          if (!EFI_ERROR (Status)) {
            (gBS->SetMem) ((VOID *) (UINTN) ImageLocation, BIN_FILE_SIZE_MAX, 0);

            ///
            /// After allocation copy VBIOS to buffer
            ///
            (gBS->CopyMem) ((VOID *) (UINTN) ImageLocation, PciIo->RomImage, VbiosSize);

            Status = gBS->LocateProtocol (
                            &gEfiLegacyBiosProtocolGuid,
                            NULL,
                            (VOID **) &LegacyBios
                            );
            if (!EFI_ERROR (Status)) {
              (gBS->SetMem) (&RegSet, sizeof (EFI_IA32_REGISTER_SET), 0);

              RegSet.H.AH = MmPci8 (0, 0, RootPortDev, RootPortFun, PCI_SBUS);
              Offset      = MemoryRead16 ((UINTN) ImageLocation + 0x40);
//[-start-140311-IB10920078-modify]//
              if ((MmPci16 (0, EndpointBus, 0, 0, PCI_VID) == AMD_VID)) {
                LegacyBios->FarCall86 (
                              LegacyBios,
                              ((UINT16) RShiftU64 ((ImageLocation & 0x000FFFF0),
                            4)),
                              ((UINT16) Offset),
                              &RegSet,
                              NULL,
                              0
                              );
              }                
//[-end-140311-IB10920078-modify]//

              Offset = MemoryRead16 ((UINTN) ImageLocation + 0x42) + (UINTN) ImageLocation;
              if (MemoryRead16 ((UINTN) ImageLocation + 0x44) == 0x0) {
                VbiosSize = MemoryRead8 ((UINTN) ImageLocation + 0x2) * 512;
              } else {
                VbiosSize = MemoryRead16 ((UINTN) ImageLocation + 0x44) * 512;
              }
              ///
              /// Copy Oprom to allocated space for the following scenario:
              /// # Load vbios and Execute vbios policy setting
              ///
              DEBUG ((EFI_D_INFO, "Copy Oprom to allocated space: Load & Execute policy satisfied\n"));
              (gBS->CopyMem) (VbiosAddress, (VOID *) Offset, VbiosSize);
              DgpuOpRomCopied = TRUE;
              (gBS->SetMem) ((VOID *) (UINTN) ImageLocation, BIN_FILE_SIZE_MAX, 0);
            }
            (gBS->FreePages) (ImageLocation, (BIN_FILE_SIZE_MAX / 4096));
          }

          ///
          /// Disable Memory Access, IO Access Bus Master enable and Rom Enable on PEG/PCH Endpoint device
          ///
          McDevFunPciCfg16And (EndpointBus, 0, 0, PCI_CMD, ~(BIT0 + BIT1 + BIT2));

          ///
          /// Disable Memory Access, IO Access Bus Master enable on PEG/PCH Root Port
          ///
          MmPci16And (0, 0, RootPortDev, RootPortFun, PCI_CMD, ~(BIT0 + BIT1 + BIT2));
        } 
		
        ///
        /// Copy Oprom to allocated space  for the following scenario:
        /// # Load vbios and Execute vbios policy setting in which dGPU execution is not called
        /// # Load vbios but don't Execute vbios policy setting
        ///
        if ((VbiosAddress!=NULL) && (!DgpuOpRomCopied)) {
          DEBUG ((EFI_D_INFO, "Copy Oprom to allocated space: Load policy satisfied\n"));
          (gBS->CopyMem) (VbiosAddress, PciIo->RomImage, VbiosSize);
          DgpuOpRomCopied = TRUE;
        }

        break;
      }
    }

  }

//[-start-140218-IB10920078-add]//
  Status = LoadTpvAcpiTables ();  
//[-end-140218-IB10920078-add]//

  if (VbiosAddress!=NULL) {
    (gBS->FreePool) (VbiosAddress);
  }

  if (HandleBuffer!=NULL) {
    (gBS->FreePool) (HandleBuffer);
  }

  return EFI_SUCCESS;
}

/**
  Read SG GPIO value

  @param[in] Value - PCH GPIO number and Active value
      Bit0 to Bit7    - PCH GPIO Number
      Bit8            - GPIO Active value (0 = Active Low; 1 = Active High)

  @retval GPIO read value.
**/
BOOLEAN
GpioRead (
  IN UINT8 Value
  )
{
  BOOLEAN Active;
  UINT32  Data;
  UINT16  BitOffset=0;
  UINT16  Offset=0;

  ///
  /// Check if SG GPIOs are supported
  ///
  if (GpioSupport == 0) {
    return FALSE;
  }
  ///
  /// Extract GPIO number and Active value
  ///
  Active = (BOOLEAN) (Value >> 7);
  Value &= 0x7F;

  if (CpuFamilyId == EnumCpuHswUlt) {
      Offset    = R_PCH_GP_N_CONFIG0 + (Value * 0x08);
      BitOffset = 30; //GPI_LVL
  } else {
    if (Value < 0x20) {
      Offset    = R_PCH_GPIO_LVL;
      BitOffset = Value;
    } else if (Value < 0x40) {
      Offset    = R_PCH_GPIO_LVL2;
      BitOffset = Value - 0x20;
    } else {
      Offset    = R_PCH_GPIO_LVL3;
      BitOffset = Value - 0x40;
    }
  }

  ///
  /// Read specified value GPIO
  ///
  Data = IoRead32 (GpioBaseAddress + Offset);
  Data >>= BitOffset;

  if (Active == 0) {
    Data = ~Data;
  }

  return (BOOLEAN) (Data & 0x1);
}

/**
  Write SG GPIO value

  @param[in] Value - PCH GPIO number and Active value
      Bit0 to Bit7    - PCH GPIO Number
      Bit8            - GPIO Active value (0 = Active Low; 1 = Active High)
  @param[in] Level - Write data (0 = Disable; 1 = Enable)
**/
VOID
GpioWrite (
  IN UINT8   Value,
  IN BOOLEAN Level
  )
{
  BOOLEAN Active;
  UINT32  Data;
  UINT16  BitOffset=0;
  UINT16  Offset=0;

  ///
  /// Check if SG GPIOs are supported
  ///
  if (GpioSupport == 0) {
    return ;
  }

  Active = (BOOLEAN) (Value >> 7);
  Value &= 0x7F;

  if (Active == 0) {
    Level = (~Level) & 0x1;
  }

  if (CpuFamilyId == EnumCpuHswUlt) {
      Offset    = R_PCH_GP_N_CONFIG0 + (Value * 0x08);
      BitOffset = 31; //GPO_LVL
  } else {
    if (Value < 0x20) {
      Offset    = R_PCH_GPIO_LVL;
      BitOffset = Value;
    } else if (Value < 0x40) {
      Offset    = R_PCH_GPIO_LVL2;
      BitOffset = Value - 0x20;
    } else {
      Offset    = R_PCH_GPIO_LVL3;
      BitOffset = Value - 0x40;
    }
  }

  Data = IoRead32 (GpioBaseAddress + Offset);
  Data &= ~(0x1 << BitOffset);
  Data |= (Level << BitOffset);
  IoWrite32 (GpioBaseAddress + Offset, Data);

  return ;
}

/**
  Do an AllocatePages () of type AllocateMaxAddress for EfiBootServicesCode
  memory.

  @param[in] AllocateType     - Allocated Legacy Memory Type
  @param[in] StartPageAddress - Start address of range
  @param[in] Pages            - Number of pages to allocate
  @param[in, out] Result      - Result of allocation

  @retval EFI_SUCCESS - Legacy16 code loaded
  @retval Other       - No protocol installed, unload driver.
**/
EFI_STATUS
AllocateLegacyMemory (
  IN  EFI_ALLOCATE_TYPE         AllocateType,
  IN  EFI_PHYSICAL_ADDRESS      StartPageAddress,
  IN  UINTN                     Pages,
  IN OUT  EFI_PHYSICAL_ADDRESS  *Result
  )
{
  EFI_STATUS            Status;
  EFI_PHYSICAL_ADDRESS  MemPage;

  ///
  /// Allocate Pages of memory less <= StartPageAddress
  ///
  MemPage = (EFI_PHYSICAL_ADDRESS) (UINTN) StartPageAddress;
  Status  = (gBS->AllocatePages) (AllocateType, EfiBootServicesCode, Pages, &MemPage);

  ///
  /// Do not ASSERT on Status error but let caller decide since some cases
  /// memory is already taken but that is ok.
  ///
  if (!EFI_ERROR (Status)) {
    *Result = (EFI_PHYSICAL_ADDRESS) (UINTN) MemPage;
  }

  return Status;
}

/**
  Load Intel SG SSDT Tables

  @param[in] None

  @retval EFI_SUCCESS - SG SSDT Table load successful.
**/
EFI_STATUS
LoadAcpiTables (
  VOID
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  BOOLEAN                       LoadTable;
  UINTN                         NumberOfHandles;
  UINTN                         Index;
  INTN                          Instance;
  UINTN                         Size;
  UINT32                        FvStatus;
  UINTN                         TableHandle;
  EFI_GUID                      AcpiTableGuid;
  EFI_FV_FILETYPE               FileType;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  EFI_FIRMWARE_VOLUME_PROTOCOL  *FwVol;
  EFI_ACPI_TABLE_PROTOCOL       *AcpiTable;
  EFI_ACPI_TABLE_VERSION        Version;
  EFI_ACPI_DESCRIPTION_HEADER   *TableHeader;
  EFI_ACPI_COMMON_HEADER        *Table;

  FwVol         = NULL;
  Table         = NULL;

  AcpiTableGuid = gSgAcpiTableStorageGuid;
  if (CpuFamilyId == EnumCpuHswUlt) {
    AcpiTableGuid = gSgAcpiTablePchStorageGuid;
  }

  DEBUG ((EFI_D_INFO, "SG:: Loading ACPI Tables...\n"));

  ///
  /// Locate FV protocol.
  ///
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolumeProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Look for FV with ACPI storage file
  ///
  for (Index = 0; Index < NumberOfHandles; Index++) {
    ///
    /// Get the protocol on this handle
    /// This should not fail because of LocateHandleBuffer
    ///
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiFirmwareVolumeProtocolGuid,
                    (VOID **) &FwVol
                    );
    ASSERT_EFI_ERROR (Status);
    if (FwVol == NULL) {
      return EFI_NOT_FOUND;
    }
    ///
    /// See if it has the ACPI storage file
    ///
    Size      = 0;
    FvStatus  = 0;
    Status = FwVol->ReadFile (
                      FwVol,
                      &AcpiTableGuid,
                      NULL,
                      &Size,
                      &FileType,
                      &Attributes,
                      &FvStatus
                      );

    ///
    /// If we found it, then we are done
    ///
    if (!EFI_ERROR (Status)) {
      break;
    }
  }
  ///
  /// Our exit status is determined by the success of the previous operations
  /// If the protocol was found, Instance already points to it.
  ///
  ///
  /// Free any allocated buffers
  ///
  (gBS->FreePool) (HandleBuffer);

  ///
  /// Sanity check that we found our data file
  ///
  ASSERT (FwVol);

  ///
  /// By default, a table belongs in all ACPI table versions published.
  ///
  Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;

  ///
  /// Locate ACPI tables
  ///
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &AcpiTable);

  ///
  /// Read tables from the storage file.
  ///
  if (FwVol == NULL) {
    ASSERT_EFI_ERROR (EFI_NOT_FOUND);
    return EFI_NOT_FOUND;
  }
  Instance = 0;

  while (Status == EFI_SUCCESS) {
    ///
    /// Read the ACPI tables
    ///
    Status = FwVol->ReadSection (
                      FwVol,
                      &AcpiTableGuid,
                      EFI_SECTION_RAW,
                      Instance,
                      (VOID **) &Table,
                      &Size,
                      &FvStatus
                      );
    if (!EFI_ERROR (Status)) {
      ///
      /// check and load SwitchableGraphics SSDT table
      ///
      LoadTable   = FALSE;
      TableHeader = (EFI_ACPI_DESCRIPTION_HEADER *) Table;

      if (((EFI_ACPI_DESCRIPTION_HEADER *) TableHeader)->OemTableId == EFI_SIGNATURE_64 (
          'S',
          'g',
          'P',
          'e',
          'g',
          0,
          0,
          0
          )
          ) {
        ///
        /// This is SG SSDT [dGPU is present on PEG RootPort]
        ///
        DEBUG ((EFI_D_INFO, "SG:: ---- SG SSDT ----\n"));
        DEBUG ((EFI_D_INFO, "SG:: Found out SSDT:SgPeg [SgSsdt.asl]. dGPU is present on PEG RootPort.\n"));
        LoadTable = TRUE;
      } 
	  
	  if (((EFI_ACPI_DESCRIPTION_HEADER *) TableHeader)->OemTableId == EFI_SIGNATURE_64 (
          'S',
          'g',
          'P',
          'c',
          'h',
          0,
          0,
          0
          )
          ) {
        ///
        /// This is SG SSDT [dGPU is present on PCH RootPort]
        ///
        DEBUG ((EFI_D_INFO, "SG:: ---- SG SSDT ----\n"));
        DEBUG ((EFI_D_INFO, "SG:: Found out SSDT:SgPch [SgSsdtPch.asl]. dGPU is present on PCH RootPort.\n"));
        LoadTable = TRUE;
      }

      ///
      /// Add the table
      ///
      if (LoadTable) {
        TableHandle = 0;
        Status = AcpiTable->InstallAcpiTable (
                                  AcpiTable,
                                  TableHeader,
                                  TableHeader->Length,
                                  &TableHandle
                                  );
      }
      ///
      /// Increment the instance
      ///
      Instance++;
      Table = NULL;
    }
  }

  return EFI_SUCCESS;
}


VOID
EFIAPI
SgExitPmAuthCallback (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  EFI_STATUS  Status;
  VOID        *ProtocolPointer;

  ///
  /// Check if this is first time called by EfiCreateProtocolNotifyEvent() or not,
  /// if it is, we will skip it until real event is triggered
  ///
//[-start-140225-IB10920078-modify]//
  Status = gBS->LocateProtocol (&gH2OSwitchableGraphicsEventProtocolGuid, NULL, (VOID **)&ProtocolPointer);
  //Status = gBS->LocateProtocol (&gExitPmAuthProtocolGuid, NULL, (VOID **)&ProtocolPointer);
//[-end-140225-IB10920078-modify]//
  if (EFI_SUCCESS != Status) {
     return;
  }

  gBS->CloseEvent (Event);

  DEBUG ((EFI_D_INFO, "SG:: ExitPmAuth Callback\n"));
  ///
  /// Load and Execute dGPU VBIOS
  ///
  Status = LoadAndExecuteDgpuVbios (mDxePlatformSaPolicy->VbiosConfig);
}


/**
  Load Third part graphics vendor support SSDT Tables

  @param[in] None

  @retval EFI_SUCCESS     - SSDT Table load successful.
  @exception EFI_UNSUPPORTED - Supported SSDT not found.
**/
EFI_STATUS
LoadTpvAcpiTables (
  VOID
  )
{
#ifdef SG_SUPPORT
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  EFI_GUID                      AcpiTableGuid;
  BOOLEAN                       LoadTable;
  INTN                          Instance;
  UINTN                         NumberOfHandles;
  UINTN                         Index;
  UINTN                         Size;
  UINTN                         TableHandle;
  UINT16                        Data16;
  UINT32                        Data32;
  UINT32                        FvStatus;
  EFI_FIRMWARE_VOLUME_PROTOCOL  *FwVol;
  EFI_FV_FILETYPE               FileType;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  EFI_ACPI_DESCRIPTION_HEADER   *TableHeader;
  EFI_ACPI_TABLE_VERSION        Version;
  EFI_ACPI_TABLE_PROTOCOL       *AcpiTable;
  EFI_ACPI_COMMON_HEADER        *Table;

  FwVol = NULL;
  Table = NULL;

  DEBUG ((EFI_D_INFO, "SG:: Load 'Third Party graphics Vendor' support SSDT Tables [LoadTpvAcpiTables] ...\n"));

	if ((MmPci16 (0, EndpointBus, 0, 0, PCI_VID) == NVIDIA_VID)) {
    //
    // If PCH-dGPU is NVIDIA and supports HG set AcpiTableGuid to Nvidia's
    //
      DEBUG ((EFI_D_INFO, "SG:: Found a NVIDIA PCIE graphics card\n"));
      AcpiTableGuid = gNvidiaAcpiTablePchGuid;
	} else if ((MmPci16 (0, EndpointBus, 0, 0, PCI_VID) == AMD_VID)) {
    //
    // If PCH-dGPU is AMD and supports HG set AcpiTableGuid to Amd's
    //
      DEBUG ((EFI_D_INFO, "SG:: Found a AMD PCIE graphics card\n"));
      AcpiTableGuid = gAmdAcpiTablePchGuid;
    } else {
    //
    // either means the Device ID is not on the list of devices we know to ensure
    // no ACPI info gets leakout we return from this function
    //
      DEBUG ((EFI_D_INFO, "SG:: Found a PCIE graphics card [not NVIDIA/not AMD]\n"));
#endif
      return EFI_UNSUPPORTED;
#ifdef SG_SUPPORT
    }
  //
  // Locate FV protocol.
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolumeProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Look for FV with ACPI storage file
  ///
  for (Index = 0; Index < NumberOfHandles; Index++) {
    ///
    /// Get the protocol on this handle
    /// This should not fail because of LocateHandleBuffer
    ///
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiFirmwareVolumeProtocolGuid,
                    &FwVol
                    );
    ASSERT_EFI_ERROR (Status);
    if (FwVol == NULL) {
       return EFI_NOT_FOUND;
    }

    ///
    /// See if it has the ACPI storage file
    ///
    Size      = 0;
    FvStatus  = 0;
    Status = FwVol->ReadFile (
                      FwVol,
                      &AcpiTableGuid,
                      NULL,
                      &Size,
                      &FileType,
                      &Attributes,
                      &FvStatus
                      );

    ///
    /// If we found it, then we are done
    ///
    if (!EFI_ERROR (Status)) {
      break;
    }
  }
  ///
  /// Our exit status is determined by the success of the previous operations
  /// If the protocol was found, Instance already points to it.
  ///
  ///
  /// Free any allocated buffers
  ///
  (gBS->FreePool) (HandleBuffer);

  ///
  /// Sanity check that we found our data file
  ///
  if (FwVol == NULL) {
      ASSERT_EFI_ERROR (EFI_NOT_FOUND);
      return EFI_NOT_FOUND;
  }

  ///
  /// By default, a table belongs in all ACPI table versions published.
  ///
  Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;

  ///
  /// Locate ACPI tables
  ///
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, &AcpiTable);

  ///
  /// Read tables from the storage file.
  ///
  Instance = 0;

  while (Status == EFI_SUCCESS) {
    ///
    /// Read the ACPI tables
    ///
    Status = FwVol->ReadSection (
                      FwVol,
                      &AcpiTableGuid,
                      EFI_SECTION_RAW,
                      Instance,
                      &Table,
                      &Size,
                      &FvStatus
                      );
    if (!EFI_ERROR (Status)) {
      ///
      /// check for SwitchableGraphics tables and decide which SSDT should be loaded
      ///
      LoadTable   = FALSE;
      TableHeader = (EFI_ACPI_DESCRIPTION_HEADER *) Table;

      switch (((EFI_ACPI_DESCRIPTION_HEADER *) TableHeader)->OemTableId) {

      case EFI_SIGNATURE_64 ('N', 'v', 'd', 'P', 'c', 'h', 0, 0):
        ///
        /// This is Nvidia SSDT
        ///
        DEBUG ((EFI_D_INFO, "SG:: ---- Nvidia SSDT ----\n"));

        if ((((EFI_ACPI_DESCRIPTION_HEADER *) TableHeader)->OemTableId) == EFI_SIGNATURE_64 ('N', 'v', 'd', 'P', 'c', 'h', 0, 0)) {
          DEBUG ((EFI_D_INFO, "SG:: Found out SSDT:NvdPch [NvsgPch.asl]. dGPU is present on PCH RootPort.\n"));
        }

        LoadTable = TRUE;
        Status    = InstallNvidiaOpRegion ();
        if (EFI_ERROR (Status)) {
          return Status;
        }     
        break;

      case EFI_SIGNATURE_64 ('A', 'm', 'd', 'P', 'c', 'h', 0, 0):
        ///
        /// This is Amd SSDT
        ///
        DEBUG ((EFI_D_INFO, "SG:: ---- AMD SSDT ----\n"));

        if ((((EFI_ACPI_DESCRIPTION_HEADER *) TableHeader)->OemTableId) == EFI_SIGNATURE_64 ('A', 'm', 'd', 'P', 'c', 'h', 0, 0)) {
          DEBUG ((EFI_D_INFO, "SG:: Found out SSDT:AmdPch [AmdpxPch.asl]. dGPU is present on PCH RootPort.\n"));
        }

        LoadTable = TRUE;
        Status    = InstallAmdOpRegion ();
        if (EFI_ERROR (Status)) {
          return Status;
        }
        ///
        /// Store the Root port Bus assignment for S3 resume path
        ///
		Data32 = MmPci32 (0, 0, RootPortDev, RootPortFun, PCI_PBUS);
    	S3BootScriptSaveMemWrite (
      		S3BootScriptWidthUint32,
      		(UINTN) (MmPciAddress (0x0,
                  0,
                  RootPortDev,
                  RootPortFun,
                  PCI_PBUS)),
      			  1,
      			  &Data32
      		);

        Data16 = MmPci16 (0, 0, RootPortDev, RootPortFun, PCI_BAR3);
        S3BootScriptSaveMemWrite (
        	S3BootScriptWidthUint16,
        	(UINTN) (MmPciAddress (0x0,
                  0,
                  RootPortDev,
                  RootPortFun,
                  PCI_BAR3)),
      			1,
      			&Data16
      			);

        ///
        /// Set a unique SSID on the AMD MXM
        ///
        Data16  = McD2PciCfg16 (PCI_SVID);
		MmPci16 (0, EndpointBus, 0, 0, AMD_SVID_OFFSET) = Data16;
    		S3BootScriptSaveMemWrite (
      		S3BootScriptWidthUint16,
          (UINTN) (MmPciAddress (0x0,
                  EndpointBus,
                  0,
                  0,
                  AMD_SVID_OFFSET)),
      			1,
      			&Data16
     			);
        DEBUG ((EFI_D_INFO, "SG:: AMD MXM SVID [Subsystem Vendor ID]: 0x%x\n", Data16));

        Data16  = McD2PciCfg16 (PCI_SID);
		MmPci16 (0, EndpointBus, 0, 0, AMD_SDID_OFFSET) = Data16;
 	   	S3BootScriptSaveMemWrite (
      		S3BootScriptWidthUint16,
          	(UINTN) (MmPciAddress (0x0,
                  EndpointBus,
                  0,
                  0,
                  AMD_SDID_OFFSET)),
      			1,
      			&Data16
      			);
        DEBUG ((EFI_D_INFO, "SG:: AMD MXM SDID [Subsystem ID]: 0x%x\n", Data16));

        break;

      default:
        break;
      }
      //
      // Add the table
      //
      if (LoadTable) {
//[-start-140213-IB10920077-add]//
        Status = InitializeOpRegion (TableHeader);
        if (EFI_ERROR (Status)) {
          return Status;
        }
//[-end-140213-IB10920077-add]//        
        TableHandle = 0;
        Status = AcpiTable->InstallAcpiTable (
                                  AcpiTable,
                                  TableHeader,
                                  TableHeader->Length,
                                  &TableHandle
                                  );
		DEBUG ((EFI_D_INFO, "SG:: Installed ACPI Table\n"));
      }
      ///
      /// Increment the instance
      ///
      Instance++;
      Table = NULL;
    }
  }

  return EFI_SUCCESS;
#endif
}

/**

  Initialize Switchable Graphics own operation region for ASL (ACPI Source Language) code usage.

  @param[in] NvStoreTable    A pointer of temporary SG SSDT that prepares modified and installed.
  @param[in] SgDxeInfoData   A pointer of SG DXE information data structure,
                             this driver or SG Operation Region will use these data.
  @param[in] SgInfoDataHob   A pointer of SG information data HOB,
                             SG PEI Module created this HOB, and passes the data from PEI phase.

  @retval EFI_SUCCESS    Load Switchable Graphics SSDT successfully.
  @retval !EFI_SUCCESS   Load Switchable Graphics SSDT failed.

**/
EFI_STATUS
InitializeOpRegion (
  IN EFI_ACPI_DESCRIPTION_HEADER  *NvStoreTable
  )
{
  EFI_ACPI_DESCRIPTION_HEADER                 *Table;
  UINT8                                       *SsdtPointer;
  UINT32                                      *Signature;

  Table = NvStoreTable;
  for (SsdtPointer =  (UINT8 *)((UINTN)Table + sizeof (EFI_ACPI_DESCRIPTION_HEADER)); \
       SsdtPointer <= (UINT8 *)((UINTN)Table + (UINTN)(Table->Length)); SsdtPointer++) {
    Signature = (UINT32*)SsdtPointer;
    switch (*Signature) {
      case (SIGNATURE_32 ('S', 'G', 'O', 'P')): // SG operation region initialize
        if (IsAmlOpRegionObject (SsdtPointer)) {
            UpdateSgOpRegion ();
            SetOpRegion (SsdtPointer, SgOpRegion, sizeof (OPERATION_REGION_SG));          
        }
      break;
      
      case (SIGNATURE_32 ('N', 'V', 'I', 'G')): // SG operation region initialize
        if (IsAmlOpRegionObject (SsdtPointer)) {
          SetOpRegion (SsdtPointer, NvidiaOpRegion.NvIgOpRegion, sizeof (NVIG_OPREGION));
        }
      break;
      
      case (SIGNATURE_32 ('N', 'V', 'H', 'M')): // VBIOS operation region initialize
        if (IsAmlOpRegionObject (SsdtPointer)) {
          SetOpRegion (SsdtPointer, NvidiaOpRegion.NvHmOpRegion, sizeof (NVHM_OPREGION));
        }
      break;
      
      case (SIGNATURE_32 ('A', 'P', 'X', 'M')): // VBIOS operation region initialize
        if (IsAmlOpRegionObject (SsdtPointer)) {
          SetOpRegion (SsdtPointer, AmdOpRegion.ApXmOpRegion, sizeof (APXM_OPREGION));
        }
      break;
      
    }
    AcpiChecksum (Table, Table->Length, ((UINTN)(&(((EFI_ACPI_DESCRIPTION_HEADER *)0)->Checksum))));
  }

  return EFI_SUCCESS;
}

/**

  Update Switchable Graphics own operation region for ASL (ACPI Source Language) code usage.

  @param[in] SgOpRegion      A pointer of SG own operation region.
  @param[in] SgDxeInfoData   A pointer of SG DXE information data structure,
                             this driver or SG Operation Region will use these data.
  @param[in] SgInfoDataHob   A pointer of SG information data HOB,
                             SG PEI Module created this HOB, and passes the data from PEI phase.

  @retval None.

**/
EFI_STATUS
UpdateSgOpRegion (
  VOID
  )
{
  EFI_STATUS                             Status;                  
  SYSTEM_AGENT_GLOBAL_NVS_AREA_PROTOCOL  *SaGlobalNvsArea;

  UINT8                                  CapOffset;
  UINTN                                  PciAddress;
  UINT8                                  DgpuBridgeBus = 0;
  UINT8                                  DgpuBridgeDevice = 1;
  UINT8                                  DgpuBridgeFunction = 0;
  UINT8                                  MasterDgpuBus;
  ///
  ///  Locate the SA Global NVS Protocol.
  ///
  Status = gBS->LocateProtocol (
                  &gSaGlobalNvsAreaProtocolGuid,
                  NULL,
                  (VOID **) &SaGlobalNvsArea
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->AllocatePool (EfiACPIMemoryNVS, sizeof (OPERATION_REGION_SG), (VOID **)&SgOpRegion);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ZeroMem (SgOpRegion, sizeof (OPERATION_REGION_SG));

  //
  // SG Mode for ASL usage
  //
  SgOpRegion->SgMode               = SaGlobalNvsArea->Area->SgMode;
  SgOpRegion->GpioBaseAddress      = SaGlobalNvsArea->Area->GpioBaseAddress;

  if (SaGlobalNvsArea->Area->SgMode == SgModeMuxless) {
    if (SaGlobalNvsArea->Area->SgGPIOSupport)  {
      //
      // GPIO Assignment for ASL usage
      //
      SgOpRegion->SgGPIOSupport   = SaGlobalNvsArea->Area->SgGPIOSupport;
      SgOpRegion->SgDgpuPwrOK     = SaGlobalNvsArea->Area->SgDgpuPwrOK;
      SgOpRegion->SgDgpuHoldRst   = SaGlobalNvsArea->Area->SgDgpuHoldRst;
      SgOpRegion->SgDgpuPwrEnable = SaGlobalNvsArea->Area->SgDgpuPwrEnable;
      SgOpRegion->SgDgpuPrsnt     = SaGlobalNvsArea->Area->SgDgpuPrsnt;
    }
  }

  //
  // PEG Endpoint Base Addresses and Capability Structure Offsets for ASL usage
  //

  PciAddress = PCI_EXPRESS_LIB_ADDRESS (
                 DgpuBridgeBus,
                 DgpuBridgeDevice,
                 DgpuBridgeFunction,
                 0x0
                 );
  SgOpRegion->DgpuPcieCfgBaseAddress = (UINT32)(PcdGet64 (PcdPciExpressBaseAddress) + PciAddress); 
  
  MasterDgpuBus = PciExpressRead8 (PciAddress);

  PciAddress = PCI_EXPRESS_LIB_ADDRESS (
                 MasterDgpuBus,
                 DGPU_DEVICE_NUM,
                 DGPU_FUNCTION_NUM,
                 0x0
                 );
 
  SgOpRegion->EndpointBaseAddress = (UINT32)(PcdGet64 (PcdPciExpressBaseAddress) + PciAddress);
  SgOpRegion->XPcieCfgBaseAddress = (UINT32)(PcdGet64 (PcdPciExpressBaseAddress));
  PciFindCapId (MasterDgpuBus, DGPU_DEVICE_NUM, DGPU_FUNCTION_NUM, PEG_CAP_ID, &CapOffset);
  SgOpRegion->EndpointPcieCapBaseAddress = CapOffset;


  return EFI_SUCCESS;
}


/**
  Nvidia Graphics OpRegion installation function.

  @param[in] None

  @retval EFI_SUCCESS     The driver installed without error.
  @retval EFI_ABORTED     The driver encountered an error and could not complete
                  installation of the ACPI tables.
**/
EFI_STATUS
InstallNvidiaOpRegion (
  VOID
  )
{
  EFI_STATUS                             Status;
  UINTN                                  Size;
  SYSTEM_AGENT_GLOBAL_NVS_AREA_PROTOCOL  *GlobalNvsArea;
//[-start-140213-IB10920078-remove]//
//  NVIDIA_OPREGION                        NvidiaOpRegion;
//[-end-140213-IB10920078-remove]//

  ///
  ///  Locate Global NVS Protocol.
  ///
  Status = gBS->LocateProtocol (
                  &gSaGlobalNvsAreaProtocolGuid,
                  NULL,
                  &GlobalNvsArea
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ///
  /// Allocate an ACPI NVS memory buffer as the Nvidia NVIG OpRegion, zero initialize
  /// the entire 1K, and set the Nvidia NVIG OpRegion pointer in the Global NVS
  /// area structure.
  ///
  Size    = sizeof (NVIG_OPREGION);
  Status  = (gBS->AllocatePool) (EfiACPIMemoryNVS, Size, &NvidiaOpRegion.NvIgOpRegion);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  (gBS->SetMem) (NvidiaOpRegion.NvIgOpRegion, Size, 0);

  ///
  /// Set up DeviceID values for _DOD.
  /// Note that Display Subtype bits[15-12] and Port index bits[7:4] are set as per NV Switchable 3.0 spec.
  /// Not used by Intel driver.
  ///
  /// Display Type - CRT
  ///
  GlobalNvsArea->Area->DeviceId1 = 0x80010100;

//  if (GlobalNvsArea->Area->ActiveLFP == 3) {
    ///
    /// If Active LFP = EDP_A
    ///
    /// Display type - LFP Device Sub Type - eDP
    ///
    GlobalNvsArea->Area->DeviceId2 = 0x8001A420;
//  } else {
    ///
    /// Display Type - LFP Device Sub Type - LVDS
    ///
//    GlobalNvsArea->Area->DeviceId2 = 0x80010410;
//  }
  ///
  /// Display type - EFP  Device Sub type - DisplayPort 1.1
  ///
  GlobalNvsArea->Area->DeviceId3 = 0x80016330;

  ///
  /// Display type - EFP  Device Sub type - HDMI 1.2 or 1.3a
  ///
  GlobalNvsArea->Area->DeviceId4 = 0x80017331;

  ///
  /// Display type - EFP  Device Sub type - HDMI 1.2 or 1.3a
  ///
  GlobalNvsArea->Area->DeviceId5 = 0x80017342;

  ///
  /// Display type - EFP  Device Sub type - DisplayPort 1.1
  ///
  GlobalNvsArea->Area->DeviceId6 = 0x80016353;

  ///
  /// Display type - EFP  Device Sub type - HDMI 1.2 or 1.3a
  ///
  GlobalNvsArea->Area->DeviceId7 = 0x80017354;

  ///
  /// DeviceId8 is not being used on Calpella SG
  ///
  GlobalNvsArea->Area->DeviceId8 = 0x0;

  ///
  /// NDID
  ///
  GlobalNvsArea->Area->NumberOfValidDeviceId = 0x7;

  ///
  /// NVIG
  ///
  GlobalNvsArea->Area->NvIgOpRegionAddress = (UINT32) (UINTN) (NvidiaOpRegion.NvIgOpRegion);

  ///
  /// NVIG Header
  ///
  (gBS->CopyMem) (NvidiaOpRegion.NvIgOpRegion->NISG, NVIG_HEADER_SIGNATURE, sizeof (NVIG_HEADER_SIGNATURE));
  NvidiaOpRegion.NvIgOpRegion->NISZ = NVIG_OPREGION_SIZE;
  NvidiaOpRegion.NvIgOpRegion->NIVR = NVIG_OPREGION_VER;

  ///
  /// Panel Scaling Preference
  ///
  NvidiaOpRegion.NvIgOpRegion->GPSP = GlobalNvsArea->Area->IgdPanelScaling;

  ///
  /// Allocate an ACPI NVS memory buffer as the Nvidia NVHM OpRegion, zero initialize
  /// the entire 62K, and set the Nvidia NVHM OpRegion pointer in the Global NVS
  /// area structure.
  ///
  Size    = sizeof (NVHM_OPREGION);
  Status  = (gBS->AllocatePool) (EfiACPIMemoryNVS, Size, &NvidiaOpRegion.NvHmOpRegion);
  if (EFI_ERROR (Status)) {
    (gBS->FreePool) (NvidiaOpRegion.NvIgOpRegion);
    return Status;
  }

  (gBS->SetMem) (NvidiaOpRegion.NvHmOpRegion, Size, 0);

  ///
  /// NVHM
  ///
  GlobalNvsArea->Area->NvHmOpRegionAddress = (UINT32) (UINTN) (NvidiaOpRegion.NvHmOpRegion);

  ///
  /// NVHM Header Signature, Size, Version
  ///
  (gBS->CopyMem) (NvidiaOpRegion.NvHmOpRegion->NVSG, NVHM_HEADER_SIGNATURE, sizeof (NVHM_HEADER_SIGNATURE));
  NvidiaOpRegion.NvHmOpRegion->NVSZ = NVHM_OPREGION_SIZE;
  NvidiaOpRegion.NvHmOpRegion->NVVR = NVHM_OPREGION_VER;

  ///
  /// NVHM opregion address
  ///
  NvidiaOpRegion.NvHmOpRegion->NVHO = (UINT32) (UINTN) (NvidiaOpRegion.NvHmOpRegion);

  ///
  /// Copy Oprom to allocated space in NV Opregion
  ///
  NvidiaOpRegion.NvHmOpRegion->RVBS = VbiosSize;
  if (VbiosAddress != NULL) {
	(gBS->CopyMem) ((VOID *) (UINTN) NvidiaOpRegion.NvHmOpRegion->RBUF, VbiosAddress, NvidiaOpRegion.NvHmOpRegion->RVBS);
  }
  return Status;
}

/**
  AMD graphics OpRegion installation function.

  @param[in] None

  @retval EFI_SUCCESS     The driver installed without error.
  @retval EFI_ABORTED     The driver encountered an error and could not complete
                  installation of the ACPI tables.
**/
EFI_STATUS
InstallAmdOpRegion (
  VOID
  )
{
  EFI_STATUS                             Status;
  UINTN                                  Size;
  SYSTEM_AGENT_GLOBAL_NVS_AREA_PROTOCOL  *GlobalNvsArea;
//[-start-140218-IB10920078-remove]//
//  AMD_OPREGION                           AmdOpRegion;
//[-end-140218-IB10920078-remove]//
//[-start-140220-IB10920078-add]//
  CHIPSET_CONFIGURATION                  *SystemConfiguration = NULL;
  EFI_SETUP_UTILITY_PROTOCOL            *SetupUtility;

  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid,
                                NULL,
                                &SetupUtility
                               );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  SystemConfiguration = (CHIPSET_CONFIGURATION *)SetupUtility->SetupNvData;
//[-end-140220-IB10920078-add]//


  ///
  ///  Locate Global NVS Protocol.
  ///
  Status = gBS->LocateProtocol (
                  &gSaGlobalNvsAreaProtocolGuid,
                  NULL,
                  &GlobalNvsArea
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ///
  /// Allocate an ACPI NVS memory buffer as the Amd APXM OpRegion, zero initialize
  /// the entire 1K, and set the Amd APXM OpRegion pointer in the Global NVS
  /// area structure.
  ///
  Size    = sizeof (APXM_OPREGION);
  Status  = (gBS->AllocatePool) (EfiACPIMemoryNVS, Size, &AmdOpRegion.ApXmOpRegion);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  (gBS->SetMem) (AmdOpRegion.ApXmOpRegion, Size, 0);

  ///
  /// APXM address
  ///
  GlobalNvsArea->Area->ApXmOpRegionAddress  = (UINT32) (UINTN) (AmdOpRegion.ApXmOpRegion);
  AmdOpRegion.ApXmOpRegion->APXA            = (UINT32) (UINTN) (AmdOpRegion.ApXmOpRegion);

  ///
  /// Set up DIDx values for _DOD
  ///
  /// Device ID - CRT on IGPU
  ///
  GlobalNvsArea->Area->DeviceId1 = 0x80010100;

  ///
  /// Device ID - LFP (LVDS or eDP)
  ///
  GlobalNvsArea->Area->DeviceId2 = 0x80010400;

  ///
  /// Display type - EFP  Device Sub type - DisplayPort 1.1
  ///
  GlobalNvsArea->Area->DeviceId3 = 0x80010300;

  ///
  /// Display type - EFP  Device Sub type - HDMI 1.2 or 1.3a
  ///
  GlobalNvsArea->Area->DeviceId4 = 0x80010301;

  ///
  /// Display type - EFP  Device Sub type - HDMI 1.2 or 1.3a
  ///
  GlobalNvsArea->Area->DeviceId5 = 0x80010302;

  ///
  /// Display type - EFP  Device Sub type - DisplayPort 1.1
  ///
  GlobalNvsArea->Area->DeviceId6 = 0x80010303;

  ///
  /// Display type - EFP  Device Sub type - HDMI 1.2 or 1.3a
  ///
  GlobalNvsArea->Area->DeviceId7 = 0x80010304;

  ///
  /// DeviceId8 is not being used on Calpella SG
  ///
  GlobalNvsArea->Area->DeviceId8 = 0x0;

  ///
  /// NDID
  ///
  GlobalNvsArea->Area->NumberOfValidDeviceId = 0x7;

  ///
  /// APXM Header
  ///
  (gBS->CopyMem) (AmdOpRegion.ApXmOpRegion->APSG, APXM_HEADER_SIGNATURE, sizeof (APXM_HEADER_SIGNATURE));
  AmdOpRegion.ApXmOpRegion->APSZ  = APXM_OPREGION_SIZE;
  AmdOpRegion.ApXmOpRegion->APVR  = APXM_OPREGION_VER;

  ///
  /// Total number of toggle list entries
  ///
  AmdOpRegion.ApXmOpRegion->NTLE = 15;

  ///
  /// The display combinations in the list...
  ///
  AmdOpRegion.ApXmOpRegion->TLEX[0] = 0x0002; ///< CRT
  AmdOpRegion.ApXmOpRegion->TLEX[1] = 0x0001; ///< LFP
  AmdOpRegion.ApXmOpRegion->TLEX[2] = 0x0008; ///< DP_B
  AmdOpRegion.ApXmOpRegion->TLEX[3] = 0x0080; ///< HDMI_B
  AmdOpRegion.ApXmOpRegion->TLEX[4] = 0x0200; ///< HDMI_C
  AmdOpRegion.ApXmOpRegion->TLEX[5] = 0x0400; ///< DP_D
  AmdOpRegion.ApXmOpRegion->TLEX[6] = 0x0800; ///< HDMI_D
  AmdOpRegion.ApXmOpRegion->TLEX[7] = 0x0003; ///< LFP+CRT
  AmdOpRegion.ApXmOpRegion->TLEX[8] = 0x0009; ///< LFP+DP_B
  AmdOpRegion.ApXmOpRegion->TLEX[9] = 0x0081; ///< LFP+HDMI_B
  AmdOpRegion.ApXmOpRegion->TLEX[10] = 0x0201; ///< LFP+HDMI_C
  AmdOpRegion.ApXmOpRegion->TLEX[11] = 0x0401; ///< LFP+DP_D
  AmdOpRegion.ApXmOpRegion->TLEX[12] = 0x0801; ///< LFP+HDMI_D
  AmdOpRegion.ApXmOpRegion->TLEX[13] = 0x0; ///< Dummy 1
  AmdOpRegion.ApXmOpRegion->TLEX[14] = 0x0; ///< Dummy 2

  ///
  /// Panel Scaling Preference
  ///
  AmdOpRegion.ApXmOpRegion->EXPM = GlobalNvsArea->Area->IgdPanelScaling;

  ///
  /// Copy Oprom to allocated space in ATI Opregion
  ///
  AmdOpRegion.ApXmOpRegion->RVBS = VbiosSize;
  if (VbiosAddress != NULL) {
  (gBS->CopyMem) ((VOID *) (UINTN) AmdOpRegion.ApXmOpRegion->RBUF, VbiosAddress, AmdOpRegion.ApXmOpRegion->RVBS);
  }

//[-start-140220-IB10920078-add]//
  if (GlobalNvsArea->Area->SgMode == SgModeMuxless) {
    AmdOpRegion.ApXmOpRegion->EXPM = SystemConfiguration->PannelScaling; // Expansion Mode
    if ((SystemConfiguration->PowerXpress == MuxlessDynamic) ||
        (SystemConfiguration->PowerXpress == MuxlessFixedDynamic)) {
      AmdOpRegion.ApXmOpRegion->PxDynamicSupportState = Support; // Dynamic Support State 1: dynamic, 0: Non-dynamic
    }
    if (SystemConfiguration->PowerXpress != MuxlessDynamic) {
      AmdOpRegion.ApXmOpRegion->PxFixedSupportState = Support; // Fixed   Support State 1: Fixed,   0: Non-fixed
    }
    if (SystemConfiguration->PowerXpress == FullDgpuPowerOffDynamic) {
      AmdOpRegion.ApXmOpRegion->PxDynamicSupportState     = Support;   // Dynamic Support State 1: dynamic, 0: Non-dynamic
      AmdOpRegion.ApXmOpRegion->PxFixedSupportState       = Unsupport; // Fixed   Support State 1: Fixed,   0: Non-fixed
      AmdOpRegion.ApXmOpRegion->PxFullDgpuPowerOffDynamic = Support;   // Full dGPU PowerOff Dynamic mode support
    }
    if (SystemConfiguration->PowerXpress == FixedFullDgpuPowerOffDynamic) {
      AmdOpRegion.ApXmOpRegion->PxDynamicSupportState     = Support; // Dynamic Support State 1: dynamic, 0: Non-dynamic
      AmdOpRegion.ApXmOpRegion->PxFixedSupportState       = Support; // Fixed   Support State 1: Fixed,   0: Non-fixed
      AmdOpRegion.ApXmOpRegion->PxFullDgpuPowerOffDynamic = Support; // Full dGPU PowerOff Dynamic mode support
    }
  }
  
  if (GlobalNvsArea->Area->SgMode == SgModeDgpu) {
    AmdOpRegion.ApXmOpRegion->EXPM =  SystemConfiguration->PannelScaling; // Expansion Mode
    AmdOpRegion.ApXmOpRegion->SlaveDgpuSupport = Inactive;
//-    if (SgInfoDataHob->SlaveMxmGpuSupport) {
//-      AmdOpRegion->SlaveDgpuSupport = Active;
//-    }
  }
//[-end-140220-IB10920078-add]//

  return Status;
}

