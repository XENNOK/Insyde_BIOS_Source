/** @file
  This function offers an interface for OEM code to change the MXM GPU power enable sequence.

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

#include <Library/PeiOemSvcChipsetLib.h>
#if 0
//
// If you need modify SG information HOB data or use SG PPI to change power enable sequence,
// please remember add HobLib and PeiServicesLib into project own INF file.
//
#include <Guid/SgInfoHob.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/SwitchableGraphics.h>
#include <SwitchableGraphicsDefine.h>

typedef
EFI_STATUS
(EFIAPI *SWITCHABLE_GRAPHICS_CUSTOMIZATION) (
  IN H2O_SWITCHABLE_GRAPHICS_PPI              *SgPpi,
  IN SG_INFORMATION_DATA_HOB                  *SgInfoDataHob
  );

//
// Function Prototypes
//
STATIC
EFI_STATUS
GetSgInfoHobAndLocateSgPpi (
  IN OUT H2O_SWITCHABLE_GRAPHICS_PPI          **SgPpi,
  IN OUT SG_INFORMATION_DATA_HOB              **SgInfoDataHob
  );

EFI_STATUS
BaseOnSkuIdChangeSgMode (
  IN H2O_SWITCHABLE_GRAPHICS_PPI              *SgPpi,
  IN SG_INFORMATION_DATA_HOB                  *SgInfoDataHob
  );

EFI_STATUS
BaseOnSgModePowerEnableGpuThroughEc (
  IN H2O_SWITCHABLE_GRAPHICS_PPI              *SgPpi,
  IN SG_INFORMATION_DATA_HOB                  *SgInfoDataHob
  );

EFI_STATUS
BaseOnSkuIdPowerEnableGpuThroughEc (
  IN H2O_SWITCHABLE_GRAPHICS_PPI              *SgPpi,
  IN SG_INFORMATION_DATA_HOB                  *SgInfoDataHob
  );

EFI_STATUS
ModifiedGpioPowerEnableSequence (
  IN H2O_SWITCHABLE_GRAPHICS_PPI              *SgPpi,
  IN SG_INFORMATION_DATA_HOB                  *SgInfoDataHob
  );
#endif

/**
  This function offers an interface for OEM code to change the MXM GPU power enable sequence
  and modify the Switchable Graphics Information data HOB.

  @param[in]         None

  @retval            EFI_UNSUPPORTED     Returns unsupported by default.
  @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
  @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                         The caller will skip the specified behavior and assuming
                                         that it has been handled completely by this function.
**/
EFI_STATUS
OemSvcMxmDgpuPowerSequence (
  VOID
  )
{
#if 0
  EFI_STATUS                                  Status;
  H2O_SWITCHABLE_GRAPHICS_PPI                 *SgPpi;
  SG_INFORMATION_DATA_HOB                     *SgInfoDataHob;
  SWITCHABLE_GRAPHICS_CUSTOMIZATION           CustomizationFunction;

  Status = GetSgInfoHobAndLocateSgPpi (&SgPpi, &SgInfoDataHob);
  if (EFI_ERROR (Status)) {
   	return EFI_UNSUPPORTED;
  }

  //
  // There are 4 samples below, OEM can base different case to change customization function.
  //
  CustomizationFunction = ModifiedGpioPowerEnableSequence;
  return (CustomizationFunction) (SgPpi, SgInfoDataHob);
#else
  return EFI_UNSUPPORTED;
#endif
}

