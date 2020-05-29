/** @file
  This driver is for Switchable Graphics Feature DXE initialize.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <SwitchableGraphicsDxe.h>

//
// Function Prototypes
//
VOID
EFIAPI
SwitchableGraphicsBdsCallback (
  IN EFI_EVENT                                Event,
  IN VOID                                     *Context
  );

STATIC
EFI_STATUS
SwitchableGraphicsDxeInitialize (
  IN OUT SG_DXE_INFORMATION_DATA              **SgDxeInfoData,
  IN OUT SG_INFORMATION_DATA_HOB              **SgInfoDataHob
  );

STATIC
EFI_STATUS
NvidiaOptimusDxeInitialize (
  IN SG_DXE_INFORMATION_DATA                  *SgDxeInfoData,
  IN SG_INFORMATION_DATA_HOB                  *SgInfoDataHob
  );

STATIC
EFI_STATUS
InitMXMSupport (
  IN EFI_GUID                                 *MxmBinaryGuid,
  IN OUT UINTN                                *Address,
  IN OUT UINTN                                *Size,
  IN UINT8                                    BootType
  );

EFI_STATUS
EFIAPI
CloseNvidiaDgpuHdAudio (
  IN EFI_EVENT                                Event,
  IN VOID                                     *Context
  );

STATIC
EFI_STATUS
SearchDiscreteGraphicsVbios (
  IN OUT SG_DXE_INFORMATION_DATA              *SgDxeInfoData
  );

STATIC
EFI_STATUS
ExecuteDiscreteGraphicsVbios (
  IN OUT SG_DXE_INFORMATION_DATA              *SgDxeInfoData,
  IN SG_INFORMATION_DATA_HOB                  *SgInfoDataHob
  );

STATIC
EFI_STATUS
SetSwitchableGraphicsSsdt (
  IN SG_DXE_INFORMATION_DATA                  *SgDxeInfoData,
  IN SG_INFORMATION_DATA_HOB                  *SgInfoDataHob
  );

STATIC
EFI_STATUS
InitializeOpRegion (
  IN EFI_ACPI_DESCRIPTION_HEADER              *NvStoreTable,
  IN SG_DXE_INFORMATION_DATA                  *SgDxeInfoData,
  IN SG_INFORMATION_DATA_HOB                  *SgInfoDataHob
  );

STATIC
VOID
UpdateSgOpRegion (
  IN OUT OPERATION_REGION_SG                  *SgOpRegion,
  IN SG_DXE_INFORMATION_DATA                  *SgDxeInfoData,
  IN SG_INFORMATION_DATA_HOB                  *SgInfoDataHob
  );

STATIC
VOID
UpdateAmdOpRegion (
  IN OUT OPERATION_REGION_AMD                 *AmdOpRegion,
  IN SG_DXE_INFORMATION_DATA                  *SgDxeInfoData,
  IN SG_INFORMATION_DATA_HOB                  *SgInfoDataHob
  );

STATIC
VOID
UpdateNvidiaOpRegion (
  IN OUT OPERATION_REGION_NVIDIA              *NvidiaOpRegion,
  IN SG_DXE_INFORMATION_DATA                  *SgDxeInfoData,
  IN SG_INFORMATION_DATA_HOB                  *SgInfoDataHob
  );

STATIC
EFI_STATUS
SgInformationUpdate (
  IN SG_DXE_INFORMATION_DATA                  *SgDxeInfoData,
  IN SG_INFORMATION_DATA_HOB                  *SgInfoDataHob
  );

EFI_STATUS
EFIAPI
SetSecondaryGrcphicsCommandRegister (
  IN EFI_EVENT                                Event,
  IN VOID                                     *Context
  );

EFI_STATUS
EFIAPI
CloseDiscreteSecondaryHdAudio (
  IN EFI_EVENT                                Event,
  IN VOID                                     *Context
  );

VOID
EFIAPI
FreeSgDxeInfoData (
  IN OUT SG_DXE_INFORMATION_DATA              **SgDxeInfoData
  );

/**

  Switchable Graphics feature driver entry point.
  This driver will handle secondary VBIOS and create feature own SSDT.

  @param[in] ImageHandle   Image handle of this driver.
  @param[in] SystemTable   Pointer to standard EFI system table.

  @retval EFI_SUCCESS    Switchable Graphics feature DXE initialized successfully.
  @retval !EFI_SUCCESS   Switchable Graphics feature doesn't be supported.

**/
EFI_STATUS
SwitchableGraphicsDxeInitEntry (
  IN EFI_HANDLE                               ImageHandle,
  IN EFI_SYSTEM_TABLE                         *SystemTable
  )
{
  EFI_STATUS                                  Status;
  H2O_SG_INFO_PROTOCOL                        *SgInfoProtocol;
  VOID                                        *Registration;

  if (FeaturePcdGet (PcdSwitchableGraphicsSupported)) {
    EfiCreateProtocolNotifyEvent (
      &gH2OSwitchableGraphicsEventProtocolGuid,
      TPL_CALLBACK,
      SwitchableGraphicsBdsCallback,
      NULL,
      &Registration
      );

    //
    // Allocate and install Switchable Graphics information Protocol.
    //
    SgInfoProtocol = AllocateReservedZeroPool (sizeof (H2O_SG_INFO_PROTOCOL));
    if (SgInfoProtocol == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    Status = gBS->InstallMultipleProtocolInterfaces (
                    &ImageHandle,
                    &gH2OSwitchableGraphicsInfoProtocolGuid,
                    SgInfoProtocol,
                    NULL
                    );

    return Status;
  }

  return EFI_SUCCESS;
}

/**

  After BDS platform driver connect Root Bridge will install SG protocol
  to trigger event to run this Switchable Graphics feature callback,
  this specific timing discrete graphics already exist then we can
  handle and initialize it.

  @param[in] Event    Event whose notification function is being invoked.
  @param[in] Context  Pointer to the notification function's context.

  @retval None.

**/
VOID
EFIAPI
SwitchableGraphicsBdsCallback (
  IN EFI_EVENT                                Event,
  IN VOID                                     *Context
  )
{
  EFI_STATUS                                  Status;
  SG_DXE_INFORMATION_DATA                     *SgDxeInfoData;
  SG_INFORMATION_DATA_HOB                     *SgInfoDataHob;

  //
  // Get SG information data HOB, and create SG DXE infromation Data.
  //
  Status = SwitchableGraphicsDxeInitialize (&SgDxeInfoData, &SgInfoDataHob);
  if (EFI_ERROR (Status)) {
    return;
  }

  //
  // Close Switchable Graphics BDS event.
  //
  gBS->CloseEvent (Event);

  if (FeaturePcdGet (PcdNvidiaOptimusSupported)) {
    //
    // Get MXM SIS, Update global NVS area variable and set SG variable for nVIDIA Optimus.
    //
    Status = NvidiaOptimusDxeInitialize (SgDxeInfoData, SgInfoDataHob);
    if (EFI_ERROR (Status)) {
      FreeSgDxeInfoData (&SgDxeInfoData);
      return;
    }
  }

  //
  // Search discrete graphics VBIOS location, and initialize secondary ATI graphics VBIOS.
  //
  Status = SearchDiscreteGraphicsVbios (SgDxeInfoData);
  if (!EFI_ERROR (Status)) {
    if (FeaturePcdGet (PcdAmdPowerXpressSupported)) {
      if ((SgInfoDataHob->SgMode == SgModeMuxless) &&
          (SgDxeInfoData->MasterDgpuVendorId == AMD_VID) &&
          (SgInfoDataHob->BootType != EfiBootType)) {
        ExecuteDiscreteGraphicsVbios (SgDxeInfoData, SgInfoDataHob);
      }
    }
  }

  //
  // Set SG SSDT and initialize SG own operation region.
  //
  Status = SetSwitchableGraphicsSsdt (SgDxeInfoData, SgInfoDataHob);
  if (EFI_ERROR (Status)) {
    FreeSgDxeInfoData (&SgDxeInfoData);
    return;
  }

  //
  // Update Switchable Graphics infromation protocol,
  // and create event to set the Secondary Grcphics Command Register.
  //
  Status = SgInformationUpdate (SgDxeInfoData, SgInfoDataHob);
  if (EFI_ERROR (Status)) {
    FreeSgDxeInfoData (&SgDxeInfoData);
  }
}

/**

  Switchable Graphics feature DXE driver initialize function.
  This function will get SG information data HOB, and create SG DXE infromation Data.

  @param[in, out] SgDxeInfoData   A double pointer of SG DXE information data structure,
                                  this driver or SG Operation Region will use these data.
  @param[in, out] SgInfoDataHob   A double pointer of SG information data HOB,
                                  SG PEI Module created this HOB, and passes the data from PEI phase.

  @retval EFI_SUCCESS    This path initialized data successfully, and need handle SG SSDT and VBIOS.
  @retval !EFI_SUCCESS   This path initialized data failed and doesn't handle SG SSDT and VBIOS.

**/
STATIC
EFI_STATUS
SwitchableGraphicsDxeInitialize (
  IN OUT SG_DXE_INFORMATION_DATA              **SgDxeInfoData,
  IN OUT SG_INFORMATION_DATA_HOB              **SgInfoDataHob
  )
{
  EFI_STATUS                                  Status;
  UINT8                                       MasterDgpuBus;
  UINT16                                      GpioBaseAddress;
  UINT16                                      MasterDgpuVendorId;
  UINTN                                       PciAddress;

  (*SgDxeInfoData) = NULL;
  //
  // Get SG related information data HOB.
  //
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, SgInfoDataHob);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  (*SgInfoDataHob) = GetNextGuidHob (&gH2OSgInformationDataHobGuid, (*SgInfoDataHob));
  if ((*SgInfoDataHob) == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // Read Master discrete GPU Bus and Venodr ID.
  //
  PciAddress = PCI_EXPRESS_LIB_ADDRESS (
                 (**SgInfoDataHob).DgpuBridgeBus,
                 (**SgInfoDataHob).DgpuBridgeDevice,
                 (**SgInfoDataHob).DgpuBridgeFunction,
                 PCI_SBUS
                 );
  MasterDgpuBus = PciExpressRead8 (PciAddress);
  PciAddress = PCI_EXPRESS_LIB_ADDRESS (
                 MasterDgpuBus,
                 DGPU_DEVICE_NUM,
                 DGPU_FUNCTION_NUM,
                 PCI_VID
                 );
  MasterDgpuVendorId = PciExpressRead16 (PciAddress);

  if ((MasterDgpuBus == 0x00) || (MasterDgpuBus == 0xFF) || (MasterDgpuVendorId == 0xFFFF) ||
      ((**SgInfoDataHob).SgMode == SgModeDisabled)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Read GPIO base address.
  //
  PciAddress = PCI_EXPRESS_LIB_ADDRESS (
                 DEFAULT_PCI_BUS_NUMBER_PCH,
                 PCI_DEVICE_NUMBER_PCH_LPC,
                 PCI_FUNCTION_NUMBER_PCH_LPC,
                 R_PCH_LPC_GPIO_BASE
                 );
  GpioBaseAddress = ((PciExpressRead16 (PciAddress)) & (~BIT0));
  if (GpioBaseAddress == 0) {
    return EFI_DEVICE_ERROR;
  }

  //
  // Allocate and initialize Switchable Graphics DXE driver information data.
  //
  (*SgDxeInfoData) = AllocateZeroPool (sizeof (SG_DXE_INFORMATION_DATA));
  if ((*SgDxeInfoData) == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Bus 0 Device 0 Function 0 Offset 54 is DEVEN - Device Enable register (decide iGPU exist or not).
  //
  PciAddress = PCI_EXPRESS_LIB_ADDRESS (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, R_SA_DEVEN);
  (**SgDxeInfoData).SaDeven = PciExpressRead8 (PciAddress);
  (**SgDxeInfoData).SlaveMxmGpuSupport = (**SgInfoDataHob).SlaveMxmGpuSupport;
  (**SgDxeInfoData).AmdSecondaryGrcphicsCommandRegister = (**SgInfoDataHob).AmdSecondaryGrcphicsCommandRegister;
  (**SgDxeInfoData).NvidiaSecondaryGrcphicsCommandRegister = (**SgInfoDataHob).NvidiaSecondaryGrcphicsCommandRegister;
  (**SgDxeInfoData).GpioBaseAddress = GpioBaseAddress;
  (**SgDxeInfoData).MasterDgpuBus = MasterDgpuBus;
  (**SgDxeInfoData).MasterDgpuVendorId = MasterDgpuVendorId;
  //
  // Read Slave discrete GPU Bus and Venodr ID.
  //
  if ((**SgDxeInfoData).SlaveMxmGpuSupport) {
    PciAddress = PCI_EXPRESS_LIB_ADDRESS (
                   (**SgInfoDataHob).SlaveDgpuBridgeBus,
                   (**SgInfoDataHob).SlaveDgpuBridgeDevice,
                   (**SgInfoDataHob).SlaveDgpuBridgeFunction,
                   PCI_SBUS
                   );
    (**SgDxeInfoData).SlaveDgpuBus = PciExpressRead8 (PciAddress);
    PciAddress = PCI_EXPRESS_LIB_ADDRESS (
                   (**SgDxeInfoData).SlaveDgpuBus,
                   DGPU_DEVICE_NUM,
                   DGPU_FUNCTION_NUM,
                   PCI_VID
                   );
    (**SgDxeInfoData).SlaveDgpuVendorId = PciExpressRead16 (PciAddress);
  }

  return EFI_SUCCESS;
}

/**

  nVIDIA Optimus DXE driver initialize function.
  This function will update global NVS area variable, set SG variable and load MXM SIS binary.

  @param[in, out] SgDxeInfoData   A pointer of SG DXE information data structure,
                                  this driver or SG Operation Region will use these data.
  @param[in]      SgInfoDataHob   A pointer of SG information data HOB,
                                  SG PEI Module created this HOB, and passes the data from PEI phase.

  @retval EFI_SUCCESS    Update global NVS area variable and set SG variable both successfully.
  @retval !EFI_SUCCESS   Update global NVS area variable or set SG variable failed.

**/
STATIC
EFI_STATUS
NvidiaOptimusDxeInitialize (
  IN OUT SG_DXE_INFORMATION_DATA              *SgDxeInfoData,
  IN SG_INFORMATION_DATA_HOB                  *SgInfoDataHob
  )
{
  EFI_EVENT                                   ReadyToBootEvent;
  EFI_GLOBAL_NVS_AREA_PROTOCOL                *GlobalNvsArea;
  EFI_STATUS                                  Status;
  SG_VARIABLE_CONFIGURATION                   *SgVariableData;

  //
  // Base on different mode to load different MXM binary.
  //
  if (SgDxeInfoData->MasterDgpuVendorId == NVIDIA_VID) {
    if(((SgDxeInfoData->SaDeven) & B_SA_DEVEN_D2EN_MASK) == Inactive) {
      InitMXMSupport (
        &(SgInfoDataHob->PegModeMasterMxmBinaryGuid),
        &(SgDxeInfoData->MasterMxmBinFile.Address),
        &(SgDxeInfoData->MasterMxmBinFile.Size),
        SgInfoDataHob->BootType
        );
    } else {
      InitMXMSupport (
        &(SgInfoDataHob->SgModeMxmBinaryGuid),
        &(SgDxeInfoData->MasterMxmBinFile.Address),
        &(SgDxeInfoData->MasterMxmBinFile.Size),
        SgInfoDataHob->BootType
        );
    }
  }
  if ((SgDxeInfoData->SlaveMxmGpuSupport) && (SgDxeInfoData->SlaveDgpuVendorId == NVIDIA_VID)) {
      InitMXMSupport (
        &(SgInfoDataHob->PegModeSlaveMxmBinaryGuid),
        &(SgDxeInfoData->SlaveMxmBinFile.Address),
        &(SgDxeInfoData->SlaveMxmBinFile.Size),
        SgInfoDataHob->BootType
        );
  }
  //
  //  Locate Global NVS Protocol and update PEG Vendor ID.
  //
  Status = gBS->LocateProtocol (
                  &gEfiGlobalNvsAreaProtocolGuid,
                  NULL,
                  (VOID **)&GlobalNvsArea
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (SgInfoDataHob->SgMode == SgModeMuxless) {
    GlobalNvsArea->Area->PegVendorID = SgDxeInfoData->MasterDgpuVendorId;
    if (SgDxeInfoData->MasterDgpuVendorId == NVIDIA_VID) {
      //
      // If NVIDIA SG variable doesn't exist, set the default value.
      //
      SgVariableData = GetVariable (L"SwitchableGraphicsVariable", &gH2OSwitchableGraphicsVariableGuid);
      if (SgVariableData == NULL) {
        SgVariableData = AllocateZeroPool (sizeof (SG_VARIABLE_CONFIGURATION));
        if (SgVariableData == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }
        SgVariableData->OptimusVariable.OptimusFlag   = Inactive;
        SgVariableData->OptimusVariable.MasterDgpuBus = SgDxeInfoData->MasterDgpuBus;
        SgVariableData->OptimusVariable.SlaveDgpuBus  = SgDxeInfoData->SlaveDgpuBus;
        Status = gRT->SetVariable (
                        L"SwitchableGraphicsVariable",
                        &gH2OSwitchableGraphicsVariableGuid,
                        EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                        sizeof (SG_VARIABLE_CONFIGURATION),
                        SgVariableData
                        );
        if (EFI_ERROR (Status)) {
          FreePool (SgVariableData);
          return Status;
        }
      }
      SgDxeInfoData->OptimusFlag = SgVariableData->OptimusVariable.OptimusFlag;
      Status = EfiCreateEventReadyToBootEx (
                 TPL_CALLBACK,
                 CloseNvidiaDgpuHdAudio,
                 SgDxeInfoData,
                 &ReadyToBootEvent
                 );
      FreePool (SgVariableData);
    }
  }

  return Status;
}

/**

  Get MXM (Mobile PCI Express Module) SIS (System Information Structure) binary from firmware volume,
  legacy boot allocate 0xE0000 legacy memory region and copy MXM SIS binary into this region
  for MXM INT 15 callback function to get this binary,
  EFI boot just get the MXM SIS binary firmware volume address.

  @param[in]      MxmBinaryGuid   MXM SIS binary GUID for search MXM SIS binary from irmware volume.
  @param[in, out] Address         EFI boot firmware volume MXM SIS binary address
                                  or legacy boot allocated 0xE0000 legacy region MXM SIS binary address.
  @param[in, out] Size            MXM SIS binary size.
  @param[in]      BootType        SCU Boot Type setting.

  @retval EFI_SUCCESS    Get MXM SIS binary successfully.
  @retval !EFI_SUCCESS   Get MXM SIS binary failed.

**/
STATIC
EFI_STATUS
InitMXMSupport (
  IN EFI_GUID                                 *MxmBinaryGuid,
  IN OUT UINTN                                *Address,
  IN OUT UINTN                                *Size,
  IN UINT8                                    BootType
  )
{
  EFI_COMPATIBILITY16_TABLE                   *EfiTable;
  EFI_IA32_REGISTER_SET                       Regs;
  EFI_LEGACY_BIOS_PROTOCOL                    *LegacyBios;
  EFI_LEGACY_REGION_PROTOCOL                  *LegacyRegion;
  EFI_STATUS                                  Status;
  UINT8                                       *Ptr;
  UINTN                                       *BuffPtr;
  UINTN                                       BufferSize;
  UINTN                                       TablePtr;

  EfiTable = NULL;
  BuffPtr  = NULL;
  (*Address) = 0;
  (*Size)    = 0;

  //
  // Locate MXM Binary
  //
  Status = GetSectionFromAnyFv (MxmBinaryGuid, EFI_SECTION_RAW, 0, &BuffPtr, &BufferSize);
  if (EFI_ERROR (Status) || BufferSize == 0) {
    return Status;
  }
  if (BootType == EfiBootType) {
    (*Address) = (UINTN)(VOID *)BuffPtr;
    (*Size)    = BufferSize;
  }

  //
  // Locate Legacy Bios and Legacy Region Protocol.
  //
  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **)&LegacyBios);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = gBS->LocateProtocol (&gEfiLegacyRegionProtocolGuid, NULL, (VOID **)&LegacyRegion);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Get EFI table from F segment.
  //
  for (Ptr = (UINT8 *)((UINTN)0xFE000); Ptr < (UINT8 *) ((UINTN) 0x100000); Ptr += 0x10) {
    if (*(UINT32 *) Ptr == SIGNATURE_32 ('I', 'F', 'E', '$')) {
      EfiTable = (EFI_COMPATIBILITY16_TABLE *)Ptr;
      break;
    }
  }
  if (EfiTable == NULL) {
    return EFI_NOT_FOUND;
  }

  Status = LegacyRegion->UnLock (LegacyRegion, 0xE0000, 0x20000, NULL);
  //
  // To find the required size of availabe free memory
  //
  ZeroMem (&Regs, sizeof (EFI_IA32_REGISTER_SET));
  Regs.X.AX = Legacy16GetTableAddress;
  Regs.X.BX = 0x02; // E0000Region
  Regs.X.CX = (UINT16)BufferSize;
  Regs.X.DX = 1;
  Status = LegacyBios->FarCall86 (
                         LegacyBios,
                         EfiTable->Compatibility16CallSegment,
                         EfiTable->Compatibility16CallOffset,
                         &Regs,
                         NULL,
                         0
                         );
  if (EFI_ERROR (Status)) {
    return Status;
  } 
  //
  // To copy the image to legacy memory
  //
  TablePtr = (UINT32) ((Regs.X.DS << 4) + Regs.X.BX);
  CopyMem ((VOID *)TablePtr, (VOID *)BuffPtr, BufferSize);
  (*Address) = TablePtr;
  (*Size)    = BufferSize;
  Status = LegacyRegion->Lock (LegacyRegion, 0xE0000, 0x20000, NULL);

  return Status;
}

/**

  Close HD Audio device expect S3/S4.
  When S3/S4 resume, check SG variable "OptimusFlag" to turn on/off HD audio device.
  dGPU device Register 0x488 Bit 25 is HD audio device power enable bit.

  @param[in]  Event     The Event this notify function registered to.
  @param[in]  Context   Pointer to the context data registerd to the Event.

  @retval EFI_SUCCESS   Set secondary Graphics command register successfully.

**/
EFI_STATUS
EFIAPI
CloseNvidiaDgpuHdAudio (
  IN EFI_EVENT                                Event,
  IN VOID                                     *Context
  )
{
  EFI_BOOT_MODE                               BootMode;
  EFI_PEI_HOB_POINTERS                        Hob;
  EFI_STATUS                                  Status;
  SG_DXE_INFORMATION_DATA                     *SgDxeInfoData;
  UINTN                                       PciAddress;
  VOID                                        *HobList;

  SgDxeInfoData = (SG_DXE_INFORMATION_DATA *)Context;
  if (SgDxeInfoData == NULL) {
    return EFI_UNSUPPORTED;
  }

  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **)&HobList);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Hob.Raw = HobList;
  if (GET_HOB_TYPE (Hob) != EFI_HOB_TYPE_HANDOFF) {
    return Status;
  }
  BootMode = Hob.HandoffInformationTable->BootMode;

  PciAddress = PCI_EXPRESS_LIB_ADDRESS (
                 SgDxeInfoData->MasterDgpuBus,
                 DGPU_DEVICE_NUM,
                 DGPU_FUNCTION_NUM,
                 NVIDIA_DGPU_HDA_REGISTER
                 );
  if ((BootMode != BOOT_ON_S3_RESUME) && (BootMode != BOOT_ON_S4_RESUME)) {
    PciExpressAnd32 (PciAddress, (UINT32)~(BIT25));
  }

  if (BootMode == BOOT_ON_S4_RESUME) {
    if (((SgDxeInfoData->OptimusFlag) & BIT0) == Inactive) {
      PciExpressAnd32 (PciAddress, (UINT32)~(BIT25));
    } else {
      PciExpressOr32 (PciAddress, (UINT32)BIT25);
    }
  }
  gBS->CloseEvent (Event);

  return EFI_SUCCESS;
}

/**

  Search discrete graphics VBIOS location and save into SG information data.

  @param[in, out] SgDxeInfoData   A pointer of SG DXE information data structure,
                                  this driver or SG Operation Region will use these data.

  @retval EFI_SUCCESS    Search discrete graphics VBIOS successfully.
  @retval !EFI_SUCCESS   Search discrete graphics VBIOS failed.

**/
STATIC
EFI_STATUS
SearchDiscreteGraphicsVbios (
  IN OUT SG_DXE_INFORMATION_DATA              *SgDxeInfoData
  )
{
  EFI_HANDLE                                  *HandleBuffer;
  EFI_PCI_IO_PROTOCOL                         *PciIo;
  EFI_STATUS                                  Status;
  PCI_3_0_DATA_STRUCTURE                      *PcirBlockPtr;
  PCI_EXPANSION_ROM_HEADER                    *VBiosRomImage;
  UINTN                                       HandleCount;
  UINTN                                       Index;

  //
  // Get all PCI IO protocols
  //
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

  //
  // Find the video BIOS by checking each PCI IO handle for DGPU video
  // BIOS OPROM.
  //
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiPciIoProtocolGuid,
                    (VOID **)&PciIo
                    );
    if (EFI_ERROR (Status) || (PciIo->RomImage == NULL)) {
      //
      // If this PCI device doesn't have a ROM image, skip to the next device.
      //
      continue;
    }
    VBiosRomImage = PciIo->RomImage;

    //
    // Get pointer to PCIR structure
    //
    PcirBlockPtr = (PCI_3_0_DATA_STRUCTURE *)((UINTN)VBiosRomImage + VBiosRomImage->PcirOffset);

    //
    // Check if we have an video BIOS OPROM for DGPU.
    //
    if ((VBiosRomImage->Signature == PCI_EXPANSION_ROM_HEADER_SIGNATURE) &&
        (SgDxeInfoData->MasterDgpuVendorId == PcirBlockPtr->VendorId) &&
        (PcirBlockPtr->ClassCode[2] == PCI_CLASS_DISPLAY)) {
      SgDxeInfoData->Vbios.Address = (UINTN)PciIo->RomImage;
      SgDxeInfoData->Vbios.Size = (PcirBlockPtr->ImageLength * 0x200);
      FreePool (HandleBuffer);
      return EFI_SUCCESS;
    }
  }
  FreePool (HandleBuffer);

  return EFI_NOT_FOUND;
}