#if 0
/**

  Prepare SG infromation data HOB and SG PPI services this module needed.

  @param[in, out] SgInfoDataHob   A double pointer of SG information data HOB,
                                  for OEM runtime modify SG related data.
  @param[in, out] SgPpi           A double pointer of PEI Switchable Graphics PPI Function
                                  for SG GPIO read/write and stall functions.

  @retval EFI_SUCCESS    Get SG information data HOB and PEI SG PPI successfully.
  @retval !EFI_SUCCESS   Get SG information data HOB or PPI service failed.

**/
STATIC
EFI_STATUS
GetSgInfoHobAndLocateSgPpi (
  IN OUT H2O_SWITCHABLE_GRAPHICS_PPI          **SgPpi,
  IN OUT SG_INFORMATION_DATA_HOB              **SgInfoDataHob
  )
{
  EFI_PEI_HOB_POINTERS                        Hob;
  EFI_STATUS                                  Status;

  (*SgInfoDataHob) = NULL;

  //
  // Locate PEI Switchable Graphics PPI
  //
  Status = PeiServicesLocatePpi (
             &gH2OSwitchableGraphicsPpiGuid,
             0,
             NULL,
             SgPpi
             );
  if (EFI_ERROR (Status)) {
   	return Status;
  }
  //
  // Get Switchable Graphics Information Data HOB
  //
  Status = PeiServicesGetHobList (&Hob.Raw);
  while (!END_OF_HOB_LIST (Hob)) {
    if (Hob.Header->HobType == EFI_HOB_TYPE_GUID_EXTENSION && CompareGuid (&Hob.Guid->Name, &gH2OSgInformationDataHobGuid)) {
      (*SgInfoDataHob) = (SG_INFORMATION_DATA_HOB *) (Hob.Header);
      break;
    }
    Hob.Raw = GET_NEXT_HOB (Hob);
  }
  if ((*SgInfoDataHob) == NULL) {
   	return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

/**

  Sample 1:
    If OEM need base on different SKU ID to change the SG mode,
    but still need SG PEIM power enable sequence, please refer this function.

  @param[in] SgInfoDataHob   A pointer of SG information data HOB,
                             for OEM runtime modify SG related data.
  @param[in] SgPpi           A pointer of PEI Switchable Graphics PPI Function
                             for SG GPIO read/write and stall functions.

  @retval EFI_MEDIA_CHANGED   It means the OEM service change the SG information data HOB,
                              but still need MXM GPU power enable sequence in SG PEI module.

**/
EFI_STATUS
BaseOnSkuIdChangeSgMode (
  IN H2O_SWITCHABLE_GRAPHICS_PPI              *SgPpi,
  IN SG_INFORMATION_DATA_HOB                  *SgInfoDataHob
  )
{
/*
  UINT8                                       SkuId;

  SkuId = GetSkuId ();

  if (SkuId == InternalOnlySkuId) {
    SgInfoDataHob->SgMode = SgModeDisabled;
  } else if (SkuId == DiscreteOnlySkuId) {
    SgInfoDataHob->SgMode = SgModeDgpu;
  } else if (SkuId == DualGraphicsSkuId) {
    SgInfoDataHob->SgMode = SgModeMuxless;
  } else {
    SgInfoDataHob->SgMode = SgModeDisabled;
  }
*/

  return EFI_MEDIA_CHANGED;
}

/**

  Sample 2:
    If OEM need base on SG mode to do different power enable sequence through EC, please refer this function.

  @param[in] SgInfoDataHob   A pointer of SG information data HOB,
                             for OEM runtime modify SG related data.
  @param[in] SgPpi           A pointer of PEI Switchable Graphics PPI Function
                             for SG GPIO read/write and stall functions.

  @retval EFI_SUCCESS         It means MXM GPU power enable sequence already done and success,
                              doesn't need do anythings else in SG PEI module.

**/
EFI_STATUS
BaseOnSgModePowerEnableGpuThroughEc (
  IN H2O_SWITCHABLE_GRAPHICS_PPI              *SgPpi,
  IN SG_INFORMATION_DATA_HOB                  *SgInfoDataHob
  )
{
/*
  if (SgInfoDataHob->SgMode == SgModeDisabled) {
    //
    // Internal only sku need power down the discrete MXM GPU.
    //
    ECRamWrite (EcOffsetPowerControl, EcOffsetDgpuPowerDisable);
  } else {
    //
    // Discrete only sku or SG sku need power enable the discrete MXM GPU.
    //
    ECRamWrite (EcOffsetPowerControl, EcOffsetDgpuPowerEnable);
  }
*/

  return EFI_SUCCESS;
}

/**

  Sample 3:
    If OEM need base on different SKU ID to change the SG mode,
    then base on SG mode to do different power enable sequence through EC, please refer this function.

  @param[in] SgInfoDataHob   A pointer of SG information data HOB,
                             for OEM runtime modify SG related data.
  @param[in] SgPpi           A pointer of PEI Switchable Graphics PPI Function
                             for SG GPIO read/write and stall functions.

  @retval EFI_SUCCESS         It means MXM GPU power enable sequence already done and success,
                              doesn't need do anythings else in SG PEI module.

**/
EFI_STATUS
BaseOnSkuIdPowerEnableGpuThroughEc (
  IN H2O_SWITCHABLE_GRAPHICS_PPI              *SgPpi,
  IN SG_INFORMATION_DATA_HOB                  *SgInfoDataHob
  )
{
/*
  UINT8                                       SkuId;

  SkuId = GetSkuId ();

  if (SkuId == InternalOnlySkuId) {
    SgInfoDataHob->SgMode = SgModeDisabled;
  } else if (SkuId == DiscreteOnlySkuId) {
    SgInfoDataHob->SgMode = SgModeDgpu;
  } else if (SkuId == DualGraphicsSkuId) {
    SgInfoDataHob->SgMode = SgModeMuxless;
  } else {
    SgInfoDataHob->SgMode = SgModeDisabled;
  }

  if (SgInfoDataHob->SgMode == SgModeDisabled) {
    //
    // Internal only sku need power down the discrete MXM GPU.
    //
    ECRamWrite (EcOffsetPowerControl, EcOffsetDgpuPowerDisable);
  } else {
    //
    // Discrete only sku or SG sku need power enable the discrete MXM GPU.
    //
    ECRamWrite (EcOffsetPowerControl, EcOffsetDgpuPowerEnable);
  }
*/

  return EFI_SUCCESS;
}

/**

  Sample 4:
    If OEM need modify the power enable sequence through GPIO pins, please refer this function.
    This sampel doesn't have dGPU_PRSNT, EDID_SELECT, dGPU_SELECT and dGPU_PWM_SELECT pins.

  @param[in] SgInfoDataHob   A pointer of SG information data HOB,
                             for OEM runtime modify SG related data.
  @param[in] SgPpi           A pointer of PEI Switchable Graphics PPI Function
                             for SG GPIO read/write and stall functions.

  @retval EFI_SUCCESS         It means MXM GPU power enable sequence already done and success,
                              doesn't need do anythings else in SG PEI module.

**/
EFI_STATUS
ModifiedGpioPowerEnableSequence (
  IN H2O_SWITCHABLE_GRAPHICS_PPI              *SgPpi,
  IN SG_INFORMATION_DATA_HOB                  *SgInfoDataHob
  )
{
  BOOLEAN                                     IsUlt;

  IsUlt = SgInfoDataHob->IsUltBoard;
  if (SgInfoDataHob->SgMode == SgModeDisabled) {
    //
    // Internal only sku need power down the discrete MXM GPU.
    //
    if (SgPpi->GpioRead (SgInfoDataHob->SgDgpuPwrEnable, IsUlt) == Active) {
      SgPpi->GpioWrite (SgInfoDataHob->SgDgpuHoldRst, Active, IsUlt);
      SgPpi->GpioWrite (SgInfoDataHob->SgDgpuPwrEnable, Inactive, IsUlt);
    } else {
      SgPpi->GpioWrite (SgInfoDataHob->SgDgpuHoldRst, Inactive, IsUlt);
    }
  } else {
    // Power enable the discrete GPU when dual VGA mode (SG) or discrete GPU only mode.
    // Power enable sequence: (1) Active DgpuHoldRst => Delay 100ms
    //                        (2) Active DgpuPwrEnable => Delay 300ms
    //                        (3) Active DgpuVron => Delay 50ms (Different Sequence Here)
    //                        (4) Inactive DgpuHoldRst => Delay 100ms
    // If DgpuVron is GPIO 46 and high active, please refer this sample.
    //
    SgPpi->GpioWrite (SgInfoDataHob->SgDgpuHoldRst, Active, IsUlt);
    SgPpi->Stall (SgInfoDataHob->ActiveDgpuHoldRstDelay * 1000);
    SgPpi->GpioWrite (SgInfoDataHob->SgDgpuPwrEnable, Active, IsUlt);
    SgPpi->Stall (SgInfoDataHob->ActiveDgpuPwrEnableDelay * 1000);
    SgPpi->GpioWrite ((46 | (1 << 7)), Active, IsUlt); // GPIO46 high active dGPU_VRON pin active
    SgPpi->Stall (50 * 1000); // delay 50ms
    SgPpi->GpioWrite (SgInfoDataHob->SgDgpuHoldRst, Inactive, IsUlt);
    SgPpi->Stall (SgInfoDataHob->InactiveDgpuHoldRstDelay * 1000);
  }

  return EFI_SUCCESS;
}
#endif