/**

  Execute discrete graphics VBIOS for ATI graphics initialize.

  @param[in, out] SgDxeInfoData   A pointer of SG DXE information data structure,
                                  this driver or SG Operation Region will use these data.
  @param[in]      SgInfoDataHob   A pointer of SG information data HOB,
                                  SG PEI Module created this HOB, and passes the data from PEI phase.

  @retval EFI_SUCCESS    Execute discrete graphics VBIOS successfully.
  @retval !EFI_SUCCESS   Execute discrete graphics VBIOS failed.

**/
STATIC
EFI_STATUS
ExecuteDiscreteGraphicsVbios (
  IN OUT SG_DXE_INFORMATION_DATA              *SgDxeInfoData,
  IN SG_INFORMATION_DATA_HOB                  *SgInfoDataHob
  )
{
  EFI_IA32_REGISTER_SET                       RegSet;
  EFI_LEGACY_BIOS_PROTOCOL                    *LegacyBios;
  EFI_PHYSICAL_ADDRESS                        ImageLocation;
  EFI_STATUS                                  Status;
  UINTN                                       Offset;
  UINTN                                       PegBridgeCmdRegAddr;
  UINTN                                       PegDeviceCmdRegAddr;
  UINTN                                       VbiosPages;
//[-start-130515-IB10920024-add]//
  UINTN                                       *VbiosAddress;
  UINTN                                       VbiosSize;

  VbiosSize = SgDxeInfoData->Vbios.Size;
  Status = (gBS->AllocatePool)(EfiBootServicesData, VbiosSize, &VbiosAddress);
  if (EFI_ERROR (Status)) {
    return Status;
  }
//[-end-130515-IB10920024-add]//
  //
  // Memory IO Bus Master needs to be enabled when we execute the VBIOS.
  //
  PegBridgeCmdRegAddr = PCI_EXPRESS_LIB_ADDRESS (
                          SgInfoDataHob->DgpuBridgeBus,
                          SgInfoDataHob->DgpuBridgeDevice,
                          SgInfoDataHob->DgpuBridgeFunction,
                          PCI_CMD
                          );
  PegDeviceCmdRegAddr = PCI_EXPRESS_LIB_ADDRESS (
                          SgDxeInfoData->MasterDgpuBus,
                          DGPU_DEVICE_NUM,
                          DGPU_FUNCTION_NUM,
                          PCI_CMD
                          );
  //
  // Enable Memory Access, IO Access Bus Master enable on PEG root port and PEG device.
  //
  PciExpressOr16 (PegBridgeCmdRegAddr, BIT0 + BIT1 + BIT2);
  PciExpressOr16 (PegDeviceCmdRegAddr, BIT0 + BIT1 + BIT2);
  //
  // Allocate under 1MB memory region (less than 640 KB).
  //
  VbiosPages = ((SgDxeInfoData->Vbios.Size) / 0x1000) + 1;
  ImageLocation = (EFI_PHYSICAL_ADDRESS)(UINTN)0xA0000;
  Status  = gBS->AllocatePages (AllocateMaxAddress, EfiBootServicesCode, VbiosPages, &ImageLocation);
  if (!EFI_ERROR (Status)) {
    ZeroMem ((VOID *)(UINTN)ImageLocation, (VbiosPages * 4096));
    //
    // After allocation copy VBIOS to buffer
    //
    CopyMem ((VOID *)(UINTN)ImageLocation, (VOID *)SgDxeInfoData->Vbios.Address, SgDxeInfoData->Vbios.Size);
    Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **)&LegacyBios);
    if (!EFI_ERROR (Status)) {
      ZeroMem (&RegSet, sizeof (EFI_IA32_REGISTER_SET));
      RegSet.H.AH = SgDxeInfoData->MasterDgpuBus;
      Offset = MemoryRead16 ((UINTN)ImageLocation + 0x40);
      LegacyBios->FarCall86 (
                    LegacyBios,
                    ((UINT16)(RShiftU64 ((ImageLocation & 0x000FFFF0), 4))),
                    ((UINT16)Offset),
                    &RegSet,
                    NULL,
                    0
                    );
      Offset = (MemoryRead16 ((UINTN)ImageLocation + 0x42)) + (UINTN)ImageLocation;
      if (MemoryRead16 ((UINTN)ImageLocation + 0x44) == 0x0) {
        SgDxeInfoData->Vbios.Size = (MemoryRead8 ((UINTN)ImageLocation + 0x2)) * 512;
      } else {
        SgDxeInfoData->Vbios.Size = (MemoryRead16 ((UINTN)ImageLocation + 0x44)) * 512;
      }
      //
      // Copy Oprom to allocated space
      //
//[-start-130515-IB109200243-modify]//
      //SgDxeInfoData->Vbios.Address = Offset;
      CopyMem (VbiosAddress, (VOID *)Offset, VbiosSize);
      SgDxeInfoData->Vbios.Address = (UINTN)VbiosAddress;
//[-end-130515-IB10920024-modify]//
      ZeroMem ((VOID *)(UINTN)ImageLocation, SgDxeInfoData->Vbios.Size);
    }
    FreePages ((VOID *)(UINTN)ImageLocation, VbiosPages);
  }
  //
  // Disable Memory Access, IO Access Bus Master enable on PEG device and PEG root port.
  //
  PciExpressAnd16 (PegDeviceCmdRegAddr, BIT0 + BIT1 + BIT2);
  PciExpressAnd16 (PegBridgeCmdRegAddr, BIT0 + BIT1 + BIT2);

  return Status;
}

/**

  Load Switchable Graphics own SSDT (Secondary System Description Table) and initialize
  Switchable Graphics own operation region for ASL (ACPI Source Language) code usage.

  @param[in] SgDxeInfoData   A pointer of SG DXE information data structure,
                             this driver or SG Operation Region will use these data.
  @param[in] SgInfoDataHob   A pointer of SG information data HOB,
                             SG PEI Module created this HOB, and passes the data from PEI phase.

  @retval EFI_SUCCESS    Load and set Switchable Graphics SSDT successfully.
  @retval !EFI_SUCCESS   Load or set Switchable Graphics SSDT failed.

**/
STATIC
EFI_STATUS
SetSwitchableGraphicsSsdt (
  IN SG_DXE_INFORMATION_DATA                  *SgDxeInfoData,
  IN SG_INFORMATION_DATA_HOB                  *SgInfoDataHob
  )
{
  EFI_ACPI_COMMON_HEADER                      *CurrentTable;
  EFI_ACPI_DESCRIPTION_HEADER                 *TempTable;
  EFI_ACPI_TABLE_PROTOCOL                     *AcpiTable;
  EFI_FIRMWARE_VOLUME2_PROTOCOL               *FirmwareVolume;
  EFI_FV_FILE_ATTRIBUTES                      Attributes;
  EFI_FV_FILETYPE                             FileType;
  EFI_GUID                                    *SsdtFileGuid;
  EFI_HANDLE                                  *HandleBuffer;
  EFI_STATUS                                  Status;
  UINT32                                      FvStatus;
  UINTN                                       TableKey;
  UINTN                                       Index;
  UINTN                                       NumberOfHandles;
  UINTN                                       Size;

  CurrentTable   = NULL;
  FirmwareVolume = NULL;
  SsdtFileGuid   = NULL;
  TempTable      = NULL;

  //
  // Choose the SSDT table base on Vendor ID and SG mode.
  //
  if ((FeaturePcdGet (PcdNvidiaOptimusSupported)) && (SgDxeInfoData->MasterDgpuVendorId == NVIDIA_VID)) {
    if (SgInfoDataHob->SgMode == SgModeDgpu) {
      SsdtFileGuid = &(SgInfoDataHob->NvidiaDiscreteSsdtGuid);
    } else {
      if (SgInfoDataHob->IsUltBoard) {
        SsdtFileGuid = &(SgInfoDataHob->NvidiaUltOptimusSsdtGuid);
      } else {
        SsdtFileGuid = &(SgInfoDataHob->NvidiaOptimusSsdtGuid);
      }
    }
  }
  if ((FeaturePcdGet (PcdAmdPowerXpressSupported)) && (SgDxeInfoData->MasterDgpuVendorId == AMD_VID)) {
    if (SgInfoDataHob->SgMode == SgModeDgpu) {
      SsdtFileGuid = &(SgInfoDataHob->AmdDiscreteSsdtGuid);
    } else {
      if (SgInfoDataHob->IsUltBoard) {
        SsdtFileGuid = &(SgInfoDataHob->AmdUltPowerXpressSsdtGuid);
      } else {
        SsdtFileGuid = &(SgInfoDataHob->AmdPowerXpressSsdtGuid);
      }
    }
  }
  if (SsdtFileGuid == NULL) {
    return EFI_UNSUPPORTED;
  }

  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **)&AcpiTable);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Looking for FV with ACPI storage file
  //
  for (Index = 0; Index < NumberOfHandles; Index++) {
    Status = gBS->HandleProtocol (HandleBuffer[Index], &gEfiFirmwareVolume2ProtocolGuid, (VOID **)&FirmwareVolume);
    if (EFI_ERROR (Status)) {
      continue;
    }
    FvStatus = 0;
    Size     = 0;
    Status = FirmwareVolume->ReadFile (
                               FirmwareVolume,
                               SsdtFileGuid,
                               NULL,
                               &Size,
                               &FileType,
                               &Attributes,
                               &FvStatus
                               );
    if (Status == EFI_SUCCESS) {
      break;
    }
  }
  FreePool (HandleBuffer);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  //
  // Read tables from the storage file.
  //
  TableKey = 0;
  Status = FirmwareVolume->ReadSection (
                             FirmwareVolume,
                             SsdtFileGuid,
                             EFI_SECTION_RAW,
                             0,
                             &CurrentTable,
                             &Size,
                             &FvStatus
                             );
  if (!EFI_ERROR (Status)) {
    TempTable = AllocateZeroPool (CurrentTable->Length);
    if (!EFI_ERROR (Status)) {
      CopyMem (TempTable, CurrentTable, CurrentTable->Length);
      Status = InitializeOpRegion ((EFI_ACPI_DESCRIPTION_HEADER*)TempTable, SgDxeInfoData, SgInfoDataHob);
      if (!EFI_ERROR (Status)) {
        Status = AcpiTable->InstallAcpiTable (AcpiTable, TempTable, CurrentTable->Length, &TableKey);
      }
      FreePool (CurrentTable);
      FreePool (TempTable);
    }
  }

  return Status;
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
STATIC
EFI_STATUS
InitializeOpRegion (
  IN EFI_ACPI_DESCRIPTION_HEADER              *NvStoreTable,
  IN SG_DXE_INFORMATION_DATA                  *SgDxeInfoData,
  IN SG_INFORMATION_DATA_HOB                  *SgInfoDataHob
  )
{
  EFI_ACPI_DESCRIPTION_HEADER                 *Table;
  EFI_STATUS                                  Status;
  OPERATION_REGION_AMD                        *AmdOpRegion;
  OPERATION_REGION_NVIDIA                     *NvidiaOpRegion;
  OPERATION_REGION_SG                         *SgOpRegion;
  OPERATION_REGION_VBIOS                      *VbiosOpRegion;
  UINT8                                       *SsdtPointer;
  UINT32                                      *Signature;

  Table = NvStoreTable;
  for (SsdtPointer =  (UINT8 *)((UINTN)Table + sizeof (EFI_ACPI_DESCRIPTION_HEADER)); \
       SsdtPointer <= (UINT8 *)((UINTN)Table + (UINTN)(Table->Length)); SsdtPointer++) {
    Signature = (UINT32*)SsdtPointer;
    switch (*Signature) {
      case (SIGNATURE_32 ('S', 'G', 'O', 'P')): // SG operation region initialize
        if (IsAmlOpRegionObject (SsdtPointer)) {
          Status = gBS->AllocatePool (EfiACPIMemoryNVS, sizeof (OPERATION_REGION_SG), (VOID **)&SgOpRegion);
          if (!EFI_ERROR (Status)) {
            ZeroMem (SgOpRegion, sizeof (OPERATION_REGION_SG));
            UpdateSgOpRegion (SgOpRegion, SgDxeInfoData, SgInfoDataHob);
            SetOpRegion (SsdtPointer, SgOpRegion, sizeof (OPERATION_REGION_SG));
          }
        }
        break;
      case (SIGNATURE_32 ('V', 'B', 'O', 'R')): // VBIOS operation region initialize
        if (IsAmlOpRegionObject (SsdtPointer)) {
          Status = gBS->AllocatePool (EfiACPIMemoryNVS, sizeof (OPERATION_REGION_VBIOS), (VOID **)&VbiosOpRegion);
          if (!EFI_ERROR (Status)) {
            ZeroMem (VbiosOpRegion, sizeof (OPERATION_REGION_VBIOS));
            VbiosOpRegion->RVBS = (UINT32)SgDxeInfoData->Vbios.Size;
            CopyMem (
              (VOID *)(VbiosOpRegion->VBOIS),
              (VOID *)(SgDxeInfoData->Vbios.Address),
              SgDxeInfoData->Vbios.Size
              );
            SgDxeInfoData->Vbios.Address = (UINTN)(VbiosOpRegion->VBOIS);
            SetOpRegion (SsdtPointer, VbiosOpRegion, sizeof (OPERATION_REGION_VBIOS));
          }
        }
        break;
      case (SIGNATURE_32 ('A', 'O', 'P', 'R')): // AMD operation region initialize
        if ((FeaturePcdGet (PcdAmdPowerXpressSupported)) && IsAmlOpRegionObject (SsdtPointer)) {
          Status = gBS->AllocatePool (EfiACPIMemoryNVS, sizeof (OPERATION_REGION_AMD), (VOID **)&AmdOpRegion);
          if (!EFI_ERROR (Status)) {
            ZeroMem (AmdOpRegion, sizeof (OPERATION_REGION_AMD));
            UpdateAmdOpRegion (AmdOpRegion, SgDxeInfoData, SgInfoDataHob);
            SetOpRegion (SsdtPointer, AmdOpRegion, sizeof (OPERATION_REGION_AMD));
          }
        }
        SsdtPointer = (UINT8 *)((UINTN)Table + (UINTN)(Table->Length));
        break;
      case (SIGNATURE_32 ('N', 'O', 'P', 'R')): // nVIDIA operation region initialize
        if ((FeaturePcdGet (PcdNvidiaOptimusSupported)) && IsAmlOpRegionObject (SsdtPointer)) {
          Status = gBS->AllocatePool (EfiACPIMemoryNVS, sizeof (OPERATION_REGION_NVIDIA), (VOID **)&NvidiaOpRegion);
          if (!EFI_ERROR (Status)) {
            ZeroMem (NvidiaOpRegion, sizeof (OPERATION_REGION_NVIDIA));
            UpdateNvidiaOpRegion (NvidiaOpRegion, SgDxeInfoData, SgInfoDataHob);
            SetOpRegion (SsdtPointer, NvidiaOpRegion, sizeof (OPERATION_REGION_NVIDIA));
          }
        }
        SsdtPointer = (UINT8 *)((UINTN)Table + (UINTN)(Table->Length));
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
STATIC
VOID
UpdateSgOpRegion (
  IN OUT OPERATION_REGION_SG                  *SgOpRegion,
  IN SG_DXE_INFORMATION_DATA                  *SgDxeInfoData,
  IN SG_INFORMATION_DATA_HOB                  *SgInfoDataHob
  )
{
  UINT8                                       CapOffset;
  UINTN                                       PciAddress;

  //
  // SG Mode for ASL usage
  //
  SgOpRegion->SgMode               = SgInfoDataHob->SgMode;
  SgOpRegion->GpioBaseAddress      = SgDxeInfoData->GpioBaseAddress;
  PciAddress = PCI_EXPRESS_LIB_ADDRESS (
                 SgInfoDataHob->DgpuBridgeBus,
                 SgInfoDataHob->DgpuBridgeDevice,
                 SgInfoDataHob->DgpuBridgeFunction,
                 0x0
                 );
  SgOpRegion->DgpuPcieCfgBaseAddress = (UINT32)(PcdGet64 (PcdPciExpressBaseAddress) + PciAddress);
  SgOpRegion->IsUltBoard = Inactive;
  if (SgInfoDataHob->IsUltBoard) {
    SgOpRegion->IsUltBoard = Active;
  }

  if (SgInfoDataHob->SgMode == SgModeMuxless) {
    if (SgInfoDataHob->SgGpioSupport)  {
      //
      // GPIO Assignment for ASL usage
      //
      SgOpRegion->SgGPIOSupport   = SgInfoDataHob->SgGpioSupport;
      SgOpRegion->SgDgpuPwrOK     = SgInfoDataHob->SgDgpuPwrOk;
      SgOpRegion->SgDgpuHoldRst   = SgInfoDataHob->SgDgpuHoldRst;
      SgOpRegion->SgDgpuPwrEnable = SgInfoDataHob->SgDgpuPwrEnable;
      SgOpRegion->SgDgpuPrsnt     = SgInfoDataHob->SgDgpuPrsnt;
      SgOpRegion->ActiveDgpuPwrEnableDelayTime = SgInfoDataHob->ActiveDgpuPwrEnableDelay;
      SgOpRegion->ActiveDgpuHoldRstDelayTime   = SgInfoDataHob->ActiveDgpuHoldRstDelay;
      SgOpRegion->InctiveDgpuHoldRstDelayTime  = SgInfoDataHob->InactiveDgpuHoldRstDelay;
    }

    //
    // PEG Endpoint Base Addresses and Capability Structure Offsets for ASL usage
    //
    PciAddress = PCI_EXPRESS_LIB_ADDRESS (
                   SgDxeInfoData->MasterDgpuBus,
                   DGPU_DEVICE_NUM,
                   DGPU_FUNCTION_NUM,
                   0x0
                   );
    SgOpRegion->EndpointBaseAddress = (UINT32)(PcdGet64 (PcdPciExpressBaseAddress) + PciAddress);
    SgOpRegion->XPcieCfgBaseAddress = (UINT32)(PcdGet64 (PcdPciExpressBaseAddress));
    PciFindCapId (SgDxeInfoData->MasterDgpuBus, DGPU_DEVICE_NUM, DGPU_FUNCTION_NUM, PEG_CAP_ID, &CapOffset);
    SgOpRegion->EndpointPcieCapBaseAddress = CapOffset;
  }
}

/**

  Update AMD own operation region for ASL (ACPI Source Language) code usage.

  @param[in] AmdOpRegion     A pointer of AMD own operation region.
  @param[in] SgDxeInfoData   A pointer of SG DXE information data structure,
                             this driver or SG Operation Region will use these data.
  @param[in] SgInfoDataHob   A pointer of SG information data HOB,
                             SG PEI Module created this HOB, and passes the data from PEI phase.

  @retval None.

**/
STATIC
VOID
UpdateAmdOpRegion (
  IN OUT OPERATION_REGION_AMD                 *AmdOpRegion,
  IN SG_DXE_INFORMATION_DATA                  *SgDxeInfoData,
  IN SG_INFORMATION_DATA_HOB                  *SgInfoDataHob
  )
{
  if (SgInfoDataHob->SgMode == SgModeMuxless) {
    AmdOpRegion->ExpansionMode = SgInfoDataHob->PannelScaling; // Expansion Mode
    if ((SgInfoDataHob->PowerXpressSetting == MuxlessDynamic) ||
        (SgInfoDataHob->PowerXpressSetting == MuxlessFixedDynamic)) {
      AmdOpRegion->PxDynamicSupportState = Support; // Dynamic Support State 1: dynamic, 0: Non-dynamic
    }
    if (SgInfoDataHob->PowerXpressSetting != MuxlessDynamic) {
      AmdOpRegion->PxFixedSupportState = Support; // Fixed   Support State 1: Fixed,   0: Non-fixed
    }
    if (SgInfoDataHob->PowerXpressSetting == FullDgpuPowerOffDynamic) {
      AmdOpRegion->PxDynamicSupportState     = Support;   // Dynamic Support State 1: dynamic, 0: Non-dynamic
      AmdOpRegion->PxFixedSupportState       = Unsupport; // Fixed   Support State 1: Fixed,   0: Non-fixed
      AmdOpRegion->PxFullDgpuPowerOffDynamic = Support;   // Full dGPU PowerOff Dynamic mode support
    }
    if (SgInfoDataHob->PowerXpressSetting == FixedFullDgpuPowerOffDynamic) {
      AmdOpRegion->PxDynamicSupportState     = Support; // Dynamic Support State 1: dynamic, 0: Non-dynamic
      AmdOpRegion->PxFixedSupportState       = Support; // Fixed   Support State 1: Fixed,   0: Non-fixed
      AmdOpRegion->PxFullDgpuPowerOffDynamic = Support; // Full dGPU PowerOff Dynamic mode support
    }
  }
  if (SgInfoDataHob->SgMode == SgModeDgpu) {
    AmdOpRegion->ExpansionMode = SgInfoDataHob->PannelScaling; // Expansion Mode
    AmdOpRegion->SlaveDgpuSupport = Inactive;
    if (SgInfoDataHob->SlaveMxmGpuSupport) {
      AmdOpRegion->SlaveDgpuSupport = Active;
    }
  }
}

/**

  Update nVIDIA own operation region for ASL (ACPI Source Language) code usage.

  @param[in] NvidiaOpRegion   A pointer of nVIDIA own operation region.
  @param[in] SgDxeInfoData    A pointer of SG DXE information data structure,
                              this driver or SG Operation Region will use these data.
  @param[in] SgInfoDataHob    A pointer of SG information data HOB,
                              SG PEI Module created this HOB, and passes the data from PEI phase.

  @retval None.

**/
STATIC
VOID
UpdateNvidiaOpRegion (
  IN OUT OPERATION_REGION_NVIDIA              *NvidiaOpRegion,
  IN SG_DXE_INFORMATION_DATA                  *SgDxeInfoData,
  IN SG_INFORMATION_DATA_HOB                  *SgInfoDataHob
  )
{
  if (SgInfoDataHob->SgMode == SgModeMuxless) {
    NvidiaOpRegion->DgpuHotPlugSupport      = SgInfoDataHob->OptimusDgpuHotPlugSupport;
    NvidiaOpRegion->DgpuPowerControlSupport = SgInfoDataHob->OptimusDgpuPowerControlSupport;
    NvidiaOpRegion->GpsFeatureSupport       = SgInfoDataHob->GpsFeatureSupport;
    NvidiaOpRegion->VenturaFeatureSupport   = SgInfoDataHob->VenturaFeatureSupport;
    NvidiaOpRegion->OptimusGc6Support       = SgInfoDataHob->OptimusGc6FeatureSupport;
    NvidiaOpRegion->SpbConfig = Active; // SpbConfig bit0: SPB 1: enable, 0: disable
    NvidiaOpRegion->ExpansionMode = SgInfoDataHob->PannelScaling; // Expansion Mode
    NvidiaOpRegion->MxmBinarySize = (UINT32)SgDxeInfoData->MasterMxmBinFile.Size; // MXM bin file size (bits)
    //
    // Copy MXM bin file to OpRegion
    //
    CopyMem (
      (VOID *)(NvidiaOpRegion->MxmBinaryBuffer),
      (VOID *)(SgDxeInfoData->MasterMxmBinFile.Address),
      SgDxeInfoData->MasterMxmBinFile.Size
      );
  }
  if (SgInfoDataHob->SgMode == SgModeDgpu) {
    NvidiaOpRegion->GpsFeatureSupport     = SgInfoDataHob->GpsFeatureSupport;
    NvidiaOpRegion->VenturaFeatureSupport = SgInfoDataHob->VenturaFeatureSupport;
    NvidiaOpRegion->SpbConfig = Active; // SpbConfig bit0: SPB 1: enable, 0: disable
    NvidiaOpRegion->SlaveDgpuSupport = Inactive;
    if (SgInfoDataHob->SlaveMxmGpuSupport) {
      NvidiaOpRegion->SlaveDgpuSupport = Active;
    }
    NvidiaOpRegion->MxmBinarySize = (UINT32)SgDxeInfoData->MasterMxmBinFile.Size; // MXM bin file size (bits)
    //
    // Copy MXM bin file to OpRegion
    //
    CopyMem (
      (VOID *)(NvidiaOpRegion->MxmBinaryBuffer),
      (VOID *)(SgDxeInfoData->MasterMxmBinFile.Address),
      SgDxeInfoData->MasterMxmBinFile.Size
      );
    NvidiaOpRegion->SlaveMxmBinarySize = (UINT32)SgDxeInfoData->SlaveMxmBinFile.Size;
    //
    // Copy Slave MXM bin file to OpRegion
    //
    CopyMem (
      (VOID *)(NvidiaOpRegion->SlaveMxmBinaryBuffer),
      (VOID *)(SgDxeInfoData->SlaveMxmBinFile.Address),
      SgDxeInfoData->SlaveMxmBinFile.Size
      );
  }
}

/**

  Update Switchable Graphics information protocol related data for nVIDIA INT 15 callback usage.
  Create Exit Boot Service event and Legacy Boot event to set the Secondary Grcphics Command Register.

  @param[in] SgDxeInfoData   A pointer of SG DXE information data structure,
                             this driver or SG Operation Region will use these data.
  @param[in] SgInfoDataHob   A pointer of SG information data HOB,
                             SG PEI Module created this HOB, and passes the data from PEI phase.

  @retval EFI_SUCCESS    Update Switchable Graphics information protocol successfully.
  @retval !EFI_SUCCESS   Locate Switchable Graphics information protocol failed.

**/
STATIC
EFI_STATUS
SgInformationUpdate (
  IN SG_DXE_INFORMATION_DATA                  *SgDxeInfoData,
  IN SG_INFORMATION_DATA_HOB                  *SgInfoDataHob
  )
{
  EFI_EVENT                                   ExitBootServicesEvent;
  EFI_EVENT                                   LegacyBootEvent;
  EFI_EVENT                                   ReadyToBootEvent;
  EFI_STATUS                                  Status;
  H2O_SG_INFO_PROTOCOL                        *SgInfoProtocol;

  Status = gBS->LocateProtocol (&gH2OSwitchableGraphicsInfoProtocolGuid, NULL, (VOID **)&SgInfoProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SgInfoProtocol->GpioBaseAddress = SgDxeInfoData->GpioBaseAddress;
  SgInfoProtocol->IsUltBoard      = SgInfoDataHob->IsUltBoard;
  SgInfoProtocol->SgMode          = SgInfoDataHob->SgMode;
  if (FeaturePcdGet (PcdNvidiaOptimusSupported)) {
    SgInfoProtocol->Vbios.Address            = SgDxeInfoData->Vbios.Address;
    SgInfoProtocol->Vbios.Size               = SgDxeInfoData->Vbios.Size;
    SgInfoProtocol->MasterMxmBinFile.Address = SgDxeInfoData->MasterMxmBinFile.Address;
    SgInfoProtocol->MasterMxmBinFile.Size    = SgDxeInfoData->MasterMxmBinFile.Size;
    SgInfoProtocol->SlaveMxmBinFile.Address  = SgDxeInfoData->SlaveMxmBinFile.Address;
    SgInfoProtocol->SlaveMxmBinFile.Size     = SgDxeInfoData->SlaveMxmBinFile.Size;
  }

  if ((SgDxeInfoData->SlaveMxmGpuSupport) && ((SgDxeInfoData->SlaveDgpuVendorId) != 0xFFFF) &&
      (SgInfoDataHob->SgMode == SgModeDgpu)) {
    Status = EfiCreateEventReadyToBootEx (
               TPL_CALLBACK,
               CloseDiscreteSecondaryHdAudio,
               SgDxeInfoData,
               &ReadyToBootEvent
               );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  if (SgInfoDataHob->BootType == EfiBootType) {
    Status = gBS->CreateEvent (
                    EVT_SIGNAL_EXIT_BOOT_SERVICES,
                    TPL_NOTIFY,
                    SetSecondaryGrcphicsCommandRegister,
                    SgDxeInfoData,
                    &ExitBootServicesEvent
                    );
  } else {
    Status = EfiCreateEventLegacyBootEx (
               TPL_NOTIFY,
               SetSecondaryGrcphicsCommandRegister,
               SgDxeInfoData,
               &LegacyBootEvent
               );
  }

  return Status;
}

/**

  Set Secondary Graphics Command Register for avoid I/O resource crash.

  @param[in]  Event     The Event this notify function registered to.
  @param[in]  Context   Pointer to the context data registerd to the Event.

  @retval EFI_SUCCESS   Set secondary Graphics command register successfully.

**/
EFI_STATUS
EFIAPI
SetSecondaryGrcphicsCommandRegister (
  IN EFI_EVENT                                Event,
  IN VOID                                     *Context
  )
{
  SG_DXE_INFORMATION_DATA                     *SgDxeInfoData;
  UINT8                                       Data;
  UINTN                                       PciAddress;

  SgDxeInfoData = (SG_DXE_INFORMATION_DATA *)Context;
  if (SgDxeInfoData == NULL) {
    return EFI_UNSUPPORTED;
  }
  Data = 0x07;
  //
  // DEVEN register bit 4 is Internal Graphics Engine (D2EN).
  //
  if (((SgDxeInfoData->SaDeven) & B_SA_DEVEN_D2EN_MASK) != Inactive) {
    //
    // If dGPU and iGPU exist at the same time, close dGPU IO port.
    // Different vendor requests to write command register with different value.
    //
    if ((SgDxeInfoData->MasterDgpuVendorId) != 0xFFFF) {
      PciAddress = PCI_EXPRESS_LIB_ADDRESS (
                     SgDxeInfoData->MasterDgpuBus,
                     DGPU_DEVICE_NUM,
                     DGPU_FUNCTION_NUM,
                     PCI_CMD
                     );
      if (SgDxeInfoData->MasterDgpuVendorId == NVIDIA_VID) {
        Data = SgDxeInfoData->AmdSecondaryGrcphicsCommandRegister;
      }
      if (SgDxeInfoData->MasterDgpuVendorId == AMD_VID) {
        Data = SgDxeInfoData->NvidiaSecondaryGrcphicsCommandRegister;
      }
      PciExpressAndThenOr8 (PciAddress, Data, Data);
    }
  }
  if ((SgDxeInfoData->SlaveMxmGpuSupport) && ((SgDxeInfoData->SlaveDgpuVendorId) != 0xFFFF)) {
    PciAddress = PCI_EXPRESS_LIB_ADDRESS (
                   SgDxeInfoData->SlaveDgpuBus,
                   DGPU_DEVICE_NUM,
                   DGPU_FUNCTION_NUM,
                   PCI_CMD
                   );
    if (SgDxeInfoData->SlaveDgpuVendorId == NVIDIA_VID) {
      Data = SgDxeInfoData->AmdSecondaryGrcphicsCommandRegister;
    }
    if (SgDxeInfoData->SlaveDgpuVendorId == AMD_VID) {
      Data = SgDxeInfoData->NvidiaSecondaryGrcphicsCommandRegister;
    }
    PciExpressAndThenOr8 (PciAddress, Data, Data);
  }
  FreeSgDxeInfoData (&SgDxeInfoData);
  gBS->CloseEvent (Event);

  return EFI_SUCCESS;
}

/**

  Discrete GPU only mode Close secondary HD Audio device.
  Close nVIDIA slave HD Audio device.
    Discrete GPU memory mapping register 0x488 Bit 25 is HD audio device power enable bit.
  Close AMD slave HD Audio device.
    1. io_wr offset=0x0, data=0x541C
    2. rd_data = io_rd  offset=0x4
    3. io_wr offset=0x4, data=rd_data & ~0x80 (for masking off STRAP_BIF_AUDIO_EN)
    For reference, here's the register composition:
    BIF_PINSTRAP0 <BIFDEC:0x541C> 32
    {
      STRAP_BIF_GEN2_EN_A             0     NUM DEF=1;  
      STRAP_BIF_CLK_PM_EN             1     NUM DEF=0;
      STRAP_BIF_BIOS_ROM_EN           2     NUM DEF=0;
      STRAP_BIF_RX_PLL_CALIB_BYPASS   3     NUM DEF=0;
      STRAP_BIF_MEM_AP_SIZE_PIN       6:4   NUM DEF=0;
      STRAP_BIF_AUDIO_EN_PIN          7     NUM DEF=0;
      STRAP_BIF_VGA_DIS_PIN           8     NUM DEF=0;
      STRAP_TX_DEEMPH_EN              9     NUM DEF=1;
      STRAP_TX_PWRS_ENB               10    NUM DEF=1;
    }   

  @param[in]  Event     The Event this notify function registered to.
  @param[in]  Context   Pointer to the context data registerd to the Event.

  @retval EFI_SUCCESS   Set secondary Graphics command register successfully.

**/
EFI_STATUS
EFIAPI
CloseDiscreteSecondaryHdAudio (
  IN EFI_EVENT                                Event,
  IN VOID                                     *Context
  )
{
  SG_DXE_INFORMATION_DATA                     *SgDxeInfoData;
  UINT16                                      SlavePegIoAddress;
  UINTN                                       PciAddress;

  SgDxeInfoData = (SG_DXE_INFORMATION_DATA *)Context;
  if (SgDxeInfoData == NULL) {
    return EFI_UNSUPPORTED;
  }

  if (SgDxeInfoData->SlaveDgpuVendorId == NVIDIA_VID) {
    PciAddress = PCI_EXPRESS_LIB_ADDRESS (
                   SgDxeInfoData->SlaveDgpuBus,
                   DGPU_DEVICE_NUM,
                   DGPU_FUNCTION_NUM,
                   NVIDIA_DGPU_HDA_REGISTER
                   );
    PciExpressAnd32 (PciAddress, (UINT32)~(BIT25));
  } else if (SgDxeInfoData->SlaveDgpuVendorId == AMD_VID) {
    PciAddress = PCI_EXPRESS_LIB_ADDRESS (
                   SgDxeInfoData->SlaveDgpuBus,
                   DGPU_DEVICE_NUM,
                   DGPU_FUNCTION_NUM,
                   PCI_BAR4
                   );
    SlavePegIoAddress = PciExpressRead16 (PciAddress);
    SlavePegIoAddress &= ~(BIT0);
    IoWrite32 (SlavePegIoAddress, 0x541C);
    IoWrite32 ((SlavePegIoAddress + 4), (IoRead32 (SlavePegIoAddress + 4) & ~(BIT7)));
  }

  return EFI_SUCCESS;
}

/**

  The function is for free SgDxeInfoData pointer.

  @param[in, out] SgDxeInfoData   A double pointer of SG DXE information data structure.

  @retval None.

**/
VOID
EFIAPI
FreeSgDxeInfoData (
  IN OUT SG_DXE_INFORMATION_DATA              **SgDxeInfoData
  )
{
  if ((*SgDxeInfoData) != NULL) {
    FreePool (*SgDxeInfoData);
    (*SgDxeInfoData) = NULL;
  }
}